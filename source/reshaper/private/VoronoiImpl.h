#pragma once

#include <execution>
#include <optional>
#include <set>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <type_traits>

/*
    Disable warnings thrown in boost
    4100 - Unreferenced formal parameter
*/
#pragma warning( push )
#pragma warning( disable: 4996 4127 4100 )

#include <boost/geometry.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>

#pragma warning( pop )

namespace dpa::voronoi::internal
{
using BlockEdge = std::tuple<std::size_t, std::size_t>;

/*
    Represents the edge configuration of a 2x2 block
    of pixels in a similarity graph

    o---o
    | X |
    o---o
*/
struct PixelBlock
{
    std::optional<BlockEdge> left;
    std::optional<BlockEdge> right;
    std::optional<BlockEdge> top;
    std::optional<BlockEdge> bottom;
    std::optional<BlockEdge> forwardDiagonal;
    std::optional<BlockEdge> backDiagonal;

    std::string serialize() const noexcept;
};

struct PixelBlockLeftTag {};
struct PixelBlockRightTag {};
struct PixelBlockTopTag {};
struct PixelBlockBottomTag {};
struct PixelBlockForwardDiagonalTag {};
struct PixelBlockBackDiagonalTag {};

template<typename... Handlers>
struct PixelBlockDispatcher : Handlers...
{
    using Handlers::operator()...;
};

template<typename... Handlers>
PixelBlockDispatcher(Handlers...)->PixelBlockDispatcher<Handlers...>;

template<typename Method, typename Visitor>
auto VisitPixelBlockEdges(const PixelBlock& block, Method method, Visitor visitor)
{
    if (block.left)
        return method(block.left.value(), visitor, PixelBlockLeftTag{});

    if (block.right)
        return method(block.right.value(), visitor, PixelBlockRightTag{});
    
    if (block.top)
        return method(block.top.value(), visitor, PixelBlockTopTag{});
    
    if (block.bottom)
        return method(block.bottom.value(), visitor, PixelBlockBottomTag{});
    
    if (block.forwardDiagonal)
        return method(block.forwardDiagonal.value(), visitor, PixelBlockForwardDiagonalTag{});
    
    if (block.backDiagonal)
        return method(block.backDiagonal.value(), visitor, PixelBlockBackDiagonalTag{});

    return std::invoke_result_t<Method, BlockEdge, Visitor, PixelBlockLeftTag>{};
}

template<typename T>
using Point2D = boost::geometry::model::point<T, 2, boost::geometry::cs::cartesian>;

/*
    The implementation of a voronoi diagram
*/
class VoronoiImpl
{
public:

    /*
        This represents a 2D grid of PixelBlocks, which are explicit edge configurations
        of a 2x2 chunk of a similarity graph
    */
    using BlockGrid = std::vector<std::vector<PixelBlock>>;

    /*
        The property stored for each vertex in the voronoi graph. Contains
        the x and y position of the node in screen-space
    */
    struct VertexProperty
    {
        double x;
        double y;
    };

    /*
        The property stored for the entire voronoi graph. This is the reference point
        used to offset all nodes in the graph
    */
    struct GraphProperty
    {
        double x;
        double y;
    };

    using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, VertexProperty, boost::no_property, GraphProperty>;
    
    using Edge = Graph::edge_descriptor;
    using Vertex = Graph::vertex_descriptor;
    
    using WeldMap = std::multimap<std::tuple<double, double>, Vertex>;
    using VoronoiConfig = std::tuple<Graph, WeldMap>;

public:

    /*
        Builds the voronoi diagram with the given edges from a fully
        resolved similarity graph

        @param edges    The remaining edges in a similarity graph, after all
                        of the heuristics have been applied
    */
    void build(const std::set<BlockEdge>& edges) noexcept;

    /*
        Sets the dimensions of the voronoi diagram

        @param graphDims    The width and height of this graph
    */
    void setDimensions(const std::tuple<int, int>& graphDims) noexcept;

    /*
        Writes a .tex file to the given ostream. This can be compiled into
        a pdf using pdflatex

        @param output The output stream to write the .tex file too
        @returns True if the write was successful, false otherwise
    */
    bool writeTex(std::ostream& output);

    /*
        Prints a non-graphical representation of the graph

        @param stream The stream to write to
    */
    void printGraph(std::ostream& stream);

    /*
        Prints a non-graphical representation of the vertices,
        with their stored properties

        @param stream The stream to write to
    */
    void printVertices(std::ostream& stream);

private:

    /*
        Dispatcher for building the block grid with the specified execution policy

        @tparam ExecutionPolicy The type of the policy to build the block grid with

        @param policy   The policy to build the block grid with
        @param edges    The list of edges from a resolved similarity graph

        @returns The fully build block grid
    */
    template<typename ExecutionPolicy>
    auto dispatchGridBuilder(ExecutionPolicy policy, const std::set<BlockEdge>& edges)
        -> std::enable_if_t<std::is_execution_policy_v<ExecutionPolicy>, BlockGrid>
    {
        return buildBlockGrid(edges, policy);
    }

    /*
        Sequential method for building the block grid

        @param edges    The list of edges from a resolved similarity graph

        @returns The fully built block grid
    */
    BlockGrid buildBlockGrid(const std::set<BlockEdge>& edges, std::execution::sequenced_policy) const;

    /*
        Builds an individual block in the pixel block grid

        @param vertices A set of 1D indices for the top, bottom, left, and right edges of the block
        @param edges    A set of all the edges in the similarity graph
    */
    PixelBlock buildBlock(const std::tuple<std::size_t, std::size_t, std::size_t, std::size_t>& vertices,
                          const std::set<BlockEdge>& edges) const noexcept;

