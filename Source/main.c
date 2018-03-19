// -------- Include Library
#include <stdio.h>    //Standard Library
#include <stdlib.h> 
#include <unistd.h>   //Used for UART
#include <fcntl.h>    //Used for UART
#include <termios.h>  //Used for UART
#include "DefinesTOPWAY.h" //Defines of TOPWAY Touch Screen
#include "FunctionsTOPWAY.h" //Headers Functions of TOPWAY Touch Screen


// -------- Main Function
int main (void){

  char LineRoutes[45] = "João Álvaro";
  FILE *Routes;
  unsigned char Counter;
  char *SucessFull_fgets = 0;

  unsigned int Adress = 0x180;

  int uart0_filestream = Config_UARTS0 ();

  //Buzzer Touch Off
  Buzzer_Touch_Off (uart0_filestream, 0);

  Routes = fopen("Routes.txt","r");
  if(Routes == NULL){
    printf("--> Read erro in Routes.txt - \n");
    return 1;

  }
  Write_String (uart0_filestream, Adress, &LineRoutes[0], 5);

  getchar();
  for (Counter = 192 ; Counter < 255; Counter++){
    LineRoutes[0] = Counter;
    Write_String (uart0_filestream, Adress, &LineRoutes[0], 1);
    printf("%d\n", Counter);
    getchar();

  }
  

  // Counter = 0;
  // while(!feof(Routes)){
  //   SucessFull_fgets = fgets(LineRoutes, 45,Routes);
  //   printf("--> %s\n", LineRoutes);
  //   if(SucessFull_fgets){
  //     Write_String (uart0_filestream, Adress, &LineRoutes[0], 10);

  //   }

  // }
  
  
  //Close UARTS0  
  close(uart0_filestream);

}