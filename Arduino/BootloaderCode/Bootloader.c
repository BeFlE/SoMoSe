#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <Wire.h>



typedef struct {
    uint8_t byte_count;
    uint16_t address;
    uint8_t type;
    uint8_t data[50];
    uint8_t checksum;
} HexRecord;

typedef struct{
    uint8_t Command;
    union 
    {
        struct 
        {
            uint8_t DataLenL;
            uint8_t DataLenH;        
        };
        uint16_t DataLen;
    };    
    uint8_t UnlockSeqL;
    uint8_t UnlockSeqH;
    union{
        struct
        {
            uint8_t AdrL;
            uint8_t AdrH;
            uint8_t AdrU;
            uint8_t AdrE;
        };
        uint32_t Adr;
    };
    uint8_t Data[50];
}
BootloadTX;

const long timeoutInterval = 2000; 
uint16_t ApplicationStartAdr = 0xFFFF;
uint16_t ApplicationLength;
extern int SCL_Pin,SDA_Pin;


bool Bootloader_ChecksumCommand(uint16_t length, uint16_t Adr, uint8_t I2CAdr) {
    uint8_t recievedData[15];
    uint16_t recievedChecksum;
    uint8_t i = 0;

    uint16_t TO = Wire.getTimeOut();
    Wire.setTimeOut(250);
    Wire.beginTransmission(byte(I2CAdr));  // transmit to device
    Wire.write(0x08);
    Wire.write(length & 0xff);
    Wire.write((length >> 8) & 0xff);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.write(Adr & 0xff);
    Wire.write((Adr >> 8) & 0xff);
    Wire.write(0x00);
    Wire.write(0x00);        
    if(Wire.endTransmission())         // stop transmitting
        return 0;

    delay(1000);
    Wire.requestFrom(byte(I2CAdr), (byte) 12);     // request 2 bytes from slave device 
    Wire.setTimeOut(TO);
    if (12 <= Wire.available())    // if two bytes were received
    { 
        while (i < 12)
            recievedData[i++] = Wire.read();    
    }
    else    
        return 0;
    if(recievedData[9] == 0x01)
    {
        recievedChecksum = (recievedData[11] << 8) + recievedData[10];
        // Serial.printf("PIC Checksum: %04X\n",recievedChecksum);
        return 1;
    }
    else
        return 0;
}
bool Bootloader_WriteFlashCommand(BootloadTX data, uint8_t I2CAdr) {
    // Sende die Struktur auf Serial2
    // Serial2.write(0x55);
    uint16_t TO = Wire.getTimeOut();
    Wire.setTimeOut(1000);
    Wire.beginTransmission(byte(I2CAdr));  // transmit to device
    Wire.write(data.Command);
    Wire.write(data.DataLenL);
    Wire.write(data.DataLenH);
    Wire.write(data.UnlockSeqL);
    Wire.write(data.UnlockSeqH);
    Wire.write(data.AdrL);
    Wire.write(data.AdrH);
    Wire.write(data.AdrU);
    Wire.write(data.AdrE);

    for (int i = 0; i < (data.DataLenH << 8) + data.DataLenL; i++) {
        Wire.write(data.Data[i]);
    }
    if(Wire.endTransmission())         // stop transmitting
        return 0;
    delay(20);
    Wire.requestFrom(byte(I2CAdr), (byte) 10);     // request 2 bytes from slave device 
    Wire.setTimeOut(TO);

    if(data.Adr < ApplicationStartAdr)
        ApplicationStartAdr = data.Adr;

    if((data.Adr - ApplicationStartAdr)> ApplicationLength)
        ApplicationLength = (data.Adr + data.DataLen) - ApplicationStartAdr -2;//Last 2 Bytes in Flash are Checksum

    uint8_t recievedData[30];
    uint8_t i = 0;

    if (10 <= Wire.available())    // if two bytes were received
    { 
        while (i < 10)
            recievedData[i++] = Wire.read();    
    }
    else    
        return 0;
    delay(5);
    if(recievedData[9] == 0x01)
    {
        return 1;
    }
    else
        return 0;
}
bool Bootloader_EraseFlashCommand(uint8_t I2CAdr) {
    // Sende die Struktur auf Serial2
    BootloadTX data;
    data.Command = 0x03;
    data.DataLenL = 0x30;
    data.DataLenH = 0x00;
    data.UnlockSeqL = 0x55;
    data.UnlockSeqH = 0xAA;
    data.AdrL = 0x00;
    data.AdrH = 0x10;
    data.AdrU = 0x00;
    data.AdrE = 0x00;

    uint16_t TO = Wire.getTimeOut();
    Wire.setTimeOut(1500);
    Wire.beginTransmission(byte(I2CAdr));  // transmit to device
    Wire.write(data.Command);
    Wire.write(data.DataLenL);
    Wire.write(data.DataLenH);
    Wire.write(data.UnlockSeqL);
    Wire.write(data.UnlockSeqH);
    Wire.write(data.AdrL);
    Wire.write(data.AdrH);
    Wire.write(data.AdrU);
    Wire.write(data.AdrE);
    if(Wire.endTransmission())         // stop transmitting
        return 0;
    Wire.setTimeOut(TO);

    delay(1500);
    uint8_t recievedData[30];
    uint8_t i = 0;

    Wire.requestFrom(byte(I2CAdr), (byte) 10);     // request 2 bytes from slave device 
    Wire.setTimeOut(TO);

    if (10 <= Wire.available())    // if two bytes were received
    { 
        while (i < 10)
        {
            recievedData[i] = Wire.read();    
            // Serial.println(recievedData[i]);
            i++;
        }
    }
    else    
        return 0;
    if(recievedData[9] == 0x01)
    {
        return 1;
    }
    else
        return 0;
}
bool Bootloader_VersionCommand(uint8_t I2CAdr) {
    uint8_t i = 0;
    uint16_t TO = Wire.getTimeOut();
    char buf[30];

    Wire.setTimeOut(250);
    Wire.beginTransmission(byte(I2CAdr));  // transmit to device
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.write(0x00);        
    if(Wire.endTransmission())         // stop transmitting
        return 0;
    delay(50);
    Wire.requestFrom(byte(I2CAdr), (byte) 25);     // request 2 bytes from slave device 
    Wire.setTimeOut(TO);
    if (25 <= Wire.available())    // if two bytes were received
    { 
        for (uint8_t j = 0; j < 25; j++)
        {
            buf[j] = Wire.read();
        }
        if(buf[9] == 0x08 && buf[10] == 0x01)
            return 1;  
        else
            return 0;
    }
    else    
        return 0;
}
bool Bootloader_ResetCommand(uint8_t I2CAdr) {
    uint8_t i = 0;
    uint16_t TO = Wire.getTimeOut();

    Wire.setTimeOut(250);
    Wire.beginTransmission(byte(I2CAdr));  // transmit to device
    Wire.write(0x09);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.write(0x00);
    Wire.write(0x00);        
    // if(Wire.endTransmission())         // stop transmitting
    //     return 0;
    Wire.endTransmission();
    delay(500);
    return 1;
}
HexRecord parse_hex_line(const char* hex_line) {
    HexRecord record;
    

    static uint8_t state = 0;

    sscanf(hex_line, ":%2X%4X%2X", &record.byte_count, &record.address, &record.type);
    for (int i = 0; i < record.byte_count; i++) {
        sscanf(hex_line + 9 + i * 2, "%2X", &record.data[i]);
    }
    sscanf(hex_line + 9 + record.byte_count * 2, "%2X", &record.checksum);

    return record;
}
void Bootloader_WriteFlash(File file,uint8_t I2CAdr)
{
    BootloadTX BlTX;
    uint8_t i = 0;

    BlTX.AdrE = 0;
    BlTX.AdrU = 0;

    char hex_line[60];
    while (file.available()) {
        file.readBytesUntil('\n', hex_line, sizeof(hex_line));
        
        if (hex_line[0] == ':') {
            HexRecord record = parse_hex_line(hex_line);

            if(record.type == 0x00)
            {
                if(BlTX.AdrE == 0 && BlTX.AdrU == 0)
                {
                    if((record.address & 0xff) + record.byte_count > 0xff)
                    {
                        uint8_t offset = record.address + record.byte_count - 256;
                        BlTX.AdrH = record.address >> 8 ;
                        BlTX.AdrL = record.address & 0xff;
                        BlTX.Command = 0x02;
                        BlTX.DataLenL = record.byte_count - offset;
                        BlTX.DataLenH = 0;
                        BlTX.UnlockSeqL = 0x55;
                        BlTX.UnlockSeqH = 0xAA;
                        for(uint8_t i = 0; i < record.byte_count - offset; i++)
                            BlTX.Data[i] = record.data[i];
                        if(!Bootloader_WriteFlashCommand(BlTX,I2CAdr))
                            break;
                        record.address += offset;
                        BlTX.AdrH = record.address >> 8 ;
                        BlTX.AdrL = record.address & 0xff;
                        BlTX.Command = 0x02;
                        BlTX.DataLenL = offset;
                        BlTX.DataLenH = 0;
                        BlTX.UnlockSeqL = 0x55;
                        BlTX.UnlockSeqH = 0xAA;
                        for(uint8_t i = offset; i < record.byte_count; i++)
                            BlTX.Data[i - offset] = record.data[i];
                        if(!Bootloader_WriteFlashCommand(BlTX,I2CAdr))
                            break;
                    }
                    else{
                        BlTX.AdrH = record.address >> 8 ;
                        BlTX.AdrL = record.address & 0xff;
                        BlTX.Command = 0x02;
                        BlTX.DataLenL = record.byte_count;
                        BlTX.DataLenH = 0;
                        BlTX.UnlockSeqL = 0x55;
                        BlTX.UnlockSeqH = 0xAA;
                        for(uint8_t i = 0; i < record.byte_count; i++)
                            BlTX.Data[i] = record.data[i];
                        if(!Bootloader_WriteFlashCommand(BlTX,I2CAdr))
                            break;
                    }
                }
            }
            else if(record.type == 0x01)
            {
                break;
            }
            else if(record.type == 0x04)
            {
                BlTX.AdrE = record.data[0] ;
                BlTX.AdrU = record.data[1];
            }
        } else {
            Serial.printf("Ungültiges Format: %s", hex_line);
        }
        if(i++ == 10)
        {
            i = 0;
            Serial.print(" .");
        }
    }
}

