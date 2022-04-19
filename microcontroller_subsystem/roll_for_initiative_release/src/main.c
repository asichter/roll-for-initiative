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
#include <stdlib.h>
#include "lcd.h"
#include <math.h>
#include "ui.h"

// GLOBAL VARIABLES
// Keypad Debouncing
uint8_t history[16];
uint8_t offset;

// Roll Values
uint8_t roll_table[64];
uint8_t roll_table_ind = 0;
int8_t advantage = 0;
int8_t disadvantage = -1;

int roll_sum = 0;
int prev_roll_sum = 0;
int prev_roll = 0;
int prev_total = 0;

// Configurables
//const static char KEY_ARRAY[] = "123+456-789AC0=D";
const static char KEY_ARRAY[] = "123A456=789MC0+R";
int mod = 0;
int prev_mod = 0;
int sub_mod = 0;
int prev_sub_mod = 0;
uint8_t c_count = 0;
uint8_t DADV = 0;
uint8_t prev_DADV = 0;
uint8_t PKG = 0;
uint8_t SIGN = 0;
uint8_t prev_SIGN = 0;
uint8_t DBG = 0;

// Font Types
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


/*********************************************************************
** SETUP FUNCTIONS
**
**  void setup_ports()              - Set pin i/o config
**  void setup_usart5()             - USART5:   DEBUG
**  void setup_usart1()             - USART1:   Raspberry Pi Comm
**  void setup_exti()               - EXTI:     Infrared Sensor
**  void setup_spi2()               - SPI2:     LCD Screen
**  void setup_i2c1()               - I2C1:     EEPROM
**  void setup_dac()                - DAC:      Speaker Audio
**
**  void setup_tim6()               - Timer 6:  DAC Speaker Audio
**  void setup_tim7()               - Timer 7:  Keypad
**  void setup_tim17()              - Timer 17: LCD SCreen
**
*********************************************************************/

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

// Setup USART 5 for DEBUG
void setup_usart5() {
    RCC -> APB1ENR |= 1 << 20;

    USART5 -> CR2 &= ~(0x00003000);
    USART5 -> BRR = 0x1a1;
    USART5 -> CR1 &= ~(0x10009401);
    USART5 -> CR1 |= 0x2c;
    USART5 -> CR1 |= 0x1;

    while(((USART5 -> CR1 >> 2) & 3) != 3);
    NVIC -> ISER[0] = 1 << 29;
}

void disable_usart5() {
    RCC -> APB1ENR &= ~(1 << 20);
    NVIC -> ICER[0] = 1 << 29;
}

// SETUP USART 1 for Raspberry Pi Communication
void setup_usart1(){
   RCC -> APB2ENR |= 1 << 14;

   USART1 -> CR2 &= ~(00003000);
   USART1 -> BRR = 0x1a1;
   USART1 -> CR1 &= ~(0x10009401);
   USART1 -> CR1 |= 0x2c;
   USART1 -> CR1 |= 0x1;

   while(((USART1 -> CR1 >> 2) & 3) != 3);
   NVIC -> ISER[0] = 1 << 27;
}

// Setup EXTI for Infrared Sensor
void setup_exti() {
    RCC -> APB2ENR |= 1;

    EXTI -> FTSR |= 1 << 1;
    EXTI -> IMR |= 1 << 1;

    NVIC -> ISER[0] = 1 << 5;
}

// Setup SPI2 for LCD Screen
void setup_spi2() {
    RCC -> APB1ENR |= 1 << 14;

    SPI2 -> CR2 = 0x070c;
    SPI2 -> CR1 = 0xc004;
    SPI2 -> CR1 |= 1 << 6;
}

// Setup Timer 7 for LCD Screen
void setup_tim7() {
    RCC -> APB1ENR |= 1 << 5;

    TIM7 -> PSC = 4799;
    TIM7 -> ARR = 9;
    TIM7 -> DIER |= 1;
    TIM7 -> CR1 |= 1;

    NVIC -> ISER[0] = 1 << 18;
}

// Setup I2C1 for EEPROM
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

// Setup DAC for Audio Output
void setup_dac() {
    RCC -> APB1ENR |= 1 << 29;

    DAC -> CR |= 1<<2;
    DAC -> CR |= 0x38;
    DAC -> CR |= 1;
}

