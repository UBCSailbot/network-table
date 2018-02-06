// Copyright 2017 UBC Sailbot

#ifndef TREE_H_
#define TREE_H_

#include <string>

#include "Value.pb.h"
#include "Node.pb.h"

namespace NetworkTable {

void PrintTree(NetworkTable::Node root);

class Tree {
 public:
    /*
     * Returns node at given uri. If it does not exist
     * @param uri - path to the node to set, seperated by '/'.
     *              must start with a forward slash.
     *              eg. "/gps/lat" or "gps/lat"
     */
    NetworkTable::Node GetNode(std::string uri);

    /*
     * Sets a given node in the tree. Creates the intermediate nodes
     * if they don't exist.
     * @param uri - path to the node to set, seperated by '/'.
     *              eg. "/gps/lat" or "gps/lat"
     * @param value - The value at the given uri.
     */
    void SetNode(std::string uri, NetworkTable::Value value);

    void PrintTree();

 private:
    NetworkTable::Node root_;
};

/*
 * This exception is thrown when the node a client is trying
 * to get does not exist, eg the wrong uri was passed.
 */
class NodeNotFoundException : public std::runtime_error {
 public:
     explicit NodeNotFoundException(std::string what) : std::runtime_error(what.c_str()) { }
};
}  // namespace NetworkTable

#endif  // TREE_H_