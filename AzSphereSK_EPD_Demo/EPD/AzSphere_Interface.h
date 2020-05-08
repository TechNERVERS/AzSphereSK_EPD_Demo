/*****************************************************************************
* | File      	:  	AzSphere_interface.h
* | Author      :   GS Gill (gsgill112.github.io) @ TechNervers
* | Function    :   Interface functions to create an abstract layer which would be used by all Driver files.
*                   Supports EPD Display by WaveShare electronics and Good Display EPD Pannels
* | Info        :   This file is specific to AzSphere Device line
* | NOTE        :   There is No Need to modify this interface lib to change EPD parameters.
*----------------
* |	This version:   V1.0
* | Date        :   2020-05-08
* | Info        :
* -----------------------------------------------------------------------------
* Change Log V1.0 (2020-05-08):
* 1. Fixed : Reset Bug 
* 2. Deep Sleep Mode Mow Works. 
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
#
******************************************************************************/
#ifndef AzSphere_EPD_INTERFACE_H
#define AzSphere_EPD_INTERFACE_H

//Global Includes
#include <errno.h>

#include "../HighLevelCore/applibs_versions.h"

#include <applibs/spi.h>
#include <applibs/gpio.h>
#include <applibs/log.h>

#include "hw/avnet_mt3620_sk.h"

// Definitions for which EPD Display are you using, You can use multiple displays also.
// Configure this in applibs_version.h File created by default.
#ifdef EPD_2in9_H	//Including Files and Parameterss coorosponding to EPD 2.9" Display 
#include "IL3820_2in9_Driver.h"
#endif

#ifdef EPD_4in2_H	//Including Files and Parameterss coorosponding to EPD 4.2" Display 
#include "IL0398_4in2_Driver.h"
#endif

// Definitions for external Pins based on Click Slot Configured
// Configure the Slot in applibs_version.h File created by default.
#if CLICK_SLOT == 2 
#define DC_PIN AVNET_MT3620_SK_GPIO2
#define BUSY_PIN  AVNET_MT3620_SK_GPIO1
#define RST_PIN AVNET_MT3620_SK_GPIO17    //Connected to RST Pin on mikroBus Slot 2
#else
#define DC_PIN AVNET_MT3620_SK_GPIO2
#define BUSY_PIN  AVNET_MT3620_SK_GPIO0
#define RST_PIN AVNET_MT3620_SK_GPIO16    //Connected to RST Pin on mikroBus Slot 1
#endif

#define EPD_COMMAND 1	//Sending if command /Data 
#define EPD_DATA 0		//Sending if command /Data 

//Defined Variables and Access Pin FD accross EPD Display 
extern int rstfd, dcfd, busyfd;
extern uint8_t *spiReadData;
extern uint8_t readGPIO;
extern ssize_t spi_transferredBytes;

// Some Global  Universal Functions
int delay_ms(uint32_t delay);  // Milisec delay generator
int delay_us(uint32_t delay);  // Microsec delay generator
int delay_s(uint32_t delay);	  // Sec delay generator
int EPD_gpio_init(void);   // Initialization of GPIO Reqd for EPD Ref above for Pin Layouts 
int EPD_reset(void);   // Initialization of GPIO Reqd for EPD Ref above for Pin Layouts
int AzSphere_Spi_Init(int fd);  // returns the SPI FD, for further use 
int EPD_Busy(void);   // Checks weather EPD IS Busy or Not (You can use Fixed Delay if Not using the pin
int Spi_Write(int fd, uint8_t dc, uint8_t *buf, uint32_t len); //Writes Byte to Spi Bus 
int Spi_Read(int fd, uint8_t reg); // Reads the Value from SPI Bus : Vestigial function as MOSI is NC in the Display Module. 
int spiWriteCmd(int fd, uint8_t buf);   // Added for Simplicity of code 
int spiWriteData(int fd, uint8_t buf);  // Added for Simplicity of code 

#endif