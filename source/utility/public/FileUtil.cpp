#include "FileUtil.h"

#include <array>
#include <filesystem>
#include <string>

namespace dpa::fileutil
{
bool isValidImage(const std::filesystem::path& filePath)
{
    static std::array<std::string, 10> extensions =
    {
        ".jpeg", ".jpg", ".png", ".tga", ".bmp",
        ".psd", ".gif", ".hdr", ".pic", ".pnm"
    };

    if (!fileExists(filePath) || !isValidImageExtension(filePath, extensions))
        return false;

    return true;
}

bool isValidDirectory(const std::filesystem::path& directoryPath)
{
    return std::filesystem::is_directory(directoryPath);
}

bool fileExists(const std::filesystem::path& file)
{
    if (std::filesystem::exists(file) &&
        std::filesystem::is_regular_file(file))
    {
        return true;
    }

    return false;
}
} // end dpa::fileutil namespace

