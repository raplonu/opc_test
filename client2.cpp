/* This work is licensed under a Creative Commons CCZero 1.0 Universal License.
 * See http://creativecommons.org/publicdomain/zero/1.0/ for more information. */

#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/client_subscriptions.h>
#include <open62541/plugin/log_stdout.h>

#include <array.hpp>

#include <stdlib.h>
#include <stdio.h>

static void
handler_TheAnswerChanged(UA_Client *client, UA_UInt32 subId, void *subContext,
                         UA_UInt32 monId, void *monContext, UA_DataValue *value) {
    fmt::print("The Answer has changed, new size {}\n", value->value.arrayLength);

    ArrayHandle<int> array(value->value, value->value.arrayLength);

    fmt::print("Values are: {}\n", array);
}


int main(int argc, char *argv[]) {
    /* Create a client and connect */
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

    /* Create a subscription */
    UA_CreateSubscriptionRequest request = UA_CreateSubscriptionRequest_default();
    UA_CreateSubscriptionResponse response = UA_Client_Subscriptions_create(client, request,
                                                                            NULL, NULL, NULL);

    UA_UInt32 subId = response.subscriptionId;
    if(response.responseHeader.serviceResult == UA_STATUSCODE_GOOD)
        printf("Create subscription succeeded, id %u\n", subId);

    UA_MonitoredItemCreateRequest monRequest =
        UA_MonitoredItemCreateRequest_default(my_integer_node_id);

    UA_MonitoredItemCreateResult monResponse =
    UA_Client_MonitoredItems_createDataChange(client, response.subscriptionId,
                                              UA_TIMESTAMPSTORETURN_BOTH,
                                              monRequest, NULL, handler_TheAnswerChanged, NULL);
    if(monResponse.statusCode == UA_STATUSCODE_GOOD)
        printf("Monitoring 'the.answer', id %u\n", monResponse.monitoredItemId);


    // waits forever
    // There is probably a better way to do this...
    while(true) {
        UA_Client_run_iterate(client, 10000000);
    }


    UA_Client_disconnect(client);
    UA_Client_delete(client);
    return EXIT_SUCCESS;
}
