#include <SimilarityGraph.h>

#include <SimilarityGraphImpl.h>

#include <stdexcept>

namespace
{
template<typename... Handlers>
struct HeuristicDispatcher : Handlers...
{
    using Handlers::operator()...;
};

template<typename... Handlers>
HeuristicDispatcher(Handlers...)->HeuristicDispatcher<Handlers...>;
}

namespace di = dpa::image;
namespace dpa::graph
{ 
SimilarityGraph::SimilarityGraph()
{}

SimilarityGraph::SimilarityGraph(const di::Image<di::RGB, stbi_uc>& image)
{
    if (!build(image))
        throw std::runtime_error("Failed to build the similarity graph.");
}

bool SimilarityGraph::build(const di::Image<di::RGB, stbi_uc>& image)
{
    return impl()->build(image);
}

void SimilarityGraph::applyHeuristic(heuristics::Heuristic heuristic)
{
    std::visit(HeuristicDispatcher
        {
            [&](const heuristics::Curves& visitor)
                {
                    impl()->applyHeuristic(visitor, heuristics::FilteredEdges::eDissimilar);
                    impl()->setEdgeProperties(visitor, [&](auto&& edge, auto value)
                        {
                            impl()->m_graph[edge].curvesWeight += std::get<double>(value);
                        });
                },
            [&](const heuristics::DissimilarPixels& visitor)
                {   
                    impl()->applyHeuristic(visitor);
                    impl()->setEdgeProperties(visitor, [&](auto edge, auto value)
                        {
                            impl()->m_graph[edge].dissimilar = std::get<bool>(value);
                        });
                },
            [&](const heuristics::Islands& visitor)
                {
                    impl()->applyHeuristic(visitor, heuristics::FilteredEdges::eDissimilar);
                    impl()->setEdgeProperties(visitor, [&](auto&& edge, auto value)
                        {
                            impl()->m_graph[edge].islandsWeight += std::get<double>(value);
                        });
                },
            [&](const heuristics::SparsePixels& visitor)
                {
                    impl()->applyHeuristic(visitor, heuristics::FilteredEdges::eDissimilar);
                    impl()->setEdgeProperties(visitor, [&](auto&& edge, auto value)
                        {
                            impl()->m_graph[edge].sparsePixelsWeight += std::get<double>(value);
                        });
                },
        }, heuristic);
}

void SimilarityGraph::printGraph(std::ostream& stream)
{
    impl()->printGraph(stream);
}

bool SimilarityGraph::writeTex(std::ostream& output, heuristics::FilteredEdges filteredEdges)
{
    return impl()->writeTex(output, filteredEdges);
}

std::set<std::tuple<std::size_t, std::size_t>> SimilarityGraph::getEdges(heuristics::FilteredEdges filteredEdges) noexcept
{
    return impl()->getEdges(filteredEdges);
}

std::shared_ptr<internal::SimilarityGraphImpl> SimilarityGraph::impl()
{
    return dpa::internal::any_pointer_cast<internal::SimilarityGraphImpl>(Implementation::impl());
}

std::shared_ptr<std::any> SimilarityGraph::createImpl()
{
    return std::make_shared<std::any>(internal::SimilarityGraphImpl{});
}
}
