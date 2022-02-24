/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f0xx.h"
#include "fifo.h"
#include "tty.h"
#include <stdio.h>
#include "lcd.h"
#include <math.h>
#include "ui.h"

// GLOBAL VARIABLES
// Keypad Debouncing
uint8_t history[16];
uint8_t offset;

// Configurables
const static char KEY_ARRAY[] = "123+456-789AC0=D";
int mod = 0;
int sub_mod = 0;
uint8_t c_count = 0;
uint8_t A = 0;
uint8_t D = 0;
uint8_t PKG = 0;
uint8_t SIGN = 1;

// DAC
#define N 1000
#define RATE 20000
short int wavetable[N];
int volume = 2048;
int stepa = 0;
int stepb = 0;
int stepc = 0;
int stepd = 0;
int offseta = 0;
int offsetb = 0;
int offsetc = 0;
int offsetd = 0;

// SETUP FUNCTIONS
void setup_ports() {
    RCC -> AHBENR |= 0x1e0000;

    GPIOA -> MODER &= ~(0xc03c03fc);
    GPIOA -> MODER |= 0x40280350;
    GPIOA -> PUPDR &= ~(0xc);
    GPIOA -> PUPDR |= 0x4;
    GPIOA -> AFR[1] &= ~(0xff0);
    GPIOA -> AFR[1] |= 0x110;

    GPIOB -> MODER &= ~(0xcf0f00ff);
    GPIOB -> MODER |= 0x8a0a0055;
    GPIOB -> AFR[1] &= ~(0xff0000ff);
    GPIOB -> AFR[1] |= 0x11;

    GPIOC -> MODER &= ~(0x300ffff);
    GPIOC -> MODER |= 0x2000000;
    GPIOC -> PUPDR &= ~(0xff00);
    GPIOC -> PUPDR |= 0xaa00;
    GPIOC -> AFR[1] &= ~(0xf0000);
    GPIOC -> AFR[1] |= 0x20000;

    GPIOD -> MODER &= ~(0x30);
    GPIOD -> MODER |= 0x20;
    GPIOD -> AFR[0] &= ~(0xf00);
    GPIOD -> AFR[0] |= 0x200;
}

void setup_usart5() {
    RCC -> APB1ENR |= 1 << 20;

    USART5 -> CR2 &= ~(0x00003000);
    USART5 -> BRR = 0x1a1;
    USART5 -> CR1 &= ~(0x10009401);
    USART5 -> CR1 |= 0x2c;
    USART5 -> CR1 |= 0x1;

    while(((USART5 -> CR1 >> 2) & 3) != 3);
    NVIC -> ISER[0] |= 1 << 29;
}

void setup_exti() {
    RCC -> APB2ENR |= 1;

    EXTI -> FTSR |= 1 << 1;
    EXTI -> IMR |= 1 << 1;

    NVIC -> ISER[0] |= 1 << 5;
}

void setup_spi2() {
    RCC -> APB1ENR |= 1 << 14;

    SPI2 -> CR2 = 0x070c;
    SPI2 -> CR1 = 0xc004;
    SPI2 -> CR1 |= 1 << 6;
}

void setup_tim7() {
    RCC -> APB1ENR |= 1 << 5;

    TIM7 -> PSC = 4799;
    TIM7 -> ARR = 9;
    TIM7 -> DIER |= 1;
    TIM7 -> CR1 |= 1;

    NVIC -> ISER[0] |= 1 << 18;
}

void setup_i2c1() {
    RCC -> APB1ENR |= 1 << 21;

    I2C1 -> CR1 &= ~(0x1);
    I2C1 -> CR1 &= ~(0x0002180);
    I2C1 -> CR2 |= 0x02000000;

    I2C1 -> TIMINGR = 0x00310309;

    I2C1 -> OAR1 &= ~(1 << 15);
    I2C1 -> OAR2 &= ~(1 << 15);

    I2C1 -> CR1 |= 1;
}

void setup_dac() {
    RCC -> APB1ENR |= 1<<29;

    DAC -> CR |= 1<<2;
    DAC -> CR |= 0x38;
    DAC -> CR |= 1;
}

