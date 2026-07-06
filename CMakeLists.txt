cmake_minimum_required(VERSION 3.10)
project(Draftz)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find FLTK package
find_package(FLTK REQUIRED)

# Include FLTK directories
include_directories(${FLTK_INCLUDE_DIR})

# Add the executable
add_executable(Draftz main.cpp)

# Link FLTK libraries
target_link_libraries(Draftz ${FLTK_LIBRARIES})

# Platform-specific settings
if(WIN32)
    target_link_libraries(Draftz ws2_32)
elseif(APPLE)
    target_link_libraries(Draftz "-framework Cocoa")
elseif(UNIX)
    target_link_libraries(Draftz X11)
endif()
