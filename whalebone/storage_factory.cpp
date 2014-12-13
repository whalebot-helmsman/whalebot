#include "storage_factory.hpp"
#include "uuid_page_storage.hpp"
#include "filename_handler.h"

IPageStorage* CreateStorage(const CPageStorageOptions& options)
{
    IPageStorage*   storage =   NULL;
    if (CPageStorageOptions::EPageStorageTypeUuid == options.Type) {
        storage =   new CUuidPageStorage( options.Uuid.BaseDirectory
                                        , options.Uuid.HierarchicalLevel    );
    }
    else {
        storage =   new CFilenameHandler(options.Plain.BaseDirectory);
    }

    return storage;
}
