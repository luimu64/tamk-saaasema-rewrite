#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <string>

#include "mqtt.h"
#include "mqtt_settings.h"
#include "send_to_rest.h"

mqtt::mqtt(const char *id, const char *host, int port) : mosquittopp(id) {

  printf("****   MQTT start connect **** \n");

  connect(host, port, 60);
};

void mqtt::on_connect(int rc) // ON connect callback
{
  printf("****   MQTT Connected with code= %d  ****\n", rc);
  if (rc == 0) {

    /* Only attempt to subscribe on a successful connect. */

    subscribe(NULL, in_topic); // subcribe to in_topic

    printf("****   MQTT subscription to topic = ");
    printf(in_topic);
    printf("    ****\n");
  }
}

void mqtt::on_subscribe(
    [[maybe_unused]] int mid, [[maybe_unused]] int qos_count,
    [[maybe_unused]] const int *granted_qos) // on subcribe callback
{
  printf("****   MQTT Topic subscription succeeded.   ****\n");
}

// on message callback
void mqtt::on_message(const struct mosquitto_message *message) {
  printf("\n ===================  Message received  "
         "================================ \n");

  std::string mqtt_message(static_cast<const char *>(message->payload),
                           message->payloadlen);

  printf("\n 	Topic   = %s", message->topic);         // print message topic
  printf("\n 	Message = %s\n", mqtt_message.c_str()); // print message payload

  if (mqtt_message.size() >= 5 && mqtt_message[0] == 'I' &&
      mqtt_message[1] == 'O' && mqtt_message[2] == 'T' &&
      mqtt_message[3] == 'J' && mqtt_message[4] == 'S') /// ICT course
  {
    process_message(mqtt_message);
  };
}

// reformate and send MQTT message to REST API as JSON

void mqtt::process_message(std::string mqtt_message) {
  std::string json_in;
  std::string parse_string;
  Json::Reader reader;
  Json::Value obj;

  //
  //	example of incoming MQTT message
  //
  // 	IOTJS={"S_name":"Temp1","S_value":3.13}
  //

  json_in = mqtt_message.substr(6); // remove "IOTJS=" from message

  printf("\n JSON Message = %s", json_in.c_str()); // print JSON message

  // try parsing and skip processing if not in JSON format
  if (reader.parse(json_in, obj) == true) {

    std::string sensor_name = check_validity<std::string>(obj, "S_name", false);
    float sensor_value = check_validity<float>(obj, "S_value", true);

    printf(" \n\n Measurement : %s", sensor_name.c_str());
    printf(" \n\n Value : %f", sensor_value);

    // quit if either value is not valid
    if (sensor_value == -2000 || sensor_name == "error") {
      std::cout << " \n\n  ---- Error in JSON parsing ---- " << std::endl;
      return;
    }

    printf("\n\n 	Parsing done OK ");

    std::string json_out =
        "{\"" + sensor_name + "\":" + std::to_string(sensor_value) + "}";

    printf(" \n\n JSON to db  = %s \n",
           json_out.c_str()); // print json string

    // send JSON to REST API to be saved in db
    send_to_Tamk_REST_API("http://webapi19sa-1.course.tamk.cloud/v1/weather",
                          json_out);
  }
}
