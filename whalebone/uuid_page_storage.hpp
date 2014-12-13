#pragma once

#include <fstream>

#include <boost/uuid/random_generator.hpp>

#include <options/options.hpp>

#include "ipage_storage.hpp"

class CUuidPageStorage : public IPageStorage {
public:
    CUuidPageStorage(const CUuidPageStorageOptions& options);

    bool StoreFile( const std::string& server
                  , const std::string& uri
                  , const std::string& ext
                  , const std::string& filename );

private:
    bool createPath( const std::string& server
                   , const std::string& uri
                   , const std::string& ext
                   , std::string &filename );

    std::ofstream                   m_linkDb;
    boost::uuids::random_generator  m_generator;
    CUuidPageStorageOptions         Options;
};
