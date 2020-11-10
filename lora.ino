#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <CayenneLPP.h>
#include "main.h"
#include "lora.h"
#include "eui-config.h"

void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

// payload to send to TTN gateway
static uint8_t payload[4];

// Pin mapping for Adafruit Feather M0 LoRa
const lmic_pinmap lmic_pins = {
    .nss = 8,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 4,
    .dio = {3, 6, 2},
    .rxtx_rx_active = 0,
    .rssi_cal = 8,              // LBT cal for the Adafruit Feather M0 LoRa, in dB
    .spi_freq = 8000000,
};

void do_send(float num, float sum, float maximum, float minimum) {
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else if (LMIC.opmode & OP_TXDATA) {
        Serial.println(F("OP_TXDATA, not sending"));
    } else if (num > 0) {
        float average = sum / num;

        /* construct packet:
            channel 1: average
            channel 2: min
            channel 3: max
        */
        CayenneLPP lpp(128);
        lpp.reset();
        lpp.addAnalogOutput(1, average);
        lpp.addAnalogOutput(2, minimum);
        lpp.addAnalogOutput(3, maximum);

        Serial.print("Sending data: "); Serial.print(average); Serial.print(", "); Serial.print(minimum); Serial.print(", "); Serial.println(maximum);

        // prepare upstream data transmission at the next possible time.
        // transmit on port 1 (the first parameter); you can use any value from 1 to 223 (others are reserved).
        // don't request an ack (the last parameter, if not zero, requests an ack from the network).
        lmic_tx_error_t result = LMIC_setTxData2(1, lpp.getBuffer(), lpp.getSize(), 0);
        switch (result) {
            case 0:
                Serial.println("packet queued");
                break;
            case -1:
                Serial.println("busy | could not queue packet");
                break;
            case -2:
                Serial.println("packet too big | could not queue packet");
                break;
            case -3:
                Serial.println("not feasible | could not queue packet");
                break;
            case -4:
                Serial.println("failed | could not queue packet");
                break;
        }
    } // else {
        // os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(SEND_INTERVAL * 60), do_send);
    // }
}

void send_dummy() {
    do_send(1, 0, 0, 0);
}
