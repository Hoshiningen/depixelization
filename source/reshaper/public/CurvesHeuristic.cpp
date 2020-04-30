#include <CurvesHeuristic.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace dpa::graph::heuristics
{
Curves::Curves()
    : m_uuid(boost::uuids::random_generator()())
{}

Curves::Curves(const utility::Point2D<int>& imageDims)
    : Curves()
{
    m_imageDims = imageDims;
}

Curves::Curves(const Curves& other)
{
    *this = other;
}

Curves& Curves::operator=(const Curves& other)
{
    if (this == &other)
        return *this;

    m_uuid = other.m_uuid;
    m_imageDims = other.m_imageDims;

    return *this;
}

const HeuristicHelper::EdgeMap& Curves::getMarkedEdges() const noexcept
{
    return HeuristicHelper::instance().getMarkedEdges(m_uuid);
}

void Curves::clearMarkedEdges() const noexcept
{
    HeuristicHelper::instance().clearMarkedEdges(m_uuid);
}
}
