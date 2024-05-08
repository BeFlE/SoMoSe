
#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

void setupI2C();
void SoMoSe_setNewI2CAddr(int oldAddr, int newAddr);
void SoMoSe_setReferenceDry(int Addr, int newValue);
void SoMoSe_setReferenceWet(int Addr, int newValue);
int SoMoSe_getAveragedSensorValue(int Addr);
int SoMoSe_getSensorValue(int Addr);
char SoMoSe_getTemperatureValue(int Addr);
int SoMoSe_getReferenceDry(int Addr);
int SoMoSe_getReferenceWet(int Addr);

//only for Somose versions > v3.4 (HW version)
float SoMoSe_getHwVersion(uint8_t Addr);
float SoMoSe_getFwVersion(uint8_t Addr);
bool SoMoSe_getLowPowerMode(uint8_t Addr);
void SoMoSe_setLowPowerMode(int Addr, bool turnOn);
void SoMoSe_startMeasurement(int Addr, uint8_t Repetitions = 100);
bool SoMoSe_isMeasurementFinished(uint8_t Addr);
