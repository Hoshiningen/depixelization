#include <SimilarityGraph.h>

#include <ImageUtil.h>

// Disable warnings thrown in boost
#pragma warning( push )
#pragma warning( disable: 4996 4127 )

#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/graph_utility.hpp>

#pragma warning( pop )

#include <memory>
#include <stdexcept>
#include <tuple>

namespace di = dpa::image;
namespace
{
/*
    A vertex property that allows us to store color data
    on each node in the similarity graph
*/
struct VertexProperty
{
    VertexProperty() = default;
    VertexProperty(stbi_uc y, stbi_uc cb, stbi_uc cr)
        : Y(y), Cb(cb), Cr(cr)
    {}

    stbi_uc Y{ 0 };
    stbi_uc Cb{ 0 };
    stbi_uc Cr{ 0 };
};

/*
    Flattens a (i, j) point into a 1D index position

    @param pos The point to flatten
    @param rowSize The width of the row
    @returns A flattened coordinate, which is an index into a 1D array
*/
auto flattenPoint(const di::internal::Point2D& pos, std::size_t rowSize)
{
    const auto [i, j] = pos;
    return j * rowSize + i;
}
}

namespace dpa::graph
{
/*
    The implementation class of the similarity graph
*/
struct SimilarityGraph::Impl
{
public:

    Impl();

    /*
        Builds the similarity graph from the given image

        @param image The image in YCbCr color space to build the graph from
        @returns True if the graph was built, false otherwise
    */
    bool build(const di::Image<di::YCbCr, stbi_uc>& image);

    /*
        Prints a non-graphical representation of the graph

        @param stream The stream to write to
    */
    void printGraph(std::ostream& stream) const noexcept;

    /*
        Connects each of the nodes together along each row

        @param dims The images dimensions
    */
    void connectHorizontally(const di::internal::Point2D& dims);

    /*
        Connects each of the nodes together along each column

        @param dims The images dimensions
    */
    void connectVertically(const di::internal::Point2D& dims);

    /*
        Connects each of the nodes together along each backward diagonal

        @param dims The images dimensions
    */
    void connectBackwardDiagonally(const di::internal::Point2D& dims);

    /*
        Connects each of the nodes together along each forward diagonal

        @param dims The images dimensions
    */
    void connectForwardDiagonally(const di::internal::Point2D& dims);

    /*
        Sets the graph's vertex properties corresponding pixel data

        @param image An image in YCbCr color space containing pixel data
        @returns True if the node properties were set, false otherwise
    */
    bool setNodeProperties(const di::Image<di::YCbCr, stbi_uc>& image);

public:

    using Graph = boost::adjacency_matrix<boost::undirectedS, VertexProperty>;

    Graph m_graph{ 0 };

};

SimilarityGraph::Impl::Impl() = default;

bool SimilarityGraph::Impl::build(const di::Image<di::YCbCr, stbi_uc>& image)
{
    // Graph may be built from a loaded or programatically created image
    if (!image.getHeight() || !image.getWidth())
        return false;

    // Create the adjacency matrix with the correct number of vertices
    m_graph = Graph(image.getWidth() * image.getHeight());
 
    // Set the pixel colors on each node
    if (!setNodeProperties(image))
        return false;

    const di::internal::Point2D imageDims = { image.getWidth(), image.getHeight() };

    // Connect each of the nodes to make an 8-connected lattice graph
    connectHorizontally(imageDims);
    connectVertically(imageDims);
    connectForwardDiagonally(imageDims);
    connectBackwardDiagonally(imageDims);

    return true;
}

void SimilarityGraph::Impl::printGraph(std::ostream& stream) const noexcept
{
    boost::print_graph(m_graph, boost::get(boost::vertex_index, m_graph), stream);
}

void SimilarityGraph::Impl::connectHorizontally(const di::internal::Point2D& dims)
{
    const auto [imageWidth, imageHeight] = dims;
    for (auto h = 0; h < imageHeight; ++h)
    {
        for (auto w = 1; w < imageWidth; ++w)
        {
            auto curr = flattenPoint({ w, h }, imageWidth);
            auto prev = flattenPoint({ w - 1, h }, imageWidth);

            boost::add_edge(prev, curr, m_graph);
        }
    }
}

void SimilarityGraph::Impl::connectVertically(const di::internal::Point2D& dims)
{
    const auto [imageWidth, imageHeight] = dims;
    for (auto w = 0; w < imageWidth; ++w)
    {
        for (auto h = 1; h < imageHeight; ++h)
        {
            auto curr = flattenPoint({ w, h }, imageWidth);
            auto prev = flattenPoint({ w, h - 1 }, imageWidth);

            boost::add_edge(prev, curr, m_graph);
        }
    }
}

void SimilarityGraph::Impl::connectBackwardDiagonally(const di::internal::Point2D& dims)
{
    const auto [imageWidth, imageHeight] = dims;
    for (auto h = 1; h < imageHeight; ++h)
    {
        for (auto w = 1; w < imageWidth; ++w)
        {
            auto curr = flattenPoint({ w, h }, imageWidth);
            auto prev = flattenPoint({ w - 1, h - 1 }, imageWidth);

            boost::add_edge(prev, curr, m_graph);
        }
    }
}

void SimilarityGraph::Impl::connectForwardDiagonally(const di::internal::Point2D& dims)
{
    const auto [imageWidth, imageHeight] = dims;
    for (auto h = 1; h < imageHeight; ++h)
    {
        for (auto w = imageWidth - 1; w > 0; --w)
        {
            auto curr = flattenPoint({ w - 1, h }, imageWidth);
            auto prev = flattenPoint({ w, h - 1 }, imageWidth);

            boost::add_edge(prev, curr, m_graph);
        }
    }
}

bool SimilarityGraph::Impl::setNodeProperties(const di::Image<di::YCbCr, stbi_uc>& image)
{
    // The graph needs to be initialized with verticies first
    if (!m_graph.m_matrix.size())
        return false;

    return di::utility::foreach_pixel(image,
        [&](const di::internal::Point2D& pos, di::YCbCr<stbi_uc> pixel)
        {
            const auto idx = flattenPoint(pos, image.getWidth());
            std::tie(m_graph[idx].Y, m_graph[idx].Cb, m_graph[idx].Cr) = pixel;

            return true;
        });
}

SimilarityGraph::SimilarityGraph()
    : m_pImpl(std::make_unique<Impl>())
{}

SimilarityGraph::SimilarityGraph(const di::Image<di::YCbCr, stbi_uc>& image)
    : m_pImpl(std::make_unique<Impl>())
{
    if (!build(image))
        throw std::runtime_error("Failed to build the similarity graph.");
}

// Makes SimilarityGraph::Impl a complete type
SimilarityGraph::~SimilarityGraph() = default;
SimilarityGraph::SimilarityGraph(SimilarityGraph&&) noexcept = default;
SimilarityGraph& SimilarityGraph::operator=(SimilarityGraph&&) noexcept = default;

bool SimilarityGraph::build(const di::Image<di::YCbCr, stbi_uc>& image)
{
    return m_pImpl->build(image);
}

void SimilarityGraph::printGraph(std::ostream& stream) const noexcept
{
    m_pImpl->printGraph(stream);
}
}
