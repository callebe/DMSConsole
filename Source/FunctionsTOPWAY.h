// -------- Include Library
#include <stdio.h>    //Standard Library
#include <string.h> //Standard Library for strings 
#include <unistd.h>   //Used for UART
#include <fcntl.h>    //Used for UART
#include <termios.h>  //Used for UART
#include "GIDXml.h" //Headers Functions of XmlReader
#include <dirent.h> //Use by ist directories
#include <time.h> //Used by RTC functions
#include <sys/time.h> // Used by RTC functions
#include <errno.h> // Used by erro detection
#include "DMSDisplay.h" //Headers Functions of DMSDIsplay

#ifndef DEF_TOPWAY
	
	#define DEF_TOPWAY
	// Header
	#define Header                    0xAA // Header each all commands
	// Commands
	#define Read_Hand_Shake           0x30 // Read a Hand Shake
	#define Read_Version              0x31 // Read firmware version
	#define Read_Pg_Id                0x32 // Read Current page ID
	#define Touch_Response            0x72 // See also Set_Sys_Config /0x73/0x77/0x78/0x79
	#define Set_Sys_Config            0xE0 // System parameter configuration and Baud Rate
	#define Sel_Project               0xE1 // System parameter configuration and Baud Rate
	#define Touch_Calib               0xE4 // Touch panel calibration
	#define Screen_Saver              0x5E // Screen saver (backlight dim down time out)
	#define Backlight_Ctrl            0x5F // Backlight brightness control (64 levels)
	#define Buzzer_Touch_Sound        0x79 // Buzzer enable time length (in 10ms step)
	#define Buzzer_Ctrl               0x7A // Buzzer control
	#define Flash_Write               0x90 // Write data to the flash
	#define Flash_Read                0x91 // Read data from the flash
	#define RTC_Set                   0x9C // Set the RTC
	#define RTC_Read                  0x9B // Read the RTC values
	#define USR_Bin_Read              0x93 // Read data from the USR_bin
	#define U_Drv_Format              0xE2 // Format the u_drive
	#define U_Drv_Unlock              0xE3 // Unlock the u_drive with pre-stored password
	#define Disp_Page                 0x70 // Display a pre-stored TML file (page)
	#define Set_Element_Fg            0x7E // Set the foreground color of STR, N16, N32 or N64
	#define Set_Element_Bg            0x7F // Set the background color of STR, N16, N32 or N64
	#define Set_Codepage              0xE7 // Sets country character set and code-page character set
	#define Suspend_Vp_Fresh          0xE8 // Set the screen to pause the refresh and deactivate the touchkey or release the pause to refresh and enable the touchkey
	#define STR_Write                 0x42 // Write string to VP_STR
	#define STR_Read                  0x43 // Read string form VP_STR
	#define STR_Fill                  0x46 // Fill strings to the VP_STR
	#define N16_Write                 0x3D // Write 16bit (signed integer) value to VP_N16
	#define N16_Read                  0x3E // Read 16bit (signed integer) value from VP_N16
	#define N16_Fill                  0x3F // Fill numbers to the VP_N16
	#define N32_Write                 0x44 // Write 32bit (signed integer) value to VP_N32
	#define N32_Read                  0x45 // Read 32bit (signed integer) value from VP_N32
	#define N32_Fill                  0x47 // Fill numbers to the VP_N32
	#define N64_Write                 0x48 // Write 64bit (signed integer) value to VP_N64
	#define N64_Read                  0x49 // Read 64bit (signed integer) value from VP_N64
	#define N64_Fill                  0x4A // Fill numbers to the VP_N64
	#define Successive_Write          0x82 // Write successive value to VP_N16, VP_N32, VP_N64
	#define Successive_Read           0x83 // Read successive value from VP_N16,VP_N32,VP_N64
	#define BP1_Write                 0x4B // Write bit-map (1bpp) data to VP_BP1
	#define BP1_Write_comp            0x4C // Write compressed bit-map (1bpp) data to VP_BP1
	#define G16_Write                 0x4D // Write 16bit (signed integer) graphic array to VP_G16
	#define G16_Write_rotate          0x4E // Rotate the VP_G16 array data inside the module and write a 16bit (signed integer) value into end-of-array
	#define Sys_Reg_Timer_Ctrl_Write  0x3B // Write System Register
	#define Sys_Reg_Timer_Ctrl_Read   0x3C // Read System Register
	// Tail
	#define Tail_A                    0xCC // Tail key for each command 
	#define Tail_B                    0x33 // Tail key for each command 
	#define Tail_C                    0xC3 // Tail key for each command 
	#define Tail_D                    0x3C // Tail key for each command
	//Command Response
	#define Execulted_Command         0x3A // Confirm that send command was execulted
	#define Wait_Next_Command         0x3E // Display wait your next command
	#define Erro_In_Exe_Command       0x21 // Erro in run sended command
	// Config
	#define XMLSourceDirectory "/home/pi/DMSConsole/Data/"
	#define USBDirectory "/media/usb/"
	// Screen
	#define ScreenTitleLength 30
	#define LimitTitleLength 75
	#define LimitTerminalLength 54
	#define LimitLinesTerminal 15
	#define LimitOfLinesInSource 5
	#define LimitOfLinesInImportUSB 7
	// Main Page
	#define MainID 0 
	#define ButtonNextDestination 0
	#define ButtonPreviousDestination 1
	#define ButtonChangeLine 2
	#define ButtonSettings 3
	#define AdressNumberLine 0x000080
	#define AdressNameLine  0x000100
	#define AdressNameDestination 0x000180
	// Select Destination Page
	#define SelectLinesID 1
	#define SelectLinesIDKeybord 2
	#define ButtonCancelChangeLine 0
	#define ButtonConfirmChangeLine 1
	#define ButtonUpChangeLine 2
	#define ButtonDownChangeLine 3
	#define ButtonHomeSelectDestinations 4
	#define ButtonSettingsSelectDestinations 5
	#define ButtonSettingsActiveKeybord 6
	#define ButtonSettingsActiveKeyDel 7
	#define ButtonSettingsActiveKeyOk 8
	#define AdressSelectNameLine 0x000200
	#define AdressSelectIDLine   0x000280
	// Setting Page
	#define SettingsMainID 3 
	#define ButtonCancelSettings 0
	#define ButtonSelectWatch 1
	#define ButtonSelectSource 2
	#define ButtonSettingsSettings 4
	#define ButtonPainelSetting 5
	#define ButtonHomeSettings 3
	#define ButtonTerminal 7
	// Watch Page
	#define WatchID 4
	#define WatchID2 12
	#define ButtonCancelChangeWatch 0
	#define ButtonConfirmChangeWatch 1
	#define ButtonHomeWatch 2
	#define ButtonSettingsWatch 3
	#define ButtonDownHora 4
	#define ButtonUpHora 5
	#define ButtonDownMin 6
	#define ButtonUpMin 7
	#define ButtonChangeDateTime 8
	#define ButtonDownYear 9
	#define ButtonUpYear 10
	#define AdressDecHora 0x080006
	#define AdressHora 0x080008
	#define AdressDecMin 0x08000A
	#define AdressMin 0x08000C
	#define AdressDecYear 0x080010
	#define AdressYear 0x08000E
	#define AdressDec 0x080012
	#define AdressDecDec 0x080014
	// Source Page
	#define SourceIDA 5
	#define SourceIDB 6
	#define SourceIDC 7
	#define SourceIDD 8
	#define SourceIDE 9
	#define ButtonCancelChangeSource 0
	#define ButtonConfirmChangeSource 1
	#define ButtonNewSource 2
	#define ButtonDownSource 3
	#define ButtonUpSource 4
	#define ButtonSettingsSource 5
	#define ButtonHomeSource 6
	#define ButtonUSBImport 7
	#define ButtonTouchA 8
	#define ButtonTouchB 9
	#define ButtonTouchC 10
	#define ButtonTouchD 11
	#define ButtonTouchE 12
	#define Status 0x080000
	#define Signals 0x080002
	#define Loading 0x080004
	#define AdressInitialSource 0x000300
	// Terminal Page
	#define TerminalID 10
	#define ButtonHomeTerminal 0
	#define AdressInitialTerminal 0x000E00
	#define AdressFinalTerminal 0x001500
	// Import USB Page
	#define ImportUSBIDA 13
	#define ImportUSBIDB 14
	#define ImportUSBIDC 15
	#define ImportUSBIDD 16
	#define ImportUSBIDE 17
	#define ImportUSBIDF 18
	#define ImportUSBIDG 19
	#define ButtonCancelUSB 0
	#define ButtonImportUSB 1
	#define ButtonUpUSB 2
	#define ButtonDownUSB 3
	#define ButtonSettingUSB 4
	#define ButtonHomeUSB 5
	#define ButtonTouchUSBA 6
	#define ButtonTouchUSBB 7
	#define ButtonTouchUSBC 8
	#define ButtonTouchUSBD 9
	#define ButtonTouchUSBE 10
	#define ButtonTouchUSBF 11
	#define ButtonTouchUSBG 12
	#define AdressInitialImportUSB 0x000900
	// Editor
	// -- File
	#define EditorArquivoID 20
	#define ButtonCancelarArquivo 0
	#define ButtonProximoArquivo 1
	#define ButtonSettingsArquivo 2
	#define ButtonHomeArquivo 3
	#define ButtonKeyboardArquivo 4
	#define EditorArquivoIDKeyboard 21
	#define EditorArquivoIDKeyboardCapsLock 22
	#define EditorArquivoIDKeyboardOp 23
	#define AdressFileName 0x000580
	// -- Line
	#define EditorLinhaID 24
	#define EditorLinhaIDKeyboard 25
	#define EditorLinhaIDKeyboardCapsLock 26
	#define EditorLinhaIDOp 27
	#define ButtonCancelarEditorLinha 0
	#define ButtonProximoEditorLinha 1
	#define ButtonSettingsEditorLinha 2
	#define ButtonHomeEditorLinha 3
	#define ButtonKeyboardNameEditorLinha 4
	#define ButtonKeyboardNumberEditorLinha 5
	#define AdressLineName 0x000600
	#define AdressLineNumber 0x000680
	// -- Destination
	#define EditorDestinoID 29
	#define EditorDestinoIDKeyboard 30
	#define EditorDestinoIDKeyboardCapsLock 31
	#define EditorDestinoIDOp 32
	#define ButtonCancelarEditorDestino 0
	#define ButtonProximoEditorDestino 1
	#define ButtonSettingsEditorDestino 2
	#define ButtonHomeEditorDestino 3
	#define ButtonKeyboardNameEditorDestino 4
	#define AdressDestinationName 0x000700
	// -- Panel
	#define EditorPanelID 33
	#define ButtonCancelEditorPainel 0
	#define ButtonConfirmEditorPainel 1
	#define ButtonSettingsEditorPainel 2
	#define ButtonHomeEditorPainel 3
	#define ButtonColumnsEditorPanel 4
	#define ButtonLinesEditorPanel 5
	#define ButtonOKEditorPanel 7
	#define ButtonDelEditorPanel 6
	#define AdressLinesNumberEditorPanel 0x080020
	#define AdressColumnsNumberEditorPanel 0x080022
	// -- Page
	#define EditorPaginaID 34
	#define ButtonCancelEditorPage 0
	#define ButtonConfirmEditorPage 1
	#define ButtonSettingEditorPage 2
	#define ButtonHomeEditorPage 3
	#define ButtonDownPostingTimeEditorPage 4
	#define ButtonUpPostingTimeEditorPage 5
	#define ButtonDownSpaceBetweenCharactersEditorPage 6
	#define ButtonUpSpaceBetweenCharactersEditorPage 7
	#define ButtonUpSpaceBetweenNumberAndCharactersEditorPage 8
	#define ButtonDownSpaceBetweenNumberAndCharactersEditorPage 9
	#define ButtonDownVisualEditorPage 10
	#define ButtonUpVisualEditorPage 11
	#define ButtonAddTxEditorPage 12
	#define ButtonAddLineEditorPage 13
	#define ButtonAddDestinationEditorPage 14
	#define ButtonAddPanelEditorPage 15
	#define ButtonAddPageEditorPage 16
	#define ButtonDownAlignNumberEditorPage 17
	#define ButtonUpAlignNumberEditorPage 18
	#define AdressPostingTimeEditorPage 0x080016
	#define AdressSpaceBetweenCharactersEditorPage 0x080018
	#define AdressSpaceBetweenNumberAndCharactersEditorPage 0x08001A
	#define AdressVisualEditorPage 0x000C80
	#define AdressNumberAlignEditorPage 0x000D80
	// -- Text
	#define EditorTextoID 35
	#define EditorTextoIDKeyboard 36
	#define EditorTextoIDKeyboardCapsLock 37
	#define EditorTextoIDOp 38
	#define ButtonCancelText 0
	#define ButtonConfirmText 1
	#define ButtonSettingText 2
	#define ButtonHomeText 3
	#define ButtonKeyText 4
	#define ButtonUpNumberText 5
	#define ButtonDownNumberText 6
	#define ButtonDownAlignText 17
	#define ButtonUpAlignText 16
	#define ButtonDownEffectText 9
	#define ButtonUpEffectText 10
	#define ButtonDownFontText 18
	#define ButtonUpFontText 19
	#define AdressInfoEditorText 0x000780
	#define AdressRefNumberEditorText 0x08001E
	#define AdressAlignEditorText 0x000D00
	#define AdressEffectEditorText 0x000800
	#define AdressFontEditorText 0x000880
	//Keyboard
	#define KeyboardEnter 13
	#define KeyboardBackSpace 8
	#define KeyboardShift 14
	#define KeyboardOp 12
	
	//Type Define Button
	typedef struct Button{
		int PagId;
		int ButtonId;
		char Direction;

	}Button;

	//Type Define Directory
	typedef struct Direct{
		unsigned char *Name;
		unsigned char Type;
		struct Direct *Next;
		struct Direct *Previous;

	}Direct;


	// -------- Prototype of Functions
	// Configuration Function
	int Config_UARTS0 (void);
	// Transmission Function
	int Tx_UARTS0 (int uart0_filestream, unsigned char *Tx, int Dimension);
	// Receive Function
	int Rx_UARTS0 (int uart0_filestream, unsigned char *Rx, int Dimension);
	// Buzzer Touch Function
	int Buzzer_Touch_Off (int uart0_filestream, unsigned char Time_Interval);
	//Integer 16b write function
	int Write_Integer16b (int uart0_filestream, unsigned int  Adress, int Value);
	// String Write Function
	int Write_String (int uart0_filestream, unsigned int  Adress , unsigned char *Text);
	//Get Buttom Event Function
	Button Get_Buttom_Event (int uart0_filestream);
	//Set RTC Function
	int Set_RTC (int uart0_filestream, unsigned char Year, unsigned char Month, unsigned char Day, unsigned char Hour, unsigned char Minute, unsigned char Second);
	//Read RTC Function
	int Read_RTC (int uart0_filestream, unsigned char *Year, unsigned char *Month, unsigned char *Day, unsigned char *Hour, unsigned char *Minute, unsigned char *Second);
	//Set Page Function
	int Set_Page (int uart0_filestream, unsigned int Page);
	// Function Search Line
	Line* SearchLine(Line *InitialLine, int CounterBuffer, unsigned char Buffer[4]);
	//Initialize Source Function
	int InitializeFunction (GID *NewGID, Group **ActualGroup, Line **ActualLine, Destination **ActualDestination);
	// Handler of Main Page
	int HandlerMain (int uart0_filestream, Line **ActualLine, Destination **ActualDestination);
	//Handler event for Select Destination
	int HandlerSelectLines (int uart0_filestream, Line *InitialLine, Line **ActualLine, Destination **ActualDestination);
	//Handler event for Settings
	int HandlerWatch(int uart0_filestream);
	//Handler event for Settings
	int HandlerSettingsMain(int uart0_filestream);
	//Handler event for Source
	int HandlerSource (int uart0_filestream, Group **ActualGroup, Line **ActualLine, Destination **ActualDestination);
	//Handler event for Import Xml Files 
	int HandlerImportUSB(int uart0_filestream);
	//Handler event for Editor
	int HandlerEditor(int uart0_filestream, Group **NewGroup);
	//Handler event for Editor Make Line
	int HandlerEditorLine(int uart0_filestream, Group *NewGroup);
	//Handler event for Editor Make Line
	int HandlerEditorDestinations(int uart0_filestream, Group *NewGroup);
	//Handler event for Editor Make Text
	int HandlerEditorText(int uart0_filestream, Page *ActualPage);
	//Handler event for Editor Make Panel
	int HandlerEditorPanel(int uart0_filestream, Group *NewGroup);
	//Handler event for Editor Make Page
	int HandlerEditorPage(int uart0_filestream, Group *NewGroup);
	//Press XML source
	int XMLSource (Group *NewGroup);
	//Handler Terminal
	int HandlerTerminal (int uart0_filestream);

#endif