void setup_tim6() {
    RCC -> APB1ENR |= 1 << 4;

    TIM6 -> PSC = 599;
    TIM6 -> ARR = 3;
    TIM6 -> DIER |= 1;
    TIM6 -> CR1 |= 1;

    NVIC -> ISER[0] = 1<<17;
}


// REGULAR FUNCTIONS
// UART
int better_putchar(int data) {
    if(data == '\n') {
        while(((USART5 -> ISR >> 7) & 1) == 0);
        USART5 -> TDR = '\r';
    }
    while(((USART5 -> ISR >> 7) & 1) == 0);
    USART5 -> TDR = data;
    return data;
}

int interrupt_getchar() {
    while(fifo_newline(&input_fifo) == 0) {
        asm volatile ("wfi");
    }
    return fifo_remove(&input_fifo);
}

int __io_putchar(int ch) {
    return better_putchar(ch);
}

int __io_getchar(void) {
    return interrupt_getchar();
}

// Keypad
void set_row() {
    GPIOB -> BSRR = 0xf0000;
    GPIOB -> BSRR = 1 << (offset & 3);
}

int get_cols() {
    return ((GPIOC -> IDR) & 0xf0) >> 4;
}

void update_hist(int cols) {
    int row = offset & 3;
    for(int i = 0; i < 4; i++) {
        history[4 * row + i] = (history[4 * row + i] << 1) + ((cols >> i) & 1);
    }
}

void toggle_A() {
    A ^= 0x1;
    D = 0;
}

void toggle_D() {
    D ^= 1;
    A = 0;
}

void clear() {
    if (c_count == 0) {
        c_count++;
        sub_mod = 0;
    } else if (c_count == 1) {
        c_count++;
        sub_mod = 0;
        mod = 0;
    } else if (c_count > 1) {
        c_count = 0;
        A = 0;
        D = 0;
        sub_mod = 0;
        mod = 0;
    }
}

void modifier(int num) {
    sub_mod = (10 * sub_mod) + num;
    if (sub_mod > 999)
        sub_mod = (sub_mod/100)%10*100 + (sub_mod/10)%10*10 + (sub_mod%10);
}

void add_mod() {
    if (SIGN == 1)
        mod += sub_mod;
    else
        mod -= sub_mod;

    sub_mod = 0;
}

void display_settings() {
    printf("Advantage: %2d\n", A);
    printf("Disadvantage: %2d\n", D);
    printf("Mod: %2d\n", mod);
    printf("Submod: %2d\n\n", sub_mod);
}

void handle_keypress(int cols) {
    int row = offset & 3;
    for(int i = 0; i < 4; i++) {
        int id = 4*row+i;
        if(history[id] == 0x3f) {

            char key = KEY_ARRAY[id];
            printf("Button %2c was pushed.\n", key);

            switch(key) {
            case 'A' :
                toggle_A();
                break;
            case 'D' :
                toggle_D();
                break;
            case 'C' :
                clear();
                break;
            case '+' :
                add_mod();
                SIGN = 1;
                PKG = 1;
                break;
            case '-' :
                add_mod();
                SIGN = 0;
                PKG = 1;
                break;
            case '=' :
                add_mod();
                PKG = 0;
                break;
            default:
                modifier(key - '0');
                break;
            }
            if(key != 'C')
                c_count = 0;
            display_settings();
            break;
        }
    }
}

// EEPROM
void i2c_start(uint32_t address, uint8_t size, uint8_t direction) {
    uint32_t tempreg = I2C1 -> CR2;
    tempreg &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES |
                 I2C_CR2_RELOAD | I2C_CR2_AUTOEND |
                 I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
    if(direction == 1)
        tempreg |= I2C_CR2_RD_WRN;
    else
        tempreg &= ~I2C_CR2_RD_WRN;
    tempreg |= ((address<<1) & I2C_CR2_SADD) | ((size<<16) & I2C_CR2_NBYTES);
    tempreg |= I2C_CR2_START;
    I2C1 -> CR2 = tempreg;
}

void i2c_stop() {
    if(I2C1 -> ISR & I2C_ISR_STOPF) {
        return;
    }
    I2C1 -> CR2 |= I2C_CR2_STOP;
    while((I2C1 -> ISR & I2C_ISR_STOPF) == 0);
    I2C1 -> ICR |= I2C_ICR_STOPCF;
}

