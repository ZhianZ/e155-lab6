// DS1722.c
// name: Zhian Zhou
// email: zzhou@g.hmc.edu
// date: 10/24/2024
// TODO: <SHORT DESCRIPTION OF WHAT THIS FILE DOES>

#include "DS1722.h"

// Read temperature from the DS1722
float DS1722_ReadTemperature(int temp_resolution) {
    int msb;
    char lsb;
    char resol;

    // set the resolution
    if(temp_resolution == 8) resol = 0b11100000;
    if(temp_resolution == 9) resol = 0b11100010;
    if(temp_resolution == 10) resol = 0b11100100;
    if(temp_resolution == 11) resol = 0b11100110;
    if(temp_resolution == 12) resol = 0b11101000;
    
    //digitalWrite(PB7, PIO_HIGH);-
    //spiSendReceive(0x80);
    //spiSendReceive(resol);
    //digitalWrite(PB7, PIO_LOW);

    digitalWrite(PB7, PIO_HIGH);
    spiSendReceive(0x02);
    msb = spiSendReceive(0x00);
    digitalWrite(PB7, PIO_LOW);

    digitalWrite(PB7, PIO_HIGH);
    spiSendReceive(0x01);
    lsb = spiSendReceive(0x00);
    digitalWrite(PB7, PIO_LOW);

    
    //delay_millis(TIM15, 100);

   
    // mask msb bit
    int mskmsb = msb & 0b01111111;
    float temp;
    
    //get sign bit
    int signbit = msb & (1 << 7);
    if (!signbit) { // check if positive
      temp = mskmsb;
      if(1 << 7 & lsb) temp += 0.5;
      if(1 << 6 & lsb) temp += 0.25;
      if(1 << 5 & lsb) temp += 0.125;
      if(1 << 4 & lsb) temp += 0.0625; 
    }
    else { // is negative
      temp = -(~mskmsb + 1);
      if(1 << 7 & lsb) temp -= 0.5;
      if(1 << 6 & lsb) temp -= 0.25;
      if(1 << 5 & lsb) temp -= 0.125;
      if(1 << 4 & lsb) temp -= 0.0625;
    }

    return temp;
}
