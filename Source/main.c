// -------- Include Library
#include <stdio.h>    //Standard Library
#include <stdlib.h>   //Standard Library
#include <pigpio.h> // Library Pi
#include "DefinesTOPWAY.h" //Defines of TOPWAY Touch Screen
#include "GIDXml.h" //Headers Functions of XmlReader
#include "DMSDisplay.h" //Headers Functions of DMSDIsplay
#include "FunctionsTOPWAY.h" //Headers Functions of TOPWAY Touch Screen


// -------- Main Function
int main (void){

	//Inicialize GPIO
	if(gpioInitialise() < 0){
		printf("Error! This application won't be able to access the gpio!!\n");
		return 1;

	}

	//Configure GPIO
	gpioSetMode(RX, PI_INPUT);
	gpioSetMode(TX, PI_OUTPUT);
	gpioSetMode(ActiveTx, PI_OUTPUT);
	gpioWrite(ActiveTx, 0);

	//Confiure Bit Bang Rx
	gpioSerialReadOpen(RX, BaudRateDisplay, BitBangByteLength);
	
	//Configure UARTS0 interface
	int uart0_filestream = Config_UARTS0();

	//Buzzer Touch Off
	Buzzer_Touch_Off (uart0_filestream, 0);

	//Loading Xml file
	GID NewGID;
	LoadXMLSource(&NewGID, XMLSource);

	//Loading Initial Sources
	Group *ActualGroup = NewGID.List;
	Line *ActualLine = ActualGroup->List;
	Destination *ActualDestination = ActualLine->List;

	//Loading Painels
	Panel_ID *VectorOfPanels = MSG_Network_Config(TX, RX, BaudRateDisplay);
	if(VectorOfPanels == NULL) printf("Dont have any Panel in this system!\n");
	else{
		Panel_ID *Last = VectorOfPanels;
		while(Last->Previous = NULL){
			printf("--> %x %x %x %x \n", Last->Adress, Last->Lines, Last->Columns, Last->SuportForAlternativeDestinations);
			Last = Last->Previous;
		}
		printf("--> %x %x %x %x \n", Last->Adress, Last->Lines, Last->Columns, Last->SuportForAlternativeDestinations);

	}

	//Allow main page in display
	Set_Page (uart0_filestream, MainID);

	//Main Loop
	unsigned char ActualPage = 0;
	while(1){
		switch(ActualPage){
			case MainID:
				ActualPage = HandlerMain(uart0_filestream, &ActualLine, &ActualDestination);
				break;

			case SelectLinesID:
				ActualPage = HandlerSelectLines(uart0_filestream, &ActualLine, &ActualDestination);
				break;

			default:
				ActualPage = 0;

		}

	}

	//Close UARTS0  
	close(uart0_filestream);

	return 0;

}
