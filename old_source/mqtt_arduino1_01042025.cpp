
///////////////////////////  5.3.2021  KN /////////////////////////////



#include <cstdio>
#include <cstring>
#include <string>
#include <functional>     	// c++11

#include <iostream>        	// due to: cout

#include <fstream>
#include <jsoncpp/json/json.h>
#include <unistd.h>

using std::cout;
using std::endl;
using namespace std; 


#include "mqtt.h"
#include "mqtt_arduino1.h"
#include "SendTamk.h"


std::string mqtt_message;					// mqtt message string

//////////////////////////////////////////////////////////////////////////
//////////
//////////    MQTT
//////////
/////////////////////////////////////////////////////////////////////////

mqtt::mqtt(const char *id, const char *host, int port) : mosquittopp(client_ID)
{
	int keepalive = 60;

	printf("****   MQTT start connect ****** \n");

	connect(host, port, (int) keepalive);
};


////////// ON CONNECT ///////////////////////////////////////////////////


void mqtt::on_connect(int rc)							// ON connect callback
{
	printf("****   MQTT Connected with code= %d  *****\n", rc);
	if(rc == 0)
	{

		/* Only attempt to subscribe on a successful connect. */

		subscribe(NULL, in_topic);					// subcribe to in_topic

		printf("****   MQTT subscription to topic = ");printf(in_topic);
		printf("    ****\n");

	}
}

//////////////////////////  ON SUBSCRIBE ///////////////////////////////////////

void mqtt::on_subscribe(int mid, int qos_count, const int *granted_qos)		// on subcribe callback
{

	printf("****   MQTT Topic subscription succeeded.   ****\n");

}


//////////////////////////// Message received //////////////////////////////////

void mqtt::on_message(const struct mosquitto_message *message)			// on message callback
{
		
        printf("\n ===================  Message received  ================================"); 


	char *ptr_c;

	ptr_c = (char *) message->payload;			// set char pointer ptr_c to message payload

	printf("\n");

	mqtt_message="";					// initiate mqtt message string

	for (int i=0;i<(message->payloadlen+1);i++)		// copy payload to mqtt_message string
	{	
		mqtt_message=mqtt_message + *ptr_c;

		//printf("<%c>",*ptr_c);

		ptr_c++;
	}

				
	printf("\n 	Topic   = %s", message->topic);		// print message topic	

	printf("\n 	Message = %s", mqtt_message.c_str());	// print message payload

	printf("\n"); 
								//
								//	example of incoming MQTT message
								//
								// 	IOTJS={"S_name":"Temp1","S_value":3.1
								//
								// 	ICT course   prefix:  IOTJS
								//

	if((mqtt_message[0]=='I') && (mqtt_message[1]=='O') && (mqtt_message[2]=='T') && (mqtt_message[3]=='J') && (mqtt_message[4]=='S'))  /// ICT course

        	{  ICT_rest();  };				// continue to parsing and sending to db section
		
 		
}	// end of on_message callback				



////////////////////////////////  Parse and send to db  //////////////////////

void mqtt::ICT_rest (void)

{

    Json::Reader reader;
    Json::Value obj;

	//
	//	example of incoming MQTT message
	//
	// 	IOTJS={"S_name":"Temp1","S_value":3.13}

	
	int i=0;
	while(mqtt_message[i]!=0)
	{
          mqtt_message[i]=mqtt_message[i+6];	// remove "IOTJS="  from beging of MQTT message
 	  i++;
	}

	mqtt_message[i]='\0';			// Pure JSON message left to parsing

	
	std::string json_in;			// string to parse
	std::string parse_string;

	json_in=mqtt_message;			// copu message to json_in
				
	printf("\n JSON Message = %s", json_in.c_str());	// print JSON message

						// dummy string start settings

	std::string 	sensor_name="-";	// sensor name string
	
	float 		sensor_value=-2000;	// sensor value string


	////////////     Parse out json tags     ///////////////////////////


 	if(reader.parse(json_in, obj)==true)	// check if parsing OK mean messagae is JSON format
	{     
  

		//   Check mqtt message content 
		//	find name value pairs
		//
		//	{ "S_name" : "temp1" , "S_value" : 3.23 }
		//
		//	S_name   
		//	S_value


		/////////////////////////////// signal name /////////////////////
		if(obj["S_name"].isString())
		{
			sensor_name=obj["S_name"].asString();
			printf(" \n\n Measurement : %s",sensor_name.c_str());
 			
		};


		////////////////////////////// signal value //////////////////////
		if(obj["S_value"].isDouble())
		{
			sensor_value=obj["S_value"].asFloat();
			printf(" = %f",sensor_value);
		}
		else
		{

			if(obj["S_value"].isInt())
			{
				sensor_value=obj["S_value"].asInt();
				//cout << " // Sensor value Int: " << sensor_value ;
				printf("%d",sensor_value);
			}
		};	


			
		printf("\n\n 	Parsing done OK ");


		///       		send to db with CURL  
		/////////////////////  Create REST-API json string  //////////////////
		//
		// 		{"device_id":"ICT_2021","data":{"Temp1":3.130000}}
		//
		//	create first this:	{"Temp1":3.130000}}
		// 

		std:: string svalue =std::to_string(sensor_value);		// convert (float) sensor_value to string

		//  create JSON string from sensor name + sensor value 

		std::string json_out ="{\"";					// start of JSON string (json_out)

		json_out=json_out + sensor_name + "\":" + svalue + "}" ;	// add sensor name, value and end of JSON 

		printf(" \n\n JSON to db  = %s \n",json_out.c_str());		// print json string



///////////////////////////////////////   SEND TO DB ////////////////////////////////////////////		

		send_to_Tamk_REST_API_0("https://webapi19sa-1.course.tamk.cloud/v1/weather",json_out);	// call url creation function	
		send_to_Tamk_REST_API_0("https://iotweb001.course.tamk.cloud/api/v1/weather",json_out);
		//send_to_Tamk_REST_API_0("https://www.vraminhos.com",json_out);	// Innovators team
		send_to_Tamk_REST_API_0("https://bweb22.course.tamk.cloud/api/add-measurement",json_out);   // Innovators-2	
		send_to_Tamk_REST_API_0("https://bweb17.course.tamk.cloud/api/add",json_out);   // Team Vuoret
		send_to_Tamk_REST_API_0("https://bweb62.course.tamk.cloud/api/add-measurement",json_out); 
		send_to_Tamk_REST_API_0("https://bweb26.course.tamk.cloud/api/add-measurement",json_out);   // Team B Positive
        send_to_Tamk_REST_API_0("https://bweb61.course.tamk.cloud/api/add-measurement",json_out);   // Team One
		send_to_Tamk_REST_API_0("https://bweb23.course.tamk.cloud/api/add-measurement",json_out);  //The Team
		send_to_Tamk_REST_API_0("https://bweb13.course.tamk.cloud/api/add-measurement",json_out);
		send_to_Tamk_REST_API_0("https://172.16.5.177:9000/api/add-measurement",json_out);
		send_to_Tamk_REST_API_0("https://bweb47.course.tamk.cloud/api/add-measurement",json_out);	
		send_to_Tamk_REST_API_0("https://embsys666.course.tamk.cloud/api/add-measurement",json_out);		
	
	} else									// JSON parsing error	
	{
		printf(" \n\n  ---- Error in JSON parsing ---- ");
	};
		usleep(500e4);
				

} // end of  ICT_rest(void)