int i2c_checknack() {
    int nack = (I2C1 -> ISR >> 4) & 1;
    I2C1 -> ICR = 1 << 4;
    return nack;
}

void i2c_waitidle() {
    while((I2C1 -> ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY);
}

int8_t i2c_senddata(uint8_t address, void* p_data, uint8_t size) {
    if(size <= 0 || p_data == 0) {
        return -1;
    }

    int index;
    int count = 0;
    uint8_t* a_data = (uint8_t*) p_data;

    i2c_waitidle();
    i2c_start(address, size, 0);

    for(index = 0; index < size; index++) {
        while((I2C1 -> ISR & I2C_ISR_TXIS) == 0) {
            count++;

            if(count > 1000000) {
                return -1;
            }

            if(i2c_checknack()) {
                i2c_stop();
                return -1;
            }
        }

        I2C1 -> TXDR = a_data[index] & I2C_TXDR_TXDATA;
    }

    while((I2C1 -> ISR & I2C_ISR_TC) == 0 && (I2C1 -> ISR & I2C_ISR_NACKF) == 0);
    if((I2C1 -> ISR & I2C_ISR_NACKF) != 0) {
        return -1;
    }
    i2c_stop();
    return 0;
}

uint8_t i2c_recvdata(uint8_t address, uint8_t* data, uint8_t size){
    if(size <= 0 || data == 0) {
        return -1;
    }

    int index;
    int count = 0;

    i2c_waitidle();
    i2c_start(address, size, 1);

    for(index = 0; index < (size & 0x1f); index++) {
        while((I2C1 -> ISR & I2C_ISR_RXNE) == 0) {
            count++;

            if(count > 1000000) {
                return -1;
            }

            if(i2c_checknack()) {
                i2c_stop();
                return -1;
            }
        }

        data[index] = I2C1 -> RXDR;
    }

    if((I2C1 -> ISR & I2C_ISR_NACKF) != 0) {
        return -1;
    }
    i2c_stop();
    return 0;
}

void i2c_write_flash(uint16_t loc, const char* data, uint8_t len) {
    char buffer[34];
    buffer[0] = (loc & 0xf00) >> 8;
    buffer[1] = loc & 0xff;

    for(int index = 0; index < len; index++) {
        buffer[2 + index] = data[index];
    }

    i2c_senddata(0x57, buffer, len + 2);
}

void i2c_read_flash(uint16_t loc, char data[], uint8_t len) {
    uint8_t location[2];
    location[0] = (loc & 0xf00) >> 8;
    location[1] = loc & 0xff;

    i2c_senddata(0x57, location, sizeof(location));

    for(int index = 0; index < len; index++) {
        i2c_recvdata(0x57, &data[index], 1);
    }
}

int i2c_write_flash_complete() {
    i2c_waitidle();
    i2c_start(0x57, 0, 0);

    while((I2C1 -> ISR & I2C_ISR_TC) == 0 && (I2C1 -> ISR & I2C_ISR_NACKF) == 0);
    if(i2c_checknack()) {
        i2c_stop();
        return 0;
    }

    i2c_stop();
    return 1;
}

// DAC
void enable_speaker() {
	GPIOA -> BRR = 1 << 15;
	DAC -> CR |= 1;
	TIM6 -> CR1 |= 1;
}

void disable_speaker() {
	GPIOA -> BSRR = 1 << 15;
	DAC -> CR &= ~(0x1);
	TIM6 -> CR1 &= ~(0x1);
}

void write_dac(int sample) {
    DAC -> DHR12R1 = sample;
    DAC -> SWTRIGR = 1;
}

void init_wavetable() {
    for(int index = 0; index < N; index++) {
        wavetable[index] = 32767 * sin(2 * M_PI * index / N);
    }
}

void set_freq_a(float f) {
    if(f == 0) {
        stepa = 0;
        offseta = 0;
    }
    else {
        stepa = f * N / RATE * (1<<16);
    }
}

void set_freq_b(float f) {
    if(f == 0) {
        stepb = 0;
        offsetb = 0;
    }
    else {
        stepb = f * N / RATE * (1<<16);
    }
}

void set_freq_c(float f) {
    if(f == 0) {
        stepc = 0;
        offsetc = 0;
    }
    else {
        stepc = f * N / RATE * (1<<16);
    }
}

void set_freq_d(float f) {
    if(f == 0) {
        stepd = 0;
        offsetd = 0;
    }
    else {
        stepd = f * N / RATE * (1<<16);
    }
}

// ISRs
void USART3_4_5_6_7_8_IRQHandler() {
    if(USART5 -> ISR & 0x8) {
        USART5 -> ICR |= 0x8;
    }
    char character = USART5 -> RDR;
    if(fifo_full(&input_fifo)) {
        return;
    }
    insert_echo_char(character);
}

void EXTI0_1_IRQHandler() {
    EXTI -> PR = 1 << 1;
    printf("Obstruction detected.\n");
}

void TIM7_IRQHandler() {
    TIM7 -> SR &= ~(1);
    int cols = get_cols();
    update_hist(cols);
    offset = (offset + 1) & 0x7;
    set_row();
    handle_keypress(cols);
}

void TIM6_DAC_IRQHandler() {
    float sample;

    TIM6 -> SR &= ~(1);
    DAC -> SWTRIGR |= 1;

    offseta += stepa;
    offsetb += stepb;
    offsetc += stepc;
    offsetd += stepd;

    if((offseta >> 16) >= N) {
        offseta -= (N << 16);
    }
    if((offsetb >> 16) >= N) {
        offsetb -= (N << 16);
    }
    if((offsetc >> 16) >= N) {
        offsetc -= (N << 16);
    }
    if((offsetd >> 16) >= N) {
        offsetd -= (N << 16);
    }

    sample = wavetable[offseta >> 16] + wavetable[offsetb >> 16] + wavetable[offsetc >> 16] + wavetable[offsetd >> 16];
    sample /= 32;
    sample += 2048;

    if(sample > 4095) {
        sample = 4095;
    }
    else if(sample < 0) {
        sample = 0;
    }

    DAC -> DHR12R1 = sample;
}

void LCD_DisplayUI() {
    u16 x = 320;
    u16 y = 240;
    u16 main_display_w = 79;
    u16 main_display_h = 97;
    u16 mod_w = 35;
    u16 mod_h = 43;
    u16 prev_w = 27;
    u16 prev_h = 33;
    u16 mod_sign_w = 25;
    u16 mod_sign_h = 25;
    u16 dadv_w = 22;
    u16 dadv_h = 27;

//    u16 main_display1_x = x - 37;
//    u16 main_display1_y = y - 120;
//    u16 main_display2_x = x - 121;
//    u16 main_display2_y = y - 120;
//    u16 main_display3_x = x - 205;
//    u16 main_display3_y = y - 120;
//
//    u16 mod1_x = x - 105;
//    u16 mod1_y = y - 55;
//    u16 mod2_x = x - 143;
//    u16 mod2_y = y - 55;
//    u16 mod3_x = x - 181;
//    u16 mod3_y = y - 55;
//
//    u16 mod_sign_x = x - 77;
//    u16 mod_sign_y = y - 65;
//
//    u16 prev1_x = x - 14;
//    u16 prev1_y = y - 14;
//    u16 prev2_x = x - 43;
//    u16 prev2_y = y - 14;
//    u16 prev3_x = x - 72;
//    u16 prev3_y = y - 14;
//
//    u16 dadv_x = x - 34;
//    u16 dadv_y = y - 63;

    u16 main_display1_x = 37;
    u16 main_display1_y = 120;
    u16 main_display2_x = 121;
    u16 main_display2_y = 120;
    u16 main_display3_x = 205;
    u16 main_display3_y = 120;

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


//    LCD_DrawRectangle(main_display1_x - main_display_w, main_display1_y, main_display1_x,
//                main_display1_y - main_display_h, RED);
//    LCD_DrawRectangle(main_display2_x - main_display_w, main_display2_y, main_display2_x,
//                main_display2_y - main_display_h, RED);
//    LCD_DrawRectangle(main_display3_x - main_display_w,  main_display3_y, main_display3_x,
//                main_display3_y - main_display_h, RED);
//
//    LCD_DrawRectangle(mod_sign_x - mod_sign_w, mod_sign_y, mod_sign_x,
//                mod_sign_y - mod_sign_h, LIGHTBLUE);
//
//    LCD_DrawRectangle(mod1_x - mod_w, mod1_y, mod1_x,
//                mod1_y - mod_h, BLUE);
//    LCD_DrawRectangle(mod2_x - mod_w, mod2_y, mod2_x,
//                mod2_y - mod_h, BLUE);
//    LCD_DrawRectangle(mod3_x - mod_w, mod3_y, mod3_x,
//                mod3_y - mod_h, BLUE);
//
//    LCD_DrawRectangle(prev1_x - prev_w, prev1_y, prev1_x,
//                prev1_y - prev_h, MAGENTA);
//    LCD_DrawRectangle(prev2_x - prev_w, prev2_y, prev2_x,
//                prev2_y - prev_h, MAGENTA);
//    LCD_DrawRectangle(prev3_x - prev_w, prev3_y, prev3_x,
//                prev3_y - prev_h, MAGENTA);
//
//    LCD_DrawRectangle(dadv_x - dadv_w, dadv_y, dadv_x,
//                dadv_y - dadv_h, LIGHTBLUE);

    LCD_DrawRectangle(main_display1_x + main_display_w, main_display1_y, main_display1_x,
                main_display1_y + main_display_h, RED);
    LCD_DrawRectangle(main_display2_x + main_display_w, main_display2_y, main_display2_x,
                main_display2_y + main_display_h, RED);
    LCD_DrawRectangle(main_display3_x + main_display_w,  main_display3_y, main_display3_x,
                main_display3_y + main_display_h, RED);

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


    LCD_DrawPicture(main_display1_x + main_display_w, main_display1_y, &lrgnum1);

}

int main(void)
{
    // Don't touch this
    setbuf(stdin, 0);
    setbuf(stdout, 0);
    setbuf(stderr, 0);

    //Or this
    setup_ports();
    setup_usart5();
    setup_exti();
    setup_spi2();
    setup_i2c1();
    setup_dac();
    init_wavetable();
    setup_tim6();
    disable_speaker();
    setup_tim7();

    // Debugging EEPROM, can be safely removed
//    const char string[] = "This is a test.";
//    int len = strlen(string) + 1;
//    i2c_write_flash(0x200, string, len);
//    while(i2c_write_flash_complete() != 1);

    // Debugging LCD, can be safely removed
    LCD_Init();
    LCD_Clear(WHITE);
    LCD_DisplayUI();
//    LCD_Clear(BLACK);
//    LCD_DrawString(0,   0, WHITE, BLACK, "Roll For Initiative!", 16, 0);
//    LCD_DrawString(0,  20, WHITE, BLACK, "Roll For Initiative!", 16, 0);
//    LCD_DrawString(0,  40, WHITE, BLACK, "Roll For Initiative!", 16, 0);
//    LCD_DrawString(0,  60, WHITE, BLACK, "Roll For Initiative!", 16, 0);
//    LCD_DrawString(0,  80, WHITE, BLACK, "Roll For Initiative!", 16, 0);
//    LCD_DrawString(0, 100, WHITE, BLACK, "Roll For Initiative!", 16, 0);
//    LCD_DrawString(0, 120, WHITE, BLACK, "Roll For Initiative!", 16, 0);
//    LCD_DrawString(0, 140, WHITE, BLACK, "Roll For Initiative!", 16, 0);
//    LCD_DrawString(0, 160, WHITE, BLACK, "Roll For Initiative!", 16, 0);
//    LCD_DrawString(0, 180, WHITE, BLACK, "Roll For Initiative!", 16, 0);
//    LCD_DrawString(0, 200, WHITE, BLACK, "Roll For Initiative!", 16, 0);
//    LCD_DrawString(0, 220, WHITE, BLACK, "Roll For Initiative!", 16, 0);
//    LCD_DrawString(0, 240, WHITE, BLACK, "Roll For Initiative!", 16, 0);
//    LCD_DrawString(0, 260, WHITE, BLACK, "Roll For Initiative!", 16, 0);
//    LCD_DrawString(0, 280, WHITE, BLACK, "Roll For Initiative!", 16, 0);
//    LCD_DrawString(0, 300, WHITE, BLACK, "More graphics coming soon!", 16, 0);

    // Debugging DAC, can be safely removed
    set_freq_a(261.626);
    set_freq_b(329.63);

    for(;;) {
        asm volatile ("wfi");
    }
}
