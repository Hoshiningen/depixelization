#include <VoronoiImpl.h>

#include <GraphUtils.h>
#include <GraphVisualizer.h>
#include <VoronoiGraphVisualizationStrategy.h>

#pragma warning(push)
#pragma warning(disable: 4996)

#include <boost/geometry/strategies/transform/matrix_transformers.hpp>
#include <boost/geometry/algorithms/transform.hpp>

#include <boost/graph/graph_utility.hpp>
#include <boost/graph/copy.hpp>

#include <boost/property_map/property_map.hpp>

#pragma warning(pop)

#include <algorithm>
#include <array>
#include <map>
#include <iterator>
#include <limits>
#include <set>
#include <sstream>
#include <tuple>

namespace
{
/*
    A DFS visitor that copies the graph it visits
*/
template<typename Graph>
class GraphCopier : public boost::default_dfs_visitor
{
public:

    /*
        Parameterized constructor.

        @param dest         The graph to write into while visiting each
                            node in the visited graph
        @param vertexOffset The amount to offset each vertex in the graph
                            when copying them into the output graph
    */
    explicit GraphCopier(Graph& dest, std::size_t vertexOffset)
        : m_graph(dest), m_offset(vertexOffset)
    {}

    /*
        Copy constructor.
    */
    GraphCopier(const GraphCopier& other)
        : m_graph(other.m_graph), m_offset(other.m_offset)
    {}

    /*
        The hook that's called when a vertex is first encountered
        in the visited graph

        @param vertex   The vertex that's being discovered
        @param graph    The graph the vertex belongs too
    */
    template<typename Vertex>
    void discover_vertex(Vertex vertex, const Graph& graph)
    {
        m_graph[vertex + m_offset].x = graph[vertex].x;
        m_graph[vertex + m_offset].y = graph[vertex].y;
    }

    /*
        A hook that's called when an edge is examined. This occurs
        whenever the edge is touched, and so this method could be called
        many times for the same edge

        @param edge     The edge to examine
        @param graph    The graph the edge belongs to
    */
    template<typename Edge>
    void examine_edge(Edge edge, const Graph& graph)
    {
        const auto start = boost::source(edge, graph);
        const auto end = boost::target(edge, graph);

        if (m_insertedEdges.find({ start, end }) == std::end(m_insertedEdges))
            boost::add_edge(start + m_offset, end + m_offset, m_graph);

        if (m_insertedEdges.find({ end, start }) == std::end(m_insertedEdges))
            boost::add_edge(end + m_offset, start + m_offset, m_graph);

        m_insertedEdges.insert({ start, end });
        m_insertedEdges.insert({ end, start });
    }

private:

    typedef typename Graph::vertex_descriptor Vertex;
    std::set<std::tuple<Vertex, Vertex>> m_insertedEdges;

    std::size_t m_offset{ 0 };
    Graph& m_graph;

};

/*
    Wraps a container in an object that provides
    convenient syntax for searching

    @tparam Container   The type of container to search through
*/
template<typename Container>
struct Searchable
{
    /*
        Parameterized constructor

        @param container    The container to search through
    */
    explicit Searchable(const Container& container) noexcept
        : m_container(container)
    {}

    /*
        Checks if the container has the given value

        @param value The value to search for
        @returns True if the value is contained, false otherwise
    */
    template<typename Value>
    bool has(const Value& value) const noexcept;

private:

