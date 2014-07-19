#pragma once
#include <string>
#include <vector>
#include <limits>
#include <boost/property_tree/ptree.hpp>

struct CStorageOptions {
public:
    std::string BaseDirectory;

    std::string FutureLinksPath;
    std::string VisitedLinksPath;
    bool        IsSaveFutureAndUsedLinks;

    std::string PageStorageDirectory;
    enum EPageStorageType {
        EPageStorageTypePlain = 0,
        EPageStorageTypeUuid,
        EPageStorageTypeUnknown
    };
    EPageStorageType    PageStorageType;
    bool                IsSavePages;

    std::string ExtractedUrlsPath;

    std::string TmpFilePath;

    void Load( const boost::property_tree::ptree& properties
             , const std::string&                 defaultBasePath );

    static const char* StorageTypeToString(EPageStorageType type);
    static EPageStorageType StorageTypeFromString(const char* type);
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

    void Load( const boost::property_tree::ptree& properties
             , const std::string&                 basePath );
};

struct CFetchOptions {
public:
    unsigned int    WaitAfterFetchInMicroseconds;
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
