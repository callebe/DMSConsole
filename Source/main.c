// -------- Include Library
#include <stdio.h>    //Standard Library
#include <stdlib.h>   //Standard Library
#include <pigpio.h> // Library Pi
#include "GIDXml.h" //Headers Functions of XmlReader
#include "DMSDisplay.h" //Headers Functions of DMSDIsplay
#include "FunctionsTOPWAY.h" //Headers Functions of TOPWAY Touch Screen

void get_popen(){
    FILE *pf;
    char command[20];
    char data[512];

    // Execute a process listing
    sprintf(command, "ps aux wwwf"); 

    // Setup our pipe for reading and execute our command.
    pf = popen(command,"r"); 

    // Error handling

    // Get the data from the process execution
    fgets(data, 512 , pf);

    // the data is now in 'data'

    if (pclose(pf) != 0)
        fprintf(stderr," Error: Failed to close command stream \n");

    return;
}

// -------- Main Function
int main (void){

	get_popen();

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

	//Loading Default Xml file
	GID NewGID;
	Group *ActualGroup;
	Line *ActualLine;
	Destination *ActualDestination;
 	InitializeFunction(&NewGID, &ActualGroup, &ActualLine, &ActualDestination);

	//Loading Painels
	Panel_ID *VectorOfPanels = MSG_Network_Config(TX, RX, BaudRateDisplay);
	if(VectorOfPanels == NULL) printf("Dont have any Panel in this system!\n");
	else{
		Panel_ID *Last = VectorOfPanels;
		while(Last->Next != NULL){
			printf("--> %x %x %x %x \n", Last->Adress, Last->Lines, Last->Columns, Last->SuportForAlternativeDestinations);
			Last = Last->Next;
		}

	}

	//Config Send MSG Info Routine
	Config_Send_MSG_Info (TX, RX, BaudRateDisplay, ActualDestination, VectorOfPanels);

	//Allow main page in display
	Set_Page (uart0_filestream, MainID);

	//Main Loop
	unsigned char ActualPage = 0;
	unsigned char NextPage = 0;
	while(1){
		switch(ActualPage){
			case MainID:
				NextPage = HandlerMain(uart0_filestream, &ActualLine, &ActualDestination);
				break;

			case SelectLinesID:
				NextPage = HandlerSelectLines(uart0_filestream, ActualGroup->List, &ActualLine, &ActualDestination);
				break;

			case SettingsMainID:
				NextPage = HandlerSettingsMain(uart0_filestream);
				break;

			case WatchID:
				NextPage = HandlerWatch(uart0_filestream);
				break;

			case SourceIDA:
				NextPage = HandlerSource(uart0_filestream, &ActualGroup, &ActualLine, &ActualDestination);
				break;

			case ImportUSBIDA:
				NextPage = HandlerImportUSB(uart0_filestream);
				break;
				
			default:
				NextPage = 0;

		}
		if(ActualPage != NextPage){
			Set_Page(uart0_filestream, NextPage);
            ActualPage = NextPage;

		}

	}

	//Close UARTS0  
	close(uart0_filestream);

	return 0;

}
