
//============================================================================
// ui.c by Alexandra E. Sichterman
//============================================================================

#include "stm32f0xx.h"
#include <stdint.h>
#include "lcd.h"
#include "ui.h"
#include "string.h"

#define TempPicturePtr(name,width,height) Picture name[(width)*(height)/6+2] = { {width,height,2} }

extern const Picture lrgnum1;
extern const Picture lrgnum2;
extern const Picture lrgnum3;
extern const Picture lrgnum4;
extern const Picture lrgnum5;
extern const Picture lrgnum6;
extern const Picture lrgnum7;
extern const Picture lrgnum8;
extern const Picture lrgnum9;
extern const Picture lrgnum0;

extern u8 SIGN;
extern int mod;
extern int sub_mod;
extern uint8_t DADV;

void printmod(int mod, u16 mode, u8 sign) {
    char str[3] = "   ";
    itoa(mod, str, 10);
    if (str[0]==' '){
            str[2] = ' ';
            str[1] = ' ';
            str[0] = ' ';
    }
    else if (str[1]=='\0') {
        str[2] = str[0];
        str[1] = ' ';
        str[0] = ' ';
    }
    else if (str[2]=='\0'){
        str[2] = str[1];
        str[1] = str[0];
        str[0] = ' ';
    }

    if(mode==0)
        printmnsign(sign);
    else if(mode==1)
        printmodsign(sign);

    printchr(0,0,BLACK,str,mode,0);
}

void printdadv(const u8 p){
    if (p==1)
        printchr(0,0,BLUE,"A",3,0);
    else if (p==2)
        printchr(0,0,RED,"D",3,0);
    else
        printchr(0,0,LGRAY,"-",3,0);
}

void printmnsign(const u8 p){
    if (p==0)
        printchr(0,0,BLACK,"+",5,0);
    else
        printchr(0,0,BLACK,"-",5,0);
}

void printmodsign(const u8 p){
    if (p==0)
        printchr(0,0,BLACK,"+",4,0);
    else
        printchr(0,0,BLACK,"-",4,0);
}

void clrmain(){
    printchr(0,0,LGRAY, "000",0,0);
    printchr(0,0,LGRAY, " ",5,0);
}

void clrmod(){
    printchr(0,0,LGRAY, "000",1,0);
    printchr(0,0,LGRAY, "-", 4,0);
}

void clrdadv(){
    printchr(0,0,LGRAY,"-",3,0);
}

//===========================================================================
// Define number areas on screen
// elem:    0 = main number
//          1 = modifier
//          2 = previous number
//          3 = advantage/disadvantage
//          4 = modifier sign
//          5 = main number sign
//===========================================================================
void printchr(u16 x,u16 y, u16 fc, const char *p, u8 elem, u8 scale)
{
    u8 size;
    u16 x_inc;

    // Main Number
    if (elem == 0) {
        x       = 37;
        x_inc   = 84;
        y       = 120;
        size    = 11;
        scale   = 8;
    }

    // Modifier
    else if (elem == 1) {
        x       = 108;
        x_inc   = 38;
        y       = 59;
        size    = 7;
        scale   = 4;
    }

    // Previous Number
    else if (elem == 2){
        x       = 16;
        x_inc   = 29;
        y       = 14;
        size    = 7;
        scale   = 3;
    }

    // Advantage/Disadvantage
    else if (elem == 3){
        x       = 36;
        x_inc   = 0;
        y       = 66;
        size    = 7;
        scale   = 2;
    }

    // Modifier Sign
    else if (elem == 4) {
            x       = 108;
            x_inc   = 38;
            y       = 59;
            size    = 7;
            scale   = 4;
    }

    // Main Number Sign
    else if (elem == 5) {
            x       = 4;
            x_inc   = 0;
            y       = 120;
            size    = 7;
            scale   = 4;
    }

    else
        return;

    while((*p<='~')&&(*p>=' '))
        {
            if(x>(lcddev.width-1)||y>(lcddev.height-1))
            return;
            LCD_DrawCharScale(x,y,fc,fc,*p,size,scale);
            x+=x_inc;
            p++;
        }

}


