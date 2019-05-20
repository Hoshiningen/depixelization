#pragma once

#include <Image.h>

#include <memory>
#include <ostream>

namespace dpa::graph
{
/*
    A graph representation of an image, where each node represents
    a pixel, and connected edges indicate pixels that are similar
*/
class SimilarityGraph final
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
    explicit SimilarityGraph(const dpa::image::Image<dpa::image::YCbCr, stbi_uc>& image);

    /*
        Declared to make the Impl struct a complete type
    */
    ~SimilarityGraph();
    SimilarityGraph(SimilarityGraph&&) noexcept;
    SimilarityGraph& operator=(SimilarityGraph&&) noexcept;

    /*
        Builds the similarity graph from the given image

        @param image The image in YCbCr color space to build the graph from
        @returns True if the graph was built, false otherwise
    */
    bool build(const dpa::image::Image<dpa::image::YCbCr, stbi_uc>& image);
    
    /*
        Prints a non-graphical representation of the graph

        @param stream The stream to write to
    */
    void printGraph(std::ostream& stream) const noexcept;

private:

    struct Impl;
    std::unique_ptr<Impl> m_pImpl{ nullptr };

};
}
