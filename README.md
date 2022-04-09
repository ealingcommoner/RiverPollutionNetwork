# RiverPollutionNetwork
Building a low-cost community pollution detector for our local river

*Work in Progress*

## Background to project
The River Brent in West London is a little river which runs through a mix of urban industrial areas and the huge Brent River Park in Hanwell. 
![The Brent River in Hanwell](https://github.com/ealingcommoner/RiverPollutionNetwork/blob/main/Brent%20River.jpg)

Every few months we see pollution incidents from a range of causes:
- Deliberate dumping of pollutants
- Storm drains
- Badly connected outlets

These cause damage to the wildlife, particularly the many fish and eels which live in the River. 
We have formed a group which keeps an eye on the River and report pollution incidents so the Environment Agency can send a team to investigate and mitigate the damage. 

But we're a small group and it can take hours or days to notice a problem. Local authorities inform us that instaling an industrial monitoring station can be upwards of £6000 and are reluctant to do so on such a small river. So I wanted to know if a community can build something that will do the job for as little money as possible. 

## Aims
Use low-cost, remotely deployed sensors to alert the community to a pollution incident. 

## Measurement
Having reviewed pollution incident reports, it appears the most common indicators are:
- Total Dissolved Oxygen (TDO)
- Total Dissolved Solids (TDS)

TDO sensors are around £130 each, whilst [this TDS sensor](https://www.dfrobot.com/product-1662.html) is £11. We could consider adding TDO in future if funding permits, but for now it seems TDS gives us the best effect for our money.

![Gravity TDS Sensor](https://github.com/ealingcommoner/RiverPollutionNetwork/blob/main/Gravity.jpg)

## Node
Each node will consist of
- 2 remote TDS sensors
- 1 remote temperature sensor (this is cruicial to calibrate our readings)
- 1 base station

Each remote station requires an ID. I have allocate 3 digit numbers in the system
- First digit: Node letter (9) for our first node
- Second digit: Type of sensor (9 for TDS, 8 for temperature)
- Third digit: Unique number. 
So our TDS sensors are 996 and 998 (it doesn't seem sensible to use '999' as an ID!) and temperature sensor is 986. All this code is configured on this system. 
This means we can add extra classes of devices (7 could be an oxygen sensor and 6 a relay for example). 

## Remote TDS Sensor
For the remote station I chose an [Adafruit Feather 32u4 with LoRa Radio Module](https://learn.adafruit.com/adafruit-feather-32u4-radio-with-lora-radio-module/setup) 
![Radiofruit module](https://cdn-learn.adafruit.com/guides/cropped_images/000/001/273/medium640/thumb2.jpg?1520544037)

It has a low power usage and a built in LoRa radio for long range communications, along with a battery management circuit. Incredibly impressive for just £33. 

In addition we need a battery. I want a pretty powerful battery because ideally we want this to operate for weeks at a time without intervention. I'm avoiding solar panels if possible as they add cost. I went with the [6700mAh High Capacity Lithium Ion Battery](https://shop.pimoroni.com/products/high-capacity-lithium-ion-battery-pack?variant=32012684591187) for the prototype. 

### Assembly
Simply connect lithium battery and connect sensor to GND, 5V and A3 pin for analogue reader. You'll need to solder on:
- Header pins
- Whip antenna (cut wire to 8.2cm for 868MHZ frequency mode)
- Alternatively solder a UFT connector to add an antenna. 

Hey presto, one remote station
![Remote station](https://github.com/ealingcommoner/RiverPollutionNetwork/blob/main/Prototype.jpg)

### Casing
I recommend an IP68 Waterproof junction box like [this one](https://www.amazon.co.uk/dp/B09NFSTPGG/ref=cm_sw_em_r_mt_dp_WT0A1Q1RJGSB5GH1H0KC?_encoding=UTF8&psc=1)
![](https://m.media-amazon.com/images/I/71FYvOoO0XL._SL1500_.jpg)

Ideally this needs to be IP68 in the event of full immersion. 
Junction boxes are:
- Resistant to the elements
- Cheap (£5)
- Discreet. This will be deployed in a public place and relies on safety through obscurity. We want our remote unit to be as boring as possible. 

### Cost
| Item     | Cost |
| ---      | ---       |
| Adafruit Feather 32u4 with LoRa Radio Module | £33         |
| Gravity TDS Sensor | £11         |
| 6700mAh Lithium Batter | £18         |
| Junction box | £5         |
| **Total** | £67         |

This meets our requirements for a low cost sensor. An antenna adds £15 to the price.
I have tested the system and with an antenna sticking through the top space this appears to be waterproof. 

![Finished prototype](https://github.com/ealingcommoner/RiverPollutionNetwork/blob/main/Sensor%20with%20aerial.jpg)

## Code
Install Arduino IDE and ensure that you have the correct board and libraries added. You will need:
- Radiohead library
- Dallas Temperature
- Onewire
- Board is Adafruit Feather 32u4

Set your ID number to agree with the IDs you have included in your base station code and flash [this code](https://github.com/ealingcommoner/RiverPollutionNetwork/blob/main/tdsstation.ino). 
Note that this transmits every 15 minutes, you can alter this to reflect your needs (in the UK you require a 0.1% duty cycle, transmission time is <1s so this is compliant)

## Temperature sensor
As above but using [Waterproof DS18B20-compatible Temperature Sensor](https://shop.pimoroni.com/products/ds18b20-programmable-resolution-1-wire-digital-thermometer?variant=32127344640083)

Again, set a suitable ID and flash [this code](https://github.com/ealingcommoner/RiverPollutionNetwork/blob/main/tempstation.ino). 

## Base station
I didn't like my original base station so have made a new one consisting of a Raspberry Pi Zero 2 and an  [Adafruit Lora Bonnet](https://shop.pimoroni.com/products/adafruit-lora-radio-bonnet-with-oled-rfm95w-915mhz-radiofruit?variant=27912635220051)

### Assembly

Even easier. Add Lora bonnet to pi, add an SD card for the operating system and plug in an antenna. 

## Bill of materials
| Item     | Cost |
| ---      | ---       |
| Raspberry Pi Zero 2W  | £13.50         |
| Adafruit Lora Bonnet| £30.60         |
| 900 Mhz Antenna | £12.30         |
| **Total** | £56.70         |

I particularly like the little screen. As this will be headless we can make good use of that to see the most recent transmissions. 

Note: Make sure all Lora devices are RFM9x as different types of devices cannot communicate. This limits our frequency to around 900Mhz. 

Important: You must make sure that you follow radio frequency laws in your area. In the UK devices must use 863-870Mhz or 433Mhz.

I have used the maximum transmission power allowed (23dB) and the transmission mode Bw125Cr45Sf128. Feel free to change but it is a rabbit hole and I would not recommend revisiting again!

![Final base station](https://github.com/ealingcommoner/RiverPollutionNetwork/blob/main/Base%20station%20zero.jpg)

## Base station code

Set up raspberry Pi as usual. Install libraries from [here](https://learn.adafruit.com/adafruit-radio-bonnets)
[Base station code](https://github.com/ealingcommoner/RiverPollutionNetwork/blob/main/brentbase_pub.py)
Our code is quite complex but uses the ID of each station to put together a payload to send to thingspeak.com. 

Recorded data includes:
- Probe readings (TDS and temperature)
- Battery readings
- Signal strength
You should set up a channel at thingspeak and enter your channel number and API key. 
I know it is best practice to have one channel per device but as I'm planning several nodes I'm considering the ensure node to be one device. 

## Calibration
I have run this in a sealed container with tap water for 12 hours and compared average readings with known readings from a handheld meter. This lets me calculate K values for our code. (I found out the hard way that an unsealed container suffers from evaporation, which means your TDS readings will climb as the water becomes more concentrated). 

Note that readings take time to settle down. The temperature sensor needs a while to settle in and so for the first hour or so readings are quite unstable. 

![calibration test](https://github.com/ealingcommoner/RiverPollutionNetwork/blob/main/calibration.jpg)

After calibration we are finding that readings are in agreement to within 2ppm and there's a variation of less than 1ppm over time. I would estimate error as 0.6%. From reading reports of pollution images we need to be able to detect changes of ~10% so I'm feeling confident. 

![Readings over time post calibration](https://github.com/ealingcommoner/RiverPollutionNetwork/blob/main/tapwatertesting.jpg)

## Battery life
I'm predicting a battery life of around 30 days, you can use a simple linear model like [this](https://github.com/ealingcommoner/RiverPollutionNetwork/blob/main/batterymodel.R) to predict battery life.

## Range
In testing I have found a range of 350m, your experience may vary based on buildings and trees in the range and aerial placement. 

## To do next
- Test sensors with a range of materials including mineral water and distilled water. 
- Deploy to river. Ideally these will just be attached to a stake and hammered in. 

You could build a cluster model with a range of sensors (TDO, TDS and temperature) for the best possible range of readings. 
To cover more river I have a few ideas, either getting local volunteers to host their own base stations or to build some simple relay stations based around the remote sensor design. I also want to make these solar powered in future. 

## Notes on project
- This is my first Arduino project and I'm extremely grateful to the Radiohead library creators for making this easy. 
- If you think this is something that can help your community please do it. Feel free to use, criticise or modify anything I've created to make it happen. 
- Massive thanks to the incredible service from The Pi Hut and Pimoroni. As ever I'm staggered by the sheer creativity of Adafruit and their incredible board. 
