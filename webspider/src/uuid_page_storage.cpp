#include <uuid_page_storage.hpp>

#include <boost/filesystem/convenience.hpp>
#include <boost/uuid/uuid_io.hpp>

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

bool CUuidPageStorage::createPath(std::string server, std::string uri, std::string const &ext, std::string &filename)
{
    static const unsigned int   kCharsPerLevel  =   2;

    std::string     uuid        =   to_string(m_generator());
    std::string     path        =   m_baseDir;
    unsigned int    maxLevel    =   uuid.size() / kCharsPerLevel - 1;
    unsigned int    curLevel    =   std::min(maxLevel, m_hierarchicalLevel);

    for (unsigned int level = 0; level != curLevel; ++level) {
        path.append("/");
        path.append(uuid, level * kCharsPerLevel, kCharsPerLevel);
    }

    bool    ret =   boost::filesystem::exists(path) | boost::filesystem::create_directories(path);

    if (false == ret) {
        return false;
    }

    filename    =   path + "/" + uuid;
    m_linkDb    <<  server << '\t' << uri << '\t' << uuid << std::endl;

    return true;
}
