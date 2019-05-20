#pragma once

#include <SimilarityGraph.h>

#include <gtest/gtest.h>

#include <filesystem>
#include <sstream>
#include <string>

using namespace dpa::graph;

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

protected:

    std::filesystem::path m_smallImage{ "../../images/small_image.png" };
    SimilarityGraph m_graph;

};
