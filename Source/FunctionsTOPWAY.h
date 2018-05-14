// -------- Include Library
#include <stdio.h>    //Standard Library
#include <string.h> //Standard Library for strings 
#include <unistd.h>   //Used for UART
#include <fcntl.h>    //Used for UART
#include <termios.h>  //Used for UART
#include "DefinesTOPWAY.h" //Defines of TOPWAY Touch Screen

// -------- Defines
// Config
#define XMLSource "/home/pi/DMSConsole/Data/GID.xml"
// Screen
#define ScreenTitleLength 30
#define LimitTitleLength 75
// Main Page
#define MainID 0 
#define ButtonNextDestination 0
#define ButtonPreviousDestination 1
#define ButtonChangeLine 2
#define ButtonSettings 3
#define AdressNumberLine 0x000080
#define AdressNameLine  0x000100
#define AdressNameDestination 0x000180
// Select Destination Page
#define SelectLinesID 1
#define SelectLinesIDKeybord 2
#define ButtonCancelChangeLine 0
#define ButtonConfirmChangeLine 1
#define ButtonUpChangeLine 2
#define ButtonDownChangeLine 3
#define AdressSelectNameLine 0x000200
#define AdressSelectIDLine   0x000280
// Setting Page
#define SettingsID 3

//Type Define Button
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
int Write_String (int uart0_filestream, unsigned int  Adress , unsigned char *Text);
//Get Buttom Event Function
Button Get_Buttom_Event (int uart0_filestream);
//Set Page Function
int Set_Page (int uart0_filestream, int Page);