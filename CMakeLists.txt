cmake_minimum_required(VERSION 3.26)

project(pngr)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(pngr lib/image/drawer.cc lib/image/image.cc lib/image/png.cc cli/cli.cc pngr.cc)

find_package(PNG REQUIRED 1.6)
target_link_libraries(pngr PNG)
