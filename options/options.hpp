#pragma once
#include <string>
#include <vector>
#include <limits>
#include <boost/property_tree/ptree.hpp>

struct CStorageOptions {
public:
    std::string BaseDirectory;

    std::string FutureLinksLoadPath;
    std::string VisitedLinksLoadPath;
    bool        IsSaveFutureAndUsedLinks;

    std::string PageStorageDirectory;
    enum EPageStorageType {
        EPageStorageTypePlain = 0,
        EPageStorageTypeUuid
    };
    EPageStorageType    PageStorageType;
    bool                IsSavePages;

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

    void Load(const boost::property_tree::ptree& properties);
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


struct CSpiderOptions {
public:
    CStorageOptions     Storage;
    CRuntimeOptions     Runtime;
    CFetchOptions       Fetch;
    CLinkFilterOptions  LinkFilter;

    void Load(const std::string& configPath);
};
