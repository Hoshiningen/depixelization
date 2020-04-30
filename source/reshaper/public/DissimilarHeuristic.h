#pragma once

#include <HeuristicHelper.h>

#include <variant>

#include <boost/graph/depth_first_search.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace dpa::graph::heuristics
{
/*
    A heuristic that determines if any two pixels are dissimilar
*/
class DissimilarPixels : public boost::default_dfs_visitor, IMarkedEdgeHelper
{
public:

    /*
        Default constructor. Creates the uuid for this instance
    */
    DissimilarPixels();

    /*
        Copy constructor, which sets this uuid to other's uuid

        @param other Another DissimilarPixels to create this object from
    */
    DissimilarPixels(const DissimilarPixels& other);

    /*
        Copy assignment. This sets this uuid to the other's uuid

        @param other Another DissimilarPixels to create this object from
        @returns A reference to this instance
    */
    DissimilarPixels& operator=(const DissimilarPixels& other);

    /*
        DFS hook overload for the examine edge event. This
        compares the pixels attached to each edge and determines
        if they're dissimilar. If they are, it marks them as so
        by inserting the offending edge into an edge map associated
        with this instance

        @param edge An edge in the graph
        @param graph The graph being searched
    */
    template <class Edge, class Graph>
    void examine_edge(Edge edge, const Graph& graph) {

        auto start = boost::source(edge, graph);
        auto end = boost::target(edge, graph);

        float deltaY = static_cast<float>(graph[start].Y - graph[end].Y);
        float deltaCb = static_cast<float>(graph[start].Cb - graph[end].Cb);
        float deltaCr = static_cast<float>(graph[start].Cr - graph[end].Cr);

        if ((deltaY >= (48.f / 255.f)) ||
            (deltaCb >= (7.f / 255.f)) ||
            (deltaCr >= (6.f / 255.f)))
        {
            HeuristicHelper::instance().insertMarkedEdge(m_uuid, { start, end }, true);
        }
    }

    const HeuristicHelper::EdgeMap& getMarkedEdges() const noexcept override;
    void clearMarkedEdges() const noexcept override;

private:

    boost::uuids::uuid m_uuid;

};
}
