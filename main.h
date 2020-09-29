#ifndef MAIN_H
#define MAIN_H

extern int num_readings;
extern double sum_readings;
extern double minimum_reading; // Readings should always be less than 3
extern double maximum_reading;

// clamp sensor
extern EnergyMonitor emon0;

#endif
