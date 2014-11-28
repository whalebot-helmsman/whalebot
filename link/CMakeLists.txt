find_package(Boost 1.37 REQUIRED program_options)

include_directories(${Boost_INCLUDE_DIRS})

set( SOURCES filters.cpp
             link_buffer.cpp
             url_normalizer.cpp
             prefix.cpp
             link_extractor.cpp
             whale_link.cpp )

add_library(whale_link STATIC ${SOURCES})

target_link_libraries ( whale_link gurl
                                   ${Boost_LIBRARIES}
                                   hubbub )
