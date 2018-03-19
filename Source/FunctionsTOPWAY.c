// -------- Include Library
#include <stdio.h>    //Standard Library
#include <unistd.h>   //Used for UART
#include <fcntl.h>    //Used for UART
#include <termios.h>  //Used for UART
#include "DefinesTOPWAY.h" //Defines of TOPWAY Touch Screen
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
  uart0_filestream = open("/dev/ttyS0", O_RDWR | O_NOCTTY);    //Open in non blocking read/write mode
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

	int c, counter = 0;
	int LengthOfBuffer = 0;
	unsigned char Buffer[Dimension+1]; //The input of read need have extra caractere for '\0'

	//The Receive get any input until counter reach Dimension of Rx
	while (counter < Dimension){
		if (uart0_filestream != -1){
			LengthOfBuffer = read(uart0_filestream, (void*)Buffer, Dimension);
			if (LengthOfBuffer > 0){
				for(c = 0; c <LengthOfBuffer; c++ ){
					Rx[counter+c] = Buffer[c];	
				}
				counter += LengthOfBuffer;
			}
		}
	}
	return 0;

}

// Buzzer Touch Function
int Buzzer_Touch_Off (int uart0_filestream, unsigned char Time_Interval){

  unsigned char Data[7];

  Data[0] = Header;
  Data[1] = Buzzer_Touch_Sound;
  Data[2] = Time_Interval;
  Data[3] = Tail_A;
  Data[4] = Tail_B;
  Data[5] = Tail_C;
  Data[6] = Tail_D;

  Tx_UARTS0 (uart0_filestream, &Data[0], 7);
  return 0;

}

// String Write Function
int Write_String (int uart0_filestream, unsigned int  Adress , unsigned char *Text, int Dimension){

  unsigned char Begin[2] = {Header, STR_Write};
  unsigned char Finish[5] = {0, Tail_A, Tail_B, Tail_C, Tail_D};
  unsigned char AuxAdress = 0;
 
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
  Tx_UARTS0 (uart0_filestream, Text, Dimension);

  //Tail
  Tx_UARTS0 (uart0_filestream, Finish, 5);

  return 0;

}