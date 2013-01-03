/* 
 * File:   webspider_options.h
 * Author: hotdox
 *
 * Created on May 1; 2010; 4:05 PM
 */

#ifndef _WEBSPIDER_OPTIONS_H
#define	_WEBSPIDER_OPTIONS_H

#include <string>


class CWebSpiderOptions {

public:

    //fields
    std::string     m_sSite;
    std::string     m_sLinkFilterFile;
    std::string     m_sErrorLogPath;
    std::string     m_sOutput;
    std::string     m_sTmpFilePath;
    std::string     m_sUsedLinksPath;
    std::string     m_sFutureLinksPath;

    bool            m_bSavePages;
    bool            m_bSaveHistory;
    bool            m_bCollectLinks;
    bool            m_bOneServer;
    
    bool            m_bAskAfterFetch;

    size_t          m_iLevel;
    unsigned int    m_iConnectionTimeoutInSeconds;
    unsigned int    m_iReadTimeoutInSeconds;
    unsigned int    m_iMaxConnections;

    std::string     m_sOptionsFile;
    
    
    CWebSpiderOptions();
    bool    readFromCmdLine(int argc, char* argv[]);
    bool    readFromFile(const std::string& path);


};



#endif	/* _WEBSPIDER_OPTIONS_H */

