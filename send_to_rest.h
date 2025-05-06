#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include <cstring>
#include <curl/curl.h>
#include <iostream>
#include <sstream>

using std::cout;
using std::endl;
using std::string;

using namespace std;

void send_to_Tamk_REST_API(string cl_st, string json);
