/*
File: Lab_6_JHB.c
Author: Zhian Zhou
Email: zzhou@g.hmc.edu
Date: 10/24/24
*/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"

/////////////////////////////////////////////////////////////////
// Provided Constants and Functions
/////////////////////////////////////////////////////////////////

//Defining the web page in two chunks: everything before the current time, and everything after the current time
char* webpageStart = "<!DOCTYPE html><html><head><title>E155 Web Server Demo Webpage</title>\
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
	</head>\
	<body><h1>E155 Web Server Demo Webpage</h1>";
char* ledStr = "<p>LED Control:</p><form action=\"ledon\"><input type=\"submit\" value=\"Turn the LED on!\"></form>\
	<form action=\"ledoff\"><input type=\"submit\" value=\"Turn the LED off!\"></form>";
char* tempStr = "<p>Temperature Resolution:</p><form action=\"8-bit\"><input type=\"submit\" value=\"8-bit resolution\"></form>\
	<form action=\"9-bit\"><input type=\"submit\" value=\"9-bit resolution\"></form>\
        <form action=\"10-bit\"><input type=\"submit\" value=\"10-bit resolution\"></form>\
        <form action=\"11-bit\"><input type=\"submit\" value=\"11-bit resolution\"></form>\
        <form action=\"12-bit\"><input type=\"submit\" value=\"12-bit resolution\"></form>";
char* webpageEnd   = "</body></html>";

//determines whether a given character sequence is in a char array request, returning 1 if present, -1 if not present
int inString(char request[], char des[]) {
	if (strstr(request, des) != NULL) {return 1;}
	return -1;
}

int updateLEDStatus(char request[])
{
	int led_status = 0;
	// The request has been received. now process to determine whether to turn the LED on or off
	if (inString(request, "ledoff")==1) {
		digitalWrite(LED_PIN, PIO_LOW);
		led_status = 0;
	}
	else if (inString(request, "ledon")==1) {
		digitalWrite(LED_PIN, PIO_HIGH);
		led_status = 1;
	}

	return led_status;
}

int updateTempResolution(char request[])
{
        int temp_status = 0;
	// The request has been received. now process to determine temperature resolution
	if (inString(request, "8-bit")==1) {
		temp_status = 8;
	}
	else if (inString(request, "9-bit")==1) {
		temp_status = 9;
	}
        else if (inString(request, "10-bit")==1) {
		temp_status = 10;
	}
        else if (inString(request, "11-bit")==1) {
		temp_status = 11;
	}
        else if (inString(request, "12-bit")==1) {
		temp_status = 12;
	}

	return temp_status;
        
}

/////////////////////////////////////////////////////////////////
// Solution Functions
/////////////////////////////////////////////////////////////////

int main(void) {
  configureFlash();
  configureClock();

  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);

  pinMode(PB3, GPIO_OUTPUT);
  
  RCC->APB2ENR |= (RCC_APB2ENR_TIM15EN);
  initTIM(TIM15);
  
  USART_TypeDef * USART = initUSART(USART1_ID, 125000);

  // TODO: Add SPI initialization code
  initSPI(1, 0, 0);  // baud rate, cpol=0, cpha=0

  while(1) {
    digitalWrite(PB7, 1);
    spiSendReceive(0b01);
    digitalWrite(PB7, 0);

  }

  while(1) {
    /* Wait for ESP8266 to send a request.
    Requests take the form of '/REQ:<tag>\n', with TAG begin <= 10 characters.
    Therefore the request[] array must be able to contain 18 characters.
    */

    // Receive web request from the ESP
    char request[BUFF_LEN] = "                  "; // initialize to known value
    int charIndex = 0;
  
    // Keep going until you get end of line character
    while(inString(request, "\n") == -1) {
      // Wait for a complete request to be transmitted before processing
      while(!(USART->ISR & USART_ISR_RXNE));
      request[charIndex++] = readChar(USART);
    }

    // TODO: Add SPI code here for reading temperature

  
    // Update string with current LED state
    int led_status = updateLEDStatus(request);

    char ledStatusStr[20];
    if (led_status == 1)
      sprintf(ledStatusStr,"LED is on!");
    else if (led_status == 0)
      sprintf(ledStatusStr,"LED is off!");

    // Update string with current temperature
    int temp_resolution = updateTempResolution(request);
    digitalWrite(PB7, PIO_HIGH);
    float temp = DS1722_ReadTemperature(temp_resolution);
    digitalWrite(PB7, PIO_LOW);

    char currentTempStr[24];
    sprintf(currentTempStr,"%f degree Celsius", temp);

    // finally, transmit the webpage over UART
    sendString(USART, webpageStart); // webpage header code
    sendString(USART, ledStr); // button for controlling LED
    sendString(USART, "<h2>LED Status</h2>");

    sendString(USART, "<p>");
    sendString(USART, ledStatusStr);
    sendString(USART, "</p>");

    sendString(USART, tempStr); // button for different temperature resolutions
    sendString(USART, "<h2>Current Temperature</h2>");

    sendString(USART, "<p>");
    sendString(USART, currentTempStr);
    sendString(USART, "</p>");

  
    sendString(USART, webpageEnd);
  }
}
