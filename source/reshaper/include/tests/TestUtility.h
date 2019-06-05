#pragma once

#include <filesystem>
#include <iterator>
#include <sstream>
#include <vector>

template<typename EdgeData>
std::vector<EdgeData> getSolutionData(const std::filesystem::path& file)
{
    std::ifstream input{ file };
    std::vector<EdgeData> ret;

    std::copy(std::istream_iterator<EdgeData>(input),
        std::istream_iterator<EdgeData>(), std::back_inserter(ret));

    return ret;
}