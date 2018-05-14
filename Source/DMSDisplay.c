// -------- Include Library
#include "DMSDisplay.h" //Headers Functions of DMSDIsplay

//Interrupt Flag
unsigned char InterruptFlag = 0;

//Bit Bang UART Receive Function
int BitBangUARTRx (unsigned char BitBangRx, unsigned int  Baudrate, char *Rx, int Dimension){
	
	unsigned char CounterRx;
	int LengthOfBuffer;
	char BufferRx[Dimension];
	
	CounterRx = 0;
	while(CounterRx < Dimension){
		LengthOfBuffer = gpioSerialRead(BitBangRx, &BufferRx[0], Dimension);
		if(LengthOfBuffer > 0){
			for(int c = 0; c<(Dimension-CounterRx); c++){
				Rx[CounterRx+c] = BufferRx[c]; 

			}
			CounterRx += LengthOfBuffer;

		}

	}
	return 0;

}

//Bit Bang UART Transmission Function
int BitBangUARTTx (unsigned char BitBangTx, unsigned int  Baudrate, char *Tx, int Dimension){
	
	gpioWaveClear();
	gpioWaveAddSerial(BitBangTx, Baudrate, 8, 2, 0, Dimension, Tx);
	int WaveSerialTx = gpioWaveCreate();
	
	//Set ActiveTx GPIO pin to '1' for Transmitting mensage
	gpioWrite(ActiveTx, 1);

	//Delay for configure drive
	InterruptFlag = 0;
	while(InterruptFlag < 2);

	//Send mensage 
	gpioWaveTxSend(WaveSerialTx, PI_WAVE_MODE_ONE_SHOT);

	//Delay for configure drive
	InterruptFlag = 0;
	while(InterruptFlag < 2);

	//Set ActiveTx GPIO pin to '0' for Transmitting mensage
	gpioWrite(ActiveTx, 0);

	return 0;

}

//Interrupt Timer Handler Function
void Timer_Handler (int signum){
	InterruptFlag++;

}

//Configure Timer Function
int Config_Timer (void){

	struct sigaction Signal_Action;
	struct itimerval Timer;

	//Install timer handler like a signal for SIGVTALRM/SIGALRM
	memset (&Signal_Action, 0, sizeof(Signal_Action));
	Signal_Action.sa_handler = &Timer_Handler;
	sigaction(SIGALRM, &Signal_Action, NULL);

	//Configure timer, timer in 1MHz, timer limite by a display is 16*100 miliseconds
	Timer.it_value.tv_sec = 1;
	Timer.it_value.tv_usec = 100000;
	Timer.it_interval.tv_sec = 1;
	Timer.it_interval.tv_usec = 100000;

	//Start virtual timer ITIMER_VIRTUAL/ITIMER_REAL
	setitimer(ITIMER_REAL, &Timer, NULL);

	return 0;

}

//Reconfigure Timer Function
int Reconfig_Timer (unsigned int TimeIntervalSeconds, unsigned int TimeIntervalMillisecond){

	struct itimerval Timer;

	getitimer(ITIMER_REAL, &Timer);
	Timer.it_value.tv_sec = TimeIntervalSeconds;
	Timer.it_value.tv_usec = TimeIntervalMillisecond;
	Timer.it_interval.tv_sec = TimeIntervalSeconds;
	Timer.it_interval.tv_usec = TimeIntervalMillisecond;
	setitimer(ITIMER_REAL, &Timer, NULL);

	return 0;

}

