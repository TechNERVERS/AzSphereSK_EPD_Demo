/*****************************************************************************
* | File      	:  	IL3820_2in9_Driver.c
* | Author      :   GS Gill (gsgill112.github.io) @ TechNervers
* | Function    :   Main Driver File for WaveShare/ Good Display 2.9" bi-color EPD display.
*                   Based on Waveshare driver and orignal Waveshare LUT's. Will add user LUT afetr testing them.
*                   Demo @YouTube Link : 
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

// Global Includes 
#include <errno.h>

#include "../HighLevelCore/applibs_versions.h"

#include <applibs/spi.h>
#include <applibs/gpio.h>
#include <applibs/log.h>

#include "AzSphere_Interface.h"
#include "IL3820_2in9_Driver.h"

/// <summary>
/// Termination codes for this application. These are used for the
/// application exit code.  They they must all be between zero and 255,
/// where zero is reserved for successful termination.
/// </summary>
typedef enum {
    ExitCode_Success = 0,

    ExitCode_TermHandler_SigTerm = 1,

    ExitCode_SPI_WriteData = 2,
    ExitCode_SPI_ReadData = 3,
   
    ExitCode_GPIO_ReadValue = 4,
    ExitCode_GPIO_WriteValue = 5,

    ExitCode_RST = 6
}ExitCode;

// LUT Values for 2.9" EPd Display 
// Origanl WaveShare Full Update LUT Table 
const unsigned char EPD_2IN9_lut_full_update[] = {
    0x50, 0xAA, 0x55, 0xAA, 0x11, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFF, 0xFF, 0x1F, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// Origanl WaveShare Partial Update LUT Table 
const unsigned char EPD_2IN9_lut_partial_update[] = {
    0x10, 0x18, 0x18, 0x08, 0x18, 0x18,
    0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x13, 0x14, 0x44, 0x12,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// u8g2 library Full Update LUT Table 
const unsigned char LUTDefault_full[] = { 
    0x32,0x02,0x02,0x01,0x11,0x12,0x12,
    0x22,0x22,0x66,0x69,0x69,0x59,0x58,
    0x99,0x99,0x88,0x00,0x00,0x00,0x00,
    0xF8,0xB4,0x13,0x51,0x35,0x51,0x51,
    0x19,0x01,0x00 
};
// u8g2 library Partial Update LUT Table 
const unsigned char LUTDefault_part[] = { 
    0x32,0x10,0x18,0x18,0x08,0x18,0x18,
    0x08,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x13,0x14,0x44,0x12,0x00,0x00,0x00,
    0x00,0x00,0x00 
};

// User Functions 

int NoBurnLoop(int fd) {
    while(1){
        EPD_Clear_2in9(fd);
        delay_ms(500);
    }
}
/******************************************************************************
function    : Wakes EPD Display from Deep Sleep Mode
parameter   : fd : SPI File Discripter used for read/Write() functions 
return      : Success Signal / Error Code
******************************************************************************/
int EPD_Wake_2in9(int fd, uint8_t mode) {
    int ret = -1;
    Log_Debug("Entering Wake Routine\n");
    ret = EPD_reset();
    ret = EPD_Init_2in9(fd, mode);
    //Disable deep sleep Mode 
    ret = spiWriteCmd(fd, 0x10);
    ret = spiWriteData(fd, 0x00);
    EPD_Busy();
    if (ret < 0) {
        Log_Debug("ERROR : EPD_Wake_2in9 : Error Comming out of DEEP_SLEEP_MODE \n");
        return ExitCode_SPI_WriteData;
    }  

    Log_Debug("INFO : EPD_Wake_2in9 : EPD Waking from DEEP_SLEEP_MODE\n");
    delay_ms(50);

    return ExitCode_Success;
}

