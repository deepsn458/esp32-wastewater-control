#ifndef TANKCONTROL_H
#define TANKCONTROL_H

#include <Arduino.h>




// Tank 1
void controlTank1();
float phSensorReadTank01();
void doseAcidTank01();
void controlPH01(void* parameters);
void controlLLS01(void* parameters);

// Tank 2
float phSensorReadTank02();
void doseAcidTank02();
void controlTank02(void* parameters);
void controlPH02(void* parameters);
void controlLLS02(void* parameters);

#endif // TANKCONTROL_H
