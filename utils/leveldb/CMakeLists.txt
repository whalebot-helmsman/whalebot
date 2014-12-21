set(PROJECT leveldb_reader)
find_package(leveldb REQUIRED)
find_package(SNAPPY REQUIRED)

set( SOURCES leveldb_reader.cpp )

add_executable(${PROJECT} ${SOURCES})

target_link_libraries(${PROJECT} whalebone
                                 whale_link)
