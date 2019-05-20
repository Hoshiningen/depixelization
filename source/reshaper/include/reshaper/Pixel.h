#pragma once

#include <stb_image.h>

#include <tuple>

namespace dpa::image
{
// Type traits to ensure our channel types are only
// specialized with valid stb_image types


template<typename BitDepth>
struct is_stb_supported;

template<typename BitDepth>
constexpr bool is_stb_supported_v = is_stb_supported<BitDepth>::value;

template<>
struct is_stb_supported<stbi_uc>
{
    static constexpr bool value = true;
};

template<>
struct is_stb_supported<stbi_us>
{
    static constexpr bool value = true;
};

template<>
struct is_stb_supported<float>
{
    static constexpr bool value = true;
};

template<typename BitDepth>
using EnableIfSTBBitDepth = std::enable_if_t<is_stb_supported_v<BitDepth>>;


// Aliases for pixel types


template<typename BitDepth, typename = EnableIfSTBBitDepth<BitDepth>>
using Y = std::tuple<BitDepth>;

template<typename BitDepth, typename = EnableIfSTBBitDepth<BitDepth>>
using YA = std::tuple<BitDepth, BitDepth>;

template<typename BitDepth, typename = EnableIfSTBBitDepth<BitDepth>>
using RGB = std::tuple<BitDepth, BitDepth, BitDepth>;

template<typename BitDepth, typename = EnableIfSTBBitDepth<BitDepth>>
using YCbCr = std::tuple<BitDepth, BitDepth, BitDepth>;

template<typename BitDepth, typename = EnableIfSTBBitDepth<BitDepth>>
using RGBA = std::tuple<BitDepth, BitDepth, BitDepth, BitDepth>;

template<template<typename> class Channels>
struct channel_count;


// Type traits for determining channel count from a pixel type


template<template<typename> class Channels>
constexpr int channel_count_v = channel_count<Channels>::value;

template<>
struct channel_count<Y>
{
    static constexpr int value = 1;
};

template<>
struct channel_count<YA>
{
    static constexpr int value = 2;
};

template<>
struct channel_count<RGB>
{
    static constexpr int value = 3;
};

template<>
struct channel_count<YCbCr>
{
    static constexpr int value = 3;
};

template<>
struct channel_count<RGBA>
{
    static constexpr int value = 4;
};


// User defined literals to ease the pain of making pixel types


inline constexpr stbi_uc operator "" _uc(unsigned long long arg) noexcept
{
    return static_cast<stbi_uc>(arg);
}

inline constexpr stbi_us operator "" _us(unsigned long long arg) noexcept
{
    return static_cast<stbi_us>(arg);
}

/*
    Makes a new pixel that has as many channels as allowed
    by the supplied "Channel" type. Template functions enforce
    that BitDepth is either stbi_uc or stbi_us, and that the
    number of given function arguments is equal to the number
    of channels in the pixel type

    @param args A parameter pack that contains the raw pixel
                data, as is appropriate for the type of pixel
                being created. I.e., when creating an RGB pixel
                the supplied function arguments should be the
                'r', 'g', and 'b' components of the pixel
    @returns A pixel (or tuple) containing the supplied arguments
*/
template<template<typename> class Channels, typename... BitDepth>
constexpr auto make_pixel(BitDepth&&... args) -> 
    std::enable_if_t<sizeof...(args) == channel_count_v<Channels>,
        Channels<std::common_type_t<BitDepth...>>>
{
    return { std::forward<BitDepth>(args)... };
}
}
