################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../include/dispi.c \
../include/main.c 

OBJS += \
./include/dispi.o \
./include/main.o 

C_DEPS += \
./include/dispi.d \
./include/main.d 


# Each subdirectory must supply rules for building sources it contributes
include/%.o: ../include/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I"/home/andrey/Program Files/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/disinclude/mysql" -include"/home/andrey/workspace/DISraspi/include/API/BCM2835_API/bcm2835.h" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