// Setup Timer 6 for Audio
void setup_tim6() {
    RCC -> APB1ENR |= 1 << 4;

    TIM6 -> PSC = 599;
    TIM6 -> ARR = 3;
    TIM6 -> DIER |= 1;
    TIM6 -> CR1 |= 1;

    NVIC -> ISER[0] = 1 << 17;
}

// Setup Timer 17 for LCD Screen
void setup_tim17() {
    RCC -> APB2ENR |= 1 << 18;

    TIM17 -> PSC = 19999;
    TIM17 -> ARR = 39;
    TIM17 -> DIER |= 1;
    TIM17 -> CR1 |= 1;

    NVIC_SetPriority(22, 192);
    NVIC -> ISER[0] |= 1 << 22;
}


/*********************************************************************
** REGULAR FUNCTIONS
**
**      - UART
**      - Keypad
**      - User Interface (UI)
**      - EEPROM
**      - DAC
**      - Interrupt Service Routines (ISRS)
**
*********************************************************************/

/*********************************************************************
** UART
**
** int better_putchar(int data)
** int interrupt_getchar()
** int __io_putchar(int ch)
** int __io_getchar(void)
**
*********************************************************************/

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

/*********************************************************************
** Keypad
**
** void set_row()
** int get_cols()
** void update_hist(int cols)
**
*********************************************************************/

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


/*********************************************************************
** User Interface
**
** void toggle_adv()
** void toggle_sign()
** void clear()
** void modifier(int num)
** void add_mod()
** void display_settings()
** void handle_debug()
** void handle_keypress(int cols)
**
*********************************************************************/

//===========================================================================
// Change advantage setting
//      0 = NONE
//      1 = adv
//      2 = dadv
//===========================================================================
void toggle_adv() {
    DADV += 1;
    if (DADV > 2)
        DADV=0;
}

void toggle_sign() {
    SIGN ^= 1;
}

