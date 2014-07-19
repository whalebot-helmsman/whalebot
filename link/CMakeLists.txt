include_directories( ${WHALEBOT_SRC_ROOT}/3dparty/hubbub-0.1.1/include/
                     ${WHALEBOT_SRC_ROOT}/3dparty/google-url
                     ${WHALEBOT_SRC_ROOT}/3dparty/google-url/googleurl
                     ${WHALEBOT_SRC_ROOT}/3dparty/google-url/googleurl/src )

set( SOURCES filters.cpp
             link_buffer.cpp
             url_normalizer.cpp
             prefix.cpp
             link_extractor.cpp
             whale_link.cpp )

add_library(whale_link STATIC ${SOURCES})

target_link_libraries ( whale_link gurl
                                   ${ICU_LIBRARY}
                                   hubbub )
