
void setupI2C();
void setNewI2CAdr(int oldAdr, int newAdr);
void setReferenceDry(int Adr,int newValue);
void setReferenceWet(int Adr,int newValue);
int getAveragedSensorValue(int Adr);
int getSensorValue(int Adr);
int getReferenceDry(int Adr);
int getReferenceWet(int Adr);
