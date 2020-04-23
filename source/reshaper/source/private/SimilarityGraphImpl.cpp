#include <SimilarityGraphImpl.h>

#include <ImageUtil.h>
#include <GraphUtils.h>
#include <SimilarityGraphVisualizationStrategy.h>

#include <boost/graph/graph_utility.hpp>

#include <numeric>
#include <tuple>

namespace
{
/*
    Composes the given functions into a single
    function that returns the sum of their return values

    @param funcA A function that returns a valid operand of operator +
    @param funcA A function that returns a valid operand of operator +
    @returns A new function that sums the given functions and returns
             a valid operand of operator +
*/
template<typename FuncA, typename FuncB>
auto recursiveSum(FuncA funcA, FuncB funcB)
{
    return [=](const auto& edge)
    {
        return funcA(edge) + funcB(edge);
    };
}
}

namespace dpa::graph::internal
{
bool SimilarityGraphImpl::build(const di::Image<di::RGB, stbi_uc>& image)
{
    // Graph may be built from a loaded or programatically created image
    if (!image.getHeight() || !image.getWidth())
        return false;

    if (auto convertedImage = di::utility::RGB_To_YCbCr(image); convertedImage)
    {
        // Create the adjacency matrix with the correct number of vertices
        m_graph = Graph(convertedImage.value().getWidth() * convertedImage.value().getHeight());

        // Set the pixel colors on each node
        if (!setNodeProperties(convertedImage.value()))
            return false;
    }

    const di::internal::Point2D imageDims = { image.getWidth(), image.getHeight() };

    // Connect each of the nodes to make an 8-connected lattice graph
    connectHorizontally(imageDims);
    connectVertically(imageDims);
    connectForwardDiagonally(imageDims);
    connectBackwardDiagonally(imageDims);

    // Set the image dimensions so we can visualize the graph properly
    m_imageDims = { image.getWidth(), image.getHeight() };

    return true;
}

void SimilarityGraphImpl::printGraph(std::ostream& stream) const noexcept
{
    boost::print_graph(m_graph, boost::get(boost::vertex_index, m_graph), stream);
}

void SimilarityGraphImpl::connectHorizontally(const utility::Point2D<Vertex>& dims)
{
    const auto [imageWidth, imageHeight] = dims;
    for (auto h = 0; h < imageHeight; ++h)
    {
        for (auto w = 1; w < imageWidth; ++w)
        {
            auto curr = utility::FlattenPoint<Vertex>({ w, h }, imageWidth);
            auto prev = utility::FlattenPoint<Vertex>({ w - 1, h }, imageWidth);

            boost::add_edge(prev, curr, m_graph);
        }
    }
}

void SimilarityGraphImpl::connectVertically(const utility::Point2D<Vertex>& dims)
{
    const auto [imageWidth, imageHeight] = dims;
    for (auto w = 0; w < imageWidth; ++w)
    {
        for (auto h = 1; h < imageHeight; ++h)
        {
            auto curr = utility::FlattenPoint<Vertex>({ w, h }, imageWidth);
            auto prev = utility::FlattenPoint<Vertex>({ w, h - 1 }, imageWidth);

            boost::add_edge(prev, curr, m_graph);
        }
    }
}

void SimilarityGraphImpl::connectBackwardDiagonally(const utility::Point2D<Vertex>& dims)
{
    const auto [imageWidth, imageHeight] = dims;
    for (auto h = 1; h < imageHeight; ++h)
    {
        for (auto w = 1; w < imageWidth; ++w)
        {
            auto curr = utility::FlattenPoint<Vertex>({ w, h }, imageWidth);
            auto prev = utility::FlattenPoint<Vertex>({ w - 1, h - 1 }, imageWidth);

            boost::add_edge(prev, curr, m_graph);
        }
    }
}

void SimilarityGraphImpl::connectForwardDiagonally(const utility::Point2D<Vertex>& dims)
{
    const auto [imageWidth, imageHeight] = dims;
    for (auto h = 1; h < imageHeight; ++h)
    {
        for (auto w = imageWidth - 1; w > 0; --w)
        {
            auto curr = utility::FlattenPoint<Vertex>({ w - 1, h }, imageWidth);
            auto prev = utility::FlattenPoint<Vertex>({ w, h - 1 }, imageWidth);

            boost::add_edge(curr, prev, m_graph);
        }
    }
}

bool SimilarityGraphImpl::setNodeProperties(const di::Image<di::YCbCr, stbi_uc>& image)
{
    // The graph needs to be initialized with verticies first
    if (!boost::num_vertices(m_graph))
        return false;

    return di::utility::foreach_pixel(image,
        [&](const di::internal::Point2D& pos, di::YCbCr<stbi_uc> pixel)
        {
            const auto idx = utility::FlattenPoint<Vertex>(pos, image.getWidth());
            std::tie(m_graph[idx].Y, m_graph[idx].Cb, m_graph[idx].Cr) = pixel;

            return true;
        });
}

SimilarityGraphImpl::OptionalEdgeFilter SimilarityGraphImpl::CreateEdgeWeightFilter(
    heuristics::FilteredEdges filteredEdges, EdgeFilter dissimilar) const noexcept
{
    return [=](const Edge& edge)
        -> std::optional<bool>
        {
            const auto source = boost::source(edge, m_graph);
            const auto target = boost::target(edge, m_graph);

            const auto [imageWidth, imageHeight] = m_imageDims;

            // Bail if we're working with a vertical or horizontal edge
            if (utility::GetSlopeClassification(source, target, imageWidth) !=
                utility::SlopeClassification::eInvalid)
            {
                // Ensure we're only evaluating crossing edges
                const auto [xSource, xTarget] = utility::GetCrossingEdge(source, target, imageWidth);
                if (auto [crossingEdge, found] = boost::edge(xSource, xTarget, m_graph); found)
                {
                    // We need to only evaluate this against the crossing edge only
                    // if the crossing edge is connected
                    if (!dissimilar(crossingEdge))
                    {
                        auto crossingEdgeComparator = CreateCrossingEdgeFilter(filteredEdges);
                        return crossingEdgeComparator(edge, crossingEdge);
                    }
                }
            }

            return std::nullopt;
        };
}

SimilarityGraphImpl::EdgeWeightFunc SimilarityGraphImpl::CreateEdgeWeightAccumulator(
    heuristics::FilteredEdges filteredEdges) const noexcept
{
    EdgeWeightFunc curvesWeight = [&](const auto& edge) { return m_graph[edge].curvesWeight; };
    EdgeWeightFunc islandsWeight = [&](const auto& edge) { return m_graph[edge].islandsWeight; };
    EdgeWeightFunc sparsePixelsWeight = [&](const auto& edge) { return m_graph[edge].sparsePixelsWeight; };

    // default to a zero ret function as to not affect the total weight
    EdgeWeightFunc edgeWeightSum = [&](const auto&) { return 0; };

    // Recursively compose weight functions based on the filter
    heuristics::VisitEdgeFilters([&](heuristics::FilteredEdges edgeFilter)
        {
            if ((edgeFilter & filteredEdges) == heuristics::FilteredEdges::eCurves)
                edgeWeightSum = recursiveSum(edgeWeightSum, curvesWeight);

            if ((edgeFilter & filteredEdges) == heuristics::FilteredEdges::eIslands)
                edgeWeightSum = recursiveSum(edgeWeightSum, islandsWeight);

            if ((edgeFilter & filteredEdges) == heuristics::FilteredEdges::eSparsePixels)
                edgeWeightSum = recursiveSum(edgeWeightSum, sparsePixelsWeight);
        });

    return edgeWeightSum;
}

SimilarityGraphImpl::OptionalCrossingEdgeFilter SimilarityGraphImpl::CreateCrossingEdgeFilter(
    heuristics::FilteredEdges filteredEdges) const noexcept
{
    return [=](const Edge& edge, const Edge& crossing) 
        -> std::optional<bool>
    {
        auto edgeWeightAccumulator = CreateEdgeWeightAccumulator(filteredEdges);

        auto edgeWeight = edgeWeightAccumulator(edge);
        auto crossingWeight = edgeWeightAccumulator(crossing);

        if (edgeWeight == crossingWeight)
            return std::make_optional(false);
        else if (edgeWeight || crossingWeight)
            return std::make_optional(edgeWeight > crossingWeight);

        return std::nullopt;
    };
}

SimilarityGraphImpl::EdgeFilter SimilarityGraphImpl::CreateEdgeFilter(heuristics::FilteredEdges filteredEdges) const noexcept
{
    // if no filters are are given, then we want to show every edge
    EdgeFilter edgeFilter = [](const auto&) { return true; };

    // If we have some flag packed in, then build the filter function
    if (filteredEdges != heuristics::FilteredEdges::eNone)
    {
        edgeFilter = [=](const auto& edge)
        {
            auto s = boost::source(edge, m_graph); s;
            auto t = boost::target(edge, m_graph); t;

            // The paradigm here is that we want to return true to keep the edge,
            // and false to remove it
            EdgeFilter dissimilar = [&](const auto& edge) { return m_graph[edge].dissimilar; };
            if (hasFilter(filteredEdges, heuristics::FilteredEdges::eDissimilar))
            {
                // If the edge is dissimilar, then we don't want to keep it. If it is similar,
                // then we want to try and evaluate it with crossing edge filters
                if (dissimilar(edge))
                    return false;
            }

            if (hasFilter(filteredEdges, heuristics::FilteredEdges::eCurves) ||
                hasFilter(filteredEdges, heuristics::FilteredEdges::eIslands) ||
                hasFilter(filteredEdges, heuristics::FilteredEdges::eSparsePixels))
            {
                // Only evaluate crossing edges that are connected
                auto edgeWeightFilter = CreateEdgeWeightFilter(filteredEdges, dissimilar);

                if (auto crossingResult = edgeWeightFilter(edge); crossingResult.has_value())
                    return crossingResult.value();
            }

            return true;
        };
    }

    return edgeFilter;
}

bool SimilarityGraphImpl::writeTex(std::ostream& output, heuristics::FilteredEdges filteredEdges)
{
    auto filter = CreateEdgeFilter(filteredEdges);
    auto filteredGraph = boost::filtered_graph(m_graph, filter);

    auto strategy = SimilarityGraphVisualizationStrategy<decltype(filteredGraph)>{};
    auto visualizer = LaTeXGraphVisualizer<decltype(filteredGraph)>{ strategy };

    return visualizer.writeTex(filteredGraph, m_imageDims, output);
}

std::set<std::tuple<std::size_t, std::size_t>> SimilarityGraphImpl::getEdges(heuristics::FilteredEdges filteredEdges) noexcept
{
    std::set<std::tuple<std::size_t, std::size_t>> edges;

    auto filter = CreateEdgeFilter(filteredEdges);
    auto filteredGraph = boost::filtered_graph(m_graph, filter);

    for (const auto& edge : boost::make_iterator_range(boost::edges(filteredGraph)))
        edges.insert({boost::source(edge, filteredGraph), boost::target(edge, filteredGraph)});

    return edges;
}
}
