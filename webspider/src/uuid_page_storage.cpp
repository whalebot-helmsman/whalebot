#include <uuid_page_storage.hpp>

#include <boost/filesystem/convenience.hpp>
#include <boost/uuid/uuid_io.hpp>

CUuidPageStorage::CUuidPageStorage(const std::string &base_dir)
: m_baseDir(base_dir)
{
    if(m_baseDir[m_baseDir.size() - 1] != '/') {
        m_baseDir  +=  "/";
    }

    boost::filesystem::create_directories(m_baseDir);
    std::string linkDbPath  =   m_baseDir + "db";
    m_linkDb.open(linkDbPath.c_str(), std::ios::app|std::ios::out);
}

bool CUuidPageStorage::createPath(std::string server, std::string uri, std::string const &ext, std::string &filename)
{
    std::string uuid    =   to_string(m_generator());
    filename            =   m_baseDir   +   uuid;
    m_linkDb    <<  server << '\t' << uri << '\t' << uuid << std::endl;
    return true;
}
