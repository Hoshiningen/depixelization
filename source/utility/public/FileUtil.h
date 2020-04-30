#pragma once

#include <algorithm>
#include <filesystem>
#include <functional>
#include <numeric>
#include <string>

namespace dpa::fileutil
{
/*
    Checks to see if the given path points to a valid image or not

    @param filePath A path to an image

    @returns True if it's a valid image, false otherwise
*/
bool isValidImage(const std::filesystem::path& filePath);

/*
    Checks to see if the given path points to a valid directory or not

    @param directoryPath    A path to a directory

    @returns True if it's a valid directory, valse otherwise
*/
bool isValidDirectory(const std::filesystem::path& directoryPath);

/*
    Checks the extension of the given path to see if it matches the ones we accept for images

    @tparam N           The size of the extentions array

    @param filePath     A path to an image file
    @param extensions   An array of valid exetensions the image can have

    @returns True if the given image has a valid extension, false otherwise
*/
template<std::size_t N>
bool isValidImageExtension(const std::filesystem::path& filePath, const std::array<std::string, N>& extensions)
{
    if (!filePath.has_extension())
        return false;

    const auto toLower = [](const std::string& value)
    {
        return std::transform_reduce(std::cbegin(value), std::cend(value), std::string{}, std::plus<std::string>{},
            [](char letter)
            {
                return std::string(1, static_cast<char>(std::tolower(static_cast<int>(letter))));
            });
    };

    return std::any_of(std::cbegin(extensions), std::cend(extensions),
        [&](const auto& item)
        {
            return item == toLower(filePath.extension().string());
        });
}

/*
    Determines if a file exists on disk

    @param file The file to check existance of

    @returns True if the file exists, false otherwise
*/
bool fileExists(const std::filesystem::path& file);
}// end  dpa::fileutil namespace
