# - Try to find Htmlcxx
# Once done this will define
#  HTMLCXX_FOUND - System has htmlcxx
#  HTMLCXX_INCLUDE_DIRS - The htmlcxx include directories
#  HTMLCXX_LIBRARIES - The libraries needed to use htmlcxx
#  HTMLCXX_DEFINITIONS - Compiler switches required for using htmlcxx

find_package(PkgConfig)
pkg_check_modules(PC_HTMLCXX QUIET htmlcxx)
set(HTMLCXX_DEFINITIONS ${PC_HTMLCXX_CFLAGS_OTHER})

find_path(HTMLCXX_INCLUDE_DIR htmlcxx/html/ParserSax.h
          HINTS ${PC_HTMLCXX_INCLUDEDIR} ${PC_HTMLCXX_INCLUDE_DIRS} )

find_library(HTMLCXX_LIBRARY NAMES htmlcxx
             HINTS ${PC_HTMLCXX_LIBDIR} ${PC_HTMLCXX_LIBRARY_DIRS} )

set(HTMLCXX_LIBRARIES ${HTMLCXX_LIBRARY} )
set(HTMLCXX_INCLUDE_DIRS ${HTMLCXX_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set HTMLCXX_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Htmlcxx  DEFAULT_MSG
                                  HTMLCXX_LIBRARY HTMLCXX_INCLUDE_DIR)

mark_as_advanced(HTMLCXX_INCLUDE_DIR HTMLCXX_LIBRARY )
