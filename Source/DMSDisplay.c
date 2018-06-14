// -------- Include Library
#include "DMSDisplay.h" //Headers Functions of DMSDIsplay

//Global Variables
unsigned char BBTx;
unsigned char BBRx;
unsigned int BR;
Destination *ADestination;
Panel_ID *LPanels;
unsigned char IndexLocal;

//Interrupt Flag
unsigned int InterruptFlagRealTimer = 0;
unsigned int InterruptFlagVirtualTimer = 0;

//Search Fonts
unsigned char SearchFonts (unsigned char FontWidth, unsigned char FontHeight){

	switch(FontHeight){
		case (5):
			switch(FontWidth){
				case(5):
					return 0;
					break;

				default:
					return 0;
			}
			break;

		case (7):
			switch(FontWidth){
				case(5):
					return 1;
					break;

				default:
					return 1;
			}
			break;

		case (8):
			switch(FontWidth){
				case(6):
					return 2;
					break;

				default:
					return 2;
			}
			break;

		case (11):
			switch(FontWidth){
				case(7):
					return 3;
					break;

				default:
					return 3;
			}
			break;

		case (12):
			switch(FontWidth){
				case(7):
					return 4;
					break;

				default:
					return 4;
			}
			break;

		case (13):
			switch(FontWidth){
				case(9):
					return 5;
					break;

				default:
					return 5;
			}
			break;

		case (14):
			switch(FontWidth){
				case(9):
					return 6;
					break;

				default:
					return 6;
			}
			break;

		case (15):
			switch(FontWidth){
				case(9):
					return 7;
					break;

				default:
					return 7;
			}
			break;

		case (16):
			switch(FontWidth){
				case(9):
					return 8;
					break;

				default:
					return 8;
			}
			break;

		case (17):
			switch(FontWidth){
				case(11):
					return 9;
					break;

				default:
					return 9;
			}
			break;

		case (19):
			switch(FontWidth){
				case(11):
					return 10;
					break;

				default:
					return 10;
			}
			break;

		default:
			return 0;
			break;
		
	}

}

//Bit Bang UART Transmission Function
int BitBangUARTTx (unsigned char BitBangTx, unsigned int  Baudrate, char *Tx, int Dimension){
	
	gpioWaveClear();
	gpioWaveAddSerial(BitBangTx, Baudrate, 8, 2, 0, Dimension, Tx);
	int WaveSerialTx = gpioWaveCreate();
	
	//Set ActiveTx GPIO pin to '1' for Transmitting mensage
	gpioWrite(ActiveTx, 1);

	//Delay for configure drive
	Config_Real_Timer (0, 5, 0, 0);
	while(InterruptFlagRealTimer == 0);

	//Send mensage 
	gpioWaveTxSend(WaveSerialTx, PI_WAVE_MODE_ONE_SHOT_SYNC);

	//Delay for configure drive
	Config_Real_Timer (0, Dimension+5, 0, 0);
	while(InterruptFlagRealTimer == 0);

	//Set ActiveTx GPIO pin to '0' for Transmitting mensage
	gpioWrite(ActiveTx, 0);

	//Reset timer
	Reconfig_Real_Timer(0,0,0,0);
	
	return 0;

}

//Bit Bang UART Receive Function
int BitBangUARTRx (unsigned char BitBangRx, unsigned int  Baudrate, char *Rx, int Dimension, unsigned int TimeOutSeconds, unsigned int TimeOutMilliseconds){
	
	unsigned char CounterRx = 0;
	int LengthOfBuffer;
	char BufferRx[Dimension];
	
	//Delay for configure drive
	Config_Real_Timer (TimeOutSeconds, TimeOutMilliseconds, 0, 0);
	
	while((CounterRx < Dimension) && (InterruptFlagRealTimer == 0)){
		LengthOfBuffer = gpioSerialRead(BitBangRx, &BufferRx[0], Dimension);
		if(LengthOfBuffer > 0){
			for(int c = 0; c<(Dimension-CounterRx); c++){
				Rx[CounterRx+c] = BufferRx[c]; 

			}
			CounterRx += LengthOfBuffer;

		}

	}

	//Reset timer
	Reconfig_Real_Timer(0,0,0,0);

	if (CounterRx < Dimension) return 1;
	else return 0;

}

