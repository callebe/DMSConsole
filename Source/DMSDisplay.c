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