/******************************************************************************
function    : Main Initialization routine for the Display
parameter   : 
       fd   :   SPI File Discripter used for read/Write() functions
       Mode :   Defines which LUT Table will you use
return      : Success Signal / Error Code
******************************************************************************/
int EPD_Init_2in9(int fd, uint8_t Mode) {
    int ret;

    // Setting DRIVER_OUTPUT_CONTROL
    ret = spiWriteCmd(fd, 0x01);
    ret = spiWriteData(fd, ((EPD_2in9_HEIGHT - 1) & 0xFF));
    ret += spiWriteData(fd, (((EPD_2in9_HEIGHT - 1) >> 8) & 0xFF));
    ret += spiWriteData(fd, 0x00); // GD = 0; SM = 0; TB = 0;

    // Gate Driving Voltage : Testing Only 
    //  ret = spiWriteCmd(fd, 0x03);
    //  ret = spiWriteData(fd, 0x00);

    // Source Driving Violtage  : Testing Only 
    //  ret = spiWriteCmd(fd, 0x04);
    //  ret = spiWriteData(fd, 0x0A);

    // BOOSTER_SOFT_START_CONTROL
    ret = spiWriteCmd(fd, 0x0C);
    ret = spiWriteData(fd, 0xD7);
    ret += spiWriteData(fd, 0xD6);
    ret += spiWriteData(fd, 0x9D);

    // WRITE_VCOM_REGISTER
    ret = spiWriteCmd(fd, 0x2C);
    ret = spiWriteData(fd, 0xA8); //vcom 7C

    // SET_DUMMY_LINE_PERIOD
    ret = spiWriteCmd(fd, 0x3A);
    ret = spiWriteData(fd, 0x1A); //4 dummy lines per gate  

    // SET_GATE_TIME
    ret = spiWriteCmd(fd, 0x3B);
    ret = spiWriteData(fd, 0x08); //2us per line 

    // BORDER_WAVEFORM_CONTROL
    ret = spiWriteCmd(fd, 0x3C);
    ret = spiWriteData(fd, 0x03);
    // DATA_ENTRY_MODE_SETTING
    ret = spiWriteCmd(fd, 0x11);
    ret = spiWriteData(fd, 0x03);

    // WRITE_LUT_REGISTER
    ret = spiWriteCmd(fd, 0x32);
    // Switches the Mode of operation by changing the LUT values; Will add more LUT Tables after testing them. 
    switch (Mode) {                    
        case WaveShare_2in9_EPD_FULL :              // Origanl Waveshare LUT for Full update 
            for (uint16_t i = 0; i < 30; i++) {
                ret += spiWriteData(fd, EPD_2IN9_lut_full_update[i]);
            }
            break;
        case WaveShare_2in9_EPD_PART : 
            for (uint16_t i = 0; i < 30; i++) {
                ret += spiWriteData(fd, EPD_2IN9_lut_partial_update[i]);
            }
            break;
        case U8G2_2in9_EPD_FULL : 
            for (uint16_t i = 0; i < 30; i++) {
                ret += spiWriteData(fd, LUTDefault_full[i]);
            }
            break;
        case U8G2_2in9_EPD_PART:
            for (uint16_t i = 0; i < 30; i++) {
                ret += spiWriteData(fd, LUTDefault_part[i]);
            }
            break;
        default :   Log_Debug("ERROR : EPD_Init_2in9 : ONLY Mode is EPD_2IN9_FULL or EPD_2IN9_PART");
    }

    delay_ms(500);
    //ret = EPD_Busy(); // wait for EPD to configure properly 
    return ExitCode_Success;
}

 
/******************************************************************************
function    : Clears the Display 
parameter   :
       fd   :   SPI File Discripter used for read/Write() functions
return      : Success Signal / Error Code
******************************************************************************/
int EPD_Clear_2in9(int fd) {
    uint16_t W, H;
    int ret = -1;

    W = EPD_WidthByte;
    H = EPD_HeightByte;

    EPD_Set_Display_Area_2in9(fd, 0, 0, EPD_2in9_WIDTH, EPD_2in9_HEIGHT);

    for (uint16_t j = 0; j < H; j++) {
        EPD_Set_Cursor_2in9(fd, 0, j);
        ret += spiWriteCmd(fd, 0x24);
        for (uint16_t i = 0; i < W; i++) {
            ret += spiWriteData(fd, 0xFF);
        }
    }
    
    delay_ms(100);
    EPD_Turn_On_Display_2in9(fd);
    return ExitCode_Success;
}

/******************************************************************************
function    : Routine to Display images onthe display 
parameter   :
       fd    :   SPI File Discripter used for read/Write() functions
       Image :   Image pointer in the proper format 8 pixels = 1 byte 
return      : Success Signal / Error Code
******************************************************************************/
int EPD_Display_Image_2in9(int fd, uint8_t *Image) {
    uint16_t W, H;
    uint32_t Addr = 0;
    int ret = -1;

    W = EPD_WidthByte;
    H = EPD_HeightByte;

    EPD_Set_Display_Area_2in9(fd, 0, 0, EPD_2in9_WIDTH, EPD_2in9_HEIGHT);

    for (uint16_t j = 0; j < H; j++) {
        EPD_Set_Cursor_2in9(fd, 0, j);
        ret += spiWriteCmd(fd, 0x24);
        for (uint16_t i = 0; i < W; i++) {
            Addr = i + j * W;
            ret += spiWriteData(fd, Image[Addr]);
        }
    }

    delay_ms(100);
    EPD_Turn_On_Display_2in9(fd);
    return ExitCode_Success;
}
 
/******************************************************************************
function    : Puts the Display in Deep Sleep Mode
parameter   :
       fd    :   SPI File Discripter used for read/Write() functions
return      : Success Signal / Error Code
******************************************************************************/
int EPD_Sleep_2in9(int fd) {
    int ret = -1;
    //DEEP_SLEEP_MODE Register
    ret = spiWriteCmd(fd, 0x10);
    if (ret < 0) {
        Log_Debug("ERROR : EPD_Sleep_2in9 : DEEP_SLEEP_MODE CMD Bytes Tfr ERROR\n");
        return ExitCode_SPI_WriteData;
    }
    // Enabling EPD Sleep Mode
    ret = spiWriteData(fd, 0x01);
    if (ret < 0) {
        Log_Debug("ERROR : EPD_Sleep_2in9 : DEEP_SLEEP_MODE Data Bytes Tfr ERROR\n");
        return ExitCode_SPI_WriteData;
    }
    //EPD_Busy();
    Log_Debug("INFO : EPD_Sleep_2in9 : EPD Entering DEEP_SLEEP_MODE\n");
    return ExitCode_Success;
}

