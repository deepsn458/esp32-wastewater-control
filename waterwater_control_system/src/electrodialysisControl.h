#ifndef ELECTRODIALYSISCONTROL_H
#define ELECTRODIALYSISCONTROL_H

/*TODO: test if these functions can be run within a while loop in the 
electrodialysis task or if separate tasks need to be created*/
void controlElectrodialysis(void* parameters);
void controlPH02(void* parameters);
void controlPressure(void* parameters);
void controlCellVoltage(void* parameters);
void controlElectrochemicalVoltage(void* parameters);
void controlLLS05(void* parameters);
void startControlPressure(TimerHandle_t controlPressureTimer);
void startControlPressure(TimerHandle_t controlPressureTimer);
void IRAM_ATTR systemShutdown();

/*TODO: make these functions global so they can be called by the different modules*/
void pumpControl(int pumpPin, int direction);
float readPH(int sensorPin);
void acidPumpControl(int acidPumpPin, int direction);
void sendAlert(char* message);
float readPressure(int sensorPin);
float readVoltage(int sensorPin);
float readConductivity(int sensorPin);
int readLiquidLevel(int sensorPin);
#endif