#!/usr/bin/env python
import time
import serial
import re
import pandas as pd
import datetime
import csv
datetime.datetime.now()
import paho.mqtt.publish as publish
import psutil
import string

# The ThingSpeak Channel ID.
# Replace <YOUR-CHANNEL-ID> with your channel ID.
channel_ID = "<YOUR-CHANNEL-ID>"

# The hostname of the ThingSpeak MQTT broker.
mqtt_host = "mqtt3.thingspeak.com"

# Your MQTT credentials for the device
mqtt_client_ID = "XXXXXXXXX"
mqtt_username  = "XXXXXXXXX"
mqtt_password  = "XXXXXXXXX"

t_transport = "websockets"
t_port = 80

# Create the topic string.
topic = "channels/" + channel_ID + "/publish"



ser = serial.Serial(
        port='/dev/ttyUSB0',
        baudrate = 9600,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
)
data = pd.DataFrame()

while 1:
        x=ser.readline()
        #print(x)
        x=str(x)
        y = [int(s) for s in re.findall(r'\b\d+\b', x)]
        if len(y)>0:
            print(y)
            reading=1
            time = datetime.datetime.now()
            v=[time,y[0],y[1], y[2]/100]
            data = data.append(pd.Series(v), ignore_index=True)
 
            f = open('sensors5.csv','a',newline='')
            wr=csv.writer(f)
            wr.writerow(v)
            f.close()
            payload = "field1=" + str(v[0]) + "&field2=" + str(v[1]) + "&field3=" + str(v[2]) +"&field4=" + str(v[3]) 
            print ("Writing Payload = ", payload," to host: ", mqtt_host, " clientID= ", mqtt_client_ID, " User ", mqtt_username, " PWD ", mqtt_password)
            publish.single(topic, payload, hostname=mqtt_host, transport=t_transport, port=t_port, client_id=mqtt_client_ID, auth={'username':mqtt_username,'password':mqtt_password})
 
            
            