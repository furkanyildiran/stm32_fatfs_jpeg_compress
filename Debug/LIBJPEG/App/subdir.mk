################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LIBJPEG/App/libjpeg.c 

OBJS += \
./LIBJPEG/App/libjpeg.o 

C_DEPS += \
./LIBJPEG/App/libjpeg.d 


# Each subdirectory must supply rules for building sources it contributes
LIBJPEG/App/%.o LIBJPEG/App/%.su LIBJPEG/App/%.cyclo: ../LIBJPEG/App/%.c LIBJPEG/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../LIBJPEG/App -I../LIBJPEG/Target -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/Third_Party/LibJPEG/include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-LIBJPEG-2f-App

clean-LIBJPEG-2f-App:
	-$(RM) ./LIBJPEG/App/libjpeg.cyclo ./LIBJPEG/App/libjpeg.d ./LIBJPEG/App/libjpeg.o ./LIBJPEG/App/libjpeg.su

.PHONY: clean-LIBJPEG-2f-App

