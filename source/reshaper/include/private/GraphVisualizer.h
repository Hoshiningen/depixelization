#pragma once

#include <GraphUtils.h>

#include <filesystem>
#include <fstream>
#include <tuple>

#include <boost/graph/depth_first_search.hpp>

namespace dpa::graph::internal
{

/*
    A DFS Visitor that writes lateX output for each node and edge
    that it visits.
*/
class LateXGraphWriter : public boost::default_dfs_visitor
{
public:

    LateXGraphWriter(const utility::Point2D<int>& imageDims, std::ostream& output)
        : m_imageDims(imageDims), m_output(output)
    {}

    /*
        Generates LaTeX output for each vertex in the graph

        @param vertex The vertex to serialize
        @graph The graph the vertex belongs too
    */
    template <class Vertex, class Graph>
    void initialize_vertex(Vertex vertex, const Graph& graph)
    {
        const auto [imageWidth, imageHeight] = m_imageDims;
        auto [x, y] = utility::ExpandIndex(vertex, imageWidth);

        // Flip the image since we store the points different than
        // how they'll be rendered
        y = imageHeight - y;

        m_output << "\\node[circle, thick, draw=black!100, minimum size=5mm, fill={rgb,255:"
                 << "red," << static_cast<int>(graph[vertex].Y) << ";"
                 << "green," << static_cast<int>(graph[vertex].Cb) << ";"
                 << "blue," << static_cast<int>(graph[vertex].Cr) << "}] (" << vertex << ")"
                 << "at (" << x << ", " << y << "){};\n";
    }


    /*
        Generates LaTeX output for each edge in the graph

        @param edge The edge to serialize
        @graph The graph the edge belongs too
    */
    template<typename Edge, typename Graph>
    void examine_edge(Edge edge, const Graph& graph)
    {
        const auto start = boost::source(edge, graph);
        const auto end = boost::target(edge, graph);

        m_output << "\\draw (" << start << ") -- (" << end << "){};\n";
    }

private:

    std::ostream& m_output;
    const utility::Point2D<int>& m_imageDims;

};

/*
    A class that produces graphical output of a similarity graph
*/
template<typename Graph>
class GraphVisualizer
{
public:

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

};

template<typename Graph>
bool GraphVisualizer<Graph>::writeTex(const Graph& graph, const std::tuple<int, int>& imageDims,
                                       std::ostream& output) const
{
    if (!output.good())
        return false;

    writeFile(graph, imageDims, output);

    return true;
}

template<typename Graph>
void GraphVisualizer<Graph>::writeFile(const Graph& graph, const std::tuple<int, int>& imageDims,
                                       std::ostream& output) const noexcept
{
    writeHeader(graph, output);
    writeDocument(graph, imageDims, output);
}

template<typename Graph>
void GraphVisualizer<Graph>::writeHeader(const Graph& graph, std::ostream& output) const noexcept
{
    boost::ignore_unused_variable_warning(graph);

    output << "\\documentclass{standalone}";
    output << "\\usepackage{tikz}";
    output << "\\usetikzlibrary{ positioning }";
    output << "\\usetikzlibrary{ patterns }";
    output << "\\usetikzlibrary{ fit }";
}

template<typename Graph>
void GraphVisualizer<Graph>::writeDocument(const Graph& graph, const std::tuple<int, int>& imageDims,
                                           std::ostream& output) const noexcept
{
    output << "\\begin{document}";
    writeTikzPicture(graph, imageDims, output);
    output << "\\end{document}";
}

template<typename Graph>
void GraphVisualizer<Graph>::writeTikzPicture(const Graph& graph, const std::tuple<int, int>& imageDims,
                                              std::ostream& output) const noexcept
{
    output << "\\begin{tikzpicture}";
    writeTikzStyles(graph, output);

    // Use the graph writer to write the nodes and edges
    boost::depth_first_search(graph, boost::visitor(LateXGraphWriter{imageDims, output}));

    output << "\\end{tikzpicture}";
}

template<typename Graph>
void GraphVisualizer<Graph>::writeTikzStyles(const Graph& graph, std::ostream& output) const noexcept
{
    boost::ignore_unused_variable_warning(graph);

    output << "[";
    output << "node/.style={circle, draw=black, thick, minimum size=7mm}";
    output << "]";
}
}