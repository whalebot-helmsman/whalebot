#include <signal.h>

#include <fstream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/date_time/time_facet.hpp>
#include <boost/scoped_ptr.hpp>

#include <version.h>
#include <one_fetcher.h>
#include <link/link_buffer.h>
#include <link/link_extractor.h>
#include <header_parser.h>
#include <filename_handler.h>
#include <uuid_page_storage.hpp>
#include <link/filters.h>
#include <link/url_normalizer.hpp>

#include <options/options.hpp>

void async_read(bool &stop){
    getchar();
    stop   =   true;
}

std::ostream*               gLogger         =   NULL;
bool*                       gStopCondition  =   NULL;
boost::asio::io_service*    gIoService      =   NULL;

void signal_catcher(int sgnum)
{
    *gLogger << "caugth signal " << sgnum << std::endl;

    if (NULL != gStopCondition) {
        *gStopCondition =   true;
    }

    if (NULL != gIoService) {
        gIoService->stop();
    }
}



int main(int argc, char* argv[])
{
    std::string seedUri;
    std::string configPath;

    boost::program_options::options_description desc("Webspider options");
    desc.add_options()
            ("help,h",    "show this message")
            ("version,v", "show version info")
            ("site,s",    boost::program_options::value<std::string>(&seedUri)->default_value(""),             "seed uri")
            ("config,c",  boost::program_options::value<std::string>(&configPath)->default_value(".whalebot"), "path to config file");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if (vm.count("version")) {
        std::cout   << "Whalebot - common web crawler, version  " << kVersion << " - Beta" << std::endl;
        return EXIT_SUCCESS;
    }

    if (vm.count("help")) {
        std::cout << "Usage: options_description [options]\n";
        std::cout << desc;
        return EXIT_SUCCESS;
    }

    CSpiderOptions  options;
    options.Load(configPath);

    boost::asio::io_service             service;
    bool                                isTimeToStop(false);

    gStopCondition  =   &isTimeToStop;
    gIoService      =   &service;

    std::ofstream   errorLogFile(options.Runtime.LogPath.c_str(), std::ios::out|std::ios::app);
    if (false == errorLogFile.is_open()) {
        std::cout << "Could not open \"" << options.Runtime.LogPath
                  << "\" for log file" << std::endl;
        return EXIT_FAILURE;
    }

    gLogger =   &errorLogFile;

    static const int            kHandledSignals[]   =   { SIGINT
                                                        , SIGQUIT
                                                        , SIGTERM };

    static const unsigned int   kHandledSignalsSize =   sizeof(kHandledSignals) / sizeof(kHandledSignals[0]);

    unsigned int    sigCount    =   0;
    bool            isOk        =   true;

    while ((true == isOk) && (sigCount != kHandledSignalsSize)) {
        int             sigNum  =   kHandledSignals[sigCount];

        struct sigaction    newAction;
        memset(&newAction, 0, sizeof(newAction));
        newAction.sa_handler    =   signal_catcher;

        struct sigaction*   oldOne      =   NULL;
        int                 sigResult   =   sigaction(sigNum, &newAction, oldOne);


        if (0 != sigResult) {
            isOk    =   false;
            errorLogFile << "cannot set signal handler for " << sigNum << std::endl;
            continue;
        }

        //does not brake signal handling of parent process
        if ((NULL != oldOne) && (SIG_IGN == oldOne->sa_handler)) {
            signal(sigNum, SIG_IGN);
        }
        ++sigCount;
    }

    if (false == isOk) {
        return EXIT_FAILURE;
    }


    errorLogFile.imbue(std::locale( errorLogFile.getloc()
                                  , new boost::posix_time::time_facet("%T") ));

    IPageStorage*                   storage =   NULL;
    if (CStorageOptions::EPageStorageTypeUuid == options.Storage.PageStorageType) {
        storage =   new CUuidPageStorage(options.Storage.PageStorageDirectory);
    }
    else {
        storage =   new CFilenameHandler(options.Storage.PageStorageDirectory);
    }
    boost::scoped_ptr<IPageStorage> storageGuard(storage);
    COneFetcher                     fetcher(service);

    CUrlNormalizer  normolizer(options.Storage.ExtractedUrlsPath);

    CFilterList     filters;
    CLinkBuffer     work_front;
    normolizer.setAcceptor(filters);
    filters.setAcceptor(work_front);

    if (true == options.LinkFilter.IsAllLinksFromOneServer) {
        filters.addFilter(new COneServerFilter());
    }

    if(false == options.LinkFilter.RequiredWordsInLink.empty()){
        filters.addFilter(new CIncludeFilter(options.LinkFilter.RequiredWordsInLink));
    }

    if (CLinkFilterOptions::kDoNotRestrictLinkLevel != options.LinkFilter.MaxLinkLevel) {
        filters.addFilter(new CLevelFilter(options.LinkFilter.MaxLinkLevel));
    }


    std::ifstream   usedlinks_file(options.Storage.VisitedLinksPath.c_str()),
                    futurelinks_file(options.Storage.FutureLinksPath.c_str());

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

    normolizer.pushLink(seedUri);

    CLink   next;
    bool    connected(false);
    int     link_counter(0),
            http_errors(0);


    if (CRuntimeOptions::EInteractiveWorkingMode == options.Runtime.Mode) {
        boost::thread   t(boost::bind(async_read, boost::ref(isTimeToStop)));
        std::cout<<"*Start working press [ENTER] to stop"<<std::endl;
    }

    boost::posix_time::ptime    start   =   boost::posix_time::microsec_clock::local_time();

    while ((work_front.pop(next))&&(!isTimeToStop)) {

        usleep(options.Fetch.WaitAfterFetchInMicroseconds);

        if(!next.isValid())
            continue;

        ++link_counter;

        boost::posix_time::ptime   now   =   boost::posix_time::microsec_clock::local_time();


        errorLogFile << now
                     << " we have " << work_front.size() + 1
                     << " links, looks at " << link_counter - 1
                     << " links"<< std::endl;




        double  time_consumption(boost::posix_time::time_period(start, now).length().total_microseconds());
        time_consumption    /=  1000000;
        errorLogFile << "speed " <<(link_counter - 1)/time_consumption<<" links/sec"<<std::endl;
        if (0 != http_errors) {
            errorLogFile << "we have " << http_errors << " errors" << std::endl;
        }

        if (CRuntimeOptions::EDebugWorkingMode == options.Runtime.Mode) {
            char    c;
            std::cout<<"continue?(y/n)"<<std::endl;
            std::cin>>c;
            if(c == 'n'){
                isTimeToStop    =   true;
                continue;
            }
        }




        errorLogFile << "*Connect to " << next.getServer() << std::endl;
        connected = fetcher.connect(next);

        if (!connected) {
            errorLogFile << "\tfailed connect to " << next.getServer() << std::endl;
            http_errors +=  1;
            continue;
        }

        normolizer.setFrom(next);

        errorLogFile << "\t*Request " << next.getUri() << std::endl;
        if (!fetcher.request(next)) {
            errorLogFile << "\t\tfailed" << std::endl;
            continue;
        }

        std::ofstream tmp(options.Storage.TmpFilePath.c_str());

        CHeaderParser header;
        errorLogFile << "\t\tget header " << std::endl;
        unsigned int status(fetcher.getHeader(header, tmp));
        if ((status != 200) && (status != 301) && (status != 302) && (status != 303)) {
            errorLogFile << "\t\t\tfailed error = " << status << std::endl;
            errorLogFile << "\t\t\tfrom server " << next.getServer() << std::endl;
            errorLogFile << "\t\t\t**************************************" << std::endl;
            for (header_map::const_iterator i = header.begin(); i != header.end(); ++i) {
                errorLogFile <<"\t\t\t"<< i->first << " = " << i->second << std::endl;
            }
            errorLogFile << "\t\t\t**************************************" << std::endl;
            ++http_errors;
            continue;
        }
        if ((status == 301) || (status == 302) || (status == 303)) {
            std::string loc;
            if (header.getField("Location", loc)) {
                errorLogFile << "\t\t\tredirected to " << loc << std::endl;
               normolizer.pushLink(loc);
            }
            continue;
        }



        std::string cookies(""),
                    ext("html");

        header.getCookies(cookies);
        header.getExtension(ext);
        next.setCookie(cookies);

        normolizer.setFrom(next);

        if((!CLinkExtractor::isParse(ext))&&(!options.Storage.IsSavePages))
            continue;

        errorLogFile << "\t*Get response" << std::endl;

        bool res(fetcher.getResponse(tmp));

        if (!res) {
            errorLogFile << "\t\tfailed" << std::endl;
            continue;
        }
        tmp.close();


        std::string filepath("");


        if(options.Storage.IsSavePages){
            if (!storage->createPath(next.getServer(), next.getUri(), ext, filepath)) {
                errorLogFile << "\t\tcouldnt create dir for " << filepath << std::endl;
                continue;
            }

            errorLogFile << "\t\twrite to " << filepath << std::endl;
            try {

                if (boost::filesystem::exists(filepath)) {
                    boost::filesystem::remove(filepath);
                }

                boost::filesystem::copy_file(options.Storage.TmpFilePath, filepath);
            } catch (...) {
                errorLogFile << "\t\t\tcoudnt copy " << options.Storage.TmpFilePath << " to " << filepath << std::endl;
                continue;
            }
        }else{
            filepath    =   options.Storage.TmpFilePath;
        }

        if (!CLinkExtractor::isParse(ext)) {
            continue;
        }

        errorLogFile << "\t*Parse " << filepath << std::endl;
        std::ifstream f(filepath.c_str());

        CLinkExtractor  extractor(normolizer);
        hubbub_error    parserStatus    =   extractor.init();

        if (HUBBUB_OK != parserStatus) {
            errorLogFile << "Error initializing HTML parser \""
                         << hubbub_error_to_string(parserStatus) << "\""
                         << std::endl;
            continue;
        }

        parserStatus    =   extractor.extract(f);
        if (HUBBUB_OK != parserStatus) {
            errorLogFile << "Error parsing HTML \""
                         << hubbub_error_to_string(parserStatus) << "\""
                         << std::endl;
        }
    }

    if (true == options.Storage.IsSaveFutureAndUsedLinks) {

        if (not work_front.IsFutureEmpty()) {
            std::ofstream   futurelinks_endfile(options.Storage.FutureLinksPath.c_str());
            errorLogFile << "saving future links to " << options.Storage.FutureLinksPath << std::endl;
            work_front.writeFutureLinks(futurelinks_endfile);
            futurelinks_endfile.close();
        }


        if (not work_front.IsUsedEmpty()) {
            std::ofstream   usedlinks_endfile(options.Storage.VisitedLinksPath.c_str());
            errorLogFile << "saving used links to " << options.Storage.VisitedLinksPath << std::endl;
            work_front.writeUsedLinks(usedlinks_endfile);
            usedlinks_endfile.close();
        }

    }

    errorLogFile << "*Stop working release resourses" << std::endl;
    if (boost::filesystem::exists(options.Storage.TmpFilePath)) {
        boost::filesystem::remove(options.Storage.TmpFilePath);
    }

    return EXIT_SUCCESS;
}
