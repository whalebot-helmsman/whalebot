#include "options.hpp"
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>

const unsigned int CLinkFilterOptions::kDoNotRestrictLinkLevel    =   std::numeric_limits<unsigned int>::max();

struct TWorkingModeToStringMapperCell {
public:
    CRuntimeOptions::EWorkingMode Value;
    const char*                   Repr;
};


static const TWorkingModeToStringMapperCell     kWorkingModeMapper[]    =   { {CRuntimeOptions::EDebugWorkingMode,       "debug"}
                                                                            , {CRuntimeOptions::EInteractiveWorkingMode, "interactive"}
                                                                            , {CRuntimeOptions::EDaemonWorkingMode,      "daemon"} };
static const unsigned int                       kWorkingModeMapperSize  =   sizeof(kWorkingModeMapper) / sizeof(kWorkingModeMapper[0]);
static const TWorkingModeToStringMapperCell*    kWorkingModeMapperBegin =   kWorkingModeMapper;
static const TWorkingModeToStringMapperCell*    kWorkingModeMapperEnd   =   kWorkingModeMapper + kWorkingModeMapperSize;

struct TByModeFinder {
public:
    TByModeFinder(CRuntimeOptions::EWorkingMode mode)
    : Mode(mode)
    {}

    bool operator () (TWorkingModeToStringMapperCell cell)
    {
        return cell.Value == Mode;
    }

private:
    CRuntimeOptions::EWorkingMode Mode;
};

const char* CRuntimeOptions::WorkingModeToString(CRuntimeOptions::EWorkingMode mode)
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

CRuntimeOptions::EWorkingMode CRuntimeOptions::WorkingModeFromString(const char* mode)
{

    const TWorkingModeToStringMapperCell* pos =   std::find_if( kWorkingModeMapperBegin
                                                              , kWorkingModeMapperEnd
                                                              , TByReprFinder(mode) );

    if (kWorkingModeMapperEnd == pos) {
        return EUnknownWorkingMode;
    }

    return pos->Value;
}

void rebasePath(const std::string& basePath, std::string& partial)
{
    boost::filesystem::path rebaseOn(basePath);
    boost::filesystem::path candidate(partial);

    if (true == candidate.is_absolute()) {
        return;
    }

    rebaseOn    /=  candidate;
    partial     =   rebaseOn.native();
}

static const boost::property_tree::ptree    kEmptyTree;

struct TStorageTypeToStringMapperCell {
public:
    CPageStorageOptions::EType   Value;
    const char*                             Repr;
};


static const TStorageTypeToStringMapperCell     kStorageTypeMapper[]    =   { {CPageStorageOptions::ETypePlain,   "plain"   }
                                                                            , {CPageStorageOptions::ETypeUuid,    "uuid"    }
                                                                            , {CPageStorageOptions::ETypeLevelDb, "leveldb" } };

void CUuidPageStorageOptions::Load( const boost::property_tree::ptree& properties
                                  , const std::string&                 basePath )
{
    HierarchicalLevel   =   properties.get<unsigned int>("hierarchical_level", 0);
    LevelLength         =   properties.get<unsigned int>("level_length",       2);
    BaseDirectory       =   properties.get("base", "pages");

    rebasePath(basePath, BaseDirectory);
}

void CLevelDbPageStorageOptions::Load( const boost::property_tree::ptree& properties
                                     , const std::string&                 basePath )
{
    DatabaseFile        =   properties.get("database", "pages");
    rebasePath(basePath, DatabaseFile);

    IsCompress          =   properties.get("is_compress",   true);
    IsSynchronyze       =   properties.get("is_sync",       false);
    MaxFileSize         =   properties.get("max_file_size", uintmax_t(1024 * 1024));//1Mb
}

void CPlainPageStorageOptions::Load( const boost::property_tree::ptree& properties
                                   , const std::string&                 basePath )
{
    BaseDirectory   =   properties.get("base", "pages");
    rebasePath(basePath, BaseDirectory);
}

void CPageStorageOptions::Load( const boost::property_tree::ptree& properties
                              , const std::string&                 basePath )
{
    Plain.Load(properties.get_child("plain",     kEmptyTree), basePath);
    Uuid.Load(properties.get_child("uuid",       kEmptyTree), basePath);
    LevelDb.Load(properties.get_child("leveldb", kEmptyTree), basePath);

    std::string type    =   properties.get<std::string>("type", kStorageTypeMapper[0].Repr);
    Type    =   StorageTypeFromString(type.c_str());
}

void CStorageOptions::Load( const boost::property_tree::ptree& properties
                          , const std::string&                 defaultBasePath )
{
    BaseDirectory   =   properties.get("base_dir", defaultBasePath);

    FutureLinksPath         =   properties.get("future_links_file",    "future");
    VisitedLinksPath        =   properties.get("visited_links_file",   "visited");
    TmpFilePath             =   properties.get("tmp_file",             "tmp");
    ExtractedUrlsPath       =   properties.get("extracted_links_file", "/dev/null");

    rebasePath(BaseDirectory, FutureLinksPath);
    rebasePath(BaseDirectory, VisitedLinksPath);
    rebasePath(BaseDirectory, TmpFilePath);
    rebasePath(BaseDirectory, ExtractedUrlsPath);

    IsSaveFutureAndUsedLinks    =   properties.get("is_save_links", true);
    IsSavePages                 =   properties.get("is_save_pages", true);

    PageStorage.Load(properties.get_child("pages", kEmptyTree), BaseDirectory);
}

