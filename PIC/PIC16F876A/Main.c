#include <xc.h>
#include "LCD_header.h"
#include "Main-Header.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <pic16f876a.h>

volatile unsigned long millisec;

void __interrupt(high_priority) isr(void)
{  
    if(PIE1bits.TMR2IE && PIR1bits.TMR2IF)
    {
        millisec++;
        PIR1bits.TMR2IF = 0;
    }
}

void main(void) 
{
    
    Init();
    
    while (1)
    {
        CLRWDT();
        if(millisec % 1000 <= 3)
        {
            millisec += 3;
            SoMoSe();
        }  
    }
}
void Init(void)
{
    lcd_init();
    
    PR2 = 255;
    T2CONbits.T2CKPS = 0b01;
    T2CONbits.TMR2ON = 1;
    
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.TMR2IE = 1 ;
    
    InitI2C();
 
    TRISBbits.TRISB5 = 1;
   
    ADCON1 = 0x00;
    ADCON0bits.ADON = 1;
}
void SoMoSe(void)
{
    unsigned char ausgabe[20];
    unsigned char LPValue, Value, UL, UH, DL, DH;
    unsigned char I2CAdr = 0xAA;
    signed char Temp;
    
            for(I2CAdr = 0; I2CAdr < 128; I2CAdr++)
            {//scan all I2C-adr, 7Bit!
                startI2C();
                if(sendI2C((I2CAdr << 1) & 0xFE))//ACK from Slave recieved?
                {//No ACK recieved, stop I2C and continue with next I2C-adr
                    stopI2C();
                    __delay_ms(1);
                    continue;
                }
                sendI2C('v');//ACK recieved, get data
                stopI2C();
                startI2C();
                sendI2C((I2CAdr << 1) | 0x01);
                LPValue = reciveI2C_ack();
                Value = reciveI2C_nack();
                stopI2C();
                
                startI2C();
                sendI2C((I2CAdr << 1) & 0xFE);
                sendI2C('u');
                stopI2C();
                startI2C();
                sendI2C((I2CAdr << 1) | 0x01);
                UH = reciveI2C_ack();
                UL = reciveI2C_nack();
                stopI2C();
                
                startI2C();
                sendI2C((I2CAdr << 1) & 0xFE);
                sendI2C('d');
                stopI2C();
                startI2C();
                sendI2C((I2CAdr << 1) | 0x01);
                DH = reciveI2C_ack();
                DL = reciveI2C_nack();
                stopI2C();
                
                startI2C();
                sendI2C((I2CAdr << 1) & 0xFE);
                sendI2C('t');
                stopI2C();
                startI2C();
                sendI2C((I2CAdr << 1) | 0x01);
                Temp = reciveI2C_nack();
                stopI2C();
                //display data                      
                sprintf((char*)ausgabe,(const char*)"A:%u L:%u V:%u",I2CAdr,LPValue,Value);//I2C-adr, averaged value, current value
                LCD_NewData((const char*)ausgabe,LCD_Zeile1,LCD_linksbuendig);
                sprintf((char*)ausgabe,(const char*)"T:%i",Temp);
//                sprintf((char*)ausgabe,(const char*)"D:%i U:%i",(DH<<8)+DL,(UH<<8)+UL);//downer reference, upper reference
                LCD_NewData((const char*)ausgabe,LCD_Zeile2,LCD_linksbuendig);
                lcd_writeArray();
            }
}