    /*
        Dispatcher for building the voronoi diagram with the specified execution policy

        @tparam ExecutionPolicy The type of the policy to build the voronoi diagram with

        @param policy   The policy to build the voronoi diagram with
        @param blocks   The 2D grid of pixel blocks

        @returns The fully built voronoi diagram
    */
    template<typename ExecutionPolicy>
    auto dispatchVoronoiBuilder(ExecutionPolicy policy, const BlockGrid& blocks)
        -> std::enable_if_t<std::is_execution_policy_v<ExecutionPolicy>, Graph>
    {
        return buildVoronoiGraph(blocks, policy);
    }

    /*
        Sequential method for building the voronoi diagram

        @param blocks   The 2D grid of pixel blocks

        @returns The fully built voronoi diagram
    */
    Graph buildVoronoiGraph(const BlockGrid& blocks, std::execution::sequenced_policy) const;

    /*
        Matches the pixel block with the pre-built voronoi cell configuration and returns
        that configuration to the user

        @param block    The pixel block to get the voronoi configuration for

        @returns A voronoi configuration for the given pixel block
    */
    VoronoiConfig getVoronoiCellConfiguration(const PixelBlock& block) const;

    struct DefaultTag {};
    struct DiagonalTag {};
    struct TriangleTag {};

    /*
        Gets the default voronoi configuration. This is the case for any pixel block
        that's not a diagonal (either forward or backwards), or a triangle

        @param block    The pixel block to get a voronoi configuration for

        @returns A voronoi configuration for the given pixel block
    */
    VoronoiConfig getConfiguration(const PixelBlock& block, DefaultTag) const noexcept;

    /*
        Gets the voronoi configuration for a pixel block that contains a diagonal

        @param block    The pixel block to get a voronoi configuration for

        @returns A voronoi configuration for the given pixel block
    */
    VoronoiConfig getConfiguration(const PixelBlock& block, DiagonalTag) const noexcept;

    /*
        Gets the voronoi configuration for a pixel block that contains a triangle

        @param block    The pixel block to get a voronoi configuration for

        @returns A voronoi configuration for the given pixel block
    */
    VoronoiConfig getConfiguration(const PixelBlock& block, TriangleTag) const noexcept;

    /*
        Plain old data struct for the transformation parameters. This contains the information
        used to rotate and translate the voronoi configurations into place after they've been created
    */
    struct TransformParameters
    {
        double theta;
        double deltaX;
        double deltaY;
    };

    /*
        Transforms the given point with the given transform parameters

        @param point        The point to transform
        @param parameters   The parameters to transform the point by

        @returns The transformed point
    */
    Point2D<double> transformPoint(const Point2D<double>& point, TransformParameters parameters) const noexcept;

    /*
        Gets the default transformation parameters for a pixel block.

        @param block    The pixel block to get the transform parameters for

        @returns A valid optional if a valid edge was present, an empty one otherwise
    */
    std::optional<TransformParameters> getTransformParameters(const dpa::voronoi::internal::PixelBlock& block, DefaultTag) const noexcept;
    
    /*
        Gets the transformation parameters for a pixel block that contains a forward or backwards diagonal.

        @param block    The pixel block to get the transform parameters for

        @returns A valid optional if a valid edge was present, an empty one otherwise
    */
    std::optional<TransformParameters> getTransformParameters(const dpa::voronoi::internal::PixelBlock& block, DiagonalTag) const noexcept;
    
    /*
        Gets the transformation parameters for a pixel block that contains a triangle.

        @param block    The pixel block to get the transform parameters for

        @returns A valid optional if a valid edge was present, an empty one otherwise
    */
    std::optional<TransformParameters> getTransformParameters(const dpa::voronoi::internal::PixelBlock& block, TriangleTag) const noexcept;

    /*
        Welds the list of disjoint voronoi configurations into a single graph, asynchronously

        @param disjointVoronoi  A list of disjoint voronoi configurations, which are the distinct
                                pieces of the voronoi diagram that have been matched with pixel blocks

        @returns A graph that contains all the nodes and edges in the voronoi diagram
    */
    Graph WeldAsync(const std::vector<std::vector<VoronoiConfig>>& disjointVoronoi) const noexcept;
    
    /*
        This welds two voronoi diagrams together, which produces a single voronoi diagram

        @param init The existing voronoi diagram that contains the accumulated graph welds
        @param rhs  The right hand side, or the new voronoi diagram that's being welded

        @returns A combined voronoi diagram that's the product of welding init and rhs together
    */
    VoronoiConfig WeldGraphs(const VoronoiConfig& init, const VoronoiConfig& rhs) const noexcept;
    
    /*
        Welds all the vertices together between two graphs, and spits the result out to lhs. The weld process
        removes overlapping vertices and inserts an edge between the nodes attached to the overlapping vertices

        @param dest         The graph to write the results into
        @param vertexOffset The amount to offset each of the nodes by when writing to dest
        @param lhsWelds     The weld information for the accumulated result of previous welds
        @param rhsWelds     The new set of weld data, which is being welded with lhsWelds
    */
    void WeldVertices(Graph& dest, std::size_t vertexOffset, const WeldMap& lhsWelds, const WeldMap& rhsWelds) const noexcept;

public:

    int m_height{ 0 };
    int m_width{ 0 };

    BlockGrid m_blockGrid{};
    Graph m_voronoiGraph{ 0 };
};
}