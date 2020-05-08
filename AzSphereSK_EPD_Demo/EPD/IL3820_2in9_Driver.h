/*****************************************************************************
* | File      	:  	IL3820_2in9_Driver.h
* | Author      :   GS Gill (gsgill112.github.io) @ TechNervers
* | Function    :   Main Driver File for WaveShare/ Good Display 2.9" bi-color EPD display.
*                   Based on Waveshare driver and orignal Waveshare LUT's. Will add user LUT afetr testing them.
*					Demo @YouTube Link : 
* | Info        :   ~2s Refresh time with Default LUT's
*----------------
* |	This version:   V0.4dbg
* | Date        :   2020-05-04
* | Info        :
* -----------------------------------------------------------------------------
* Change Log V0.4dbg (2020-05-04):
* 1. Added : Support for Waveshare/ Good Display 2.9" Bi-Colour EPD pannels with Display driver IL3820
*            Datasheet and reference materials can be found in Reference Section
* 2. Support for 128x296 Image Works, with the standard (orignal) LUT Provided by Waveshare
* 3. Both Standard and Partial Mode work
* 4. EPD Clear function can be called to avoid permanent damage if not using Sleep
*-------------------------------------------------------------------------------
*
*  INFO : Please check Readme to understand how to include this library in your project and test.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), for
# non-commercial use.
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
******************************************************************************/
#ifdef  EPD_2in9_H	

//Global Includes
#include <errno.h>

#include "../HighLevelCore/applibs_versions.h"

#include <applibs/spi.h>
#include <applibs/gpio.h>
#include <applibs/log.h>

#include "AzSphere_Interface.h"

//Defined Variables and Access Pin FD
extern uint16_t max_h, max_w;


// Display resolution
#define EPD_2in9_WIDTH 128
#define EPD_2in9_HEIGHT 296

#define EPD_WidthByte (EPD_2in9_WIDTH % 8 == 0) ? (EPD_2in9_WIDTH / 8) : (EPD_2in9_WIDTH / 8 + 1);	// used to combine Pixels in 8 pixel =  1 byte format
#define EPD_HeightByte EPD_2in9_HEIGHT

#define EPD_ByteBuffer EPD_WidthByte * EPD_HeightByte	
#define EPD_Buffer EPD_2in9_WIDTH * EPD_2in9_HEIGHT

#define WaveShare_2in9_EPD_FULL			0
#define WaveShare_2in9_EPD_PART			1
#define U8G2_2in9_EPD_FULL				2
#define U8G2_2in9_EPD_PART				3


#define EPD_2in9_ORIENT_POTRAIT 0 // Sets Potrait Orient 
#define EPD_2in9_ORIENT_LANDSCAPE 1  // Sets Landscape Orient

//#define BUFFER_SIZE EPD_2in9_WIDTH*EPD_2in9_HEIGHT  //Display Bufffer Size 

// Standard register set for ILI3820 I/F
// This is redundant info, ONLY FOR REFERENCE, AS THIS DOESNT NOT CHANGE HARDCODED VALUES ARE USED 
#define  DRIVER_OUTPUT_CONTROL 0x01
#define  BOOSTER_SOFT_START_CONTROL 0x0C
#define  GATE_SCAN_START_POSITION 0x0F
#define  DEEP_SLEEP_MODE 0x10
#define  DATA_ENTRY_MODE_SETTING 0x11
#define  SW_RESET 0x12
#define  TEMPERATURE_SENSOR_CONTROL 0x1A
#define  MASTER_ACTIVATION 0x20
#define  DISPLAY_UPDATE_CONTROL_1 0x21
#define  DISPLAY_UPDATE_CONTROL_2 0x22
#define  WRITE_RAM 0x24
#define  WRITE_VCOM_REGISTER 0x2C
#define  WRITE_LUT_REGISTER 0x32
#define  SET_DUMMY_LINE_PERIOD 0x3A
#define  SET_GATE_TIME 0x3B
#define  BORDER_WAVEFORM_CONTROL 0x3C
#define  SET_RAM_X_ADDRESS_START_END_POSITION 0x44
#define  SET_RAM_Y_ADDRESS_START_END_POSITION 0x45
#define  SET_RAM_X_ADDRESS_COUNTER 0x4E
#define  SET_RAM_Y_ADDRESS_COUNTER 0x4F
#define  TERMINATE_FRAME_READ_WRITE 0xFF

// User Functions
int EPD_Init_2in9(int fd, uint8_t Mode);	// Main function that initialiazes the display and sets the LUT.
int EPD_Clear_2in9(int fd);		//Clears the Display 				
int EPD_Display_Image_2in9(int fd, uint8_t* Image);		// Used to display an image on EPD, max resolution is 128x296
int EPD_Sleep_2in9(int fd);		// Display enters Sleep Mode 
int EPD_Wake_2in9(int fd, uint8_t mode);		// Wakes the Display from Sleep
int EPD_Turn_On_Display_2in9(int fd);	// Displays the on chip image buffer on the EPD Pannel. Pl fill buffer before using this function. 
//int EPD_Set_Orientation_2in9(int orient);	// Sets the Orientation of the Display 
int EPD_Set_Cursor_2in9(int fd, uint16_t Xs, uint16_t Ys);	// Sets the drawing cursor 
int EPD_Set_Display_Area_2in9(int fd, uint16_t Xs, uint16_t Ys, uint16_t Xe, uint16_t Ye);	// Sets the drawing display Area
int Display_Init_2in9(int fd, int Mode);	// Overall display Initialiazation function 
int NoBurnLoop(int fd);

#endif