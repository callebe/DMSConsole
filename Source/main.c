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
int HandlerMain (int uart0_filestream, Line **ActualLine, Destination **ActualDestination);
//Handler event for Select Destination
int HandlerSelectLines (int uart0_filestream, Line **ActualLine, Destination **ActualDestination);

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


//Handler event for main Page
int HandlerMain (int uart0_filestream, Line **ActualLine, Destination **ActualDestination){
  
	Destination *Last = *ActualDestination;

	Write_String (uart0_filestream, AdressNumberLine, (*ActualLine)->Number);
	Write_String (uart0_filestream, AdressNameLine, (*ActualLine)->Name);
	Write_String (uart0_filestream, AdressNameDestination, (*ActualDestination)->Name);


  	Button Bt = Get_Buttom_Event (uart0_filestream);

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
				return MainID;

		}

	}

	return MainID;

}

//Handler event for Select Destination
int HandlerSelectLines (int uart0_filestream, Line **ActualLine, Destination **ActualDestination){
  
	Line *Last = *ActualLine;
	int Break = -1;

	Write_String (uart0_filestream, AdressSelectNameLine, Last->Name);
	Write_String (uart0_filestream, AdressSelectIDLine, Last->Number);

	while(Break < 0){
		Button Bt = Get_Buttom_Event (uart0_filestream);
		if((Bt.PagId == SelectLinesID) || (Bt.PagId == SelectLinesIDKeybord)){
			switch (Bt.ButtonId){
				case ButtonUpChangeLine:
					if(Last != NULL){
						if(Last->Next != NULL) Last = Last->Next;
						Write_String (uart0_filestream, AdressSelectNameLine, Last->Name);
						Write_String (uart0_filestream, AdressSelectIDLine, Last->Number);

					}
					Break = -1;
					break;

				case ButtonDownChangeLine:
					if(Last != NULL){
						if(Last->Previous != NULL) Last = Last->Previous;
						Write_String (uart0_filestream, AdressSelectNameLine, Last->Name);
						Write_String (uart0_filestream, AdressSelectIDLine, Last->Number);
					}
					Break = -1; 
					break;

				case ButtonCancelChangeLine:
					Break = MainID;
					break;

				case ButtonConfirmChangeLine:
					*ActualDestination = Last->List;
					*ActualLine = Last;
					Break = MainID;
					break;

				default:
					Break = -1;

			}

		}
	}

	return Break;

}