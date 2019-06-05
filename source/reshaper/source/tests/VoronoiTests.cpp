#include <VoronoiTests.h>
#include <SimilarityGraph.h>
#include <Heuristics.h>

#pragma warning(push)
#pragma warning(disable: 4996)

#include <boost/geometry.hpp>

#pragma warning(pop)

#include <set>

TEST_F(VoronoiTests, Build_TriangleConfiguration_1)
{
    std::set<std::tuple<std::size_t, std::size_t>> edges;

    /*
        0   1
        | \  
        2 - 3
    */
    edges.insert({ 0, 2 });
    edges.insert({ 0, 3 });
    edges.insert({ 2, 3 });

    VoronoiDiagram voronoi{ std::make_tuple(2, 2) };
    voronoi.build(edges);

    std::ostringstream output;
    voronoi.printVertices(std::ref(output));

    EXPECT_EQ(getTriangleConfiguration1Answer(), output.str());
}

TEST_F(VoronoiTests, Build_TriangleConfiguration_2)
{
    std::set<std::tuple<std::size_t, std::size_t>> edges;

    /*
        0 - 1
        | /
        2   3
    */
    edges.insert({ 0, 1 });
    edges.insert({ 0, 2 });
    edges.insert({ 1, 2 });

    VoronoiDiagram voronoi{ std::make_tuple(2, 2) };
    voronoi.build(edges);

    std::ostringstream output;
    voronoi.printVertices(std::ref(output));

    EXPECT_EQ(getTriangleConfiguration2Answer(), output.str());
}

TEST_F(VoronoiTests, Build_TriangleConfiguration_3)
{
    std::set<std::tuple<std::size_t, std::size_t>> edges;

    /*
        0 - 1
          \ |
        2   3
    */
    edges.insert({ 0, 1 });
    edges.insert({ 0, 3 });
    edges.insert({ 1, 3 });

    VoronoiDiagram voronoi{ std::make_tuple(2, 2) };
    voronoi.build(edges);

    std::ostringstream output;
    voronoi.printVertices(std::ref(output));

    EXPECT_EQ(getTriangleConfiguration3Answer(), output.str());
}

TEST_F(VoronoiTests, Build_TriangleConfiguration_4)
{
    std::set<std::tuple<std::size_t, std::size_t>> edges;

    /*
        0   1
          / |
        2 - 3
    */
    edges.insert({ 1, 2 });
    edges.insert({ 1, 3 });
    edges.insert({ 2, 3 });

    VoronoiDiagram voronoi{ std::make_tuple(2, 2) };
    voronoi.build(edges);

    std::ostringstream output;
    voronoi.printVertices(std::ref(output));

    EXPECT_EQ(getTriangleConfiguration4Answer(), output.str());
}

TEST_F(VoronoiTests, Build_DiagonalConfiguration_1)
{
    std::set<std::tuple<std::size_t, std::size_t>> edges;

    /*
        0   1
          / 
        2   3
    */

    edges.insert({ 2, 1 });

    VoronoiDiagram voronoi{ std::make_tuple(2, 2) };
    voronoi.build(edges);

    std::ostringstream output;
    voronoi.printVertices(std::ref(output));

    EXPECT_EQ(getDiagonalConfiguration1Answer(), output.str());
}

TEST_F(VoronoiTests, Build_DiagonalConfiguration_2)
{
    std::set<std::tuple<std::size_t, std::size_t>> edges;

    /*
        0   1
          \
        2   3
    */

    edges.insert({ 0, 3 });

    VoronoiDiagram voronoi{ std::make_tuple(2, 2) };
    voronoi.build(edges);

    std::ostringstream output;
    voronoi.printVertices(std::ref(output));

    EXPECT_EQ(getDiagonalConfiguration2Answer(), output.str());
}

