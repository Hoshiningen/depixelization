#include <ImageUtil.h>

#include <Pixel.h>

#include <optional>

namespace dpa::image::utility
{
template<>
std::optional<Image<YCbCr, stbi_uc>> RGB_To_YCbCr(const Image<RGB, stbi_uc>& image)
{
    if (!image.isLoaded() || !image.getHeight() || !image.getWidth())
        return {};

    Image<YCbCr, stbi_uc> ret{ std::make_tuple(image.getWidth(), image.getHeight()) };

    // Transform each pixel of the source image and copy it into the ret value
    auto setResult = foreach_pixel(image, [&](internal::Point2D pos, std::optional<RGB<stbi_uc>> pixel)
        {
            const auto [R, G, B] = pixel.value();
    
            auto Y = static_cast<stbi_uc>(  0.299000 * R + 0.587000 * G + 0.114000 * B);
            auto Cb = static_cast<stbi_uc>(-0.168736 * R - 0.331264 * G + 0.500000 * B + 128);
            auto Cr = static_cast<stbi_uc>( 0.500000 * R - 0.418688 * G - 0.081312 * B + 128);
            
            return ret.setPixelAt(pos, make_pixel<YCbCr>(Y, Cb, Cr));
        });
    
    if (setResult)
        return std::make_optional(ret);

    return {};
}

template<>
std::optional<Image<RGB, stbi_uc>> YCbCr_To_RGB(const Image<YCbCr, stbi_uc>& image)
{
    if (!image.isLoaded() || !image.getHeight() || !image.getWidth())
        return {};

    Image<RGB, stbi_uc> ret{ std::make_tuple(image.getWidth(), image.getHeight()) };

    // Transform each pixel of the source image and copy it into the ret value
    auto setResult = foreach_pixel(image, [&](internal::Point2D pos, std::optional<YCbCr<stbi_uc>> pixel)
        {
            const auto [Y, Cb, Cr] = pixel.value();
    
            auto R = static_cast<stbi_uc>(Y + 1.402000 * (Cr - 128));
            auto G = static_cast<stbi_uc>(Y - 0.344136 * (Cb - 128) - 0.714136 * (Cr - 128));
            auto B = static_cast<stbi_uc>(Y + 1.772000 * (Cb - 128));
    
            return ret.setPixelAt(pos, make_pixel<RGB>(R, G, B));
        });

    if (setResult)
        return std::make_optional(ret);

    return {};
}
}
