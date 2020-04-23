# Reshaper Public Interface

include(${CMAKE_DIR}/LinkSTB.cmake)

set(RESHAPER_INCLUDE_DIR ${SOURCE_DIR}/reshaper/include/reshaper)

set(HEURISTICS_SOURCE
    CurvesHeuristic.cpp
    DissimilarHeuristic.cpp
    Heuristics.cpp
    IslandsHeuristic.cpp
    SparsePixelsHeuristic.cpp)

set(HEURISTICS_INCLUDE
    ${RESHAPER_INCLUDE_DIR}/CurvesHeuristic.h
    ${RESHAPER_INCLUDE_DIR}/DissimilarHeuristic.h
    ${RESHAPER_INCLUDE_DIR}/Heuristics.h
    ${RESHAPER_INCLUDE_DIR}/IslandsHeuristic.h
    ${RESHAPER_INCLUDE_DIR}/SparsePixelsHeuristic.h)  

set(GRAPH_SOURCE
    SimilarityGraph.cpp
    Voronoi.cpp)

set(GRAPH_INCLUDE
    ${RESHAPER_INCLUDE_DIR}/SimilarityGraph.h
    ${RESHAPER_INCLUDE_DIR}/Voronoi.h)

set(IMAGE_SOURCE
    Image.cpp
    ImageUtil.cpp)

set(IMAGE_INCLUDE
    ${RESHAPER_INCLUDE_DIR}/Image.h
    ${RESHAPER_INCLUDE_DIR}/ImageUtil.h
    ${RESHAPER_INCLUDE_DIR}/Pixel.h)

set(sources ${GRAPH_SOURCE} ${HEURISTICS_SOURCE} ${IMAGE_SOURCE})
set(includes ${GRAPH_INCLUDE} ${HEURISTICS_INCLUDE} ${IMAGE_INCLUDE})

add_library(reshaper STATIC ${sources} ${includes})
add_dependencies(reshaper reshaper-impl)

# Find the third party libraries
find_package(Boost 1.70 REQUIRED)

# Link things
LinkSTB(reshaper PRIVATE)

target_link_libraries(reshaper PRIVATE reshaper-impl)

target_include_directories(reshaper PUBLIC
    ${Boost_INCLUDE_DIRS}
    ${RESHAPER_INCLUDE_DIR}
    ${SOURCE_DIR}/reshaper/include/private)

set_target_properties(reshaper PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

# Treat warnings as errors
if(MSVC)
    target_compile_options(reshaper PRIVATE /W4 /WX)
else()
    target_compile_options(reshaper PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
