#pragma once

#include <json/value.h>
#include <math.h>
#include <mosquittopp.h>
#include <string>

class mqtt : public mosqpp::mosquittopp {
public:
  mqtt(const char *id, const char *host, int port);
  ~mqtt() {};

  void on_connect(int rc);
  void on_message(const struct mosquitto_message *message);
  void on_subscribe(int mid, int qos_count, const int *granted_qos);
  void process_message(std::string mqtt_message);

  template <typename T>
  T check_validity(const Json::Value &obj, std::string name, bool number) {
    if constexpr (std::is_floating_point_v<T>) {
      if (number && obj[name].isNumeric()) {
        return obj[name].asFloat();
      } else {
        return static_cast<T>(-2000); // e.g., -2000.0f
      }
    } else if constexpr (std::is_same_v<T, std::string>) {
      if (!number && obj[name].isString()) {
        return obj[name].asString();
      } else {
        return "error";
      }
    }
  }
};

//
// struct mosquitto_message{
//	int mid;
//	char *topic;
//	void *payload;
//	int payloadlen;
//	int qos;
//	bool retain;
//};
//
