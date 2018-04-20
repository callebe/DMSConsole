// -------- Include Library
#include <stdio.h>    //Standard Library
#include <string.h> //Standard Library for strings
#include <pigpio.h> // Library Pi

//Bit Bang UART Transmission Function
int BitBangUARTTx (unsigned char BitBangTx, unsigned int  Baudrate, char *Tx, int Dimension);
//Bit Bang UART Receive Function
int BitBangUARTRx (unsigned char BitBangRx, unsigned int  Baudrate, char *Rx, int Dimension);
