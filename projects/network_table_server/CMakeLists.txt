# Set a variable for commands below
set(PROJECT_NAME network_table_server)

# Define your project and language
project(${PROJECT_NAME} CXX)

# Define the source code
set(${PROJECT_NAME}_SRCS 
    main.cpp
        )

# Define the executable
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS})
target_link_libraries(${PROJECT_NAME} ${ZMQ_LIBRARIES} 
    ${PROTOBUF_LIBRARIES} ${Boost_SYSTEM_LIBRARY} 
    ${Boost_FILESYSTEM_LIBRARY} ${Boost_SERIALIZATION_LIBRARY}
    nt_server)
