# Reshaper Public Interface

include(${CMAKE_DIR}/LinkSTB.cmake)

set(HEURISTICS_SOURCE
    CurvesHeuristic.cpp
    DissimilarHeuristic.cpp
    Heuristics.cpp
    IslandsHeuristic.cpp
    SparsePixelsHeuristic.cpp)

set(HEURISTICS_INCLUDE
    CurvesHeuristic.h
    DissimilarHeuristic.h
    Heuristics.h
    IslandsHeuristic.h
    SparsePixelsHeuristic.h)  

set(GRAPH_SOURCE
    SimilarityGraph.cpp
    Voronoi.cpp)

set(GRAPH_INCLUDE
    SimilarityGraph.h
    Voronoi.h)

set(IMAGE_SOURCE
    Image.cpp
    ImageUtil.cpp)

set(IMAGE_INCLUDE
    Image.h
    ImageUtil.h
    Pixel.h)

set(sources ${GRAPH_SOURCE} ${HEURISTICS_SOURCE} ${IMAGE_SOURCE})
set(includes ${GRAPH_INCLUDE} ${HEURISTICS_INCLUDE} ${IMAGE_INCLUDE})

add_library(reshaper STATIC ${sources} ${includes})
add_dependencies(reshaper reshaper-impl)

# Find the third party libraries
find_package(Boost 1.70 REQUIRED)

# Link things
LinkSTB(reshaper PRIVATE)

target_link_libraries(reshaper PRIVATE reshaper-impl utility)

target_include_directories(reshaper
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${SOURCE_DIR}/utility/public
    PUBLIC ${Boost_INCLUDE_DIRS})

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
