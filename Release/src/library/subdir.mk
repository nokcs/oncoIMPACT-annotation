################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/library/calculator.cpp \
../src/library/input_output.cpp 

OBJS += \
./src/library/calculator.o \
./src/library/input_output.o 

CPP_DEPS += \
./src/library/calculator.d \
./src/library/input_output.d 


# Each subdirectory must supply rules for building sources it contributes
src/library/%.o: ../src/library/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -D__NO_INLINE__ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