void CLinkFilterOptions::Load(const boost::property_tree::ptree& properties)
{
    IsAllLinksFromOneServer =   properties.get("one_server", false);
    MaxLinkLevel            =   properties.get("max_level", kDoNotRestrictLinkLevel);

    BOOST_FOREACH(const boost::property_tree::ptree::value_type& word, properties.get_child("required", kEmptyTree)) {
        RequiredWordsInLink.push_back(word.second.get_value<std::string>());
    }
}

void CFetchOptions::Load(const boost::property_tree::ptree& properties)
{
    WaitAfterFetchInMicroseconds    =   properties.get("wait", 0);
    IsUseReferer                    =   properties.get("is_use_referer", false);
}

void CRuntimeOptions::Load( const boost::property_tree::ptree& properties
                          , const std::string&                 basePath )
{
    std::string mode    =   properties.get<std::string>("mode", kWorkingModeMapper[1].Repr);
    Mode                =   WorkingModeFromString(mode.c_str());
    LogPath             =   properties.get("log_file", "/dev/stdout");

    unsigned int    defaultPeriod   =   kDoNotSaveStateDuringCrawl;
    StateSavingPeriod   =   properties.get("save_period", defaultPeriod);

    rebasePath(basePath, LogPath);
}

void CHtmlExtractorOptions::Load(const boost::property_tree::ptree& properties)
{
    IsUseDefaultExtractor   =   properties.get("is_use_default_extractor", true);
    IsExtractImages         =   properties.get("is_extract_images",        false);
    IsExtractCss            =   properties.get("is_extract_css",           false);
    IsExtractFavicons       =   properties.get("is_extract_favicons",      false);
    IsExtractStructure      =   properties.get("is_extract_structure",     false);

    BOOST_FOREACH(const boost::property_tree::ptree::value_type& extractor, properties.get_child("extractors", kEmptyTree)) {
        CTagExtractorOptions    tagExtractor;
        tagExtractor.Tag                    =   extractor.second.get("tag",               "");
        tagExtractor.TargetAttribute        =   extractor.second.get("target_attribute",  "");
        tagExtractor.ControlAttribute       =   extractor.second.get("control_attribute", "");
        tagExtractor.ControlAttributeValue  =   extractor.second.get("control_value",     "");

        CustomTagExtractors.push_back(tagExtractor);
    }
}

void CSpiderOptions::Load(const std::string& configPath)
{
    boost::property_tree::ptree properties;
    read_json(configPath, properties);

    Storage.Load(properties.get_child("storage", kEmptyTree), boost::filesystem::path(configPath).parent_path().native());
    Runtime.Load(properties.get_child("runtime", kEmptyTree), Storage.BaseDirectory);
    Fetch.Load(properties.get_child("fetch", kEmptyTree));
    HtmlExtractor.Load(properties.get_child("html_extractor", kEmptyTree));
    LinkFilter.Load(properties.get_child("link_filters", kEmptyTree));
}

static const unsigned int                       kStorageTypeMapperSize  =   sizeof(kStorageTypeMapper) / sizeof(kStorageTypeMapper[0]);
static const TStorageTypeToStringMapperCell*    kStorageTypeMapperBegin =   kStorageTypeMapper;
static const TStorageTypeToStringMapperCell*    kStorageTypeMapperEnd   =   kStorageTypeMapper + kStorageTypeMapperSize;

struct TByTypeFinder {
public:
    TByTypeFinder(CPageStorageOptions::EType type)
    : Type(type)
    {}

    bool operator () (TStorageTypeToStringMapperCell cell)
    {
        return cell.Value == Type;
    }

private:
    CPageStorageOptions::EType   Type;
};

const char* CPageStorageOptions::StorageTypeToString(EType type)
{

    const TStorageTypeToStringMapperCell* pos =   std::find_if( kStorageTypeMapperBegin
                                                              , kStorageTypeMapperEnd
                                                              , TByTypeFinder(type) );

    if (kStorageTypeMapperEnd == pos) {
        return "unknown";
    }

    return pos->Repr;
}

struct TByTypeReprFinder {
public:
    TByTypeReprFinder(const char* type)
    : Type(type)
    {}

    bool operator () (TStorageTypeToStringMapperCell cell)
    {
        return 0 == strcmp(cell.Repr, Type);
    }

private:
    const char* Type;
};

CPageStorageOptions::EType CPageStorageOptions::StorageTypeFromString(const char* type)
{

    const TStorageTypeToStringMapperCell* pos =   std::find_if( kStorageTypeMapperBegin
                                                              , kStorageTypeMapperEnd
                                                              , TByTypeReprFinder(type) );

    if (kStorageTypeMapperEnd == pos) {
        return ETypeUnknown;
    }

    return pos->Value;
}
