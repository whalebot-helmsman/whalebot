#include "storage_factory.hpp"
#include "uuid_page_storage.hpp"
#include "filename_handler.h"

IPageStorage* CreateStorage(const CStorageOptions& options)
{
    IPageStorage*   storage =   NULL;
    if (CStorageOptions::EPageStorageTypeUuid == options.PageStorageType) {
        storage =   new CUuidPageStorage( options.PageStorageDirectory
                                        , options.HierarchicalLevel    );
    }
    else {
        storage =   new CFilenameHandler(options.PageStorageDirectory);
    }

    return storage;
}