//Interrupt Real Timer Handler Function
void Real_Timer_Handler (int signum){
	
	InterruptFlagRealTimer++;

}

//Configure Real Timer Function
int Config_Real_Timer (unsigned int TimeValueSeconds, unsigned int TimeValueMillisecond, unsigned int TimeIntervalSeconds, unsigned int TimeIntervalMillisecond){

	struct sigaction Signal_Action;
	struct itimerval Timer;

	//Install timer handler like a signal for SIGVTALRM/SIGALRM
	memset (&Signal_Action, 0, sizeof(Signal_Action));
	Signal_Action.sa_handler = &Real_Timer_Handler;
	sigaction(SIGALRM, &Signal_Action, NULL);

	//Configure timer, timer in 1MHz, timer limite by a display is 16*100 miliseconds
	Timer.it_value.tv_sec = TimeValueSeconds;
	Timer.it_value.tv_usec = TimeValueMillisecond*1000;
	Timer.it_interval.tv_sec = TimeIntervalSeconds;
	Timer.it_interval.tv_usec = TimeIntervalMillisecond*1000;

	//Start virtual timer ITIMER_VIRTUAL/ITIMER_REAL
	setitimer(ITIMER_REAL, &Timer, NULL);

	InterruptFlagRealTimer = 0;

	return 0;

}

//Reconfigure Real Timer Function
int Reconfig_Real_Timer (unsigned int TimeValueSeconds, unsigned int TimeValueMillisecond, unsigned int TimeIntervalSeconds, unsigned int TimeIntervalMillisecond){

	struct itimerval Timer;

	getitimer(ITIMER_REAL, &Timer);
	Timer.it_value.tv_sec = TimeValueSeconds;
	Timer.it_value.tv_usec = TimeValueMillisecond*1000;
	Timer.it_interval.tv_sec = TimeIntervalSeconds;
	Timer.it_interval.tv_usec = TimeIntervalMillisecond*1000;
	setitimer(ITIMER_REAL, &Timer, NULL);

	InterruptFlagRealTimer = 0;

	return 0;

}

//Interrupt Virtual Timer Handler Function
void Virtual_Timer_Handler (int signum){

	Send_MSG_Info (BBTx, BBRx, BR, IndexLocal, ADestination, LPanels);

}

//Configure Virtual Timer Function
int Config_Virtual_Timer (unsigned int TimeValueSeconds, unsigned int TimeValueMillisecond, unsigned int TimeIntervalSeconds, unsigned int TimeIntervalMillisecond){

	struct sigaction Signal_Action;
	struct itimerval Timer;

	//Install timer handler like a signal for SIGVTALRM/SIGALRM
	memset (&Signal_Action, 0, sizeof(Signal_Action));
	Signal_Action.sa_handler = &Virtual_Timer_Handler;
	sigaction(SIGVTALRM, &Signal_Action, NULL);

	//Configure timer, timer in 1MHz, timer limite by a display is 16*100 miliseconds
	Timer.it_value.tv_sec = TimeValueSeconds;
	Timer.it_value.tv_usec = TimeValueMillisecond*1000;
	Timer.it_interval.tv_sec = TimeIntervalSeconds;
	Timer.it_interval.tv_usec = TimeIntervalMillisecond*1000;

	//Start virtual timer ITIMER_VIRTUAL/ITIMER_REAL
	setitimer(ITIMER_VIRTUAL, &Timer, NULL);

	InterruptFlagVirtualTimer = 0;

	return 0;

}

