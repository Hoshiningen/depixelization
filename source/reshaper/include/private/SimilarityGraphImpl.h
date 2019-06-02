#pragma once

#include <GraphVisualizer.h>
#include <GraphUtils.h>
#include <Heuristics.h>
#include <Image.h>
#include <Pixel.h>

/*
    Disable warnings thrown in boost
    4100 - Unreferenced formal parameter
*/
#pragma warning( push )
#pragma warning( disable: 4996 4127 4100 )

#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/filtered_graph.hpp>

#pragma warning( pop )

#include <functional>
#include <type_traits>

namespace di = dpa::image;
namespace dpa::graph::internal
{
/*
    A vertex property that allows us to store color data
    on each node in the similarity graph
*/
struct VertexProperty
{
    stbi_uc Y{ 0 };
    stbi_uc Cb{ 0 };
    stbi_uc Cr{ 0 };
};

/*
    An edge property that supports marking edges as removed
    by specific heuristics. This enables creating filtered graphs
    based on each heuristic
*/
struct EdgeProperty
{
    bool dissimilar{ false };

    double curvesWeight{ 0 };
    double islandsWeight{ 0 };
    double sparsePixelsWeight{ 0 };
};

/*
    The implementation of the similarity graph
*/
class SimilarityGraphImpl final
{
public:

    using Graph = boost::adjacency_matrix<boost::undirectedS, VertexProperty, EdgeProperty>;
    using Edge = Graph::edge_descriptor;
    using Vertex = Graph::vertex_descriptor;
  
    using EdgeWeightFunc = std::function<double(const Edge&)>;
    using EdgeFilter = std::function<bool(const Edge&)>;
    using OptionalEdgeFilter = std::function<std::optional<bool>(const Edge&)>;
    using OptionalCrossingEdgeFilter = std::function<std::optional<bool>(const Edge&, const Edge&)>;

public:

    /*
        Builds the similarity graph from the given image

        @param image The image in YCbCr color space to build the graph from
        @returns True if the graph was built, false otherwise
    */
    bool build(const di::Image<di::RGB, stbi_uc>& image);

    /*
        Applies the given heuristic to the similarity graph,
        which modifies the edges in the graph

        @param visitor The heuristic to apply to the graph
        @param edgeFilter The filter to apply to the graph prior to running the heuristic
    */
    template<typename Visitor,
        typename = std::enable_if_t<std::is_base_of_v<boost::dfs_visitor<>, Visitor>>>
    void applyHeuristic(const Visitor& visitor,
        heuristics::FilteredEdges edgeFilter = heuristics::FilteredEdges::eNone) const noexcept
    {
        if (edgeFilter == heuristics::FilteredEdges::eNone)
        {
            boost::depth_first_search(m_graph, boost::visitor(visitor));
            return;
        }
   
        EdgeFilter filter = CreateEdgeFilter(edgeFilter);
        auto filteredGraph = boost::filtered_graph(m_graph, filter);

        boost::depth_first_search(filteredGraph, boost::visitor(visitor));
    }

    /*
        Prints a non-graphical representation of the graph

        @param stream The stream to write to
    */
    void printGraph(std::ostream& stream) const noexcept;

    /*
        Connects each of the nodes together along each row

        @param dims The images dimensions
    */
    void connectHorizontally(const utility::Point2D<Vertex>& dims);

    /*
        Connects each of the nodes together along each column

        @param dims The images dimensions
    */
    void connectVertically(const utility::Point2D<Vertex>& dims);

    /*
        Connects each of the nodes together along each backward diagonal

        @param dims The images dimensions
    */
    void connectBackwardDiagonally(const utility::Point2D<Vertex>& dims);

    /*
        Connects each of the nodes together along each forward diagonal

        @param dims The images dimensions
    */
    void connectForwardDiagonally(const utility::Point2D<Vertex>& dims);

    /*
        Sets the graph's vertex properties corresponding pixel data

        @param image An image in YCbCr color space containing pixel data
        @returns True if the node properties were set, false otherwise
    */
    bool setNodeProperties(const di::Image<di::YCbCr, stbi_uc>& image);

    /*
        Writes a .tex file to the given ostream. This can be compiled into
        a pdf using pdflatex

        @param output The output stream to write the .tex file too
        @returns True if the write was successful, false otherwise
    */
    bool writeTex(std::ostream& output, heuristics::FilteredEdges flags);

    /*
        Sets the edge properties based on the results of the applied
        heuristic

        @param heuristic The heuristic that was run
        @param callback A callback function that sets the appropriate
                        edge property in the graph
    */
    template<typename Visitor, typename Callback>
    void setEdgeProperties(const Visitor& heuristic, Callback callback)
    {
        for (const auto [markedEdge, value] : heuristic.getMarkedEdges())
        {
            const auto [start, end] = markedEdge;
            const auto [edge, found] = boost::edge(start, end, m_graph);

            if (!found)
                throw std::runtime_error("Couldn't map the marked edge with what's in the graph");

            callback(edge, value);
        }
    }

    /*
        Creates and edge filtering function based on the specified filters.
        In order for the specified edges to be removed, the respective heuristic
        has to be run first. I.e., if you want to filter out edges flagged by the
        dissimilar pixels heuristic, you have to apply that heuristic first

        @param filteredEdges A flag containing edge types to filter from the graph
        @returns An edge filtering function that removes the edges hidden by the
                 specified filters
    */
    EdgeFilter CreateEdgeFilter(heuristics::FilteredEdges filteredEdges) const noexcept;
    
    /*
        Creates a function that takes a set of crossing edges, and evaluates whether or not
        the edge in the first parameter slot should be filtered or not. Invalid input
        causes the returned function to return an std::nullopt

        @param filteredEdges A flag containing edge types to filter from the graph
        @returns A function that evaluates crossing edges
    */
    OptionalCrossingEdgeFilter CreateCrossingEdgeFilter(heuristics::FilteredEdges filteredEdges) const noexcept;
    
    /*
        Creates a function that can filter any type of crossing edge filter

        @param filteredEdges A flag containing edge types to filter from the graph
        @returns A function that can process any kind of crossing edge filter
    */
    OptionalEdgeFilter CreateEdgeWeightFilter(heuristics::FilteredEdges filteredEdges, EdgeFilter dissimilar) const noexcept;
    
    /*
        Creates a function that accumulates the edge weights of the specified filters

        @param filteredEdges A flag containing edge types to filter from the graph
        @returns A function that accumulates the specified edges weights
    */
    EdgeWeightFunc CreateEdgeWeightAccumulator(heuristics::FilteredEdges filteredEdges) const noexcept;

public:

    Graph m_graph{ 0 };
    dpa::image::internal::Point2D m_imageDims;

};
}
