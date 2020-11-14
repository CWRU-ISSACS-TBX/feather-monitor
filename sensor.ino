#include <lmic.h>
#include <EmonLib.h>
#include "main.h"
#include "lora.h"
#include "sensor.h"
#include "config.h"

extern EnergyMonitor emon0;

extern int num_readings;
extern double sum_readings;
extern double minimum_reading;
extern double maximum_reading;

void read_current(osjob_t* j) {
    /* read from sensor */
    float amps = (float) emon0.calcIrms(1480);
    /* random fake reading */
    //float amps = 0.5 + (os_getTime() % 10 );

    num_readings++;
    sum_readings += amps;
    if (amps < minimum_reading)
        minimum_reading = amps;
    if (amps > maximum_reading)
        maximum_reading = amps;

    // Schedule next reading
    os_setTimedCallback(&readjob, os_getTime()+ms2osticks(READING_INTERVAL), read_current);

    if (num_readings % (SEND_INTERVAL / 2) == 0) {
        if (SERIALDEBUG) {
            Serial.print("num_readings: "); Serial.println(num_readings);
        }
    }
    if (num_readings >= SEND_INTERVAL) {
        do_send(num_readings, sum_readings, maximum_reading, minimum_reading);
        num_readings = 0;
        sum_readings = 0;
        minimum_reading = 1000;
        maximum_reading = 0;
    }
}
