################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SDK/CMSIS/system_MK64F12.c 

C_DEPS += \
./SDK/CMSIS/system_MK64F12.d 

OBJS += \
./SDK/CMSIS/system_MK64F12.o 


# Each subdirectory must supply rules for building sources it contributes
SDK/CMSIS/%.o: ../SDK/CMSIS/%.c SDK/CMSIS/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\ucosiii_config" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uC-CPU\ARM-Cortex-M4\GNU" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uC-CPU" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uC-LIB" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uCOS-III\Ports\ARM-Cortex-M4\Generic\GNU" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uCOS-III\Source" -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mgeneral-regs-only -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-SDK-2f-CMSIS

clean-SDK-2f-CMSIS:
	-$(RM) ./SDK/CMSIS/system_MK64F12.d ./SDK/CMSIS/system_MK64F12.o

.PHONY: clean-SDK-2f-CMSIS

