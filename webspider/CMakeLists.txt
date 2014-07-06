include_directories( ${WHALEBOT_SRC_ROOT}/3dparty/google-url
                     ${WHALEBOT_SRC_ROOT}/3dparty/google-url/googleurl
                     ${WHALEBOT_SRC_ROOT}/3dparty/google-url/googleurl/src
                     ${WHALEBOT_SRC_ROOT}/3dparty/hubbub-0.1.1/include/
                     ${CMAKE_CURRENT_SOURCE_DIR}/include
                     ${WHALEBOT_SRC_ROOT}/statsem_string/include
                     ${Boost_INCLUDE_DIRS} )

find_package(Boost 1.37 REQUIRED filesystem system thread program_options date_time)

aux_source_directory("./src/" SOURCES)

list ( APPEND CMAKE_CXX_FLAGS "-Wall")

add_executable( whalebot ${SOURCES} )
target_link_libraries ( whalebot ${Boost_LIBRARIES}
                                 string_parse
                                 pthread
                                 whale_link )

install ( TARGETS whalebot RUNTIME DESTINATION bin CONFIGURATIONS RELEASE )