    const Container& m_container;
};

template<typename Container>
template<typename Value>
bool Searchable<Container>::has(const Value& target) const noexcept
{
    return std::any_of(std::cbegin(m_container), std::cend(m_container),
        [&](auto value)
        {
            return value == target;
        });
}

/*
    Converts the given point into a tuple

    @param point A point to convert
    @returns A tuple containing the point's values
*/
std::tuple<double, double> AsTuple(const dpa::voronoi::internal::Point2D<double>& point)
{
    return std::make_tuple(point.get<0>(), point.get<1>());
}

/*
    Gets the upper left corner of the minimum bounding box around the given edge

    @param edge The edge to get the reference point from
    @param rowSize The width of the graph

    @returns The reference point of the edge
*/
std::tuple<double, double> GetReferencePoint(const dpa::voronoi::internal::PixelBlock& block, std::size_t rowSize)
{
    namespace dvi = dpa::voronoi::internal;

    const auto UnpackEdge = [rowSize](dvi::BlockEdge edge, auto retFn, auto tag)
    {
        const auto [s, t] = edge;

        const auto [x1, y1] = dpa::graph::utility::ExpandIndex(s, rowSize);
        const auto [x2, y2] = dpa::graph::utility::ExpandIndex(t, rowSize);

        return retFn(x1, y1, x2, y2, tag);
    };

    return VisitPixelBlockEdges(block, UnpackEdge, dvi::PixelBlockDispatcher
        {
            [](std::size_t x1, std::size_t y1, std::size_t x2, std::size_t y2, dvi::PixelBlockLeftTag)
            {
                return std::make_tuple(static_cast<double>(std::min(x1, x2)), static_cast<double>(std::min(y1, y2)));
            },
            
            [](std::size_t x1, std::size_t y1, std::size_t x2, std::size_t y2, dvi::PixelBlockRightTag)
            {
                return std::make_tuple(static_cast<double>(std::min(x1, x2) - 1), static_cast<double>(std::min(y1, y2)));
            },

            [](std::size_t x1, std::size_t y1, std::size_t x2, std::size_t y2, dvi::PixelBlockTopTag)
            {
                return std::make_tuple(static_cast<double>(std::min(x1, x2)), static_cast<double>(std::min(y1, y2)));
            },

            [](std::size_t x1, std::size_t y1, std::size_t x2, std::size_t y2, dvi::PixelBlockBottomTag)
            {
                return std::make_tuple(static_cast<double>(std::min(x1, x2)), static_cast<double>(std::min(y1, y2)) - 1);
            },

            [](std::size_t x1, std::size_t y1, std::size_t x2, std::size_t y2, dvi::PixelBlockForwardDiagonalTag)
            {
                return std::make_tuple(static_cast<double>(std::min(x1, x2)), static_cast<double>(std::min(y1, y2)));
            },

            [](std::size_t x1, std::size_t y1, std::size_t x2, std::size_t y2, dvi::PixelBlockBackDiagonalTag)
            {
                return std::make_tuple(static_cast<double>(std::min(x1, x2)), static_cast<double>(std::min(y1, y2)));
            },
        });
}

constexpr auto k_doubleMin = std::numeric_limits<double>::min();
}