TEST_F(VoronoiTests, Build_DefaultConfiguration_1)
{
    std::set<std::tuple<std::size_t, std::size_t>> edges;

    /*
        0   1
        |   |
        2   3
    */
    edges.insert({ 0, 2 });
    edges.insert({ 1, 3 });

    VoronoiDiagram voronoi{ std::make_tuple(2, 2) };
    voronoi.build(edges);

    std::ostringstream output;
    voronoi.printVertices(std::ref(output));

    EXPECT_EQ(getDefaultConfigurationAnswer(), output.str());
}

TEST_F(VoronoiTests, Build_DefaultConfiguration_2)
{
    std::set<std::tuple<std::size_t, std::size_t>> edges;

    /*
        0 - 1
           
        2 - 3
    */
    edges.insert({ 0, 1 });
    edges.insert({ 2, 3 });

    VoronoiDiagram voronoi{ std::make_tuple(2, 2) };
    voronoi.build(edges);

    std::ostringstream output;
    voronoi.printVertices(std::ref(output));

    EXPECT_EQ(getDefaultConfigurationAnswer(), output.str());
}

TEST_F(VoronoiTests, Build_DefaultConfiguration_3)
{
    std::set<std::tuple<std::size_t, std::size_t>> edges;

    /*
        0   1
        |
        2   3
    */
    edges.insert({ 0, 2 });

    VoronoiDiagram voronoi{ std::make_tuple(2, 2) };
    voronoi.build(edges);

    std::ostringstream output;
    voronoi.printVertices(std::ref(output));

    EXPECT_EQ(getDefaultConfigurationAnswer(), output.str());
}

TEST_F(VoronoiTests, Build_DefaultConfiguration_4)
{
    std::set<std::tuple<std::size_t, std::size_t>> edges;

    /*
        0   1
            |
        2   3
    */
    edges.insert({ 1, 3 });

    VoronoiDiagram voronoi{ std::make_tuple(2, 2) };
    voronoi.build(edges);

    std::ostringstream output;
    voronoi.printVertices(std::ref(output));

    EXPECT_EQ(getDefaultConfigurationAnswer(), output.str());
}

TEST_F(VoronoiTests, Build_DefaultConfiguration_5)
{
    std::set<std::tuple<std::size_t, std::size_t>> edges;

    /*
        0 - 1
            
        2   3
    */
    edges.insert({ 0, 1 });

    VoronoiDiagram voronoi{ std::make_tuple(2, 2) };
    voronoi.build(edges);

    std::ostringstream output;
    voronoi.printVertices(std::ref(output));

    EXPECT_EQ(getDefaultConfigurationAnswer(), output.str());
}

TEST_F(VoronoiTests, Build_DefaultConfiguration_6)
{
    std::set<std::tuple<std::size_t, std::size_t>> edges;

    /*
        0   1

        2 - 3
    */
    edges.insert({ 2, 3 });

    VoronoiDiagram voronoi{ std::make_tuple(2, 2) };
    voronoi.build(edges);

    std::ostringstream output;
    voronoi.printVertices(std::ref(output));

    EXPECT_EQ(getDefaultConfigurationAnswer(), output.str());
}

TEST_F(VoronoiTests, Weld)
{
    using namespace dpa::image;
    using namespace dpa::graph;

    Image<RGB, stbi_uc> image{ m_curve };
    
    SimilarityGraph simGraph;
    simGraph.build(image);

    auto imageDims = std::make_tuple(image.getWidth(), image.getHeight());

    simGraph.applyHeuristic(heuristics::DissimilarPixels{});
    simGraph.applyHeuristic(heuristics::Curves{ imageDims });
    simGraph.applyHeuristic(heuristics::Islands{ imageDims });
    simGraph.applyHeuristic(heuristics::SparsePixels{ imageDims });

    VoronoiDiagram voronoi{ imageDims };
    voronoi.build(simGraph.getEdges());

    std::ostringstream output;
    voronoi.printVertices(std::ref(output));

    std::ifstream input{ m_curves_weld_solution };
    std::string solution{ std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>() };

    EXPECT_EQ(solution, output.str());
}
