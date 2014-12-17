find_package(Boost 1.37 REQUIRED filesystem)
find_package(leveldb REQUIRED)
find_package(SNAPPY REQUIRED)

include_directories( ${Boost_INCLUDE_DIRS}
                     ${LEVELDB_INCLUDE_DIRS} )
set( SOURCES uuid_page_storage.cpp
             filename_handler.cpp
             storage_factory.cpp
             leveldb_storage.cpp   )

add_library(whalebone STATIC ${SOURCES})

target_link_libraries( whalebone ${Boost_LIBRARIES}
                                 ${LEVELDB_LIBRARIES}
                                 ${SNAPPY_LIBRARIES}  )
