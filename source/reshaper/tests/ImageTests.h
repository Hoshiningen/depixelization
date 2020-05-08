#pragma once

#include <Pixel.h>

#include <filesystem>
#include <vector>

#include <gtest/gtest.h>

using namespace dpa::image;

class ImageTests : public ::testing::Test
{
protected:

    void TearDown() override
    {
        if (std::filesystem::exists(m_blankPath))
            std::filesystem::remove(m_blankPath);
    }

    std::vector<std::vector<RGB<stbi_uc>>> CurveTestPixelArray() const noexcept
    {
        auto White = make_pixel<RGB>(255_uc, 255_uc, 255_uc);
        auto Black = make_pixel<RGB>(0_uc, 0_uc, 0_uc);

        return std::vector<std::vector<RGB<stbi_uc>>>
        {
            { White, White, White, White, White, White },
            { White, White, Black, Black, White, White },
            { White, Black, White, White, Black, White },
            { White, Black, White, White, Black, White },
            { White, White, White, White, Black, White },
            { White, White, White, White, Black, White },
            { White, White, White, White, White, White },
        };
    }

    std::vector<std::vector<RGB<stbi_uc>>> NewImagePixelConfiguration() const noexcept
    {
        auto deathKnight = make_pixel<RGB>(196_uc, 31_uc, 59_uc);
        auto demonHunter = make_pixel<RGB>(163_uc, 48_uc, 201_uc);
        auto druid = make_pixel<RGB>(255_uc, 125_uc, 10_uc);
        auto hunter = make_pixel<RGB>(171_uc, 212_uc, 115_uc);
        auto mage = make_pixel<RGB>(64_uc, 199_uc, 235_uc);
        auto monk = make_pixel<RGB>(0_uc, 255_uc, 150_uc);
        auto paladin = make_pixel<RGB>(245_uc, 140_uc, 186_uc);
        auto priest = make_pixel<RGB>(255_uc, 255_uc, 255_uc);
        auto rogue = make_pixel<RGB>(255_uc, 245_uc, 105_uc);
        auto shaman = make_pixel<RGB>(0_uc, 112_uc, 222_uc);
        auto warlock = make_pixel<RGB>(135_uc, 135_uc, 237_uc);
        auto warrior = make_pixel<RGB>(199_uc, 156_uc, 110_uc);

        return std::vector<std::vector<RGB<stbi_uc>>>
        {
            { deathKnight, demonHunter, druid },
            { hunter, mage, monk },
            { paladin, priest, rogue },
            { shaman, warlock, warrior },
        };
    }

protected:

    std::filesystem::path m_imagePath{ "../../images/curve_test.png" };
    std::filesystem::path m_blankPath{ "../../images/newImage.png" };
};
