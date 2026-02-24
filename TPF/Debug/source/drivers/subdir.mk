################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/drivers/CRead.c \
../source/drivers/PWM.c \
../source/drivers/building.c \
../source/drivers/display_7s.c \
../source/drivers/dma.c \
../source/drivers/encoder.c \
../source/drivers/gpio.c \
../source/drivers/matrix.c 

C_DEPS += \
./source/drivers/CRead.d \
./source/drivers/PWM.d \
./source/drivers/building.d \
./source/drivers/display_7s.d \
./source/drivers/dma.d \
./source/drivers/encoder.d \
./source/drivers/gpio.d \
./source/drivers/matrix.d 

OBJS += \
./source/drivers/CRead.o \
./source/drivers/PWM.o \
./source/drivers/building.o \
./source/drivers/display_7s.o \
./source/drivers/dma.o \
./source/drivers/encoder.o \
./source/drivers/gpio.o \
./source/drivers/matrix.o 


# Each subdirectory must supply rules for building sources it contributes
source/drivers/%.o: ../source/drivers/%.c source/drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\ucosiii_config" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uC-CPU\ARM-Cortex-M4\GNU" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uC-CPU" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uC-LIB" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uCOS-III\Ports\ARM-Cortex-M4\Generic\GNU" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uCOS-III\Source" -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mgeneral-regs-only -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-drivers

clean-source-2f-drivers:
	-$(RM) ./source/drivers/CRead.d ./source/drivers/CRead.o ./source/drivers/PWM.d ./source/drivers/PWM.o ./source/drivers/building.d ./source/drivers/building.o ./source/drivers/display_7s.d ./source/drivers/display_7s.o ./source/drivers/dma.d ./source/drivers/dma.o ./source/drivers/encoder.d ./source/drivers/encoder.o ./source/drivers/gpio.d ./source/drivers/gpio.o ./source/drivers/matrix.d ./source/drivers/matrix.o

.PHONY: clean-source-2f-drivers

