#ifndef _UI_H_
#define _UI_H_
#include "stdlib.h"
#include "lcd.h"

void LCD_FontTest();
void LCD_DisplayUIBox(u8 mode);
void LCD_DrawCharScale(u16 x,u16 y,u16 fc, u16 bc, char num, u8 size, u16 scale);
void printchr(u16 x,u16 y, u16 fc, const char *p, u8 elem, u8 scale);
void printmod(int mod, u16 mode, u8 sign);
void printdadv(const u8 p);
void printmnsign(const u8 p);
void printmodsign(const u8 p);
void clrmain();
void clrmod();
void clrdadv();


const Picture lrgnum1;
const Picture lrgnum2;
const Picture lrgnum3;
const Picture lrgnum4;
const Picture lrgnum5;
const Picture lrgnum6;
const Picture lrgnum7;
const Picture lrgnum8;
const Picture lrgnum9;
const Picture lrgnum0;

#endif
