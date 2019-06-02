#pragma once

#include <Pixel.h>

#include <stb_image.h>

#include <array>
#include <optional>
#include <stdexcept>
#include <tuple>

namespace dpa::image::internal
{
using Point2D = std::tuple<int, int>;

/*
    Represents a view for an image loaded by stb. Provides
    an interface for pixel I/O
*/
template<template<typename> class Channels, typename BitDepth>
class ImageView final
{
public:

    ImageView() = default;
    ImageView(BitDepth* pixelData, const Point2D& dimensions);

    std::optional<Channels<BitDepth>> getPixelAt(const Point2D& position) const noexcept;
    bool setPixelAt(const Point2D& position, const Channels<BitDepth>& pixel) noexcept;

    BitDepth* getData() const noexcept;

    int getChannels() const noexcept;
    int getHeight() const noexcept;
    int getWidth() const noexcept;

private:

    bool checkDimensions(const Point2D& given, const Point2D& actual) const noexcept;
    BitDepth* seekTo(const Point2D& position) const noexcept;

    template<typename Array, std::size_t... Index>
    auto array_to_tuple_impl(const Array& array, std::index_sequence<Index...>) const noexcept;

    template<typename Indices = std::make_index_sequence<channel_count_v<Channels>>>
    auto ArrayToTuple(const std::array<BitDepth, channel_count_v<Channels>> & array) const noexcept;

    template<std::size_t... I> 
    auto tuple_to_array_impl(const Channels<BitDepth>& pixel, std::index_sequence<I...>) const noexcept;

    template<typename... Rest>
    auto TupleToArray(const std::tuple<BitDepth, Rest...>& tuple) const noexcept;

private:

    int m_width{ 0 };
    int m_height{ 0 };
    int m_channels{ 0 };

