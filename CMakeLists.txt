cmake_minimum_required(VERSION 3.6)
project(RTKLIB)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake" "${CMAKE_CURRENT_LIST_DIR}/cmake/common")
include(CCache)
include(ClangFormat)
swift_setup_clang_format()
include(ClangTidy)

add_subdirectory(src)
add_subdirectory(app/consapp/str2str)

