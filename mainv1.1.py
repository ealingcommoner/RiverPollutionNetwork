from machine import ADC,Pin
import onewire, ds18x20
import uos
import utime

network = 1 # 1=EE 2=Vodafone
ID = 151
Vsys = machine.ADC(29)

conversion_factor = (3.3 / (65535) )
cal=1

if ID==151:
    apikey= "WQQ3FFZWPIPEAPEO" #151 key
elif ID == 152:
    apikey= "PKWF9JQZ1EC5IXCS" #152 key
host="api.thingspeak.com"

#setup pins
led = Pin(25, Pin.OUT)
uart0 = machine.UART(0, baudrate=9600,rx=Pin(17), tx=Pin(16))   #model control
tds_probe = Pin(22, Pin.OUT) #probe power
temp_probe = Pin(19, Pin.OUT) #probe power
ds_pin = machine.Pin(18) #temp readings
DTR_pin = Pin(20, Pin.OUT) #governs modem sleep
adc=ADC(26) # tds readings

#power off pins
tds_probe.off()
temp_probe.off()
DTR_pin.off()

#2 sec timeout is arbitrarily chosen
def sendCMD_waitResp(cmd, uart=uart0, timeout=2000):
    print("CMD: " + cmd)
    uart.write(cmd+"\r\n")
    waitResp(uart, timeout)
    print()
    
def waitResp(uart=uart0, timeout=2000):
    prvMills = utime.ticks_ms()
    resp = b""
    while (utime.ticks_ms()-prvMills)<timeout:
        if uart.any():
            resp = b"".join([resp, uart.read()])
            
            if resp != '':
            #if (True):
                print(resp.decode())

def calculate_TDS(AV, T, Cal):
    comp = 1+(0.02*(T-25))
    compV = AV / comp
    TDS=(133.42*compV*compV*compV - 255.86*compV*compV + 857.39*compV)*0.5
    TDS= TDS*Cal
    return TDS    

def gettdsvoltage(adc):
    tds_probe.on()
    utime.sleep(0.1)
    readings=[]
    while(len(readings)<10):
    
        volt= adc.read_u16()* conversion_factor
        readings.append(volt)
        utime.sleep(0.1)
    tds_probe.off()
    averagereadings= sum(readings)/len(readings)
    return(averagereadings)

def gettemperature():
    temp_probe.on()
    utime.sleep(0.1)
    ds_sensor = ds18x20.DS18X20(onewire.OneWire(ds_pin))
    roms = ds_sensor.scan()
    ds_sensor.convert_temp()
    utime.sleep(0.1)
    for rom in roms:
        temp_reading= ds_sensor.read_temp(rom)
    temp_probe.off()
    return(temp_reading)
    

def collect_readings(adc,cal, conversion_factor):
    outputs= []
    system_voltage = Vsys.read_u16() * conversion_factor *3
    outputs.append(system_voltage)
    
    temp_reading = gettemperature() 
    outputs.append(temp_reading)

    tddvoltage=gettdsvoltage(adc)
    tds= calculate_TDS(tddvoltage, temp_reading, cal)
    outputs.append(tds)
    
    sensor_temp = machine.ADC(4)
    system_temp = sensor_temp.read_u16() * conversion_factor
    system_temp = 27 - (system_temp - 0.706)/0.001721
    outputs.append(system_temp)
    
    return outputs

def makeurl(readings,apikey,host):
    url= "http://"+host+"/update?api_key="+apikey
    n=1
    for reading in readings:
        url= url+"&field"+str(n)+"="+str(reading)
        n=n+1
    return url

def modemwake():
    DTR_pin.on()
    sendCMD_waitResp("AT", timeout=10)
    sendCMD_waitResp("AT+CSCLK=1")
    DTR_pin.off()
    
def modemsleep():
    DTR_pin.on()
    sendCMD_waitResp("AT+CSCLK=1")
    utime.sleep(2)
    DTR_pin.off()

def posttothing(network, url):
    modemwake()
    utime.sleep(10)
    post_command= 'AT+HTTPPARA="URL","'+url+'"'
    print(post_command)
    sendCMD_waitResp("AT+CFUN=1\r\n") #enable full funct
    sendCMD_waitResp('AT+SAPBR=3,1,Contype,"GPRS"\r\n')
    if network==1:
        sendCMD_waitResp('AT+SAPBR=3,1,APN,"everywhere"\r\n')
    elif network==2:
        sendCMD_waitResp('AT+SAPBR=3,1,APN,"pp.vodafone.co.uk"\r\n')
        sendCMD_waitResp('AT+SAPBR=3,1,USER,"wap"\r\n')
        sendCMD_waitResp('AT+SAPBR=3,1,PWD,"wap"\r\n')
    sendCMD_waitResp('AT+SAPBR=1,1\r\n') #get network info
    utime.sleep(1)
    sendCMD_waitResp('AT+HTTPINIT')
    sendCMD_waitResp(post_command)
    utime.sleep(0.1)
    sendCMD_waitResp('AT+HTTPPARA=”CID”,1\r\n')
    sendCMD_waitResp('AT+HTTPACTION=0\r\n')
    sendCMD_waitResp('AT+HTTPREAD\r\n')
    utime.sleep(0.1)
    sendCMD_waitResp('AT+HTTPTERM\r\n')
    sendCMD_waitResp('AT+SAPBR=0,1\r\n')
    modemsleep()
    
    
utime.sleep(12)
while(True):

    led.toggle()
    try:
        readings= collect_readings(adc, cal, conversion_factor)
        print(readings)
        url=makeurl(readings,apikey,host)
        print(url)
        posttothing(network, url)
    except:
        print("Error")
    led.off()
    utime.sleep(0.5)
    led.on()
    utime.sleep(0.5)
    led.off()
    utime.sleep(0.5)
    led.on()
    utime.sleep(0.5)
    led.off()    
    utime.sleep(60*30)