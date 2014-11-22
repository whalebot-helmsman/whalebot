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

    std::string fullPath    =   m_baseDir + "/" +path;
    bool        ret         =   boost::filesystem::exists(fullPath) | boost::filesystem::create_directories(fullPath);

    if (false == ret) {
        return false;
    }

    filename    =   fullPath + "/" + uuid;
    m_linkDb    <<  server << '\t' << uri << '\t' << path + "/" + uuid << std::endl;

    return true;
}
