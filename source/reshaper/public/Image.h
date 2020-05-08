
#pragma once

#include <FileUtil.h>
#include <ImageView.h>
#include <Pixel.h>

#include <algorithm>
#include <array>
#include <filesystem>
#include <map>
#include <numeric>
#include <optional>
#include <string>
#include <tuple>
#include <variant>

#include <stdlib.h>

#include <stb_image_write.h>

namespace dpa::image
{
/*
    Represents a bitmap image, and supports getting and setting
    pixels, and reading and writing images of various formats
*/
template<template<typename> class Channels, typename BitDepth>
class Image final
{
public:

    Image() = default;
    Image(const std::filesystem::path& filePath);
    Image(const internal::Point2D& dimensions);

    Image(const Image& other);
    Image& operator=(const Image& other);

    ~Image();

    bool save(const std::filesystem::path& destPath) const noexcept;

    bool isLoaded() const noexcept;

    int getWidth() const noexcept;
    int getHeight() const noexcept;
    int getChannels() const noexcept;

    std::optional<Channels<BitDepth>> getPixelAt(const internal::Point2D&) const;
    bool setPixelAt(const internal::Point2D& position, const Channels<BitDepth>& pixel);

private:

    void createImageView(const internal::Point2D& dimensions);

    std::optional<std::tuple<int, int>> load(const std::filesystem::path& filePath);
    BitDepth* loadImpl(const std::filesystem::path& filePath, int& width, int& height, int& channels);

    struct png_write_tag{};
    struct bmp_write_tag{};
    struct tga_write_tag{};
    struct jpg_write_tag{};
    struct hdr_write_tag{};

    using TagVariant = std::variant<png_write_tag, bmp_write_tag,
        tga_write_tag, jpg_write_tag, hdr_write_tag>;

    bool saveImpl(const std::filesystem::path& filePath,TagVariant variant) const noexcept;
 
private:

    bool m_loaded{ false };

