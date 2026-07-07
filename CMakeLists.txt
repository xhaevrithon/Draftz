cmake_minimum_required(VERSION 3.16)

project(Draftz)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(FLTK REQUIRED)

add_executable(Draftz
    main.cpp
)

target_include_directories(Draftz PRIVATE
    header
)

target_link_libraries(Draftz PRIVATE
    fltk
)
