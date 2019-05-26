#pragma once

#include <CurvesHeuristic.h>
#include <DissimilarHeuristic.h>
#include <IslandsHeuristic.h>
#include <SparsePixelsHeuristic.h>

#include <variant>

namespace dpa::graph::heuristics
{
/*
    Flags for filtering edges in the similarity graph
*/
enum class FilteredEdges
{
    eNone = 0,
    eCurves = 1,
    eDissimilar = 2,
    eIslands = 4,
    eSparsePixels = 8,
    eAll = 15,
};

FilteredEdges operator&(FilteredEdges a, FilteredEdges b);
FilteredEdges operator|(FilteredEdges a, FilteredEdges b);
FilteredEdges operator^(FilteredEdges a, FilteredEdges b);

/*
    Determines if the target filter was bitwise or'd into the given filters

    @filters A set of filters or'd together
    @target A filter to determine if it resides in the filter flags
    @returns True if the target filter is packed in the filter flag
*/
bool hasFilter(FilteredEdges filters, FilteredEdges target) noexcept;

/*
    Visits each of the filter edges in the enum with
    the given visitor function

    @param visitor A function that takes a filter edge and returns void
*/
template<typename Visitor>
void VisitEdgeFilters(Visitor visitor)
{
    auto filters =
    {
        FilteredEdges::eCurves, FilteredEdges::eDissimilar,
        FilteredEdges::eIslands, FilteredEdges::eSparsePixels
    };

    for (auto filter : filters)
        visitor(filter);
}

using Heuristic = std::variant<Curves, DissimilarPixels, Islands, SparsePixels>;
}
