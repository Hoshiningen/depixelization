#include <Image.h>

#ifndef STB_IMAGE_IMPLEMENTATION
    #define STB_IMAGE_IMPLEMENTATION
#endif

#include <stb_image.h>

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
    #define STBI_MSC_SECURE_CRT
    #define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#include <stb_image_write.h>

namespace dpa::image
{
/*
    The following overload specializations are defined so that we can load images
    with any bit depth / channel combination that's supported by stb_image.
    It's a compilation error to give a bit depth or channel type that's not used in
    these overload specializations
*/

// Grey specializations
template<>
stbi_uc* Image<Y, stbi_uc>::loadImpl(const std::filesystem::path& filePath,
                                     int& width, int& height, int& channels)
{
    return stbi_load(filePath.string().c_str(), &width, &height, &channels, STBI_grey);
}

template<>
stbi_us* Image<Y, stbi_us>::loadImpl(const std::filesystem::path& filePath,
                                     int& width, int& height, int& channels)
{
    return stbi_load_16(filePath.string().c_str(), &width, &height, &channels, STBI_grey);
}

template<>
float* Image<Y, float>::loadImpl(const std::filesystem::path& filePath,
                                 int& width, int& height, int& channels)
{
    return stbi_loadf(filePath.string().c_str(), &width, &height, &channels, STBI_grey);
}

// Grey alpha specializations
template<>
stbi_uc* Image<YA, stbi_uc>::loadImpl(const std::filesystem::path& filePath,
                                      int& width, int& height, int& channels)
{
    return stbi_load(filePath.string().c_str(), &width, &height, &channels, STBI_grey_alpha);
}

template<>
stbi_us* Image<YA, stbi_us>::loadImpl(const std::filesystem::path& filePath,
                                      int& width, int& height, int& channels)
{
    return stbi_load_16(filePath.string().c_str(), &width, &height, &channels, STBI_grey_alpha);
}

template<>
float* Image<YA, float>::loadImpl(const std::filesystem::path& filePath,
                                  int& width, int& height, int& channels)
{
    return stbi_loadf(filePath.string().c_str(), &width, &height, &channels, STBI_grey_alpha);
}

// RGB specializations
template<>
stbi_uc* Image<RGB, stbi_uc>::loadImpl(const std::filesystem::path& filePath,
                                       int& width, int& height, int& channels)
{
    return stbi_load(filePath.string().c_str(), &width, &height, &channels, STBI_rgb);
}

template<>
stbi_us* Image<RGB, stbi_us>::loadImpl(const std::filesystem::path& filePath,
                                       int& width, int& height, int& channels)
{
    return stbi_load_16(filePath.string().c_str(), &width, &height, &channels, STBI_rgb);
}

template<>
float* Image<RGB, float>::loadImpl(const std::filesystem::path& filePath,
                                   int& width, int& height, int& channels)
{
    return stbi_loadf(filePath.string().c_str(), &width, &height, &channels, STBI_rgb);
}

// YCbCr specializations
template<>
stbi_uc* Image<YCbCr, stbi_uc>::loadImpl(const std::filesystem::path& filePath,
                                       int& width, int& height, int& channels)
{
    return stbi_load(filePath.string().c_str(), &width, &height, &channels, STBI_rgb);
}

template<>
stbi_us* Image<YCbCr, stbi_us>::loadImpl(const std::filesystem::path& filePath,
                                       int& width, int& height, int& channels)
{
    return stbi_load_16(filePath.string().c_str(), &width, &height, &channels, STBI_rgb);
}

template<>
float* Image<YCbCr, float>::loadImpl(const std::filesystem::path& filePath,
                                   int& width, int& height, int& channels)
{
    return stbi_loadf(filePath.string().c_str(), &width, &height, &channels, STBI_rgb);
}


// RGB alpha specializations
template<>
stbi_uc* Image<RGBA, stbi_uc>::loadImpl(const std::filesystem::path& filePath,
                                        int& width, int& height, int& channels)
{
    return stbi_load(filePath.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
}

template<>
stbi_us* Image<RGBA, stbi_us>::loadImpl(const std::filesystem::path& filePath,
                                        int& width, int& height, int& channels)
{
    return stbi_load_16(filePath.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
}

template<>
float* Image<RGBA, float>::loadImpl(const std::filesystem::path& filePath,
                                    int& width, int& height, int& channels)
{
    return stbi_loadf(filePath.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
}
}