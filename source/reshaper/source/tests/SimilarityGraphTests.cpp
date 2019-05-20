#include <SimilarityGraphTests.h>

#include <Image.h>
#include <ImageUtil.h>

using namespace dpa::image;

TEST_F(SimilarityGraphTests, BuildGraph)
{
    Image<RGB, stbi_uc> testImage{ m_smallImage };
    dpa::image::utility::RGB_To_YCbCr(testImage);

    Image<YCbCr, stbi_uc> convertedImage{
        std::make_tuple(testImage.getWidth(), testImage.getHeight()) };

    // Copy the converted image
    dpa::image::utility::foreach_pixel(testImage,
        [&](const dpa::image::internal::Point2D& pos, dpa::image::RGB<stbi_uc> pixel)
        {
            return convertedImage.setPixelAt(pos, pixel);
        });

    // Build the graph from the conerted image
    m_graph.build(convertedImage);

    std::ostringstream out;
    m_graph.printGraph(std::ref(out));

    EXPECT_EQ(getSmallImageLatticeGraphAnswer(), out.str());
}
