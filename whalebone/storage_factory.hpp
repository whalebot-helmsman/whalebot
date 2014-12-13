#pragma once
#include "ipage_storage.hpp"
#include <options/options.hpp>

IPageStorage* CreateStorage(const CPageStorageOptions& options);
