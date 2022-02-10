/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f4xx.h"
#include "fifo.h"
#include "tty.h"
#include <stdio.h>
#include <string.h>

char history[16];
char offset;

uint8_t advantage = 0;
uint8_t disadvantage = 0;
uint8_t sign;
uint16_t modifier = 0;


// DEBUG
/******************************************************************************
To verify the clock speed, uncomment the two functions below and enable Timer 2

void setup_tim2() {
    RCC -> AHB1ENR |= 1 << 2;
    RCC -> APB1ENR |= 1;

    GPIOC -> MODER &= ~(0x3);
    GPIOC -> MODER |= 0x1;

    TIM2 -> PSC = 99;
    TIM2 -> ARR = 167999;
    TIM2 -> DIER |= 1;
    TIM2 -> CR1 |= 1;
    NVIC -> ISER[0] |= 1 << 28;
}

void TIM2_IRQHandler() {
    TIM2 -> SR &= ~(1);
    GPIOC -> ODR ^= 1;
}

******************************************************************************/


// SETUP FUNCTIONS
void setup_ports() {
    RCC -> AHB1ENR |= 0xf;

    GPIOA -> MODER &= ~(0xcffc);
    GPIOA -> MODER |= 0x8a50;
    GPIOA -> PUPDR &= ~(0xc);
    GPIOA -> PUPDR |= 0x4;
    GPIOA -> AFR[0] &= ~(0xf0ff0000);
    GPIOA -> AFR[0] |= 0x50550000;

    GPIOB -> MODER &= ~(0xff);
    GPIOB -> MODER |= 0x55;

    GPIOC -> MODER &= ~(0x300ffff);
    GPIOC -> MODER |= 0x2000000;
    GPIOC -> PUPDR &= ~(0xff00);
    GPIOC -> PUPDR |= 0xaa00;
    GPIOC -> AFR[1] &= ~(0xf0000);
    GPIOC -> AFR[1] |= 0x80000;

    GPIOD -> MODER &= ~(0x30);
    GPIOD -> MODER |= 0x20;
    GPIOD -> AFR[0] &= ~(0xf00);
    GPIOD -> AFR[0] |= 0x800;
}

void setup_exti1() {
    RCC -> APB2ENR |= 1 << 14;

    EXTI -> FTSR |= 1 << 1;
    EXTI -> IMR |= 1 << 1;

    NVIC -> ISER[0] |= 1 << 7;
}

void setup_tim7() {
    RCC -> APB1ENR |= 1 << 5;
    TIM7 -> PSC = 4199;
    TIM7 -> ARR = 9;
    TIM7 -> DIER |= 1;
    TIM7 -> CR1 |= 1;
    NVIC -> ISER[1] |= 1 << 23;
}

void setup_uart5() {
	RCC -> APB1ENR |= 1 << 20;

	UART5 -> CR2  &= ~(0x3000);
	UART5 -> BRR = 0x16d;
	UART5 -> CR1 &= ~(0x97);
	UART5 -> CR1 |= 0x2c;
	UART5 -> CR1 |= 1 << 13;

	while(((UART5 -> CR1 >> 2) & 3) != 3);
	NVIC -> ISER[1] |= 1 << 21;
}

void setup_spi1() {
    RCC -> APB2ENR |= 1 << 12;

    SPI1 -> CR2 |= 0x4;
    SPI1 -> CR1 = 0xca3c;
    SPI1 -> CR1 |= 1 << 6;
}


// REGULAR FUNCTIONS
// Keyboard
void set_row() {
	GPIOB -> BSRRH = 0xf;
    GPIOB -> BSRRL = (1 << (offset & 3));
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

void advantage_pressed() {
    advantage ^= 0x1;
    disadvantage = 0;

    // DEBUGGING
    if(advantage == 1) {
        printf("Rolling with advantage.\n");
    }
    else {
        printf("Rolling without advantage.\n");
    }
}

void disadvantage_pressed() {
    disadvantage ^= 0x1;
    advantage = 0;

    // DEBUGGING
    if(disadvantage == 1) {
        printf("Rolling with disadvantage.\n");
    }
    else {
        printf("Rolling without disadvantage.\n");
    }
}

void number_pressed(uint8_t number) {
    modifier = modifier * 10 + number;

    // DEBUGGING
    printf("Modifier set to %d.\n", modifier);
}

void sign_pressed(uint8_t s) {
    sign = s;

    // DEBUGGING
    char si = s == 0 ? '+' : '-';
    printf("Sign set to %c.\n", si);
}

void clear_pressed() {
    modifier = 0;

    // DEBUGGING
    printf("Modifier set to %d.\n", modifier);
}

void handle_keypress(int cols) {
    int row = offset & 3;
    for(int i = 0; i < 4; i++) {
        if(history[4 * row + i] == 0x3f) {
            switch(4 * row + i) {
            case 0:  number_pressed(1);
                     break;
            case 1:  number_pressed(2);
                     break;
            case 2:  number_pressed(3);
                     break;
            case 3:  sign_pressed(0);
                     break;
            case 4:  number_pressed(4);
                     break;
            case 5:  number_pressed(5);
                     break;
            case 6:  number_pressed(6);
                     break;
            case 7:  sign_pressed(1);
                     break;
            case 8:  number_pressed(7);
                     break;
            case 9:  number_pressed(8);
                     break;
            case 10: number_pressed(9);
                     break;
            case 11: advantage_pressed();
                     break;
            case 12: clear_pressed();
                     break;
            case 13: number_pressed(0);
                     break;
            case 14: printf("Pressed \'=\' button.\n");
                     break;
            case 15: disadvantage_pressed();
                     break;
            }
            break;
        }
    }
}

// UART
int better_putchar(int data) {
    if(data == '\n') {
        while(((UART5 -> SR >> 7) & 1) == 0);
        UART5 -> DR = '\r';
    }
    while(((UART5 -> SR >> 7) & 1) == 0);
    UART5 -> DR = data;
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


// ISRs
void TIM7_IRQHandler() {
    TIM7 -> SR &= ~(1);
    int cols = get_cols();
    update_hist(cols);
    offset = (offset + 1) & 0x7;
    set_row();
    handle_keypress(cols);
}

void UART5_IRQHandler() {
	while(!(UART5 -> SR & 0x8));
	char character = UART5 -> DR;
	if(fifo_full(&input_fifo)) {
	    return;
	}
	insert_echo_char(character);
}

void EXTI1_IRQHandler() {
    EXTI -> PR = 1 << 1;
    printf("Obstruction detected.\n");
}


// MAIN
int main(void) {
	setbuf(stdin, 0);
	setbuf(stdout, 0);
	setbuf(stderr, 0);

	setup_ports();
	setup_exti1();
	setup_uart5();
	setup_spi1();
	setup_tim7();

	for(;;) {
	    asm volatile ("wfi");
	}
}
