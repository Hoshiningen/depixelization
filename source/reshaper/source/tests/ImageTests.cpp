#include <ImageTests.h>

TEST_F(ImageTests, WriteNewImage)
{
    auto pixelConfig = NewImagePixelConfiguration();
    auto imageDims = std::make_tuple(3, 4);

    Image<RGB, stbi_uc> newImage(imageDims);

    // Write the image based on our pixel configuration
    for (auto h = 0; h < pixelConfig.size(); ++h)
    {
        for (auto w = 0; w < pixelConfig[0].size(); ++w)
        {
            newImage.setPixelAt({ w, h }, pixelConfig[h][w]);
        }
    }

    newImage.save(m_blankPath);

    Image<RGB, stbi_uc> loadedImage(m_blankPath);

    ASSERT_TRUE(loadedImage.isLoaded());
    ASSERT_EQ(loadedImage.getChannels(), 3);
    ASSERT_EQ(loadedImage.getHeight(), 4);
    ASSERT_EQ(loadedImage.getWidth(), 3);

    // Compare the loaded image with what we actually wrote
    for (auto h = 0; h < pixelConfig.size(); ++h)
    {
        for (auto w = 0; w < pixelConfig[0].size(); ++w)
        {
            if (auto pixel = newImage.getPixelAt({ w, h }); pixel)
                ASSERT_EQ(pixel.value(), pixelConfig[h][w]);
            else
                FAIL();
        }
    }
}

TEST_F(ImageTests, Load_InvalidPath_Directory)
{
    Image<RGB, stbi_uc> image{ "../../images" };
    
    EXPECT_FALSE(image.isLoaded());
}

TEST_F(ImageTests, Load_InvalidPath_NoExtension)
{
    Image<RGB, stbi_uc> image{ "../../images/curve_test" };

    EXPECT_FALSE(image.isLoaded());
}

TEST_F(ImageTests, Load_InvalidPath_NonSupportedExtension)
{
    Image<RGB, stbi_uc> image{ "../../images/curve_test.tiff" };

    EXPECT_FALSE(image.isLoaded());
}

TEST_F(ImageTests, Save_InvalidPath_Directory)
{
    Image<RGB, stbi_uc> image{ m_imagePath };

    ASSERT_TRUE(image.isLoaded());
    EXPECT_FALSE(image.save("../../image"));
}

TEST_F(ImageTests, Save_InvalidPath_NoExtension)
{
    Image<RGB, stbi_uc> image{ m_imagePath };

    ASSERT_TRUE(image.isLoaded());
    EXPECT_FALSE(image.save("../../images/newImage"));
}

TEST_F(ImageTests, Save_InvalidPath_NonSupportedExtension)
{
    Image<RGB, stbi_uc> image{ m_imagePath };

    ASSERT_TRUE(image.isLoaded());
    EXPECT_FALSE(image.save("../../images/newImage.tiff"));
}
