#include <link/whale_link.h>
#include <whalebone/uuid_page_storage.hpp>
#include <options/options.hpp>

#include <leveldb/db.h>
#include <boost/program_options.hpp>
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

int main(int argc, char* argv[]) {
    TLevelDbReaderOptions           options;
    //TODO: calculate hierarchical options of uuid page storage using size of leveldb storage
    options.Output.HierarchicalLevel    =   3;
    options.Output.LevelLength          =   2;

    boost::program_options::options_description desc("leveldb converter options");
    desc.add_options()
            ("help,h",     "show this message")
            ("input,i",    boost::program_options::value<std::string>(&options.InputPath)->default_value("")->required(),            "path to leveldb storage")
            ("output,o",   boost::program_options::value<std::string>(&options.Output.BaseDirectory)->default_value("")->required(), "path to new uuid storage")
            ("tmp-file,t", boost::program_options::value<std::string>(&options.TmpFilePath)->default_value("tmp"),                   "path to tmp file")
            ("progress,p", boost::program_options::value<unsigned int>(&options.ProgressChunk)->default_value(1000),                 "show progress messages every X records");

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << "Usage: options_description [options]\n";
        std::cout << desc;
        return EXIT_SUCCESS;
    }

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
