#pragma once

#include <array>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

#include <gtest/gtest.h>

class UtilityTests : public ::testing::Test
{
protected:

    void TearDown() override
    {

    }

    std::optional<std::filesystem::path> createFile(const std::filesystem::path& directory, const std::string& fileName) const
    {
        std::filesystem::path filePath = directory;
        filePath /= fileName;

        if (std::filesystem::exists(filePath))
            return {};

        if (std::ofstream file{ filePath }; !file.is_open())
            return {};

        return filePath;
    }

protected:

    const std::array<std::string, 10> m_validExtensions =
    {
        ".jpeg", ".jpg", ".png", ".tga", ".bmp",
        ".psd", ".gif", ".hdr", ".pic", ".pnm"
    };

    const std::filesystem::path m_dataDir{ "../../source/utility/tests/data" };

};
