all:
	g++ main.cpp send_to_rest.cpp mqtt.cpp -std=c++20 -pthread -lmosquitto -lmosquittopp -lcurl -ljsoncpp -o go

debug:
	g++ main.cpp send_to_rest.cpp mqtt.cpp -std=c++20 -pthread -lmosquitto -lmosquittopp -lcurl -ljsoncpp -Wall -Wextra -o debug
