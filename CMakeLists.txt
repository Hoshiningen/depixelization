# Depixelization

cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(Depixelization CXX)

option(DEPIXELIZATION_BUILD_TESTS "Build unit tests" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set directory paths
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(sources
    ${SOURCE_DIR}/depixelization/public/main.cpp
    ${SOURCE_DIR}/depixelization/public/ProgramDriver.cpp)

set(includes
    ${SOURCE_DIR}/depixelization/public/ProgramDriver.h)

add_executable(depixelization ${sources} ${includes})

include(${CMAKE_DIR}/LinkSTB.cmake)
LinkSTB(depixelization PRIVATE)

include(${CMAKE_DIR}/LinkArgParse.cmake)
LinkArgParse(depixelization PRIVATE)

find_package(Boost 1.70 REQUIRED)

# Configure the executable
target_link_libraries(depixelization PUBLIC reshaper ${Boost_LIBRARY_DIRS} utility)
target_include_directories(depixelization
    PUBLIC ${SOURCE_DIR}/depixelization/public
    PUBLIC ${SOURCE_DIR}/reshaper/public
    PUBLIC ${SOURCE_DIR}/utility/public
    PRIVATE ${SOURCE_DIR}/reshaper/private)

set_target_properties(depixelization PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

# Add projects to the executable
add_subdirectory(${SOURCE_DIR}/reshaper/public)
add_subdirectory(${SOURCE_DIR}/reshaper/private)
add_subdirectory(${SOURCE_DIR}/utility/public)

# Set project folders
set_target_properties(depixelization PROPERTIES FOLDER Depixelization)
set_target_properties(reshaper PROPERTIES FOLDER Depixelization/Reshaper)
set_target_properties(reshaper-impl PROPERTIES FOLDER Depixelization/Reshaper)
set_target_properties(utility PROPERTIES FOLDER Depixelization/Utility)

if (DEPIXELIZATION_BUILD_TESTS)
    enable_testing()

    add_subdirectory(${SOURCE_DIR}/reshaper/tests)
    set_target_properties(reshaper-tests PROPERTIES FOLDER Depixelization/Reshaper)

    add_subdirectory(${SOURCE_DIR}/utility/tests)
    set_target_properties(utility-tests PROPERTIES FOLDER Depixelization/Utility)
endif()
