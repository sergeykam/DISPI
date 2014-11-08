################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../include/API/DIS_API/dis_API.c 

OBJS += \
./include/API/DIS_API/dis_API.o 

C_DEPS += \
./include/API/DIS_API/dis_API.d 


# Each subdirectory must supply rules for building sources it contributes
include/API/DIS_API/%.o: ../include/API/DIS_API/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -I"/home/andrey/Program Files/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/disinclude/mysql" -include"/home/andrey/workspace/DISraspi/include/API/BCM2835_API/bcm2835.h" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


