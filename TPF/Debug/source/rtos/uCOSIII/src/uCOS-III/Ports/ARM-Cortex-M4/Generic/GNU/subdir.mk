################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu_c.c 

S_UPPER_SRCS += \
../source/rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu_a.S 

C_DEPS += \
./source/rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu_c.d 

OBJS += \
./source/rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu_a.o \
./source/rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu_c.o 


# Each subdirectory must supply rules for building sources it contributes
source/rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/%.o: ../source/rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/%.S source/rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU Assembler'
	arm-none-eabi-gcc -c -x assembler-with-cpp -I../source -I../ -I../SDK -g3 -gdwarf-4 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

source/rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/%.o: ../source/rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/%.c source/rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\ucosiii_config" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uC-CPU\ARM-Cortex-M4\GNU" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uC-CPU" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uC-LIB" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uCOS-III\Ports\ARM-Cortex-M4\Generic\GNU" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uCOS-III\Source" -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mgeneral-regs-only -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-rtos-2f-uCOSIII-2f-src-2f-uCOS-2d-III-2f-Ports-2f-ARM-2d-Cortex-2d-M4-2f-Generic-2f-GNU

clean-source-2f-rtos-2f-uCOSIII-2f-src-2f-uCOS-2d-III-2f-Ports-2f-ARM-2d-Cortex-2d-M4-2f-Generic-2f-GNU:
	-$(RM) ./source/rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu_a.o ./source/rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu_c.d ./source/rtos/uCOSIII/src/uCOS-III/Ports/ARM-Cortex-M4/Generic/GNU/os_cpu_c.o

.PHONY: clean-source-2f-rtos-2f-uCOSIII-2f-src-2f-uCOS-2d-III-2f-Ports-2f-ARM-2d-Cortex-2d-M4-2f-Generic-2f-GNU

