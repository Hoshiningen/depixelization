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
/*
    Represents the curves heuristic, which resolves crossing
    edges between two curves. The curve with the longest length
    remains, and in the case of a tie, both edges are removed
*/
class Curves : public boost::default_dfs_visitor, IMarkedEdgeHelper
{
public:

    /*
        Default constructor. Creates the uuid for this instance
    */
    Curves();

    /*
        Parameterized ctor, which pulls in the image dimensions
        for point expansion during the dfs
    */
    explicit Curves(const utility::Point2D<int>& imageDims);

    /*
        Copy constructor, which sets this uuid to other's uuid

        @param other Another DissimilarPixels to create this object from
    */
    Curves(const Curves& other);

    /*
        Copy assignment. This sets this uuid to the other's uuid

        @param other Another DissimilarPixels to create this object from
        @returns A reference to this instance
    */
    Curves& operator=(const Curves& other);

    /*
        Examines each edge, looking for crossing edges. If
        a crossing edge is found, the lengths of each curve
        is computed, and the longest curve is awarded the
        difference in lengths as an additional edge weight

        @param edge An edge to examine
        @param graph The graph containing the edges we're looking at
    */
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
            long long lengthA = getCurveLength(edgeSource, graph);
            long long lengthB = getCurveLength(boost::source(crossingEdge, graph), graph);

            // I'm cutting edge weights in half due to the graph being
            // undirected. There's two edges for each connection between
            // pixels, and so the weights are doubled
            if (lengthA > lengthB)
            {
                HeuristicHelper::instance().insertMarkedEdge(m_uuid,
                    { edgeSource, edgeTarget }, (lengthA - lengthB) / 2.0);
            }
            else if (lengthB > lengthA)
            {
                HeuristicHelper::instance().insertMarkedEdge(m_uuid,
                    { xSource , xTarget }, (lengthB - lengthA) / 2.0);
            }
        }
    }

    const HeuristicHelper::EdgeMap& getMarkedEdges() const noexcept override;
    void clearMarkedEdges() const noexcept override;

private:

    /*
        Gets the length of the curve, starting at the given vertex

        @returns The length of the curve
    */
    template<typename Vertex, typename Graph>
    long long getCurveLength(Vertex vertex, Graph graph) const noexcept;

private:

    boost::uuids::uuid m_uuid;
    utility::Point2D<int> m_imageDims{};

};

template<typename Vertex, typename Graph>
long long Curves::getCurveLength(Vertex vertex, Graph graph) const noexcept
{
    // Build the color map to keep track of visited nodes
    std::vector<boost::default_color_type> colorMap{ boost::num_vertices(graph) };
    auto colorPropertyMap = boost::make_iterator_property_map(std::begin(colorMap),
        boost::get(boost::vertex_index, graph), colorMap[0]);

    // Launch the depth first visit with out edge counter

    long long edgeCount{ 0 };
    utility::EdgeCounter counter{ edgeCount };
    boost::depth_first_visit(graph, vertex, counter, colorPropertyMap,
        [](Vertex vertex, const Graph& graph)
        {
            return boost::out_degree(vertex, graph) != 2;
        });

    // If the dfs hits the terminator function on the
    // first vertex, then it doesn't process any edges.
    // There is always at least 1 edge in the curve
    return !edgeCount ? static_cast<long long>(1) : edgeCount;
}
}
