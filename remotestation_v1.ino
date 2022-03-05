#include <RHNRFSPIDriver.h>
#include <RH_TCP.h>
#include <RH_RF69.h>
#include <RHCRC.h>
#include <RH_NRF905.h>
#include <RH_RF24.h>
#include <RHMesh.h>
#include <RH_RF22.h>
#include <RHTcpProtocol.h>
#include <RHRouter.h>
#include <RH_NRF51.h>
#include <RHSPIDriver.h>
#include <RH_RF95.h>
#include <RadioHead.h>
#include <radio_config_Si4460.h>
#include <RH_Serial.h>
#include <RHDatagram.h>
#include <RHHardwareSPI.h>
#include <RHSoftwareSPI.h>
#include <RHGenericDriver.h>
#include <RH_MRF89.h>
#include <RHReliableDatagram.h>
#include <RH_ASK.h>
#include <RH_NRF24.h>
#include <RHGenericSPI.h>
#include <RH_CC110.h>

#include <RHNRFSPIDriver.h>
#include <RH_TCP.h>
#include <RH_RF69.h>
#include <RHCRC.h>
#include <RH_NRF905.h>
#include <RH_RF24.h>
#include <RHMesh.h>
#include <RH_RF22.h>
#include <RHTcpProtocol.h>
#include <RHRouter.h>
#include <RH_NRF51.h>
#include <RHSPIDriver.h>
#include <RH_RF95.h>
#include <RadioHead.h>
#include <radio_config_Si4460.h>
#include <RH_Serial.h>
#include <RHDatagram.h>
#include <RHHardwareSPI.h>
#include <RHSoftwareSPI.h>
#include <RHGenericDriver.h>
#include <RH_MRF89.h>
#include <RHReliableDatagram.h>
#include <RH_ASK.h>
#include <RH_NRF24.h>
#include <RHGenericSPI.h>
#include <RH_CC110.h>

// Feather9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_RX

#include <SPI.h>
#include <RH_RF95.h>

// for feather32u4 
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7
//

/* for feather m0  
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
*/

/* for shield 
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 7
*/

/* Feather 32u4 w/wing
#define RFM95_RST     11   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     2    // "SDA" (only SDA/SCL/RX/TX have IRQ!)
*/

/* Feather m0 w/wing 
#define RFM95_RST     11   // "A"
#define RFM95_CS      10   // "B"
#define RFM95_INT     6    // "D"
*/

#if defined(ESP8266)
  /* for ESP w/featherwing */ 
  #define RFM95_CS  2    // "E"
  #define RFM95_RST 16   // "D"
  #define RFM95_INT 15   // "B"

#elif defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2)
  #define RFM95_INT     9  // "A"
  #define RFM95_CS      10  // "B"
  #define RFM95_RST     11  // "C"
  #define LED           13
  
#elif defined(ESP32)  
  /* ESP32 feather w/wing */
  #define RFM95_RST     27   // "A"
  #define RFM95_CS      33   // "B"
  #define RFM95_INT     12   //  next to A

#elif defined(NRF52)  
  /* nRF52832 feather w/wing */
  #define RFM95_RST     7   // "A"
  #define RFM95_CS      11   // "B"
  #define RFM95_INT     31   // "C"
  
#elif defined(TEENSYDUINO)
  /* Teensy 3.x w/wing */
  #define RFM95_RST     9   // "A"
  #define RFM95_CS      10   // "B"
  #define RFM95_INT     4    // "C"
#endif

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() 
{
  pinMode(RFM95_RST, OUTPUT);
  #define VBATPIN A9
  #define VTSDPIN A3
  digitalWrite(RFM95_RST, HIGH);
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  while (!Serial) {
    delay(1);
  }

  delay(100);

  Serial.println("Feather LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop()
{
  #define VBATPIN A9
  #define VTSDPIN A3
  sleep(7200000); // Wait 1 second between transmits, could also 'sleep' here!
  Serial.println("Transmitting..."); // Send a message to rf95_server
  float measuredvbat = analogRead(VBATPIN);
  float vtds = analogRead(VTSDPIN);
  int tds = vtds * 1000;
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  int percent = (measuredvbat-3.2)*10000;
  
//  Serial.print("Sending "); Serial.println(radiopacket);
//  Serial.println(measuredvbat);
  //Serial.println("Sending...");
  digitalWrite(13, HIGH);
  delay(10);

//variables used in formatting:
  double P;
  int ID;
  ID = 999;
  byte sendLen;
  char Pstr[8];
  char buffer[50]; //final byte array that gets passed to radio.send
  dtostrf(P, 3,2, Pstr);  // format a double variable into a small string (byte array)
  sprintf(buffer, "ID:%d, VTDS: %d, batt: %d, OVER", ID , tds, percent);
  sendLen = strlen(buffer);  //get the length of buffer

  rf95.send(buffer, sendLen); //finally pass the string (byte array) to the radio to send
  

  Serial.println(buffer);
  Serial.println(percent);
  Serial.println(vtds);
  //Serial.println("Waiting for packet to complete..."); 
  delay(10);
  rf95.waitPacketSent();
  // Now wait for a reply
  //uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  //uint8_t len = sizeof(buf);
  //digitalWrite(13, LOW);

  //Serial.println("Waiting for reply...");
  //if (rf95.waitAvailableTimeout(1000))
  //{ 
  //  // Should be a reply message for us now   
  //  if (rf95.recv(buf, &len))
  // {
  //    Serial.print("Got reply: ");
  //    digitalWrite(13, HIGH);
  //    Serial.println((char*)buf);
  //    Serial.print("RSSI: ");
  //    Serial.println(rf95.lastRssi(), DEC);    
  //  }
  //  else
  //  {
  //    Serial.println("Receive failed");
  //  }
  //}
  //else
  //{
  //  Serial.println("No reply, is there a listener around?");
  //}

}
