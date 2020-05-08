#pragma once

#include <Image.h>
#include <ImageView.h>
#include <Pixel.h>

#include <array>
#include <filesystem>
#include <vector>

#include <gtest/gtest.h>
#include <stb_image.h>

namespace di = dpa::image;
namespace dii = dpa::image::internal;

// View aliases
using Grey8View = dii::ImageView<di::Y, stbi_uc>;
using Grey16View = dii::ImageView<di::Y, stbi_us>;

using GreyAlpha8View = dii::ImageView<di::YA, stbi_uc>;
using GreyAlpha16View = dii::ImageView<di::YA, stbi_us>;

using RGB8View = dii::ImageView<di::RGB, stbi_uc>;
using RGB16View = dii::ImageView<di::RGB, stbi_us>;

using RGBAlpha8View = dii::ImageView<di::RGBA, stbi_uc>;
using RGBAlpha16View = dii::ImageView<di::RGBA, stbi_us>;

class ImageViewTests : public ::testing::Test
{
protected:

    void SetUp() override
    {
        auto components = 0;

        m_8bitData =
        {
            stbi_load(m_imagePath.string().c_str(), &m_width, &m_height, &components, 1),
            stbi_load(m_imagePath.string().c_str(), &m_width, &m_height, &components, 2),
            stbi_load(m_imagePath.string().c_str(), &m_width, &m_height, &components, 3),
            stbi_load(m_imagePath.string().c_str(), &m_width, &m_height, &components, 4),
        };

        m_16bitData =
        {
            stbi_load_16(m_imagePath.string().c_str(), &m_width, &m_height, &components, 1),
            stbi_load_16(m_imagePath.string().c_str(), &m_width, &m_height, &components, 2),
            stbi_load_16(m_imagePath.string().c_str(), &m_width, &m_height, &components, 3),
            stbi_load_16(m_imagePath.string().c_str(), &m_width, &m_height, &components, 4),
        };
    }

    void TearDown() override
    {
        for (auto data : m_8bitData)
            stbi_image_free(data);

        for (auto data : m_16bitData)
            stbi_image_free(data);
    }

    std::vector<std::vector<dpa::image::RGB<stbi_uc>>> CurveTestPixelArray() const noexcept
    {
        using namespace di;
        auto White = make_pixel<RGB>(255_uc, 255_uc, 255_uc);
        auto Black = make_pixel<RGB>(0_uc, 0_uc, 0_uc);

        return std::vector<std::vector<dpa::image::RGB<stbi_uc>>>
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

protected:
    
    // Image dimensions (w * h)
    int m_width{ 0 };
    int m_height{ 0 };

    std::filesystem::path m_imagePath{ "../../images/curve_test.png" };

    // Data pointers for loaded views
    // progresses from grey to rbgalpha for each bit depth
    std::array<stbi_uc*, 4> m_8bitData{};
    std::array<stbi_us*, 4> m_16bitData{};
};
