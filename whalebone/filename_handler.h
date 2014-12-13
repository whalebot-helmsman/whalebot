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
    bool createPath( std::string server
                   , std::string uri
                   , std::string const &ext
                   , std::string &filename );
private:
    std::string m_baseDir;

};



#endif	/* _FILENAMEHANDLER_H */

