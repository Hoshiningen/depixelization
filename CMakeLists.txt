# Depixelization

cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(depixelization CXX)

option(DEPIXELIZATION_BUILD_TESTS "Build unit tests" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set directory paths
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_executable(depixelization ${SOURCE_DIR}/main.cpp)

include(${CMAKE_DIR}/LinkSTB.cmake)
LinkSTB(depixelization PRIVATE)

find_package(Boost 1.70 REQUIRED)

# Configure the executable
target_link_libraries(depixelization PUBLIC reshaper)
target_include_directories(depixelization PUBLIC
    ${SOURCE_DIR}/reshaper/include/reshaper
    ${Boost_INCLUDE_DIRS})

set_target_properties(depixelization PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

# Add projects to the executable
add_subdirectory(${SOURCE_DIR}/reshaper/source/reshaper)
add_subdirectory(${SOURCE_DIR}/reshaper/source/private)

# Set project folders
set_target_properties(depixelization PROPERTIES FOLDER Depixelization)
set_target_properties(reshaper PROPERTIES FOLDER Depixelization/lib/reshaper)
set_target_properties(reshaper-impl PROPERTIES FOLDER Depixelization/lib/reshaper)

if (DEPIXELIZATION_BUILD_TESTS)
    enable_testing()
    add_subdirectory(${SOURCE_DIR}/reshaper/source/tests)
    set_target_properties(reshaper-tests PROPERTIES FOLDER Depixelization/lib/reshaper)
endif()
