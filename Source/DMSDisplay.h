// -------- Include Library
#include <stdio.h> //Standard Library
#include <stdlib.h> //Standard Library  
#include <string.h> //Standard Library for strings
#include <pigpio.h> // Library Pi
#include <signal.h> // Linux Signal Library 
#include <sys/time.h> // Linux Timer
#include "GIDXml.h" //Headers Functions of XmlReader

#ifndef DEF_DMSDisplay
	
	#define DEF_DMSDisplay

	//--------- Defines
	#define BitBangByteLength   8
	#define BaudRateDisplay     9600
	#define ActiveTx            4
	#define RX                  24
	#define TX                  23
	#define SOH                 0x81
	#define STX                 0x82
	#define ETX                 0x83
	#define ACK                 0x86 
	#define NACK                0x95
	#define TimeDalyResponse    100 //100 milliseconds
	#define LimitOfDisplays     16
	
	//Type Define Panel ID
	typedef struct Panel_ID{
		int Adress;
		int Lines;
		int Columns;
		int SuportForAlternativeDestinations;
		struct Panel_ID *Next;
		struct Panel_ID *Previous;

	}Panel_ID;

	//Search Fonts
	unsigned char SearchFonts (unsigned char FontWidth, unsigned char FontHeight);
	//Bit Bang UART Transmission Function
	int BitBangUARTTx (unsigned char BitBangTx, unsigned int  Baudrate, char *Tx, int Dimension);
	//Bit Bang UART Receive Function
	int BitBangUARTRx (unsigned char BitBangRx, unsigned int  Baudrate, char *Rx, int Dimension,  unsigned int TimeOutSeconds, unsigned int TimeOutMilliseconds);
	//Handle Interrupt Real Timer Handler Function
	void Real_Timer_Handler (int signum);
	//Configure Real Timer Function
	int Config_Real_Timer (unsigned int TimeValueSeconds, unsigned int TimeValueMillisecond, unsigned int TimeIntervalSeconds, unsigned int TimeIntervalMillisecond);
	//Reconfigure Real Timer Function
	int Reconfig_Real_Timer (unsigned int TimeValueSeconds, unsigned int TimeValueMillisecond, unsigned int TimeIntervalSeconds, unsigned int TimeIntervalMillisecond);
	//Interrupt Virtual Timer Handler Function
	void Virtual_Timer_Handler (int signum);
	//Configure Virtual Timer Function
	int Config_Virtual_Timer (unsigned int TimeValueSeconds, unsigned int TimeValueMillisecond, unsigned int TimeIntervalSeconds, unsigned int TimeIntervalMillisecond);
	//Reconfigure Virtual Timer Function
	int Reconfig_Virtual_Timer (unsigned int TimeValueSeconds, unsigned int TimeValueMillisecond, unsigned int TimeIntervalSeconds, unsigned int TimeIntervalMillisecond);
	//Request displays info function
	Panel_ID* MSG_Network_Config (unsigned char BitBangTx, unsigned char BitBangRx, unsigned int  Baudrate);
	//Send Mensage of Information function
	int Send_MSG_Info (unsigned char BitBangTx, unsigned char BitBangRx, unsigned int  Baudrate, unsigned char Index, Destination *ActualDestination, Panel_ID *ListOfPanels);
	//Config Send MSG Info Routine
	int Config_Send_MSG_Info (unsigned char BitBangTx, unsigned char BitBangRx, unsigned int Baudrate, Destination *ActualDestination, Panel_ID *ListOfPanels);
	//Refresh Send MSG Info Routine
	int Refresh_Send_MSG_Info_Routine (Destination *ActualDestination, Panel_ID *ListOfPanels);


#endif