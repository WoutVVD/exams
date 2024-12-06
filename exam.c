#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MQTTClient.h>
#include "include.h"            //file with #includes

volatile MQTTClient_deliveryToken deliveredtoken;

//structure of tbl
struct tbl
{
    char datum_tijd_stroom[DATE_TIME_LEN]; 
    int tarief_indicator;
    float actueel_stroomverbruik;
    float actueel_spanning;
    float totaal_dagverbruik;
    float totaal_nachtverbruik;
    float totaal_dagopbrengst;
    float totaal_nachtopbrengst;
    char datum_tijd_gas[DATE_TIME_LEN];
    float totaal_gasverbruik;
    struct tbl *next;
};
struct tbl *head = NULL;
struct tbl *current = NULL;

//insert first
void insert_first(struct tbl **head, char *datetime, int rate_indicator, float current_power, float current_voltage, float total_day_consum, float total_night_consum, float total_day_output, float total_night_output, float total_gas_consum)
{
    struct tbl *lk = (struct tbl *)malloc(sizeof(struct tbl));

    strcpy(lk->datum_tijd_stroom, datetime);
    strcpy(lk->datum_tijd_gas, datetime);

    lk->tarief_indicator = rate_indicator;
    lk->actueel_stroomverbruik, current_power;
    lk->actueel_spanning, current_voltage;
    lk->totaal_dagverbruik, total_day_consum;
    lk->totaal_nachtverbruik, total_night_consum;
    lk->totaal_dagopbrengst, total_day_output;
    lk->totaal_nachtopbrengst, total_night_output;
    lk->totaal_gasverbruik, total_gas_consum;

    lk->next = NULL;

    *head = lk;
}

//insert next
void insert_next(struct tbl *list, char *datetime, int rate_indicator, float current_power, float current_voltage, float total_day_consum, float total_night_consum, float total_day_output, float total_night_output, float total_gas_consum)
{
    struct tbl *lk = (struct tbl *)malloc(sizeof(struct tbl));

    strcpy(lk->datum_tijd_stroom, datetime);
    strcpy(lk->datum_tijd_gas, datetime);

    lk->tarief_indicator = rate_indicator;
    lk->actueel_stroomverbruik, current_power;
    lk->actueel_spanning, current_voltage;
    lk->totaal_dagverbruik, total_day_consum;
    lk->totaal_nachtverbruik, total_night_consum;
    lk->totaal_dagopbrengst, total_day_output;
    lk->totaal_nachtopbrengst, total_night_output;
    lk->totaal_gasverbruik, total_gas_consum;

    lk->next = NULL;

    list->next = lk;
}

// This function is called upon when a message is delivered
void delivered(void *context, MQTTClient_deliveryToken dt) {
    #ifdef DEBUG
        printf("Message with token value %d delivery confirmed\n", dt);
        printf( "-----------------------------------------------\n" );
    #endif    
    deliveredtoken = dt;
}

// This function is called upon when an incoming message from mqtt is arrived
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char *msg = message->payload;
    logfile_write(msg);

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

//write to logfile
void logfile_write(char error_in[MAX_MSG_LEN]){
    FILE *logfile;
    logfile = fopen(logFilepath, "a");
    fprintf(logfile, "%s\n", error_in);
    fclose(logfile);
}

//print on cmd/terminal
void cmdPrint_all(struct tbl **list){
    
    struct tbl *temp = head;

    //print header
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
           "Elektriciteit- en gas verbruik - totalen per dag\n"
           "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n"   //extra empty line
           "STARTWAARDEN\n\n"                                                                              //extra empty line
           "DATUM - TIJD:  %s\n"
           "DAG \t Totaal verbruik \t = %f kWh\n"
           "DAG \t Totaal opbrengst \t = %f kWh\n"
           "NACHT \t Totaal verbruik \t = %f kWh\n"
           "NACHT \t Totaal opbrengst \t = %f kWh\n"
           "GAS \t Totaal verbruik \t = %f m3\n"
           "------------------------------------------------------------------------------------------\n"
           "TOTALEN\n"
           "------------------------------------------------------------------------------------------\n\n" //extra empty line
           ,temp->datum_tijd_stroom,temp->totaal_dagverbruik,temp->totaal_dagopbrengst,temp->totaal_nachtverbruik,temp->totaal_nachtopbrengst,temp->totaal_gasverbruik
    );



    //calculate totals
    float total_consum = temp->totaal_dagverbruik + temp->totaal_nachtverbruik;
    float total_output = temp->totaal_dagopbrengst + temp->totaal_nachtopbrengst;

    //print data
    printf("Datum: %s\n"
           "---------------\n"
           "STROOM:\n"
           "\t\t Totaal verbruik \t = %f kWh\n"
           "\t\t Totaal opbrengs \t = %f kWh\n"
           "GAS:\n"
           "\t\t Totaal verbruik \t = %f kWh\n"
           "*\n"
           ,total_consum,total_output,temp->totaal_gasverbruik
    );

    //print footer
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
           "Einde van dit rapport\n"
           "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
    );
}

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
