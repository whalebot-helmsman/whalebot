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

    rebaseOn    +=  candidate;
    partial     =   rebaseOn.native();
}

static const boost::property_tree::ptree    kEmptyTree;

struct TStorageTypeToStringMapperCell {
public:
    CStorageOptions::EPageStorageType   Value;
    const char*                         Repr;
};


static const TStorageTypeToStringMapperCell     kStorageTypeMapper[]    =   { {CStorageOptions::EPageStorageTypePlain, "plain"}
                                                                            , {CStorageOptions::EPageStorageTypeUuid,  "uuid" } };

void CStorageOptions::Load( const boost::property_tree::ptree& properties
                          , const std::string&                 defaultBasePath )
{
    BaseDirectory   =   properties.get("base_dir", defaultBasePath);

    FutureLinksLoadPath     =   properties.get("future_links_file",    "future");
    VisitedLinksLoadPath    =   properties.get("visited_links_file",   "visited");
    TmpFilePath             =   properties.get("tmp_file",             "tmp");
    PageStorageDirectory    =   properties.get("pages_dir",            "pages");
    ExtractedUrlsPath       =   properties.get("extracted_links_file", "/dev/null");

    rebasePath(BaseDirectory, FutureLinksLoadPath);
    rebasePath(BaseDirectory, VisitedLinksLoadPath);
    rebasePath(BaseDirectory, TmpFilePath);
    rebasePath(BaseDirectory, PageStorageDirectory);
    rebasePath(BaseDirectory, ExtractedUrlsPath);

    IsSaveFutureAndUsedLinks    =   properties.get("is_save_links", true);
    IsSavePages                 =   properties.get("is_save_pages", true);

    std::string type    =   properties.get<std::string>("page_storage_type", kStorageTypeMapper[0].Repr);
    PageStorageType     =   StorageTypeFromString(type.c_str());
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
}

void CRuntimeOptions::Load(const boost::property_tree::ptree& properties)
{
    std::string mode    =   properties.get<std::string>("mode", kWorkingModeMapper[1].Repr);
    Mode    =   WorkingModeFromString(mode.c_str());
}

void CSpiderOptions::Load(const std::string& configPath)
{
    boost::property_tree::ptree properties;
    read_json(configPath, properties);

    Storage.Load(properties.get_child("storage", kEmptyTree), boost::filesystem::path(configPath).parent_path().native());
    Runtime.Load(properties.get_child("runtime", kEmptyTree));
    Fetch.Load(properties.get_child("fetch", kEmptyTree));
    LinkFilter.Load(properties.get_child("link_filters", kEmptyTree));
}

static const unsigned int                       kStorageTypeMapperSize  =   sizeof(kStorageTypeMapper) / sizeof(kStorageTypeMapper[0]);
static const TStorageTypeToStringMapperCell*    kStorageTypeMapperBegin =   kStorageTypeMapper;
static const TStorageTypeToStringMapperCell*    kStorageTypeMapperEnd   =   kStorageTypeMapper + kStorageTypeMapperSize;

struct TByTypeFinder {
public:
    TByTypeFinder(CStorageOptions::EPageStorageType type)
    : Type(type)
    {}

    bool operator () (TStorageTypeToStringMapperCell cell)
    {
        return cell.Value == Type;
    }

private:
    CStorageOptions::EPageStorageType   Type;
};

const char* CStorageOptions::StorageTypeToString(EPageStorageType type)
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

CStorageOptions::EPageStorageType CStorageOptions::StorageTypeFromString(const char* type)
{

    const TStorageTypeToStringMapperCell* pos =   std::find_if( kStorageTypeMapperBegin
                                                              , kStorageTypeMapperEnd
                                                              , TByTypeReprFinder(type) );

    if (kStorageTypeMapperEnd == pos) {
        return EPageStorageTypeUnknown;
    }

    return pos->Value;
}
