#include <ProgramDriver.h>

#include <FileUtil.h>
#include <Image.h>
#include <ScopedTimer.h>
#include <SimilarityGraph.h>
#include <Voronoi.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace
{
/*
    This method determines whether or not we should overwrite the given file

    @param fileName The file to prompt the user for permission to overwrite

    @returns True if the user wants to overwrite the file, false otherwise
*/
bool ShouldOverwriteFile(const std::string& fileName)
{
    std::ostringstream prompt;
    prompt << "The file: '" << fileName << "' already exists in the output directory. ";
    prompt << "Do you want to overwrite this file [Y/N]? ";

    const auto isValidResponse = [](char resp)
    {
        return resp == 'Y' || resp == 'y' || resp == 'N' || resp == 'n';
    };

    char response = ' ';

    std::cout << prompt.str();

    // Annoyingly loop until the user enters 'Y' or 'N'
    while (!isValidResponse(response))
    {
        std::cin >> response;

        if (!isValidResponse(response))
            std::cout << prompt.str();
    }

    std::cout << "\n";

    return (response == 'Y' || response == 'y') ? true : false;
}
}

int ProgramDriver::go()
{
    using namespace dpa::image;
    using namespace dpa::graph;
    using namespace dpa::voronoi;

    if (Image<RGB, stbi_uc> imageData{ m_imagePath }; imageData.isLoaded())
    {
        bool isVerbose = m_parser.get<bool>("--verbose");
        if (isVerbose)
        {
            std::cout << "-- Image [";
            std::cout << "Width: " << imageData.getWidth() << "\t";
            std::cout << "Height: " << imageData.getHeight() << "\t";
            std::cout << "Channels: " << imageData.getChannels() << "] loaded\n\n";
        }

        auto imageDims = std::make_tuple(imageData.getWidth(), imageData.getHeight());

        dpa::graph::SimilarityGraph simGraph;
        {
            ScopedTimer timer = {
                isVerbose,
                "-- Building the similarity graph\n",
                "-- Similarity graph built in: ",
                [&]() { simGraph.build(imageData); },
                [&](long long delta) { m_totalExecutionTime += delta; }
            };
        }

        applyHeuristics(simGraph,
            heuristics::DissimilarPixels{},
            heuristics::Curves{ imageDims },
            heuristics::Islands{ imageDims },
            heuristics::SparsePixels{ imageDims }
        );

        if (m_parser["--similarity_graph"] == true)
            render(simGraph);

        VoronoiDiagram voronoiGraph{ imageDims };
        {
            ScopedTimer timer = {
                isVerbose,
                "-- Building the voronoi graph\n",
                "-- Voronoi graph built in: ",
                [&]() { voronoiGraph.build(simGraph.getEdges()); },
                [&](long long delta) { m_totalExecutionTime += delta; }
            };
        }

        if (m_parser["--voronoi_graph"] == true)
            render(voronoiGraph);

        if (isVerbose)
            std::cout << "-- Total execution time: " << m_totalExecutionTime << "ms\n";
    }
    else
    {
        printError("Could not load the specified image.");
    }

    return 1;
}

argparse::ArgumentParser ProgramDriver::prepareArguments(const std::string& programName)
{
    argparse::ArgumentParser program{ programName };

    program.add_argument("image")
        .help("The input image to depixelize")
        .action([](auto arg) { return std::filesystem::path(arg); });

    program.add_argument("-o", "--output")
        .help("The destination directory to write the output files to")
        .required()
        .action([](auto arg) { return std::filesystem::path(arg); });

    program.add_argument("-sg", "--similarity_graph")
        .help("Also output the similarity graph as a .tex file")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-vg", "--voronoi_graph")
        .help("Also output the voronoi graph as a .tex file")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-v", "--verbose")
        .help("Display verbose messages")
        .default_value(false)
        .implicit_value(true);

    return program;
}

bool ProgramDriver::validateArguments(int argc, const char* const* argv)
{
    try
    {
        m_parser.parse_args(argc, argv);

        m_imagePath = m_parser.get<std::filesystem::path>("image");
        m_outputPath = m_parser.get<std::filesystem::path>("--output");
    }
    catch (const std::exception& error)
    {
        printError(error.what());
    }
     
    return dpa::fileutil::isValidImage(m_imagePath) && dpa::fileutil::isValidDirectory(m_outputPath);
}

bool ProgramDriver::render(dpa::graph::SimilarityGraph& graph)
{
    std::string fileName = m_imagePath.stem().filename().string() + "_similarity.tex";
    
    std::filesystem::path outPath = m_outputPath;
    outPath.append(fileName);

    // Helper lambda to write the tex file
    const auto WriteFile = [this, &graph](const auto& filePath)
    {
        std::ofstream outFile{ filePath };

        if (m_parser.get<bool>("--verbose"))
            std::cout << "-- Writing: " << filePath.string() << "\n\n";

        if (outFile.is_open())
            return graph.writeTex(outFile, dpa::graph::heuristics::FilteredEdges::eAll);

        return false;
    };

    // Prompt that the file will be overwritten if it already exists
    if (dpa::fileutil::fileExists(outPath))
        return (ShouldOverwriteFile(fileName)) ? WriteFile(outPath) : false;

    return WriteFile(outPath);
}

bool ProgramDriver::render(dpa::voronoi::VoronoiDiagram& graph)
{
    std::string fileName = m_imagePath.stem().filename().string() + "_voronoi.tex";

    std::filesystem::path outPath = m_outputPath;
    outPath.append(fileName);

    // Helper lambda to write the tex file
    const auto WriteFile = [this, &graph](const auto& filePath)
    {
        std::ofstream outFile{ filePath };

        if (m_parser.get<bool>("--verbose"))
            std::cout << "-- Writing: " << filePath.string() << "\n\n";

        if (outFile.is_open())
            return graph.writeTex(outFile);

        return false;
    };

    // Prompt that the file will be overwritten if it already exists
    if (dpa::fileutil::fileExists(outPath))
        return (ShouldOverwriteFile(fileName)) ? WriteFile(outPath) : false;

    return WriteFile(outPath);
}
