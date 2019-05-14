# Reshaper testing

include(${CMAKE_DIR}/LinkGTest.cmake)
include(${CMAKE_DIR}/LinkSTB.cmake)
include(GoogleTest)

set(TESTS_INCLUDE_DIR ${SOURCE_DIR}/reshaper/include/tests)

set(sources ImageViewTests.cpp ImageTests.cpp)
set(includes 
    ${TESTS_INCLUDE_DIR}/ImageViewTests.h
    ${TESTS_INCLUDE_DIR}/ImageTests.h)

add_executable(reshaper-tests ${sources} ${includes})

LinkGTest(reshaper-tests PRIVATE)
LinkSTB(reshaper-tests PRIVATE)
target_link_libraries(reshaper-tests PRIVATE reshaper reshaper-impl)

target_include_directories(reshaper-tests PRIVATE
    ${TESTS_INCLUDE_DIR}
    ${SOURCE_DIR}/reshaper/include/private
    ${SOURCE_DIR}/reshaper/include/reshaper)

set_target_properties(reshaper-tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

# Ignore warnings
if(MSVC)
    target_compile_options(reshaper-tests PRIVATE /w)
else()
    target_compile_options(reshaper-tests PRIVATE -w)
endif()

gtest_add_tests(
    TARGET reshaper-tests
    SOURCES ${sources}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
    