// -------- Include Library
#include <stdio.h>    //Standard Library
#include <stdlib.h>   //Standard Library
#include <unistd.h>   //Used for UART
#include <fcntl.h>    //Used for UART
#include <termios.h>  //Used for UART
#include <pigpio.h> // Library Pi
#include "DefinesTOPWAY.h" //Defines of TOPWAY Touch Screen
#include "GIDXml.h" //Headers Functions of XmlReader
#include "FunctionsTOPWAY.h" //Headers Functions of TOPWAY Touch Screen
#include "DMSDisplay.h" //Headers Functions of DMSDIsplay

// Handler of Main Page
int HandlerMain (int uart0_filestream, Destination **ActualDestination);


// -------- Main Function
int main (void){

	//Inialize GPIO
	gpioInitialise();

	//Configure GPIO
	gpioSetMode(RX, PI_INPUT);
	gpioSetMode(TX, PI_OUTPUT);

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

	//Allow main page in display
	Set_Page (uart0_filestream, MainID);

	//Set initial information for display
	Write_String (uart0_filestream, AdressNumberLine, ActualLine->Number);
	Write_String (uart0_filestream, AdressNameLine, ActualLine->Name);
	Write_String (uart0_filestream, AdressNameDestination, ActualDestination->Name);

	unsigned char ActualPage = 0;
	//Main Loop
	while(1){
		switch(ActualPage){
			case MainID:
				ActualPage = HandlerMain(uart0_filestream, &ActualDestination);
				break;

			case SelectLinesID:
				//ActualPage = HandlerSelectDestination(uart0_filestream, ActualDestination);
				ActualPage = 0;
				break;

			default:
				ActualPage = 0;

		}

	}

	//Close UARTS0  
	close(uart0_filestream);

	return 0;

}


//Handler event for main Page
int HandlerMain (int uart0_filestream, Destination **ActualDestination){
  
  Destination *Last = *ActualDestination;
  Button Bt;
  Bt = Get_Buttom_Event (uart0_filestream);

  if(Bt.PagId == MainID){
    switch (Bt.ButtonId){
      case ButtonNextDestination:
        if(Last != NULL){
			if(Last->Next != NULL) Last = Last->Next;
			*ActualDestination = Last;
			Write_String (uart0_filestream, AdressNameDestination, Last->Name);

        } 
        break;

      case ButtonPreviousDestination:
        if(Last != NULL){
          if(Last->Previous != NULL) Last = Last->Previous;
          *ActualDestination = Last;
          Write_String (uart0_filestream, AdressNameDestination, Last->Name);
        } 
        break;

      case ButtonChangeLine:
        return SelectLinesID;
        break;

      case ButtonSettings:
        return SettingsID;
        break;

      default:
        return 0;

    }
  }

  return 0;

}