//===========================================================================
// Define number areas on screen
// When mode is set, display UI boxes
//===========================================================================
void LCD_DisplayUIBox(u8 mode) {
    u16 x = 320;
    u16 y = 240;
    u16 main_display_w = 79;
    u16 main_display_h = 97;
    u16 main_sign_w = 35;
    u16 main_sign_h = 35;
    u16 mod_w = 35;
    u16 mod_h = 43;
    u16 prev_w = 27;
    u16 prev_h = 33;
    u16 mod_sign_w = 25;
    u16 mod_sign_h = 25;
    u16 dadv_w = 22;
    u16 dadv_h = 27;

    u16 main_display1_x = 37;
    u16 main_display1_y = 120;
    u16 main_display2_x = 121;
    u16 main_display2_y = 120;
    u16 main_display3_x = 205;
    u16 main_display3_y = 120;

    u16 main_sign_x = 4;
    u16 main_sign_y = 120;

    u16 mod1_x = 105;
    u16 mod1_y = 55;
    u16 mod2_x = 143;
    u16 mod2_y = 55;
    u16 mod3_x = 181;
    u16 mod3_y = 55;

    u16 mod_sign_x = 77;
    u16 mod_sign_y = 65;

    u16 prev1_x = 14;
    u16 prev1_y = 14;
    u16 prev2_x = 43;
    u16 prev2_y = 14;
    u16 prev3_x = 72;
    u16 prev3_y = 14;

    u16 dadv_x = 34;
    u16 dadv_y = 63;

    if (mode) {
    LCD_DrawRectangle(main_display1_x + main_display_w, main_display1_y, main_display1_x,
                main_display1_y + main_display_h, RED);
    LCD_DrawPoint(main_display1_x + main_display_w, main_display1_y + main_display_h, GREEN);
    LCD_DrawRectangle(main_display2_x + main_display_w, main_display2_y, main_display2_x,
                main_display2_y + main_display_h, RED);
    LCD_DrawRectangle(main_display3_x + main_display_w,  main_display3_y, main_display3_x,
                main_display3_y + main_display_h, RED);

    LCD_DrawRectangle(main_sign_x + main_sign_w, main_sign_y, main_sign_x,
                main_sign_y + main_sign_h, GREEN);

    LCD_DrawRectangle(mod_sign_x + mod_sign_w, mod_sign_y, mod_sign_x,
                mod_sign_y + mod_sign_h, LIGHTBLUE);

    LCD_DrawRectangle(mod1_x + mod_w, mod1_y, mod1_x,
                mod1_y + mod_h, BLUE);
    LCD_DrawRectangle(mod2_x + mod_w, mod2_y, mod2_x,
                mod2_y + mod_h, BLUE);
    LCD_DrawRectangle(mod3_x + mod_w, mod3_y, mod3_x,
                mod3_y + mod_h, BLUE);

    LCD_DrawRectangle(prev1_x + prev_w, prev1_y, prev1_x,
                prev1_y + prev_h, MAGENTA);
    LCD_DrawRectangle(prev2_x + prev_w, prev2_y, prev2_x,
                prev2_y + prev_h, MAGENTA);
    LCD_DrawRectangle(prev3_x + prev_w, prev3_y, prev3_x,
                prev3_y + prev_h, MAGENTA);

    LCD_DrawRectangle(dadv_x + dadv_w, dadv_y, dadv_x,
                dadv_y + dadv_h, LIGHTBLUE);
    }

    // Main Number Scale: 8
    LCD_DrawCharScale(main_display1_x, main_display1_y, BLACK, BLACK, '1', 11, 8);
    LCD_DrawCharScale(main_display2_x, main_display2_y, BLACK, BLACK, '2', 11, 8);
    LCD_DrawCharScale(main_display3_x, main_display3_y, BLACK, BLACK, '3', 11, 8);

    // Main Sign
    LCD_DrawCharScale(main_sign_x, main_sign_y, BLACK, BLACK, '+', 7, 4);

    // Modifier Number Scale: 4
    LCD_DrawCharScale(mod1_x+3, mod1_y+4, GRAY, GRAY, '1', 7, 4);
    LCD_DrawCharScale(mod2_x+3, mod2_y+4, GRAY, GRAY, '2', 7, 4);
    LCD_DrawCharScale(mod3_x+3, mod3_y+4, GRAY, GRAY, '3', 7, 4);

    // Previous Number
    LCD_DrawCharScale(prev1_x+2, prev1_y+3, LGRAY, LGRAY, '1', 7, 3);
    LCD_DrawCharScale(prev2_x+2, prev2_y+3, LGRAY, LGRAY, '2', 7, 3);
    LCD_DrawCharScale(prev3_x+2, prev3_y+3, LGRAY, LGRAY, '3', 7, 3);

    // Advantage
    LCD_DrawCharScale(dadv_x+2, dadv_y+3, BLACK, BLACK, 'A', 7, 2);

    // Modifier Sign
    LCD_DrawCharScale(mod_sign_x+4, mod_sign_y+3, GRAY, GRAY, '+', 7, 2);

}

