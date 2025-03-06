#ifndef TANK1CONTROL_H
#define TANK1CONTROL_H

void controlTank01(void *params);
void controlPH01(void* params);
void controlLLS01(void* params);

void controlLLS04(void* params);

void controlPG01(void* params);

extern TaskHandle_t controlTank01Handle; 
#endif


