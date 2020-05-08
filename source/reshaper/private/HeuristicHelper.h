#pragma once

#include <map>
#include <unordered_map>
#include <tuple>
#include <variant>

#include <boost/uuid/uuid_hash.hpp>

namespace dpa::graph::heuristics
{
/*
    A singleton class that stores arbitrary edge properties
    based on what a specific heuristic discovers during its
    graph traversal
*/
class HeuristicHelper
{
public:

    using Edge = std::tuple<std::size_t, std::size_t>;
    using EdgeProperty = std::variant<bool, double>;
    using EdgeMap = std::map<Edge, EdgeProperty>;
    using HashedEdgeMap = std::unordered_map<boost::uuids::uuid, EdgeMap,
        boost::hash<boost::uuids::uuid>>;

public:

    /*
        Clears the edges set by the heuristic with the given uuid

        @param uuid The uuid of the heuristic
    */
    void clearMarkedEdges(const boost::uuids::uuid& uuid) noexcept;

    /*
        Gets the marked edges set by the heuristic with the given uuid
        
        @param uuid The uuid of the heuristic
        @returns A constant reference to the marked edges
    */
    const EdgeMap& getMarkedEdges(const boost::uuids::uuid& uuid) noexcept;

    /*
        Inserts a marked edge into the edge map associated with the
        heuristic with the given uuid

        @param uuid
        @param edge An edge_descriptor in the graph
    */
    void insertMarkedEdge(const boost::uuids::uuid& uuid, const Edge& edge,
                          EdgeProperty value) noexcept;

private:

    HashedEdgeMap m_markedEdges{};

private:

    friend class Curves;
    friend class DissimilarPixels;
    friend class Islands;
    friend class SparsePixels;

    // Just look the other way..
    HeuristicHelper() = default;

    static HeuristicHelper& instance() noexcept;

};

/*
    Interface for getting and clearing marked edges
*/
class IMarkedEdgeHelper
{
public:

    /*
        Gets the edges that were marked as dissimilar

        @returns A const reference to the marked edges
    */
    virtual const HeuristicHelper::EdgeMap& getMarkedEdges() const noexcept = 0;

    /*
        Clears the edges set by the heuristic with the given uuid

        @param uuid The uuid of the heuristic
    */
    virtual void clearMarkedEdges() const noexcept = 0;

};
}
