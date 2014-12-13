#pragma once
#include <string>

class IPageStorage {
public:
    virtual bool StoreFile( const std::string& server
                          , const std::string& uri
                          , const std::string& ext
                          , const std::string& filename ) = 0;

    virtual ~IPageStorage(){};
};
