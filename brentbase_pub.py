# -*- coding: utf-8 -*-
"""
Created on Fri Mar 25 15:51:31 2022

@author: johnn
"""
#!/usr/bin/env python
import time
import datetime

datetime.datetime.now()
import paho.mqtt.publish as publish

import urllib.request as urllib2
import json
import paho.mqtt.subscribe as subscribe
import re
# Import Python System Libraries
# Import Blinka Libraries
import busio
from digitalio import DigitalInOut, Direction, Pull
import board
# Import the SSD1306 module.
import adafruit_ssd1306
# Import RFM9x
import adafruit_rfm9x


# Create the I2C interface.
i2c = busio.I2C(board.SCL, board.SDA)

# 128x32 OLED Display
reset_pin = DigitalInOut(board.D4)
display = adafruit_ssd1306.SSD1306_I2C(128, 32, i2c, reset=reset_pin)
# Clear the display.
display.fill(0)
display.show()
width = display.width
height = display.height

# Configure LoRa Radio
CS = DigitalInOut(board.CE1)
RESET = DigitalInOut(board.D25)
spi = busio.SPI(board.SCK, MOSI=board.MOSI, MISO=board.MISO)
rfm9x = adafruit_rfm9x.RFM9x(spi, CS, RESET, 869.9, baudrate=1000000)
#rfm9x.signal_bandwidth = 31250
#rfm9x.destination= 255  
#rfm9x.node= 255
#rfm9x.coding_rate= 8 #5-8
#rfm9x.spreading_factor= 10 #6-12
#rfm9x_enable_crc= False
rfm9x.tx_power = 23
#for last knonw good Bw125Cr48Sf4096 125000/8/12
# if Bw31_25Cr48Sf512 31250/8/12
prev_packet = None
ID= 0
v =[]
READ_API_KEY='XXXXXXXXX' #add key and channel ID
CHANNEL_ID=XXXXXXX
#SSID=$(/sbin/iwgetid --raw)
LKT= 15 # Last known temp in case temp cannot be obtained. Seed with a close value to the water you are observing
K996 = 0.704441
K998 = 0.667664
#Obtain temperature from thingspeak. Note that it will return LKT if cannot be obtained
def gettemp(LKT):
    try:
        conn = urllib2.urlopen("http://api.thingspeak.com/channels/%s/feeds/last.json?api_key=%s" \
                           % (CHANNEL_ID,READ_API_KEY))
        response = conn.read()
    #print "http status code=%s" % (conn.getcode())
        data=json.loads(response)
        T= data['field7']
        conn.close()
        T= float(T)
    except:
        T=LKT
    return T

#Calculate TDS from average voltage, using temperature and calibration coefficient
def calculate_TDS(AV, T, Cal):
    comp = 1+(0.02*(T-25))
    compV = AV / comp
    TDS=(133.42*compV*compV*compV - 255.86*compV*compV + 857.39*compV)*0.5
    TDS= TDS*Cal
    return TDS    

# The ThingSpeak Channel ID.
# Replace <YOUR-CHANNEL-ID> with your channel ID.
channel_ID = "1668767"

# The hostname of the ThingSpeak MQTT broker.
mqtt_host = "mqtt3.thingspeak.com"

# Your MQTT credentials for the device
mqtt_client_ID = "Hx4YBDYeBTYnJBg8GRggNS8"
mqtt_username  = "Hx4YBDYeBTYnJBg8GRggNS8"
mqtt_password  = "ths8m4ebTNLJggjselxOTyv2"

t_transport = "websockets"
t_port = 80

# Create the topic string.
topic = "channels/" + channel_ID + "/publish"

#Not used, but if you don't have a workign temp prob you could use this to obtain a temperature value from a nearby sensor
def henley():
    msg = subscribe.simple("sensor/thames1/temp/river", hostname="dl1.findlays.net")
    temp= str(msg.payload)
    tempnum = [int(s) for s in re.findall(r'\b\d+\b', temp)]
    return tempnum[0]

display.fill(0)
display.text('Brent LoRa', 35, 0, 1)
now = datetime.datetime.now()
current_time = now.strftime("%H:%M:%S")
while True:
    packet = None
    #time = datetime.now()
    # draw a box to clear the image


    # check for packet rx
    packet = rfm9x.receive()
    if packet is None:
        display.show()
        display.text(str(v), 15, 0, 1)
        display.text(str(current_time), 15, 20, 1)
        
        #display.text(str(ID), 35, 0, 1)
    else:
        # Display the packet text and rssi
        now = datetime.datetime.now()
        current_time = now.strftime("%H:%M:%S")
        display.fill(0)
        prev_packet = packet
        try: 
            packet_text = str(prev_packet, "utf-8")
            print(packet_text)
            print(rfm9x.last_rssi)
        except:
            print("packet error")
            display.text("packet error", 35, 0, 1)
        #display.text('RX: ', 0, 0, 1)
        #display.text(packet_text, 25, 0, 1)
        time.sleep(1)

        x=packet_text 
        y = [int(s) for s in re.findall(r'\b\d+\b', x)]
        if len(y)>2:
            print(y)

            ID = y[0]
            reading = y[1]/10000 #prpbe multiples value to create a text sting, reverse it here
            battery = y[2]/100
            v=[ID,reading, battery]
            display.text(str(v[0]), 35, 0, 1)
            #idenfity station by ID and prepare payload to send
            if ID == 996:
                T = float(gettemp(LKT))
                print(T)
                readinga = calculate_TDS(reading, T, K996)
                payload = "field1=" + str(readinga) + "&field2=" + str(battery) + "&field5=" + str(rfm9x.last_rssi) + "&field7=" + str(LKT)
            if ID == 998:
                T = float(gettemp(LKT))
                print(T)
                readinga = calculate_TDS(reading, T, K998)
                payload = "field3=" + str(readinga) + "&field4=" + str(battery)+ "&field6=" + str(rfm9x.last_rssi) + "&field7=" + str(LKT)
            #if ID == 997:
            #    payload = "field5=" + str(reading) + "&field6=" + str(battery)
            if ID == 986:
                T = reading*10
                payload = "field7=" + str(T) + "&field8=" + str(battery)
                LKT= T #refresj last known temperature
            #print(time)
        print ("Writing Payload = ", payload," to host: ", mqtt_host, " clientID= ", mqtt_client_ID, " User ", mqtt_username, " PWD ", mqtt_password)
        try:    
            publish.single(topic, payload, hostname=mqtt_host, transport=t_transport, port=t_port, client_id=mqtt_client_ID, auth={'username':mqtt_username,'password':mqtt_password})
   
            display.text("Sent", 15, 20, 1)
        except:
            display.text("Failed", 15, 20, 1)
    display.show()
    time.sleep(5)
    display.fill(0)
            
            