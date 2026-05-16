################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Utilities/Fonts/font12.c \
../Core/Utilities/Fonts/font16.c \
../Core/Utilities/Fonts/font20.c \
../Core/Utilities/Fonts/font24.c \
../Core/Utilities/Fonts/font8.c 

OBJS += \
./Core/Utilities/Fonts/font12.o \
./Core/Utilities/Fonts/font16.o \
./Core/Utilities/Fonts/font20.o \
./Core/Utilities/Fonts/font24.o \
./Core/Utilities/Fonts/font8.o 

C_DEPS += \
./Core/Utilities/Fonts/font12.d \
./Core/Utilities/Fonts/font16.d \
./Core/Utilities/Fonts/font20.d \
./Core/Utilities/Fonts/font24.d \
./Core/Utilities/Fonts/font8.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Utilities/Fonts/%.o Core/Utilities/Fonts/%.su Core/Utilities/Fonts/%.cyclo: ../Core/Utilities/Fonts/%.c Core/Utilities/Fonts/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Core/Utilities/Fonts -I../Drivers/BSP/STM32F429I-Discovery -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Utilities-2f-Fonts

clean-Core-2f-Utilities-2f-Fonts:
	-$(RM) ./Core/Utilities/Fonts/font12.cyclo ./Core/Utilities/Fonts/font12.d ./Core/Utilities/Fonts/font12.o ./Core/Utilities/Fonts/font12.su ./Core/Utilities/Fonts/font16.cyclo ./Core/Utilities/Fonts/font16.d ./Core/Utilities/Fonts/font16.o ./Core/Utilities/Fonts/font16.su ./Core/Utilities/Fonts/font20.cyclo ./Core/Utilities/Fonts/font20.d ./Core/Utilities/Fonts/font20.o ./Core/Utilities/Fonts/font20.su ./Core/Utilities/Fonts/font24.cyclo ./Core/Utilities/Fonts/font24.d ./Core/Utilities/Fonts/font24.o ./Core/Utilities/Fonts/font24.su ./Core/Utilities/Fonts/font8.cyclo ./Core/Utilities/Fonts/font8.d ./Core/Utilities/Fonts/font8.o ./Core/Utilities/Fonts/font8.su

.PHONY: clean-Core-2f-Utilities-2f-Fonts

