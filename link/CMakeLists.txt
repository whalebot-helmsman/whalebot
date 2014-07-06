include_directories( ${WHALEBOT_SRC_ROOT}/3dparty/google-url
                     ${WHALEBOT_SRC_ROOT}/3dparty/google-url/googleurl
                     ${WHALEBOT_SRC_ROOT}/3dparty/google-url/googleurl/src )

set( SOURCES collector_link.cpp
             filters.cpp
             link_buffer.cpp
             link_factory.cpp
             prefix.cpp
             whale_link.cpp )

add_library(whale_link STATIC ${SOURCES})

target_link_libraries ( whale_link gurl
                                   ${ICU_LIBRARY}
                                   hubbub )
