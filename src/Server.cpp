// Copyright 2017 UBC Sailbot

#include "Server.h"
#include "GetValuesReply.pb.h"
#include "SubscribeReply.pb.h"
#include "Request.pb.h"

#include <algorithm>
#include <boost/filesystem.hpp>
#include <iostream>

NetworkTable::Server::Server()
    : context_(1), init_socket_(context_, ZMQ_REP) {
    // Create the folder where sockets go.
    boost::filesystem::create_directory("/tmp/sailbot");

    init_socket_.bind("ipc:///tmp/sailbot/NetworkTable");
}

void NetworkTable::Server::Run() {
    while (true) {
        // Poll all the zmq sockets.
        // This includes init_socket_,
        // and all the ZMQ_PAIR sockets which
        // have been created.
        int num_sockets = 1 + sockets_.size();
        std::vector<zmq::pollitem_t> pollitems;

        zmq::pollitem_t pollitem;
        pollitem.socket = static_cast<void*>(init_socket_);
        pollitem.events = ZMQ_POLLIN;
        pollitems.push_back(pollitem);

        for (unsigned int i = 0; i < sockets_.size(); i++) {
            pollitem.socket = static_cast<void*>(*sockets_[i]);
            pollitem.events = ZMQ_POLLIN;
            pollitems.push_back(pollitem);
        }

        // Set the timeout to -1, which means poll
        // indefinitely until something happens.
        // This line will block until a socket is ready.
        zmq::poll(pollitems.data(), num_sockets, -1);

        if (pollitems[0].revents & ZMQ_POLLIN) {
            CreateNewConnection();
        }
        // Do not directly pass the sockets_ vector
        // into the HandleRequest function.
        // Instead pass in a copy of the sockets_ vector.
        // This is because HandleRequest may call DisconnectSocket
        // which will modify sockets_ (specifically,
        // remove a socket from sockets_.
        std::vector<socket_ptr> sockets_copy = sockets_;

        for (int i = 0; i < num_sockets-1; i++) {
            if (pollitems[i+1].revents & ZMQ_POLLIN) {
                HandleRequest(sockets_copy[i]);
            }
        }
    }
}

void NetworkTable::Server::CreateNewConnection() {
    zmq::message_t request;
    init_socket_.recv(&request);

    std::string message = static_cast<char*>(request.data());

    if (message == "connect") {
        /* A new client has connected to Network Table Server.
         * Create a new ZMQ_PAIR socket, and send them
         * the location of it.
         */
        // Get a location for new socket.
        std::string filepath = "ipc:///tmp/sailbot/" \
            + std::to_string(current_socket_number_);
        current_socket_number_++;

        // Add new socket to sockets_ and bind it.
        socket_ptr socket = std::make_shared<zmq::socket_t>(context_, ZMQ_PAIR);
        socket->bind(filepath);
        sockets_.push_back(socket);

        // Reply to client with location of socket.
        std::string reply_body = filepath;
        zmq::message_t reply(reply_body.size()+1);
        memcpy(reply.data(), reply_body.c_str(), reply_body.size()+1);
        init_socket_.send(reply);
    } else {
        std::string reply_body = "error unknown request: " + message;
        zmq::message_t reply(reply_body.size()+1);
        memcpy(reply.data(), reply_body.c_str(), reply_body.size()+1);
        init_socket_.send(reply);
    }
}

void NetworkTable::Server::HandleRequest(socket_ptr socket) {
    zmq::message_t message;
    socket->recv(&message);

    // First check to see if the client wanted to disconnect from the server.
    if (strcmp(static_cast<char*>(message.data()), "disconnect") == 0) {
        DisconnectSocket(socket);
        return;
    }

    std::string serialized_request(static_cast<char*>(message.data()), \
            message.size());

    NetworkTable::Request request;
    if (!request.ParseFromString(serialized_request)) {
        std::cout << "Error parsing message\n";
        return;
    }

    switch (request.type()) {
        case NetworkTable::Request::SETVALUES: {
            if (request.has_setvalues_request()) {
                SetValues(request.setvalues_request());
            }
            break;
        }
        case NetworkTable::Request::GETVALUES: {
            if (request.has_getvalues_request()) {
                GetValues(request.getvalues_request(), socket);
            }
            break;
        }
        case NetworkTable::Request::SUBSCRIBE: {
            if (request.has_subscribe_request()) {
                Subscribe(request.subscribe_request(), socket);
            }
            break;
        }
        case NetworkTable::Request::UNSUBSCRIBE: {
            if (request.has_unsubscribe_request()) {
                Unsubscribe(request.unsubscribe_request(), socket);
            }
            break;
        }
        default: {
            std::cout << "Don't know how to handle request: "\
                      << request.type()\
                      << std::endl;
        }
    }
}

