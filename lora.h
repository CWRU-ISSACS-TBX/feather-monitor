#ifndef LORA_H
#define LORA_H

const int SEND_INTERVAL = 60; // unit is the number of readings
extern osjob_t sendjob;
extern osjob_t readjob;

void do_send();

#endif
