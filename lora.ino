#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "main.h"
#include "lora.h"

// This EUI must be in little-endian format, so least-significant-byte
// first. When copying an EUI from ttnctl output, this means to reverse
// the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
// 0x70.
static const u1_t PROGMEM APPEUI[8] = { 0xB7, 0x09, 0x03, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

// This should also be in little endian format, see above.
static const u1_t PROGMEM DEVEUI[8] = { 0xE8, 0x7C, 0x91, 0x1D, 0x92, 0x62, 0x79, 0x00 };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

// This key should be in big endian format (or, since it is not really a
// number but a block of memory, endianness does not really apply). In
// practice, a key taken from the TTN console can be copied as-is.
static const u1_t PROGMEM APPKEY[16] = { 0xE2, 0xC1, 0xF4, 0xAE, 0x3D, 0x97, 0xE7, 0xBC, 0x71, 0x03, 0x1A, 0x94, 0x38, 0x25, 0x53, 0x2C };
void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

// payload to send to TTN gateway
static uint8_t payload[4];
osjob_t sendjob;
osjob_t readjob;

// Pin mapping for Adafruit Feather M0 LoRa
const lmic_pinmap lmic_pins = {
    .nss = 8,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 4,
    .dio = {3, 6, LMIC_UNUSED_PIN},
    .rxtx_rx_active = 0,
    .rssi_cal = 8,              // LBT cal for the Adafruit Feather M0 LoRa, in dB
    .spi_freq = 8000000,
};

void do_send() {
    // Check if there is not a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND) {
        // Serial.println(F("OP_TXRXPEND, not sending"));
    } else if (num_readings > 0) {
        double average = sum_readings / num_readings;
        double minimum = minimum_reading;
        double maximum = maximum_reading;
        sum_readings = 0;
        num_readings = 0;
        minimum_reading = 1000;
        maximum_reading = 0;
        // float -> int
        // note: this uses the sflt16 datum (https://github.com/mcci-catena/arduino-lmic#sflt16)
        //uint16_t payloadTemp = LMIC_f2sflt16(temperature);

        // Make payload: [average, min, max]
        payload[0] = round(average * 100);
        payload[1] = round(minimum * 100);
        payload[2] = round(maximum * 100);

        // Serial.print("Sending data: "); Serial.print(average); Serial.print(", "); Serial.print(minimum); Serial.print(", "); Serial.println(maximum);

        // prepare upstream data transmission at the next possible time.
        // transmit on port 1 (the first parameter); you can use any value from 1 to 223 (others are reserved).
        // don't request an ack (the last parameter, if not zero, requests an ack from the network).
        // Remember, acks consume a lot of network resources; don't ask for an ack unless you really need it.
        LMIC_setTxData2(1, payload, sizeof(payload)-1, 0);
        // Serial.println("Send Complete");
    } // else {
        // os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(SEND_INTERVAL * 60), do_send);
    // }
}