//Reconfigure Virtual Timer Function
int Reconfig_Virtual_Timer (unsigned int TimeValueSeconds, unsigned int TimeValueMillisecond, unsigned int TimeIntervalSeconds, unsigned int TimeIntervalMillisecond){

	struct itimerval Timer;

	getitimer(ITIMER_VIRTUAL, &Timer);
	Timer.it_value.tv_sec = TimeIntervalSeconds;
	Timer.it_value.tv_usec = TimeIntervalMillisecond*1000;
	Timer.it_interval.tv_sec = TimeIntervalSeconds;
	Timer.it_interval.tv_usec = TimeIntervalMillisecond*1000;
	setitimer(ITIMER_VIRTUAL, &Timer, NULL);

	InterruptFlagVirtualTimer = 0;

	return 0;

}

//Request displays info function
Panel_ID* MSG_Network_Config (unsigned char BitBangTx, unsigned char BitBangRx, unsigned int  Baudrate){

	int Failure = 2;
	Panel_ID *Last = NULL;
	Panel_ID *First = NULL;

	char CHKS = (unsigned char)(SOH + 0xFF + 0x00 + STX + 0x00 + 0x01 + 0x22 + ETX);
	//                         <Dest> <Orig>         <N>    <Type>  
	unsigned char Tx[9] = {SOH, 0xFF, 0x00, STX, 0x00, 0x01, 0x22, ETX, CHKS};
	unsigned char Rx[13];
	unsigned char CHKSRx;


	for(int counter = 0; (counter<3 && Failure == 2); counter++){
		
		// Transmitting in Broadcast
		BitBangUARTTx (BitBangTx, Baudrate, &Tx[0], 9);
		// Clean Buffer
		BitBangUARTRx (BitBangRx, Baudrate, &Rx[0], 1, 1, 10);

		do{
			// Receiving first byte
			// The time limit for Rx is LimitOfDisplays * 100 ms = 1,6 seconds
			Failure = BitBangUARTRx (BitBangRx, Baudrate, &Rx[0], 1, 1, 600);
			if(Rx[0] != NACK &&  Failure == 0){
				// Receiving the last bytes
				Failure = BitBangUARTRx (BitBangRx, Baudrate, &Rx[1], 12, 1, 600);
				CHKSRx = (unsigned char)(Rx[0] + Rx[1] + Rx[2] + Rx[3] + Rx[4] + Rx[5] + Rx[6] + Rx[7] + Rx[8] + Rx[9] + Rx[10] + Rx[11]);

				if(Failure == 0 && CHKSRx == Rx[12]){
					//Chaining
					if(Last == NULL){
						Last = (Panel_ID *)malloc(sizeof(Panel_ID));
						Last->Previous = NULL;
						First = Last;

					}
					else{
						Last->Next = (Panel_ID *)malloc(sizeof(Panel_ID));
						Last->Next->Previous = Last;
						Last = Last->Next;

					}
					Last->Next = NULL;
					Last->Adress = Rx[7];
					Last->Lines = Rx[8];
					Last->Columns = Rx[9];
					Last->SuportForAlternativeDestinations = Rx[10];
				}
				else{
					if(Failure == 0){
						Failure = 2;
						if(Last != NULL){
							while(Last->Previous = NULL){
								Last = Last->Previous;
								free (Last->Next);
							}
							free(Last);
							Last = NULL;
							First = NULL;
						}

					}
				}
			}
			else{
				if(Failure == 0){
					Failure = 2;
					if(Last != NULL){
						while(Last->Previous = NULL){
							Last = Last->Previous;
							free (Last->Next);
						}
						free(Last);
						Last = NULL;
						First = NULL;
					}
					
				} 
			}

		}while(Failure == 0 && Failure != 2);

	}

	return First;

}

