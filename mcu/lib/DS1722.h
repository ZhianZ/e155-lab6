// DS1722.h
// name: Zhian Zhou
// email: zzhou@g.hmc.edu
// date: 10/24/2024
// TODO: <SHORT DESCRIPTION OF WHAT THIS FILE DOES>

#ifndef DS1722_H
#define DS1722_H

#include <stdio.h>
#include "stm32l432xx.h"
#include "STM32L432KC_TIM.h"
#include "STM32L432KC_SPI.h"

// DS1722 Configuration Settings
#define DS1722_WRITE      0x80  // Command to write to a register
#define DS1722_READ       0x00  // Command to read from a register
#define DS1722_TEMP_REG       0x01  // Temperature register address
#define DS1722_CONFIG_REG     0x02  // Configuration register address

// Temperature Sensor Configuration Values
#define DS1722_CONTINUOUS_CONV  0x01  // Continuous conversion mode
#define DS1722_ONE_SHOT         0x00  // One-shot conversion mode

// Function Prototypes
float DS1722_ReadTemperature(int temp_status);

#endif