cmake_minimum_required(VERSION 3.16)
project(Draftz CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# On macOS, Homebrew installs to a different prefix depending on the
# chip (Apple Silicon vs Intel). Add both so find_package(FLTK) can
# actually locate it no matter which runner we're on.
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH /opt/homebrew /usr/local)
endif()

find_package(FLTK REQUIRED)

add_executable(Draftz main.cpp)

# main.cpp does #include "header/features.h" - this makes that path
# resolve relative to the project root, on every platform.
target_include_directories(Draftz PRIVATE ${CMAKE_SOURCE_DIR} ${FLTK_INCLUDE_DIR})
target_link_libraries(Draftz PRIVATE ${FLTK_LIBRARIES})

if(WIN32)
    # Build as a real GUI app (no console window popping up behind it).
    set_target_properties(Draftz PROPERTIES WIN32_EXECUTABLE TRUE)

    if(MINGW)
        # Statically link the compiler runtime so the .exe we ship
        # doesn't depend on the user having MinGW DLLs installed -
        # it just works standalone on Windows 10/11.
        target_link_options(Draftz PRIVATE -static -static-libgcc -static-libstdc++)
    endif()
endif()
