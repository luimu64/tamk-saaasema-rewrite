
//////////////////////////////////////  5.3.2021 //////////////////////////////
///
///	g++ main.cpp SendTamk.cpp  mqtt_arduino1.cpp -std=c++0x -pthread
///-lmosquitto -lmosquittopp -lcurl -ljsoncpp
///
///////////////////////////////////////////////////////////////////////////////

#include <signal.h>
#include <sys/time.h>

#include <atomic>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include <fstream>
#include <json/json.h>

#include <pthread.h>
#include <thread>
#include <time.h>
#include <unistd.h>

#include "SendTamk.h"
#include "mqtt.h"
#include "mqtt_arduino1.h"

long count = 0;

std::atomic<long> time_i;

std::atomic<long> time_b;
std::atomic<unsigned char> tmr_flag;

using namespace std;

///////////////////////////////////////////////////////////////
/////
/////		MQTT thread
/////
///////////////////////////////////////////////////////////////

void mqtt_loop() {
  int rc;

  mqtt *mqttHdl_2 = new mqtt(in_topic, host_addr, port_number);
  // mqtt *mqttHdl_2 = new mqtt("IoTTi_out_kuo1", "193.167.167.59", 1883);

  mosqpp::lib_init();

  while (1) {
    usleep(0.5);

    rc = mqttHdl_2->loop();
    if (rc) {
      mqttHdl_2->reconnect();
      usleep(1e5);
    }
  }

  mosqpp::lib_cleanup();
}

//////////////////////////////////// mqtt thread end /////////////

//////////////////////////////////////////////////////////////////
/////
/////		Timer thread
/////
/////////////////////////////////////////////////////////////////

/////////////////////////////////// timer routine ///////////////

void timer_routine(int signum) {
  long a;
  time_b++;

  if (time_b > 100) {
    time_b = 0;
    tmr_flag = 1;
  }; // DB time sending rate  = 10 sec

  a = time_b;
  time_i = a;
  usleep(1e4);
}

void timer_start() {
  struct sigaction sa;
  struct itimerval timer;

  /* Install timer_handler as the signal handler for SIGVTALRM. */
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = &timer_routine;
  sigaction(SIGALRM, &sa, NULL);

  /* Configure the timer to expire after 250 msec... */
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 100000;

  /* ... and every 3 000 msec after that. */
  timer.it_interval.tv_sec = 0;      // Timer time set sec
  timer.it_interval.tv_usec = 10000; // Timer time set usec

  /* Start a virtual timer. It counts down whenever this process is executing.
   */
  setitimer(ITIMER_REAL, &timer, NULL);

  while (1)
    ;
  usleep(10e4);
}

//////////////////////////////////// timer thread end /////////////////

///////////////////////////////////////////////////////////////////////////
////////
///////			Send URL
///////
//////////////////////////////////////////////////////////////////////////

void count_loop(void)

{
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
  usleep(10e4);
}

//////////////////////////////////// send URL thread end /////////////////

int main(int argc, char *argv[]) {
  tmr_flag = 0;

  thread mqtt_th(mqtt_loop);
  thread send_th(count_loop);
  thread tmr_th(timer_start);

  printf("\n       Subscribed to listen MQTT messages \n");
  printf("       Sending to DB when message arrived \n\n");

  mqtt_th.join();
  send_th.join();
  tmr_th.join();

  return 0;
}
