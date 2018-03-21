// -------- Include Library
#include <stdio.h>    //Standard Library
#include <unistd.h>   //Used for UART
#include <fcntl.h>    //Used for UART
#include <termios.h>  //Used for UART
#include "DefinesTOPWAY.h" //Defines of TOPWAY Touch Screen

//Type Define
typedef struct Button{
	int PagId;
	int ButtonId;
}Button;

// -------- Prototype of Functions
// Configuration Function
int Config_UARTS0 (void);
// Transmission Function
int Tx_UARTS0 (int uart0_filestream, unsigned char *Tx, int Dimension);
// Receive Function
int Rx_UARTS0 (int uart0_filestream, unsigned char *Rx, int Dimension);
// Buzzer Touch Function
int Buzzer_Touch_Off (int uart0_filestream, unsigned char Time_Interval);
// String Write Function
int Write_String (int uart0_filestream, unsigned int  Adress , unsigned char *Text, int Dimension);
//Get Buttom Event Function
Button Get_Buttom_Event (int uart0_filestream);
//Set Page Function
int Set_Page (int uart0_filestream, int Page);