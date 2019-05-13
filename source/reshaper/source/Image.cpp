#include "Image.h"

#include <private/ImageView.h>
#include <reshaper/Pixel.h>

#ifndef STB_IMAGE_IMPLEMENTATION
    #define STB_IMAGE_IMPLEMENTATION
#endif

#include <stb_image.h>

#include <algorithm>
#include <array>
#include <filesystem>
#include <memory>
#include <numeric>
#include <optional>

#include <cctype>

namespace
{
std::string to_lower(const std::string& value)
{
    return std::transform_reduce(std::cbegin(value), std::cend(value), std::string{}, std::plus<std::string>{},
        [](char letter)
        {
            return std::string(1, static_cast<char>(std::tolower(static_cast<int>(letter))));
        });
}
} // namespace

namespace dpa::image
{
struct Image::Impl
{
public:

    Impl() = default;
    Impl(const std::filesystem::path& filePath);

    ~Impl();

    bool load(const std::filesystem::path& filePath);
    bool checkExtension(const std::filesystem::path& filePath) const noexcept;

public:

    bool m_loaded{ false };
    int m_width{ 0 };
    int m_height{ 0 };
    int m_channels{ 0 };

    // internal::ImageView m_view{ {0, 0} };
    stbi_us* m_pData{ nullptr };
};

Image::Impl::Impl(const std::filesystem::path& filePath)
{
    load(filePath);
}

Image::Impl::~Impl()
{
    m_width = 0;
    m_height = 0;
    m_channels = 0;

    stbi_image_free(m_pData);
}

bool Image::Impl::load(const std::filesystem::path& filePath)
{
    if (!std::filesystem::is_regular_file(filePath) ||
        !std::filesystem::exists(filePath) ||
        !checkExtension(filePath))
    {
        return false;
    }

    m_pData = stbi_load_16(filePath.string().c_str(), &m_width, &m_height, &m_channels, STBI_rgb);

    return m_pData != nullptr;
}

bool Image::Impl::checkExtension(const std::filesystem::path& filePath) const noexcept
{
    if (!std::filesystem::is_regular_file(filePath) ||
        !std::filesystem::exists(filePath) ||
        !filePath.has_extension())
    {
        return false;
    }

    std::array<std::string, 9> extensions =
    {
        ".jpeg", ".png", ".tga", ".bmp", ".psd", ".gif", ".hdr", ".pic", ".pnm"
    };

    return std::any_of(std::cbegin(extensions), std::cend(extensions),
        [&](const auto& item)
        {
            return item == to_lower(filePath.extension().string());
        });
}

Image::Image()
    : m_pImpl(std::make_unique<Impl>())
{}

Image::Image(const std::filesystem::path& filePath)
    : m_pImpl(std::make_unique<Impl>(filePath))
{}

Image::~Image() = default;

bool Image::load(const std::filesystem::path& filePath)
{
    return m_pImpl->load(filePath);
}

bool Image::isLoaded() const noexcept
{
    return m_pImpl->m_loaded;
}

int Image::getWidth() const noexcept
{
    return m_pImpl->m_width;
}

int Image::getHeight() const noexcept
{
    return m_pImpl->m_height;
}

int Image::getChannels() const noexcept
{
    return m_pImpl->m_channels;
}

std::optional<char> Image::getPixelAt() const noexcept
{
    return {};
}
} // namespace dpa::image
