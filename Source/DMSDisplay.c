// -------- Include Library
#include "DMSDisplay.h" //Headers Functions of DMSDIsplay

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
	gpioWaveTxSend(WaveSerialTx, PI_WAVE_MODE_ONE_SHOT);
	return 0;

}

int Timer_Handler_Network_Config (int signum){
	printf("Foiii ! \n");

}


//Configure Timer by Network Config
int Config_Timer_Network_Conifg (void){

	struct sigaction Sig;
	struct itimerval Timer;

	//Configure timer, f = 1 MHz, timer limite by a display is 16*100 miliseconds
	Timer.it_value.tv_sec = 0;
	Timer.it_value.tv_usec = 1600000;
	Timer.it_interval.tv_sec = 0;
	Timer.it_interval.tv_usec = 0;

	//Install timer handler like a signal for SIGVTALRM
	memset (&Sig, 0, sizeof(Sig));
	Sig.sa_handler = &Timer_Handler_Network_Config;
	sigaction(SIGVTALRM, &Sig, NULL);

	//Start virtual timer
	setittimer(ITIMER_VIRTUAL, &Timer, NULL);

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

//Request displays info function
Panel_ID* MSG_Network_Config (unsigned char BitBangTx, unsigned char BitBangRx, unsigned int  Baudrate){

	unsigned char ConfirmReceiver;
	char CHKS = (char)(SOH + 0xFF + STX + 0x01 + 0x22 + ETX + CHKS);
	//                <Dest>      <N>  <Type>  
	char Tx[7] = {SOH, 0xFF, STX, 0x01, 0x22, ETX, CHKS};
	char Rx = 0;
	
	for(int i = 0; i<4; i++){
		for (int c = 0; (i<3 || ConfirmReceiver == 0); i++){
			// Transmitting
			BitBangUARTTx (BitBangTx, Baudrate, &Tx[0], 7);
			// Receiving
			BitBangUARTRx (BitBangRx, Baudrate, &Rx, 1);
			if(Rx != NACK){
				ConfirmReceiver = 1;

			}
		}

	}

	if(ConfirmReceiver == 0) return 1;
	return 0;

}


