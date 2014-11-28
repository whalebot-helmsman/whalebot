
find_package(Boost 1.37 REQUIRED program_options)

include_directories(${Boost_INCLUDE_DIRS})
set( SOURCES options.cpp )

add_library(whale_options STATIC ${SOURCES})
target_link_libraries(whale_options ${Boost_LIBRARIES})
