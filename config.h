#ifndef CONFIG_H
#define CONFIG_H

/* number of readings before sending to Helium */
const int SEND_INTERVAL = 120; // unit is the number of readings

/* interval between current readings */
const float READING_INTERVAL = 500; // milliseconds

const float SENSOR_CAL = 100.0;

/* 0 for no printing to serial
 * 1 for printing to serial (for debugging)
 */
const u1_t SERIALDEBUG = 0;

#endif
