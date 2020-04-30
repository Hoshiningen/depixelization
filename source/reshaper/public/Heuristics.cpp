#include <Heuristics.h>
#include <type_traits>

namespace dpa::graph::heuristics
{
FilteredEdges operator&(FilteredEdges a, FilteredEdges b)
{
    return static_cast<FilteredEdges>(static_cast<int>(a) & static_cast<int>(b));
}

FilteredEdges operator|(FilteredEdges a, FilteredEdges b)
{
    return static_cast<FilteredEdges>(static_cast<int>(a) | static_cast<int>(b));
}

FilteredEdges operator^(FilteredEdges a, FilteredEdges b)
{
    return static_cast<FilteredEdges>(static_cast<int>(a) ^ static_cast<int>(b));
}

bool hasFilter(FilteredEdges filters, FilteredEdges target) noexcept
{
    return (filters & target) == target;
}
}