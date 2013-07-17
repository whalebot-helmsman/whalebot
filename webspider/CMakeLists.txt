cmake_minimum_required(VERSION 2.6)
project(webspider)

find_package(Boost 1.37 REQUIRED filesystem system thread program_options date_time)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include
                     ${WHALEBOT_SRC_ROOT}/statsem_string/include
                     ${WHALEBOT_SRC_ROOT}/3dparty/google-url
                     ${WHALEBOT_SRC_ROOT}/3dparty/google-url/googleurl
                     ${WHALEBOT_SRC_ROOT}/3dparty/google-url/googleurl/src
                     ${Boost_INCLUDE_DIRS}
                     ${WHALEBOT_SRC_ROOT}/3dparty/hubbub-0.1.1/include/ )

aux_source_directory("./src/" SOURCES)

list ( APPEND CMAKE_CXX_FLAGS "-Wall")

add_executable( whalebot ${SOURCES} )
target_link_libraries ( whalebot ${Boost_LIBRARIES}
                                 string_parse
                                 pthread
                                 gurl
                                 ${ICU_LIBRARY}
                                 hubbub )

install ( TARGETS whalebot RUNTIME DESTINATION bin CONFIGURATIONS RELEASE )
