#pragma once

#include <ScopedTimer.h>
#include <SimilarityGraph.h>
#include <Voronoi.h>

#include <algorithm>
#include <array>
#include <filesystem>
#include <iostream>
#include <string>
#include <type_traits>

#include <argparse.hpp>

/*
    A class that drives the depixelization process based on user-supplied arguments
*/
class ProgramDriver
{
public:

    /*
        Constructs a new instance of the class.
    */
    ProgramDriver(const std::string& programName, int argc, const char* const* argv)
       : m_parser(prepareArguments(programName))
    {
        using namespace dpa::image;

        if (!validateArguments(argc, argv))
            printError("One or more of the provided arguments were not valid.");
    }

    /*
        Initiates the depixelization process

        @returns 0 on failure, 1 on success
    */
    int go();

private:

    /*
        Sets up the argument parser so it accepts arguments

        @param programName  The name of the executable

        @returns An instance of the initialized argument parser
    */
    argparse::ArgumentParser prepareArguments(const std::string& programName);
    
    /*
        Validates the user-supplied arguments, ensuring they comply with what
        the parser requires

        @param argc The number of arguments passed into the program
        @param argv The list of arguments passed in by the user

        @returns True if the arguments were valid, false otherwise
    */
    bool validateArguments(int argc, const char* const* argv);

    /*
        Prints the given error message, program usage, and terminates the program

        @tparam Message The type of message to print

        @param mesesage The message to print to stdout
    */
    template<typename Message>
    void printError(const Message& message) const;

    /*
        Processes the parameter pack of heuristics. Operates on all items except
        the last. Head is the current tip of the parameter pack, and tail is the
        remainder. The method strips off the head in each iteration (while there
        are reamining items), and processes it

        @tparam Heuristic The type of the head item
        @tparam Heuristics A parameter pack of types that remain

        @param graph    The similarity graph to apply the heuristic too
        @param head     The tip of the parameter pack
        @param tail     The remaining items in the parameter pack
    */
    template<typename Heuristic, typename...Heuristics>
    void applyHeuristics(dpa::graph::SimilarityGraph& graph, const Heuristic& head, const Heuristics&... tail);

    /*
        This processes the last item in the parameter pack

        @tparam Heuristic   The type of the last item in the parameter pack

        @param graph        The similarity graph to apply the heuristic too
        @param heuristic    The heurisitc to apply to the similarity graph
    */
    template<typename Heuristic>
    void applyHeuristics(dpa::graph::SimilarityGraph& graph, const Heuristic& heuristic);

    /*
        Applies the given heuristic to the given graph

        @tparam Heuristic   The type of heuristic to apply

        @param graph        The similarity graph to apply the heuristic too
        @param heuristic    The heuristic to apply
    */
    template<typename Heuristic>
    void applyHeuristic(dpa::graph::SimilarityGraph& graph, const Heuristic& heuristic);

    /*
        Gets the name of the heuristic based on its type

        @tpararm Heuristic  The type of the given heuristic

        @param heuristic    The heuristic to get a name for

        @returns The name of the heuristic if there is one, an empty string otherwise
    */
    template<typename Heuristic>
    std::string getHeuristicName(const Heuristic& heuristic) const;

    /*
        Renders the given similarity graph to a tex file

        @param graph    The similarity graph to render
    */
    bool render(dpa::graph::SimilarityGraph& graph);

    /*
        Renders the given voronoi graph to a tex file

        @param graph    The voronoi graph to render
    */
    bool render(dpa::voronoi::VoronoiDiagram& graph);

private:

    argparse::ArgumentParser m_parser;

    std::filesystem::path m_imagePath;
    std::filesystem::path m_outputPath;

    long long m_totalExecutionTime{ 0 };
};

template<typename Message>
void ProgramDriver::printError(const Message& message) const
{
    std::cout << message << "\n";
    std::cout << m_parser;

    std::exit(0);
}

template<typename Heuristic>
void ProgramDriver::applyHeuristic(dpa::graph::SimilarityGraph& graph, const Heuristic& heuristic)
{
    const std::string heuristicName = getHeuristicName(heuristic);

    ScopedTimer timer = {
        m_parser.get<bool>("--verbose"),
        "-- Applying the " + heuristicName + " heuristic\n",
        "-- Heuristic applied in: ",
        [&]() { graph.applyHeuristic(heuristic); },
        [&](long long delta) { m_totalExecutionTime += delta; }
    };
}

template<typename Heuristic>
void ProgramDriver::applyHeuristics(dpa::graph::SimilarityGraph& graph, const Heuristic& heuristic)
{
    applyHeuristic(graph, heuristic);
}

template<typename Heuristic, typename... Heuristics>
void ProgramDriver::applyHeuristics(dpa::graph::SimilarityGraph& graph, const Heuristic& head, const Heuristics&... tail)
{
    applyHeuristic(graph, head);
    applyHeuristics(graph, tail...);
}

template<typename Heuristic>
std::string ProgramDriver::getHeuristicName(const Heuristic& heuristic) const
{
    static const std::string curves = "Curves";
    static const std::string dissimilar = "Dissimilar Pixels";
    static const std::string islands = "Islands";
    static const std::string sparse = "Sparse Pixels";

    if (std::is_same_v<Heuristic, dpa::graph::heuristics::Curves>)
        return curves;

    if (std::is_same_v<Heuristic, dpa::graph::heuristics::DissimilarPixels>)
        return dissimilar;

    if (std::is_same_v<Heuristic, dpa::graph::heuristics::Islands>)
        return islands;

    if (std::is_same_v<Heuristic, dpa::graph::heuristics::SparsePixels>)
        return sparse;

    return "";
}
