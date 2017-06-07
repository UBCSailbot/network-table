// Copyright 2017 UBC Sailbot

#ifndef NETWORK_TABLE_H_
#define NETWORK_TABLE_H_

#include <string>
#include <map>

class NetworkTable {
 public:
    /*
     * Starts the network table, which will then be able
     * to accept requests from clients.
     */
    void Run();

 private:
    std::map<std::string, std::string> table_;
};

#endif  // NETWORK_TABLE_H_