void LCD_DrawCharScale(u16 x,u16 y,u16 fc, u16 bc, char num, u8 size, u16 scale)
{
    short temp;
    u16 pos,t,scl1,scl2,width;
    u16 xpos,ypos;
    num=num-' ';
    LCD_SetWindow(x,y,(x+size/2-1)*scale,(y+size-1)*scale);
    for(pos=0;pos<size;pos++) {
        for(scl1=0;scl1<=scale;scl1++){
            if (size==12)
                temp=asc2_1206[num][pos];
            else if (size==16)
                temp=asc2_1608[num][pos];
            else if (size==11){
                if (num==' '-' ')
                    temp=lrgfnt1[0][pos];
                else
                    temp=lrgfnt1[num-15][pos];
            }

            else if (size==7)
                if (num=='A'-' ')
                    temp=smlfnt1[11][pos];
                else if (num=='D'-' ')
                    temp=smlfnt1[12][pos];
                else if (num=='+'-' ')
                    temp=smlfnt1[13][pos];
                else if (num=='-'-' ')
                    temp=smlfnt1[14][pos];
                else if (num==' '-' ')
                    temp=smlfnt1[0][pos];
                else
                    temp=smlfnt1[num-15][pos];
            else {
                temp=smlfnt1[num-15][pos];
            }

            if (size == 11) {
                width = 12;
//                    temp>>=3;
            }
            else if (size == 7)
                width = 6;
            else
                width = size;
            for (t=0;t<width;t++)
            {
                for (scl2=0;scl2<=scale;scl2++) {
                    xpos=x+t+scl2+(t*scale);
                    ypos=y+pos+scl1+(scale*pos);
                    if(temp&0x01) {
                        LCD_DrawPoint(xpos,ypos,fc);
                    }
                    else {
                        LCD_DrawPoint(xpos,ypos,WHITE);
                    }

                }
                temp>>=1;

            }
        }
    }
}

void pic_subset(Picture *dst, const Picture *src, int sx, int sy) {
    int dw = dst->width;
    int dh = dst->height;
    if (dw + sx > src->width)
        for(;;)
            ;
    if (dh + sy > src->height)
        for(;;)
            ;
    for(int y=0; y<dh; y++)
        for(int x=0; x<dw; x++)
            dst->pix2[dw * y + x] = src->pix2[src->width * (y+sy) + x + sx];
}

void pic_overlay(Picture *dst, int xoffset, int yoffset, const Picture *src, int transparent) {
    for(int y=0; y<src->height; y++) {
        int dy = y+yoffset;
        if (dy < 0 || dy >= dst->height)
            continue;
        for(int x=0; x<src->width; x++) {
            int dx = x+xoffset;
            if (dx < 0 || dx >= dst->width)
                continue;
            unsigned short int p = src->pix2[y*src->width + x];
            if (p != transparent)
                dst->pix2[dy*dst->width + dx] = p;
        }
    }
}

void LCD_InitUI() {
    LCD_Clear(WHITE);

//    LCD_DrawFillRectangle(13, 74, 88, 99, BLACK);
    //LCD_DrawLine(15, 97, 33, 97, WHITE);
//    LCD_DrawFillRectangle(122, 74, 197, 99, BLACK);
    //LCD_DrawLine(124, 97, 142, 97, WHITE);
//    LCD_DrawFillRectangle(231, 74, 306, 99, BLACKs);
    //LCD_DrawLine(233, 97, 251, 97, WHITE);

    LCD_DrawString(35, 58, BLACK, WHITE, "Roll", 16, 0);
    LCD_DrawString(148, 58, BLACK, WHITE, "Mod", 16, 0);
    LCD_DrawString(243, 58, BLACK, WHITE, "Current", 16, 0);

    LCD_DrawTotal(0);
    LCD_DrawRoll(0);
    LCD_DrawSubMod();
    LCD_DrawMod();
    LCD_DrawDAdv();
}