//Send Mensage of Information
int Send_MSG_Info (unsigned char BitBangTx, unsigned char BitBangRx, unsigned int  Baudrate, unsigned char Index, Destination *ActualDestination, Panel_ID *ListOfPanels){

	Panel *LastPanel;
	unsigned char Rx[9]; //<SOH> 0x00 Orig <STX> 0 1 0x86 <STX> <CHKS>
	unsigned char Tx[100];
	char CHKS;
	int cont;
	int Counter;
	int Control;
	unsigned char VisualInformation;
	unsigned char NumberOfTextLines;
	unsigned char BytesForLineRepresentation;
	unsigned char ConfirmReceiver;

	while(ListOfPanels != NULL){
		LastPanel = ActualDestination->List;
		while(LastPanel != NULL){
			if((LastPanel->Lines == ListOfPanels->Lines) && (LastPanel->Columns == ListOfPanels->Columns)){

				// SOH
				Tx[0] = SOH;
				//Destination
				Tx[1] = ListOfPanels->Adress;
				//Origin
				Tx[2] = 0x00;
				// Start Text
				Tx[3] = STX;
				// Type of Data
				Tx[6] = 0x21;
				// Data
				// -- Index
				Tx[7] = Index;
				// -- Page general Info
				//    NumberOfPages(2b) + PostingTime3(6b)
				Tx[8] = (unsigned char)(((LastPanel->NumberOfPages) << 6) + (LastPanel->List[1].PostingTime%64));
				//    PostingTime2(1B) 
				Tx[9] = (unsigned char)(LastPanel->List[1].PostingTime);
				//    PostingTime1(1B)
				Tx[10] = (unsigned char)(LastPanel->List[0].PostingTime);
				// -- STREAM ID
				Tx[11] = 'A';
				Tx[12] = 'S';
				Tx[13] = 'C';
				Tx[14] = 'I';
				Tx[15] = 'I';
				Control = 16;
				// -- Page N info
				for (cont = 0; (cont<LastPanel->NumberOfPages && cont<LimitOfPages); cont++){
					//    Header
            		VisualInformation = 0;
            		NumberOfTextLines = 0;
            		if(LastPanel->List[cont].NumberList.FontWidth == 0){
            			if(LastPanel->List[cont].TextList[0].FontWidth == 0) VisualInformation = 3;
            			else VisualInformation = 1;
            		}
            		else{
            			if(LastPanel->List[cont].TextList[0].FontWidth == 0) VisualInformation = 0;
            			else VisualInformation = 2;
            		}
            		for(Counter = 0; (Counter<4 && LastPanel->List[cont].NumberList.Info[Counter] != '\0'); Counter++);
            		//    --> NumberOfLines + VisualInformation + NumberAlignment + BytesForLineRepresentation
            		Tx[Control++] = ((LastPanel->List[cont].NumberOfFields % 4) << 6) + ((VisualInformation % 4) << 4) + ((LastPanel->List[cont].NumberList.Align % 4) << 2) + (Counter % 4);
            		//    --> FildAlign1 + FildAlign2 + FildAlign3 + SpaceBetweenCharacters
            		Tx[Control++] = ((LastPanel->List[cont].TextList[0].Align  % 4) << 6) + ((LastPanel->List[cont].TextList[1].Align % 4) << 4) + ((LastPanel->List[cont].TextList[2].Align % 4) << 2) + (LastPanel->List[cont].SpaceBetweenCharacters % 4);
            		//    --> SpaceBetweenNumbersAndText + FxNumber + FxFild1 
            		Tx[Control++] = ((LastPanel->List[cont].SpaceBetweenNumberAndCharacters % 16) << 4) + ((LastPanel->List[cont].NumberList.Effect % 4) << 2) + (LastPanel->List[cont].TextList[0].Effect % 4);
            		//    --> FxFild2 + FxFild3  
            		Tx[Control++] = ((LastPanel->List[cont].TextList[0].Effect % 16) << 4) + (LastPanel->List[cont].TextList[2].Effect % 16);
            		//    --> ID FontNumber
            		Tx[Control++] = SearchFonts(LastPanel->List[cont].NumberList.FontWidth, LastPanel->List[cont].NumberList.FontHeight);
            		//    --> ID FontFild1
            		Tx[Control++] = SearchFonts(LastPanel->List[cont].TextList[0].FontWidth, LastPanel->List[cont].TextList[0].FontHeight);
           			//    --> ID FontFild2
           			Tx[Control++] = SearchFonts(LastPanel->List[cont].TextList[1].FontWidth, LastPanel->List[cont].TextList[1].FontHeight);
           			//    --> ID FontFild3
           			Tx[Control++] = SearchFonts(LastPanel->List[cont].TextList[2].FontWidth, LastPanel->List[cont].TextList[2].FontHeight);
            		// BUS LINE
            		for(Counter = 0; (Counter<4 && LastPanel->List[cont].NumberList.Info[Counter] != '\0'); Counter++){
            			Tx[Control++] = LastPanel->List[cont].NumberList.Info[Counter];
            		}
            		// INFO LINE
            		if((VisualInformation  == 1) || (VisualInformation  == 2)){
	            		for(int FildCounter = 0; FildCounter<LastPanel->List[cont].NumberOfFields; FildCounter++){
		            		for(Counter = 0; (Counter<LimitOfCharPerLine && LastPanel->List[cont].TextList[FildCounter].Info[Counter] != '\0'); Counter++){
		            			Tx[Control++] = LastPanel->List[cont].TextList[FildCounter].Info[Counter];
		            		}
		            		if(FildCounter+1 < LastPanel->List[cont].NumberOfFields) Tx[Control++] = 13;
	            		}

            		}
            		// CR
            		Tx[Control++] = 13;
            		// LF
            		Tx[Control++] = 10;

            	}
            	// -- END
                Tx[Control++] = 0x23;
                // End of text
				Tx[Control++] = ETX;
                // Number of Bytes
				Tx[4] = (unsigned char)(Control/256);
				Tx[5] = (unsigned char)(Control%256);
				// CHKS
				CHKS = 0;
				for(Counter = 0; Counter<Control; Counter++) CHKS = (unsigned char)(CHKS + Tx[Counter]);
				Tx[Control++] = CHKS;
				// Transmiting Process
				ConfirmReceiver = 0;
				for(int i = 0; (i<1 && ConfirmReceiver == 0); i++){
					// Transmitting
					BitBangUARTTx (BitBangTx, Baudrate, &Tx[0], Control);
					// Receiving
					BitBangUARTRx (BitBangRx, Baudrate, &Rx[0], 9, 0, 10);
					if(Rx[1] == 0 && Rx[6] == ACK) ConfirmReceiver = 1;
				}
				if(ConfirmReceiver == 0) return 1;

			}
			LastPanel = LastPanel->Next;
		}
		ListOfPanels = ListOfPanels->Next;
	}

	return 0;

}

//Config Send MSG Info Routine
int Config_Send_MSG_Info (unsigned char BitBangTx, unsigned char BitBangRx, unsigned int Baudrate, Destination *ActualDestination, Panel_ID *ListOfPanels){
	
	BBTx = BitBangTx;
	BBRx = BitBangRx;
	BR = Baudrate;
	ADestination = ActualDestination;
	LPanels = ListOfPanels;
	IndexLocal = 0;
	Send_MSG_Info (BBTx, BBRx, BR, IndexLocal, ADestination, LPanels);
	Config_Virtual_Timer(10, 0, 10, 0);
}

//Refresh Send MSG Info Routine
int Refresh_Send_MSG_Info_Routine (Destination *ActualDestination, Panel_ID *ListOfPanels){

	ADestination = ActualDestination;
	LPanels = ListOfPanels;
	IndexLocal++;

}



