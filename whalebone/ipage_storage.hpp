#pragma once
#include <string>

class IPageStorage {
public:
    virtual bool createPath( std::string server
                           , std::string uri
                           , std::string const &ext
                           , std::string &filename ) = 0;
    virtual ~IPageStorage(){};
};
