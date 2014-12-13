#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "uuid_page_storage.hpp"

CUuidPageStorage::CUuidPageStorage( const std::string& base_dir
                                  , unsigned int       hierarchicalLevel )
: m_baseDir(base_dir)
, m_hierarchicalLevel(hierarchicalLevel)
{
    if(m_baseDir[m_baseDir.size() - 1] == '/') {
        m_baseDir.resize(m_baseDir.size() - 1);
    }

    boost::filesystem::create_directories(m_baseDir);
    std::string linkDbPath  =   m_baseDir + "/" + "db";
    m_linkDb.open(linkDbPath.c_str(), std::ios::app|std::ios::out);
}

bool CUuidPageStorage::StoreFile( const std::string& server
                                , const std::string& uri
                                , const std::string& ext
                                , const std::string& filename )
{
    std::string resultPath      =   "";
    bool        isPathCreated   =   createPath(server, uri, ext, resultPath);
    if (false == isPathCreated) {
        return false;
    }
    boost::system::error_code   error;
    boost::filesystem::rename(filename, resultPath, error);
    return error.value() == 0;
}

bool CUuidPageStorage::createPath( const std::string& server
                                 , const std::string& uri
                                 , const std::string& ext
                                 , std::string &filename )
{
    static const unsigned int   kCharsPerLevel  =   2;

    std::string     uuid        =   to_string(m_generator());
    std::string     path        =   "";
    unsigned int    maxLevel    =   uuid.size() / kCharsPerLevel - 1;
    unsigned int    curLevel    =   std::min(maxLevel, m_hierarchicalLevel);

    for (unsigned int level = 0; level != curLevel; ++level) {
        path.append("/");
        if (0 == level) {
            path.append("_");
        }
        path.append(uuid, level * kCharsPerLevel, kCharsPerLevel);
    }

    std::string                 fullPath    =   m_baseDir + "/" +path;
    boost::system::error_code   error;
    bool                        ret         =   (  (true == boost::filesystem::exists(fullPath))
                                                || (  (true == boost::filesystem::create_directories(fullPath, error))
                                                   && (0 == error.value()) ) );

    if (false == ret) {
        return false;
    }

    filename    =   fullPath + "/" + uuid;
    m_linkDb    <<  server << '\t' << uri << '\t' << path + "/" + uuid << std::endl;

    return true;
}
