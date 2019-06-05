#pragma once

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

#include <execution>
#include <optional>
#include <set>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <type_traits>

namespace dpa::voronoi::internal
{
using BlockEdge = std::tuple<std::size_t, std::size_t>;

/*
    Represents the edge configuration of a 2x2 block
    of pixels in a similarity graph
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

struct PixelBlockLeft {};
struct PixelBlockRight {};
struct PixelBlockTop {};
struct PixelBlockBottom {};
struct PixelBlockForwardDiagonal {};
struct PixelBlockBackDiagonal {};

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
        return method(block.left.value(), visitor, PixelBlockLeft{});

    if (block.right)
        return method(block.right.value(), visitor, PixelBlockRight{});
    
    if (block.top)
        return method(block.top.value(), visitor, PixelBlockTop{});
    
    if (block.bottom)
        return method(block.bottom.value(), visitor, PixelBlockBottom{});
    
    if (block.forwardDiagonal)
        return method(block.forwardDiagonal.value(), visitor, PixelBlockForwardDiagonal{});
    
    if (block.backDiagonal)
        return method(block.backDiagonal.value(), visitor, PixelBlockBackDiagonal{});

    return std::invoke_result_t<Method, BlockEdge, Visitor, PixelBlockLeft>{};
}

template<typename T>
using Point2D = boost::geometry::model::point<T, 2, boost::geometry::cs::cartesian>;

/*
    The implementation of a voronoi diagram
*/
class VoronoiImpl
{
public:

    using BlockGrid = std::vector<std::vector<PixelBlock>>;

    struct VertexProperty
    {
        double x;
        double y;
    };

    struct GraphProperty
    {
        double x;
        double y;
    };

    using Graph = boost::adjacency_matrix<boost::undirectedS, VertexProperty, boost::no_property, GraphProperty>;
    
    using Edge = Graph::edge_descriptor;
    using Vertex = Graph::vertex_descriptor;
    
    using WeldMap = std::multimap<std::tuple<double, double>, Vertex>;
    using VoronoiConfig = std::tuple<Graph, WeldMap>;

public:

    void build(const std::set<BlockEdge>& edges) noexcept;
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

    template<typename ExecutionPolicy>
    auto dispatchGridBuilder(ExecutionPolicy policy, const std::set<BlockEdge>& edges)
        -> std::enable_if_t<std::is_execution_policy_v<ExecutionPolicy>, BlockGrid>
    {
        return buildBlockGrid(edges, policy);
    }

    BlockGrid buildBlockGrid(const std::set<BlockEdge>& edges, std::execution::sequenced_policy) const;
    BlockGrid buildBlockGrid(const std::set<BlockEdge>& edges, std::execution::parallel_policy) const;

    PixelBlock buildBlock(const std::tuple<std::size_t, std::size_t, std::size_t, std::size_t>& vertices,
                          const std::set<BlockEdge>& edges) const noexcept;

    template<typename ExecutionPolicy>
    auto dispatchVoronoiBuilder(ExecutionPolicy policy, const BlockGrid& blocks)
        -> std::enable_if_t<std::is_execution_policy_v<ExecutionPolicy>, Graph>
    {
        return buildVoronoiGraph(blocks, policy);
    }

    Graph buildVoronoiGraph(const BlockGrid& blocks, std::execution::sequenced_policy) const;
    Graph buildVoronoiGraph(const BlockGrid& blocks, std::execution::parallel_policy) const;

    VoronoiConfig getVoronoiCellConfiguration(const PixelBlock& block) const;

    struct DefaultTag {};
    struct DiagonalTag {};
    struct TriangleTag {};

    VoronoiConfig getConfiguration(const PixelBlock& block, DefaultTag) const noexcept;
    VoronoiConfig getConfiguration(const PixelBlock& block, DiagonalTag) const noexcept;
    VoronoiConfig getConfiguration(const PixelBlock& block, TriangleTag) const noexcept;

    struct TransformParameters
    {
        double theta;
        double deltaX;
        double deltaY;
    };

    Point2D<double> transformPoint(const Point2D<double>& point, TransformParameters parameters) const noexcept;

    std::optional<TransformParameters> getTransformParameters(const dpa::voronoi::internal::PixelBlock& block, DefaultTag) const noexcept;
    std::optional<TransformParameters> getTransformParameters(const dpa::voronoi::internal::PixelBlock& block, DiagonalTag) const noexcept;
    std::optional<TransformParameters> getTransformParameters(const dpa::voronoi::internal::PixelBlock& block, TriangleTag) const noexcept;

    Graph WeldAsync(const std::vector<std::vector<VoronoiConfig>>& disjointVoronoi) const noexcept;
    VoronoiConfig WeldGraphs(const VoronoiConfig& init, const VoronoiConfig& rhs) const noexcept;
    void WeldVertices(Graph& lhs, std::size_t vertexOffset, const WeldMap& weldPoints, const WeldMap& rhsWelds) const noexcept;

public:

    int m_height{ 0 };
    int m_width{ 0 };

    BlockGrid m_blockGrid{};
    Graph m_voronoiGraph{ 0 };
};
}