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

But we're a small group and it can take hours or days to notice a problem. 

## Aims
Use low-cost, remotely deployed sensors to alert the community to a pollution incident. 

## Measurement
Having reviewed pollution incident reports, it appears the most common indicators are:
- Total Dissolved Oxygen (TDO)
- Total Dissolved Solids (TDS)

TDO sensors are around £130 each, whilst [this TDS sensor](https://www.dfrobot.com/product-1662.html) is £11. We could consider adding TDO in future if funding permits, but for now it seems TDS gives us the best effect for our money.

![Gravity TDS Sensor](https://github.com/ealingcommoner/RiverPollutionNetwork/blob/main/Gravity.jpg)

## Remote Station
For the remote station I chose an [Adafruit Feather 32u4 with LoRa Radio Module](https://learn.adafruit.com/adafruit-feather-32u4-radio-with-lora-radio-module/setup) 
![Radiofruit module](https://cdn-learn.adafruit.com/guides/cropped_images/000/001/273/medium640/thumb2.jpg?1520544037)

It has a low power usage and a built in LoRa radio for long range communications, along with a battery management circuit. Incredibly impressive for just £33. 

In addition we need a battery. I want a pretty powerful battery because ideally we want this to operate for weeks at a time without intervention. I'm avoiding solar panels if possible as they add cost. I went with the [6700mAh High Capacity Lithium Ion Battery](https://shop.pimoroni.com/products/high-capacity-lithium-ion-battery-pack?variant=32012684591187) for the prototype. 

### Assembly
Simply connect lithium battery and connect sensor to GND, 5V and A3 pin for analogue reader. You'll need to solder on:
- Header pins
- Whip antenna (cut wire to 7.8cm for 915MHZ frequency mode)

Hey presto, one remote station
![Remote station](https://github.com/ealingcommoner/RiverPollutionNetwork/blob/main/Prototype.jpg)

### Casing
** To do ** 
I'm looking at junction box like [this one](https://www.toolstation.com/junction-box-ip66/p47979)
![](https://cdn.aws.toolstation.com/images/141020-UK/800/47979.jpg)

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

This meets our requirements for a low cost sensor.

## Base station
I have a Raspberry Pi 3 currently gathering dust. So I attached a [Seeeduino Lotus ](https://wiki.seeedstudio.com/Seeeduino_Lotus/)and a [Grove Lora Module](https://wiki.seeedstudio.com/Grove_LoRa_Radio/) and stuffed them in old takeaway containers for that just cobbled together charm. 
* Note:: all modules will run at 915MHz, make sure you buy the correct type. 
* Note: Original plan was to use a LoRa HAT for the pi, mine was faulty so I went with what I had. The HAT has a better antenna so may have a better range.
![](https://github.com/ealingcommoner/RiverPollutionNetwork/blob/main/Base%20station.jpg)

###Assembly
Even easier. Connect Lotus to USB on pi and Grove Lora to port D5. 

##Bill of materials
| Item     | Cost |
| ---      | ---       |
| Raspberry Pi 3  | £34         |
| Seeeduino Lotus| £13         |
| Grove Lora module | £19.90         |
| **Total** | £66.90         |
* Note: Pi3 is not currently available. A pi4 would do fine, I suspect even a pi zero would be able to run this but not tested. 

## Base station code

[For arduino](https://github.com/ealingcommoner/RiverPollutionNetwork/blob/main/rf95_clientbase.ino)

All I've done here is adapt the rf95client code in Radiohead library. it will transmit a test signal and listen for a transmission from the remote station and send this to the serial port. 

[For pi](https://github.com/ealingcommoner/RiverPollutionNetwork/blob/main/Readfrombasestationandlog.py)
This code reads the transmission from the remote station, parses to individual variables and logs.
** To do ** Add uploading to a suitable IOT service and start building alerts. 

## Range testing
Upload this code to the Feather module
[For arduino](https://github.com/ealingcommoner/RiverPollutionNetwork/blob/main/transmitwithvoltage.ino)

This:
- Reads battery voltage
- Transmits to base station. 
- Listens for base station commands and flashes LED when received. 

Now we can get out on the road and test our signal with our not at all suspicious looking box. 
![](https://github.com/ealingcommoner/RiverPollutionNetwork/blob/main/signaltester.png)
