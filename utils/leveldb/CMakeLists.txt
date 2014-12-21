set(PROJECT leveldb_reader)
find_package(leveldb REQUIRED)
find_package(SNAPPY REQUIRED)
find_package(Boost 1.37 REQUIRED program_options)

set( SOURCES leveldb_reader.cpp )

add_executable(${PROJECT} ${SOURCES})

target_link_libraries(${PROJECT} whalebone
                                 whale_link
                                 ${Boost_LIBRARIES})