namespace dpa::voronoi::internal
{
std::string PixelBlock::serialize() const noexcept
{
    std::ostringstream edgeConfig;

    if (left)
        edgeConfig << "l";

    if (right)
        edgeConfig << "r";

    if (top)
        edgeConfig << "t";

    if (bottom)
        edgeConfig << "b";

    if (forwardDiagonal)
        edgeConfig << "[fD]";

    if (backDiagonal)
        edgeConfig << "[bD]";

    return edgeConfig.str();
}

void VoronoiImpl::build(const std::set<BlockEdge>& edges) noexcept
{
    // Handle the single row or column special case...
    if (m_height < 2 || m_width < 2)
    {
        return;
    }

    // Handle everything else
    m_blockGrid = dispatchGridBuilder(std::execution::seq, edges);
    m_voronoiGraph = dispatchVoronoiBuilder(std::execution::seq, m_blockGrid);
}

void VoronoiImpl::setDimensions(const std::tuple<int, int>& graphDims) noexcept
{
    std::tie(m_width, m_height) = graphDims;
}

bool VoronoiImpl::writeTex(std::ostream& output)
{
    auto strategy = VoronoiVisualizationStrategy<Graph>{};
    auto visualizer = dpa::graph::internal::LaTeXGraphVisualizer<Graph>{ strategy };

    return visualizer.writeTex(m_voronoiGraph, std::make_tuple(m_width, m_height), output);
}

void VoronoiImpl::printGraph(std::ostream& stream)
{
    boost::print_graph(m_voronoiGraph, boost::get(boost::vertex_index, m_voronoiGraph), stream);
}

void VoronoiImpl::printVertices(std::ostream& stream)
{
    const auto toString = [&](auto vertexDescriptor)
    {
        auto x = std::to_string(m_voronoiGraph[vertexDescriptor].x);
        auto y = std::to_string(m_voronoiGraph[vertexDescriptor].y);

        return "(" + x + ", " + y + ")";
    };

    for (auto iter : boost::make_iterator_range(boost::vertices(m_voronoiGraph)))
        stream << iter << ": " << toString(iter) << "\n";
}

VoronoiImpl::BlockGrid VoronoiImpl::buildBlockGrid(const std::set<BlockEdge>& edges, std::execution::sequenced_policy) const
{
    // O(nlog(n))
    using namespace dpa::graph::utility;

    BlockGrid blocks;
    for (std::size_t h = 0; h < m_height - 1ull; ++h)
    {
        std::vector<PixelBlock> row;
        for (std::size_t w = 0; w < m_width - 1ull; ++w)
        {
            std::size_t topLeft = FlattenPoint<std::size_t>({ w, h }, m_width);
            std::size_t topRight = FlattenPoint<std::size_t>({ w + 1, h }, m_width);
            std::size_t bottomLeft = FlattenPoint<std::size_t>({ w, h + 1 }, m_width);
            std::size_t bottomRight = FlattenPoint<std::size_t>({ w + 1, h + 1 }, m_width);

            PixelBlock block = buildBlock({ topLeft, topRight, bottomLeft, bottomRight }, edges);
            row.push_back(block);
        }

        blocks.push_back(row);
    }

    return blocks;
}

PixelBlock VoronoiImpl::buildBlock(const std::tuple<std::size_t, std::size_t, std::size_t, std::size_t>& vertices,
                                   const std::set<BlockEdge>& edges) const noexcept
{
    using namespace dpa::graph::utility;

    const auto [TL, TR, BL, BR] = vertices;
    const auto GetEdge = [&](auto s, auto t) -> std::optional<BlockEdge>
    {
        return edges.find({ s, t }) != std::end(edges) || edges.find({ t, s }) != std::end(edges) ?
            std::make_optional(BlockEdge{ s, t }) : std::nullopt;
    };

    return
    {
        GetEdge(TL, BL),
        GetEdge(TR, BR),
        GetEdge(TL, TR),
        GetEdge(BL, BR),
        GetEdge(BL, TR),
        GetEdge(TL, BR),
    };
}

VoronoiImpl::Graph VoronoiImpl::buildVoronoiGraph(const BlockGrid& blocks, std::execution::sequenced_policy) const
{
    WeldMap weldPoints;

    std::vector<std::vector<VoronoiConfig>> disjointVoronoi;
    for (auto h = 0; h < blocks.size(); ++h)
    {
        std::vector<VoronoiConfig> row;
        for (auto w = 0; w < blocks.front().size(); ++w)
        {
            auto [config, points] = getVoronoiCellConfiguration(blocks[h][w]);
            row.push_back({ config, points });
        }

        disjointVoronoi.push_back(row);
    }

    return WeldAsync(disjointVoronoi);
}

VoronoiImpl::VoronoiConfig VoronoiImpl::getVoronoiCellConfiguration(const PixelBlock& block) const
{
    std::string edgeConfig = block.serialize();
    static const auto pred = [&](auto config) { return config == edgeConfig; };
    
    static const std::array<std::string, 4> triangleConfigs =
    {
        "lb[bD]", "rb[fD]", "lt[fD]", "rt[bD]"
    };

    static const std::array<std::string, 2> diagonalConfigs =
    {
        "[fD]", "[bD]"
    };
 
    if (Searchable{ triangleConfigs }.has(edgeConfig))
        return getConfiguration(block, TriangleTag{});
    else if (Searchable{ diagonalConfigs }.has(edgeConfig))
        return getConfiguration(block, DiagonalTag{});
    
    return getConfiguration(block, DefaultTag{});
}

VoronoiImpl::VoronoiConfig VoronoiImpl::getConfiguration(const PixelBlock& block, DefaultTag) const noexcept
{
    /*
            *   
            |
        *---*---*
            |
            *
    */

    // Edge configuration is centered at 0, 0, then translated into position
    static const std::array<Point2D<double>, 5> pointCache =
    {
        Point2D<double>{k_doubleMin, -.5},
        Point2D<double>{-.5, k_doubleMin},
        Point2D<double>{k_doubleMin, k_doubleMin},
        Point2D<double>{.5, k_doubleMin},
        Point2D<double>{k_doubleMin, .5},
    };

    if (auto parameters = getTransformParameters(block, DefaultTag{}); parameters)
    {
        std::vector<Point2D<double>> ret;
        std::transform(std::cbegin(pointCache), std::cend(pointCache), std::back_inserter(ret),
            [=](const auto& point)
            {
                return transformPoint(point, parameters.value());
            });

        Graph graph{ 5 };

        boost::add_edge(0, 2, graph);
        boost::add_edge(1, 2, graph);
        boost::add_edge(2, 3, graph);
        boost::add_edge(2, 4, graph);

        for (auto i = 0; i < pointCache.size(); ++i)
            std::tie(graph[i].x, graph[i].y) = AsTuple(ret.at(i));

        WeldMap points;
        points.insert({ std::make_tuple(graph[0].x, graph[0].y), 0 });
        points.insert({ std::make_tuple(graph[1].x, graph[1].y), 1 });
        points.insert({ std::make_tuple(graph[3].x, graph[3].y), 3 });
        points.insert({ std::make_tuple(graph[4].x, graph[4].y), 4 });

        return { graph, points };
    }

    return { Graph{ 0 }, WeldMap{} };
}

VoronoiImpl::VoronoiConfig VoronoiImpl::getConfiguration(const PixelBlock& block, TriangleTag) const noexcept
{
    // Edge configuration is centered at 0, 0, then translated into position
    static const std::array<Point2D<double>, 6> pointCache =
    {
        Point2D<double>{k_doubleMin, -.5},
        Point2D<double>{.25, -.25},
        Point2D<double>{-.5, k_doubleMin},
        Point2D<double>{k_doubleMin, k_doubleMin},
        Point2D<double>{.5, k_doubleMin},
        Point2D<double>{k_doubleMin, .5}
    };

    if (auto parameters = getTransformParameters(block, TriangleTag{}); parameters)
    {
        std::vector<Point2D<double>> ret;
        std::transform(std::cbegin(pointCache), std::cend(pointCache), std::back_inserter(ret),
            [=](const auto& point)
            {
                return transformPoint(point, parameters.value());
            });

        Graph graph{ 6 };

        boost::add_edge(0, 1, graph);
        boost::add_edge(2, 3, graph);
        boost::add_edge(3, 1, graph);
        boost::add_edge(1, 4, graph);
        boost::add_edge(3, 5, graph);

        for (auto i = 0; i < pointCache.size(); ++i)
            std::tie(graph[i].x, graph[i].y) = AsTuple(ret.at(i));

        WeldMap points;
        points.insert({ std::make_tuple(graph[0].x, graph[0].y), 0 });
        points.insert({ std::make_tuple(graph[2].x, graph[2].y), 2 });
        points.insert({ std::make_tuple(graph[4].x, graph[4].y), 4 });
        points.insert({ std::make_tuple(graph[5].x, graph[5].y), 5 });

        return { graph, points };
    }

    return { Graph{ 0 }, WeldMap{} };
}

VoronoiImpl::VoronoiConfig VoronoiImpl::getConfiguration(const PixelBlock& block, DiagonalTag) const noexcept
{
    // Edge configuration is centered at 0, 0, then translated into position
    static const std::array<Point2D<double>, 7> pointCache =
    {
        Point2D<double>{k_doubleMin, -.5},
        Point2D<double>{-.25, -.25},
        Point2D<double>{-.5, k_doubleMin},
        Point2D<double>{k_doubleMin, k_doubleMin},
        Point2D<double>{.5, k_doubleMin},
        Point2D<double>{.25, .25},
        Point2D<double>{k_doubleMin, .5},
    };

    if (auto parameters = getTransformParameters(block, DiagonalTag{}); parameters)
    {
        std::vector<Point2D<double>> ret;
        std::transform(std::cbegin(pointCache), std::cend(pointCache), std::back_inserter(ret),
            [=](const auto& point)
            {
                return transformPoint(point, parameters.value());
            });

        Graph graph{ 7 };

        boost::add_edge(0, 1, graph);
        boost::add_edge(1, 2, graph);
        boost::add_edge(1, 3, graph);
        boost::add_edge(3, 5, graph);
        boost::add_edge(5, 4, graph);
        boost::add_edge(6, 5, graph);

        for(auto i = 0; i < pointCache.size(); ++i)
            std::tie(graph[i].x, graph[i].y) = AsTuple(ret.at(i));

        WeldMap points;
        points.insert({ std::make_tuple(graph[0].x, graph[0].y), 0 });
        points.insert({ std::make_tuple(graph[2].x, graph[2].y), 2 });
        points.insert({ std::make_tuple(graph[4].x, graph[4].y), 4 });
        points.insert({ std::make_tuple(graph[6].x, graph[6].y), 6 });

        return { graph, points };
    }

    return { Graph{ 0 }, WeldMap{} };
}

Point2D<double> VoronoiImpl::transformPoint(const Point2D<double>& point, TransformParameters parameters) const noexcept
{
    using namespace boost::geometry::strategy;

    Point2D<double> transformed;

    // Each point is contained within a 1x1 box that's rotated about the origin.
    // I need to translate the box back into the first quadrant, which is the
    // screen space the image is rendered in (top left is 0, 0)
    constexpr double offset = .5;

    transform::rotate_transformer<boost::geometry::degree, double, 2, 2> rotate(parameters.theta);
    transform::translate_transformer<double, 2, 2> translate(parameters.deltaX + offset, parameters.deltaY + offset);

    boost::geometry::transform(point, transformed, rotate);
    boost::geometry::transform(transformed, transformed, translate);

    auto round = [](auto value, auto radixPoint)
    {
        const auto radixOffset = std::pow(10, radixPoint);

        value *= radixOffset;
        value = std::round(value);
        value /= radixOffset;

        return value;
    };

    auto value1 = boost::geometry::get<0>(transformed);
    auto value2 = boost::geometry::get<1>(transformed);

    boost::geometry::set<0>(transformed, round(value1, 2));
    boost::geometry::set<1>(transformed, round(value2, 2));

    return transformed;
}

std::optional<VoronoiImpl::TransformParameters> VoronoiImpl::getTransformParameters(const dpa::voronoi::internal::PixelBlock& block, DefaultTag) const noexcept
{
    const auto [x, y] = GetReferencePoint(block, m_width);

    if (block.left)
        return TransformParameters{ 0, x, y };

    if (block.top)
        return TransformParameters{ 0, x, y };

    if (block.right)
        return TransformParameters{ 0, std::max(x, x - 1), y };

    if (block.bottom)
        return TransformParameters{ 0, x, std::max(y, y - 1) };

    return std::nullopt;
}

std::optional<VoronoiImpl::TransformParameters> VoronoiImpl::getTransformParameters(const dpa::voronoi::internal::PixelBlock& block, DiagonalTag) const noexcept
{
    const auto [x, y] = GetReferencePoint(block, m_width);

    if (block.forwardDiagonal)
        return TransformParameters{ 0, x, y };

    if (block.backDiagonal)
        return TransformParameters{ 90, x, y };

    return std::nullopt;
}

std::optional<VoronoiImpl::TransformParameters> VoronoiImpl::getTransformParameters(const dpa::voronoi::internal::PixelBlock& block, TriangleTag) const noexcept
{
    /*
          *
          |\
          | \
          *--*
    */
    const auto [x, y] = GetReferencePoint(block, m_width);

    if (block.left && block.bottom && block.backDiagonal)
        return TransformParameters{ 0, x, y };

    /*
        *--*
        | /
        |/
        *
    */
    else if (block.left && block.top && block.forwardDiagonal)
        return TransformParameters{ 270, x, y };

    /*
        *--*
         \ |
          \|
           *
    */
    else if (block.right && block.top && block.backDiagonal)
        return TransformParameters{ 180, x, y };

    /*
           *
          /|
         / |
        *--*
    */
    else if (block.right && block.bottom && block.forwardDiagonal)
        return TransformParameters{ 90, x, y };

    return std::nullopt;
}

VoronoiImpl::Graph VoronoiImpl::WeldAsync(const std::vector<std::vector<VoronoiConfig>>& disjointVoronoi) const noexcept
{
    // Initial value
    VoronoiConfig voronoiConfig{ Graph{0}, WeldMap{} };
    auto combining = [&](auto init, auto rhs)
    {
        return WeldGraphs(init, rhs);
    };

    auto combineRow = [&](auto row)
    {
        return std::reduce(std::execution::par_unseq, std::begin(row), std::end(row),
            voronoiConfig, combining);
    };

    // This will combine all of the graphs into one, by reducing all the rows
    // first, then reducing the remaining column
    auto [graph, welds] = std::transform_reduce(std::execution::par_unseq,
        std::begin(disjointVoronoi), std::end(disjointVoronoi), voronoiConfig, combining, combineRow);

    std::set<Vertex> removedVertices;
    for (auto vertex : boost::make_iterator_range(boost::vertices(graph)))
    {
        if (!boost::in_degree(vertex, graph))
            removedVertices.insert(vertex);
    }

    long vertexOffset = 0;
    for (auto vertex : removedVertices)
    {
        boost::remove_vertex(vertex + vertexOffset, graph);

        // we need to decrement the vertex count since the
        // other vertices do not get updated as removals occur
        --vertexOffset;
    }

    return graph;
}

VoronoiImpl::VoronoiConfig VoronoiImpl::WeldGraphs(const VoronoiConfig& init, const VoronoiConfig& rhs) const noexcept
{
    auto [initGraph, initWelds] = init;
    auto [rhsGraph, rhsWelds] = rhs;

    const auto debugWelds = [](auto graph, int width, int height)
    {
        static int ID = 0;
        std::filesystem::path dest{ "C:/Users/Brian/Desktop/graph/WeldDebug" };

        auto strategy = VoronoiVisualizationStrategy<decltype(graph)>{};
        auto visualizer = dpa::graph::internal::LaTeXGraphVisualizer<decltype(graph)>{ strategy };

        dest += "/Block_";
        dest += std::to_string(ID++);
        dest += ".tex";

        std::ofstream output{ dest };

        if (output.is_open())
            return visualizer.writeTex(graph, std::make_tuple(width, height), output);

        return false;
    };

    // Combined graph, which contains the merge between init and rhs
    Graph welded{ boost::num_vertices(initGraph) + boost::num_vertices(rhsGraph) };

    // The init graph is what everything is being accumulated into,
    // and its vertex descriptors will already be correct. Do not
    // offset those vertices when copying the graph
    boost::depth_first_search(initGraph, boost::visitor(GraphCopier{ welded, 0 }));

    // The rhs graph will be what we're folding into init, and will have
    // vertex descriptors in local space. These need to be offset by the
    // number of nodes in the init graph
    boost::depth_first_search(rhsGraph, boost::visitor(GraphCopier{ welded, boost::num_vertices(initGraph) }));

    debugWelds(initGraph, static_cast<int>(boost::num_vertices(initGraph)), static_cast<int>(boost::num_vertices(initGraph)));
    debugWelds(rhsGraph, static_cast<int>(boost::num_vertices(rhsGraph)), static_cast<int>(boost::num_vertices(rhsGraph)));

    // Perform any welds that need to happen to ensure connectivity between components
    WeldVertices(welded, boost::num_vertices(initGraph), initWelds, rhsWelds);
    
    // update the RHS vertices by an offset to ensure they're
    // correct when performing merges across rows
    for (auto& [point, vertex] : rhsWelds)
        vertex += boost::num_vertices(initGraph);

    // Fold the weld points together
    WeldMap weldPoints{ initWelds };
    weldPoints.merge(rhsWelds);

    return { welded, weldPoints };
}

void VoronoiImpl::WeldVertices(Graph& dest, std::size_t vertexOffset, const WeldMap& lhsWelds, const WeldMap& rhsWelds) const noexcept
{
    // Comparator to check if one weld point is smaller than the other
    auto less = [](auto lhs, auto rhs) { return lhs.first < rhs.first; };

    WeldMap mutualPoints;

    // O(n)
    std::set_intersection(std::cbegin(rhsWelds), std::cend(rhsWelds),
                          std::cbegin(lhsWelds), std::cend(lhsWelds),
                          std::inserter(mutualPoints, std::begin(mutualPoints)), less);

    for (const auto [rhsPoint, rhsVertex] : mutualPoints)
    {
        if (auto lhsEntry = lhsWelds.find(rhsPoint); lhsEntry != std::end(lhsWelds))
        {
            const auto [lhsPoint, lhsVertex] = *lhsEntry;
            if (boost::in_degree(rhsVertex + vertexOffset, dest) != 2)
            {
                assert(false);
                break;
            }

            auto [s1, e1] = boost::in_edges(rhsVertex + vertexOffset, dest);
            auto t1 = boost::source(*s1, dest);

            auto [s2, e2] = boost::in_edges(lhsVertex, dest);
            auto t2 = boost::source(*s2, dest);

            boost::clear_vertex(rhsVertex + vertexOffset, dest);
            boost::clear_vertex(lhsVertex, dest);

            boost::add_edge(t1, t2, dest);
            boost::add_edge(t2, t1, dest);
        }
    }
}
}
