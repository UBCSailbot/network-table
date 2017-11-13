// Copyright 2017 UBC Sailbot

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "Reply.pb.h"
#include "Request.pb.h"
#include "Value.pb.h"

#include <atomic>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <exception>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <queue>
#include <utility>
#include <zmq.hpp>

namespace NetworkTable {
class Connection {
 public:
    Connection();

    void Connect();

    void Disconnect();

    /*
     * Set a value in the network table, or create it
     * if it doesn't exist already.
     *
     * @param key - which key in the table to set.
     * @param value - what value to set the value at that key to.
     */
    void SetValue(std::string key, const NetworkTable::Value &value);

    /*
     * Set multiple values in the network table, or create
     * them if they don't exist.
     * @param values - map from string to Value, where the string
     *                 is the key, and Value is what to set the
     *                 value at that key to.
     */
    void SetValues(const std::map<std::string, NetworkTable::Value> &values);

    /*
     * Get a value from the network table.
     * Returns a NetworkTable::Value with type NONE
     * if no value at the specified key exists.
     * 
     * @param key - returns the value located at this key.
     */
    NetworkTable::Value GetValue(std::string key);

    /*
     * Get multiple values from the network table.
     * The values are returned in the same order that
     * the keys were sent in.
     *
     * @param keys - which keys to get which values for.
     */
    std::map<std::string, NetworkTable::Value> GetValues(std::set<std::string> keys);

    /*
     * Begin receiving updates on a key in
     * the network table. The callback function is
     * ran anytime a change occurs. The callback function
     * returns void, and takes a single argument of type
     * NetworkTable::Value.
     *
     * @param key - what key to subscribe to
     * @param callback - what function to call when the key changes.
     *                   the function takes a single argument: the new value.
     */
    void Subscribe(std::string key, void (*callback)(NetworkTable::Value value));

    /*
     * Stop receiving updates on a key in the network table.
     * Has no effect if the key is not subscribed to.
     */
    void Unsubscribe(std::string key);

    /*
     * Set timeout for all methods which communicate with server.
     * @param timeout - timeout in milliseconds; if negative then there is no timeout
     */
    void SetTimeout(int timeout);

 private:
    void Send(const NetworkTable::Request &request, zmq::socket_t *socket);

    /*
     * Returns true only if a message was received.
     */
    bool Receive(NetworkTable::Reply *reply, zmq::socket_t *socket);

    bool TimedOut(std::chrono::steady_clock::time_point start_time);

    void ManageSocket();

    std::thread socket_thread_;  // This interacts with the socket.
    std::atomic_bool connected_;  // True when connected to the server.
                                  // This is set by the manage socket thread
                                  // and read by the main thread.
    std::atomic_bool terminate_;  // This is used to gracefully
                                  // terminate the manage socket thread.
                                  // It is set by the main thread
                                  // and read by the manage socket thread.

    // The request_queue_ is a queue
    // containing pairs of uuids and
    // network table requests.
    // The uuid is used to link a request to a reply.
    std::queue< \
        std::pair< \
            boost::uuids::uuid, \
            NetworkTable::Request>> request_queue_;
    std::queue< \
        std::pair< \
            boost::uuids::uuid, \
            NetworkTable::Reply>> reply_queue_;

    std::mutex request_queue_mutex_;
    std::mutex reply_queue_mutex_;

    std::map<std::string, \
        void (*)(NetworkTable::Value)> callbacks_;  // Map from table key
                                                    // to callback function.
    int timeout_;  // How long to wait before throwing an exception when
                   // communicating with server. This is in milliseconds.
};

/*
 * This exception can be thrown if a request to the server times out.
 * A timeout can occur because the server is busy, or because
 * the connection is lost
 */
class TimeoutException : public std::runtime_error {
 public:
     explicit TimeoutException(char* what) : std::runtime_error(what) { }
};

}  // namespace NetworkTable

#endif  // CONNECTION_H_
