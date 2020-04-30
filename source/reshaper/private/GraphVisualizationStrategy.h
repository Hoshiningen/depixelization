#pragma once

#include <fstream>
#include <tuple>

namespace dpa::graph::internal
{
/*
    An interface for LaTeX graph visualization strategies. It provides
    the necessary methods to generate LaTeX files
*/
template<typename Graph>
class ILaTeXVisualizationStrategy
{
public:

    /*
        Writes the LaTeX file

        @param graph The similarity graph to visualize
        @param imageDims The dimensions of the image the graph was built from
        @output A stream to write the graph output too
    */
    virtual void writeFile(const Graph& graph, const std::tuple<int, int>& imageDims,
        std::ostream& output) const noexcept = 0;

    /*
        Writes the LaTeX header, which contains the important librarie
        and other settings specifc to the graph output

        @param graph The similarity graph to visualize
        @output A stream to write the graph output too
    */
    virtual void writeHeader(const Graph& graph, std::ostream& output) const noexcept = 0;

    /*
        Writes the LaTeX document, which is all the text that makes up
        the main content of the latex file.

        @param graph The similarity graph to visualize
        @param imageDims The dimensions of the image the graph was built from
        @output A stream to write the graph output too
    */
    virtual void writeDocument(const Graph& graph, const std::tuple<int, int>& imageDims,
        std::ostream& output) const noexcept = 0;
    /*
        Writes the LaTeX tikz picture, which contains all of the node
        and edge declarations

        @param graph The similarity graph to visualize
        @param imageDims The dimensions of the image the graph was built from
        @output A stream to write the graph output too
    */
    virtual void writeTikzPicture(const Graph& graph, const std::tuple<int, int>& imageDims,
        std::ostream& output) const noexcept = 0;

    /*
        Writes the LaTeX styles, which can be used to style nodes and edges
        in the tiks picture

        @param graph The similarity graph to visualize
        @output A stream to write the graph output too
    */
    virtual void writeTikzStyles(const Graph& graph, std::ostream& output) const noexcept = 0;

};
}
