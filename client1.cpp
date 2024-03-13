/* This work is licensed under a Creative Commons CCZero 1.0 Universal License.
 * See http://creativecommons.org/publicdomain/zero/1.0/ for more information. */

#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/client_subscriptions.h>
#include <open62541/plugin/log_stdout.h>

#include <array.hpp>

#include <stdlib.h>
#include <stdio.h>

#include <fmt/core.h>


/**
 * Writes an array to a specified node in an OPC UA client.
 *
 * @param client The OPC UA client.
 * @param node The node ID of the target node.
 * @param handle The handle to the array to be written.
 * @tparam T The type of the array elements.
 */
template<typename T>
void write_array_to(UA_Client *client, UA_NodeId node, ArrayHandle<T>& handle) {
    check_error( UA_Client_writeValueAttribute(client, node, &handle.var) );
}

/**
 * Reads an array of type T from the OPC UA server using the provided client and node ID.
 *
 * @param client The OPC UA client.
 * @param node The node ID of the array to read.
 * @return An ArrayHandle<T> object containing the read array.
 * @throws std::runtime_error if the variant type is invalid.
 */
template<typename T>
ArrayHandle<T> read_array_from(UA_Client *client, UA_NodeId node) {
    UA_Variant var;
    UA_Variant_init(&var);
    check_error( UA_Client_readValueAttribute(client, node, &var) );

    if (!UA_Variant_hasArrayType(&var, data_type_of<T>())) {
        throw std::runtime_error("Invalid variant type");
    }
    return ArrayHandle<T>(var, var.arrayLength);
}

int main(int argc, char *argv[]) {
    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
    /* Connect to a server */
    /* anonymous connect would be: retval = UA_Client_connect(client, "opc.tcp://localhost:4840"); */
    UA_StatusCode retval = UA_Client_connectUsername(client, "opc.tcp://localhost:4840", "user1", "password");
    if(retval != UA_STATUSCODE_GOOD) {
        printf("Could not connect\n");
        UA_Client_delete(client);
        return EXIT_SUCCESS;
    }

    UA_NodeId my_integer_node_id = UA_NODEID_STRING(1, "the.answer");

    ArrayHandle<int> array = read_array_from<int>(client, my_integer_node_id);

    // print all values
    fmt::print("values are: [{}]\n", array);

    fmt::print("Adding 10 elements to array\n");
    // Adds 10 new values.
    for (std::size_t i = 0; i < 10; i++) {
        array.append(i);
    }

    fmt::print("send array\n");
    write_array_to(client, my_integer_node_id, array);

    UA_Client_disconnect(client);
    UA_Client_delete(client);
    return EXIT_SUCCESS;
}
