/*******************************************************************************
 * The Things Network - Sensor Data Example
 *
 * Example of sending a valid LoRaWAN packet with DHT22 temperature and
 * humidity data to The Things Networ using a Feather M0 LoRa.
 *
 * Learn Guide: https://learn.adafruit.com/the-things-network-for-feather
 *
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman
 * Copyright (c) 2018 Terry Moore, MCCI
 * Copyright (c) 2018 Brent Rubell, Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *******************************************************************************/
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <EmonLib.h>
#include "lora.h"
#include "sensor.h"
#include "config.h"

int num_readings = 0;
double sum_readings = 0;
double minimum_reading = 1000; // Readings should always be less than 3
double maximum_reading = 0;

// Clamp sensor
EnergyMonitor emon0;

osjob_t sendjob;
osjob_t readjob;

/* TODO: need to fix this, it's ugly */
void onEvent (ev_t ev) {
    // Serial.print(os_getTime());
    // Serial.print(": ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            // Serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            // Serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            // Serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            // Serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            // Serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            // Serial.println(F("EV_JOINED"));
            /*{
                u4_t netid = 0;
                devaddr_t devaddr = 0;
                u1_t nwkKey[16];
                u1_t artKey[16];
                LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
                Serial.print("netid: ");
                Serial.println(netid, DEC);
                Serial.print("devaddr: ");
                Serial.println(devaddr, HEX);
                Serial.print("AppSKey: ");
                for (size_t i=0; i<sizeof(artKey); ++i) {
                    if (i != 0)
                    Serial.print("-");
                    printHex2(artKey[i]);
                }
                Serial.println("");
                Serial.print("NwkSKey: ");
                for (size_t i=0; i<sizeof(nwkKey); ++i) {
                    if (i != 0)
                       Serial.print("-");
                    printHex2(nwkKey[i]);
                }
                Serial.println();
            }*/
            // Disable link check validation (automatically enabled
            // during join, but because slow data rates change max TX
            // size, we don't use it in this example.
            LMIC_setLinkCheckMode(0);
            break;
        case EV_RFU1:
            // Serial.println(F("EV_RFU1"));
            break;
        case EV_JOIN_FAILED:
            // Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            // Serial.println(F("EV_REJOIN_FAILED"));
            break;
        case EV_TXCOMPLETE:
            //Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (SERIALDEBUG) {
                Serial.println(F("Packet transmitted"));
            }
            if (LMIC.txrxFlags & TXRX_ACK)
                // Serial.println(F("Received ack"));
            if (LMIC.dataLen) {
                // Serial.println(F("Received "));
                // Serial.println(LMIC.dataLen);
                // Serial.println(F(" bytes of payload"));

                // for (int i = 0; i < LMIC.dataLen; i++) {
                    // if (LMIC.frame[LMIC.dataBeg + i] < 0x10) {
                        // Serial.print(F("0"));
                    // }
                    // Serial.print(LMIC.frame[LMIC.dataBeg + i], HEX);
                // }
            }
            // Schedule next transmission
            //os_setTimedCallback(&readjob, os_getTime()+sec2osticks(READING_INTERVAL), read_current);
            break;
        case EV_LOST_TSYNC:
            // Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            // Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            // Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            // Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            // Serial.println(F("EV_LINK_ALIVE"));
            break;
        case EV_SCAN_FOUND:
            // Serial.println(F("EV_SCAN_FOUND"));
            break;
        case EV_TXSTART:
            // Serial.println(F("EV_TXSTART"));
            break;
        case EV_TXCANCELED:
            // Serial.println(F("EV_TXCANCELED"));
            break;
        case EV_RXSTART:
            // do not print anything -- it wrecks timing
            break;
        case EV_JOIN_TXCOMPLETE:
            // Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
            break;
        default:
            // Serial.print(F("Unknown event: "));
            // Serial.println((unsigned) ev);
            break;
    }
}

void setup() {
    delay(5000);
    if (SERIALDEBUG) {
        while (!Serial)
            ;
        Serial.begin(9600);
        Serial.println(F("Starting"));
        Serial.print(F("Sensor readings interval: ")); Serial.print(READING_INTERVAL); Serial.println(F(" ms"));
        Serial.print(F("Sending interval: ")); Serial.print(SEND_INTERVAL); Serial.println(F(" readings"));
    }

    // Set up current clamp
    emon0.current(0, SENSOR_CAL); // Current: input pin, calibration.
    // Take a few initial readings. Otherwise the sensor doesn't read properly
    for (int i = 0; i < 10; i++) {
      emon0.calcIrms(1480);
    }

    // LMIC init
    os_init();
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
    // Disable link-check mode and ADR, because ADR tends to complicate testing.
    LMIC_setLinkCheckMode(0);
    // Set the data rate to Spreading Factor 7.  This is the fastest supported rate for 125 kHz channels, and it
    // minimizes air time and battery power. Set the transmission power to 14 dBi (25 mW).
    LMIC_setDrTxpow(DR_SF7,14);
    // helium uses sub-band 2 (0-indexed)
    LMIC_selectSubBand(1);

    LMIC_setClockError(1 * MAX_CLOCK_ERROR / 40);

    /* send a dummy packet for activation */
    send_dummy();
    /* starting cycle of reading currents */
    read_current(&readjob);
}

void loop() {
  // we call the LMIC's runloop processor. This will cause things to happen based on events and time. One
  // of the things that will happen is callbacks for transmission complete or received messages. We also
  // use this loop to queue periodic data transmissions.  You can put other things here in the `loop()` routine,
  // but beware that LoRaWAN timing is pretty tight, so if you do more than a few milliseconds of work, you
  // will want to call `os_runloop_once()` every so often, to keep the radio running.
  os_runloop_once();
}
