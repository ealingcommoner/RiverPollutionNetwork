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

