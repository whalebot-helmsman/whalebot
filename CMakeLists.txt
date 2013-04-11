cmake_minimum_required(VERSION 2.6)

set(WHALEBOT_SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

add_subdirectory( statsem_string )
add_subdirectory( 3dparty/google-url )
add_subdirectory( webspider )
