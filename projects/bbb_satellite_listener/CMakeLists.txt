# Set a variable for commands below
set(PROJECT_NAME bbb_satellite_listener)

# Define your project and language
project(${PROJECT_NAME} CXX)

# Define the source code
set(${PROJECT_NAME}_SRCS 
    main.cpp
        )

# Define the executable
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS})
find_package(Boost REQUIRED COMPONENTS thread)
target_link_libraries(${PROJECT_NAME} Boost::thread ${Boost_libraries} ${ZMQ_LIBRARIES} ${PROTOBUF_LIBRARIES} nt_client)
