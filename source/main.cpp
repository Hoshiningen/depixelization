
#include <Image.h>
#include <SimilarityGraph.h>
#include <Heuristics.h>
#include <Voronoi.h>

#include <algorithm>
#include <iostream>
#include <variant>
#include <typeinfo>
#include <filesystem>
#include <fstream>
#include <numeric>
#include <execution>
#include <mutex>

int main()
{
    using namespace dpa::image;
    using namespace dpa::graph;

    std::filesystem::path bigImage{ "../images/curve_test.png" };
    Image<RGB, stbi_uc> image{ bigImage };

    SimilarityGraph simGraph;
    simGraph.build(image);

    auto imageDims = std::make_tuple(image.getWidth(), image.getHeight());

    simGraph.applyHeuristic(heuristics::DissimilarPixels{});
    simGraph.applyHeuristic(heuristics::Curves{ imageDims });
    simGraph.applyHeuristic(heuristics::Islands{ imageDims });
    simGraph.applyHeuristic(heuristics::SparsePixels{ imageDims });

    dpa::voronoi::VoronoiDiagram voronoi{ imageDims };
    voronoi.build(simGraph.getEdges());

    std::ofstream out{ "C:/Users/Brian/Desktop/graph/full_voronoi.tex" };
    voronoi.writeTex(out);
    out.close();
}
