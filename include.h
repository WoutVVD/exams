#define ADDRESS     "tcp://192.168.0.106:1883"                  // !! IP-address:Port-ID
#define CLIENTID    "RaspberryPiClient"                         // Unique client ID
#define SUB_TOPIC   "ExamWout"                                  // MQTT topic
#define QOS         1                                           // Quality of Service level
#define TIMEOUT     10000L                                      // Timeout in milliseconds

#define logFilepath    "./logging.log"                           //path to log file
#define valuesFilepath "./values.txt"                            //path to values file