#include "LCD_header.h"
#include "Main-Header.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>


volatile unsigned char DispText[4][22];


void LCD_NewData(const char* Daten,unsigned char Zeile,unsigned char Ausrichtung)
{
    char* pointer;
    const char* pointerConst = Daten;
    unsigned char length = 0;
    unsigned char auffuellen;
    unsigned char auffuellenrechts;
    unsigned char buf[20];
    
    DispText[Zeile][0] = 0;
    
    while(*pointerConst != '\0' && length < LCD_ZeilenLaenge)
    {
        if(*pointerConst != lcd_CursorPos)
            length ++;
        pointerConst ++;
    }
    if(length < LCD_ZeilenLaenge)
    {
        auffuellen = (unsigned)(LCD_ZeilenLaenge - length);
        if(Ausrichtung == LCD_linksbuendig)
        {
            strcpy((char*)DispText[Zeile],(const char *)Daten);
            for(unsigned char i = 0; i < auffuellen; i++)
            {
                buf[i] = ' ';
            }
            buf[auffuellen] = '\0';
            strcat((char*)DispText[Zeile],(const char *)buf);
        }
        else if(Ausrichtung == LCD_mittig)
        {
            if(auffuellen % 2)
                auffuellenrechts = (auffuellen/2u)+1u;
            else
                auffuellenrechts = auffuellen/2u;
            auffuellen /=2u;
            for(unsigned char i = 0; i < auffuellen; i++)
            {
                buf[i] = ' ';
            }
            buf[auffuellen] = '\0';
            strcpy((char*)DispText[Zeile],(const char *)buf);
            strcat((char*)DispText[Zeile],(const char *)Daten);
            for(unsigned char i = 0; i < auffuellenrechts; i++)
            {
                buf[i] = ' ';
            }
            buf[auffuellenrechts] = '\0';
            strcat((char*)DispText[Zeile],(const char *)buf);
        }
        else if(Ausrichtung == LCD_rechtsbuendig)
        { 
            for(unsigned char i = 0; i < auffuellen; i++)
            {
                buf[i] = ' ';
            }
            buf[auffuellen] = '\0';
            strcpy((char*)DispText[Zeile],(const char *)buf);
            strcat((char*)DispText[Zeile],(const char *)Daten);
        }
    }
    else
    {
        strcpy((char*)DispText[Zeile],(const char *)Daten);
    }
    pointer = (char*)&DispText[Zeile][0];
    while(*pointer != '\0')
    {
        if(*pointer == 'ä'){
            *pointer = 0xE1u;}
        else if(*pointer == 'ü'){
            *pointer = 0xF5u;}
        else if(*pointer == 'ö'){
            *pointer = 0xEFu;}
        else if(*pointer == 'ß'){
            *pointer = 0xE2u;}
        else if(*pointer == '°'){
            *pointer = 0xDFu;}
        else if(*pointer == '<'){
            *pointer = 0x7Fu;}
        else if(*pointer == '>'){
            *pointer = 0x7Eu;}
        else if(*pointer == 'è'){
            *pointer = 0x80u;}
        else if(*pointer == 'é'){
            *pointer = 0x01u;}
        else if(*pointer == 'ê'){
            *pointer = 0x02u;}
        else if(*pointer == 'à'){
            *pointer = 0x03u;}
        else if(*pointer == 'á'){
            *pointer = 0x04u;}
        else if(*pointer == 'â'){
            *pointer = 0x05u;}
        else if(*pointer == '´'){
            *pointer = 0x27u;}
        pointer ++;
    }
}

