#pragma once

#include <vector>

#include <leveldb/db.h>
#include <boost/scoped_ptr.hpp>

#include <options/options.hpp>

#include "ipage_storage.hpp"

class CLevelDbPageStorage : public IPageStorage {
public:
    CLevelDbPageStorage(const CLevelDbPageStorageOptions& options);

    bool StoreFile( const std::string& server
                  , const std::string& uri
                  , const std::string& ext
                  , const std::string& filename );

private:
    boost::scoped_ptr<leveldb::DB>      Db;
    const CLevelDbPageStorageOptions&   Options;
    std::vector<char>                   FileBuffer;
};

