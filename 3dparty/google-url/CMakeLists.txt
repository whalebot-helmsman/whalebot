cmake_minimum_required(VERSION 2.6)
project(gurl)


include_directories("./")
include_directories("./googleurl")

set (TEST_SOURCES '')

aux_source_directory("./googleurl/src/" SOURCES)


foreach (SOURCE ${SOURCES})    
    if (SOURCE MATCHES ".*test.*")
        list (APPEND TEST_SOURCES ${SOURCE})
    endif(SOURCE MATCHES ".*test.*")
endforeach(SOURCE)

list (REMOVE_ITEM SOURCES ${TEST_SOURCES})

list ( APPEND SOURCES "./googleurl/base/string16.cc")

list ( APPEND CMAKE_CXX_FLAGS "-Wno-deprecated")

add_library(gurl STATIC ${SOURCES})
