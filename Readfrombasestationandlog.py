#Read in libraries
# Thanks to https://classes.engineering.wustl.edu/ese205/core/index.php?title=Serial_Communication_between_Raspberry_Pi_%26_Arduino

#!/usr/bin/env python
import time
import serial
import re
import pandas as pd
import datetime
import csv
datetime.datetime.now()


# Read from serial (be careful to check USB port and baudrate
ser = serial.Serial(
        port='/dev/ttyUSB0',
        baudrate = 9600,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
)
#Build dataframe
data = pd.DataFrame()

# Loop to monitor, clean and record data
while 1:
        x=ser.readline()
        #print(x) #Uncomment to see raw input
        x=str(x)
        y = [int(s) for s in re.findall(r'\b\d+\b', x)] #Extracts only numbers
        if len(y)>0:
            print(y) # Prints cleaned data to command line 
            time = datetime.datetime.now()
            v=[time,y[0],y[1], y[2]/1000] #Create data line for time, ID, Voltage and input pin voltage. 
		#Note that pin voltage is multiplied by 1000 on arduino for tranmission
		#Our conversion to TDS will go here 
	
            data = data.append(pd.Series(v), ignore_index=True)
 
            f = open('sensors5.csv','a',newline='')
            wr=csv.writer(f)
            wr.writerow(v)
		#Here we will add our upload to IOT cloud server (Work in progress)
            f.close()

            
            