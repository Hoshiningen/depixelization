#pragma once

#include <TestUtility.h>
#include <Voronoi.h>

#include <algorithm>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

using namespace dpa::voronoi;

struct WeldData
{
    std::size_t index;
    std::tuple<double, double> point;
};

std::istream& operator>>(std::istream& input, WeldData& data)
{
    std::size_t index = 0;
    double x = 0.0;
    double y = 0.0;

    input >> index >> x >> y;

    data.index = index;
    data.point = { x, y };

    return input;
}

class VoronoiTests : public ::testing::Test
{
protected:

    std::string getTriangleConfiguration1Answer() const noexcept
    {
        std::ostringstream out;
        out << "0: (0.500000, 0.000000)\n";
        out << "1: (0.750000, 0.250000)\n";
        out << "2: (0.000000, 0.500000)\n";
        out << "3: (0.500000, 0.500000)\n";
        out << "4: (1.000000, 0.500000)\n";
        out << "5: (0.500000, 1.000000)\n";

        return out.str();
    }

    std::string getTriangleConfiguration2Answer() const noexcept
    {
        std::ostringstream out;
        out << "0: (1.000000, 0.500000)\n";
        out << "1: (0.750000, 0.750000)\n";
        out << "2: (0.500000, 0.000000)\n";
        out << "3: (0.500000, 0.500000)\n";
        out << "4: (0.500000, 1.000000)\n";
        out << "5: (0.000000, 0.500000)\n";

        return out.str();
    }

    std::string getTriangleConfiguration3Answer() const noexcept
    {
        std::ostringstream out;
        out << "0: (0.500000, 1.000000)\n";
        out << "1: (0.250000, 0.750000)\n";
        out << "2: (1.000000, 0.500000)\n";
        out << "3: (0.500000, 0.500000)\n";
        out << "4: (0.000000, 0.500000)\n";
        out << "5: (0.500000, 0.000000)\n";

        return out.str();
    }

    std::string getTriangleConfiguration4Answer() const noexcept
    {
        std::ostringstream out;
        out << "0: (0.000000, 0.500000)\n";
        out << "1: (0.250000, 0.250000)\n";
        out << "2: (0.500000, 1.000000)\n";
        out << "3: (0.500000, 0.500000)\n";
        out << "4: (0.500000, 0.000000)\n";
        out << "5: (1.000000, 0.500000)\n";

        return out.str();
    }

    std::string getDiagonalConfiguration1Answer() const noexcept
    {
        std::ostringstream out;
        out << "0: (0.500000, 0.000000)\n";
        out << "1: (0.250000, 0.250000)\n";
        out << "2: (0.000000, 0.500000)\n";
        out << "3: (0.500000, 0.500000)\n";
        out << "4: (1.000000, 0.500000)\n";
        out << "5: (0.750000, 0.750000)\n";
        out << "6: (0.500000, 1.000000)\n";

        return out.str();
    }

    std::string getDiagonalConfiguration2Answer() const noexcept
    {
        std::ostringstream out;
        out << "0: (0.000000, 0.500000)\n";
        out << "1: (0.250000, 0.750000)\n";
        out << "2: (0.500000, 1.000000)\n";
        out << "3: (0.500000, 0.500000)\n";
        out << "4: (0.500000, 0.000000)\n";
        out << "5: (0.750000, 0.250000)\n";
        out << "6: (1.000000, 0.500000)\n";

        return out.str();
    }

    std::string getDefaultConfigurationAnswer() const noexcept
    {
        std::ostringstream out;
        out << "0: (0.500000, 0.000000)\n";
        out << "1: (0.000000, 0.500000)\n";
        out << "2: (0.500000, 0.500000)\n";
        out << "3: (1.000000, 0.500000)\n";
        out << "4: (0.500000, 1.000000)\n";

        return out.str();
    }

protected:

    // Test image paths
    std::filesystem::path m_smallImage{ "../../images/small_image.png" };
    std::filesystem::path m_bigImage{ "../../images/big_image.png" };
    std::filesystem::path m_reallyBigImage{ "../../images/really_big_image.png" };
    std::filesystem::path m_dissimilar{ "../../images/curve_test.png" };
    std::filesystem::path m_sparsePixels{ "../../images/sparse_pixels_test.png" };
    std::filesystem::path m_curve{ "../../images/curve_test.png" };
    std::filesystem::path m_islands{ "../../images/islands_test.png" };
    std::filesystem::path m_enemy_1{ "../../images/enemy_1.png" };
    std::filesystem::path m_enemy_2{ "../../images/enemy_2.png" };
    std::filesystem::path m_torch{ "../../images/torch.png" };
    std::filesystem::path m_skull{ "../../images/skull.png" };

    std::filesystem::path m_curves_weld_solution{ "../../source/reshaper/tests/data/curves_voronoi_weld_solution.txt" };

};
