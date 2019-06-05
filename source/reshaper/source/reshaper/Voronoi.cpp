#include <Voronoi.h>
#include <VoronoiImpl.h>

#include <any>
#include <memory>
#include <tuple>
#include <vector>

namespace dpa::voronoi
{
VoronoiDiagram::VoronoiDiagram(std::tuple<int, int>& imageDims) noexcept
{
    impl()->setDimensions(imageDims);
}

void dpa::voronoi::VoronoiDiagram::build(const std::set<BlockEdge>& edges) noexcept
{
    impl()->build(edges);
}

bool VoronoiDiagram::writeTex(std::ostream& output)
{
    return impl()->writeTex(output);
}

void VoronoiDiagram::printGraph(std::ostream& stream)
{
    impl()->printGraph(stream);
}

void VoronoiDiagram::printVertices(std::ostream& stream)
{
    impl()->printVertices(stream);
}

std::shared_ptr<std::any> dpa::voronoi::VoronoiDiagram::createImpl()
{
    return std::make_shared<std::any>(internal::VoronoiImpl{});
}

std::shared_ptr<internal::VoronoiImpl> dpa::voronoi::VoronoiDiagram::impl()
{
    return dpa::internal::any_pointer_cast<internal::VoronoiImpl>(Implementation::impl());
}
}
