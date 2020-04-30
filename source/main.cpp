
#include <Heuristics.h>
#include <Image.h>
#include "ScopedTimer.h"
#include <SimilarityGraph.h>
#include <Voronoi.h>

#include <argparse.hpp>

/*
    Prints out an error message and terminates the program

    @tparam Message The type of the message to print out. Must be writable to std:::cout

    @param parser   The program argument parser. Prints the usage message
    @param message  The error message to print
*/
template<typename Message>
void printError(const argparse::ArgumentParser& parser, const Message& message)
{
    std::cout << message << "\n";
    std::cout << parser;

    std::exit(0);
}

std::tuple<std::filesystem::path, std::filesystem::path> ValidateArguments(argparse::ArgumentParser& program, int argc, char* argv[])
{
    program.parse_args(argc, argv);

    std::filesystem::path image = program.get<std::filesystem::path>("image");
    std::filesystem::path dest = program.get<std::filesystem::path>("destination");
    
    return { image, dest };
}

/*
    Sets up the argument parser, adding all the argumnets the program accepts

    @param programName  The name of the program that's being executed

    @returns An argument parser with arguments added to it
*/
argparse::ArgumentParser PrepareArguments(const std::string& programName)
{
    argparse::ArgumentParser program{ programName };

    program.add_argument("image")
        .help("The input image to depixelize")
        .action([](auto arg) { return std::filesystem::path(arg); });

    program.add_argument("destination")
        .help("The destination directory to write the output files to")
        .action([](auto arg) { return std::filesystem::path(arg); });

    program.add_argument("-sg", "--similarity_graph")
        .help("Also output the similarity graph")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-vg", "--voronoi_graph")
        .help("Also output the voronoi graph")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("-v", "--verbose")
        .help("Display verbose messages")
        .default_value(false)
        .implicit_value(true);

    return program;
}

int main(int argc, char* argv[])
{
    using namespace dpa::image;
    using namespace dpa::graph;
    using namespace dpa::voronoi;
    using namespace std::chrono_literals;

    argparse::ArgumentParser program = PrepareArguments("depixelization");
    
    std::filesystem::path inputImage = "";
    std::filesystem::path destination = "";

    try
    {
        std::tie(inputImage, destination) = ValidateArguments(program, argc, argv);
    }
    catch (const std::exception& error)
    {
        printError(program, error.what());
    }

    const bool IsVerbose = program.get<bool>("--verbose");

    if (IsVerbose)
        std::cout << "Loading the image: \'" << inputImage << "\'\n";

    Image<RGB, stbi_uc> imageData{ inputImage };
    if (!imageData.isLoaded())
    {
        printError(program, "Error: unable to load the image: " + inputImage.string() + "\n\n");
    }

    // The dimensions of the loaded image
    auto imageDims = std::make_tuple(imageData.getWidth(), imageData.getHeight());

    if (IsVerbose)
    {
        std::cout << "Image was loaded successfully";
        std::cout << "Image Dimensions: [" << imageData.getWidth() << ", " << imageData.getHeight() << "]\n";
    }

    if (IsVerbose)
    {
        std::cout << "-- Image loaded\n";
    }
    
    // Records the total execution time of the program, and is printed if in verbose mode
    long long totalExecutionTime = 0;

    // The similarity graph of the loaded image
    SimilarityGraph simGraph;
    {
        ScopedTimer timer = {
            IsVerbose,
            "-- Building the similarity graph\n",
            "-- Similarity graph built in: ",
            [&]() { simGraph.build(imageData); },
            [&](long long delta) { totalExecutionTime += delta; }
        };
    }
    
    // Apply the dissimilar pixels heuristic
    {
        ScopedTimer timer = {
            IsVerbose,
            "-- Appling the dissimiliar pixels heuristic\n",
            "-- Heuristic applied in: ",
            [&]() { simGraph.applyHeuristic(heuristics::DissimilarPixels{}); },
            [&](long long delta) { totalExecutionTime += delta; }
        };
    }

    // Apply the curves heuristic
    {
        ScopedTimer timer = {
            IsVerbose,
            "-- Appling the curves heuristic\n",
            "-- Heuristic applied in: ",
            [&]() { simGraph.applyHeuristic(heuristics::Curves{ imageDims }); },
            [&](long long delta) { totalExecutionTime += delta; }
        };
    }

    // Apply the islands heuristic
    {
        ScopedTimer timer = {
            IsVerbose,
            "-- Appling the islands heuristic\n",
            "-- Heuristic applied in: ",
            [&]() { simGraph.applyHeuristic(heuristics::Islands{ imageDims }); },
            [&](long long delta) { totalExecutionTime += delta; }
        };
    }

    // Apply the sparse pixels heuristic
    {
        ScopedTimer timer = {
            IsVerbose,
            "-- Appling the sparse pixels heuristic\n",
            "-- Heuristic applied in: ",
            [&]() { simGraph.applyHeuristic(heuristics::SparsePixels{ imageDims }); },
            [&](long long delta) { totalExecutionTime += delta; }
        };
    }

    VoronoiDiagram voronoiGraph{ imageDims };
    {
        ScopedTimer timer = {
            IsVerbose,
            "-- Building the voronoi graph\n",
            "-- Voronoi graph built in: ",
            [&]() { voronoiGraph.build(simGraph.getEdges()); },
            [&](long long delta) { totalExecutionTime += delta; }
        };
    }

    if (IsVerbose)
        std::cout << "Total execution time: " << totalExecutionTime << "ms\n\n";

    // std::ofstream out{ "C:/Users/Brian/Desktop/graph/full_voronoi.tex" };
    // voronoi.writeTex(out);
    // out.close();

    return 1;
}
