// -------- Include Library
#include "FunctionsTOPWAY.h" //Headers Functions of TOPWAY Touch Screen


// -------- Functions
// Configuration Function
int Config_UARTS0 (void){

  int uart0_filestream = -1;
  
  //OPEN UARTS0
  // # The flags (defined in fcntl.h):
  //   Access modes (use 1 of these):
  //   --> O_RDONLY - Open for reading only.
  //   --> O_RDWR - Open for reading and writing.
  //   --> O_WRONLY - Open for writing only.
  //   Enables nonblocking mode. When set read 
  //  requests on the file can return immediately 
  //  with a failure status. If there is no input 
  //  immediately available (instead of blocking). 
  //  Likewise, write requests can also return  
  //  immediately with a failure status if the output 
  //  can't be written immediately.   
  //   --> O_NDELAY / O_NONBLOCK (same function)
  //                      
  //    When set and path identifies a terminal device, 
  //  open() shall not cause the terminal device to become 
  //  the controlling terminal for the process.
  //   --> O_NOCTTY 
  uart0_filestream = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);    //Open in non blocking read/write mode
  if (uart0_filestream == -1){
    //ERROR - CAN'T OPEN SERIAL PORT
    printf("Error - Unable to open UART.\n");
  }
  
  //CONFIGURE THE UARTS0
  // # The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
  //   --> Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
  //   --> CSIZE:- CS5, CS6, CS7, CS8
  //   --> CLOCAL - Ignore modem status lines
  //   --> CREAD - Enable receiver
  //   --> IGNPAR = Ignore characters with parity errors
  //   --> ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
  //   --> PARENB - Parity enable
  //   --> PARODD - Odd parity (else even)
  struct termios options;
  tcgetattr(uart0_filestream, &options);
  options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
  options.c_iflag = IGNPAR;
  options.c_oflag = 0;
  options.c_lflag = 0;
  tcflush(uart0_filestream, TCIFLUSH);
  tcsetattr(uart0_filestream, TCSANOW, &options);

  return uart0_filestream;
}

// Transmission Function
int Tx_UARTS0 (int uart0_filestream, unsigned char *Tx, int Dimension){
  
  int Control = 0;

  if (uart0_filestream != -1){
    Control = write(uart0_filestream, Tx, Dimension); 
    if (Control < 0){
      printf("UART Tx error\n"); 
      return 1;

    } 

  }

  return 0;
}

// Receive Function
int Rx_UARTS0 (int uart0_filestream, unsigned char *Rx, int Dimension){

  unsigned char BufferRx[Dimension];
  int LengthOfBuffer = 0;
  int CounterRx = 0;
  int c;

	//The Receive get any input until counter reach Dimension of Rx
  if (uart0_filestream != -1){
    while(CounterRx < Dimension){
		LengthOfBuffer = read(uart0_filestream, (void*)BufferRx, Dimension);
		if(LengthOfBuffer > 0){
			for(c = 0; c<(Dimension-CounterRx); c++){
			Rx[CounterRx+c] = BufferRx[c]; 

			}
			CounterRx += LengthOfBuffer;

		}

  	}
  }
	return 0;

}

// Buzzer Touch Function
int Buzzer_Touch_Off (int uart0_filestream, unsigned char Time_Interval){

  unsigned char Data[7];
  unsigned char ConfirmCommand[2];

  Data[0] = Header;
  Data[1] = Buzzer_Touch_Sound;
  Data[2] = Time_Interval;
  Data[3] = Tail_A;
  Data[4] = Tail_B;
  Data[5] = Tail_C;
  Data[6] = Tail_D;

  Tx_UARTS0 (uart0_filestream, &Data[0], 7);
  //Error verification
  Rx_UARTS0 (uart0_filestream, &ConfirmCommand[0], 2);
  if(ConfirmCommand[0] == Erro_In_Exe_Command){
    printf("Erro in send Beep Off Function!\n");
    return 1;
  }
  printf("OK\n");
  return 0;

}

// String Write Function
int Write_String (int uart0_filestream, unsigned int  Adress, unsigned char *Text){

  unsigned char Begin[2] = {Header, STR_Write};
  unsigned char Finish[5] = {0, Tail_A, Tail_B, Tail_C, Tail_D};
  unsigned char AuxAdress = 0;
  unsigned char ConfirmCommand[2];
 
  //Header and Command
  Tx_UARTS0 (uart0_filestream, Begin, 2);

  //Adress
  AuxAdress = (int)(Adress / (8388608));
  Tx_UARTS0 (uart0_filestream, &AuxAdress, 1);
  Adress = Adress - AuxAdress*(8388608);
  AuxAdress = (int)(Adress / (32768));
  Tx_UARTS0 (uart0_filestream, &AuxAdress, 1);
  Adress = Adress - AuxAdress*(32768);
  AuxAdress = (int)(Adress / (256));
  Tx_UARTS0 (uart0_filestream, &AuxAdress, 1);
  AuxAdress = Adress - AuxAdress*(256);
  Tx_UARTS0 (uart0_filestream, &AuxAdress,1);

  //Text
  for(int Count=0; (Count<LimitTitleLength)&&(Text[Count] != '\0'); Count++){
    Tx_UARTS0 (uart0_filestream, &Text[Count], 1);
      
  }
  

  //Tail
  Tx_UARTS0 (uart0_filestream, Finish, 5);

  //Error Verification
  Rx_UARTS0 (uart0_filestream, &ConfirmCommand[0], 2);
  if(ConfirmCommand[0] == Erro_In_Exe_Command){
    printf("Erro in send Write String Function!\n");
    return 1;

  }

  return 0;

}

//Get Buttom Event Function
Button Get_Buttom_Event (int uart0_filestream){

  Button Bt;
  unsigned char bufferRx[9];

  Rx_UARTS0 (uart0_filestream, &bufferRx[0], 9);

  if((bufferRx[0] == 0xAA) && (bufferRx[1] == 0x79) && (bufferRx[5] == 0xCC) && (bufferRx[6] == 0x33) && (bufferRx[7] == 0xC3)  && (bufferRx[8] == 0x3C)){
    Bt.PagId = bufferRx[2]*256 + bufferRx[3];
    Bt.ButtonId = bufferRx[4];
  }
  else{
    Bt.PagId = -1;
    Bt.ButtonId = -1;
  }
  

  return Bt;
}

//Set Page Function
int Set_Page (int uart0_filestream, int Page){

  unsigned char Command[8];
  unsigned char ConfirmCommand[2];

  Command[0] = Header;
  Command[1] = Disp_Page;
  Command[2] = (int)(Page/256);
  Command[3] = Command[2]*256 - Page;
  Command[4] = Tail_A;
  Command[5] = Tail_B;
  Command[6] = Tail_C;
  Command[7] = Tail_D;

  Tx_UARTS0(uart0_filestream, &Command[0], 8);
  
  //Error Verification
  Rx_UARTS0 (uart0_filestream, &ConfirmCommand[0], 2);
  if(ConfirmCommand[0] == Erro_In_Exe_Command){
    printf("Erro in send Set Page Function!\n");
    return 1;

  }

  return 0;

}