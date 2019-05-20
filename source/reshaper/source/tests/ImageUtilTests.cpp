#include <ImageUtilTests.h>

#include <Image.h>
#include <ImageUtil.h>

using namespace dpa::image::utility;

TEST_F(ImageUtilTests, YCbCr_To_RGB_RoundTrip)
{
    Image<RGB, stbi_uc> baseImage{ m_imagePath };
    ASSERT_TRUE(baseImage.isLoaded());

    // Convert the image to YCbCr color space
    auto converted = RGB_To_YCbCr(baseImage);
    ASSERT_TRUE(converted);


    // Save it so we can visualize
    std::filesystem::path YCbCrPath = "../../images/enemy_2_YCbCr.png";
    if (converted)
        baseImage.save(YCbCrPath);

    Image<YCbCr, stbi_uc> baseImage_YCbCr{ "../../images/enemy_2_YCbCr.png" };
    ASSERT_TRUE(baseImage_YCbCr.isLoaded());
    
    // Convert the loaded YCbCr back to RGB color space
    auto converted2 = YCbCr_To_RGB(baseImage_YCbCr);
    ASSERT_TRUE(converted2);

    // Save it so we can visualize
    std::filesystem::path RGBPath = "../../images/enemy_2_rgb.png";
    if (converted2)
        baseImage_YCbCr.save(RGBPath);


    // Compare the converted RGB image with the original
    Image<RGB, stbi_uc> baseImage_RGB(RGBPath);
    Image<RGB, stbi_uc> original{ m_imagePath };

    ASSERT_TRUE(baseImage_RGB.isLoaded());
    ASSERT_EQ(baseImage_RGB.getChannels(), baseImage.getChannels());
    ASSERT_EQ(baseImage_RGB.getHeight(), baseImage.getHeight());
    ASSERT_EQ(baseImage_RGB.getWidth(), baseImage.getWidth());

    // Compare the loaded image with what we actually wrote
    for (auto h = 0; h < baseImage_RGB.getHeight(); ++h)
    {
        for (auto w = 0; w < baseImage_RGB.getWidth(); ++w)
        {
            if (auto pixel = baseImage_RGB.getPixelAt({ w, h }); pixel)
            {
                constexpr auto pixelTollerance = 1;
                auto inTolleranceRange = [](auto a, auto b, auto tollerance)
                {
                    return a <= (b + tollerance) || a >= (b - tollerance);
                };
                
                const auto [pR, pG, pB] = pixel.value();
                const auto [oR, oG, oB] = original.getPixelAt({ w, h }).value();
                
                ASSERT_TRUE(inTolleranceRange(pR, oR, pixelTollerance));
                ASSERT_TRUE(inTolleranceRange(pG, oG, pixelTollerance));
                ASSERT_TRUE(inTolleranceRange(pB, oB, pixelTollerance));
            }
            else
                FAIL();
        }
    }

    if (std::filesystem::exists(RGBPath))
        std::filesystem::remove(RGBPath);

    if (std::filesystem::exists(YCbCrPath))
        std::filesystem::remove(YCbCrPath);
}
