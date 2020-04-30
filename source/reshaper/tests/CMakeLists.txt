# Reshaper testing

include(${CMAKE_DIR}/LinkGTest.cmake)
include(${CMAKE_DIR}/LinkSTB.cmake)
include(GoogleTest)

set(sources 
    ImageTests.cpp 
    ImageUtilTests.cpp
    ImageViewTests.cpp
    SimilarityGraphTests.cpp
    VoronoiTests.cpp)

set(includes 
    ImageTests.h
    ImageUtilTests.h
    ImageViewTests.h
    SimilarityGraphTests.h
    VoronoiTests.h
    TestUtility.h)

add_executable(reshaper-tests ${sources} ${includes})

LinkGTest(reshaper-tests PRIVATE)
LinkSTB(reshaper-tests PRIVATE)
target_link_libraries(reshaper-tests PRIVATE reshaper reshaper-impl)

target_include_directories(reshaper-tests
    PRIVATE ${Boost_INCLUDE_DIRS}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${SOURCE_DIR}/reshaper/private
    PRIVATE ${SOURCE_DIR}/reshaper/public)

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
    