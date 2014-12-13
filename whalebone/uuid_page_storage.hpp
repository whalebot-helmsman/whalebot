#pragma once

#include "ipage_storage.hpp"

#include <boost/uuid/random_generator.hpp>

#include <fstream>

class CUuidPageStorage : public IPageStorage {
public:
    CUuidPageStorage(const std::string &base_dir, unsigned int hierarchicalLevel);
    bool createPath( std::string server
                   , std::string uri
                   , std::string const &ext
                   , std::string &filename );
private:
    std::string                     m_baseDir;
    std::ofstream                   m_linkDb;
    boost::uuids::random_generator  m_generator;
    unsigned int                    m_hierarchicalLevel;
};
