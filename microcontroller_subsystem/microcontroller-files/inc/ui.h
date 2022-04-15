#ifndef _UI_H_
#define _UI_H_
#include "stdlib.h"
#include "lcd.h"

void LCD_FontTest();
void LCD_DisplayUIBox(u8 mode);
void LCD_DrawCharScale(u16 x,u16 y,u16 fc, u16 bc, char num, u8 size, u16 scale);
void LCD_DrawScale2(const Picture *pic, uint8_t scale);
void printchr(u16 x,u16 y, u16 fc, const char *p, u8 elem, u8 scale);
void printmod(int mod, u16 mode, u8 sign);
void printdadv(const u8 p);
void printmnsign(const u8 p);
void printmodsign(const u8 p);
void clrmain();
void clrmod();
void clrdadv();

#endif
