# Reshaper implementation

include(${CMAKE_DIR}/LinkSTB.cmake)

set(sources
    HeuristicHelper.cpp
    ImageView.cpp
    Implementation.cpp
    SimilarityGraphImpl.cpp
    VoronoiImpl.cpp)

set(includes
    GraphUtils.h
    GraphVisualizer.h
    GraphVisualizationStrategy.h
    HeuristicHelper.h
    ImageView.h
    Implementation.h
    SimilarityGraphImpl.h
    SimilarityGraphVisualizationStrategy.h
    VoronoiImpl.h
    VoronoiGraphVisualizationStrategy.h)

add_library(reshaper-impl STATIC ${sources} ${includes})

# Link things
LinkSTB(reshaper-impl PRIVATE)

target_include_directories(reshaper-impl
    PUBLIC ${Boost_INCLUDE_DIRS}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${SOURCE_DIR}/reshaper/public
    PRIVATE ${SOURCE_DIR}/utility/public)

set_target_properties(reshaper-impl PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

# Treat warnings as errors
if(MSVC)
    target_compile_options(reshaper-impl PRIVATE /W4 /WX)
else()
    target_compile_options(reshaper-impl PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
