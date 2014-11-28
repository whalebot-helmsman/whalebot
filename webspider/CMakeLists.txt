 include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include
                      ${WHALEBOT_SRC_ROOT}/statsem_string/include
                      ${Boost_INCLUDE_DIRS} )

find_package(Boost 1.37 REQUIRED filesystem system thread date_time)

aux_source_directory("./src/" SOURCES)

list ( APPEND CMAKE_CXX_FLAGS "-Wall")

add_executable( whalebot ${SOURCES} )
target_link_libraries ( whalebot ${Boost_LIBRARIES}
                                 string_parse
                                 pthread
                                 whale_options
                                 whale_link )

install ( TARGETS whalebot RUNTIME DESTINATION bin CONFIGURATIONS RELEASE )
