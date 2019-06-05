#pragma once

#include <GraphUtils.h>
#include <GraphVisualizationStrategy.h>

#include <fstream>
#include <tuple>

#include <boost/graph/depth_first_search.hpp>

namespace dpa::graph::internal
{
/*
    A class that produces graphical output of a graph
*/
template<typename Graph>
class LaTeXGraphVisualizer
{
public:

    explicit LaTeXGraphVisualizer(ILaTeXVisualizationStrategy<Graph>& strategy) noexcept;

    /*
        Sets the visualization strategy for the graph

        @param strategy The strategy to visualize the graph with
    */
    void setVisualizationStrategy(ILaTeXVisualizationStrategy<Graph>& strategy) noexcept;

    /*
        Writes the graph into a LaTeX format, which can be visualized
        by compiling the LaTeX through pdflatex or another LaTeX compiler

        @param graph The similarity graph to visualize
        @param imageDims The dimensions of the image the graph was built from
        @output A stream to write the graph output too
        @returns True if the file was written, false otherwise
    */
    bool writeTex(const Graph& graph, const std::tuple<int, int>& imageDims,
                  std::ostream& output) const;

private:

    /*
        Writes the LaTeX file

        @param graph The similarity graph to visualize
        @param imageDims The dimensions of the image the graph was built from
        @output A stream to write the graph output too
    */
    void writeFile(const Graph& graph, const std::tuple<int, int>& imageDims,
                   std::ostream& output) const noexcept;

    /*
        Writes the LaTeX header, which contains the important librarie
        and other settings specifc to the graph output

        @param graph The similarity graph to visualize
        @output A stream to write the graph output too
    */
    void writeHeader(const Graph& graph, std::ostream& output) const noexcept;


    /*
        Writes the LaTeX document, which is all the text that makes up
        the main content of the latex file.

        @param graph The similarity graph to visualize
        @param imageDims The dimensions of the image the graph was built from
        @output A stream to write the graph output too
    */
    void writeDocument(const Graph& graph, const std::tuple<int, int>& imageDims,
                       std::ostream& output) const noexcept;
    /*
        Writes the LaTeX tikz picture, which contains all of the node
        and edge declarations

        @param graph The similarity graph to visualize
        @param imageDims The dimensions of the image the graph was built from
        @output A stream to write the graph output too
    */
    void writeTikzPicture(const Graph& graph, const std::tuple<int, int>& imageDims,
                          std::ostream& output) const noexcept;

    /*
        Writes the LaTeX styles, which can be used to style nodes and edges
        in the tiks picture

        @param graph The similarity graph to visualize
        @output A stream to write the graph output too
    */
    void writeTikzStyles(const Graph& graph, std::ostream& output) const noexcept;

private:

    ILaTeXVisualizationStrategy<Graph>& m_strategy;

};

template<typename Graph>
LaTeXGraphVisualizer<Graph>::LaTeXGraphVisualizer(ILaTeXVisualizationStrategy<Graph>& strategy) noexcept
    : m_strategy(strategy)
{}

template<typename Graph>
void LaTeXGraphVisualizer<Graph>::setVisualizationStrategy(ILaTeXVisualizationStrategy<Graph>& strategy) noexcept
{
    m_strategy = strategy;
}

template<typename Graph>
bool LaTeXGraphVisualizer<Graph>::writeTex(const Graph& graph, const std::tuple<int, int>& imageDims,
                                           std::ostream& output) const
{
    if (!output.good())
        return false;

    writeFile(graph, imageDims, output);

    return true;
}

template<typename Graph>
void LaTeXGraphVisualizer<Graph>::writeFile(const Graph& graph, const std::tuple<int, int>& imageDims,
                                            std::ostream& output) const noexcept
{
    m_strategy.writeHeader(graph, output);
    m_strategy.writeDocument(graph, imageDims, output);
}

template<typename Graph>
void LaTeXGraphVisualizer<Graph>::writeHeader(const Graph& graph, std::ostream& output) const noexcept
{
    m_strategy.writeHeader(graph, output);
}

template<typename Graph>
void LaTeXGraphVisualizer<Graph>::writeDocument(const Graph& graph, const std::tuple<int, int>& imageDims,
                                                std::ostream& output) const noexcept
{
    m_strategy.writeDocument(graph, imageDims, output);
}

template<typename Graph>
void LaTeXGraphVisualizer<Graph>::writeTikzPicture(const Graph& graph, const std::tuple<int, int>& imageDims,
                                                   std::ostream& output) const noexcept
{
    m_strategy.writeTikzPicture(graph, imageDims, output);
}

template<typename Graph>
void LaTeXGraphVisualizer<Graph>::writeTikzStyles(const Graph& graph, std::ostream& output) const noexcept
{
    m_strategy.writeTikzStyles(graph, output);
}
}