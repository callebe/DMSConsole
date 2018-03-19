// -------- Include Library
#include <stdio.h>    //Standard Library
#include <unistd.h>   //Used for UART
#include <fcntl.h>    //Used for UART
#include <termios.h>  //Used for UART
#include "DefinesTOPWAY.h" //Defines of TOPWAY Touch Screen
#include "FunctionsTOPWAY.h" //Headers Functions of TOPWAY Touch Screen


// -------- Main Function
int main (void){

  unsigned char rx_buffer[] = "Brazillllllllllllll";
  unsigned int Adress = 0x180;

  int uart0_filestream = Config_UARTS0 ();

  //Buzzer Touch Off
  Buzzer_Touch_Off (uart0_filestream, 0);


  //Rx_UARTS0 (uart0_filestream, &rx_buffer[0], 12);
  //Tx_UARTS0 (uart0_filestream, &rx_buffer[0], 12);

  Write_String (uart0_filestream, Adress, &rx_buffer[0], 12);
  
  //Close UARTS0  
  close(uart0_filestream);

}