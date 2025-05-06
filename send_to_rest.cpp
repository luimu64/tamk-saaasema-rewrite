#include "send_to_rest.h"
#include "mqtt.h"

using namespace std;

void send_to_Tamk_REST_API(string db_url,
                           string json_dat) // create URL with CURL library
{
  CURLcode ret; // URL send return code
  CURL *hnd;    // CURL Handle
  struct curl_slist *slist;

  slist = NULL;
  slist =
      curl_slist_append(slist, "Content-Type: application/json;charset=UTF-8");

  std::string json_db = "{ }";

  //		message example
  //
  //		 {"device_id":"ICT_2021","data":{"Temp1":3.13}}
  //
  //              Add device ID secton to JSON message

  json_db = "{\"device_id\":\"ICT_2021\",\"data\":" + json_dat + "}";

  ///////////////////////////Rest Api  URL created //////////////////////

  printf("\n POST with cURL = %s \n\n", json_db.c_str());

  ////////////////////////// CURL field setting //////////////////

  hnd = curl_easy_init();

  curl_easy_setopt(hnd, CURLOPT_URL, db_url.c_str()); // URL addres to db

  curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);

  curl_easy_setopt(hnd, CURLOPT_HEADER, 1L);

  curl_easy_setopt(hnd, CURLOPT_POSTFIELDS,
                   json_db.c_str()); // JSON data string adding to URL

  int lng = json_db.length();

  curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)lng);

  curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.38.0");
  curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist);
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_SSH_KNOWNHOSTS, "/home/pi/.ssh/known_hosts");
  curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

  ret = curl_easy_perform(hnd);

  printf("\n");
  printf("\n Server Return code =");
  cout << ret << endl;
  printf("\n");

  curl_easy_cleanup(hnd);
  hnd = NULL;

  curl_slist_free_all(slist);
  slist = NULL;
}
