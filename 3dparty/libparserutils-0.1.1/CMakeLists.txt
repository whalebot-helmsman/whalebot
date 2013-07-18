set(TITLE parserutils)

include_directories( include/parserutils )
include_directories( include )
include_directories( src )

add_custom_command( OUTPUT            ${CMAKE_CURRENT_SOURCE_DIR}/src/charset/aliases.inc
                    COMMAND           perl ${CMAKE_CURRENT_SOURCE_DIR}/build/make-aliases.pl
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    MAIN_DEPENDENCY   ${CMAKE_CURRENT_SOURCE_DIR}/build/Aliases
                    COMMENT           "Build parserutils aliases.inc" )

find_package(Iconv)

if (NOT ICONV_FOUND)
    message(STATUS "Iconv not found, build parserutils without various encoding support")
    list(APPEND CMAKE_C_FLAGS "-DWITHOUT_ICONV_FILTER")
else(NOT ICONV_FOUND)
    include_directories(${ICONV_INCLUDE_DIR})
endif(NOT ICONV_FOUND)

set( SOURCES src/input/inputstream.c
             src/input/filter.c
             src/charset/aliases.c
             src/charset/codec.c
             src/charset/encodings/utf16.c
             src/charset/encodings/utf8.c
             src/charset/codecs/codec_utf8.c
             src/charset/codecs/codec_ext8.c
             src/charset/codecs/codec_ascii.c
             src/charset/codecs/codec_utf16.c
             src/charset/codecs/codec_8859.c
             src/utils/errors.c
             src/utils/vector.c
             src/utils/stack.c
             src/utils/buffer.c )

set(PARSER_UTILS_ALIASES parserutils_aliases)
add_custom_target(${PARSER_UTILS_ALIASES} DEPENDS src/charset/aliases.inc)

add_library(${TITLE} STATIC ${SOURCES})
target_link_libraries(${TITLE} ${ICONV_LIBRARIES})
add_dependencies(${TITLE} ${PARSER_UTILS_ALIASES})
