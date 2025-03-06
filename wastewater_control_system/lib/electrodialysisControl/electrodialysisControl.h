#ifndef ELECTRODIALYSISCONTROL_H
#define ELECTRODIALYSISCONTROL_H

/*TODO: test if these functions can be run within a while loop in the 
electrodialysis task or if separate tasks need to be created*/
void controlElectrodialysis(void* params);
void controlPH02(void* params);
void controlPressure(void* params);
void controlCellVoltage(void* params);
void controlElectrochemicalVoltage(void* params);
void controlLLS05(void* params);
void startControlPress(void* params);
void startControlPressure(TimerHandle_t controlPressure);

extern TaskHandle_t controlElectrodialysisHandle;
#endif