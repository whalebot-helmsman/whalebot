#include <iostream>
#include <fstream>
#include <algorithm>


#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <options.h>

#include <webspider_options.h>
#include <version.h>
#include <config_const.h>

struct TWorkingModeToStringMapperCell {
public:
    CWebSpiderOptions::EWorkingMode Value;
    const char*                     Repr;
};


static const TWorkingModeToStringMapperCell     kWorkingModeMapper[]    =   { {CWebSpiderOptions::EDebugWorkingMode,       "debug"}
                                                                            , {CWebSpiderOptions::EInteractiveWorkingMode, "interactive"}
                                                                            , {CWebSpiderOptions::EDaemonWorkingMode,       "daemon"} };
static const unsigned int                       kWorkingModeMapperSize  =   sizeof(kWorkingModeMapper) / sizeof(kWorkingModeMapper[0]);
static const TWorkingModeToStringMapperCell*    kWorkingModeMapperBegin =   kWorkingModeMapper;
static const TWorkingModeToStringMapperCell*    kWorkingModeMapperEnd   =   kWorkingModeMapper + kWorkingModeMapperSize;

struct TByModeFinder {
public:
    TByModeFinder(CWebSpiderOptions::EWorkingMode mode)
    : Mode(mode)
    {}

    bool operator () (TWorkingModeToStringMapperCell cell)
    {
        return cell.Value == Mode;
    }

private:
    CWebSpiderOptions::EWorkingMode Mode;
};

const char* CWebSpiderOptions::WorkingModeToString(CWebSpiderOptions::EWorkingMode mode)
{

    const TWorkingModeToStringMapperCell* pos =   std::find_if( kWorkingModeMapperBegin
                                                              , kWorkingModeMapperEnd
                                                              , TByModeFinder(mode) );

    if (kWorkingModeMapperEnd == pos) {
        return "unknown";
    }

    return pos->Repr;
}

struct TByReprFinder {
public:
    TByReprFinder(const char* mode)
    : Mode(mode)
    {}

    bool operator () (TWorkingModeToStringMapperCell cell)
    {
        return 0 == strcmp(cell.Repr, Mode);
    }

private:
    const char* Mode;
};

CWebSpiderOptions::EWorkingMode CWebSpiderOptions::WorkingModeFromString(const char* mode)
{

    const TWorkingModeToStringMapperCell* pos =   std::find_if( kWorkingModeMapperBegin
                                                              , kWorkingModeMapperEnd
                                                              , TByReprFinder(mode) );

    if (kWorkingModeMapperEnd == pos) {
        return EUnknownWorkingMode;
    }

    return pos->Value;
}


CWebSpiderOptions::CWebSpiderOptions()
: m_sSite("")
, m_sLinkFilterFile("")
, m_sErrorLogPath("")
, m_sOutput("")
, m_sTmpFilePath("")
, m_sUsedLinksPath("")
, m_sFutureLinksPath("")

, m_bSavePages(true)
, m_bSaveHistory(true)
, m_bCollectLinks(false)
, m_bOneServer(false)
, m_bIsUseUuidStorage(false)
, m_eWorkingMode(EInteractiveWorkingMode)

, m_sOptionsFile("")

, m_iWaitAfterFetchInMicroseconds(0)
{}
void validate(boost::any& v, const std::vector<std::string>& values, CWebSpiderOptions::EWorkingMode*, int)
{
    CWebSpiderOptions::EWorkingMode mode;

    if (1 != values.size()) {
        throw boost::program_options::validation_error(boost::program_options::validation_error::multiple_values_not_allowed);
    }

    mode    =   CWebSpiderOptions::WorkingModeFromString(values[0].c_str());

    if (CWebSpiderOptions::EUnknownWorkingMode == mode) {
        throw boost::program_options::validation_error(boost::program_options::validation_error::invalid_option_value);
    }

    v   =   mode;
}

