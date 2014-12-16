#include "leveldb_storage.hpp"

CLevelDbPageStorage::CLevelDbPageStorage(const CLevelDbPageStorageOptions& options)
{}

bool CLevelDbPageStorage::StoreFile( const std::string& server
                                   , const std::string& uri
                                   , const std::string& /*ext*/
                                   , const std::string& filename )
{
    return true;
}

CLevelDbPageStorage::~CLevelDbPageStorage()
{}