void clear() {
    if (c_count == 0) {
        c_count++;
        sub_mod = 0;
        SIGN = 0;
    } else if (c_count == 1) {
        c_count++;
        sub_mod = 0;
        mod = 0;
    } else if (c_count > 1) {
        c_count = 0;
        DADV = 0;
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
    if (SIGN == 1) {
        mod += sub_mod;
    }
    else if (SIGN == 2) {
        mod -= sub_mod;
    }
    else
        return;

    sub_mod = 0;
}

void display_settings() {
    if (DBG == 0) {
        return;
    }
    printf("Advantage: %2d\n", DADV);
    printf("Mod: %2d\n", mod);
    printf("Submod: %2d\n", sub_mod);
    printf("Sign: %2d\n", SIGN);
    printf("Roll Sum: %2d\n\n", roll_sum);
}

void handle_debug() {
    if (history[0] == 0xff && history[7] == 0xff && history[14] == 0x3f && DBG == 0) {
        DBG = 1;
        setup_usart5();
        printf("Debug Mode enabled.\n");
    }
    else if (history[0] == 0xff && history[7] == 0xff && history[14] == 0x3f) {
        DBG = 0;
        disable_usart5();
    }
}

void handle_keypress(int cols) {
    int row = offset & 3;
    for(int i = 0; i < 4; i++) {
        int id = 4*row+i;
        if(history[id] == 0x3f) {
            char key = KEY_ARRAY[id];
            if (DBG == 1)
                printf("Button %2c was pushed.\n", key);

            switch(key) {
            case 'A' : toggle_adv();
                       break;
            case 'C' : clear();
                       break;
            case '+' : if (PKG==1) {
                           add_mod();
                           SIGN = 0;
                           PKG = 0;
                       } else {
                           toggle_sign();
                       }
                       break;
            case '=' : mod = SIGN == 0 ? mod + sub_mod : mod - sub_mod;
                       sub_mod = 0;
                       break;
            case 'M' : // Mute speaker
                       break;
            case 'R' : // Reroll
                       break;
            default:   PKG = 1;
                       modifier(key - '0');
                       break;
            }

            if (key != 'C')
                c_count = 0;
            if (DBG != 0)
                display_settings();
            break;
        }
    }
}


/*********************************************************************
** EEPROM
**
** void i2c_start(uint32_t address, uint8_t size, uint8_t direction)
** void i2c_stop()
** int i2c_checknack()
** void i2c_waitidle()
** int8_t i2c_senddata(uint8_t address, void* p_data, uint8_t size)
** uint8_t i2c_recvdata(uint8_t address, uint8_t* data, uint8_t size)
** void i2c_write_flash(uint16_t loc, const char* data, uint8_t len)
** void i2c_read_flash(uint16_t loc, char data[], uint8_t len)
** int i2c_write_flash_complete()
**
*********************************************************************/

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

/*********************************************************************
** Digital to Analog Converter (DAC)
**
** void enable_speaker()
** void disable_speaker()
** void write_dac(int sample)
** void init_wavetable()
** void set_freq_a(float f)
** void set_freq_b(float f)
** void set_freq_c(float f)
** void set_freq_d(float f)
**
*********************************************************************/

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

/*********************************************************************
** Interrupt Service Routines
**
** void USART1_IRQHandler()              - Raspberry Pi/Micro Communication
** void USART3_4_5_6_7_8_IRQHandler()    - Debug
** void EXTI0_1_IRQHandler()             - Infrared Detection, Triggers USART1
** void TIM7_IRQHandler()                - Keypad Handling
** void TIM6_DAC_IRQHandler()            - DAC Audio
** void TIM17_IRQHandler()               - Update LCD
**
*********************************************************************/

// Raspberry Pi / Micro Communication
void USART1_IRQHandler() {
    if(USART1 -> ISR & USART_ISR_RXNE)
        USART1 -> ICR |= USART_ISR_RXNE;
    printchr(0,0,BLUE, "420", 2, 0);
    uint8_t roll = USART1 -> RDR;
    roll_table[roll_table_ind++] = roll;
    if(roll != 255) {
        if(roll > advantage)
            advantage = roll;
        if(roll < disadvantage)
            disadvantage = roll;
        roll_sum += roll;
    } else {
        roll_table_ind = 0;
        if (DBG != 0) {
            for(int i = 0; roll_table[i] != 255; i++) {
                printf("%d\n", roll_table[i]);
            }
        }
        advantage = 0;
        disadvantage = 255;
        roll_sum = 0;
    }
}

// Debug Handler
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

// Infrared Sensor Detection
void EXTI0_1_IRQHandler() {
    EXTI -> PR = 1 << 1;
    USART1 -> TDR = 255;
    if (DBG != 0) {
        printf("Obstruction detected.\n");
    }
}

// Keypad Handler
void TIM7_IRQHandler() {
    TIM7 -> SR &= ~(1);
    int cols = get_cols();
    update_hist(cols);
    offset = (offset + 1) & 0x7;
    set_row();
    handle_keypress(cols);
    handle_debug();
}

// Audio Handler
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

// LCD Handler
void TIM17_IRQHandler() {
    int roll;
    int total;

    if (DADV == 1) {
        roll = advantage;
    }
    else if (DADV == 2) {
        roll = disadvantage;
    }
    else {
        roll = roll_sum;
    }

    total = roll + mod;

    if (roll != prev_roll) {
        LCD_DrawRoll(roll);
    }

    if (total != prev_total) {
        LCD_DrawTotal(total);
    }

    if (DADV != prev_DADV) {
        LCD_DrawDAdv();
    }

    if (mod != prev_mod) {
        LCD_DrawMod();
    }

    if (sub_mod != prev_sub_mod || SIGN != prev_SIGN) {
        LCD_DrawSubMod();
    }

    prev_roll = roll;
    prev_total = total;
    prev_DADV = DADV;
    prev_mod = mod;
    prev_sub_mod = sub_mod;
    prev_SIGN = SIGN;
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
    //setup_usart1();
    setup_exti();
    setup_spi2();
    //setup_i2c1();
    setup_dac();
    init_wavetable();
    setup_tim6();
    //disable_speaker();
    setup_tim7();

    printf("Roll For Initiative.\n");
    printf("Press 1 -> B -> 0 to enter Debug Mode.\n\n");

    // Debugging LCD, can be safely removed
    LCD_Init();
    LCD_InitUI();
    setup_tim17();

    /*printchr(0,0,BLACK, "420", 0, 8);
    printchr(0,0,RED, "69", 1, 0);
    printchr(0,0,BLUE, "666", 2, 0);
    printchr(0,0,LGRAYBLUE, "A", 3, 0);
    printmnsign(SIGN);*/
    //LCD_DisplayUIBox(1);
    disable_usart5();

    for(;;) {
        asm volatile ("wfi");
    }
}