bool CWebSpiderOptions::readFromCmdLine(int argc, char* argv[])
{

    boost::program_options::options_description desc("Webspider options");
    desc.add_options()
            ("help,h", "show this message")
            ("version,v", "show version info")

            ("site,s", boost::program_options::value<std::string> (&m_sSite)->default_value(""), "seed uri")
            (kCollectLinksAttrCmd.c_str(), "collect extracted links in raw form")
            (kStorageAttrCmd.c_str(), "is use uuid-based page storage engine with db file")
            (kSavePagesAttrCmd.c_str(), "dont save fetched pages")
            (kOneServerAttrCmd.c_str(), "fetch only from one server")
            (kLinkFilterFileAttrCmd.c_str(), boost::program_options::value<std::string> (&m_sLinkFilterFile)->default_value(""), "file with words to link filter")
            (kLevelAttrCmd.c_str(), boost::program_options::value<size_t> (&m_iLevel)->default_value(0), "max level of link in one site")

            (kOutputAttrCmd.c_str(), boost::program_options::value<std::string> (&m_sOutput)->default_value(boost::filesystem::initial_path().string()), "output dir")
            (kErrorLogPathAttrCmd.c_str(), boost::program_options::value<std::string> (&m_sErrorLogPath)->default_value(""), "path to log file")
            (kTmpFilePathAttrCmd.c_str(), boost::program_options::value<std::string> (&m_sTmpFilePath)->default_value("tmp"), "tmp filename")

            (kUsedLinksPathAttrCmd.c_str(), boost::program_options::value<std::string> (&m_sUsedLinksPath)->default_value("usedlinks.txt"), "path to used links file")
            (kFutureLinksPathAttrCmd.c_str(), boost::program_options::value<std::string> (&m_sFutureLinksPath)->default_value("futurelinks.txt"), "path future links file")

            (kWaitAfterAttrCmd.c_str(), boost::program_options::value<unsigned int> (&m_iWaitAfterFetchInMicroseconds)->default_value(0), "wait after fetched page in microseconds")

            (kWorkingModeAttrCmd.c_str(), boost::program_options::value<EWorkingMode> (&m_eWorkingMode)->default_value(EInteractiveWorkingMode), "working modes - debug, interactive or daemon")
            (kSaveHistoryAttrCmd.c_str(),  "do not save links after stop")

            (kOptionsFileAttrCmd.c_str(), boost::program_options::value<std::string>(&m_sOptionsFile)->default_value(""), "file with configurations")
            ;

    boost::program_options::positional_options_description p;
    p.add("site", -1);

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    boost::program_options::notify(vm);

    if (vm.count("version")) {
        std::cout   << "WebSpider - tool for statsem package v " << kVersion << " - Beta" << std::endl
                    << "Author Vostretsov Nikita started 15.03.2009" << std::endl;
        return false;
    }

    if (vm.count("help")) {
        std::cout << "Usage: options_description [options]\n";
        std::cout << desc;
        return false;
    }

    if (vm.count(kSavePagesAttr)) {
        m_bSavePages  =   false;
    }

    if (vm.count(kStorageAttr)) {
        m_bIsUseUuidStorage  =   true;
    }

    if (vm.count(kOneServerAttr)) {
        m_bOneServer  =   true;
    }

    if(vm.count(kSaveHistoryAttr)){
        m_bSaveHistory    =   false;
    }

    if(vm.count(kCollectLinksAttr)){
        m_bCollectLinks   =   true;
    }

    if(not m_sOptionsFile.empty()) {
        readFromFile(m_sOptionsFile);
    }

    return true;
}


bool CWebSpiderOptions::readFromFile(const std::string& path)
{
    std::ifstream   file(path.c_str());

    if (not file.is_open()) {
        return false;
    }

    COptions    options;
    options.read(file);


    options.get(kCollectLinksAttr, m_bCollectLinks);
    options.get(kErrorLogPathAttr, m_sErrorLogPath);
    options.get(kFutureLinksPathAttr, m_sFutureLinksPath);
    options.get(kLevelAttr, m_iLevel);
    options.get(kLinkFilterFileAttr, m_sLinkFilterFile);
    options.get(kOneServerAttr, m_bOneServer);
    options.get(kOutputAttr, m_sOutput);
    options.get(kSaveHistoryAttr, m_bSaveHistory);
    options.get(kSavePagesAttr, m_bSavePages);
    options.get(kTmpFilePathAttr, m_sTmpFilePath);
    options.get(kUsedLinksPathAttr, m_sUsedLinksPath);

    return true;
}
