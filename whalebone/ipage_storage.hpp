#pragma once
#include <string>

//TODO: add init method which one can return error
class IPageStorage {
public:
    virtual bool StoreFile( const std::string& server
                          , const std::string& uri
                          , const std::string& ext
                          , const std::string& filename ) = 0;

    virtual ~IPageStorage(){};
};