void LCD_DrawScale2(u16 x0, u16 y0, const Picture *pic, u16 color) {
    TempPicturePtr(temp, 18, 22);
    for(int y = 0; y < temp->height; y++) {
        for(int x = 0; x < temp->width; x++) {
            if (color == 0) {
                temp->pix2[y*temp->width + x] = pic->pix2[(y>>1)*pic->width + (x>>1)];
            }
            else {
                if (pic->pix2[(y>>1)*pic->width + (x>>1)] != 65535) {
                    temp->pix2[y*temp->width + x] = color;
                }
                else {
                    temp->pix2[y*temp->width + x] = 65535;
                }
            }
        }
    }

    LCD_DrawPicture(x0, y0, temp);
}

void LCD_DrawScale3(u16 x0, u16 y0, const Picture *pic, u16 color) {
    TempPicturePtr(temp, 27, 33);
    for(int y = 0; y < temp->height; y++) {
        for(int x = 0; x < temp->width; x++) {
            if (color == 0) {
                temp->pix2[y*temp->width + x] = pic->pix2[(y/3)*pic->width + (x)/3];
            }
            else {
                if (pic->pix2[(y/3)*pic->width + (x)/3] != 65535) {
                    temp->pix2[y*temp->width + x] = color;
                }
                else {
                    temp->pix2[y*temp->width + x] = 65535;
                }
            }
        }
    }

    LCD_DrawPicture(x0, y0, temp);
}

void LCD_DrawScale4(u16 x0, u16 y0, const Picture *pic, u16 color) {
    TempPicturePtr(temp, 36, 44);
    for(int y = 0; y < temp->height; y++) {
        for(int x = 0; x < temp->width; x++) {
            if (color == 0) {
                temp->pix2[y*temp->width + x] = pic->pix2[(y>>2)*pic->width + (x>>2)];
            }
            else {
                if (pic->pix2[(y>>2)*pic->width + (x>>2)] != 65535) {
                    temp->pix2[y*temp->width + x] = color;
                }
                else {
                    temp->pix2[y*temp->width + x] = 65535;
                }
            }
        }
    }

    LCD_DrawPicture(x0, y0, temp);
}

void LCD_DrawScale8(u16 x0, u16 y0, const Picture *pic, u16 color) {
    TempPicturePtr(temp, 72, 88);
    for(int y = 0; y < temp->height; y++) {
        for(int x = 0; x < temp->width; x++) {
            if (color == 0) {
                temp->pix2[y*temp->width + x] = pic->pix2[(y>>3)*pic->width + (x>>3)];
            }
            else {
                if (pic->pix2[(y>>3)*pic->width + (x>>3)] != 65535) {
                    temp->pix2[y*temp->width + x] = color;
                }
                else {
                    temp->pix2[y*temp->width + x] = 65535;
                }
            }
        }
    }

    LCD_DrawPicture(x0, y0, temp);
}

