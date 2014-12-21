cmake_minimum_required(VERSION 2.6)

project(webspider)

set(WHALEBOT_SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(WHALEBOT_BUILD_ROOT ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${WHALEBOT_BUILD_ROOT}/lib" )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${WHALEBOT_BUILD_ROOT}/lib" )
set(EXECUTABLE_OUTPUT_PATH         "${WHALEBOT_BUILD_ROOT}/bin" )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#include directories for 3d-party libs
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/3dparty/google-url
                     ${CMAKE_CURRENT_SOURCE_DIR}/3dparty/google-url/googleurl
                     ${CMAKE_CURRENT_SOURCE_DIR}/3dparty/google-url/googleurl/src
                     ${CMAKE_CURRENT_SOURCE_DIR}/3dparty/hubbub-0.1.1/include )

add_subdirectory( statsem_string )
add_subdirectory( 3dparty )
add_subdirectory( webspider )
add_subdirectory( link )
add_subdirectory( options )
add_subdirectory( whalebone )
add_subdirectory( utils )
