#pragma once


#include <options/options.hpp>

#include "ipage_storage.hpp"

class CLevelDbPageStorage {
public:
    CLevelDbPageStorage(const CLevelDbPageStorageOptions& options);

    bool StoreFile( const std::string& server
                  , const std::string& uri
                  , const std::string& ext
                  , const std::string& filename );

    ~CLevelDbPageStorage();
private:
};

