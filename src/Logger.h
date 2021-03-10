// Copyright 2021 UBC Sailbot

#ifndef LOGGER_H_
#define LOGGER_H_

// Boost includes
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/expressions.hpp>

#include <string>

#include <Exceptions.h>
#include <Connection.h>

namespace NetworkTable {
    class Logger {
    public:
        // Constructor
        Logger(bool text_logging);

        /*
         * Different levels of logging severity
         * 
         * @param msg - error message passed in by caller
        */
        void Trace(std::string msg); // Trace log
        void Debug(std::string msg); // Debug log
        void Info(std::string msg); // Info log
        void Warning(std::string msg); // Warning log
        void Error(std::string msg); // Error log
        void Fatal(std::string msg); // Fatal log

        /*
         * Setter for Network Table connection
         * 
         * @param connection 
        */
        void SetNTConn(NetworkTable::Connection conn);

    private:
        // Init function to set up boost log paramters
        void Init();

        /*
        * Generates a protobuf error object
        */
        NetworkTable::Value GenPBErr(std::string msg, int severity);

        NetworkTable::Connection nt_conn;
        std::string file_path;

    };
}


#endif  // LOGGER_H_