void LCD_DrawTotal(int total) {
    int num = total < 0 ? 0 - total : total;

    u8 ones = num % 10;
    u8 tens = (num / 10) % 10;
    u8 huns = num / 100;

    switch(huns) {
    case 0: LCD_DrawScale8(57, 125, &lrgnum0, 0);
            break;
    case 1: LCD_DrawScale8(57, 125, &lrgnum1, 0);
            break;
    case 2: LCD_DrawScale8(57, 125, &lrgnum2, 0);
            break;
    case 3: LCD_DrawScale8(57, 125, &lrgnum3, 0);
            break;
    case 4: LCD_DrawScale8(57, 125, &lrgnum4, 0);
            break;
    case 5: LCD_DrawScale8(57, 125, &lrgnum5, 0);
            break;
    case 6: LCD_DrawScale8(57, 125, &lrgnum6, 0);
            break;
    case 7: LCD_DrawScale8(57, 125, &lrgnum7, 0);
            break;
    case 8: LCD_DrawScale8(57, 125, &lrgnum8, 0);
            break;
    case 9: LCD_DrawScale8(57, 125, &lrgnum9, 0);
            break;
    }

    switch(tens) {
    case 0: LCD_DrawScale8(141, 125, &lrgnum0, 0);
            break;
    case 1: LCD_DrawScale8(141, 125, &lrgnum1, 0);
            break;
    case 2: LCD_DrawScale8(141, 125, &lrgnum2, 0);
            break;
    case 3: LCD_DrawScale8(141, 125, &lrgnum3, 0);
            break;
    case 4: LCD_DrawScale8(141, 125, &lrgnum4, 0);
            break;
    case 5: LCD_DrawScale8(141, 125, &lrgnum5, 0);
            break;
    case 6: LCD_DrawScale8(141, 125, &lrgnum6, 0);
            break;
    case 7: LCD_DrawScale8(141, 125, &lrgnum7, 0);
            break;
    case 8: LCD_DrawScale8(141, 125, &lrgnum8, 0);
            break;
    case 9: LCD_DrawScale8(141, 125, &lrgnum9, 0);
            break;
    }

    switch(ones) {
    case 0: LCD_DrawScale8(225, 125, &lrgnum0, 0);
            break;
    case 1: LCD_DrawScale8(225, 125, &lrgnum1, 0);
            break;
    case 2: LCD_DrawScale8(225, 125, &lrgnum2, 0);
            break;
    case 3: LCD_DrawScale8(225, 125, &lrgnum3, 0);
            break;
    case 4: LCD_DrawScale8(225, 125, &lrgnum4, 0);
            break;
    case 5: LCD_DrawScale8(225, 125, &lrgnum5, 0);
            break;
    case 6: LCD_DrawScale8(225, 125, &lrgnum6, 0);
            break;
    case 7: LCD_DrawScale8(225, 125, &lrgnum7, 0);
            break;
    case 8: LCD_DrawScale8(225, 125, &lrgnum8, 0);
            break;
    case 9: LCD_DrawScale8(225, 125, &lrgnum9, 0);
            break;
    }

    if (total < 0)
        LCD_DrawCharScale(28, 155, BLACK, WHITE, '-', 7, 3);
    else
        LCD_DrawCharScale(28, 155, BLACK, WHITE, '+', 7, 3);
}

void LCD_DrawRoll(int roll) {
    int num = roll < 0 ? 0 - roll : roll;

    u8 ones = num % 10;
    u8 tens = (num / 10) % 10;
    u8 huns = num / 100;

    switch(huns) {
    case 0: LCD_DrawScale2(33, 76, &lrgnum0, RED);
            break;
    case 1: LCD_DrawScale2(33, 76, &lrgnum1, RED);
            break;
    case 2: LCD_DrawScale2(33, 76, &lrgnum2, RED);
            break;
    case 3: LCD_DrawScale2(33, 76, &lrgnum3, RED);
            break;
    case 4: LCD_DrawScale2(33, 76, &lrgnum4, RED);
            break;
    case 5: LCD_DrawScale2(33, 76, &lrgnum5, RED);
            break;
    case 6: LCD_DrawScale2(33, 76, &lrgnum6, RED);
            break;
    case 7: LCD_DrawScale2(33, 76, &lrgnum7, RED);
            break;
    case 8: LCD_DrawScale2(33, 76, &lrgnum8, RED);
            break;
    case 9: LCD_DrawScale2(33, 76, &lrgnum9, RED);
            break;
    }

    switch(tens) {
    case 0: LCD_DrawScale2(51, 76, &lrgnum0, RED);
            break;
    case 1: LCD_DrawScale2(51, 76, &lrgnum1, RED);
            break;
    case 2: LCD_DrawScale2(51, 76, &lrgnum2, RED);
            break;
    case 3: LCD_DrawScale2(51, 76, &lrgnum3, RED);
            break;
    case 4: LCD_DrawScale2(51, 76, &lrgnum4, RED);
            break;
    case 5: LCD_DrawScale2(51, 76, &lrgnum5, RED);
            break;
    case 6: LCD_DrawScale2(51, 76, &lrgnum6, RED);
            break;
    case 7: LCD_DrawScale2(51, 76, &lrgnum7, RED);
            break;
    case 8: LCD_DrawScale2(51, 76, &lrgnum8, RED);
            break;
    case 9: LCD_DrawScale2(51, 76, &lrgnum9, RED);
            break;
    }

    switch(ones) {
    case 0: LCD_DrawScale2(69, 76, &lrgnum0, RED);
            break;
    case 1: LCD_DrawScale2(69, 76, &lrgnum1, RED);
            break;
    case 2: LCD_DrawScale2(69, 76, &lrgnum2, RED);
            break;
    case 3: LCD_DrawScale2(69, 76, &lrgnum3, RED);
            break;
    case 4: LCD_DrawScale2(69, 76, &lrgnum4, RED);
            break;
    case 5: LCD_DrawScale2(69, 76, &lrgnum5, RED);
            break;
    case 6: LCD_DrawScale2(69, 76, &lrgnum6, RED);
            break;
    case 7: LCD_DrawScale2(69, 76, &lrgnum7, RED);
            break;
    case 8: LCD_DrawScale2(69, 76, &lrgnum8, RED);
            break;
    case 9: LCD_DrawScale2(69, 76, &lrgnum9, RED);
            break;
    }

    if (roll < 0)
        LCD_DrawCharScale(15, 76, RED, WHITE, '-', 7, 2);
    else
        LCD_DrawCharScale(15, 76, RED, WHITE, '+', 7, 2);
}

