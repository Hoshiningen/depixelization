#pragma once

#include <SimilarityGraph.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

using namespace dpa::graph;

/*
    A structure to hold deserialized dissimilar edge data.
    This data contains the edge, and a boolean value, which
    denotes the edge as being dissimilar or not
*/
struct DissimilarEdgeData
{
    std::tuple<std::size_t, std::size_t> edge;
    bool value;
};


/*
    A structure to hold deserialized crossing edge data.
    This data contains the edge, and a double value, which
    is the weight that value was given when the parent heuristic
    was applied to the similarity graph
*/
struct CrossingEdgeData
{
    std::tuple<std::size_t, std::size_t> edge;
    double value;
};

std::istream& operator>>(std::istream& input, DissimilarEdgeData& data)
{
    std::size_t s;
    std::size_t t;


    input >> s >> t;

    data.edge = std::make_tuple(s, t);
    data.value = true;

    return input;
}

std::istream& operator>>(std::istream& input, CrossingEdgeData& data)
{
    std::size_t s;
    std::size_t t;
    double value;

    input >> s >> t >> value;

    data.edge = std::make_tuple(s, t);
    data.value = value;

    return input;
}

class SimilarityGraphTests : public ::testing::Test
{
protected:

    std::string getSmallImageLatticeGraphAnswer() const noexcept
    {
        std::ostringstream out;
        out << "0 <--> 1 3 4 \n";
        out << "1 <--> 0 2 3 4 5 \n";
        out << "2 <--> 1 4 5 \n";
        out << "3 <--> 0 1 4 6 7 \n";
        out << "4 <--> 0 1 2 3 5 6 7 8 \n";
        out << "5 <--> 1 2 4 7 8 \n";
        out << "6 <--> 3 4 7 \n";
        out << "7 <--> 3 4 5 6 8 \n";
        out << "8 <--> 4 5 7 \n";

        return out.str();
    }

    template<typename EdgeData>
    std::vector<EdgeData> getSolutionData(const std::filesystem::path& file)
    {
        std::ifstream input{ file };
        std::vector<EdgeData> ret;

        std::copy(std::istream_iterator<EdgeData>(input), std::istream_iterator<EdgeData>(),
            std::back_inserter(ret));

        return ret;
    }

protected:

    // Test image paths
    std::filesystem::path m_smallImage{ "../../images/small_image.png" };
    std::filesystem::path m_dissimilar{ "../../images/curve_test.png" };
    std::filesystem::path m_sparsePixels{ "../../images/sparse_pixels_test.png" };
    std::filesystem::path m_curve{ "../../images/curve_test.png" };
    std::filesystem::path m_islands{ "../../images/islands_test.png" };
    std::filesystem::path m_enemy_1{ "../../images/enemy_1.png" };
    std::filesystem::path m_enemy_2{ "../../images/enemy_2.png" };
    std::filesystem::path m_torch{ "../../images/torch.png" };
    std::filesystem::path m_skull{ "../../images/skull.png" };

    // Heuristic solutions paths
    std::filesystem::path m_curves_solution{ "../../source/reshaper/source/tests/data/curves_edges_solution.txt" };
    std::filesystem::path m_dissimilar_solution{ "../../source/reshaper/source/tests/data/dissimilar_edges_solution.txt" };
    std::filesystem::path m_islands_solution{ "../../source/reshaper/source/tests/data/islands_edges_solution.txt" };
    std::filesystem::path m_sparse_pixels_solution{ "../../source/reshaper/source/tests/data/sparse_pixels_edges_solution.txt" };

    SimilarityGraph m_graph;

};
