#ifndef MAIN_HEADER
#define	MAIN_HEADER

#include <xc.h> // include processor files - each processor file is guarded.  

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = ON        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

void Init(void);
void SoMoSe(void);
void LEDDriver(void);
void CheckLedDriver(void);
void DisplayLedDriver(void);

void InitI2C(void);
bool startI2C(void);
void stopI2C(void);
bool idleI2C(void);
unsigned char sendI2C(unsigned char byte);
unsigned char reciveI2C_nack(void);
unsigned char reciveI2C_ack(void);
bool waitI2C(void);

#define _XTAL_FREQ  4000000

#endif	/* XC_HEADER_TEMPLATE_H */