/******************************************************************************
function    : Turns on the display -- Puts the content of On Chip RAM/ buffer to the EPD Pannel 
parameter   :
       fd    :   SPI File Discripter used for read/Write() functions
return      : Success Signal / Error Code
******************************************************************************/
int EPD_Turn_On_Display_2in9(int fd) {
    int ret;

    // DISPLAY_UPDATE_CONTROL_2
    ret = spiWriteCmd(fd, 0x22);
    ret = spiWriteData(fd, 0xC4);
    // MASTER_ACTIVATION
    ret = spiWriteCmd(fd, 0x20);
    // TERMINATE_FRAME_READ_WRITE
    ret = spiWriteCmd(fd, 0xFF);
    
    delay_ms(100);
    ret = EPD_Busy();
    //delay_ms(1800);

    return ExitCode_Success;
}

/******************************************************************************
NOTE    :    NOT YET IMPLEMENTED 
function    : Change sthe Orientation of the display 
parameter   :
     orient    :    Convert to Potrait or Landscape Mode
return      : Success Signal / Error Code
******************************************************************************/
int EPD_Set_Orientation_2in9( int orient) {
    if (orient == EPD_2in9_ORIENT_POTRAIT) {
        max_w = 128;
        max_h = 296;
    }
    else if (orient == EPD_2in9_ORIENT_LANDSCAPE) {
        max_h = 128;
        max_w = 296;
    }
    else Log_Debug("WARNING : ILLIGAL ORIENTATION TYPE USING DEFAULT ORIENTATION LANDSCAPE");
    max_w = 296;
    max_h = 128;

    return ExitCode_Success;
}

/******************************************************************************
function    : Set the Cursor Loc on the Display
parameter   :
       fd    :   SPI File Discripter used for read/Write() functions
       Xs    :   X Start Point
       Ys    :   Y Start Point 
return      : Success Signal / Error Code
******************************************************************************/
int EPD_Set_Cursor_2in9(int fd, uint16_t Xs, uint16_t Ys) {
    int ret;
    // SET_RAM_X_ADDRESS_COUNTER
    ret = spiWriteCmd(fd, 0x4E);
    ret = spiWriteData(fd, ((Xs >> 3) & 0xFF));
    //ret = spiWriteData(fd, (Xs & 0xFF));

    // SET_RAM_Y_ADDRESS_COUNTER
    ret = spiWriteCmd(fd, 0x4F);
    ret = spiWriteData(fd, (Ys & 0xFF));
    ret = spiWriteData(fd, ((Ys >> 8) & 0xFF));

    return ExitCode_Success;
}
 
/******************************************************************************
function    : Sets the Display Window For Display 
parameter   :
       fd    :   SPI File Discripter used for read/Write() functions
       Xs   :   X Start Point
       Ys   :   Y Start Point
       Xe   :   X End Point 
       Ye   :   Y End Point 
return      : Success Signal / Error Code
******************************************************************************/
int EPD_Set_Display_Area_2in9(int fd, uint16_t Xs, uint16_t Ys, uint16_t Xe, uint16_t Ye) {
    int ret;
    // SET_RAM_X_ADDRESS_START_END_POSITION
    ret = spiWriteCmd(fd, 0x44);
    ret = spiWriteData(fd, ((Xs >> 3) & 0xFF));
    ret = spiWriteData(fd, ((Xe >> 3) & 0xFF));

    // SET_RAM_Y_ADDRESS_START_END_POSITION
    ret = spiWriteCmd(fd, 0x45);
    ret = spiWriteData(fd, (Ys & 0xFF));
    ret = spiWriteData(fd, ((Ys >> 8) & 0xFF));
    ret = spiWriteData(fd, (Ye & 0xFF));
    ret = spiWriteData(fd, ((Ye >> 8) & 0xFF));
    //ret = EPD_Busy();
    delay_ms(100);
    return ExitCode_Success;
}

// Does everything 
/******************************************************************************
function    : Does everything from Start 
parameter   :
       fd   :   SPI File Discripter used for read/Write() functions
       Mode :   Defines which LUT Table will you use
return      : Success Signal / Error Code
******************************************************************************/
int Display_Init_2in9(int fd, int mode) {
    int ret;
    // Init GPIO
    ret = EPD_gpio_init();  

    //Resetting the Display 
    ret = EPD_reset();
    delay_ms(50);

    // Wakes the Display if it ws sleeping prior to using this command
    ret = EPD_Wake_2in9(fd, mode);

    // Initialiaze the Display 
    ret = EPD_Init_2in9(fd, mode);

    Log_Debug("Initialiazed the Display\n");
    return ExitCode_Success;
}