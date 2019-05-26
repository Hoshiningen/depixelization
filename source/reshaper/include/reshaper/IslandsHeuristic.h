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
class Islands : public boost::default_dfs_visitor, IMarkedEdgeHelper
{
public:

    /*
        Default constructor. Creates the uuid for this instance
    */
    Islands();

    /*
        Parameterized ctor, which pulls in the image dimensions
        for point expansion during the dfs
    */
    explicit Islands(const utility::Point2D<int>& imageDims);

    /*
        Copy constructor, which sets this uuid to other's uuid

        @param other Another islands heuristic to create this object from
    */
    Islands(const Islands& other);

    /*
        Copy assignment. This sets this uuid to the other's uuid

        @param other Another islands heuristic to create this object from
        @returns A reference to this instance
    */
    Islands& operator=(const Islands& other);

    /*
        Evaluates crossing edges by determining if cutting one of the
        edges would produce an island, which is just a single node (or pixel).
        It votes to keep that edge between the two with a fixed weight

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

        auto hasValance1Node = [&](Vertex source, Vertex target)
        {
            return boost::out_degree(source, graph) == 1 || boost::out_degree(target, graph) == 1;
        };

        const auto [xSource, xTarget] = utility::GetCrossingEdge(edgeSource, edgeTarget, imageWidth);
        if (auto [crossingEdge, found] = boost::edge(xSource, xTarget, graph); found)
        {
            bool edgeHasIsland = hasValance1Node(edgeSource, edgeTarget);
            bool crossingHasIsland = hasValance1Node(xSource, xTarget);

            if (edgeHasIsland && !crossingHasIsland)
                HeuristicHelper::instance().insertMarkedEdge(m_uuid, { edgeSource, edgeTarget }, 2.5);
            else if (!edgeHasIsland && crossingHasIsland)
                HeuristicHelper::instance().insertMarkedEdge(m_uuid, { xSource, xTarget }, 2.5);
        }
    }

    const HeuristicHelper::EdgeMap& getMarkedEdges() const noexcept override;
    void clearMarkedEdges() const noexcept override;

private:

    boost::uuids::uuid m_uuid;
    utility::Point2D<int> m_imageDims{};

};
}
