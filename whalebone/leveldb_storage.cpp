#include <fstream>

#include <boost/filesystem.hpp>

#include "leveldb_storage.hpp"

CLevelDbPageStorage::CLevelDbPageStorage(const CLevelDbPageStorageOptions& options)
: Db(NULL)
, Options(options)
{
    leveldb::Options    openOptions;

    //TODO: create directory for db file
    //TODO: handle logger openOptions.info_log
    if (true == Options.IsCompress) {
        openOptions.compression =   leveldb::kSnappyCompression;
    }
    openOptions.create_if_missing   =   true;

    leveldb::DB*    db      =   NULL;
    //TODO: report error to top level
    leveldb::Status isOpen  =   leveldb::DB::Open( openOptions
                                                 , options.DatabaseFile
                                                 , &db );
    Db.reset(db);
    FileBuffer.resize(Options.MaxFileSize);
}

bool CLevelDbPageStorage::StoreFile( const std::string& server
                                   , const std::string& uri
                                   , const std::string& /*ext*/
                                   , const std::string& filename )
{
    if (NULL == Db.get()) {
        return false;
    }

    boost::filesystem::path file(filename);

    if (  (false == boost::filesystem::exists(file))
       || (false == boost::filesystem::is_regular_file(file)) ) {
        return false;
    }

    uintmax_t   fileSize    =   boost::filesystem::file_size(file);

    if (fileSize > FileBuffer.size()) {
        //TODO: pass big files to uuid storage
        return false;
    }

    std::ifstream   reader(filename.c_str());
    if (false == reader.is_open()) {
        return false;
    }

    reader.read(&*FileBuffer.begin(), fileSize);

    std::string             key         =   server + uri;
    leveldb::Slice          keySlice(key);
    leveldb::Slice          valueSlice(&*FileBuffer.begin(), fileSize);
    leveldb::WriteOptions   writeOptions;

    writeOptions.sync   =   Options.IsSynchronyze;

    leveldb::Status isWritten   =   Db->Put(writeOptions, keySlice, valueSlice);

    return isWritten.ok();
}

