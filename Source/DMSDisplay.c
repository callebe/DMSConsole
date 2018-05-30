// -------- Include Library
#include "DMSDisplay.h" //Headers Functions of DMSDIsplay

//Interrupt Flag
unsigned int InterruptFlagRealTimer = 0;
unsigned int InterruptFlagVirtualTimer = 0;

//Bit Bang UART Transmission Function
int BitBangUARTTx (unsigned char BitBangTx, unsigned int  Baudrate, char *Tx, int Dimension){
	
	gpioWaveClear();
	gpioWaveAddSerial(BitBangTx, Baudrate, 8, 2, 0, Dimension, Tx);
	int WaveSerialTx = gpioWaveCreate();
	
	//Set ActiveTx GPIO pin to '1' for Transmitting mensage
	gpioWrite(ActiveTx, 1);

	//Delay for configure drive
	Config_Real_Timer (0, 10, 0, 0);
	while(InterruptFlagRealTimer == 0);

	//Send mensage 
	gpioWaveTxSend(WaveSerialTx, PI_WAVE_MODE_ONE_SHOT);

	//Delay for configure drive
	Config_Real_Timer (0, Dimension+1, 0, 0);
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
	
	InterruptFlagVirtualTimer++;

}

//Configure Virtual Timer Function
int Config_Virtual_Timer (unsigned int TimeIntervalSeconds, unsigned int TimeIntervalMillisecond){

	struct sigaction Signal_Action;
	struct itimerval Timer;

	//Install timer handler like a signal for SIGVTALRM/SIGALRM
	memset (&Signal_Action, 0, sizeof(Signal_Action));
	Signal_Action.sa_handler = &Virtual_Timer_Handler;
	sigaction(SIGVTALRM, &Signal_Action, NULL);

	//Configure timer, timer in 1MHz, timer limite by a display is 16*100 miliseconds
	Timer.it_value.tv_sec = TimeIntervalSeconds;
	Timer.it_value.tv_usec = TimeIntervalMillisecond*1000;
	Timer.it_interval.tv_sec = TimeIntervalSeconds;
	Timer.it_interval.tv_usec = TimeIntervalMillisecond*1000;

	//Start virtual timer ITIMER_VIRTUAL/ITIMER_REAL
	setitimer(ITIMER_VIRTUAL, &Timer, NULL);

	InterruptFlagVirtualTimer = 0;

	return 0;

}

//Reconfigure Virtual Timer Function
int Reconfig_Virtual_Timer (unsigned int TimeIntervalSeconds, unsigned int TimeIntervalMillisecond){

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
int Send_MSG_Info (unsigned char BitBangTx, unsigned char BitBangRx, unsigned int  Baudrate, unsigned char Index, Destination *ActualDestination, Panel_ID *RealPanels){

	Panel *ExpectedPanels;
	char Rx = 0;
	char Tx[7];
	int cont;

	// SOH
	Tx[0] = SOH;
	//Destination
	Tx[1] = RealPanels->Adress;
	//Origin
	Tx[2] = 0x00;
	// Start Text
	Tx[3] = STX;
	// Number of Bytes
	Tx[4] = N;
	// Type od Data
	Tx[5] = 0x21;
	// Data
	// -- Index
	Tx[6] = Index;
	// -- Page general Info
	Page *Last = ExpectedPanels->List;
	for(cont = 0 ; (cont<LimitOfPages-1 && Last != NULL); cont++){
		Tx[7+cont] = Last->PostingTime;
		Last = Last->Next;
	}
	Tx[7+cont] = (ExpectedPanels->NumberOfPages << 6) + Tx[7+cont];
	// -- Page
	Tx[8+cont] = ;
	Tx[9+cont] = ;
	Tx[10] = ;
	// End of text
	Tx[7] = ETX;
	// CHKS 
	Tx[8] = CHKS;

	// Constants
	while(RealPanels != NULL){
		ExpectedPanels = ActualDestination->List;
		while(ExpectedPanels != NULL){
			if((RealPanels->Lines == RealPanels->Line) && (RealPanels->Columns == RealPanels->Columns)){
				

			}
			ExpectedPanels = ExpectedPanels->Next;
		}
		RealPanels = RealPanels->Next;
	}


	char CHKS = (unsigned char)(SOH + 0xFF + 0x00 + STX + 0x01 + 0x22 + ETX);
	unsigned char Tx[8] = {SOH, 0xFF, 0x00, STX, 0x01, 0x22, ETX, CHKS};
	unsigned char ConfirmReceiver = 0;
	char CHKS = (char)(SOH + 0xFF + STX + 0x01 + 0x21 + ETX + CHKS);
	//                <Dest>      <N>  <Type>  
	char Tx[7] = {SOH, 0xFF, STX, 0x01, 0x21, ETX, CHKS};
	
	
	for(int i = 0; (i<3 || ConfirmReceiver == 0); i++){
		// Transmitting
		BitBangUARTTx (BitBangTx, Baudrate, &Tx[0], 7);
		// Receiving
		BitBangUARTRx (BitBangRx, Baudrate, &Rx, 1);
		if(Rx == ACK) ConfirmReceiver = 1;

	}

	if(ConfirmReceiver == 0) return 1;
	return 0;

}

// //Send Mensage of Reset for All displays
// int Send_MSG_Reset_All (unsigned char BitBangTx, unsigned char BitBangRx, unsigned int  Baudrate){

// 	unsigned char ConfirmReceiver = 0;
// 	char CHKS = (char)(SOH + 0xFF + STX + 0x01 + 0x01 + ETX);
// 	//                <Dest>      <N>  <Type>               
// 	char Tx[7] = {SOH, 0xFF, STX, 0x01, 0x01, ETX, CHKS};
// 	char Rx = 0;
	
// 	for(int i = 0; (i<4 || ConfirmReceiver == 0); i++){
// 		// Transmitting
// 		BitBangUARTTx (BitBangTx, Baudrate, &Tx[0], 7);
// 		// Receiving
// 		BitBangUARTRx (BitBangRx, Baudrate, &Rx, 1);
// 		if(Rx == ACK) ConfirmReceiver = 1;

// 	}

// 	if(ConfirmReceiver == 0) return 1;
// 	return 0;

// }

// //Send Mensage of Clear for All displays
// int Send_MSG_Clear_All (unsigned char BitBangTx, unsigned char BitBangRx, unsigned int  Baudrate){

// 	unsigned char ConfirmReceiver = 0;
// 	char CHKS = (char)(SOH + 0xFF + STX + 0x01 + 0x20 + ETX);
// 	//                <Dest>      <N>  <Type>  
// 	char Tx[7] = {SOH, 0xFF, STX, 0x01, 0x20, ETX, CHKS};
// 	char Rx = 0;
	
// 	for(int i = 0; (i<4 || ConfirmReceiver == 0); i++){
// 		// Transmitting
// 		BitBangUARTTx (BitBangTx, Baudrate, &Tx[0], 7);
// 		// Receiving
// 		BitBangUARTRx (BitBangRx, Baudrate, &Rx, 1);
// 		if(Rx == ACK) ConfirmReceiver = 1;

// 	}

// 	if(ConfirmReceiver == 0) return 1;
// 	return 0;

// }


