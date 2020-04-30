#pragma once

#include <cmath>

#include <limits>
#include <tuple>

#include <boost/graph/depth_first_search.hpp>

namespace dpa::graph::utility
{
/*
    A generic, 2D point
*/
template<typename Vertex>
using Point2D = std::tuple<Vertex, Vertex>;

/*
    Adds two points together

    @param a The first point
    @param b The second point
    @returns A point containing the sum of the two arguments
*/
template<typename T1, typename T2>
constexpr auto operator+(const Point2D<T1> a, const Point2D<T2> b) noexcept
    -> Point2D<decltype(T1{} + T2{})>
{
    return { std::get<0>(a) + std::get<0>(b), std::get<1>(a) + std::get<1>(b) };
}

/*
    Expands a 1D point into a 2D point

    @param vertex The 1D point to expand
    @param rowSize The size of the row in the 2D grid the point is expanded into
    @returns An expanded point
*/
template<typename Vertex>
constexpr Point2D<Vertex> ExpandIndex(Vertex vertex, std::size_t rowSize) noexcept
{
    return { static_cast<int>(vertex % rowSize),
             static_cast<int>(std::floor(vertex / rowSize)) };
}

/*
    Flattens a (i, j) point into a 1D index position

    @param pos The point to flatten
    @param rowSize The width of the row
    @returns A flattened coordinate, which is an index into a 1D array
*/
template<typename Vertex>
constexpr Vertex FlattenPoint(const Point2D<Vertex>& pos, const std::size_t rowSize)
{
    const auto [i, j] = pos;
    return j * rowSize + i;
}

/*
    An enumeration of slope classifications
*/
enum class SlopeClassification
{
    eInvalid, ePositive, eNegative
};

/*
    Classifies the given slope (rise / run) value

    @param slopeValue A slope to classify
    @returns The classification of the slope
*/
constexpr SlopeClassification ClassifySlope(const float slopeValue)
{
    if (slopeValue == std::numeric_limits<float>::infinity() ||
        slopeValue == -std::numeric_limits<float>::infinity() ||
        slopeValue == -0 || slopeValue == +0)
    {
        return SlopeClassification::eInvalid;
    }
    else if (slopeValue > 0.f)
    {
        // This is reversed due to the memory and screen coordinate spaces differing
        return SlopeClassification::ePositive;
    }
    else if (slopeValue < 0.f)
    {
        // This is reversed due to the memory and screen coordinate spaces differing
        return SlopeClassification::eNegative;
    }

    return SlopeClassification::eInvalid;
}

/*
    Computes the slope of the line formed by the source and target
    vertex, and classifies the slope appropriately

    @param source The start vertex of the edge
    @param target The target vertex of the edge
    @param imageWidth The width of the image
    @returns The classification of the slope
*/
template<typename Vertex>
constexpr SlopeClassification GetSlopeClassification(const Vertex source, const Vertex target,
                                      const std::size_t imageWidth)
{
    const auto [x1, y1] = ExpandIndex(source, imageWidth);
    const auto [x2, y2] = ExpandIndex(target, imageWidth);

    const auto rise = static_cast<float>(y2 - y1);
    const auto run = static_cast<float>(x2 - x1);

    return ClassifySlope(rise / run);
}

/*
    Gets a pair of vertices that are associated with an
    edge that could cross the edge formed by the supplied
    vertices

    @param start The starting vertex of the edge
    @param end The ending vertex of the edge
    @param imageWidth The width of the image
    @returns An edge, represented by vertices, that could cross the
             edge formed by the supplied vertices
*/
template<typename Vertex>
constexpr Point2D<Vertex> GetCrossingEdge(const Vertex start, const Vertex end,
                                          const std::size_t imageWidth)
{
    Point2D<Vertex> cStart = ExpandIndex(start, imageWidth);
    Point2D<Vertex> cEnd = ExpandIndex(end, imageWidth);

    // Swap y values, which flips the slope
    std::swap(std::get<1>(cStart), std::get<1>(cEnd));

    Vertex startPointX = FlattenPoint<Vertex>(cStart, imageWidth);
    Vertex endPointX = FlattenPoint<Vertex>(cEnd, imageWidth);

    return { startPointX, endPointX };
}

/*
    A dfs visitor that counts the number of edges in a curve
    feature in the similarity graph
*/
class EdgeCounter : public boost::default_dfs_visitor
{
public:

    explicit EdgeCounter(long long& count)
        : m_length(count)
    {}

    EdgeCounter(const EdgeCounter& other)
        : m_length(other.m_length)
    {}

    /*
        Increments the edge count by 1 for every edge examined

        @param edge Unused
        @param graph Unused
    */
    template<typename Edge, typename Graph>
    void tree_edge(Edge, const Graph&)
    {
        ++m_length;
    }

private:

    long long& m_length;

};
}