void NetworkTable::Server::SetValues(const NetworkTable::SetValuesRequest &request) {
    for (auto const &entry : request.values()) {
        std::string key = entry.first;
        NetworkTable::Value value = entry.second;
        SetValueInTable(key, value);
    }
}

void NetworkTable::Server::GetValues(const NetworkTable::GetValuesRequest &request, \
            socket_ptr socket) {
    auto *getvalues_reply = new NetworkTable::GetValuesReply();
    auto *mutable_nodes = getvalues_reply->mutable_nodes();

    for (int i = 0; i < request.keys_size(); i++) {
        std::string key = request.keys(i);
        NetworkTable::Node node = GetNodeFromTable(key);
        (*mutable_nodes)[key] = node;
    }

    NetworkTable::Reply reply;
    reply.set_type(NetworkTable::Reply::GETVALUES);
    reply.set_allocated_getvalues_reply(getvalues_reply);

    SendReply(reply, socket);
}

void NetworkTable::Server::Subscribe(const NetworkTable::SubscribeRequest &request, \
            socket_ptr socket) {
    subscriptions_table_[request.key()].insert(socket);
}

void NetworkTable::Server::Unsubscribe(const NetworkTable::UnsubscribeRequest &request, \
            socket_ptr socket) {
    subscriptions_table_[request.key()].erase(socket);
}

void NetworkTable::Server::DisconnectSocket(socket_ptr socket) {
    // Make sure to remove any subscriptions this socket had.
    // Without this, the server will still try to send
    // updates to the socket.
    for (auto &entry : subscriptions_table_) {
        entry.second.erase(socket);
    }

    {
        // Remove the socket from our list of sockets to poll
        // and delete it.
        auto it = std::find(sockets_.begin(), sockets_.end(), socket);
        if (it != sockets_.end()) {
            sockets_.erase(it);
        }
    }
}

NetworkTable::Value NetworkTable::Server::GetValueFromTable(std::string key) {
    try {
        NetworkTable::Node node = values_.GetNode(key);
        return node.value();
    } catch (NetworkTable::NodeNotFoundException &e) {
        // If the value wasn't found, create a new value
        // of type NONE and return that to the client.
        NetworkTable::Value value;
        value.set_type(NetworkTable::Value::NONE);
        return value;
    }
}

NetworkTable::Node NetworkTable::Server::GetNodeFromTable(std::string key) {
    try {
        NetworkTable::Node node = values_.GetNode(key);
        return node;
    } catch (NetworkTable::NodeNotFoundException &e) {
        // If the node wasn't found, create a new node
        // with value of type NONE and return that to the client.
        NetworkTable::Node node;
        NetworkTable::Value *value = new NetworkTable::Value();
        value->set_type(NetworkTable::Value::NONE);
        node.set_allocated_value(value);
        return node;
    }
}

void NetworkTable::Server::SetValueInTable(std::string key, \
        const NetworkTable::Value &value) {
    values_.SetNode(key, value);

    // When the table has changed, make sure to
    // notify anyone who subscribed to that key.
    NotifySubscribers(key, value);
}

void NetworkTable::Server::NotifySubscribers(std::string key, \
        const NetworkTable::Value &value) {
    // Construct the subscribe reply (update) message for the key
    auto *subscribe_reply = new NetworkTable::SubscribeReply();
    subscribe_reply->set_allocated_value(new NetworkTable::Value(value));
    subscribe_reply->set_key(key);
    NetworkTable::Reply reply;
    reply.set_type(NetworkTable::Reply::SUBSCRIBE);
    reply.set_allocated_subscribe_reply(subscribe_reply);

    // Get list of subscriptions (sockets) for the key
    std::set<socket_ptr> subscription_sockets = subscriptions_table_[key];
    // Send the update message to each socket
    for (const auto& socket : subscription_sockets) {
        SendReply(reply, socket);
    }
}

void NetworkTable::Server::SendReply(const NetworkTable::Reply &reply, socket_ptr socket) {
    std::string serialized_reply;
    reply.SerializeToString(&serialized_reply);

    zmq::message_t message(serialized_reply.length());
    memcpy(message.data(), serialized_reply.data(), serialized_reply.length());
    socket->send(message, ZMQ_DONTWAIT);
}