    BitDepth* m_pData{ nullptr };
    internal::ImageView<Channels, BitDepth> m_view;
};

/*
    Constructs a new image object from the given file path. The
    image at the file path will be decoded and its data will be
    stored in memory. If an invalid file path is given, then
    the image will not be loaded

    @param filePath The path to the image to load
*/
template<template<typename> class Channels, typename BitDepth>
Image<Channels, BitDepth>::Image(const std::filesystem::path& filePath)
{
    if (auto imageAttributes = load(filePath); imageAttributes)
    {
        m_loaded = true;
        m_view = internal::ImageView<Channels, BitDepth>{ m_pData, imageAttributes.value() };
    }
}

/*
    Constructs a new image of the given dimensions, with the specified
    number of channels and bit depth

    @param dimensions The width and height of the new image
*/
template<template<typename> class Channels, typename BitDepth>
Image<Channels, BitDepth>::Image(const internal::Point2D& dimensions)
{
    createImageView(dimensions);
}

template<template<typename> class Channels, typename BitDepth>
Image<Channels, BitDepth>::Image(const Image& other)
    : Image(std::make_tuple(other.getWidth(), other.getHeight()))
{
    *this = other;
}

template<template<typename> class Channels, typename BitDepth>
Image<Channels, BitDepth>& Image<Channels, BitDepth>::operator=(const Image& other)
{
    if (this == &other)
        return *this;

    m_loaded = other.isLoaded();
    createImageView(std::make_tuple(other.getWidth(), other.getHeight()));

    for (auto h = 0; h < other.getHeight(); ++h)
    {
        for (auto w = 0; w < other.getWidth(); ++w)
        {
            if (auto pixel = other.getPixelAt({ w, h }); pixel)
            {
                if (!setPixelAt({ w, h }, pixel.value()))
                    throw std::runtime_error("Could not set the source image's pixels.");
            }
        }
    }

    return *this;
}

/*
    Destructor
*/
template<template<typename> class Channels, typename BitDepth>
Image<Channels, BitDepth>::~Image()
{
    stbi_image_free(m_pData);
}

/*
    Saves an image to disk

    @param destPath The file name to save the image as. This has to be a
                    valid path to a file, and may or may not already exist
    @returns True if the image was saved, false otherwise
*/
template<template<typename> class Channels, typename BitDepth>
bool Image<Channels, BitDepth>::save(const std::filesystem::path& destPath) const noexcept
{
    static std::array<std::string, 5> extensions =
    {
        ".jpeg", ".png", ".tga", ".bmp", ".hdr"
    };

    static std::map<std::string, TagVariant> writeTags =
    {
        { ".jpeg", jpg_write_tag{} },
        { ".png",  png_write_tag{} },
        { ".tga",  tga_write_tag{} },
        { ".bmp",  bmp_write_tag{} },
        { ".hdr",  hdr_write_tag{} },
    };

    if (!dpa::fileutil::isValidImageExtension(destPath, extensions))
        return false;

    return saveImpl(destPath, writeTags[destPath.extension().string()]);
}

/*
    Loads the image at the given filepath

    @param filePath The path to the image to load
    @returns An valid optional containing the images dimensions, an empty optional otherwise
*/
template<template<typename> class Channels, typename BitDepth>
std::optional<std::tuple<int, int>> Image<Channels, BitDepth>::load(const std::filesystem::path& filePath)
{
    if (!dpa::fileutil::isValidImage(filePath))
        return {};

    int width = 0;
    int height = 0;
    int channels = 0;

    m_pData = loadImpl(filePath, width, height, channels);

    if (m_pData != nullptr)
        return {{ width, height }};
    else
        return {};
}

/*
    Gets the loaded status of the image

    @returns True if the image was loaded, false otherwise
*/
template<template<typename> class Channels, typename BitDepth>
bool Image<Channels, BitDepth>::isLoaded() const noexcept
{
    return m_loaded;
}

/*
    Gets the width of the image

    @returns The width of the image
*/
template<template<typename> class Channels, typename BitDepth>
int Image<Channels, BitDepth>::getWidth() const noexcept
{
    return m_view.getWidth();
}

/*
    Gets the height of the iamge

    @returns The height of the image
*/
template<template<typename> class Channels, typename BitDepth>
int Image<Channels, BitDepth>::getHeight() const noexcept
{
    return m_view.getHeight();
}

/*
    Gets the number of channels in the image

    @returns The number of channels in the image
*/
template<template<typename> class Channels, typename BitDepth>
int Image<Channels, BitDepth>::getChannels() const noexcept
{
    return m_view.getChannels();
}

/*
    Gets the pixel at the specified location

    @param position The position of the pixel to retrieve
    @returns A valid optional containing the retrieved pixel if successful,
             an empty optional otherwise
*/
template<template<typename> class Channels, typename BitDepth>
std::optional<Channels<BitDepth>> Image<Channels, BitDepth>::getPixelAt(const internal::Point2D& position) const
{
    return m_view.getPixelAt(position);
}

/*
    Sets a pixel at the given position in the image

    @param position The position of the pixel to set
    @param pixel The data of the pixel being set
    @returns True if the pixel was set, false otherwise
*/
template<template<typename> class Channels, typename BitDepth>
bool Image<Channels, BitDepth>::setPixelAt(const internal::Point2D& position, const Channels<BitDepth>& pixel)
{
    return m_view.setPixelAt(position, pixel);
}

template<template<typename> class Channels, typename BitDepth>
void Image<Channels, BitDepth>::createImageView(const internal::Point2D& dimensions)
{
    const auto [width, height] = dimensions;
    const auto dataSize = width * height * channel_count_v<Channels>;

    m_pData = reinterpret_cast<BitDepth*>(calloc(dataSize, sizeof(BitDepth)));
    m_view = internal::ImageView<Channels, BitDepth>{ m_pData, dimensions };
}

/*
    This calls the correct stbi_write function based on the tag packed into the
    variant

    @param destPath The path to save the file at. This has to be either an existing
                    or new path to a regular file.
    @param tagVariant A variant containing the tag of the save method
    @returns True if the image was saved, false otherwise
*/
template<template<typename> class Channels, typename BitDepth>
bool Image<Channels, BitDepth>::saveImpl(const std::filesystem::path& destPath, TagVariant tagVariant) const noexcept
{
    return std::visit([&](auto&& tag)
        {
            using TagType = std::decay_t<decltype(tag)>;

            if constexpr (std::is_same_v<TagType, png_write_tag>)
            {
                return stbi_write_png(destPath.string().c_str(), getWidth(),
                    getHeight(), getChannels(), m_pData, 0);
            }
            else if constexpr (std::is_same_v<TagType, bmp_write_tag>)
            {
                return stbi_write_bmp(destPath.string().c_str(), getWidth(),
                    getHeight(), getChannels(), m_pData);
            }
            else if constexpr (std::is_same_v<TagType, tga_write_tag>)
            {
                return stbi_write_tga(destPath.string().c_str(), getWidth(),
                    getHeight(), getChannels(), m_pData);
            }
            else if constexpr (std::is_same_v<TagType, jpg_write_tag>)
            {
                return stbi_write_jpg(destPath.string().c_str(), getWidth(),
                    getHeight(), getChannels(), m_pData, 100);
            }
            else if constexpr (std::is_same_v<TagType, hdr_write_tag>)
            {
                if (std::is_same_v<decltype(m_pData), float*>)
                {
                    return stbi_write_hdr(destPath.string().c_str(), getWidth(),
                        getHeight(), getChannels(), reinterpret_cast<const float*>(m_pData));
                }

                return 0;
            }

        }, tagVariant);
}
}