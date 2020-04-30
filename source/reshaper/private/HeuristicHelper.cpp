#include <HeuristicHelper.h>

namespace dpa::graph::heuristics
{
void HeuristicHelper::clearMarkedEdges(const boost::uuids::uuid& uuid) noexcept
{
    m_markedEdges.erase(uuid);
}

const HeuristicHelper::EdgeMap& HeuristicHelper::getMarkedEdges(const boost::uuids::uuid& uuid) noexcept
{
    return m_markedEdges[uuid];
}

void HeuristicHelper::insertMarkedEdge(const boost::uuids::uuid& uuid, const Edge& edge,
                                       EdgeProperty value) noexcept
{
    m_markedEdges[uuid][edge] = value;
}

HeuristicHelper& HeuristicHelper::instance() noexcept
{
    static HeuristicHelper instance;
    return instance;
}
}