void LCD_DrawSubMod() {
    int num = sub_mod < 0 ? 0 - sub_mod : sub_mod;

    u8 ones = num % 10;
    u8 tens = (num / 10) % 10;
    u8 huns = num / 100;

    switch(huns) {
    case 0: LCD_DrawScale2(251, 76, &lrgnum0, 0);
            break;
    case 1: LCD_DrawScale2(251, 76, &lrgnum1, 0);
            break;
    case 2: LCD_DrawScale2(251, 76, &lrgnum2, 0);
            break;
    case 3: LCD_DrawScale2(251, 76, &lrgnum3, 0);
            break;
    case 4: LCD_DrawScale2(251, 76, &lrgnum4, 0);
            break;
    case 5: LCD_DrawScale2(251, 76, &lrgnum5, 0);
            break;
    case 6: LCD_DrawScale2(251, 76, &lrgnum6, 0);
            break;
    case 7: LCD_DrawScale2(251, 76, &lrgnum7, 0);
            break;
    case 8: LCD_DrawScale2(251, 76, &lrgnum8, 0);
            break;
    case 9: LCD_DrawScale2(251, 76, &lrgnum9, 0);
            break;
    }

    switch(tens) {
    case 0: LCD_DrawScale2(269, 76, &lrgnum0, 0);
            break;
    case 1: LCD_DrawScale2(269, 76, &lrgnum1, 0);
            break;
    case 2: LCD_DrawScale2(269, 76, &lrgnum2, 0);
            break;
    case 3: LCD_DrawScale2(269, 76, &lrgnum3, 0);
            break;
    case 4: LCD_DrawScale2(269, 76, &lrgnum4, 0);
            break;
    case 5: LCD_DrawScale2(269, 76, &lrgnum5, 0);
            break;
    case 6: LCD_DrawScale2(269, 76, &lrgnum6, 0);
            break;
    case 7: LCD_DrawScale2(269, 76, &lrgnum7, 0);
            break;
    case 8: LCD_DrawScale2(269, 76, &lrgnum8, 0);
            break;
    case 9: LCD_DrawScale2(269, 76, &lrgnum9, 0);
            break;
    }

    switch(ones) {
    case 0: LCD_DrawScale2(287, 76, &lrgnum0, 0);
            break;
    case 1: LCD_DrawScale2(287, 76, &lrgnum1, 0);
            break;
    case 2: LCD_DrawScale2(287, 76, &lrgnum2, 0);
            break;
    case 3: LCD_DrawScale2(287, 76, &lrgnum3, 0);
            break;
    case 4: LCD_DrawScale2(287, 76, &lrgnum4, 0);
            break;
    case 5: LCD_DrawScale2(287, 76, &lrgnum5, 0);
            break;
    case 6: LCD_DrawScale2(287, 76, &lrgnum6, 0);
            break;
    case 7: LCD_DrawScale2(287, 76, &lrgnum7, 0);
            break;
    case 8: LCD_DrawScale2(287, 76, &lrgnum8, 0);
            break;
    case 9: LCD_DrawScale2(287, 76, &lrgnum9, 0);
            break;
    }

    if (!SIGN)
        LCD_DrawCharScale(233, 76, BLACK, WHITE, '+', 7, 2);
    else
        LCD_DrawCharScale(233, 76, BLACK, WHITE, '-', 7, 2);
}