void InitI2C(void)
{
    TRISCbits.TRISC3 = 1;
    TRISCbits.TRISC4 = 1;
    SSPSTATbits.SMP = 1;   //Initialisierung der I2C
    SSPSTATbits.CKE = 0;
    SSPCONbits.SSPEN = 1;
    SSPCONbits.SSPM = 0b1000;
    SSPADD = 124;          //I2C Takt auf 128KHz einstellen(dann 124)
    
}
bool startI2C(void)
{
    if(idleI2C())                     // Ist der Bus frei?
        return 0;                     // Fehler im Bus/länger nicht frei
    SSPCON2bits.SEN = 1;             // Auslösen einer START-Sequenze
    while( SSPCON2bits.SEN );        // Beendet, wenn SEN gelöscht wurde*
    return 1;
}
void stopI2C(void)
{
    idleI2C();                        // Ist der Bus frei?
    SSPCON2bits.PEN = 1;             // Auslösen einer RESTART-Sequenze
    while( SSPCON2bits.PEN );        // Beendet, wenn RSEN gelöscht wurde*
    // *Oder das Bit SSPxIF gesetzt wurde (siehe Datenblatt)
}
bool idleI2C(void)
{
    unsigned long Time = millisec;
    
    while(SSPSTATbits.R_W)        // Läuft eine Übertragung?
    {
        CLRWDT();
        if(Time + 200 < millisec)
            return 1;
    }
    Time = millisec;
    while( SSPCON2 & 0x1F )         // Ist irgendwas anderes aktiv?
    {
        CLRWDT();
        if(Time + 200 < millisec)
            return 1;
    }
    return 0;
}
unsigned char sendI2C(unsigned char byte)
{
    idleI2C();                        // Ist der Bus verfügbar?
    PIR1bits.SSPIF = 0;              // Flag löschen (wird in waitI2C() abgefragt)
    SSPBUF = byte;                   // Durch füllen des Puffers Sendevorgang auslösen
    if(waitI2C())                     // Warten bis Übertragung abgeschlossen ist
        return 1;

    return (unsigned)SSPCON2 & 0x40; // Return 0: ACK empfangen, 0x40: kein ACK empfangen
}
unsigned char reciveI2C_nack(void)
{
    unsigned char incomming = 0;            // Einlesepuffer
    unsigned long Time = millisec;

    idleI2C();                        // Ist der Bus verfügbar?
    PIR1bits.SSPIF = 0;              // Flag löschen (wird in waitI2C() abgefragt)
    SSPCON2bits.RCEN = 1;            // Als Empfänger konfigurieren (wird autom. wieder gelöscht)
    waitI2C();                        // Warten bis Übertragung (lesend) abgeschlossen ist
    SSPCON2bits.ACKDT = 1;           // Mit NACK quitieren (Nicht weiter einlesen)
    SSPCON2bits.ACKEN = 1;           // NACK aussenden
    while( SSPCON2bits.ACKEN )      // NACK abgeschlossen?
    {
        CLRWDT();
        if(Time + 200 < millisec)
            return 0xFF;
    }
    incomming = SSPBUF;              // Empfangenes Byte in den Puffer
    return incomming;                 // Und zurück geben
}
unsigned char reciveI2C_ack(void)
{
    unsigned char incomming = 0;            // Einlesepuffer
    unsigned long Time = millisec;

    idleI2C();                        // Ist der Bus verfügbar?
    PIR1bits.SSPIF = 0;              // Flag löschen (wird in waitI2C() abgefragt)
    SSPCON2bits.RCEN = 1;            // Als Empfänger konfigurieren (wird autom. wieder gelöscht)
    waitI2C();                        // Warten bis Übertragung (lesend) abgeschlossen ist
    SSPCON2bits.ACKDT = 0;           // Mit AACK quitieren (weiter einlesen)
    SSPCON2bits.ACKEN = 1;           // ACK aussenden
    while( SSPCON2bits.ACKEN )      // ACK abgeschlossen?
    {
        CLRWDT();
        if(Time + 200 < millisec)
            return 0xFF;
    }
    incomming = SSPBUF;              // Empfangenes Byte in den Puffer

    return incomming;                 // Und zurück geben
}
bool waitI2C(void)
{
    unsigned long Time = millisec;
    
    while(!PIR1bits.SSPIF)          // Aktion* wurde beendet
    {
        CLRWDT();
        if(Time + 200 < millisec)
            return 1;
    }
    PIR1bits.SSPIF = 0;              // Flag zurücksetzten
    // *Zum Beispiel: START-Sequenze, Übertragung von 8 Bit, ...
    return 0;
}
