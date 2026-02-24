################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_cfg_app.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_core.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_dbg.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_flag.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_int.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_mem.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_msg.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_mutex.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_pend_multi.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_prio.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_q.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_sem.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_stat.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_task.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_tick.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_time.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_tmr.c \
../source/rtos/uCOSIII/src/uCOS-III/Source/os_var.c 

C_DEPS += \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_cfg_app.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_core.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_dbg.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_flag.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_int.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_mem.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_msg.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_mutex.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_pend_multi.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_prio.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_q.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_sem.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_stat.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_task.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_tick.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_time.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_tmr.d \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_var.d 

OBJS += \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_cfg_app.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_core.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_dbg.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_flag.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_int.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_mem.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_msg.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_mutex.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_pend_multi.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_prio.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_q.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_sem.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_stat.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_task.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_tick.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_time.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_tmr.o \
./source/rtos/uCOSIII/src/uCOS-III/Source/os_var.o 


# Each subdirectory must supply rules for building sources it contributes
source/rtos/uCOSIII/src/uCOS-III/Source/%.o: ../source/rtos/uCOSIII/src/uCOS-III/Source/%.c source/rtos/uCOSIII/src/uCOS-III/Source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\ucosiii_config" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uC-CPU\ARM-Cortex-M4\GNU" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uC-CPU" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uC-LIB" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uCOS-III\Ports\ARM-Cortex-M4\Generic\GNU" -I"C:\Users\pedro\OneDrive\Escritorio\ITBA\Sistemas Embebidos\TPs\TPF\TPF\source\rtos\uCOSIII\src\uCOS-III\Source" -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mgeneral-regs-only -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-rtos-2f-uCOSIII-2f-src-2f-uCOS-2d-III-2f-Source

clean-source-2f-rtos-2f-uCOSIII-2f-src-2f-uCOS-2d-III-2f-Source:
	-$(RM) ./source/rtos/uCOSIII/src/uCOS-III/Source/os_cfg_app.d ./source/rtos/uCOSIII/src/uCOS-III/Source/os_cfg_app.o ./source/rtos/uCOSIII/src/uCOS-III/Source/os_core.d ./source/rtos/uCOSIII/src/uCOS-III/Source/os_core.o ./source/rtos/uCOSIII/src/uCOS-III/Source/os_dbg.d ./source/rtos/uCOSIII/src/uCOS-III/Source/os_dbg.o ./source/rtos/uCOSIII/src/uCOS-III/Source/os_flag.d ./source/rtos/uCOSIII/src/uCOS-III/Source/os_flag.o ./source/rtos/uCOSIII/src/uCOS-III/Source/os_int.d ./source/rtos/uCOSIII/src/uCOS-III/Source/os_int.o ./source/rtos/uCOSIII/src/uCOS-III/Source/os_mem.d ./source/rtos/uCOSIII/src/uCOS-III/Source/os_mem.o ./source/rtos/uCOSIII/src/uCOS-III/Source/os_msg.d ./source/rtos/uCOSIII/src/uCOS-III/Source/os_msg.o ./source/rtos/uCOSIII/src/uCOS-III/Source/os_mutex.d ./source/rtos/uCOSIII/src/uCOS-III/Source/os_mutex.o ./source/rtos/uCOSIII/src/uCOS-III/Source/os_pend_multi.d ./source/rtos/uCOSIII/src/uCOS-III/Source/os_pend_multi.o ./source/rtos/uCOSIII/src/uCOS-III/Source/os_prio.d ./source/rtos/uCOSIII/src/uCOS-III/Source/os_prio.o ./source/rtos/uCOSIII/src/uCOS-III/Source/os_q.d ./source/rtos/uCOSIII/src/uCOS-III/Source/os_q.o ./source/rtos/uCOSIII/src/uCOS-III/Source/os_sem.d ./source/rtos/uCOSIII/src/uCOS-III/Source/os_sem.o ./source/rtos/uCOSIII/src/uCOS-III/Source/os_stat.d ./source/rtos/uCOSIII/src/uCOS-III/Source/os_stat.o ./source/rtos/uCOSIII/src/uCOS-III/Source/os_task.d ./source/rtos/uCOSIII/src/uCOS-III/Source/os_task.o ./source/rtos/uCOSIII/src/uCOS-III/Source/os_tick.d ./source/rtos/uCOSIII/src/uCOS-III/Source/os_tick.o ./source/rtos/uCOSIII/src/uCOS-III/Source/os_time.d ./source/rtos/uCOSIII/src/uCOS-III/Source/os_time.o ./source/rtos/uCOSIII/src/uCOS-III/Source/os_tmr.d ./source/rtos/uCOSIII/src/uCOS-III/Source/os_tmr.o ./source/rtos/uCOSIII/src/uCOS-III/Source/os_var.d ./source/rtos/uCOSIII/src/uCOS-III/Source/os_var.o

.PHONY: clean-source-2f-rtos-2f-uCOSIII-2f-src-2f-uCOS-2d-III-2f-Source

