#include <UtilityTests.h>

#include "FileUtil.h"

TEST_F(UtilityTests, IsValidImageTest)
{
    std::filesystem::path validImageDir = m_dataDir;
    validImageDir /= ("valid");

    ASSERT_TRUE(std::filesystem::create_directory(validImageDir));

    // Create all the valid image files to test against
    for (const std::string& ext : m_validExtensions)
    {
        const auto optPath = createFile(validImageDir, "valid" + ext);
        EXPECT_TRUE(optPath.has_value() && dpa::fileutil::isValidImage(optPath.value()));
    }

    std::filesystem::remove_all(validImageDir);
}

TEST_F(UtilityTests, IsNotValidImageTest)
{
    std::filesystem::path invalidImageDir = m_dataDir;
    invalidImageDir /= ("invalid");

    ASSERT_TRUE(std::filesystem::create_directory(invalidImageDir));

    // A directory is not a valid image
    EXPECT_FALSE(dpa::fileutil::isValidImage(invalidImageDir));

    {
        const auto optPath = createFile(invalidImageDir, "temp.txt");
        ASSERT_TRUE(optPath.has_value());

        // A regular file with the wrong extension is not valid
        EXPECT_FALSE(dpa::fileutil::isValidImage(optPath.value()));
    }

    {
        const auto optPath = createFile(invalidImageDir, "temp");
        ASSERT_TRUE(optPath.has_value());

        // A regular file with no extension is not valid
        EXPECT_FALSE(dpa::fileutil::isValidImage(optPath.value()));
    }

    // Clean up the temp data
    std::filesystem::remove_all(invalidImageDir);
}

TEST_F(UtilityTests, IsValidExtensionTest)
{
    for (const std::string& ext : m_validExtensions)
    {
        std::filesystem::path validPath = m_dataDir;
        validPath.append("enemy_1" + ext);

        ASSERT_TRUE(dpa::fileutil::isValidImageExtension(validPath, m_validExtensions));
    }
}
