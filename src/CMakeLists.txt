###########
# SOURCES #
###########

set(NT_SERVER_SRCS
        Server.cpp
        Help.cpp
        )

set(NT_SERVER_HDRS
        Server.h
        Help.h
        )

set(NT_CLIENT_SRCS
        Connection.cpp
        Help.cpp
        NonProtoConnection.cpp
        Logger.cpp
        )

set(NT_CLIENT_HDRS
        Connection.h
        Help.h
        NonProtoConnection.h
        Logger.h
        )

# just manually go into the protofiles subrepo
# and compile our protofiles, instead of
# using the cmakelists file inside protofiles.
# if we used it, we would compile some random
# files we don't need, and compile time
# on the BBB is already slow enough
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
    protofiles/network_table/ActuationAngle.proto
    protofiles/network_table/Node.proto
    protofiles/network_table/Reply.proto
    protofiles/network_table/Request.proto
    protofiles/network_table/SetValuesRequest.proto
    protofiles/network_table/GetNodesReply.proto
    protofiles/network_table/GetNodesRequest.proto
    protofiles/network_table/Satellite.proto
    protofiles/network_table/Sensors.proto
    protofiles/network_table/SubscribeRequest.proto
    protofiles/network_table/SubscribeReply.proto
    protofiles/network_table/Uccms.proto
    protofiles/network_table/UnsubscribeRequest.proto
    protofiles/network_table/Value.proto
    protofiles/network_table/ErrorReply.proto
    )

add_library(nt_server STATIC ${NT_SERVER_SRCS} ${NT_SERVER_HDRS} ${PROTO_SRCS} ${PROTO_HDRS})
add_library(nt_client STATIC ${NT_CLIENT_SRCS} ${NT_CLIENT_HDRS} ${PROTO_SRCS} ${PROTO_HDRS})
target_compile_definitions(nt_client PUBLIC)
target_compile_definitions(nt_server PUBLIC)

add_subdirectory(python)

# Executing network-table/scripts/generate_frameID_json.py
# Generated file will be in network-table/src/frame_ids.json
execute_process(COMMAND python3 generate_frameID_json.py WORKING_DIRECTORY "../scripts/utility")