bool BootloadDevice(uint8_t I2CAdr, String hexFilePath)//to flash the SoMoSe on "I2CAdr", simply execute this function. first load the hexfile into the file system and specify the path
{  
    

    uint8_t state = 0;

    Serial.println("entering bootloading mode");

    if(!Bootloader_VersionCommand(I2CAdr))
    {
        Wire.beginTransmission(byte(I2CAdr));  // transmit to device
        Wire.write('B');         
        Wire.write(0x01);         
        if(Wire.endTransmission())         // stop transmitting
        {
            return 0;
        }
    }
    Serial.print("Bootloading-Modus gestartet...\n");

    File file = LittleFS.open(hexFilePath.c_str(), "r");
    if (!file) {
        Serial.println("Fehler beim Öffnen der Datei.");
        return 0;
    }
    
    uint8_t attempts = 0;

    while(1)
    {
        switch (state)
        {
            case 0:
                if(Bootloader_VersionCommand(I2CAdr))
                    state = 1;
                else
                    state = 0xff;
                break;

            case 1:
                if(attempts++ >= 3)
                    state = 0xff;
                if(Bootloader_EraseFlashCommand(I2CAdr))
                    state = 2;
                else
                    state = 0xff;
                break;

            case 2:
                Serial.print("Flash gelöscht...\n");
                Serial.print("schreibe Update (Dauer: ca. 30 Sekunden)");
                Bootloader_WriteFlash(file,I2CAdr);
                state = 3;
                break;

            case 3:
                if(Bootloader_ChecksumCommand(ApplicationLength,ApplicationStartAdr,Adr))
                    state = 4;
                else
                    state = 1;
                break;

            case 4:
                Serial.print("Update erfolgreich\n");
                if(Bootloader_ResetCommand(I2CAdr))
                    state = 5;
                else 
                    state = 0xff;
                break;

            case 5:
                file.close();
                return 1;

            case 0xff:
                file.close();
                return 0;
            
            default:
                state = 0xff;
                break;
        }
    }
}

