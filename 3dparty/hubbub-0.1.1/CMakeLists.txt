
set(HUBBUB_ENTITIES hubbub_entities)
add_custom_command( OUTPUT  src/tokeniser/entities.inc
                    DEPENDS build/Entities
                    COMMAND perl build/make-entities.pl
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
add_custom_target(${HUBBUB_ENTITIES} ALL DEPENDS src/tokeniser/entities.inc )

set(TITLE hubbub)

include_directories(../libparserutils-0.1.1/include)
include_directories(include/hubbub)
include_directories(include)
include_directories(src)

set( SOURCES src/treebuilder/in_head_noscript.c
             src/treebuilder/in_caption.c
             src/treebuilder/generic_rcdata.c
             src/treebuilder/after_after_body.c
             src/treebuilder/in_row.c
             src/treebuilder/in_body.c
             src/treebuilder/initial.c
             src/treebuilder/in_foreign_content.c
             src/treebuilder/before_head.c
             src/treebuilder/after_after_frameset.c
             src/treebuilder/after_body.c
             src/treebuilder/before_html.c
             src/treebuilder/in_select_in_table.c
             src/treebuilder/in_cell.c
             src/treebuilder/in_table_body.c
             src/treebuilder/treebuilder.c
             src/treebuilder/in_frameset.c
             src/treebuilder/in_table.c
             src/treebuilder/in_column_group.c
             src/treebuilder/in_select.c
             src/treebuilder/after_frameset.c
             src/treebuilder/after_head.c
             src/treebuilder/in_head.c
             src/charset/detect.c
             src/parser.c
             src/tokeniser/tokeniser.c
             src/tokeniser/entities.c
             src/utils/errors.c
             src/utils/string.c )

add_library(${TITLE} STATIC ${SOURCES})
target_link_libraries(${TITLE} parserutils)
add_dependencies(${TITLE} ${HUBBUB_ENTITIES})
