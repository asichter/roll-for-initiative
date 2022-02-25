################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/fifo.c \
../src/lcd.c \
../src/lrgfnt.c \
../src/main.c \
../src/syscalls.c \
../src/system_stm32f0xx.c \
../src/tty.c \
../src/ui.c 

OBJS += \
./src/fifo.o \
./src/lcd.o \
./src/lrgfnt.o \
./src/main.o \
./src/syscalls.o \
./src/system_stm32f0xx.o \
./src/tty.o \
./src/ui.o 

C_DEPS += \
./src/fifo.d \
./src/lcd.d \
./src/lrgfnt.d \
./src/main.d \
./src/syscalls.d \
./src/system_stm32f0xx.d \
./src/tty.d \
./src/ui.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F0 -DSTM32F091RCTx -DDEBUG -DSTM32F091 -DUSE_STDPERIPH_DRIVER -I"C:/Users/Alexandra/workspace/roll-for-initiative/StdPeriph_Driver/inc" -I"C:/Users/Alexandra/workspace/roll-for-initiative/inc" -I"C:/Users/Alexandra/workspace/roll-for-initiative/CMSIS/device" -I"C:/Users/Alexandra/workspace/roll-for-initiative/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