void lcd_writeArray(void)
{    
    unsigned char CursorPosition = 0;
    
    lcd_write_ctrl(lcdCtrl_ReturnHome);
    
    for(unsigned char zeile = 0; zeile < 4; zeile++)
    {
        if(zeile == 1)
            lcd_write_ctrl(lcdCtrl_SetDDRAM | lcd_beginnZeile2);
        else if(zeile == 2)
            lcd_write_ctrl(lcdCtrl_SetDDRAM | lcd_beginnZeile3);
        else if(zeile == 3)
            lcd_write_ctrl(lcdCtrl_SetDDRAM | lcd_beginnZeile4);
        for(unsigned char spalte = 0; spalte < 22; spalte++)
        {       
            if(DispText[zeile][spalte] == 0)
                break;
            else 
            if(DispText[zeile][spalte] == lcd_CursorPos)
            {
                if(zeile == 0)
                    CursorPosition = spalte;
                else if(zeile == 1)
                    CursorPosition = 0x40u + spalte;
                else if(zeile == 2)
                    CursorPosition = 0x14u + spalte;
                else if(zeile == 3)
                    CursorPosition = 0x54u + spalte;
            }
            else
                lcd_write_data(DispText[zeile][spalte]);
        }
    }
    if(CursorPosition)
    {
        lcd_write_ctrl(lcdCtrl_CursorOn);
        lcd_write_ctrl(lcdCtrl_SetDDRAM | (unsigned)CursorPosition);
    }
    else
        lcd_write_ctrl(lcdCtrl_CursorOff);
}
void lcd_write_data(unsigned char c)
{
    
    if(c == 0x80)
        c = 0;
    
    
        lcd_data_write = ((((unsigned)c >> 4u) & 0x0Fu) | 0x10u);
        lcd_rw = 0;
        lcd_rs = 1;  
        lcd_busy();
        // Senden des oberen Nibbles
        lcd_e = 1;
        lcd_busy();
        lcd_e = 0;
        
        // Senden des unteren Nibbles
        lcd_data_write = ((unsigned)c & 0x0Fu) | 0x10u;
        lcd_rw = 0;
        lcd_rs = 1;
        lcd_busy();
        lcd_e = 1;
        lcd_busy();
        lcd_e = 0;
        CLRWDT();
        while(!lcd_read_BusyFlag())
        {
        }
}
void lcd_write_ctrl(unsigned char c)
{
    
    
        lcd_data_write = ((((unsigned)c >> 4u) & 0x0Fu) | 0x10u);
        lcd_rw = 0;
        lcd_rs = 0;  
        lcd_busy();
        // Senden des oberen Nibbles
        lcd_e = 1;
        lcd_busy();
        lcd_e = 0;
        
        // Senden des unteren Nibbles
        lcd_data_write = ((unsigned)c & 0x0Fu) | 0x10u;
        lcd_rw = 0;
        lcd_rs = 0;
        lcd_busy();
        lcd_e = 1;
        lcd_busy();
        lcd_e = 0;
        CLRWDT();
        while(!lcd_read_BusyFlag())
        {
        }
}
bool lcd_read_BusyFlag(void)
{
        unsigned char buf = 0;

        lcd_data_TRIS = 0x0F;
        lcd_e = 0;
        lcd_rw = 1;
        lcd_rs = 0; 
         
        // Lesen des oberen Nibbles       
        lcd_e = 1;
        lcd_busy();
        buf = (lcd_data_read &(unsigned)0x0F)<< (unsigned)4;
        lcd_e = 0;       
        // Lesen des unteren Nibbles
        lcd_busy();        
        lcd_e = 1;
        lcd_busy();
        buf += lcd_data_read & (unsigned)0x0F;
        lcd_e = 0;
        lcd_data_TRIS = 0x00;
        
        if(buf & 0x80)
            return  0;
        else 
            return 1;
}
//unsigned char lcd_read_DDRAMAdr(void)
//{       
//        lcd_data_TRIS = 0x0F;
//        unsigned char buf = 0;
//        lcd_e = 0;
//        lcd_rw = 1;
//        lcd_rs = 0; 
//        lcd_busy();       
//        // Lesen des oberen Nibbles       
//        lcd_e = 1;
//        lcd_busy();
//        buf = (lcd_data_read &(unsigned)0x07)<< (unsigned)4;
//        lcd_e = 0;       
//        // Lesen des unteren Nibbles
//        lcd_busy();        
//        lcd_e = 1;
//        lcd_busy();
//        buf += lcd_data_read & (unsigned)0x0F;
//        lcd_e = 0;
//        lcd_data_TRIS = 0x00;
//        return (buf);
//}
void lcd_init(void)
{
    
    lcd_data_TRIS = 0x00;
    TRISC = 0x00;
    for(unsigned char i = 0; i < 5; i++)
    {
        __delay_ms(10);
    }
    // #1
    lcd_write_ctrl(0b00000011);		// Auf 8 Bit setzten
//    __delay_ms(18);
//    __delay_ms(18);
    __delay_ms(5);
    // #2
    lcd_write_ctrl(0b00000011);		// ..
    __delay_ms(1);
    // #3
    lcd_write_ctrl(0b00000011);		// ..
    lcd_write_ctrl(0b00000010);		// Auf 4 Bit setzen
    lcd_write_ctrl(0x01);           // löschen und cusor home
    lcd_write_ctrl(0x28);			// Functionset (2 Zeilen)
    lcd_write_ctrl(0x08);			// display off
    lcd_write_ctrl(0x06);           // entry mode
    lcd_write_ctrl(0x03);			// cursor home
    lcd_write_ctrl(0x0f);           // display on
    lcd_busy();
    lcd_write_ctrl(0x01);           // Clear the display
    CLRWDT();
    while(!lcd_read_BusyFlag())
    {

    }
    lcd_writeCustomCharacters();
}
void lcd_busy(void)
{
    __delay_us(50);
}
void lcd_writeCustomCharacters(void)
{
    lcd_write_ctrl(0x40);
    for(unsigned char i = 0; i < 8; i++)
    {
        for(unsigned char j = 0; j < 8; j++)
            lcd_write_data(CustomChars[i][j]);
    }
}