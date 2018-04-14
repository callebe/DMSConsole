// -------- Defines
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
