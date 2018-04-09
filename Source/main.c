// -------- Include Library
#include <stdio.h>    //Standard Library
#include <stdlib.h>   //Standard Library
#include <unistd.h>   //Used for UART
#include <fcntl.h>    //Used for UART
#include <termios.h>  //Used for UART
#include "DefinesTOPWAY.h" //Defines of TOPWAY Touch Screen
#include "FunctionsTOPWAY.h" //Headers Functions of TOPWAY Touch Screen
#include <pigpio.h> // Library Pi

// -------- Defines
#define XMLSource "Data/Routes.txt"
#define BaudRateDisplay 9600
#define BitBangByteLength 8
#define ScreenTitleLength 30
#define LimitTitleLength 75
#define AdressRouteTitlePgSelectDestinations 0x180
#define AdressNumberRoutePgMain 0x200
#define AdressInfoRoutePgMain 0x280
#define ButtonChangeRoute 0
#define ButtonUpSelectRoute 0
#define ButtonDownSelectRoute 1
#define ButtonConfirmSelectRoute 3
#define ButtonCancelSelectRoute 2
#define MainPage 0
#define SelectDestinations 1
#define LoadingPage 2
#define RX 24
#define TX 23

// -------- Main Function
int main (void){

	int NumberOfRouteLines = 0;
	Button Bt;
	unsigned char** RouteLines = NULL;
	FILE *Routes;
	int Counter;
	int CurrentDestination = 0;
	char *SucessFull_fgets = 0;
	char TXXX[14] ="Hellow its EE!";

	//Inialize GPIO
	gpioInitialise();

	//Configure GPIO
	gpioSetMode(RX, PI_INPUT);
	gpioSetMode(TX, PI_OUTPUT);

	//Confiure Bit Bang Rx
	gpioSerialReadOpen(RX, BaudRateDisplay, BitBangByteLength);

	BitBangUARTRx (RX, BaudRateDisplay, TXXX, 14);
	BitBangUARTTx (TX, BaudRateDisplay, TXXX, 14);

	//Configure UARTS0 interface
	int uart0_filestream = Config_UARTS0 ();

	//Buzzer Touch Off
	Buzzer_Touch_Off (uart0_filestream, 0);

	//Loading Routes file
	Routes = fopen(XMLSource,"r");
	if(Routes == NULL){
		printf("--> Read erro in Routes.txt - \n");
		return 1;
	}

	//Counting number of Lines
	while(!feof(Routes)){
		if(fgetc(Routes) == '\n'){
			NumberOfRouteLines++;
		}
	}
	NumberOfRouteLines++;
	

	//Close file
	fclose(Routes);

	//Allocation of Routes
	RouteLines = (unsigned char **)malloc(NumberOfRouteLines*sizeof(unsigned char*));
	if(RouteLines == NULL){
		printf("Allocation of Route Lines error. \n");
		return 1;
	}

	for(Counter = 0; Counter < NumberOfRouteLines; Counter++){
		RouteLines[Counter] = (unsigned char*)malloc((LimitTitleLength) * sizeof(unsigned char));
		if(RouteLines[Counter] == NULL){
			printf("Allocation of Route Lines error. \n");
			return 1;
		}

	}

	//Reloading Routes file
	Routes = fopen("Routes.txt","r");
	if(Routes == NULL){
		printf("Read erro in Routes.txt - \n");
		return 1;
	}

	//Filling the Lines Routes
	Counter = 0;
	while(!feof(Routes)){
		SucessFull_fgets = fgets(RouteLines[Counter], LimitTitleLength, Routes);
		if(SucessFull_fgets) Counter ++;	

	}

	//Reclose file
	fclose(Routes);

	//Allow main page in display
	Set_Page (uart0_filestream, MainPage);

	//Set initial information for display
	Write_String (uart0_filestream, AdressNumberRoutePgMain, &RouteLines[CurrentDestination][0], 3);
	Write_String (uart0_filestream, AdressInfoRoutePgMain, &RouteLines[CurrentDestination][5], ScreenTitleLength);

	
	//Main Loop
	while(1){
		Bt = Get_Buttom_Event (uart0_filestream);
		if((Bt.PagId == MainPage) && (Bt.ButtonId == ButtonChangeRoute)){
			Write_String (uart0_filestream, AdressRouteTitlePgSelectDestinations, &RouteLines[CurrentDestination][0], ScreenTitleLength);
			Counter = CurrentDestination;

		}
		else{
			if(Bt.PagId == SelectDestinations){
				switch (Bt.ButtonId){

					case ButtonUpSelectRoute :
						if(Counter < NumberOfRouteLines) Counter++;
						Write_String (uart0_filestream, AdressRouteTitlePgSelectDestinations, &RouteLines[Counter][0], ScreenTitleLength);
						

					break;

					case ButtonDownSelectRoute:
						if(Counter > 0) Counter--;
						Write_String (uart0_filestream, AdressRouteTitlePgSelectDestinations, &RouteLines[Counter][0], ScreenTitleLength);
						

					break;

					case ButtonConfirmSelectRoute :
						CurrentDestination = Counter;
						Write_String (uart0_filestream, AdressNumberRoutePgMain, &RouteLines[CurrentDestination][0], 3);
						Write_String (uart0_filestream, AdressInfoRoutePgMain, &RouteLines[CurrentDestination][5], ScreenTitleLength);

					break;

					case ButtonCancelSelectRoute:
						Write_String (uart0_filestream, AdressNumberRoutePgMain, &RouteLines[CurrentDestination][0], 3);
						Write_String (uart0_filestream, AdressInfoRoutePgMain, &RouteLines[CurrentDestination][5], ScreenTitleLength);

					break;

					default :
						Write_String (uart0_filestream, AdressRouteTitlePgSelectDestinations, &RouteLines[CurrentDestination][0], ScreenTitleLength);

				}

			}
		}

	}

	//Close UARTS0  
	close(uart0_filestream);

}