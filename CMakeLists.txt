cmake_minimum_required(VERSION 2.6)

project(webspider)

set(WHALEBOT_SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(WHALEBOT_BUILD_ROOT ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${WHALEBOT_BUILD_ROOT}/lib" )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${WHALEBOT_BUILD_ROOT}/lib" )
set(EXECUTABLE_OUTPUT_PATH         "${WHALEBOT_BUILD_ROOT}/bin" )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory( statsem_string )
add_subdirectory( 3dparty )
add_subdirectory( webspider )
add_subdirectory( link )
