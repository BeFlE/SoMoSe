
#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

void setupI2C();
void setNewI2CAdr(int oldAdr, int newAdr);
void setReferenceDry(int Adr,int newValue);
void setReferenceWet(int Adr,int newValue);
int getAveragedSensorValue(int Adr);
int getSensorValue(int Adr);
char getTemperatureValue(int Adr);
int getReferenceDry(int Adr);
int getReferenceWet(int Adr);

//only for Somose versions > v3.4 (HW version)
float SoMoSe_readHwVersion(uint8_t Adr);
float SoMoSe_readFwVersion(uint8_t Adr);
bool SoMoSe_readLowPowerMode(uint8_t Adr);
void SoMoSe_setLowPowerMode(int Adr, bool turnOn);
void SoMoSe_startMeassurment(int Adr, uint8_t Repetitions = 100);
bool SoMoSe_MeassurementFinished(uint8_t Adr);
