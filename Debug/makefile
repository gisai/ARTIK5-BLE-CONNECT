################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: SEND-ARTIK5

# Tool invocations
SEND-ARTIK5: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	arm-linux-gnueabihf-gcc -LC:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot/usr/lib -LC:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot/lib --sysroot=C:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot -Wl,--rpath-link C:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot/lib/arm-linux-gnueabihf -Wl,--rpath-link C:/ARTIK/SDK/A5_ubuntu/v1.6/sysroot/usr/lib/arm-linux-gnueabihf -o "SEND-ARTIK5" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) SEND-ARTIK5
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