//Request displays info function
Panel_ID* MSG_Network_Config (unsigned char BitBangTx, unsigned char BitBangRx, unsigned int  Baudrate){

	Panel_ID *Last = NULL;
	unsigned char ConfirmReceiver;
	char CHKS = (char)(SOH + 0xFF + STX + 0x01 + 0x22 + ETX + CHKS);
	//                <Dest>      <N>  <Type>  
	char Tx[7] = {SOH, 0xFF, STX, 0x01, 0x22, ETX, CHKS};
	char Rx[4] = 0;
	
	InterruptFlag = 0;
	while(InterruptFlag < LimitOfDisplays){
		for (int c = 0; (c<3 || ConfirmReceiver == 0); c++){
			// Transmitting
			BitBangUARTTx (BitBangTx, Baudrate, &Tx[0], 7);
			// Receiving first byte
			BitBangUARTRx (BitBangRx, Baudrate, &Rx, 1);
			if(Rx != NACK){
				ConfirmReceiver = 1;
				// Receiving last 3 bytes
				BitBangUARTRx (BitBangRx, Baudrate, &Rx[1], 3);

				//Chaining
				if(Last == NULL){
					Last = (Panel_ID *)malloc(sizeof(Panel_ID));
					Last->Previous = NULL;

				}
				else{
					while(Last->Next != NULL) Last = Last->Next;
					Last->Next = (Field *)malloc(sizeof(Field));
					Last->Next->Previous = Last;
					Last = Last->Next;

				}
				Last->Next = NULL;
				Last->Adress = Rx[0];
				Last->Lines = Rx[1];
				Last->Columns = Rx[2];
				Last->SuportForAlternativeDestinations =Rx[3];

			}
		}

	}

	return Last;

}

//Send Mensage of Reset for All displays
int Send_MSG_Reset_All (unsigned char BitBangTx, unsigned char BitBangRx, unsigned int  Baudrate){

	unsigned char ConfirmReceiver = 0;
	char CHKS = (char)(SOH + 0xFF + STX + 0x01 + 0x01 + ETX);
	//                <Dest>      <N>  <Type>               
	char Tx[7] = {SOH, 0xFF, STX, 0x01, 0x01, ETX, CHKS};
	char Rx = 0;
	
	for(int i = 0; (i<4 || ConfirmReceiver == 0); i++){
		// Transmitting
		BitBangUARTTx (BitBangTx, Baudrate, &Tx[0], 7);
		// Receiving
		BitBangUARTRx (BitBangRx, Baudrate, &Rx, 1);
		if(Rx == ACK) ConfirmReceiver = 1;

	}

	if(ConfirmReceiver == 0) return 1;
	return 0;

}

//Send Mensage of Clear for All displays
int Send_MSG_Clear_All (unsigned char BitBangTx, unsigned char BitBangRx, unsigned int  Baudrate){

	unsigned char ConfirmReceiver = 0;
	char CHKS = (char)(SOH + 0xFF + STX + 0x01 + 0x20 + ETX);
	//                <Dest>      <N>  <Type>  
	char Tx[7] = {SOH, 0xFF, STX, 0x01, 0x20, ETX, CHKS};
	char Rx = 0;
	
	for(int i = 0; (i<4 || ConfirmReceiver == 0); i++){
		// Transmitting
		BitBangUARTTx (BitBangTx, Baudrate, &Tx[0], 7);
		// Receiving
		BitBangUARTRx (BitBangRx, Baudrate, &Rx, 1);
		if(Rx == ACK) ConfirmReceiver = 1;

	}

	if(ConfirmReceiver == 0) return 1;
	return 0;

}



//Send Mensage of Information
int Send_MSG_Info (unsigned char BitBangTx, unsigned char BitBangRx, unsigned int  Baudrate){

	unsigned char ConfirmReceiver = 0;
	char CHKS = (char)(SOH + 0xFF + STX + 0x01 + 0x21 + ETX + CHKS);
	//                <Dest>      <N>  <Type>  
	char Tx[7] = {SOH, 0xFF, STX, 0x01, 0x21, ETX, CHKS};
	char Rx = 0;
	
	for(int i = 0; (i<4 || ConfirmReceiver == 0); i++){
		// Transmitting
		BitBangUARTTx (BitBangTx, Baudrate, &Tx[0], 7);
		// Receiving
		BitBangUARTRx (BitBangRx, Baudrate, &Rx, 1);
		if(Rx == ACK) ConfirmReceiver = 1;

	}

	if(ConfirmReceiver == 0) return 1;
	return 0;

}