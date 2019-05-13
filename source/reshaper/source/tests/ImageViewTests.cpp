#include <ImageViewTests.h>

#include <tuple>

TEST_F(ImageViewTests, GetPixelValidPosition)
{
    RGB8View view{ m_8bitData[2], { m_width, m_height } };
    auto pixelArray = CurveTestPixelArray();

    for (auto h = 0; h < m_height; ++h)
    {
        for (auto w = 0; w < m_width; ++w)
        {
            auto pixel = view.getPixelAt({ w, h });
            ASSERT_TRUE(pixel);

            // A = actual, E = expected
            const auto [red_A, green_A, blue_A] = pixel.value();
            const auto [red_E, green_E, blue_E] = pixelArray[h][w];
            
            EXPECT_EQ(red_A, red_E);
            EXPECT_EQ(green_A, blue_E);
            EXPECT_EQ(blue_A, blue_E);
        }
    }
}

TEST_F(ImageViewTests, GetPixelInvalidPositionNegativeX)
{
    RGB8View view{ m_8bitData[2], { m_width, m_height } };
    EXPECT_FALSE(view.getPixelAt({ -1, 0 }));
}

TEST_F(ImageViewTests, GetPixelInvalidPositionNegativeY)
{
    RGB8View view{ m_8bitData[2], { m_width, m_height } };
    EXPECT_FALSE(view.getPixelAt({ 0, -1 }));
}

TEST_F(ImageViewTests, GetPixelInvalidPositionNegativeXY)
{
    RGB8View view{ m_8bitData[2], { m_width, m_height } };
    EXPECT_FALSE(view.getPixelAt({ -1, -1 }));
}

TEST_F(ImageViewTests, GetPixelInvalidPositionOutOfBoundsX)
{
    RGB8View view{ m_8bitData[2], { m_width, m_height } };
    EXPECT_FALSE(view.getPixelAt({ m_width + 1, 0 }));
}

TEST_F(ImageViewTests, GetPixelInvalidPositionOutOfBoundsY)
{
    RGB8View view{ m_8bitData[2], { m_width, m_height } };
    EXPECT_FALSE(view.getPixelAt({ 0, m_height + 1 }));
}

TEST_F(ImageViewTests, GetPixelInvalidPositionOutOfBoundsXY)
{
    RGB8View view{ m_8bitData[2], { m_width, m_height } };
    EXPECT_FALSE(view.getPixelAt({ m_width + 1, m_height + 1 }));
}

TEST_F(ImageViewTests, SetPixelValidPosition)
{
    RGB8View view{ m_8bitData[2], { m_width, m_height } };

    auto testPixel = std::make_tuple<stbi_uc, stbi_uc, stbi_uc>(1, 2, 3);
    auto result = view.setPixelAt({ 0, 0 }, testPixel);
    ASSERT_TRUE(result);

    auto pixel = view.getPixelAt({ 0, 0 });
    ASSERT_TRUE(pixel);

    const auto [red, green, blue] = pixel.value();
    EXPECT_EQ(red, 1);
    EXPECT_EQ(green, 2);
    EXPECT_EQ(blue, 3);
}

TEST_F(ImageViewTests, SetPixelInvalidPositionNegativeX)
{
    RGB8View view{ m_8bitData[2], { m_width, m_height } };

    auto testPixel = std::make_tuple<stbi_uc, stbi_uc, stbi_uc>(1, 2, 3);
    EXPECT_FALSE(view.setPixelAt({ -1, 0 }, testPixel));
}

TEST_F(ImageViewTests, SetPixelInvalidPositionNegativeY)
{
    RGB8View view{ m_8bitData[2], { m_width, m_height } };

    auto testPixel = std::make_tuple<stbi_uc, stbi_uc, stbi_uc>(1, 2, 3);
    EXPECT_FALSE(view.setPixelAt({ 0, -1 }, testPixel));
}

TEST_F(ImageViewTests, SetPixelInvalidPositionNegativeXY)
{
    RGB8View view{ m_8bitData[2], { m_width, m_height } };

    auto testPixel = std::make_tuple<stbi_uc, stbi_uc, stbi_uc>(1, 2, 3);
    EXPECT_FALSE(view.setPixelAt({ -1, -1 }, testPixel));
}

TEST_F(ImageViewTests, SetPixelInvalidPositionOutOfBoundsX)
{
    RGB8View view{ m_8bitData[2], { m_width, m_height } };

    auto testPixel = std::make_tuple<stbi_uc, stbi_uc, stbi_uc>(1, 2, 3);
    EXPECT_FALSE(view.setPixelAt({ m_width + 1, 0 }, testPixel));
}

TEST_F(ImageViewTests, SetPixelInvalidPositionOutOfBoundsY)
{
    RGB8View view{ m_8bitData[2], { m_width, m_height } };

    auto testPixel = std::make_tuple<stbi_uc, stbi_uc, stbi_uc>(1, 2, 3);
    EXPECT_FALSE(view.setPixelAt({ 0, m_height + 1 }, testPixel));
}

TEST_F(ImageViewTests, SetPixelInvalidPositionOutOfBoundsXY)
{
    RGB8View view{ m_8bitData[2], { m_width, m_height } };

    auto testPixel = std::make_tuple<stbi_uc, stbi_uc, stbi_uc>(1, 2, 3);
    EXPECT_FALSE(view.setPixelAt({ m_width + 1, m_height + 1 }, testPixel));
}