void LCD_DrawMod() {
    int num = mod < 0 ? 0 - mod : mod;

    u8 ones = num % 10;
    u8 tens = (num / 10) % 10;
    u8 huns = num / 100;

    switch(huns) {
    case 0: LCD_DrawScale2(142, 76, &lrgnum0, BLUE);
            break;
    case 1: LCD_DrawScale2(142, 76, &lrgnum1, BLUE);
            break;
    case 2: LCD_DrawScale2(142, 76, &lrgnum2, BLUE);
            break;
    case 3: LCD_DrawScale2(142, 76, &lrgnum3, BLUE);
            break;
    case 4: LCD_DrawScale2(142, 76, &lrgnum4, BLUE);
            break;
    case 5: LCD_DrawScale2(142, 76, &lrgnum5, BLUE);
            break;
    case 6: LCD_DrawScale2(142, 76, &lrgnum6, BLUE);
            break;
    case 7: LCD_DrawScale2(142, 76, &lrgnum7, BLUE);
            break;
    case 8: LCD_DrawScale2(142, 76, &lrgnum8, BLUE);
            break;
    case 9: LCD_DrawScale2(142, 76, &lrgnum9, BLUE);
            break;
    }

    switch(tens) {
    case 0: LCD_DrawScale2(160, 76, &lrgnum0, BLUE);
            break;
    case 1: LCD_DrawScale2(160, 76, &lrgnum1, BLUE);
            break;
    case 2: LCD_DrawScale2(160, 76, &lrgnum2, BLUE);
            break;
    case 3: LCD_DrawScale2(160, 76, &lrgnum3, BLUE);
            break;
    case 4: LCD_DrawScale2(160, 76, &lrgnum4, BLUE);
            break;
    case 5: LCD_DrawScale2(160, 76, &lrgnum5, BLUE);
            break;
    case 6: LCD_DrawScale2(160, 76, &lrgnum6, BLUE);
            break;
    case 7: LCD_DrawScale2(160, 76, &lrgnum7, BLUE);
            break;
    case 8: LCD_DrawScale2(160, 76, &lrgnum8, BLUE);
            break;
    case 9: LCD_DrawScale2(160, 76, &lrgnum9, BLUE);
            break;
    }

    switch(ones) {
    case 0: LCD_DrawScale2(178, 76, &lrgnum0, BLUE);
            break;
    case 1: LCD_DrawScale2(178, 76, &lrgnum1, BLUE);
            break;
    case 2: LCD_DrawScale2(178, 76, &lrgnum2, BLUE);
            break;
    case 3: LCD_DrawScale2(178, 76, &lrgnum3, BLUE);
            break;
    case 4: LCD_DrawScale2(178, 76, &lrgnum4, BLUE);
            break;
    case 5: LCD_DrawScale2(178, 76, &lrgnum5, BLUE);
            break;
    case 6: LCD_DrawScale2(178, 76, &lrgnum6, BLUE);
            break;
    case 7: LCD_DrawScale2(178, 76, &lrgnum7, BLUE);
            break;
    case 8: LCD_DrawScale2(178, 76, &lrgnum8, BLUE);
            break;
    case 9: LCD_DrawScale2(178, 76, &lrgnum9, BLUE);
            break;
    }

    if (mod < 0)
        LCD_DrawCharScale(124, 76, BLUE, WHITE, '-', 7, 2);
    else
        LCD_DrawCharScale(124, 76, BLUE, WHITE, '+', 7, 2);
}

void LCD_DrawDAdv() {
    if (DADV == 0) {
        LCD_DrawCharScale(240, 20, LIGHTGRAY, WHITE, 'A', 7, 2);
        LCD_DrawCharScale(280, 20, LIGHTGRAY, WHITE, 'D', 7, 2);
    }
    else if (DADV == 1) {
        LCD_DrawCharScale(240, 20, RED, WHITE, 'A', 7, 2);
        LCD_DrawCharScale(280, 20, WHITE, WHITE, 'D', 7, 2);
    }
    else if (DADV == 2) {
        LCD_DrawCharScale(240, 20, WHITE, WHITE, 'A', 7, 2);
        LCD_DrawCharScale(280, 20, BLUE, WHITE, 'D', 7, 2);
    }
}
