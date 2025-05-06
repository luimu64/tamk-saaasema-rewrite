#include <atomic>
#include <chrono>
#include <json/json.h>
#include <thread>

#include "mqtt.h"
#include "mqtt_settings.h"

std::atomic<unsigned char> tmr_flag;

using namespace std;

void mqtt_loop() {
  int rc;

  mqtt mqtt_handle = mqtt(client_id, host_addr, port_number);

  mosqpp::lib_init();

  while (1) {
    rc = mqtt_handle.loop();

    if (rc) {
      mqtt_handle.reconnect();
    }

    this_thread::sleep_for(chrono::milliseconds(10));
  }

  mosqpp::lib_cleanup();
}

void timer_loop() {
  int time_b = 0;

  while (1) {
    this_thread::sleep_for(chrono::milliseconds(10));
    time_b++;

    if (time_b > 100) {
      tmr_flag = 1;
      time_b = 0;
    }
  }
}

void count_loop(void)

{
  long count = 0;

  while (1) {

    if (tmr_flag == 1) {

      printf(" Count: %ld \n", count);

      count++;
      if (count > 10) {
        count = 0;
      };

      tmr_flag = 0;
    }
  }
  this_thread::sleep_for(chrono::milliseconds(1000));
}

//////////////////////////////////// send URL thread end /////////////////

int main() {
  tmr_flag = 0;

  thread mqtt_th(mqtt_loop);
  thread send_th(count_loop);
  thread tmr_th(timer_loop);

  printf("\n       Subscribed to listen MQTT messages \n");
  printf("       Sending to DB when message arrived \n\n");

  mqtt_th.join();
  send_th.join();
  tmr_th.join();

  return 0;
}
