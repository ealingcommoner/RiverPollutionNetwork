#include <RHUartDriver.h>
#include <RHMesh.h>
#include <RHRouter.h>
#include <RH_RF95.h>
#include <RadioHead.h>
#include <RHDatagram.h>
#include <RHGenericDriver.h>
#include <RHReliableDatagram.h>

// rf95_client.pde
// -*- mode: C++ -*-
// Based on Radiohead rf95client sample code

#include <RH_RF95.h>

#ifdef __AVR__
    #include <SoftwareSerial.h>
    SoftwareSerial SSerial(5, 6); // RX, TX
    #define COMSerial SSerial
    #define ShowSerial Serial

    RH_RF95<SoftwareSerial> rf95(COMSerial);
#endif

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define COMSerial Serial1
    #define ShowSerial SerialUSB

    RH_RF95<Uart> rf95(COMSerial);
#endif

#ifdef ARDUINO_ARCH_STM32F4
    #define COMSerial Serial
    #define ShowSerial SerialUSB

    RH_RF95<HardwareSerial> rf95(COMSerial);
#endif




void setup() {
    ShowSerial.begin(9600);
    ShowSerial.println("RF95 client test.");
    pinMode(LED_BUILTIN, OUTPUT);

    if (!rf95.init()) {
        ShowSerial.println("init failed");
        while (1);
    }

    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

    // The default transmitter power is 13dBm, using PA_BOOST.
    // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
    // you can set transmitter powers from 5 to 23 dBm:
    rf95.setTxPower(23, false);

    rf95.setFrequency(915.0);
}

void loop() {
    // ShowSerial.println("Sending to rf95_server");
    // Send a message to rf95_server
    uint8_t data[] = "Base calling!";
    rf95.send(data, sizeof(data));

    rf95.waitPacketSent();

    // Now wait for a reply
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.waitAvailableTimeout(3000)) {
        // Should be a reply message for us now
        if (rf95.recv(buf, &len)) {
            digitalWrite(LED_BUILTIN, HIGH);
            //ShowSerial.print("got reply: ");
            ShowSerial.println((char*)buf);
            digitalWrite(LED_BUILTIN, LOW);
        } else {
            //ShowSerial.println("recv failed");
        }
    } else {
        //ShowSerial.println("No reply, is rf95_server running?");
    }

    delay(1000);
}
