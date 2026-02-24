################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/rtos/uCOSIII/BSP/cpu_bsp.c 

C_DEPS += \
./source/rtos/uCOSIII/BSP/cpu_bsp.d 

OBJS += \
./source/rtos/uCOSIII/BSP/cpu_bsp.o 


# Each subdirectory must supply rules for building sources it contributes
source/rtos/uCOSIII/BSP/%.o: ../source/rtos/uCOSIII/BSP/%.c source/rtos/uCOSIII/BSP/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\ucosiii_config" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uC-CPU\ARM-Cortex-M4\GNU" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uC-CPU" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uC-LIB" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uCOS-III\Ports\ARM-Cortex-M4\Generic\GNU" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uCOS-III\Source" -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mgeneral-regs-only -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-rtos-2f-uCOSIII-2f-BSP

clean-source-2f-rtos-2f-uCOSIII-2f-BSP:
	-$(RM) ./source/rtos/uCOSIII/BSP/cpu_bsp.d ./source/rtos/uCOSIII/BSP/cpu_bsp.o

.PHONY: clean-source-2f-rtos-2f-uCOSIII-2f-BSP

