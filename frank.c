#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MQTTClient.h>
#include "include.h"            //file with #includes

volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt) {
    #ifdef DEBUG
        printf("Message with token value %d delivery confirmed\n", dt);
        printf( "-----------------------------------------------\n" );
    #endif    
    deliveredtoken = dt;
}

// This function is called upon when an incoming message from mqtt is arrived
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char *error_in = message->payload;

    printf( "Msg in : <%s>\n", error_in );

    // Create a new client to publish the message
    MQTTClient client = (MQTTClient)context;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    
    return 1;
}

//connection lost
void connlost(void *context, char *cause) {
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

// This is the main function:
// A infenite loop doing:
// (1) using the startup parameter to set the language file name to be read
// (2) read the error file and put the error code & error message in a linked list
// (3) Open MQTT Client for listening
//     Set MQTT msgarrvd function => This function is called upon when a message arrives
//     In this function, read the incoming message, format the outgoing message and send it out
// 
int main() {
   // Open MQTT client for listening
    
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    MQTTClient_setCallbacks(client, client, connlost, msgarrvd, delivered);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    MQTTClient_subscribe(client, SUB_TOPIC, QOS);

    // Keep the program running to continue receiving and publishing messages
    for(;;) {
        delay(1);
    }

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}