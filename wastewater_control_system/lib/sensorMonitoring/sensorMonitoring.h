#ifndef SENSORMONITORING_H
#define SENSORMONITORING_H


extern const int PH_SENSOR_ADDRESS_TREATED;
extern const int PH_SENSOR_ADDRESS_EC1;
extern const int PH_SENSOR_ADDRESS_EC2;
extern const int PH_SENSOR_ADDRESS_HYDROLYSIS;

extern const int COND_SENSOR_ADDRESS_EC1;
extern const int COND_SENSOR_ADDRESS_EC2;
extern const int COND_SENSOR_ADDRESS_TREATED;
extern const int COND_SENSOR_ADDRESS_ADJUSTMENT;

extern const int DO_SENSOR_ADDRESS_HYDROLYSIS;
extern const int DO_SENSOR_ADDRESS_TREATED;
extern const int TEMP_SENSOR_ADDRESS_HYDROLYSIS;
extern const int PH_SENSOR_ADDRESS_ADJUSTMENT;

void monitorSensors(void* parameters);
void addSensors();
#endif