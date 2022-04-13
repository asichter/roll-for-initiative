################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/fifo.c \
../src/lcd.c \
../src/main.c \
../src/syscalls.c \
../src/system_stm32f4xx.c \
../src/tty.c 

OBJS += \
./src/fifo.o \
./src/lcd.o \
./src/main.o \
./src/syscalls.o \
./src/system_stm32f4xx.o \
./src/tty.o 

C_DEPS += \
./src/fifo.d \
./src/lcd.d \
./src/main.d \
./src/syscalls.d \
./src/system_stm32f4xx.d \
./src/tty.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32F4 -DSTM32F407VGTx -DDEBUG -DSTM32F40XX -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -I"C:/Users/Alex Sichterman/workspace/roll-for-initiative/StdPeriph_Driver/inc" -I"C:/Users/Alex Sichterman/workspace/roll-for-initiative/inc" -I"C:/Users/Alex Sichterman/workspace/roll-for-initiative/CMSIS/device" -I"C:/Users/Alex Sichterman/workspace/roll-for-initiative/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


