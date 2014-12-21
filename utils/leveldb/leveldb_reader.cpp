#include <link/whale_link.h>
#include <whalebone/uuid_page_storage.hpp>
#include <options/options.hpp>

#include <leveldb/db.h>
#include <boost/scoped_ptr.hpp>

#include <iostream>
#include <fstream>
#include <cstdlib>

struct TLevelDbReaderOptions {
public:
    std::string             InputPath;
    CUuidPageStorageOptions Output;
    unsigned int            ProgressChunk;
    std::string             TmpFilePath;
};

int main() {
    TLevelDbReaderOptions           options;
    options.InputPath                   =   "/home/hotdox/tmp/whale-test/pages/";
    options.Output.HierarchicalLevel    =   3;
    options.Output.LevelLength          =   2;
    options.Output.BaseDirectory        =   "/home/hotdox/tmp/hierarchical/";
    options.ProgressChunk               =   100;
    options.TmpFilePath                 =   "/home/hotdox/tmp/tmp";

    boost::scoped_ptr<leveldb::DB>  db(NULL);
    leveldb::Options                openOptions;
    leveldb::DB*                    dbPtr   =   NULL;
    leveldb::Status                 isOpen  =   leveldb::DB::Open( openOptions
                                                                 , options.InputPath
                                                                 , &dbPtr );

    if (false == isOpen.ok()) {
        std::cerr << "cannot open \"" << options.InputPath << "\"" << std::endl;
        return EXIT_FAILURE;
    }
    db.reset(dbPtr);

    CUuidPageStorage        outputStorage(options.Output);
    leveldb::ReadOptions    readOptions;
    readOptions.verify_checksums    =   true;
    readOptions.fill_cache          =   false;

    unsigned int                            recordNum   =   0;
    bool                                    isOk        =   true;
    boost::scoped_ptr<leveldb::Iterator>    recordIter(db->NewIterator(readOptions));
    recordIter->SeekToFirst();
    while ((true == recordIter->Valid()) && (true == isOk))  {
        std::string     strUrl  =   "http://";
        strUrl  +=  recordIter->key().ToString();

        CLink           url(strUrl);
        leveldb::Slice  page    =   recordIter->value();
        recordIter->Next();

        if ((0 != recordNum) && (0 == recordNum % options.ProgressChunk)) {
            std::cout << "Write " << recordNum << " records" << std::endl;
        }
        recordNum   +=  1;

        std::ofstream   tmpFile(options.TmpFilePath.c_str(), std::ios::trunc|std::ios::out);
        if (false == tmpFile.is_open()) {
            isOk    =   false;
            std::cerr << "cannot open \"" << options.TmpFilePath << "\"" << std::endl;
            continue;
        }
        tmpFile.write(page.data(), page.size());
        tmpFile.close();

        bool    isStoraged  =   outputStorage.StoreFile( url.getServer()
                                                       , url.getUri()
                                                       , ""
                                                       , options.TmpFilePath );

        if (false == isStoraged) {
            isOk    =   false;
            std::cerr << "cannot store \"" << options.TmpFilePath << "\"" << std::endl;
            continue;
        }

    }

    if (false == isOk) {
        return EXIT_FAILURE;
    }



    return EXIT_SUCCESS;
}
