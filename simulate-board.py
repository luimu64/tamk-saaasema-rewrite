import time
import random
import paho.mqtt.client as mqtt

# --- Configuration ---
BROKER = "localhost"               # Same as in your Arduino: byte server[] = {10, 6, 0, 23};
PORT = 1883                        # Standard MQTT port
TOPIC = "ICT4_out_2020"           # Same topic
CLIENT_ID = "24TIETOA"      # Same as Arduino clientId
PASSWORD = "tamk"                 # deviceSecret

# --- MQTT Setup ---
client = mqtt.Client(client_id=CLIENT_ID)
client.username_pw_set(CLIENT_ID, PASSWORD)

client.connect(BROKER, PORT, keepalive=60)

# Alternate messages: direction and speed
suffixes = ["ts", "tn"]
messageDedicator = 0

try:
    while True:
        suffix = suffixes[messageDedicator]
        value = round(random.uniform(0, 100), 2)  # Simulate wind data

        # Construct payload exactly like Arduino
        payload = f'IOTJS={{"S_name":"{suffix}","S_value":{value}}}'

        client.publish(TOPIC, payload)
        print(f"Published: {payload}")

        messageDedicator = 1 - messageDedicator
        time.sleep(5)  # Mimic delay in Arduino
except KeyboardInterrupt:
    client.disconnect()
    print("Disconnected.")
