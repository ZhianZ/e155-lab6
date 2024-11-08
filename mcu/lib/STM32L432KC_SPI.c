// STM32L432KC_SPI.c
// name: Zhian Zhou
// email: zzhou@g.hmc.edu
// date: 10/24/2024
// This file provides SPI driver functions for the STM32L432KC MCU, including
// SPI initialization and data transmission for communicating with SPI peripherals.

#include "STM32L432KC_SPI.h"

///////////////////////////////////////////////////////////////////////////////
// Function Definitions
///////////////////////////////////////////////////////////////////////////////

void initSPI(int br, int cpol, int cpha) {  
    // Enable GPIOB clock
    gpioEnable(GPIO_PORT_B);

    // Configure GPIOB pins for SPI1: SCK (PB5), SDO (PB4), SDI (PB6), CE (PB7)
    pinMode(PIN_SCLK, GPIO_ALT); // SCLK
    pinMode(PIN_SDO, GPIO_ALT); // SDO
    pinMode(PIN_SDI, GPIO_ALT); // SDI
    pinMode(PIN_CE, GPIO_OUTPUT); // CE

    // Set to AF05 for SPI alternate functions
    GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL4, 5);
    GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 5);
    GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL6, 5);

    // Set output speed type to high for SCK
    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED3);
    
    // Enable SPI1 clock
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; 

    // Configure SPI_CR1 register
    // SPI1->CR1 = 0; // Reset control register 1
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpol); // set clock polarity
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPHA, cpha); // set clock phase
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_BR, br); // Set baud rate divider
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_MSTR, 0b1); // Select master mode
    SPI1->CR1 &= ~_VAL2FLD(SPI_CR1_LSBFIRST, 0b1); // Set frame format (LSB or MSB first)


    // Configure Software Slave Management (SSM) and Internal Slave Select (SSI)
    //SPI1->CR1 |= _VAL2FLD(SPI_CR1_CRCEN, 0b0);
    SPI1->CR1 &= ~_VAL2FLD(SPI_CR1_SSM, 0b1);
    SPI1->CR1 &= ~_VAL2FLD(SPI_CR1_SSI, 0b1); 

    // Configure SPI_CR2 register
    // SPI1->CR2 = 0; // Reset control register 2
    SPI1->CR2 |= _VAL2FLD(SPI_CR2_DS, 0b0111); // Set data length (DS[3:0] bits) 8-bit
    SPI1->CR2 |= _VAL2FLD(SPI_CR2_SSOE, 0b1);// Set SSOE bit for master mode
    SPI1->CR2 |= _VAL2FLD(SPI_CR2_FRXTH, 0b1);  // Set FRXTH bit (RX FIFO threshold to 1/4 of the RX FIFO, aligning with 8-bit data access)
    SPI1->CR2 &= ~_VAL2FLD(SPI_CR2_FRF, 0b1);

    // Enable SPI by setting SPE bit in SPI_CR1
    SPI1->CR2 &= ~SPI_CR2_NSSP;
    SPI1->CR1 |= SPI_CR1_SPE;
}


char spiSendReceive(char send) {
    // Wait until TXE (Transmit buffer empty) flag is set
    while (!(SPI1->SR & SPI_SR_TXE));

    // Write data to SPI data register to start transmission
    *((volatile uint8_t *)&SPI1->DR) = send;

    // Wait until RXNE (Receive buffer not empty) flag is set
    while (!(SPI1->SR & SPI_SR_RXNE));

    // Read and return the received data
    return *((volatile uint8_t *)&SPI1->DR);
}
