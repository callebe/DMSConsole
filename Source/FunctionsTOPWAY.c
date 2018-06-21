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
  return 0;

}

//Integer 16b write function
int Write_Integer16b (int uart0_filestream, unsigned int  Adress, int Value){

  unsigned char Begin[2] = {Header, N16_Write};
  unsigned char Finish[4] = {Tail_A, Tail_B, Tail_C, Tail_D};
  unsigned char AuxAdress = 0;
  unsigned char ConfirmCommand[2];
 
  //Header and Command
  Tx_UARTS0 (uart0_filestream, Begin, 2);
  //Adress
  AuxAdress = (unsigned char)(Adress / (16777216));
  Tx_UARTS0 (uart0_filestream, &AuxAdress, 1);
  Adress = Adress - AuxAdress*(16777216);
  AuxAdress = (unsigned char)(Adress / (65536));
  Tx_UARTS0 (uart0_filestream, &AuxAdress, 1);
  Adress = Adress - AuxAdress*(65536);
  AuxAdress = (unsigned char)(Adress / (256));
  Tx_UARTS0 (uart0_filestream, &AuxAdress, 1);
  AuxAdress = Adress - AuxAdress*(256);
  Tx_UARTS0 (uart0_filestream, &AuxAdress,1);

  //Integer 2 Bytes
  unsigned char ValueConv = (unsigned char)(Value/256);
  Tx_UARTS0 (uart0_filestream, &ValueConv, 1);
  ValueConv = (unsigned char)(Value%256);
  Tx_UARTS0 (uart0_filestream, &ValueConv, 1);
  

  //Tail
  Tx_UARTS0 (uart0_filestream, Finish, 4);

  //Error Verification
  Rx_UARTS0 (uart0_filestream, &ConfirmCommand[0], 2);
  if(ConfirmCommand[0] == Erro_In_Exe_Command){
    printf("Erro in send Integer(16 bits) Function!\n");
    return 1;

  }

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
  AuxAdress = (unsigned char)(Adress / (16777216));
  Tx_UARTS0 (uart0_filestream, &AuxAdress, 1);
  Adress = Adress - AuxAdress*(16777216);
  AuxAdress = (unsigned char)(Adress / (65536));
  Tx_UARTS0 (uart0_filestream, &AuxAdress, 1);
  Adress = Adress - AuxAdress*(65536);
  AuxAdress = (unsigned char)(Adress / (256));
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

  //Receiving data
  Rx_UARTS0 (uart0_filestream, &bufferRx[0], 9);

  //if touck key is Down
  if((bufferRx[0] == 0xAA) && (bufferRx[1] == 0x79) && (bufferRx[5] == 0xCC) && (bufferRx[6] == 0x33) && (bufferRx[7] == 0xC3)  && (bufferRx[8] == 0x3C)){
    Bt.PagId = bufferRx[2]*256 + bufferRx[3];
    Bt.ButtonId = bufferRx[4];
    Bt.Direction = 0;
  }
  else{
    //if touck key is Up
    if((bufferRx[0] == 0xAA) && (bufferRx[1] == 0x78) && (bufferRx[5] == 0xCC) && (bufferRx[6] == 0x33) && (bufferRx[7] == 0xC3)  && (bufferRx[8] == 0x3C)){
      Bt.PagId = bufferRx[2]*256 + bufferRx[3];
      Bt.ButtonId = bufferRx[4];
      Bt.Direction = 1;
    }
    //Dont acknowledge
    else{
      Bt.PagId = -1;
      Bt.ButtonId = -1;
    }
  }
  return Bt;

}

//Set RTC Function
int Set_RTC (int uart0_filestream, unsigned char Year, unsigned char Month, unsigned char Day, unsigned char Hour, unsigned char Minute, unsigned char Second){

  unsigned char Command[12];
  unsigned char ConfirmCommand[2];

  Command[0] = Header;
  Command[1] = RTC_Set;
  Command[2] = Year;
  Command[3] = Month;
  Command[4] = Day;
  Command[5] = Hour;
  Command[6] = Minute;
  Command[7] = Second;
  Command[8] = Tail_A;
  Command[9] = Tail_B;
  Command[10] = Tail_C;
  Command[11] = Tail_D;

  Tx_UARTS0(uart0_filestream, &Command[0], 12);
  
  //Error Verification
  Rx_UARTS0 (uart0_filestream, &ConfirmCommand[0], 2);
  if(ConfirmCommand[0] == Erro_In_Exe_Command){
    printf("Erro in send Set RTC Function!\n");
    return 1;
  }
}

//Read RTC Function
int Read_RTC (int uart0_filestream, unsigned char *Year, unsigned char *Month, unsigned char *Day, unsigned char *Hour, unsigned char *Minute, unsigned char *Second){

  unsigned char Command[6];
  unsigned char Read[12];

  Command[0] = Header;
  Command[1] = RTC_Read;
  Command[2] = Tail_A;
  Command[3] = Tail_B;
  Command[4] = Tail_C;
  Command[5] = Tail_D;

  Tx_UARTS0(uart0_filestream, &Command[0], 6);
  
  //Reading data
  Rx_UARTS0 (uart0_filestream, &Read[0], 12);
  if(Read[0] == Erro_In_Exe_Command){
    printf("Erro in send Set RTC Function!\n");
    return 1;

  }
  else{
    *Year = Read[2];
    *Month = Read[3];
    *Day = Read[4];
    *Hour = Read[5];
    *Minute = Read[6];
    *Second = Read[7];
    Rx_UARTS0 (uart0_filestream, &Read[0], 2);
    if(Read[0] == Erro_In_Exe_Command){
      printf("Erro in send Set RTC Function!\n");
      return 1;

    }
    return 0;
  }

}

