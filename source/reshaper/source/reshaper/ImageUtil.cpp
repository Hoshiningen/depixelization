#include <ImageUtil.h>

#include <Pixel.h>

#include <optional>

namespace dpa::image::utility
{
template<>
bool RGB_To_YCbCr(Image<RGB, stbi_uc>& image)
{
    if (!image.isLoaded() || !image.getHeight() || !image.getWidth())
        return false;

    return foreach_pixel_mutable(image, [](std::optional<RGB<stbi_uc>> pixel)
        {
            const auto [R, G, B] = pixel.value();

            stbi_uc Y = static_cast<stbi_uc>(  0.299000 * R + 0.587000 * G + 0.114000 * B);
            stbi_uc Cb = static_cast<stbi_uc>(-0.168736 * R - 0.331264 * G + 0.500000 * B + 128);
            stbi_uc Cr = static_cast<stbi_uc>( 0.500000 * R - 0.418688 * G - 0.081312 * B + 128);

            return make_pixel<YCbCr>(Y, Cb, Cr);
        });
}

template<>
bool YCbCr_To_RGB(Image<YCbCr, stbi_uc>& image)
{
    if (!image.isLoaded() || !image.getHeight() || !image.getWidth())
        return false;

    return foreach_pixel_mutable(image, [](std::optional<YCbCr<stbi_uc>> pixel)
        {
            const auto [Y, Cb, Cr] = pixel.value();

            stbi_uc R = static_cast<stbi_uc>(Y + 1.402000 * (Cr - 128));
            stbi_uc G = static_cast<stbi_uc>(Y - 0.344136 * (Cb - 128) - 0.714136 * (Cr - 128));
            stbi_uc B = static_cast<stbi_uc>(Y + 1.772000 * (Cb - 128));

            return make_pixel<RGB>(R, G, B);
        });
}
}
