#pragma once

#include <GraphUtils.h>
#include <HeuristicHelper.h>

#include <boost/graph/depth_first_search.hpp>

namespace boost::uuids
{
    struct uuid;
}

namespace dpa::graph::heuristics
{
class SparsePixels : public boost::default_dfs_visitor, IMarkedEdgeHelper
{
public:

    /*
        Default constructor. Creates the uuid for this instance
    */
    SparsePixels();

    /*
        Parameterized ctor, which pulls in the image dimensions
        for point expansion during the dfs
    */
    explicit SparsePixels(const utility::Point2D<int>& imageDims);

    /*
        Copy constructor, which sets this uuid to other's uuid

        @param other Another sparse pixels heuristic to create this object from
    */
    SparsePixels(const SparsePixels& other);

    /*
        Copy assignment. This sets this uuid to the other's uuid

        @param other Another sparse pixels heuristic to create this object from
        @returns A reference to this instance
    */
    SparsePixels& operator=(const SparsePixels& other);

    template <class Edge, class Graph>
    void examine_edge(Edge edge, const Graph& graph)
    {
        using Vertex = Graph::vertex_descriptor;

        const auto [imageWidth, imageHeight] = m_imageDims;

        Vertex edgeSource = boost::source(edge, graph);
        Vertex edgeTarget = boost::target(edge, graph);

        // Bail if we're working with a vertical or horizontal edge
        if (utility::GetSlopeClassification(edgeSource, edgeTarget, imageWidth) ==
            utility::SlopeClassification::eInvalid)
        {
            return;
        }

        const auto [xSource, xTarget] = utility::GetCrossingEdge(edgeSource, edgeTarget, imageWidth);
        if (auto [crossingEdge, found] = boost::edge(xSource, xTarget, graph); found)
        {
            auto extents = getSearchExtents(edge, crossingEdge, graph);

            long long lengthA = getComponentSize(edgeSource, graph, extents);
            long long lengthB = getComponentSize(boost::source(crossingEdge, graph), graph, extents);
        
            // I'm cutting edge weights in half due to the graph being
            // undirected. There's two edges for each connection between
            // pixels, and so the weights are doubled
            if (lengthA < lengthB)
            {
                HeuristicHelper::instance().insertMarkedEdge(m_uuid,
                    { edgeSource, edgeTarget }, (lengthB - lengthA) / 2.0);
            }
            else if (lengthB < lengthA)
            {
                HeuristicHelper::instance().insertMarkedEdge(m_uuid,
                    { xSource , xTarget }, (lengthA - lengthB) / 2.0);
            }
        }
    }

    const HeuristicHelper::EdgeMap& getMarkedEdges() const noexcept override;
    void clearMarkedEdges() const noexcept override;

private:

    using Extents = std::tuple<long long, long long, long long, long long>;

    template<typename Vertex>
    bool withinExtents(Vertex vertex, const Extents& extents) const noexcept;

    template<typename Edge, typename Graph>
    Extents getSearchExtents(const Edge& first, const Edge& second, const Graph& graph) const noexcept;

    template<typename Vertex, typename Graph>
    long long getComponentSize(Vertex vertex, const Graph& graph, const Extents& extents) const noexcept;

private:

    boost::uuids::uuid m_uuid;
    utility::Point2D<int> m_imageDims{};

};

template<typename Vertex>
bool SparsePixels::withinExtents(Vertex vertex, const Extents& extents) const noexcept
{
    const auto [x, y] = utility::ExpandIndex<long long>(vertex, std::get<0>(m_imageDims));
    const auto [left, top, right, bottom] = extents;

    return (left <= x && right >= x) && (bottom >= y && top <= y);
}

template<typename Edge, typename Graph>
SparsePixels::Extents SparsePixels::getSearchExtents(const Edge& first,
    const Edge& second, const Graph& graph) const noexcept
{
    auto s1 = boost::source(first, graph);
    auto t1 = boost::target(first, graph);
    auto s2 = boost::source(second, graph);
    auto t2 = boost::target(second, graph);

    const auto [x1, y1] = utility::ExpandIndex<long long>(s1, std::get<0>(m_imageDims));
    const auto [x2, y2] = utility::ExpandIndex<long long>(s2, std::get<0>(m_imageDims));
    const auto [x3, y3] = utility::ExpandIndex<long long>(t1, std::get<0>(m_imageDims));
    const auto [x4, y4] = utility::ExpandIndex<long long>(t2, std::get<0>(m_imageDims));

    // clockwise winding order
    return
    {
        std::min({ x1, x2, x3, x4 }) - 3,
        std::min({ y1, y2, y3, y4 }) - 3,
        std::max({ x1, x2, x3, x4 }) + 3,
        std::max({ y1, y2, y3, y4 }) + 3,
    };
}

template<typename Vertex, typename Graph>
long long SparsePixels::getComponentSize(Vertex vertex, const Graph& graph,
    const Extents& extents) const noexcept
{
    // Build the color map to keep track of visited nodes
    std::vector<boost::default_color_type> colorMap{ boost::num_vertices(graph) };
    auto colorPropertyMap = boost::make_iterator_property_map(std::begin(colorMap),
        boost::get(boost::vertex_index, graph), colorMap[0]);
 
    long long edgeCount{ 0 };
    utility::EdgeCounter counter{ edgeCount };
    boost::depth_first_visit(graph, vertex, counter, colorPropertyMap,
        [&](Vertex vertex, const Graph& graph)
        {
            return !withinExtents(vertex, extents);
        });

    // If the dfs hits the terminator function on the
    // first vertex, then it doesn't process any edges.
    // There is always at least 1 edge in the curve
    return edgeCount;
}
}
