#include <string>

#include <json/json.h>

using namespace std;

#include "mqtt.h"
#include "mqtt_settings.h"
#include "send_to_rest.h"

mqtt::mqtt(const char *id, const char *host, int port) : mosquittopp(id) {

  printf("****   MQTT start connect ****** \n");

  connect(host, port, 60);
};

void mqtt::on_connect(int rc) // ON connect callback
{
  printf("****   MQTT Connected with code= %d  *****\n", rc);
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

void mqtt::on_message(
    const struct mosquitto_message *message) // on message callback
{
  printf("\n ===================  Message received  "
         "================================ \n");

  std::string mqtt_message(static_cast<const char *>(message->payload),
                           message->payloadlen);

  printf("\n 	Topic   = %s", message->topic); // print message topic

  printf("\n 	Message = %s", mqtt_message.c_str()); // print message payload

  printf("\n");

  if (mqtt_message.size() >= 5 && mqtt_message[0] == 'I' &&
      mqtt_message[1] == 'O' && mqtt_message[2] == 'T' &&
      mqtt_message[3] == 'J' && mqtt_message[4] == 'S') /// ICT course
  {
    ICT_rest(mqtt_message);
  }; // continue to parsing and sending to db section

} // end of on_message callback

////////////////////////////////  Parse and send to db  //////////////////////

void mqtt::ICT_rest(std::string mqtt_message)

{

  Json::Reader reader;
  Json::Value obj;

  //
  //	example of incoming MQTT message
  //
  // 	IOTJS={"S_name":"Temp1","S_value":3.13}
  //

  mqtt_message = mqtt_message.substr(6); // remove "IOTJS=" from message

  std::string json_in; // string to parse
  std::string parse_string;

  json_in = mqtt_message; // copu message to json_in

  printf("\n JSON Message = %s", json_in.c_str()); // print JSON message

  // dummy string start settings

  std::string sensor_name = "sensorname_default"; // sensor name string

  float sensor_value = -2000; // sensor value string

  ////////////     Parse out json tags     ///////////////////////////

  if (reader.parse(json_in, obj) ==
      true) // check if parsing OK mean messagae is JSON format
  {

    //   Check mqtt message content
    //	find name value pairs
    //
    //	{ "S_name" : "temp1" , "S_value" : 3.23 }
    //
    //	S_name
    //	S_value

    /////////////////////////////// signal name /////////////////////
    if (obj["S_name"].isString()) {
      sensor_name = obj["S_name"].asString();
      printf(" \n\n Measurement : %s", sensor_name.c_str());

    } else {
      printf("\n\n!!!Signal name couldn't be parsed!!!\n\n");
    }

    ////////////////////////////// signal value //////////////////////
    // printf("S_value in parser is: ");
    // sensor_value=obj["S_value"].asString();
    // string seppo = obj["S_value"].asString();
    // printf("%s\n",seppo.c_str());

    float seppo = stof(obj["S_value"].asString());
    string typeof_seppo = typeid(seppo).name();

    if (typeof_seppo == "f") {
      sensor_value = seppo;
      printf("\n Sensor_value = %f", sensor_value);
    }
    /*
                    if(obj["S_value"].isDouble())			//old
       stuff below
                    {
                            sensor_value=obj["S_value"].asFloat();
                            printf(" = %f",sensor_value);
                    }
                    else
                    {
                            printf("S_value wasn't double :( ");
                            if(obj["S_value"].isInt())
                            {
                                    sensor_value=obj["S_value"].asInt();
                                    //cout << " // Sensor value Int: " <<
       sensor_value ; printf("%d",sensor_value);
                            }
                    }

    */

    printf("\n\n 	Parsing done OK ");

    ///       		send to db with CURL
    /////////////////////  Create REST-API json string  //////////////////
    //
    // 		{"device_id":"ICT_2021","data":{"Temp1":3.130000}}
    //
    //	create first this:	{"Temp1":3.130000}}
    //

    std::string svalue =
        std::to_string(sensor_value); // convert (float) sensor_value to string

    //  create JSON string from sensor name + sensor value

    std::string json_out = "{\""; // start of JSON string (json_out)

    json_out = json_out + sensor_name + "\":" + svalue +
               "}"; // add sensor name, value and end of JSON

    printf(" \n\n JSON to db  = %s \n", json_out.c_str()); // print json string

    ///////////////////////////////////////   SEND TO DB
    ///////////////////////////////////////////////

    send_to_Tamk_REST_API("http://webapi19sa-1.course.tamk.cloud/v1/weather",
                          json_out); // call url creation function
    //	send_to_Tamk_REST_API_0("http://www.vraminhos.com",json_out);	//
    // Innovators team
    //	send_to_Tamk_REST_API_0("http://bweb17.course.tamk.cloud/api/add-measurement",json_out);
    //// Team Vuoret
    //	send_to_Tamk_REST_API_0("http://bweb62.course.tamk.cloud/api/add-measurement",json_out);
    //	send_to_Tamk_REST_API_0("http://bweb26.course.tamk.cloud/api/add-measurement",json_out);
    //// Team B Positive
    //	send_to_Tamk_REST_API_0("http://bweb61.course.tamk.cloud/api/add-measurement",json_out);
    //// Team One
    //	send_to_Tamk_REST_API_0("http://bweb23.course.tamk.cloud/api/add-measurement",json_out);
    //// The Team vji
    //////////////////////////////////////////////////
    // check which of these are actually up!!!!!!!
    //////////////////////////////////////////////////
  } else // JSON parsing error
  {
    printf(" \n\n  ---- Error in JSON parsing ---- ");
  };

} // end of  ICT_rest(void)
