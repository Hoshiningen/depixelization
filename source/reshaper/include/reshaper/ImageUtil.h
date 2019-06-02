#pragma once

#include <Image.h>

#include <optional>
#include <type_traits>

namespace dpa::image::utility
{
/*
    Iterates over every pixel in the image, in row-major order, applying
    the given predicate to each pixel. This assigns the results of the
    of the predicate to the given pixel.

    @param image The image to iterate over
    @param pred The predicate to apply to each pixel in the image
    returns True if the predicate was applied to all pixels, false otherwise
*/
template<template<typename> class Channels, typename BitDepth, typename Predicate>
bool foreach_pixel_mutable(dpa::image::Image<Channels, BitDepth>& image, Predicate pred)
{
    for (auto h = 0; h < image.getHeight(); ++h)
    {
        for (auto w = 0; w < image.getWidth(); ++w)
        {
            // Retrieve the pixel
            if (auto sourcePixel = image.getPixelAt({ w, h }); sourcePixel)
            {
                // If we were able to get it, transform it using the predicate,
                // then set the transformation back into the image
                if (!image.setPixelAt({ w, h }, pred(sourcePixel.value())))
                    return false;
            }
            else
            {
                return false;
            }
        }
    }

    return true;
}

/*
    Iterates over every pixel in the image, in row-major order, applying
    the given predicate to each pixel. This does not modify the pixel
    the predicate was called with.

    @param image The image to iterate over
    @param pred The predicate to apply to each pixel in the image
    returns True if the predicate was applied to all pixels, false otherwise
*/
template<template<typename> class Channels, typename BitDepth, typename Predicate>
bool foreach_pixel(const dpa::image::Image<Channels, BitDepth>& image, Predicate pred)
{
    for (auto h = 0; h < image.getHeight(); ++h)
    {
        for (auto w = 0; w < image.getWidth(); ++w)
        {
            // Retrieve the pixel
            if (auto sourcePixel = image.getPixelAt({ w, h }); sourcePixel)
            {
                if (!pred({ w, h }, sourcePixel.value()))
                    return false;
            }
            else
            {
                return false;
            }
        }
    }

    return true;
}

/*
    Converts an image in RGB color space to YCbCr colorspace

    @param image The image to transform
    @returns True if every pixel was transformed, false otherwise
*/
template<template<typename> class Channels, typename BitDepth>
std::optional<Image<YCbCr, BitDepth>> RGB_To_YCbCr(const Image<Channels, BitDepth>& image);

/*
    Converts an image in YCbCr color space to RGB colorspace

    @param image The image to transform
    @returns True if every pixel was transformed, false otherwise
*/
template<template<typename> class Channels, typename BitDepth>
std::optional<Image<RGB, BitDepth>> YCbCr_To_RGB(const Image<Channels, BitDepth>& image);
}
