#include <DissimilarHeuristic.h>

namespace dpa::graph::heuristics
{
DissimilarPixels::DissimilarPixels()
    : m_uuid(boost::uuids::random_generator()())
{}

DissimilarPixels::DissimilarPixels(const DissimilarPixels& other)
{
    *this = other;
}

DissimilarPixels& DissimilarPixels::operator=(const DissimilarPixels& other)
{
    if (this == &other)
        return *this;

    m_uuid = other.m_uuid;

    return *this;
}

const HeuristicHelper::EdgeMap& DissimilarPixels::getMarkedEdges() const noexcept
{
    return HeuristicHelper::instance().getMarkedEdges(m_uuid);
}

void DissimilarPixels::clearMarkedEdges() const noexcept
{
    HeuristicHelper::instance().clearMarkedEdges(m_uuid);
}
}
