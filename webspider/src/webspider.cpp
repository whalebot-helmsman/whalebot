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
#include <whalebone/storage_factory.hpp>
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

void saveState(const CSpiderOptions& options, const CLinkBuffer& work_front, std::ofstream& errorLogFile)
{
    if (true == options.Storage.IsSaveFutureAndUsedLinks) {

        std::ofstream   futurelinks_endfile(options.Storage.FutureLinksPath.c_str());
        errorLogFile << "saving future links to " << options.Storage.FutureLinksPath << std::endl;
        work_front.writeFutureLinks(futurelinks_endfile);
        futurelinks_endfile.close();


        std::ofstream   usedlinks_endfile(options.Storage.VisitedLinksPath.c_str());
        errorLogFile << "saving used links to " << options.Storage.VisitedLinksPath << std::endl;
        work_front.writeUsedLinks(usedlinks_endfile);
        usedlinks_endfile.close();

    }
}

hubbub_error parseHtml( const std::string&           filepath
                      , const CHtmlExtractorOptions& options
                      , CUrlNormalizer&              normolizer)
{

        std::ifstream file(filepath.c_str());

        if (false == file.is_open()) {
            return HUBBUB_FILENOTFOUND;
        }

        CLinkExtractor  extractor(normolizer, options);
        hubbub_error    parserStatus    =   extractor.init();

        if (HUBBUB_OK != parserStatus) {
            return parserStatus;
        }

        parserStatus    =   extractor.extract(file);
        return parserStatus;
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

    IPageStorage*                   storage =   CreateStorage(options.Storage.PageStorage);
    if (NULL == storage) {
        errorLogFile << "Couldn't open storage" << std::endl;
        return EXIT_FAILURE;
    }

    boost::scoped_ptr<IPageStorage> storageGuard(storage);

    COneFetcher                     fetcher(service, options.Fetch);

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

    if (false == seedUri.empty()) {
        normolizer.pushLink(seedUri);
    }

    CLink   next;
    bool    connected(false);
    int     link_counter(0),
            http_errors(0);


    if (CRuntimeOptions::EInteractiveWorkingMode == options.Runtime.Mode) {
        boost::thread   t(boost::bind(async_read, boost::ref(isTimeToStop)));
        std::cout<<"*Start working press [ENTER] to stop"<<std::endl;
    }

    boost::posix_time::ptime    start           =   boost::posix_time::microsec_clock::local_time();
    boost::posix_time::ptime    lastStateSaving =   start;
    double                      sleepTime       =   0.0;
    double                      bytesFetched    =   0.0;

    while ((work_front.pop(next))&&(!isTimeToStop)) {

        usleep(options.Fetch.WaitAfterFetchInMicroseconds);
        sleepTime   +=  options.Fetch.WaitAfterFetchInMicroseconds;

        if(!next.isValid())
            continue;

        ++link_counter;

        boost::posix_time::ptime   now   =   boost::posix_time::microsec_clock::local_time();

        if (  (CRuntimeOptions::kDoNotSaveStateDuringCrawl != options.Runtime.StateSavingPeriod)
           && (static_cast<unsigned int>(boost::posix_time::time_period(lastStateSaving, now).length().total_seconds()) > options.Runtime.StateSavingPeriod) ) {
            saveState(options, work_front, errorLogFile);
            lastStateSaving =   now;
        }

        errorLogFile << now
                     << " we have " << work_front.size() + 1
                     << " links, looks at " << link_counter - 1
                     << " links"<< std::endl;

        double  timeConsumed    =   boost::posix_time::time_period(start, now).length().total_microseconds();
        double  timeToWork      =   timeConsumed - sleepTime;
        double  mbFetched       =   bytesFetched / (1024 * 1024);

        double  consumedPageSpeed   =   (link_counter - 1) / (timeConsumed / 1000000);
        double  workPageSpeed       =   (link_counter - 1) / (timeToWork / 1000000);
        double  consumedMbSpeed     =   mbFetched / (timeConsumed / 1000000);
        double  workMbSpeed         =   mbFetched / (timeToWork / 1000000);

        errorLogFile << "speed             "
                     << std::fixed << std::setw(6) << std::setprecision(3) << consumedPageSpeed
                     << " links/sec, "
                     << std::fixed << std::setw(8) << std::setprecision(4) << consumedMbSpeed
                     << " Mb/sec" << std::endl;

        if (0 != options.Fetch.WaitAfterFetchInMicroseconds) {
            errorLogFile << "speed w/o waiting "
                         << std::fixed << std::setw(6) << std::setprecision(3) << workPageSpeed
                         << " links/sec, "
                         << std::fixed << std::setw(8) << std::setprecision(4) << workMbSpeed
                         << " Mb/sec" << std::endl;
        }

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

        bytesFetched    +=  tmp.tellp();

        if (!res) {
            errorLogFile << "\t\tfailed" << std::endl;
            continue;
        }
        tmp.close();


        if (true == CLinkExtractor::isParse(ext)) {
            errorLogFile << "\t*Parse html" << std::endl;
            hubbub_error    parserStatus    =   parseHtml( options.Storage.TmpFilePath
                                                         , options.HtmlExtractor
                                                         , normolizer );
            if (HUBBUB_OK != parserStatus) {
                errorLogFile << "Error initializing HTML parser \""
                             << hubbub_error_to_string(parserStatus) << "\""
                             << std::endl;
            }
        }

        if (options.Storage.IsSavePages) {
            errorLogFile << "\t\twrite page" << std::endl;
            bool    isStored    =   storage->StoreFile( next.getServer()
                                                      , next.getUri()
                                                      , ext
                                                      , options.Storage.TmpFilePath );

            if (false == isStored) {
                errorLogFile << "\t\t\tcoudnt store \"" << options.Storage.TmpFilePath << "\"" << std::endl;
            }
        }
    }

    saveState(options, work_front,  errorLogFile);

    errorLogFile << "*Stop working release resourses" << std::endl;
    if (boost::filesystem::exists(options.Storage.TmpFilePath)) {
        boost::filesystem::remove(options.Storage.TmpFilePath);
    }

    return EXIT_SUCCESS;
}
