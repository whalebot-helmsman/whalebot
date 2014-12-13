/*
 * File:   FilenameHandler.h
 * Author: hotdox
 *
 * Created on February 23, 2009, 6:05 PM
 */

#ifndef _FILENAMEHANDLER_H
#define	_FILENAMEHANDLER_H

#include "ipage_storage.hpp"

class CFilenameHandler : public IPageStorage {
public:
    CFilenameHandler(const std::string &base_dir);
    bool createPath( const std::string& server
                   , const std::string& uri
                   , const std::string& ext
                   , std::string &filename );
private:
    std::string m_baseDir;

};



#endif	/* _FILENAMEHANDLER_H */