//Set Page Function
int Set_Page (int uart0_filestream, unsigned int Page){

  unsigned char Command[8];
  unsigned char ConfirmCommand[2];

  Command[0] = Header;
  Command[1] = Disp_Page;
  Command[2] = (unsigned char)(Page/256);
  Command[3] = (unsigned char)(Page%256);
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

// Function Search Line
Line* SearchLine(Line *InitialLine, int CounterBuffer, unsigned char Buffer[4]){

  Line *Last = InitialLine;
  while(Last != NULL){
    if(strncmp(&Buffer[0], Last->Number, CounterBuffer) == 0){
      return Last;
    }
    Last=Last->Next;
  }

  return NULL;

}

//Initialize Source Function
int InitializeFunction (GID *NewGID, Group **ActualGroup, Line **ActualLine, Destination **ActualDestination){
   
  int  Break = 0;
  unsigned char Aux[100] = XMLSourceDirectory;
  DIR *Directory = opendir(XMLSourceDirectory);
  struct dirent *DirectoryEntry;

  if (Directory == NULL){
  printf("Erro, don't be able to open USB Directory!\n" );
      return MainID;
  }

  //Loading Files
  while (((DirectoryEntry = readdir(Directory)) != NULL) && Break == 0 ){
    if(DirectoryEntry->d_type != DT_DIR){
      unsigned int lengthAux = strlen(DirectoryEntry->d_name)-4;
      if(strcmp(&DirectoryEntry->d_name[lengthAux],".xml") == 0){
        strcat(Aux, DirectoryEntry->d_name);
        if(LoadXMLSource(NewGID, Aux) == 0) Break = 1;

      }

    }

  }
  closedir(Directory);

  if(Break == 1){
    //Loading Initial Sources
    *ActualGroup = NewGID->List;
    *ActualLine = (*ActualGroup)->List;
    *ActualDestination = (*ActualLine)->List;
    return 0;

  }
  else{
    NewGID = NULL;
    *ActualGroup = NULL;
    *ActualLine = NULL;
    *ActualDestination = NULL;
    return 1;

  }
  
}

//Handler event for main Page
int HandlerMain (int uart0_filestream, Line **ActualLine, Destination **ActualDestination){
  
  Destination *Last = *ActualDestination;
  Button Bt;

  //Ploting info in display
  if((*ActualLine) != NULL){
    Write_String (uart0_filestream, AdressNumberLine, (*ActualLine)->Number);
    Write_String (uart0_filestream, AdressNameLine, (*ActualLine)->Name);
    Write_String (uart0_filestream, AdressNameDestination, (*ActualDestination)->Name);

  }

  Bt = Get_Buttom_Event (uart0_filestream);
  if(Bt.PagId == MainID){
    if(Bt.Direction == 0){
      switch (Bt.ButtonId){
        case ButtonNextDestination:
          if(Last != NULL){
            if(Last->Next != NULL) Last = Last->Next;
            *ActualDestination = Last;
            Write_String (uart0_filestream, AdressNameDestination, Last->Name);
          } 
          break;

        case ButtonPreviousDestination:
          if(Last != NULL){
            if(Last->Previous != NULL) Last = Last->Previous;
            *ActualDestination = Last;
             Write_String (uart0_filestream, AdressNameDestination, Last->Name);
          } 
          break;

        case ButtonChangeLine:
              return SelectLinesID;
              break;

        case ButtonSettings:
          return SettingsMainID;
          break;

        default:
          return MainID;
      }
    }

  }

  return MainID;

}

//Handler event for Select Destination
int HandlerSelectLines (int uart0_filestream, Line *InitialLine, Line **ActualLine, Destination **ActualDestination){
  
  Line *Last = *ActualLine;
  Line *FindLine = *ActualLine;
  int Break = -1;
  unsigned char Buffer[4];
  int CounterBuffer = 0;

  if((*ActualLine) != NULL){
    Write_String (uart0_filestream, AdressSelectNameLine, Last->Name);
    Write_String (uart0_filestream, AdressSelectIDLine, Last->Number);
  }

  while(Break < 0){
    Button Bt = Get_Buttom_Event (uart0_filestream);
    if((Bt.Direction == 0) && ((Bt.PagId == SelectLinesID) || (Bt.PagId == SelectLinesIDKeybord))){
      switch (Bt.ButtonId){
        case ButtonUpChangeLine:
          if(Last != NULL){
            if(Last->Next != NULL) Last = Last->Next;
            Write_String (uart0_filestream, AdressSelectNameLine, Last->Name);
            Write_String (uart0_filestream, AdressSelectIDLine, Last->Number);

          }
          Break = -1;
          break;

        case ButtonDownChangeLine:
          if(Last != NULL){
            if(Last->Previous != NULL) Last = Last->Previous;
            Write_String (uart0_filestream, AdressSelectNameLine, Last->Name);
            Write_String (uart0_filestream, AdressSelectIDLine, Last->Number);
          }
          Break = -1; 
          break;

        case ButtonHomeSelectDestinations:
          Break = MainID;
          break;

        case ButtonSettingsSelectDestinations:
          Break = SettingsMainID;
          break;

        case ButtonCancelChangeLine:
          Break = MainID;
          break;

        case ButtonConfirmChangeLine:
          if(Last != NULL){
            *ActualDestination = Last->List;
            *ActualLine = Last;
          }
          Break = MainID;
          break;

        case ButtonSettingsActiveKeyOk:
          if(Last != NULL){
            *ActualDestination = Last->List;
            *ActualLine = Last;
          }
          Break = MainID;
          break;

        case ButtonSettingsActiveKeyDel:
          if(CounterBuffer>0){
            Write_String (uart0_filestream, AdressSelectNameLine, "  ");
            Buffer[--CounterBuffer] = '\0';
            Write_String (uart0_filestream, AdressSelectIDLine, Buffer);
            FindLine = SearchLine(InitialLine, CounterBuffer, Buffer);
            if(FindLine != NULL){
              Last = FindLine;
              Write_String (uart0_filestream, AdressSelectNameLine, Last->Name);
            }
          }
          Break = -1;
          break;

        default:
          if(Bt.ButtonId > ButtonSettingsActiveKeyOk){
            if(CounterBuffer<3){
              Write_String (uart0_filestream, AdressSelectNameLine, "  ");
              Buffer[CounterBuffer++] = 39 + Bt.ButtonId;
              Buffer[CounterBuffer] = '\0';
              Write_String (uart0_filestream, AdressSelectIDLine, Buffer);
              FindLine = SearchLine(InitialLine, CounterBuffer, Buffer);
              if(FindLine != NULL){
                Last = FindLine;
                Write_String (uart0_filestream, AdressSelectNameLine, Last->Name);
              }
            }
          }
          Break = -1;

      }

    }
  }

  return Break;

}

//Handler event for Settings
int HandlerWatch(int uart0_filestream){

  int Break = -1;
  unsigned char Year = 1;
  unsigned char Month = 1;
  unsigned char Day = 18;
  unsigned char Hour;
  unsigned char Minute;
  unsigned char Second;
  unsigned char Date[] = "sudo date +\%Y\%m\%d -s \"20AAMMDD\"";
  unsigned char Time[] = "sudo date +\%T -s \"HH:MM:SS\"";

  //Reading info about date and time
  Read_RTC(uart0_filestream, &Year, &Month, &Day, &Hour, &Minute, &Second);
  //Writing information on screen
  Write_Integer16b(uart0_filestream, AdressDecHora, (int)(Hour/10));
  Write_Integer16b(uart0_filestream, AdressHora, (int)(Hour%10));
  Write_Integer16b(uart0_filestream, AdressDecMin, (int)(Minute/10));
  Write_Integer16b(uart0_filestream, AdressMin, (int)(Minute%10));

  //Reading Buttons
  while(Break < 0){
    Button Bt = Get_Buttom_Event(uart0_filestream);
    if((Bt.Direction == 0) && (Bt.PagId == WatchID || Bt.PagId == WatchID2)){
      switch (Bt.ButtonId){
        case ButtonCancelChangeWatch:
          Break = MainID;
          break;

        case ButtonConfirmChangeWatch:
          //Setting date and time in Display
          Set_RTC (uart0_filestream, Year, Month, Day, Hour, Minute, Second);
          //Setting date and time in Linux
          Date[24] = (unsigned int)((Year/10)+48);
          Date[25] = (unsigned int)((Year%10)+48);
          Date[26] = (unsigned int)((Month/10)+48);
          Date[27] = (unsigned int)((Month%10)+48);
          Date[28] = (unsigned int)((Day/10)+48);
          Date[29] = (unsigned int)((Day%10)+48);
          Time[18] = (unsigned int)((Hour/10)+48);
          Time[19] = (unsigned int)((Hour%10)+48);
          Time[21] = (unsigned int)((Minute/10)+48);
          Time[22] = (unsigned int)((Minute%10)+48);
          Time[24] = (unsigned int)((Second/10)+48);
          Time[25] = (unsigned int)((Second%10)+48);
          //Setting system date
          system(Time);
          system(Date);
          Break = MainID; 
          break;

        case ButtonHomeWatch:
          Break = MainID;
          break;

        case ButtonSettingsWatch:
          Break = SettingsMainID;
          break;

        case ButtonDownHora:
          if(Bt.PagId == WatchID){
            if(Hour > 0){
              Write_Integer16b(uart0_filestream, AdressDecHora, (int)(--Hour/10));
              Write_Integer16b(uart0_filestream, AdressHora, (int)(Hour%10));
            }
          }
          else{
            if(Month > 1){
              Write_Integer16b(uart0_filestream, AdressDecHora, (int)(--Month/10));
              Write_Integer16b(uart0_filestream, AdressHora, (int)(Month%10));

            }
          }
          Break = -1;
          break;

        case ButtonUpHora:
          if(Bt.PagId == WatchID){
            if(Hour < 23){
              Write_Integer16b(uart0_filestream, AdressDecHora, (int)(++Hour/10));
              Write_Integer16b(uart0_filestream, AdressHora, (int)(Hour%10));
            }
          }
          else{
            if(Month < 12){
              Write_Integer16b(uart0_filestream, AdressDecHora, (int)(++Month/10));
              Write_Integer16b(uart0_filestream, AdressHora, (int)(Month%10));

            }
          }
          Break = -1;
          break;

        case ButtonDownMin:
          if(Bt.PagId == WatchID){
            if(Minute > 0){
              Write_Integer16b(uart0_filestream, AdressDecMin, (int)(--Minute/10));
              Write_Integer16b(uart0_filestream, AdressMin, (int)(Minute%10));
            }
          }
          else{
            if(Day > 1){
              Write_Integer16b(uart0_filestream, AdressDecMin, (int)(--Day/10));
              Write_Integer16b(uart0_filestream, AdressMin, (int)(Day%10));

            }
          }
          Break = -1;
          break;

        case ButtonUpMin:
          if(Bt.PagId == WatchID){
            if(Minute < 59){
              Write_Integer16b(uart0_filestream, AdressDecMin, (int)(++Minute/10));
              Write_Integer16b(uart0_filestream, AdressMin, (int)(Minute%10));
            }
          }
          else{
            if(Day < 31){
              Write_Integer16b(uart0_filestream, AdressDecMin, (int)(++Day/10));
              Write_Integer16b(uart0_filestream, AdressMin, (int)(Day%10));

            }
          }
          Break = -1;
          break;

        case ButtonDownYear:
          if(Year > 0){
            Write_Integer16b(uart0_filestream, AdressDecYear, (int)(--Year/10));
            Write_Integer16b(uart0_filestream, AdressYear, (int)(Year%10));
          }
          Break = -1;
          break;

        case ButtonUpYear:
          if(Year < 99){
            Write_Integer16b(uart0_filestream, AdressDecYear, (int)(++Year/10));
            Write_Integer16b(uart0_filestream, AdressYear, (int)(Year%10));
          }
          Break = -1;
          break;

        case ButtonChangeDateTime:
          if(Bt.PagId == WatchID){
            //Setting news imagens
            Write_Integer16b(uart0_filestream, AdressDecHora, (int)(Month/10));
            Write_Integer16b(uart0_filestream, AdressHora, (int)(Month%10));
            Write_Integer16b(uart0_filestream, AdressDecMin, (int)(Day/10));
            Write_Integer16b(uart0_filestream, AdressMin, (int)(Day%10));
            Write_Integer16b(uart0_filestream, AdressDecDec, 2);
            Write_Integer16b(uart0_filestream, AdressDec, 0);
            Write_Integer16b(uart0_filestream, AdressDecYear, (int)(Year/10));
            Write_Integer16b(uart0_filestream, AdressYear, (int)(Year%10));
            //Setting page WatchID2
            Set_Page (uart0_filestream, WatchID2);
            Get_Buttom_Event(uart0_filestream);

          }
          else{
            //Setting news imagens
            Write_Integer16b(uart0_filestream, AdressDecHora, (int)(Hour/10));
            Write_Integer16b(uart0_filestream, AdressHora, (int)(Hour%10));
            Write_Integer16b(uart0_filestream, AdressDecMin, (int)(Minute/10));
            Write_Integer16b(uart0_filestream, AdressMin, (int)(Minute%10));
            //Setting page WatchID
            Set_Page (uart0_filestream, WatchID);
            Get_Buttom_Event(uart0_filestream);

          } 
          Break = -1;
          break;

        default:
          Break = -1;

      }

    }
  }

  return Break;

}

//Handler event for Settings
int HandlerSettingsMain(int uart0_filestream){

  int Break = -1;

  while(Break < 0){
    Button Bt = Get_Buttom_Event (uart0_filestream);
    if((Bt.Direction == 0) && Bt.PagId == SettingsMainID){
      switch (Bt.ButtonId){
        case ButtonCancelSettings:
          Break = MainID;
          break;

        case ButtonSelectWatch:
          Break = WatchID; 
          break;

        case ButtonSelectSource:
          Break = SourceIDA;
          break;

        case ButtonHomeSettings:
          Break = MainID;
          break;

        default:
          Break = -1;

      }

    }
  }

  return Break;

}

//Handler event for Source
int HandlerSource (int uart0_filestream, Group **ActualGroup, Line **ActualLine, Destination **ActualDestination){
  
  int Break = -1;
  int cont;
  unsigned char Location[100];
  Direct *InitialWindows = NULL;
  Direct *FinalWindows = NULL;
  Direct *Aux = NULL;
  struct dirent *DirectoryEntry;
  int CounterDirect = 0;

  //Default Directory
  DIR *Directory = opendir(XMLSourceDirectory);

  if (Directory == NULL){
      printf("Erro, don't be able to open this directory!\n" );
      return MainID;
  }

  //Loading Files
  while ((DirectoryEntry = readdir(Directory)) != NULL){
    if(strcmp(DirectoryEntry->d_name,".") != 0 && strcmp(DirectoryEntry->d_name,"..") != 0){
      FinalWindows = (Direct *) malloc(sizeof(Direct));
      FinalWindows->Name = DirectoryEntry->d_name;
      if(Aux != NULL) Aux->Next = FinalWindows;
      else InitialWindows = FinalWindows;
      FinalWindows->Next = NULL;
      FinalWindows->Previous = Aux;
      Aux = FinalWindows;
      CounterDirect++; 
    }

  }
  closedir(Directory);

  //Allocation pointers
  FinalWindows = InitialWindows;
  for(cont=0; (cont<(LimitOfLinesInSource-1) && cont<(CounterDirect-1)); cont++){
   FinalWindows = FinalWindows->Next;
  }

  //Loading Start List
  Aux = InitialWindows;
  int StartAdress = AdressInitialSource;
  for(cont=0; (cont<LimitOfLinesInSource && cont<CounterDirect); cont++){
    Write_String (uart0_filestream, StartAdress, Aux->Name);
    StartAdress = StartAdress + 0x000080;
    Aux = Aux->Next;
  }
  for(; cont<LimitOfLinesInSource; cont++){
    Write_String (uart0_filestream, StartAdress, " ");
    StartAdress = StartAdress + 0x000080;

  }

  // Enable Buttons and disable Loading screen
  Write_Integer16b (uart0_filestream, Loading, 0);
  Write_Integer16b (uart0_filestream, Signals, 0);
  Write_Integer16b (uart0_filestream, Status, 1);

  //Forced debounce
  Get_Buttom_Event (uart0_filestream);

  //--Function for buttons 
  while(Break < 0){
    Button Bt = Get_Buttom_Event (uart0_filestream);
    if((Bt.Direction == 0) && ((Bt.PagId == SourceIDA)||(Bt.PagId == SourceIDB)||(Bt.PagId == SourceIDC)||(Bt.PagId == SourceIDD)||(Bt.PagId == SourceIDE))){
      switch (Bt.ButtonId){
        case ButtonCancelChangeSource:
          Break = MainID;
          break;

        case ButtonConfirmChangeSource:
          // Disable Buttons and enable Loading screen
          Write_Integer16b (uart0_filestream, Loading, 1);
          Write_Integer16b (uart0_filestream, Signals, 0);
          Write_Integer16b (uart0_filestream, Status, 0);
          //Loading Default Xml file
          Aux = InitialWindows;
          for(cont = 0; cont<(Bt.PagId-SourceIDA); cont++) Aux = Aux->Next;
          Location[0] = '\0';
          strcat(Location, XMLSourceDirectory);
          strcat(Location, Aux->Name);
          //Loading Initial Sources
          GID NewGID;
          if(LoadXMLSource(&NewGID, Location) == 1){
            printf("Error in Loading Source %s\n", Location);
            //Error, plot icon error
            Write_Integer16b (uart0_filestream, Loading, 0);
            Write_Integer16b (uart0_filestream, Signals, 2);
            Write_Integer16b (uart0_filestream, Status, 0);
            sleep(1);
            Write_Integer16b (uart0_filestream, Loading, 0);
            Write_Integer16b (uart0_filestream, Signals, 0);
            Write_Integer16b (uart0_filestream, Status, 1);
            Break = -1;

          }
          else{
            *ActualGroup = NewGID.List;
            *ActualLine = NewGID.List->List;
            *ActualDestination = NewGID.List->List->List;
            //free allocations
            while(InitialWindows != NULL){
              Aux = InitialWindows;
              InitialWindows = InitialWindows->Next;
              free(Aux);
            } 
            sleep(1);
            Write_Integer16b (uart0_filestream, Loading, 0);
            Write_Integer16b (uart0_filestream, Signals, 0);
            Write_Integer16b (uart0_filestream, Status, 1);
            Break = MainID;
          }
           
          break;

        case ButtonNewSource:
          //free allocations
          while(InitialWindows != NULL){
            Aux = InitialWindows;
            InitialWindows = InitialWindows->Next;
            free(Aux);
          }  
          Break = EditorArquivoID;
          break;

        case ButtonUSBImport:
          //free allocations
          while(InitialWindows != NULL){
            Aux = InitialWindows;
            InitialWindows = InitialWindows->Next;
            free(Aux);
          } 
          Break = ImportUSBIDA;
          break;

        case ButtonSettingsSource:
          //free allocations
          while(InitialWindows != NULL){
            Aux = InitialWindows;
            InitialWindows = InitialWindows->Next;
            free(Aux);
          } 
          Break = SettingsMainID;
          break;

        case ButtonHomeSource:
          //free allocations
          while(InitialWindows != NULL){
            Aux = InitialWindows;
            InitialWindows = InitialWindows->Next;
            free(Aux);
          } 
        Break = MainID;
          break;

        case ButtonUpSource:
          if(Bt.PagId == SourceIDA){
            if(InitialWindows->Previous != NULL){
              Aux = InitialWindows = InitialWindows->Previous;
              StartAdress = AdressInitialSource;
              for(cont=0; (cont<LimitOfLinesInSource && cont<CounterDirect); cont++){
                Write_String (uart0_filestream, StartAdress, Aux->Name);
                StartAdress = StartAdress + 0x000080;
                Aux = Aux->Next;

              }
              FinalWindows = FinalWindows->Previous;  
            }
          }
          else{
            Set_Page(uart0_filestream, Bt.PagId-1);
            Get_Buttom_Event (uart0_filestream);
          }
          Break = -1;
          break;

        case ButtonDownSource:
          if(Bt.PagId == SourceIDE){
            if(FinalWindows->Next != NULL){
              Aux = InitialWindows = InitialWindows->Next;
              StartAdress = AdressInitialSource;
              for(cont=0; (cont<LimitOfLinesInSource && cont<CounterDirect); cont++){
                Write_String (uart0_filestream, StartAdress, Aux->Name);
                StartAdress = StartAdress + 0x000080;
                Aux = Aux->Next;

              }
              FinalWindows = FinalWindows->Next;
            }
          }
          else{
            if((Bt.PagId-SourceIDA) < CounterDirect-1){
              Set_Page(uart0_filestream, Bt.PagId+1);
              Get_Buttom_Event (uart0_filestream);
            }
          } 
          Break = -1;
          break;

        default:
          if(Bt.ButtonId > (ButtonTouchA-1) && ((Bt.ButtonId-ButtonTouchA) < CounterDirect)){
            Set_Page(uart0_filestream, ((Bt.ButtonId-ButtonTouchA)+SourceIDA));
            Get_Buttom_Event (uart0_filestream);
          }
          Break = -1;

      }

    }
  }

  return Break;

}

//Handler event for Import Xml Files 
int HandlerImportUSB(int uart0_filestream){

  int Break = -1;
  int cont;
  int conte;
  Direct *InitialWindows;
  Direct *FinalWindows;
  Direct *Aux;
  unsigned char AuxDirec[ScreenTitleLength];
  unsigned char Location[100] = "/media/usb/";
  unsigned char CommandA[100] = "sudo mount /dev/sda1 /media/usb -o uid=pi,gid=pi";
  unsigned char ActualOp = 0; 
  struct dirent *DirectoryEntry;
  unsigned int lengthAux;
  int CounterDirect;
  int StartAdress;

  //Cleaning display
  for(cont=0; cont<LimitOfLinesInImportUSB; cont++){
    Write_String (uart0_filestream, StartAdress, " ");
    StartAdress = StartAdress + 0x000080;
  }

  // Loading Icon
  Write_Integer16b (uart0_filestream, Loading, 1);
  Write_Integer16b (uart0_filestream, Signals, 0);
  Write_Integer16b (uart0_filestream, Status, 0);

  //Mount USB
  int MountedConfirm = system(CommandA);
  //Confirm mounting
  if(MountedConfirm != 0){
    //Unmont usb
    CommandA[0] = '\0';
    strcpy(CommandA, "sudo umount /media/usb");
    system(CommandA);
    //Mensage Error
    Write_Integer16b (uart0_filestream, Loading, 0);
    Write_Integer16b (uart0_filestream, Signals, 1);
    Write_Integer16b (uart0_filestream, Status, 0);
    sleep(2);
    //Return 
    Write_Integer16b (uart0_filestream, Loading, 0);
    Write_Integer16b (uart0_filestream, Signals, 0);
    Write_Integer16b (uart0_filestream, Status, 1);
    return(SourceIDA);

  }
  else sleep(1);

  // ---- Process to Loading Directories 
  //Default Directory
  DIR *Directory = opendir(USBDirectory);
  if (Directory == NULL){
      printf("Erro, don't be able to open USB Directory!\n" );
      return MainID;
  }
  //Loading Files
  Aux = InitialWindows = FinalWindows = NULL;
  CounterDirect = 0;
  while ((DirectoryEntry = readdir(Directory)) != NULL){
    if(strcmp(DirectoryEntry->d_name,".") != 0){
      FinalWindows = (Direct *) malloc(sizeof(Direct));
      FinalWindows->Name = DirectoryEntry->d_name;
      if(DirectoryEntry->d_type == DT_DIR) FinalWindows->Type = 1;
      else FinalWindows->Type = 0; 
      if(Aux != NULL) Aux->Next = FinalWindows;
      else InitialWindows = FinalWindows;
      FinalWindows->Next = NULL;
      FinalWindows->Previous = Aux;
      Aux = FinalWindows;
      CounterDirect++; 
    }

  }
  closedir(Directory);
  //Allocation pointers
  FinalWindows = InitialWindows;
  for(cont=0; (cont<(LimitOfLinesInImportUSB-1) && cont<(CounterDirect-1)); cont++){
   FinalWindows = FinalWindows->Next;
  }
  //Loading Start List
  Aux = InitialWindows;
  StartAdress = AdressInitialImportUSB;
  for(cont=0; (cont<LimitOfLinesInImportUSB && cont<CounterDirect); cont++){
    strcpy(AuxDirec, Aux->Name);
    Write_String (uart0_filestream, StartAdress, AuxDirec);
    StartAdress = StartAdress + 0x000080;
    Aux = Aux->Next;
  }
  for(; cont<LimitOfLinesInImportUSB; cont++){
    Write_String (uart0_filestream, StartAdress, " ");
    StartAdress = StartAdress + 0x000080;
  }

  //---- Enable Buttons and disable Loading screen
  Write_Integer16b (uart0_filestream, Loading, 0);
  Write_Integer16b (uart0_filestream, Signals, 0);
  Write_Integer16b (uart0_filestream, Status, 1);

  //---- Process to Reading Buttons
  while(Break < 0){
    Button Bt = Get_Buttom_Event (uart0_filestream);
    if((Bt.Direction == 0) && ((Bt.PagId-ImportUSBIDA) == ActualOp)){
      //Select Buttons
      switch (Bt.ButtonId){
        case ButtonCancelUSB:
          // -- Unmont usb
          CommandA[0] = '\0';
          strcpy(CommandA, "sudo umount /media/usb");
          system(CommandA);
          // -- free allocations
          while(InitialWindows != NULL){
            Aux = InitialWindows;
            InitialWindows = InitialWindows->Next;
            free(Aux);
          } 
          Break = SourceIDA;
          break;

        case ButtonImportUSB:
          // -- Find Reference Number of Touch Key Path
          Aux = InitialWindows;
          for(cont = 0; cont<ActualOp; cont++) Aux = Aux->Next;
          // -- Different actions for Directory and Files
          if(Aux->Type != 1){
            lengthAux = strlen(Aux->Name)-4;
            if(strcmp(&Aux->Name[lengthAux], ".xml") == 0){
              //Copings files to default folder
              CommandA[0] = '\0';
              strcat(CommandA, "sudo cp ");
              conte = 8;
              for(cont = 0; (cont<100 && Location[cont] != '\0'); cont++){
                if(Location[cont] == ' ')  CommandA[conte++] = 0x5C;
                CommandA[conte++] = Location[cont];
              }
              CommandA[conte] = '\0';
              strcat(CommandA, Aux->Name);
              strcat(CommandA, " ");
              strcat(CommandA, XMLSourceDirectory);
              system(CommandA);
              //Loading Icon
              Write_Integer16b (uart0_filestream, Loading, 1);
              Write_Integer16b (uart0_filestream, Signals, 0);
              Write_Integer16b (uart0_filestream, Status, 0);
              sleep(1);
              Write_Integer16b (uart0_filestream, Loading, 0);
              Write_Integer16b (uart0_filestream, Signals, 0);
              Write_Integer16b (uart0_filestream, Status, 1);
              //Unmont usb
              CommandA[0] = '\0';
              strcpy(CommandA, "sudo umount /media/usb");
              system(CommandA);
              //free allocations
              while(InitialWindows != NULL){
                Aux = InitialWindows;
                InitialWindows = InitialWindows->Next;
                free(Aux);
              }
              //Return 
              Break = SourceIDA;

            }
            else{
              // Erro Icon
              Write_Integer16b (uart0_filestream, Loading, 0);
              Write_Integer16b (uart0_filestream, Signals, 2);
              Write_Integer16b (uart0_filestream, Status, 0);
              sleep(2);
              Write_Integer16b (uart0_filestream, Loading, 0);
              Write_Integer16b (uart0_filestream, Signals, 0);
              Write_Integer16b (uart0_filestream, Status, 1);
              Break = -1;
            }
          }
          else{
            // -- Select Path
            if(strcmp(Aux->Name,"..") == 0){
              for(cont = strlen(Location)-2; (cont>10 && Location[cont] != '/'); cont--) Location[cont] = '\0';
            }
            else{
              strcat(Location, Aux->Name);
              strcat(Location, "/");
            }
            printf("New Location : %s\n", Location);
            // -- free allocations
            while(InitialWindows != NULL){
              Aux = InitialWindows;
              InitialWindows = InitialWindows->Next;
              free(Aux);
            }
            // -- Process to Loading Directories 
            // -- Reloading Directory
            DIR *Directory = opendir(Location);
            if (Directory == NULL){
                printf("Erro, don't be able to open USB Directory!\n" );
                return MainID;
            }
            // -- Loading Files
            Aux = InitialWindows = FinalWindows = NULL;
            CounterDirect = 0;
            while ((DirectoryEntry = readdir(Directory)) != NULL){
              if(strcmp(DirectoryEntry->d_name,".") != 0){
                FinalWindows = (Direct *) malloc(sizeof(Direct));
                FinalWindows->Name = DirectoryEntry->d_name;
                if(DirectoryEntry->d_type == DT_DIR) FinalWindows->Type = 1;
                else FinalWindows->Type = 0; 
                if(Aux != NULL) Aux->Next = FinalWindows;
                else InitialWindows = FinalWindows;
                FinalWindows->Next = NULL;
                FinalWindows->Previous = Aux;
                Aux = FinalWindows;
                CounterDirect++; 
              }

            }
            closedir(Directory);
            printf("Novo diretorio tem um tamanho de %d\n", CounterDirect);
            // -- Allocation pointers
            FinalWindows = InitialWindows;
            for(cont=0; (cont<(LimitOfLinesInImportUSB-1) && cont<(CounterDirect-1)); cont++){
             FinalWindows = FinalWindows->Next;
            }
            // -- Loading Start List
            Aux = InitialWindows;
            int StartAdress = AdressInitialImportUSB;
            for(cont=0; (cont<LimitOfLinesInImportUSB && cont<CounterDirect); cont++){
              strcpy(AuxDirec, Aux->Name);
              Write_String (uart0_filestream, StartAdress, AuxDirec);
              StartAdress = StartAdress + 0x000080;
              Aux = Aux->Next;
            }
            for(; cont<LimitOfLinesInImportUSB; cont++){
              Write_String (uart0_filestream, StartAdress, " ");
              StartAdress = StartAdress + 0x000080;
            }
            // -- Reset Page Import USB
            Set_Page(uart0_filestream, ImportUSBIDA);
            // -- Sync Actual Option
            ActualOp = 0;
            // -- Debounce forced
            Get_Buttom_Event (uart0_filestream);
            // -- Return
            Break = -1;
          }
          break;

        case ButtonSettingUSB:
          // -- Unmont usb
          CommandA[0] = '\0';
          strcpy(CommandA, "sudo umount /media/usb");
          system(CommandA);
          // -- free allocations
          while(InitialWindows != NULL){
            Aux = InitialWindows;
            InitialWindows = InitialWindows->Next;
            free(Aux);
          }
          // -- Return
          Break = SettingsMainID;
          break;

        case ButtonHomeUSB:
          // -- Unmont usb
          CommandA[0] = '\0';
          strcpy(CommandA, "sudo umount /media/usb");
          system(CommandA);
          // -- free allocations
          while(InitialWindows != NULL){
            Aux = InitialWindows;
            InitialWindows = InitialWindows->Next;
            free(Aux);
          }
          // -- Return
          Break = MainID;
          break;

        case ButtonUpUSB:
          if(Bt.PagId == ImportUSBIDA){
            if(InitialWindows->Previous != NULL){
              Aux = InitialWindows = InitialWindows->Previous;
              StartAdress = AdressInitialImportUSB;
              for(cont=0; (cont<LimitOfLinesInImportUSB && cont<CounterDirect); cont++){
                Write_String (uart0_filestream, StartAdress, Aux->Name);
                StartAdress = StartAdress + 0x000080;
                Aux = Aux->Next;

              }
              FinalWindows = FinalWindows->Previous;  
            }
          }
          else{
            // -- Select page
            Set_Page(uart0_filestream, Bt.PagId-1);
            // -- Sync Actual Option
            Get_Buttom_Event (uart0_filestream);
            ActualOp = (Bt.PagId-1-ImportUSBIDA);
          }
          Break = -1;
          break;

        case ButtonDownUSB:
          if(Bt.PagId == ImportUSBIDG){
            if(FinalWindows->Next != NULL){
              Aux = InitialWindows = InitialWindows->Next;
              StartAdress = AdressInitialImportUSB;
              for(cont=0; (cont<LimitOfLinesInImportUSB && cont<CounterDirect); cont++){
                Write_String (uart0_filestream, StartAdress, Aux->Name);
                StartAdress = StartAdress + 0x000080;
                Aux = Aux->Next;

              }
              FinalWindows = FinalWindows->Next;
            }
          }
          else{
            if((Bt.PagId-ImportUSBIDA) < CounterDirect-1){
              // Select page
              Set_Page(uart0_filestream, Bt.PagId+1);
              // -- Sync Actual Option
              ActualOp = (Bt.PagId+1-ImportUSBIDA);
              // -- Debounce forced
              Get_Buttom_Event (uart0_filestream);
            }
          }
          Break = -1;
          break;

        default:
          if(Bt.ButtonId > (ButtonTouchUSBA-1) && ((Bt.ButtonId-ButtonTouchUSBA) < CounterDirect)){
            if((Bt.ButtonId-ButtonTouchUSBA) == ActualOp){
              // -- Find Reference Number of Touch Key Path
              Aux = InitialWindows;
              for(cont = 0; cont<(ActualOp); cont++) Aux = Aux->Next;
              // -- Different action about directories
              if(Aux->Type == 1){
                if(strcmp(Aux->Name,"..") == 0){
                  for(cont = strlen(Location)-2; (cont>10 && Location[cont] != '/'); cont--) Location[cont] = '\0';
                }
                else{
                  strcat(Location, Aux->Name);
                  strcat(Location, "/");
                }
                // -- free allocations
                while(InitialWindows != NULL){
                  Aux = InitialWindows;
                  InitialWindows = InitialWindows->Next;
                  free(Aux);
                }
                // -- Reloading File List
                DIR *Directory = opendir(Location);
                if (Directory == NULL){
                    printf("Erro, don't be able to open USB Directory!\n" );
                    return MainID;
                }
                // -- Loading Files
                Aux = InitialWindows = FinalWindows = NULL;
                CounterDirect = 0;
                while ((DirectoryEntry = readdir(Directory)) != NULL){
                  if(strcmp(DirectoryEntry->d_name,".") != 0){
                    FinalWindows = (Direct *) malloc(sizeof(Direct));
                    FinalWindows->Name = DirectoryEntry->d_name;
                    if(DirectoryEntry->d_type == DT_DIR) FinalWindows->Type = 1;
                    else FinalWindows->Type = 0; 
                    if(Aux != NULL) Aux->Next = FinalWindows;
                    else InitialWindows = FinalWindows;
                    FinalWindows->Next = NULL;
                    FinalWindows->Previous = Aux;
                    Aux = FinalWindows;
                    CounterDirect++; 
                  }

                }
                closedir(Directory);
                // -- Allocation pointers
                FinalWindows = InitialWindows;
                for(cont=0; (cont<(LimitOfLinesInImportUSB-1) && cont<(CounterDirect-1)); cont++){
                 FinalWindows = FinalWindows->Next;
                }
                // -- Loading Start List
                Aux = InitialWindows;
                StartAdress = AdressInitialImportUSB;
                for(cont=0; (cont<LimitOfLinesInImportUSB && cont<CounterDirect); cont++){
                  strcpy(AuxDirec, Aux->Name);
                  Write_String (uart0_filestream, StartAdress, AuxDirec);
                  StartAdress = StartAdress + 0x000080;
                  Aux = Aux->Next;
                }
                for(; cont<LimitOfLinesInImportUSB; cont++){
                  Write_String (uart0_filestream, StartAdress, " ");
                  StartAdress = StartAdress + 0x000080;
                }
                // -- Reset Page Import USB
                Set_Page(uart0_filestream, ImportUSBIDA);
                // Sync  Actual Option
                ActualOp = 0;
                // -- Debounce forced
                Get_Buttom_Event (uart0_filestream);
              }
            }
            else{
              // -- Select page
              Set_Page(uart0_filestream, ((Bt.ButtonId-ButtonTouchUSBA)+ImportUSBIDA));
              // Sync  Actual Option
              ActualOp = (Bt.ButtonId-ButtonTouchUSBA);
              // -- Debounce forced
              Get_Buttom_Event (uart0_filestream);
            }
            
          }
          Break = -1;

      }

    }
  }

  return Break;

}

//Handler event for Editor
int HandlerEditor(int uart0_filestream, Group **NewGroup){

  unsigned char Name[30];
  int position = 0;
  int Break = -1;

  Name[0]= '\0';

  //Cleanig display
  Write_String (uart0_filestream, AdressFileName, " ");

  //Forced debounce
  Get_Buttom_Event (uart0_filestream);

  //---- Process to Reading Buttons
  while(Break < 0){
    Button Bt = Get_Buttom_Event (uart0_filestream);
    if((Bt.Direction == 0) && ((Bt.PagId == EditorArquivoID) || (Bt.PagId == EditorArquivoIDKeyboard) || (Bt.PagId == EditorArquivoIDKeyboardCapsLock) || (Bt.PagId == EditorArquivoIDKeyboardOp))){
      //Select Buttons
      switch(Bt.ButtonId){
        case ButtonCancelarArquivo:
          // -- Return
          Break = SourceIDA;
          break;

        case ButtonProximoArquivo:
          // -- Return
          if(position > 0){
            //Crete a new Group
            *NewGroup = (Group *)malloc(sizeof(Group));
            strcpy((*NewGroup)->ID, Name);
            Break = EditorLinhaID;
              
          }
          else{
            //Mensage Error
            Write_Integer16b (uart0_filestream, Loading, 0);
            Write_Integer16b (uart0_filestream, Signals, 2);
            Write_Integer16b (uart0_filestream, Status, 0);
            sleep(2);
            //Return 
            Write_Integer16b (uart0_filestream, Loading, 0);
            Write_Integer16b (uart0_filestream, Signals, 0);
            Write_Integer16b (uart0_filestream, Status, 1);
            Break = -1;
          }
          
          break;

        case ButtonSettingsArquivo:
          // -- Return
          Break = SettingsMainID;
          break;

        case ButtonHomeArquivo:
          // -- Return
          Break = MainID;
          break;

        case ButtonKeyboardArquivo:
          // -- Reset Page Import USB
          Set_Page(uart0_filestream, EditorArquivoIDKeyboard);
          // -- Debounce forced
          Get_Buttom_Event (uart0_filestream);
          // -- Return
          Break = -1;
          break;

        case KeyboardEnter:
          // -- Reset Page Import USB
          Set_Page(uart0_filestream, EditorArquivoID);
          // -- Debounce forced
          Get_Buttom_Event (uart0_filestream);
          // -- Return
          Break = -1;
          break;

        case KeyboardBackSpace:
          if(position > 0) Name[--position] = '\0';
          else Name[0] = '\0';
          //writing in display
          Write_String (uart0_filestream, AdressFileName, Name);
          Break = -1;
          break;

        case KeyboardShift:
          if(Bt.PagId != EditorArquivoIDKeyboardCapsLock){
            // -- Set to page with Caps Lock
            Set_Page(uart0_filestream, EditorArquivoIDKeyboardCapsLock);
            // -- Debounce forced
            Get_Buttom_Event (uart0_filestream);

          }
          else{
            // -- Set to page without Caps Lock
            Set_Page(uart0_filestream, EditorArquivoIDKeyboard);
            // -- Debounce forced
            Get_Buttom_Event (uart0_filestream);

          }
          Break = -1;
          break;

        case KeyboardOp:
          if(Bt.PagId != EditorArquivoIDKeyboardOp){
            // -- Set to page with Optional simbols
            Set_Page(uart0_filestream, EditorArquivoIDKeyboardOp);
            // -- Debounce forced
            Get_Buttom_Event (uart0_filestream);

          }
          else{
            // -- Set to page with convencional simbols
            Set_Page(uart0_filestream, EditorArquivoIDKeyboard);
            // -- Debounce forced
            Get_Buttom_Event (uart0_filestream);
         
          }
          Break = -1;
          break;

        default:
          if(position < ScreenTitleLength){
            Name[position++] = Bt.ButtonId;
            Name[position] = '\0';
            //writing in display
            Write_String (uart0_filestream, AdressFileName, Name);

          }
          Break = -1;

      }

    }
    
  }

  return Break;

}

//Handler event for Editor Make Line
int HandlerEditorLine(int uart0_filestream, Group *NewGroup){

  unsigned char NameLine[30];
  unsigned char IDLine[5];
  int positionName = 0;
  int positionID = 0;
  unsigned char Select = 0;
  Line *LastLine =  NULL;
  Line *ActualLine =  NULL;
  int Break = -1;

  NameLine[0]= '\0';
  IDLine[0] = '\0'; 

  //Cleanig display
  Write_String (uart0_filestream, AdressLineName, " ");
  Write_String (uart0_filestream, AdressLineNumber, " ");

  //Forced debounce
  Get_Buttom_Event (uart0_filestream);

  //---- Process to Reading Buttons
  while(Break < 0){
    Button Bt = Get_Buttom_Event (uart0_filestream);
    if((Bt.Direction == 0) && ((Bt.PagId == EditorLinhaID) || (Bt.PagId == EditorLinhaIDKeyboard) || (Bt.PagId == EditorLinhaIDKeyboardCapsLock) || (Bt.PagId == EditorLinhaIDOp))){
      //Select Buttons
      switch(Bt.ButtonId){
        case ButtonCancelarEditorLinha:
          // -- Return
          Break = SourceIDA;
          break;

        case ButtonProximoEditorLinha:
          if((positionName > 0) && (positionID > 0)){
            //Crete a new line
            ActualLine = (Line *)malloc(sizeof(Line));
            strcpy(ActualLine->Number, IDLine);
            strcpy(ActualLine->Name, NameLine);
            LastLine = NewGroup->List;
            if(LastLine != NULL){
              while(LastLine->Next != NULL) LastLine = LastLine->Next;
              LastLine->Next = ActualLine;
            }
            else NewGroup->List = ActualLine;
            ActualLine->Next = NULL;
            ActualLine->Previous = LastLine;
            Break = EditorDestinoID;
              
          }
          else{
            //Mensage Error
            Write_Integer16b (uart0_filestream, Loading, 0);
            Write_Integer16b (uart0_filestream, Signals, 2);
            Write_Integer16b (uart0_filestream, Status, 0);
            sleep(2);
            //Return 
            Write_Integer16b (uart0_filestream, Loading, 0);
            Write_Integer16b (uart0_filestream, Signals, 0);
            Write_Integer16b (uart0_filestream, Status, 1);
            Break = -1;
          }
          break;

        case ButtonSettingsEditorLinha:
          // -- Return
          Break = SettingsMainID;
          break;

        case ButtonHomeEditorLinha:
          // -- Return
          Break = MainID;
          break;

        case ButtonKeyboardNameEditorLinha:
          Select = 0;
          // -- Reset Page Import USB
          Set_Page(uart0_filestream, EditorLinhaIDKeyboard);
          // -- Debounce forced
          Get_Buttom_Event (uart0_filestream);
          // -- Return
          Break = -1;
          break;

        case ButtonKeyboardNumberEditorLinha:
          Select = 1;
          // -- Reset Page Import USB
          Set_Page(uart0_filestream, EditorLinhaIDKeyboard);
          // -- Debounce forced
          Get_Buttom_Event (uart0_filestream);
          // -- Return
          Break = -1;
          break;

        case KeyboardEnter:
          // -- Reset Page Import USB
          Set_Page(uart0_filestream, EditorLinhaID);
          // -- Debounce forced
          Get_Buttom_Event (uart0_filestream);
          // -- Return
          Break = -1;
          break;

        case KeyboardBackSpace:
          if(Select == 1){
            if(positionID > 0) IDLine[--positionID] = '\0';
            else IDLine[0] = '\0';
            //writing in display
            Write_String (uart0_filestream, AdressLineNumber, IDLine);
          }
          else{
            if(positionName > 0) NameLine[--positionName] = '\0';
            else NameLine[0] = '\0';
            //writing in display
            Write_String (uart0_filestream, AdressLineName, NameLine);

          }
          Break = -1;
          break;

        case KeyboardShift:
          if(Bt.PagId != EditorLinhaIDKeyboardCapsLock){
            // -- Set to page with Caps Lock
            Set_Page(uart0_filestream, EditorLinhaIDKeyboardCapsLock);
            // -- Debounce forced
            Get_Buttom_Event (uart0_filestream);

          }
          else{
            // -- Set to page without Caps Lock
            Set_Page(uart0_filestream, EditorLinhaIDKeyboard);
            // -- Debounce forced
            Get_Buttom_Event (uart0_filestream);

          }
          Break = -1;
          break;

        case KeyboardOp:
          if(Bt.PagId != EditorLinhaIDOp){
            // -- Set to page with Optional simbols
            Set_Page(uart0_filestream, EditorLinhaIDOp);
            // -- Debounce forced
            Get_Buttom_Event (uart0_filestream);

          }
          else{
            // -- Set to page with convencional simbols
            Set_Page(uart0_filestream, EditorLinhaIDKeyboard);
            // -- Debounce forced
            Get_Buttom_Event (uart0_filestream);
         
          }
          Break = -1;
          break;

        default:
          if(Select == 1){
            if(positionID < 4){
              IDLine[positionID++] = Bt.ButtonId;
              IDLine[positionID] = '\0';
              //writing in display
              Write_String (uart0_filestream, AdressLineNumber, IDLine);

            }
          }
          else{
            if(positionName < ScreenTitleLength){
              NameLine[positionName++] = Bt.ButtonId;
              NameLine[positionName] = '\0';
              //writing in display
              Write_String (uart0_filestream, AdressLineName, NameLine);

            }
          }
          Break = -1;

      }

    }
    
  }

  return Break;

}

//Handler event for Editor Make Destination
int HandlerEditorDestinations(int uart0_filestream, Group *NewGroup){


  unsigned char Name[30];
  int position = 0;
  int Break = -1;
  Line *LastLine =  NULL;
  Destination *LastDestination =  NULL;
  Destination *ActualDestination =  NULL;

  Name[0]= '\0';

  //Cleanig display
  Write_String (uart0_filestream, AdressDestinationName, " ");

  //Forced debounce
  Get_Buttom_Event (uart0_filestream);

  //---- Process to Reading Buttons
  while(Break < 0){
    Button Bt = Get_Buttom_Event (uart0_filestream);
    if((Bt.Direction == 0) && ((Bt.PagId == EditorDestinoID) || (Bt.PagId == EditorDestinoIDKeyboard) || (Bt.PagId == EditorDestinoIDKeyboardCapsLock) || (Bt.PagId == EditorDestinoIDOp))){
      //Select Buttons
      switch(Bt.ButtonId){
        
        case ButtonCancelarEditorDestino:
          Break = SourceIDA;
          break;

        case ButtonProximoEditorDestino:
          if(position > 0){
            //Crete a new line
            ActualDestination = (Destination *)malloc(sizeof(Destination));
            strcpy(ActualDestination->Name, Name);
            // Search a last line
            LastLine = NewGroup->List;
            while(LastLine->Next != NULL) LastLine = LastLine->Next;
            LastDestination = LastLine->List;
            // Chain
            if(LastDestination != NULL){
              while(LastDestination->Next != NULL) LastDestination = LastDestination->Next;
              LastDestination->Next = ActualDestination;
            }
            else LastLine->List = ActualDestination;
            ActualDestination->Next = NULL;
            ActualDestination->Previous = LastDestination;
            Break = EditorPanelID;
            Get_Buttom_Event(uart0_filestream);
              
          }
          else{
            //Mensage Error
            Write_Integer16b (uart0_filestream, Loading, 0);
            Write_Integer16b (uart0_filestream, Signals, 2);
            Write_Integer16b (uart0_filestream, Status, 0);
            sleep(2);
            //Return 
            Write_Integer16b (uart0_filestream, Loading, 0);
            Write_Integer16b (uart0_filestream, Signals, 0);
            Write_Integer16b (uart0_filestream, Status, 1);
            Break = -1;
          }
          break;

        case ButtonSettingsEditorDestino:
          // -- Return
          Break = SettingsMainID;
          break;

        case ButtonHomeEditorDestino:
          // -- Return
          Break = MainID;
          break;

        case ButtonKeyboardNameEditorDestino:
          // -- Reset Page Import USB
          Set_Page(uart0_filestream, EditorDestinoIDKeyboard);
          // -- Debounce forced
          Get_Buttom_Event (uart0_filestream);
          // -- Return
          Break = -1;
          break;

        case KeyboardEnter:
          // -- Reset Page Import USB
          Set_Page(uart0_filestream, EditorDestinoID);
          // -- Debounce forced
          Get_Buttom_Event (uart0_filestream);
          // -- Return
          Break = -1;
          break;

        case KeyboardBackSpace:
          if(position > 0) Name[--position] = '\0';
          else Name[0] = '\0';
          //writing in display
          Write_String (uart0_filestream, AdressDestinationName, Name);
          Break = -1;
          break;

        case KeyboardShift:
          if(Bt.PagId != EditorDestinoIDKeyboardCapsLock){
            // -- Set to page with Caps Lock
            Set_Page(uart0_filestream, EditorDestinoIDKeyboardCapsLock);
            // -- Debounce forced
            Get_Buttom_Event (uart0_filestream);

          }
          else{
            // -- Set to page without Caps Lock
            Set_Page(uart0_filestream, EditorDestinoIDKeyboard);
            // -- Debounce forced
            Get_Buttom_Event (uart0_filestream);

          }
          Break = -1;
          break;

        case KeyboardOp:
          if(Bt.PagId != EditorDestinoIDOp){
            // -- Set to page with Optional simbols
            Set_Page(uart0_filestream, EditorDestinoIDOp);
            // -- Debounce forced
            Get_Buttom_Event (uart0_filestream);

          }
          else{
            // -- Set to page with convencional simbols
            Set_Page(uart0_filestream, EditorDestinoIDKeyboard);
            // -- Debounce forced
            Get_Buttom_Event (uart0_filestream);
         
          }
          Break = -1;
          break;

        default:
          if(position < ScreenTitleLength){
            Name[position++] = Bt.ButtonId;
            Name[position] = '\0';
            //writing in display
            Write_String (uart0_filestream, AdressDestinationName, Name);

          }
          Break = -1;

      }

    }
    
  }

  return Break;

}

//Handler event for Editor Make Panel
int HandlerEditorPanel(int uart0_filestream, Group *NewGroup){

  unsigned char BufferLines[6];
  unsigned char BufferColumns[6];
  unsigned char Select = 0;
  int positionLines = 0;
  int positionColumns = 0;
  int Break = -1;
  Line *LastLine =  NULL;
  Destination *LastDestination =  NULL;
  Panel *LastPanel =  NULL;
  Panel *ActualPanel =  NULL;

  BufferLines[0] = '\0';
  BufferColumns[0] = '\0';

  //Cleanig display
  Write_Integer16b (uart0_filestream, AdressLinesNumberEditorPanel, 0);
  Write_Integer16b (uart0_filestream, AdressColumnsNumberEditorPanel, 0);

  //Forced debounce
  Get_Buttom_Event (uart0_filestream);

  //---- Process to Reading Buttons
  while(Break < 0){
    Button Bt = Get_Buttom_Event (uart0_filestream);
    if((Bt.Direction == 0) && (Bt.PagId == EditorPanelID)){
      //Select Buttons
      switch(Bt.ButtonId){
        case ButtonCancelEditorPainel:
          Break = SourceIDA;
          break;

        case ButtonConfirmEditorPainel:
          if(positionLines > 0 && positionColumns > 0){
            //Crete a new Panel
            ActualPanel = (Panel *)malloc(sizeof(Panel));
            ActualPanel->Lines = atoi(BufferLines);
            ActualPanel->Columns = atoi(BufferColumns);
            for(int cont = 0; cont<LimitOfPages; cont++) ActualPanel->List[cont].ID = 0;
            // Search a last Panel
            LastLine = NewGroup->List;
            while(LastLine->Next != NULL) LastLine = LastLine->Next;
            LastDestination = LastLine->List;
            while(LastDestination->Next != NULL) LastDestination = LastDestination->Next;
            LastPanel = LastDestination->List;
            // Chain
            if(LastPanel != NULL){
              while(LastPanel->Next != NULL) LastPanel = LastPanel->Next;
              LastPanel->Next = ActualPanel;
            }
            else LastDestination->List = ActualPanel;
            ActualPanel->Next = NULL;
            ActualPanel->Previous = LastPanel;
            Break = EditorPaginaID;
              
          }
          else{
            //Mensage Error
            Write_Integer16b (uart0_filestream, Loading, 0);
            Write_Integer16b (uart0_filestream, Signals, 2);
            Write_Integer16b (uart0_filestream, Status, 0);
            sleep(2);
            //Return 
            Write_Integer16b (uart0_filestream, Loading, 0);
            Write_Integer16b (uart0_filestream, Signals, 0);
            Write_Integer16b (uart0_filestream, Status, 1);
            Break = -1;
          }
          break;

        case ButtonSettingsEditorPainel:
          // -- Return
          Break = SettingsMainID;
          break;

        case ButtonHomeEditorPainel:
          // -- Return
          Break = MainID;
          break;

        case ButtonColumnsEditorPanel:
          Select = 1;
          Break = -1;
          break;

        case ButtonLinesEditorPanel:
          Select = 0;
          Break = -1;
          break;

        case ButtonOKEditorPanel:
          Break = -1;
          break;

        case ButtonDelEditorPanel:
          if(Select == 1){
            if(positionColumns > 0){
              BufferColumns[--positionColumns] = '\0';
              Write_Integer16b (uart0_filestream, AdressColumnsNumberEditorPanel, atoi(BufferColumns));
            }

          }
          else{
            if(positionLines > 0){
              BufferLines[--positionLines] = '\0';
              Write_Integer16b (uart0_filestream, AdressLinesNumberEditorPanel, atoi(BufferLines));
            }

          }
          Break = -1;
          break;

        default:
          if(Select == 1){
            if(positionColumns<5){
              BufferColumns[positionColumns++] = Bt.ButtonId+40;
              BufferColumns[positionColumns] = '\0';
              Write_Integer16b (uart0_filestream, AdressColumnsNumberEditorPanel, atoi(BufferColumns));
            }

          }
          else{
            if(positionLines<5){
              BufferLines[positionLines++] = Bt.ButtonId+40;
              BufferLines[positionLines] = '\0';
              Write_Integer16b (uart0_filestream, AdressLinesNumberEditorPanel, atoi(BufferLines));
            }

          }
          Break = -1;

      }
    }   
  }

  return Break;

}

//Constant
const unsigned char RefFonts[11][2] = {{5,5}, {7,5}, {8,6}, {11,7}, {12,7}, {13,9}, {14,9}, {15,9}, {16,9}, {17,11}, {19,11}};

//Handler event for Editor Make Text
int HandlerEditorText(int uart0_filestream, Page List){

  int Break = -1;
  unsigned char Text[30];
  unsigned char position = 0;
  int conte = 0;

  Text[0] = '\0';

  //Inicialize
  Write_Integer16b (uart0_filestream, AdressRefNumberEditorText, 0);
  Write_String (uart0_filestream, AdressInfoEditorText, " ");
  Write_String (uart0_filestream, AdressAlignEditorText, " ");
  Write_String (uart0_filestream, AdressEffectEditorText, " ");
  Write_String (uart0_filestream, AdressFontEditorText, " ");

  while(Break < 0){
    Button Bt = Get_Buttom_Event (uart0_filestream);
    if((Bt.Direction == 0) && (Bt.PagId == EditorPaginaID)){
      //Select Buttons
      switch(Bt.ButtonId){
        case ButtonCancelText:
          Break = 0;
          break;

        case ButtonConfirmText:
          Break = 0;
          break;

        case ButtonSettingText:
          Break = 1;
          break;

        case ButtonHomeText:
          Break = 2;
          break;

        case ButtonKeyText:
          // -- Reset Page Import USB
          Set_Page(uart0_filestream, EditorTextoIDKeyboard);
          // -- Debounce forced
          Get_Buttom_Event (uart0_filestream);
          // -- Return
          Break = -1;
          break;

        case KeyboardEnter:
          // -- Reset Page Import USB
          Set_Page(uart0_filestream, EditorTextoID);
          // -- Debounce forced
          Get_Buttom_Event (uart0_filestream);
          // -- Return
          Break = -1;
          break;

        case KeyboardShift:
          if(Bt.PagId != EditorTextoIDKeyboardCapsLock){
            // -- Reset Page Import USB
            Set_Page(uart0_filestream, EditorTextoIDKeyboardCapsLock);
            // -- Debounce forced
            Get_Buttom_Event (uart0_filestream);
            // -- Return

          }
          else{
            // -- Reset Page Import USB
            Set_Page(uart0_filestream, EditorTextoIDKeyboard);
            // -- Debounce forced
            Get_Buttom_Event (uart0_filestream);
            // -- Return

          }

        case KeyboardOp
          if(Bt.PagId != EditorTextoIDOp){
            // -- Reset Page Import USB
            Set_Page(uart0_filestream, EditorTextoIDOp);
            // -- Debounce forced
            Get_Buttom_Event (uart0_filestream);

          }
          else{
            // -- Reset Page Import USB
            Set_Page(uart0_filestream, EditorTextoIDKeyboard);
            // -- Debounce forced
            Get_Buttom_Event (uart0_filestream);
            

          }
          // -- Return
          Break = -1;

        case KeyboardBackSpace:
          if(position > 0) Text[--position] = '\0';
          Break = -1;
          break;

        case ButtonUpNumberText;
          if(conte < 3){
            conte++;

          }
          break;
      
      }
    }
  }
  return Break;

}

//Handler event for Editor Make Page
int HandlerEditorPage(int uart0_filestream, Group *NewGroup){

  int Break = -1;
  int cont = 0;
  unsigned char CounterText = 0;
  unsigned char PostingTime = 0;
  unsigned char SpaceBetweenCharacters = 0;
  unsigned char SpaceBetweenNumberAndCharacters = 0;
  unsigned char Select = 0;
  unsigned char AlignNumber = 0;
  Line *LastLine =  NULL;
  Destination *LastDestination =  NULL;
  Panel *LastPanel =  NULL;
  unsigned char Info[LimitOfFields][LimitOfCharPerLine];
  unsigned char Effect[LimitOfFields];
  unsigned char Align[LimitOfFields];
  unsigned char FontWidth[LimitOfFields];

  //Cleanig display
  Write_Integer16b (uart0_filestream, AdressPostingTimeEditorPage, 0);
  Write_Integer16b (uart0_filestream, AdressSpaceBetweenCharactersEditorPage, 0);
  Write_Integer16b (uart0_filestream, AdressSpaceBetweenNumberAndCharactersEditorPage, 0);
  Write_String (uart0_filestream, AdressNumberAlignEditorPage, "Esquerda");
  Write_String (uart0_filestream, AdressVisualEditorPage, "Numero");

  //Forced debounce
  Get_Buttom_Event (uart0_filestream);

  // Search a last line
  LastLine = NewGroup->List;
  while(LastLine->Next != NULL) LastLine = LastLine->Next;
  LastDestination = LastLine->List;
  while(LastDestination->Next != NULL) LastDestination = LastDestination->Next;
  LastPanel = LastDestination->List;
  while(LastPanel->Next != NULL) LastPanel = LastPanel->Next;
  for(cont = 0; cont<LimitOfPages && LastPanel->List[cont].ID != 0; cont++);

  //---- Process to Reading Buttons
  while(Break < 0){
    Button Bt = Get_Buttom_Event (uart0_filestream);
    if((Bt.Direction == 0) && (Bt.PagId == EditorPaginaID)){
      //Select Buttons
      switch(Bt.ButtonId){
        
        case ButtonCancelEditorPage:
          Break = SourceIDA;
          break;

        case ButtonConfirmEditorPage:
          if(((Select == 1 || Select == 2) && CounterText>0) || (Select == 3) || (Select == 0)){
            LastPanel->List[cont].ID = cont + 1;
            LastPanel->List[cont].SpaceBetweenCharacters = SpaceBetweenCharacters;
            LastPanel->List[cont].SpaceBetweenNumberAndCharacters = SpaceBetweenNumberAndCharacters;
            LastPanel->List[cont].PostingTime = PostingTime;
            LastPanel->List[cont].NumberOfFields = CounterText;
            for(int conte = 0; conte<CounterText; conte++){
              strcpy(LastPanel->List[cont].TextList[conte].Info, Info[conte]);
              LastPanel->List[cont].TextList[conte].Effect = Effect[conte];
              LastPanel->List[cont].TextList[conte].Align = Align[conte];
              LastPanel->List[cont].TextList[conte].FontWidth = RefFonts[conte][0];
              LastPanel->List[cont].TextList[conte].FontHeight = RefFonts[conte][1];

            }
            Break = SourceIDA;
              
          }
          else{
            //Mensage Error
            Write_Integer16b (uart0_filestream, Loading, 0);
            Write_Integer16b (uart0_filestream, Signals, 2);
            Write_Integer16b (uart0_filestream, Status, 0);
            sleep(2);
            //Return 
            Write_Integer16b (uart0_filestream, Loading, 0);
            Write_Integer16b (uart0_filestream, Signals, 0);
            Write_Integer16b (uart0_filestream, Status, 1);
            Break = -1;
          }
          break;

        case ButtonSettingEditorPage:
          // -- Return
          Break = SettingsMainID;
          break;

        case ButtonHomeEditorPage:
          // -- Return
          Break = MainID;
          break;

        case ButtonDownPostingTimeEditorPage:
          if(PostingTime > 0){
            PostingTime--;
            Write_Integer16b(uart0_filestream, AdressPostingTimeEditorPage, PostingTime);
          
          } 
          Break = -1;
          break;

        case ButtonUpPostingTimeEditorPage:
          if((PostingTime<64 && cont == 0) || (PostingTime<256 && cont > 0)){
            PostingTime++;
            Write_Integer16b(uart0_filestream, AdressPostingTimeEditorPage, PostingTime);

          }
          Break = -1;
          break;

        case ButtonUpSpaceBetweenCharactersEditorPage:
          if(SpaceBetweenCharacters < 4){
            SpaceBetweenCharacters++;
            Write_Integer16b(uart0_filestream, AdressSpaceBetweenCharactersEditorPage, SpaceBetweenCharacters);

          }
          Break = -1;
          break;

        case ButtonDownSpaceBetweenCharactersEditorPage:
          if(SpaceBetweenCharacters > 0){
            SpaceBetweenCharacters--;
            Write_Integer16b(uart0_filestream, AdressSpaceBetweenCharactersEditorPage, SpaceBetweenCharacters);
            
          }
          Break = -1;
          break;

        case ButtonUpSpaceBetweenNumberAndCharactersEditorPage:
          if(SpaceBetweenNumberAndCharacters < 16){
            SpaceBetweenNumberAndCharacters++;
            Write_Integer16b(uart0_filestream, AdressSpaceBetweenNumberAndCharactersEditorPage, SpaceBetweenNumberAndCharacters);

          }
          Break = -1;
          break;

        case ButtonDownSpaceBetweenNumberAndCharactersEditorPage:
          if(SpaceBetweenNumberAndCharacters > 0){
            SpaceBetweenNumberAndCharacters--;
            Write_Integer16b(uart0_filestream, AdressSpaceBetweenNumberAndCharactersEditorPage, SpaceBetweenNumberAndCharacters);
            
          }
          Break = -1;
          break;

        case ButtonDownVisualEditorPage:
          if(Select > 0){
            Select--;
            switch(Select){
              case 1:
                Write_String (uart0_filestream, AdressVisualEditorPage, "Texto");
                break;

              case 2:
                Write_String (uart0_filestream, AdressVisualEditorPage, "Numero + Texto");
                break;

              case 3:
                Write_String (uart0_filestream, AdressVisualEditorPage, "Sem Info.");
                break;

              default:
                Write_String (uart0_filestream, AdressVisualEditorPage, "Numero");
            }
          }
          Break = -1;
          break;

        case ButtonUpVisualEditorPage:
          if(Select < 2){
            Select++;
            switch(Select){
              case 1:
                Write_String (uart0_filestream, AdressVisualEditorPage, "Texto");
                break;

              case 2:
                Write_String (uart0_filestream, AdressVisualEditorPage, "Numero + Texto");
                break;

              case 3:
                Write_String (uart0_filestream, AdressVisualEditorPage, "Sem Info.");
                break;

              default:
                Write_String (uart0_filestream, AdressVisualEditorPage, "Numero");

            }
          }
          Break = -1;
          break;

        case ButtonDownAlignNumberEditorPage:
          if(AlignNumber > 0){
            AlignNumber--;
            switch(AlignNumber){
              case 1:
                Write_String (uart0_filestream, AdressNumberAlignEditorPage, "Centro");
                break;

              case 2:
                Write_String (uart0_filestream, AdressNumberAlignEditorPage, "Direita");
                break;

              default:
                Write_String (uart0_filestream, AdressNumberAlignEditorPage, "Esquerda");
            }
          }
          Break = -1;
          break;

        case ButtonUpAlignNumberEditorPage:
          if(AlignNumber < 2){
            AlignNumber++;
            switch(AlignNumber){
              case 1:
                Write_String (uart0_filestream, AdressNumberAlignEditorPage, "Centro");
                break;

              case 2:
                Write_String (uart0_filestream, AdressNumberAlignEditorPage, "Direita");
                break;

              default:
                Write_String (uart0_filestream, AdressNumberAlignEditorPage, "Esquerda");
            }
          }
          Break = -1;
          break;

        case ButtonAddTxEditorPage:
          if(Select == 1 || Select == 2){
            HandlerEditorText(uart0_filestream, &List[cont]);
          }
          Break = -1;
          break;

        case ButtonAddLineEditorPage:
          Break = EditorLinhaID;
          break;

        case ButtonAddDestinationEditorPage:
          Break = EditorDestinoID;
          break;

        case ButtonAddPanelEditorPage:
          Break = EditorPanelID;
          break;

        case ButtonAddPageEditorPage:
          Break = EditorPaginaID;
          break;

        default:
          Break = -1;

      }

    }
    
  }

  return Break;

}