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


// Modified from remoteserverrm95 from Radiohead library



// Feather9x_TX
// -*- mode: C++ -*-
// TDS station based on example sketch
// Make sure you include radiohead libaries

#include <SPI.h>

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
#define RF95_FREQ 869.9

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
#define TdsSensorPin A3
float VREF= 3.3;      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
//float averageVoltage = 0,tdsValue = 0,temperature = 25;

void setup() 
{

  pinMode(RFM95_RST, OUTPUT);
  #define VBATPIN A9
  digitalWrite(RFM95_RST, HIGH);
  pinMode(13, OUTPUT);
  pinMode(TdsSensorPin,INPUT);
  Serial.begin(9600);
  //while (!Serial) {
  //  delay(1);
  //}

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
  rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128);
 
  //rf95.setModemConfig(RH_RF95::Bw31_25Cr48Sf512);
  rf95.setTxPower(23, false);
  delay(100);
}

//int16_t packetnum = 0;  // packet counter, we increment per xmission

void loop()
{
  #define VBATPIN A9
  Serial.println("Transmitting..."); // Send a message to rf95_server
  digitalWrite(13, HIGH);
  delay(100);
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  int percent = (measuredvbat-3.2)*10000;
	//take 25 readings of sensor pin reading and an average
  int count = 25;
  int raw = 0;
  for (int i=0; i< count; i++) {
    raw += analogRead(TdsSensorPin);
    delay(100); 
  }
  raw = raw / count;
  //convert to voltage
  float averageVoltage= raw *  VREF /1024.0;
  int VTDS= averageVoltage * 10000; //we multiply to give a full string to transmit, reverse at base. 
  Serial.println(averageVoltage);
  delay(10);

//variables used in formatting:
  double P;
  int ID;
  ID = 996; //identification number of station. 
  byte sendLen;
  char Pstr[8];
  char buffer[50]; //final byte array that gets passed to radio.send
  dtostrf(P, 3,2, Pstr);  // format a double variable into a small string (byte array)
  sprintf(buffer, "ID:%d, VTDS: %d, batt: %d, OVER", ID , VTDS, percent);
  sendLen = strlen(buffer);  //get the length of buffer

  rf95.send(buffer, sendLen); //finally pass the string (byte array) to the radio to send
    Serial.println(buffer);

  //now go into deep sleep for two hours
  rf95.waitPacketSent();
  digitalWrite(13, LOW);
  rf95.sleep(); 
  delay(3600000/4); //15 minute cycle
 
}