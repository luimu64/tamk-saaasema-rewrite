#/bin/bash
#	.............................................................
#	:Simple watchdog script to keep the mqtt parser upper online:
#	:By your's truly 10.4.2025				    :
#	.............................................................

while true; do
	if ! ps -A |grep -w "go" > /dev/null; then
		echo "Prosessi kuoli :( uudellenkäynnistetään..."
		./go
	fi
	sleep 5
done
 
