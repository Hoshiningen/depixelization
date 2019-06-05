#pragma once

#include <Implementation.h>

#include <any>
#include <fstream>
#include <memory>
#include <tuple>
#include <set>

namespace dpa::voronoi
{
namespace internal
{
class VoronoiImpl;
}

/*
    Represents a voronoi diagram, built from a resolved similarity graph.
    This graph is the reshaped pixel cells of the original pixel art
*/
class VoronoiDiagram : private dpa::internal::Implementation
{
public:

    using BlockEdge = std::tuple<std::size_t, std::size_t>;

    explicit VoronoiDiagram(std::tuple<int, int>& imageDims) noexcept;

    /*
        Builds the voronoi diagram

        @param edges The set of edges to build the diagram from
    */
    void build(const std::set<BlockEdge>& edges) noexcept;

    /*
        Writes a .tex file to the given ostream. This can be compiled into
        a pdf using pdflatex

        @param output The output stream to write the .tex file too
        @returns True if the write was successful, false otherwise
    */
    bool writeTex(std::ostream& output);

    /*
        Prints a non-graphical representation of the graph

        @param stream The stream to write to
    */
    void printGraph(std::ostream& stream);

    /*
        Prints a non-graphical representation of the vertices,
        with their stored properties

        @param stream The stream to write to
    */
    void printVertices(std::ostream& stream);

private:

    /*
        Creates the implementation object
    */
    std::shared_ptr<std::any> createImpl() override;

    /*
        Retrieves the implementation object
    */
    std::shared_ptr<internal::VoronoiImpl> impl();

};
}