#include "EmonLib.h"
#include "main.h"
#include "lora.h"
#include "sensor.h"

extern EnergyMonitor emon0;

extern int num_readings;
extern double sum_readings;
extern double minimum_reading; // Readings should always be less than 3
extern double maximum_reading;

void read_current(osjob_t* j) {
    double amps = emon0.calcIrms(1480);

    // Serial.print("Reading: "); Serial.println(amps);

    num_readings++;
    sum_readings += amps;
    if (amps < minimum_reading)
        minimum_reading = amps;
    if (amps > maximum_reading)
        maximum_reading = amps;

    // Schedule next reading
    os_setTimedCallback(&readjob, os_getTime()+sec2osticks(READING_INTERVAL * 60), read_current);

    // Serial.print("num_readings: "); Serial.println(num_readings);
    if (num_readings >= SEND_INTERVAL) {
        do_send();
    }
}
