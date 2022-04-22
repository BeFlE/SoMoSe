// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef LCD_H
#define	LCD_H

#include <xc.h> // include processor files - each processor file is guarded. 
#include <stdbool.h>

#define lcd_rs          PORTCbits.RC5
#define lcd_rw          PORTCbits.RC6
#define lcd_e           PORTCbits.RC7
#define lcd_light       PORTCbits.RC2
#define lcd_data_write  PORTB
#define lcd_data_read   PORTB    
#define lcd_data_TRIS   TRISB

#define lcdCtrl_ClearDisplay    0x01
#define lcdCtrl_ReturnHome      0x02
#define lcdCtrl_CursorOn        0x0F
#define lcdCtrl_CursorOff       0x0C
#define lcdCtrl_SetDDRAM        0x80
#define lcdCtrl_DspOff          0x08

#define lcd_beginnZeile1        0x00
#define lcd_beginnZeile2        0x40
#define lcd_beginnZeile3        0x14
#define lcd_beginnZeile4        0x54

#define lcd_CursorPos   0x98 // \230(oktal)

#define LCD_ZeilenLaenge    16

//Definitionen für LCD-Textausrichtung
#define LCD_Zeile1              0u
#define LCD_Zeile2              1u
#define LCD_Zeile3              2u
#define LCD_Zeile4              3u

#define LCD_linksbuendig        0x01u
#define LCD_mittig              0x02u
#define LCD_rechtsbuendig       0x03u


void LCD_StateMachine(bool aktDisp);
void LCD_NewData(const char* Daten,unsigned char Zeile,unsigned char Ausrichtung);
void lcd_writeArray(void);
void lcd_write_data(unsigned char c);
void lcd_write_ctrl(unsigned char c);
bool lcd_read_BusyFlag(void);
void lcd_init(void);
void lcd_busy(void);
void lcd_writeCustomCharacters(void);


const char CustomChars[8][8] =  {  {0b01000,//è
                                    0b00100,
                                    0b01110,
                                    0b10001,
                                    0b11111,
                                    0b10000,
                                    0b01110,
                                    0b00000,},
                                    
                                   {0b00010,//é
                                    0b00100,
                                    0b01110,
                                    0b10001,
                                    0b11111,
                                    0b10000,
                                    0b01110,
                                    0b00000,},  
                                    
                                   {0b00100,//ê
                                    0b01010,
                                    0b01110,
                                    0b10001,
                                    0b11111,
                                    0b10000,
                                    0b01110,
                                    0b00000,},  
                                    
                                   {0b01000,//à
                                    0b00100,
                                    0b01110,
                                    0b00001,
                                    0b01111,
                                    0b10001,
                                    0b01111,
                                    0b00000,},
                                    
                                   {0b00010,//á
                                    0b00100,
                                    0b01110,
                                    0b00001,
                                    0b01111,
                                    0b10001,
                                    0b01111,
                                    0b00000,},  
                                    
                                   {0b00100,//â
                                    0b01010,
                                    0b01110,
                                    0b00001,
                                    0b01111,
                                    0b10001,
                                    0b01111,
                                    0b00000,}, 
                                };

#endif
