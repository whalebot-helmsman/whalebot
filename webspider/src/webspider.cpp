#include <signal.h>

#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/date_time/time_facet.hpp>


//#include <neon/ne_session.h>

#include <one_fetcher.h>
#include <link_buffer.h>
#include <link_extractor.h>
#include <header_parser.h>
#include <filename_handler.h>
#include <filters.h>
#include <link.h>
#include <ilink_factory.h>
#include <link_factory.h>
#include <collector_link.h>
#include <webspider_options.h>

void async_read(bool &stop){
    getchar();
    stop   =   true;
}

std::ostream*   gLogger         =   NULL;
bool*           gStopCondition  =   NULL;

void signal_catcher(int sgnum)
{
    *gLogger << "caugth signal " << sgnum << std::endl;
    *gStopCondition =   true;
}



int main(int argc, char* argv[]) {

 //   ne_sock_init();

    boost::posix_time::time_facet facet("%T");


    CWebSpiderOptions   options;

    if (not options.readFromCmdLine(argc, argv)) {
        return 0;
    }

    bool                isTimeToStop(false);
    gStopCondition  =   &isTimeToStop;


    std::ostream    *errorlog(&std::cout);
    bool            need_delete(false);
    if (!options.m_sErrorLogPath.empty()) {
        std::ofstream *file = new std::ofstream(options.m_sErrorLogPath.c_str());
        if (file->is_open()){
            errorlog    =   file;
            need_delete =   true;
        }
    }

    gLogger =   errorlog;

    static const int            kHandledSignals[]   =   { SIGINT
                                                        , SIGQUIT
                                                        , SIGTERM };

    static const unsigned int   kHandledSignalsSize =   sizeof(kHandledSignals) / sizeof(kHandledSignals[0]);

    unsigned int    sigCount    =   0;
    bool            isOk        =   true;

    while ((true == isOk) && (sigCount != kHandledSignalsSize)) {
        int             sigNum  =   kHandledSignals[sigCount];
        sighandler_t    oldOne  =   signal(sigNum, signal_catcher);

        if (SIG_ERR == oldOne) {
            isOk    =   false;
            (*errorlog) << "cannot set signal handler for " << sigNum << std::endl;
            continue;
        }

        //does not brake signal handling of parent process
        if (SIG_IGN == oldOne) {
            signal(sigNum, SIG_IGN);
        }
        ++sigCount;
    }

    if (false == isOk) {
        return EXIT_FAILURE;
    }


    errorlog->imbue(std::locale(std::cout.getloc(), &facet));

    CFilenameHandler    files(options.m_sOutput);
    COneFetcher         fetcher;

    ILinkFactory*       factory(0);
    if (options.m_bCollectLinks) {
        factory =   new CCollectorLinkFactory();
    }
    else {
        factory =   new CLinkFactory();
    }

    CFilterList     filters;
    CLinkBuffer     work_front;
    factory->setAcceptor(filters);
    filters.setAcceptor(work_front);

    if(options.m_bOneServer){
        filters.addFilter(new COneServerFilter());
    }

    if(!options.m_sLinkFilterFile.empty()){
        std::ifstream   link_file(options.m_sLinkFilterFile.c_str());
        filters.addFilter(new CIncludeFilter(link_file));
    }

    if(options.m_iLevel > 0){
        filters.addFilter(new CLevelFilter(options.m_iLevel));
    }


    std::ifstream   usedlinks_file(options.m_sUsedLinksPath.c_str()),
                    futurelinks_file(options.m_sFutureLinksPath.c_str());

    if(futurelinks_file.is_open()){
        work_front.readFutureLinks(futurelinks_file);
        futurelinks_file.close();
        futurelinks_file.clear();
    }

    if(usedlinks_file.is_open()){
        work_front.readUsedLinks(usedlinks_file);
        usedlinks_file.close();
        usedlinks_file.clear();
    }

    factory->pushLink(options.m_sSite);

    CLink   next;
    bool    connected(false);
    int     link_counter(0),
            http_errors(0);


    if(CWebSpiderOptions::EInteractiveWorkingMode == options.m_eWorkingMode){
        boost::thread   t(boost::bind(async_read, boost::ref(isTimeToStop)));
        std::cout<<"*Start working press [ENTER] to stop"<<std::endl;
    }

    boost::posix_time::ptime    start   =   boost::posix_time::microsec_clock::local_time();

    while ((work_front.pop(next))&&(!isTimeToStop)) {


        if(!next.isValid())
            continue;

        ++link_counter;

        boost::posix_time::ptime   now   =   boost::posix_time::microsec_clock::local_time();


        (*errorlog) << now
                    << " we have " << work_front.size() + 1
                    << " links, looks at " << link_counter - 1
                    << " links, found "<< std::endl;




        double  time_consumption(boost::posix_time::time_period(start, now).length().total_microseconds());
        time_consumption    /=  1000000;
        (*errorlog) << "speed " <<(link_counter - 1)/time_consumption<<" links/sec"<<std::endl;
        (*errorlog) << "we have "<<http_errors<<" errors" <<std::endl;

        if(CWebSpiderOptions::EDebugWorkingMode == options.m_eWorkingMode){
            char    c;
            std::cout<<"continue?(y/n)"<<std::endl;
            std::cin>>c;
            if(c == 'n'){
                isTimeToStop    =   true;
                continue;
            }
        }




        (*errorlog) << "*Connect to " << next.getServer() << std::endl;
        connected = fetcher.connect(next);

        if (!connected) {
            (*errorlog) << "\tfailed connect to " << next.getServer() << std::endl;
            continue;
        }

        factory->setFrom(next);

        (*errorlog) << "\t*Request " << next.getUri() << std::endl;
        if (!fetcher.request(next)) {
            (*errorlog) << "\t\tfailed" << std::endl;
            continue;
        }

        std::ofstream tmp(options.m_sTmpFilePath.c_str());

        CHeaderParser header;
        (*errorlog) << "\t\tget header " << std::endl;
        unsigned int status(fetcher.getHeader(header, tmp));
        if ((status != 200) && (status != 301) && (status != 302) && (status != 303)) {
            (*errorlog) << "\t\t\tfailed error = " << status << std::endl;
            (*errorlog) << "\t\t\tfrom server " << next.getServer() << std::endl;
            (*errorlog) << "\t\t\t**************************************" << std::endl;
            for (header_map::const_iterator i = header.begin(); i != header.end(); ++i) {
                (*errorlog) <<"\t\t\t"<< i->first << " = " << i->second << std::endl;
            }
            (*errorlog) << "\t\t\t**************************************" << std::endl;
            ++http_errors;
            continue;
        }
        if ((status == 301) || (status == 302) || (status == 303)) {
            std::string loc;
            if (header.getField("Location", loc)) {
                (*errorlog) << "\t\t\tredirected to " << loc << std::endl;
               factory->pushLink(loc);
            }
            continue;
        }



        std::string cookies(""),
                    ext("html");

        header.getCookies(cookies);
        header.getExtension(ext);
        next.setCookie(cookies);

        factory->setFrom(next);

        if((!CLinkExtractor<int>::isParse(ext))&&(!options.m_bSavePages))
            continue;

        (*errorlog) << "\t*Get response" << std::endl;

        bool res(fetcher.getResponse(tmp));

        if (!res) {
            (*errorlog) << "\t\tfailed" << std::endl;
            continue;
        }
        tmp.close();


        std::string filepath("");


        if(options.m_bSavePages){
            if (!files.createPath(next.getServer(), next.getUri(), ext, filepath)) {
                (*errorlog) << "\t\tcouldnt create dir for " << filepath << std::endl;
                continue;
            }

            (*errorlog) << "\t\twrite to " << filepath << std::endl;
            try {

                if (boost::filesystem::exists(filepath)) {
                    boost::filesystem::remove(filepath);
                }

                boost::filesystem::copy_file(options.m_sTmpFilePath, filepath);
            } catch (...) {
                (*errorlog) << "\t\t\tcoudnt copy " << options.m_sTmpFilePath << " to " << filepath << std::endl;
                continue;
            }
        }else{
            filepath    =   options.m_sTmpFilePath;
        }

        if (!CLinkExtractor<int>::isParse(ext)) {
            continue;
        }

        (*errorlog) << "\t*Parse " << filepath << std::endl;
        std::ifstream f(filepath.c_str());

        CLinkExtractor<ILinkFactory> extractor(*factory);
        extractor.extract(f);
    }

    if(options.m_bSaveHistory){

        if (not work_front.IsFutureEmpty()) {
            std::ofstream   futurelinks_endfile(options.m_sFutureLinksPath.c_str());
            (*errorlog) << "saving future links to " << options.m_sFutureLinksPath << std::endl;
            work_front.writeFutureLinks(futurelinks_endfile);
            futurelinks_endfile.close();
        }


        if (not work_front.IsUsedEmpty()) {
            std::ofstream   usedlinks_endfile(options.m_sUsedLinksPath.c_str());
            (*errorlog) << "saving used links to " << options.m_sUsedLinksPath << std::endl;
            work_front.writeUsedLinks(usedlinks_endfile);
            usedlinks_endfile.close();
        }

    }

    (*errorlog) << "*Stop working release resourses" << std::endl;
    if (boost::filesystem::exists(options.m_sTmpFilePath)) {
        boost::filesystem::remove(options.m_sTmpFilePath);
    }

    if(need_delete) {
        delete errorlog;
    }

    delete factory;

//    ne_sock_exit();

    return 0;
}
