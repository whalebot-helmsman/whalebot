#pragma once
#include <string>
#include <vector>
#include <limits>
#include <boost/property_tree/ptree.hpp>

struct CUuidPageStorageOptions {
public:
    unsigned int    HierarchicalLevel;
    unsigned int    LevelLength;
    std::string     BaseDirectory;

    void Load( const boost::property_tree::ptree& properties
             , const std::string&                 basePath );
};

struct CPlainPageStorageOptions {
public:
    std::string     BaseDirectory;

    void Load( const boost::property_tree::ptree& properties
             , const std::string&                 basePath );
};

struct CLevelDbPageStorageOptions {
public:
    std::string DatabaseFile;
    bool        IsCompress;
    bool        IsSynchronyze;

    void Load( const boost::property_tree::ptree& properties
             , const std::string&                 basePath );
};

struct CPageStorageOptions {
public:

    enum EType {
        ETypePlain = 0,
        ETypeUuid,
        ETypeLevelDb,
        ETypeUnknown
    };

    EType                       Type;
    CUuidPageStorageOptions     Uuid;
    CPlainPageStorageOptions    Plain;
    CLevelDbPageStorageOptions  LevelDb;

    void Load( const boost::property_tree::ptree& properties
             , const std::string&                 basePath );

    static const char* StorageTypeToString(EType type);
    static EType StorageTypeFromString(const char* type);
};

struct CStorageOptions {
public:
    std::string BaseDirectory;

    std::string FutureLinksPath;
    std::string VisitedLinksPath;
    bool        IsSaveFutureAndUsedLinks;

    bool                IsSavePages;
    CPageStorageOptions PageStorage;

    std::string ExtractedUrlsPath;

    std::string TmpFilePath;

    void Load( const boost::property_tree::ptree& properties
             , const std::string&                 defaultBasePath );
};

struct CRuntimeOptions {
public:
    enum EWorkingMode {
        EDebugWorkingMode = 0,
        EInteractiveWorkingMode,
        EDaemonWorkingMode,
        EUnknownWorkingMode
    };

    static const char* WorkingModeToString(EWorkingMode mode);
    static EWorkingMode WorkingModeFromString(const char* mode);
    EWorkingMode    Mode;
    std::string     LogPath;
    unsigned int    StateSavingPeriod;

    static const unsigned int   kDoNotSaveStateDuringCrawl  =   0;

    void Load( const boost::property_tree::ptree& properties
             , const std::string&                 basePath );
};

struct CFetchOptions {
public:
    unsigned int    WaitAfterFetchInMicroseconds;
    bool            IsUseReferer;
    void Load(const boost::property_tree::ptree& properties);
};

struct CLinkFilterOptions {
public:
    bool            IsAllLinksFromOneServer;

    static const unsigned int kDoNotRestrictLinkLevel;
    unsigned int    MaxLinkLevel;

    typedef std::vector<std::string>    CWordList;

    CWordList       RequiredWordsInLink;

    void Load(const boost::property_tree::ptree& properties);
};

struct CTagExtractorOptions {
public:
    std::string Tag;
    std::string TargetAttribute;
    std::string ControlAttribute;
    std::string ControlAttributeValue;
};

struct CHtmlExtractorOptions {
public:
    bool    IsUseDefaultExtractor;
    bool    IsExtractImages;
    bool    IsExtractCss;
    bool    IsExtractFavicons;
    bool    IsExtractStructure;

    typedef std::vector<CTagExtractorOptions>   CTagExtractors;
    CTagExtractors  CustomTagExtractors;

    void Load(const boost::property_tree::ptree& properties);
};

struct CSpiderOptions {
public:
    CStorageOptions         Storage;
    CRuntimeOptions         Runtime;
    CFetchOptions           Fetch;
    CLinkFilterOptions      LinkFilter;
    CHtmlExtractorOptions   HtmlExtractor;

    void Load(const std::string& configPath);
};
