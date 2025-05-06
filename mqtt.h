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
  void ICT_rest(std::string mqtt_message);
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
