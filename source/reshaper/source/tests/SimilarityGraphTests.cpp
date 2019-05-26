#include <SimilarityGraphTests.h>

#include <Heuristics.h>
#include <Image.h>
#include <ImageUtil.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

using namespace dpa::image;
using namespace dpa::graph::heuristics;

TEST_F(SimilarityGraphTests, BuildGraph)
{
    Image<RGB, stbi_uc> testImage{ m_smallImage };

    // Build the graph from the converted image
    m_graph.build(testImage);

    std::ostringstream out;
    m_graph.printGraph(std::ref(out));

    EXPECT_EQ(getSmallImageLatticeGraphAnswer(), out.str());
}

TEST_F(SimilarityGraphTests, DissimilarPixels)
{
    Image<RGB, stbi_uc> testImage{ m_skull };
    m_graph.build(testImage);

    auto dissimilar = DissimilarPixels{};
    m_graph.applyHeuristic(dissimilar);
    
    auto heuristicData = dissimilar.getMarkedEdges();
    auto solutionData = getSolutionData<DissimilarEdgeData>(m_dissimilar_solution);

    ASSERT_EQ(solutionData.size(), heuristicData.size());

    auto solutionIter = std::begin(solutionData);
    for (const auto [edge, value] : dissimilar.getMarkedEdges())
    {
        EXPECT_EQ(solutionIter->edge, edge);
        EXPECT_EQ(solutionIter->value, std::get<bool>(value));

        solutionIter = std::next(solutionIter);
    }

    dissimilar.clearMarkedEdges();
}

TEST_F(SimilarityGraphTests, Curves)
{
    Image<RGB, stbi_uc> testImage{ m_curve };
    m_graph.build(testImage);

    auto dissimilar = DissimilarPixels{};
    m_graph.applyHeuristic(dissimilar);
    dissimilar.clearMarkedEdges();

    auto curves = Curves{{ testImage.getWidth(), testImage.getHeight() }};
    m_graph.applyHeuristic(curves);

    auto heuristicData = curves.getMarkedEdges();
    auto solutionData = getSolutionData<CrossingEdgeData>(m_curves_solution);

    ASSERT_EQ(solutionData.size(), heuristicData.size());

    auto solutionIter = std::begin(solutionData);
    for (const auto [edge, value] : curves.getMarkedEdges())
    {
        EXPECT_EQ(solutionIter->edge, edge);
        EXPECT_EQ(solutionIter->value, std::get<double>(value));

        solutionIter = std::next(solutionIter);
    }

    curves.clearMarkedEdges();
}

TEST_F(SimilarityGraphTests, Islands)
{
    Image<RGB, stbi_uc> testImage{ m_islands };
    m_graph.build(testImage);

    auto dissimilar = DissimilarPixels{};
    m_graph.applyHeuristic(dissimilar);
    dissimilar.clearMarkedEdges();

    auto islands = Islands{ { testImage.getWidth(), testImage.getHeight() } };
    m_graph.applyHeuristic(islands);

    auto heuristicData = islands.getMarkedEdges();
    auto solutionData = getSolutionData<CrossingEdgeData>(m_islands_solution);

    ASSERT_EQ(solutionData.size(), heuristicData.size());

    auto solutionIter = std::begin(solutionData);
    for (const auto [edge, value] : islands.getMarkedEdges())
    {
        EXPECT_EQ(solutionIter->edge, edge);
        EXPECT_EQ(solutionIter->value, std::get<double>(value));

        solutionIter = std::next(solutionIter);
    }

    islands.clearMarkedEdges();
}

TEST_F(SimilarityGraphTests, SparsePixels)
{
    Image<RGB, stbi_uc> testImage{ m_sparsePixels };
    m_graph.build(testImage);

    auto dissimilar = DissimilarPixels{};
    m_graph.applyHeuristic(dissimilar);
    dissimilar.clearMarkedEdges();

    auto sparsePixels = SparsePixels{ { testImage.getWidth(), testImage.getHeight() } };
    m_graph.applyHeuristic(sparsePixels);
    
    auto heuristicData = sparsePixels.getMarkedEdges();
    auto solutionData = getSolutionData<CrossingEdgeData>(m_sparse_pixels_solution);

    ASSERT_EQ(solutionData.size(), heuristicData.size());

    auto solutionIter = std::begin(solutionData);
    for (const auto [edge, value] : sparsePixels.getMarkedEdges())
    {
        EXPECT_EQ(solutionIter->edge, edge);
        EXPECT_EQ(solutionIter->value, std::get<double>(value));

        solutionIter = std::next(solutionIter);
    }

    sparsePixels.clearMarkedEdges();
}