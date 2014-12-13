find_package(Boost 1.37 REQUIRED filesystem)

include_directories(${Boost_INCLUDE_DIRS})
set( SOURCES uuid_page_storage.cpp
             filename_handler.cpp )

add_library(whalebone STATIC ${SOURCES})
target_link_libraries(whalebone ${Boost_LIBRARIES})
