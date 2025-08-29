cmake_minimum_required(VERSION 3.16)
project(RealmsOfEldoria VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find required packages
find_package(SDL2 REQUIRED)

# Include directories
include_directories(include)
include_directories(lib)

# Core library
file(GLOB_RECURSE LIB_SOURCES "lib/*.cpp" "lib/*.h")
add_library(RealmsLib ${LIB_SOURCES})
target_link_libraries(RealmsLib SDL2::SDL2)

# Game client executable
file(GLOB_RECURSE CLIENT_SOURCES "client/*.cpp" "client/*.h")
add_executable(RealmsClient ${CLIENT_SOURCES})
target_link_libraries(RealmsClient RealmsLib SDL2::SDL2)

# Optional server executable
file(GLOB_RECURSE SERVER_SOURCES "server/*.cpp" "server/*.h")
add_executable(RealmsServer ${SERVER_SOURCES})
target_link_libraries(RealmsServer RealmsLib)

# Set output directories
set_target_properties(RealmsClient PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
set_target_properties(RealmsServer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)