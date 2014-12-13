#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "uuid_page_storage.hpp"

CUuidPageStorage::CUuidPageStorage(const CUuidPageStorageOptions& options)
: Options(options)
{
    if(Options.BaseDirectory[Options.BaseDirectory.size() - 1] == '/') {
        Options.BaseDirectory.resize(Options.BaseDirectory.size() - 1);
    }

    boost::filesystem::create_directories(Options.BaseDirectory);
    std::string linkDbPath  =   Options.BaseDirectory + "/" + "_db";
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
    std::string     uuid        =   to_string(m_generator());
    std::string     path        =   "";
    unsigned int    maxLevel    =   uuid.size() / Options.LevelLength - 1;
    unsigned int    curLevel    =   std::min(maxLevel, Options.HierarchicalLevel);

    for (unsigned int level = 0; level != curLevel; ++level) {
        path    +=  "/";
        path.append(uuid, level * Options.LevelLength, Options.LevelLength);
    }

    std::string                 fullPath    =   Options.BaseDirectory + "/" +path;
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
