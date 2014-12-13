#pragma once

#include "ipage_storage.hpp"

#include <boost/uuid/random_generator.hpp>

#include <fstream>

class CUuidPageStorage : public IPageStorage {
public:
    CUuidPageStorage(const std::string &base_dir, unsigned int hierarchicalLevel);

    bool StoreFile( const std::string& server
                  , const std::string& uri
                  , const std::string& ext
                  , const std::string& filename );

private:
    bool createPath( const std::string& server
                   , const std::string& uri
                   , const std::string& ext
                   , std::string &filename );

    std::string                     m_baseDir;
    std::ofstream                   m_linkDb;
    boost::uuids::random_generator  m_generator;
    unsigned int                    m_hierarchicalLevel;
};
