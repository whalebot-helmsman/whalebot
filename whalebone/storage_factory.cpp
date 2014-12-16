#include "storage_factory.hpp"
#include "uuid_page_storage.hpp"
#include "filename_handler.h"
#include "leveldb_storage.hpp"

IPageStorage* CreateStorage(const CPageStorageOptions& options)
{
    IPageStorage*   storage =   NULL;

    switch(options.Type) {

        case CPageStorageOptions::ETypeUuid    : {
            storage =   new CUuidPageStorage(options.Uuid);;
            break;
        }

        case CPageStorageOptions::ETypePlain   : {
            storage =   new CFilenameHandler(options.Plain.BaseDirectory);
            break;
        }

        case CPageStorageOptions::ETypeLevelDb : {
            storage =   new CLevelDbPageStorage(options.LevelDb);
            break;
        }

        default : {
            storage =   NULL;
            break;
        }

    }

    return storage;
}
