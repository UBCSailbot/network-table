// Copyright 2017 UBC Sailbot
//
// Sends requests to the network table
// Returns the number of errors which occured.

#include "network-table/Connection.h"
#include "Reply.pb.h"
#include "Request.pb.h"
#include "GetValueReply.pb.h"
#include "GetValueRequest.pb.h"
#include "SetValueRequest.pb.h"
#include "Value.pb.h"

#include <iostream>
#include <string>

/*
 * This is a basic "stress test"
 * for the network table server.
 * The total number of errors
 * which occur when querying the network
 * table is returned.
 */
int main() {
    int num_queries = 10;
    int num_errors = 0;

    NetworkTable::Connection connection;

    for (int i = 0; i < num_queries; i++) {
        // SET windspeed to 100
        try {
            NetworkTable::Value *value = new NetworkTable::Value();
            value->set_type(NetworkTable::Value::INT);
            value->set_int_data(100);

            NetworkTable::SetValueRequest *setvalue_request = new NetworkTable::SetValueRequest();
            setvalue_request->set_key("windspeed");
            setvalue_request->set_allocated_value(value);

            NetworkTable::Request request;
            request.set_type(NetworkTable::Request::SETVALUE);
            request.set_allocated_setvalue_request(setvalue_request);

            connection.Send(request);
        } catch (...) {
            num_errors++;
        }
        // GET windspeed
        try {
            NetworkTable::GetValueRequest *getvalue_request = new NetworkTable::GetValueRequest();
            getvalue_request->set_key("windspeed");

            NetworkTable::Request request;
            request.set_type(NetworkTable::Request::GETVALUE);
            request.set_allocated_getvalue_request(getvalue_request);

            connection.Send(request);

            NetworkTable::Reply reply;
            connection.Receive(&reply);

            // There's no point in checking the value of the reply
            // because there is no garuantee that is will be the same
            // value as when we last set it.
        } catch (...) {
            num_errors++;
        }
        // GET garbage
        try {
            NetworkTable::GetValueRequest *getvalue_request = new NetworkTable::GetValueRequest();
            getvalue_request->set_key("garbage");

            NetworkTable::Request request;
            request.set_type(NetworkTable::Request::GETVALUE);
            request.set_allocated_getvalue_request(getvalue_request);

            connection.Send(request);

            NetworkTable::Reply reply;
            connection.Receive(&reply);
        } catch (...) {
            num_errors++;
        }
    }

    return num_errors;
}
