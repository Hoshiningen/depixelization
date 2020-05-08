# Reshaper testing

include(${CMAKE_DIR}/LinkGTest.cmake)
include(GoogleTest)

set(sources 
    UtilityTests.cpp)

set(includes 
    UtilityTests.h)

add_executable(utility-tests ${sources} ${includes})

LinkGTest(utility-tests PRIVATE)
target_link_libraries(utility-tests PRIVATE utility)

target_include_directories(utility-tests
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${SOURCE_DIR}/utility/public)

set_target_properties(utility-tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

# Ignore warnings
if(MSVC)
    target_compile_options(utility-tests PRIVATE /w)
else()
    target_compile_options(utility-tests PRIVATE -w)
endif()

gtest_add_tests(
    TARGET utility-tests
    SOURCES ${sources}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
    