    BitDepth* m_pData{ nullptr };
};

/*
    Constructs a new image view. This requires that an image as been loaded
    successfully, and that its memory pointer is passed in as a non-nullptr value

    @param pixelData A pointer to the loaded image data
    @param dimensions The image's dimensions
*/
template<template<typename> class Channels, typename BitDepth>
ImageView<Channels, BitDepth>::ImageView(BitDepth* pixelData, const Point2D& dimensions)
    : m_channels(channel_count_v<Channels>), m_pData(pixelData)
{
    std::tie(m_width, m_height) = dimensions;

    if (!m_pData || m_width <= 0 || m_height <= 0)
        throw std::invalid_argument("Invalid arguments given to the view.");
}

/*
    Gets the pixel at the desired location

    @param position The position to retrive the pixel from
    @returns A valid optional containing the desired pixel data,
             if the desired position was within the bounds of the image. An
             empty optional otherwise          
*/
template<template<typename> class Channels, typename BitDepth>
std::optional<Channels<BitDepth>> ImageView<Channels, BitDepth>::getPixelAt(const Point2D& position) const noexcept
{
    if (!checkDimensions(position, { m_width, m_height }))
        return {};

    auto pixelView = Channels<BitDepth>{};
    BitDepth* pixelOffset = seekTo(position);

    std::array<unsigned char, channel_count_v<Channels>> tempArray;
    std::copy(pixelOffset, pixelOffset + m_channels, std::begin(tempArray));

    return { ArrayToTuple(tempArray) };
}

/*
    Sets the given pixel at the desired location

    @param position The position to set the pixel at
    @param pixel The pixel containing the data to set
    @returns True if the data was set, false otherwise
*/
template<template<typename> class Channels, typename BitDepth>
bool ImageView<Channels, BitDepth>::setPixelAt(const Point2D& position,
                                               const Channels<BitDepth>& pixel) noexcept
{
    if (!checkDimensions(position, { m_width, m_height }))
        return false;

    BitDepth* pixelOffset = seekTo(position);

    auto componentsArray = TupleToArray(pixel);
    std::copy(std::cbegin(componentsArray), std::cend(componentsArray), pixelOffset);

    return true;
}

/*
    Gets the pixel data that was stored within the view
*/
template<template<typename> class Channels, typename BitDepth>
BitDepth* ImageView<Channels, BitDepth>::getData() const noexcept
{
    return m_pData;
}

/*
    Gets the number of channels in the image
*/
template<template<typename> class Channels, typename BitDepth>
int ImageView<Channels, BitDepth>::getChannels() const noexcept
{
    return m_channels;
}

/*
    Gets the image height
*/
template<template<typename> class Channels, typename BitDepth>
int ImageView<Channels, BitDepth>::getHeight() const noexcept
{
    return m_height;
}

/*
    Gets the image width
*/
template<template<typename> class Channels, typename BitDepth>
int ImageView<Channels, BitDepth>::getWidth() const noexcept
{
    return m_width;
}

/*
    Checks that corresponding values in given are <= to the values in actual

    @param given The value we're testing
    @param actual The value we're comparing against
    @returns True if given is <= actual
*/
template<template<typename> class Channels, typename BitDepth>
bool ImageView<Channels, BitDepth>::checkDimensions(const Point2D& given, const Point2D& expected) const noexcept
{
    const auto [x1, y1] = given;
    const auto [x2, y2] = expected;

    auto isValid = [](auto a, auto b)
    {
        return 0 <= a && a <= b;
    };

    return isValid(x1, x2) && isValid(y1, y2);
}

/*
    Seeks to the desired position in the pixel data

    @param position The position we want to find in the pixel data
    @returns A pointer to the first channel of the desired pixel
*/
template<template<typename> class Channels, typename BitDepth>
BitDepth* ImageView<Channels, BitDepth>::seekTo(const Point2D& position) const noexcept
{
    const auto [x, y] = position;

    // | row |  *  |     row size     |  +  | column offset |
    //    y     *  [(channels) * width]  +    (channels) * x
    return m_pData + m_channels * ((y * m_width) + x);
}

/*
    The implementation of the array to tuple method. This creates
    the tuple by expanding the array sequence with the packed indices

    @param array The array we want to convert into a tuple
    @returns A tuple containing every element in the given array
*/
template<template<typename> class Channels, typename BitDepth>
template<typename Array, std::size_t... Index>
auto ImageView<Channels, BitDepth>::array_to_tuple_impl(const Array& array, std::index_sequence<Index...>) const noexcept
{
    return std::make_tuple(array[Index]...);
}

/*
    Converts an array into a tuple

    @param array The array we want to convert into a tuple
    @returns A tuple containing all elements that were in the array
*/
template<template<typename> class Channels, typename BitDepth>
template<typename Indices>
auto ImageView<Channels, BitDepth>::ArrayToTuple(const std::array<BitDepth, channel_count_v<Channels>>& array) const noexcept
{
    return array_to_tuple_impl(array, Indices{});
}

/*
    The implementation of the tuple to array method. This works by
    creating an integer sequence of the index parameter pack, I, which
    sets the parameter pack with indices [0 - channel_count_v<Channels> - 1].
    An array is created by expanding the parameter pack with the get() method,
    which retrieves each element in the tuple based on the indices in the sequence

    @param tuple The tuple to convert into an array
    @returns an array containing every element that was in the tuple
*/
template<template<typename> class Channels, typename BitDepth>
template<std::size_t ...I>
auto ImageView<Channels, BitDepth>::tuple_to_array_impl(const Channels<BitDepth>& tuple, 
                                                        std::index_sequence<I...>) const noexcept
{
    return std::array<BitDepth, channel_count_v<Channels>>{ std::get<I>(tuple)... };
}

/*
    Converts a tuple into an array

    @param tuple The tuple to convert
    @returns An array containing every element that was in the tuple
*/
template<template<typename> class Channels, typename BitDepth>
template<typename... Rest>
auto ImageView<Channels, BitDepth>::TupleToArray(const std::tuple<BitDepth, Rest...>& tuple) const noexcept
{
    return tuple_to_array_impl(tuple, std::make_index_sequence<channel_count_v<Channels>>());
}
}
