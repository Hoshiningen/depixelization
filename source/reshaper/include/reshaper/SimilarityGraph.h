#pragma once

#include <Heuristics.h>
#include <Image.h>
#include <Implementation.h>

#include <memory>
#include <ostream>
#include <set>
#include <variant>

namespace dpa::graph
{
namespace internal
{
class SimilarityGraphImpl;
}

/*
    A graph representation of an image, where each node represents
    a pixel, and connected edges indicate pixels that are similar
*/
class SimilarityGraph final : private dpa::internal::Implementation
{
public:

    /*
        Default constructs a similarity graph. Requires a call
        to "build" in order to create the lattice graph
    */
    SimilarityGraph();

    /*
        Parameterized ctor. Builds the graph from the given image

        @param image The image to build the graph from
    */
    explicit SimilarityGraph(const dpa::image::Image<dpa::image::RGB, stbi_uc>& image);

    /*
        Builds the similarity graph from the given image

        @param image The image in YCbCr color space to build the graph from
        @returns True if the graph was built, false otherwise
    */
    bool build(const dpa::image::Image<dpa::image::RGB, stbi_uc>& image);
    
    /*
        Applies the given heuristic to the similarity graph,
        which modifies the edges in the graph

        @param heuristic The heuristic to apply to the graph
    */
    void applyHeuristic(heuristics::Heuristic heuristic);

    /*
        Prints a non-graphical representation of the graph

        @param stream The stream to write to
    */
    void printGraph(std::ostream& stream);

    /*
        Writes a .tex file to the given ostream. This can be compiled into
        a pdf using pdflatex

        @param output The output stream to write the .tex file too
        @returns True if the write was successful, false otherwise
    */
    bool writeTex(std::ostream& output,
        heuristics::FilteredEdges filteredEdges = heuristics::FilteredEdges::eNone);

    /*
        Gets all the edges from the similarity graph

        @param filteredEdges The edges to filter from the graph
        @returns The edges in the similarity graph
    */
    std::set<std::tuple<std::size_t, std::size_t>> getEdges(
        heuristics::FilteredEdges filteredEdges = heuristics::FilteredEdges::eAll) noexcept;

private:

    /*
        Creates the implementation object
    */
    std::shared_ptr<std::any> createImpl() override;

    /*
        Retrieves the implementation object
    */
    std::shared_ptr<internal::SimilarityGraphImpl> impl();

};
}
