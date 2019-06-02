#include <SparsePixelsHeuristic.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace dpa::graph::heuristics
{
SparsePixels::SparsePixels()
    : m_uuid(boost::uuids::random_generator()())
{}

SparsePixels::SparsePixels(const utility::Point2D<int>& imageDims)
    : SparsePixels()
{
    m_imageDims = imageDims;
}

SparsePixels::SparsePixels(const SparsePixels& other)
{
    *this = other;
}

SparsePixels& SparsePixels::operator=(const SparsePixels& other)
{
    if (this == &other)
        return *this;

    m_uuid = other.m_uuid;
    m_imageDims = other.m_imageDims;

    return *this;
}

const HeuristicHelper::EdgeMap& SparsePixels::getMarkedEdges() const noexcept
{
    return HeuristicHelper::instance().getMarkedEdges(m_uuid);
}

void SparsePixels::clearMarkedEdges() const noexcept
{
    HeuristicHelper::instance().clearMarkedEdges(m_uuid);
}
}