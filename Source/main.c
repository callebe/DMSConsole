// -------- Include Library
#include <stdio.h>    //Standard Library
#include <stdlib.h> 
#include <unistd.h>   //Used for UART
#include <fcntl.h>    //Used for UART
#include <termios.h>  //Used for UART
#include "DefinesTOPWAY.h" //Defines of TOPWAY Touch Screen
#include "FunctionsTOPWAY.h" //Headers Functions of TOPWAY Touch Screen

// -------- Defines
#define ScreenTitleLength 30
#define LimitTitleLength 75
#define AdressRouteTitlePgB 0x180
#define ButtonUpSelectRoute 1
#define ButtonDownSelectRoute 2
#define ButtonConfirmSelectRoute 3
#define ButtonCancelSelectRoute 4
#define MainPage 0
#define LoadingPage 1

// -------- Main Function
int main (void){

	int NumberOfRouteLines = 0;
	Button Bt;
	unsigned char** RouteLines = NULL;
	FILE *Routes;
	int Counter;
	char *SucessFull_fgets = 0;

	//Configure UARTS0 interface
	int uart0_filestream = Config_UARTS0 ();

	//Buzzer Touch Off
	Buzzer_Touch_Off (uart0_filestream, 0);

	//Loading Routes file
	Routes = fopen("Routes.txt","r");
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

	Counter = 0;
	while(1){
		Bt = Get_Buttom_Event (uart0_filestream);
		if(Bt.ButtonId == ButtonUpSelectRoute){
			Write_String (uart0_filestream, AdressRouteTitlePgB, &RouteLines[Counter++][0], ScreenTitleLength);
			if(Counter == NumberOfRouteLines) Counter = NumberOfRouteLines-1;

		}
		else{
			if(Bt.ButtonId == ButtonDownSelectRoute){
				Write_String (uart0_filestream, AdressRouteTitlePgB, &RouteLines[Counter--][0], ScreenTitleLength);
				if(Counter < 0) Counter = 0;
			}
		}

	}

	//Close UARTS0  
	close(uart0_filestream);

}