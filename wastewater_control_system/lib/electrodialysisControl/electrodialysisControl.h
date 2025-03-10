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
void startControlPress1(TimerHandle_t controlPress1Timer);
void startControlPress2(TimerHandle_t controlPress2Timer);
void IRAM_ATTR systemShutdown();

/*TODO: make these functions global so they can be called by the different modules*/
void setupPumps();
void pumpControl(uint16_t pumpAddr, int direction);
float readPH(int sensorPin);
void acidPumpControl(int acidPumpPin, int direction);
void sendAlert(const char* message);
float readSensor(Ezo_board sensor);
float readVoltage(int sensorPin);
int readLiquidLevel(int sensorPin);
#endif