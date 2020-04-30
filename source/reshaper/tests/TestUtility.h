#pragma once

#include <filesystem>
#include <iterator>
#include <sstream>
#include <vector>

/*
    This reads in solution data from a file, based on the supplied
    format of the stored solution data

    @tparam EntryFormat The format of a single record in the solution file

    @param file The file to read the solution data from

    @returns The solution data parsed into a vector of data blocks
*/
template<typename EntryFormat>
std::vector<EntryFormat> getSolutionData(const std::filesystem::path& file)
{
    std::ifstream input{ file };
    std::vector<EntryFormat> ret;

    std::copy(std::istream_iterator<EntryFormat>(input),
        std::istream_iterator<EntryFormat>(), std::back_inserter(ret));

    return ret;
}