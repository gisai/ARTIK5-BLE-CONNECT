################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SEND-ARTIK5.c 

OBJS += \
./SEND-ARTIK5.o 

C_DEPS += \
./SEND-ARTIK5.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -IC:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot/usr/include/ -IC:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot/usr/include/artik/base -IC:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot/usr/include/artik/base/platform -IC:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot/usr/include/artik/bluetooth -IC:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot/usr/include/artik/connectivity -IC:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot/usr/include/artik/lwm2m -IC:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot/usr/include/artik/media -IC:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot/usr/include/artik/mqtt -IC:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot/usr/include/artik/sensor -IC:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot/usr/include/artik/sensor/platform -IC:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot/usr/include/artik/systemio -IC:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot/usr/include/artik/wifi -IC:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot/usr/include/artik/zigbee -O0 -g3 -Wall -c -fmessage-length=0 --sysroot=C:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


