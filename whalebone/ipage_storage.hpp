#pragma once
#include <string>

class IPageStorage {
public:
    virtual bool createPath( const std::string& server
                           , const std::string& uri
                           , const std::string& ext
                           , std::string &filename ) = 0;
    virtual ~IPageStorage(){};
};
