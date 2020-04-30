#include <IslandsHeuristic.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace dpa::graph::heuristics
{
Islands::Islands()
    : m_uuid(boost::uuids::random_generator()())
{}

Islands::Islands(const utility::Point2D<int>& imageDims)
    : Islands()
{
    m_imageDims = imageDims;
}

Islands::Islands(const Islands& other)
{
    *this = other;
}

Islands& Islands::operator=(const Islands& other)
{
    if (this == &other)
        return *this;

    m_uuid = other.m_uuid;
    m_imageDims = other.m_imageDims;

    return *this;
}

const HeuristicHelper::EdgeMap& Islands::getMarkedEdges() const noexcept
{
    return HeuristicHelper::instance().getMarkedEdges(m_uuid);
}

void Islands::clearMarkedEdges() const noexcept
{
    HeuristicHelper::instance().clearMarkedEdges(m_uuid);
}
}