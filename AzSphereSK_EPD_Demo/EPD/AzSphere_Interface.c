/*****************************************************************************
* | File      	:  	AzSphere_interface.c
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
* Change Log V0.4dbg (2020-05-04):
* 1. Added : Support for Waveshare/ Good Display 2.9" Bi-Colour EPD pannels with Display driver IL3820
*            Datasheet and reference materials can be found in Reference Section
* 2. Support for 128x296 Image Works, with the standard (orignal) LUT Provided by Waveshare
* 3. Both Standard and Partial Mode work
* 4. EPD Clear function can be called to avoid permanent damage if not using Sleep Mode.
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


//Global Includes
#include <time.h>
#include <errno.h>

#include "../HighLevelCore/applibs_versions.h"

#include <applibs/spi.h>
#include <applibs/gpio.h>
#include <applibs/log.h>

#include "AzSphere_Interface.h"
#include "hw/avnet_mt3620_sk.h"


//Global Variables
SPIMaster_Config config;
ssize_t spi_transferredBytes = -1;
uint8_t readGPIO = 255;

//Initialiazinf GPIO FD for EPd Display 
rstfd = -1;
dcfd = -1;
busyfd = -1;

//Constants Defined for Custom Delay Generation
const struct timespec delay_1s = { 1, 0 };
const struct timespec delay_1us = { 0, 1000 };
const struct timespec delay_1ms = { 0, 1000000 };

/*
Termination codes for this application. These are used for the
application exit code.  They they must all be between zero and 255,
where zero is reserved for successful termination.
*/
typedef enum {
    ExitCode_Success = 0,

    ExitCode_TermHandler_SigTerm = 1,

    ExitCode_Gpio_Open = 2,
    ExitCode_Gpio_Read = 3,

    ExitCode_Init_InitConfig = 4,
    ExitCode_Init_OpenSpiMaster = 5,
    ExitCode_Init_SetBusSpeed = 6,
    ExitCode_Init_SetMode = 7,
    ExitCode_SpiTransfers = 8,

    ExitCode_Spi_Read = 9,
    ExitCode_SPi_Write = 10

}ExitCode;

// Global Universal Functions Definitations
 
// Milisec delay generator
int delay_ms(uint32_t delay) {
    while (delay != 0) {
        nanosleep(&delay_1ms, NULL);
        --delay;
    }
    return ExitCode_Success;
}

// Microsec delay generator
int delay_us(uint32_t delay) {
    while (delay != 0) {
        nanosleep(&delay_1us, NULL);
        --delay;
    }
    return ExitCode_Success;
}

// Sec delay generator
int delay_s(uint32_t delay) {
    while (delay != 0) {
        nanosleep(&delay_1s, NULL);
        --delay;
    }
    return ExitCode_Success;
}


/******************************************************************************
function    : Initialization of GPIO Reqd for EPD Ref above for Pin Layouts
parameter   : NONE 
return      : Success Signal / Error Code 
******************************************************************************/
int EPD_gpio_init(void) {
    int ret;

    // initialiazing Extra Pins required for Display operation.
    // This is automatically calculated based on the Click Board parameter selected by your program 
    // Pl refer readme on how to use this. DO NOT MODIFY THIS IS AUTOMATICALLY SELECTED. 
    rstfd = GPIO_OpenAsOutput(RST_PIN, GPIO_OutputMode_PushPull, GPIO_Value_High);  //Reset Pin 
    dcfd = GPIO_OpenAsOutput(DC_PIN, GPIO_OutputMode_PushPull, GPIO_Value_High);    //Data/Comd Pin
    busyfd = GPIO_OpenAsInput(BUSY_PIN);  // Busy Pin

    if (rstfd == -1 || dcfd == -1 || busyfd == -1) {
        Log_Debug("ERROR : EPD_gpio_init : Could not open gpio Fd : Rstfd - %d , dcfd - %d , busyfd = %d\n", rstfd, dcfd, busyfd);
        return ExitCode_Gpio_Open;
    }

    // setting d/c to d mode
    ret = GPIO_SetValue(dcfd, GPIO_Value_High);
    if (ret != 0) {
        Log_Debug("ERROR : EPD_gpio_init : Could not set dcfd gpio\n");
        return ExitCode_Gpio_Open;
    }

    // Setting RST Pin to RESET Pos. 
    ret = GPIO_SetValue(rstfd, GPIO_Value_High);
    if (ret != 0) {
        Log_Debug("ERROR : EPD_gpio_init : Could not set RstFd gpio\n");
        return ExitCode_Gpio_Open;
    }

    return ExitCode_Success;
}

/******************************************************************************
function    : Resets EPD Display 
parameter   : NONE
return      : Success Signal / Error Code
******************************************************************************/
int EPD_reset(void) {
    int ret; 

    ret = GPIO_SetValue(rstfd, GPIO_Value_High);
    if (ret != 0) {
        Log_Debug("ERROR : EPD_reset : Could not set RstFd gpio\n");
        return ExitCode_Gpio_Open;
    }

    ret = GPIO_SetValue(rstfd, GPIO_Value_Low);
    if (ret != 0) {
        Log_Debug("ERROR : EPD_reset : Could not set RstFd gpio\n");
        return ExitCode_Gpio_Open;
    }

    delay_ms(200);
    ret = GPIO_SetValue(rstfd, GPIO_Value_High);
    if (ret != 0) {
        Log_Debug("ERROR : EPD_reset : Could not set RstFd gpio\n");
        return ExitCode_Gpio_Open;
    }
    // Kinna Over Kill
    /*
    ret = GPIO_SetValue(rstfd, GPIO_Value_Low);
    if (ret != 0) {
        Log_Debug("ERROR : EPD_reset : Could not set RstFd gpio\n");
        return ExitCode_Gpio_Open;
    }

    delay_ms(100);
    ret = GPIO_SetValue(rstfd, GPIO_Value_High);
    if (ret != 0) {
        Log_Debug("ERROR : EPD_reset : Could not set RstFd gpio\n");
        return ExitCode_Gpio_Open;
    }
    delay_ms(10);
    */
    return ExitCode_Success;
}
 
/******************************************************************************
function: Initialiases the SPI interface and returns the SPI FD
parameter:
    fd   :   File Discripter for SPI Interface to be intialiased by the user refer Readme
return      : Success Signal / Error Code
******************************************************************************/
int AzSphere_Spi_Init(int fd) {
    int ret = SPIMaster_InitConfig(&config);    // init spi 
    if (ret != 0) {
        Log_Debug("ERROR : AzSphere_Spi_Init : SPIMaster_InitConfig = %d\n", ret);
        return ExitCode_Init_InitConfig;
    }
    config.csPolarity = SPI_ChipSelectPolarity_ActiveLow;   // CS as active low refere EPD Datasheet 
    fd = SPIMaster_Open(ILI9341_SK_ISU1_SPI, ILI9341_SPI_CS, &config);
    if (fd < 0) {
        Log_Debug("ERROR: SPIMaster_Open : %d\n", fd);
        return ExitCode_Init_OpenSpiMaster;
    }

    ret =  SPIMaster_SetBitOrder(fd, SPI_BitOrder_MsbFirst);    // Set SPI Bit Order MSB First
    if (ret != 0) {
        Log_Debug("ERROR : AzSphere_Spi_Init : SPI_BitOrder_MsbFirst = %d\n", ret);
        return ExitCode_Init_InitConfig;
    }

    int result = SPIMaster_SetBusSpeed(fd, 400000); // Sets SPI Speed to HS 4Mz 
    if (result != 0) {
        Log_Debug("ERROR: SPIMaster_SetBusSpeed: %d\n", result);
        return ExitCode_Init_SetBusSpeed;
    }

    //result = SPIMaster_SetMode(fd, SPI_Mode_3);   // SPI mode 3: clock polarity (CPOL) = 1, clock phase (CPHA) = 1
    result = SPIMaster_SetMode(fd, SPI_Mode_0);     // SPI mode 0: clock polarity (CPOL) = 0, clock phase (CPHA) = 0
    if (result != 0) {
        Log_Debug("ERROR: SPIMaster_SetMode: %d\n", result);
        return ExitCode_Init_SetMode;
    }

    return fd; // returns FD as  this will be used by all other functions Ensure this is not -1; 
}

/******************************************************************************
function    : Checks if EPD is Busy
parameter   : NONE
return      : Success Signal / Error Code
******************************************************************************/
int EPD_Busy(void) {
    int ret;
    GPIO_Value_Type readgpio = 0;

    Log_Debug("INFO : EPD_Busy : e-Paper busy \n");   // to know that module is not busy
    
    ret = GPIO_GetValue(busyfd, &readgpio); // BUSY pin is High is Module is busy 
    if (ret < 0) {
        Log_Debug("ERROR : EPD_Busy : Failed to read GPIO Value : %d\n", ret);
        return ExitCode_Gpio_Read;
    }
    
    while (readgpio == GPIO_Value_High) {      //LOW: idle, HIGH: busy
        GPIO_GetValue(busyfd, &readgpio); // BUSY pin is High is Module is busy 
        //delay_ms(100);   //Not ideal but works so :)
    }
    
    Log_Debug("INFO : EPD_Busy : e-Paper busy released \n"); // to know that module is not busy 
    
    return ExitCode_Success;
}

/******************************************************************************
function    : Reads the Value from SPI Bus : Vestigial function as MOSI is NC in the Display Module. 
              Stores the Read data into *spiReadData for global use 
    NOTE        :   Havent Tested this Function !! USE AT OWN RISK !!
parameter   : 
        fd      :   SPi File Discripter
        reg     :   Register Addr from which value is to be read
        buf     :   data read will be in this buffer 
return      : Success Signal / Error Code
******************************************************************************/
int Spi_Read(int fd, uint8_t reg) {

    if (((uint8_t)SPIMaster_WriteThenRead(fd, &reg, 1, spiReadData, sizeof(*spiReadData))) < 0) {
        Log_Debug("ERROR: SPI Read : unable to WriteThenRead\n");
        return ExitCode_Spi_Read;
    }
    return ExitCode_Success;
}

//Writes Byte to Spi Bus 
/******************************************************************************
function    : Checks if EPD is Busy
parameter   : NONE
return      : No of Spi Bytes Transfered 
******************************************************************************/
int Spi_Write(int fd, uint8_t dc, uint8_t *buf, uint32_t len) {
    uint8_t ret = 0;
    const size_t       transferCount = 1;
    SPIMaster_Transfer transfers;

    int result = SPIMaster_InitTransfers(&transfers, transferCount);
    if (result != 0) {
        Log_Debug("ERROR : Spi_Write : Error Initialiazing SPI Transfers%d \n", result);
        return ExitCode_SpiTransfers;
    }
    
    transfers.flags = SPI_TransferFlags_Write;
    transfers.writeData = buf;
    transfers.length = len;     // To send multiple bytes over spi

    if (dc == 1) //if sending a Command
        GPIO_SetValue(dcfd, GPIO_Value_Low);

    spi_transferredBytes = SPIMaster_TransferSequential(fd, &transfers, transferCount);
    Log_Debug("INFO : Spi_Write : No of Bytes Transfered over SPIFD=%d : %d \n",fd, spi_transferredBytes);
    
    if (dc == 1) // changing to Data 
        GPIO_SetValue(dcfd, GPIO_Value_High);

    if (spi_transferredBytes == transfers.length) {
        Log_Debug("INFO : Spi_Write : Leaving SPIFD=%d : %d \n", fd, spi_transferredBytes);
        return spi_transferredBytes;        // Returning no of Tfr Bytes 
    }

    return ExitCode_SPi_Write;
}

// These functions are for ease of use 
/******************************************************************************
function    : Writes 01_Byte of data to Spi Bus; uses SPI Transfers which means that you cant read and write at same time.
parameter   : 
        fd      :   Spi Fd 
        buf     :   ONLY ONE BYTE (uint8_t) Data max to 0xFF can be sent using this function 
return      : No of Spi Bytes Transfered 
******************************************************************************/
int spiWriteData(int fd, uint8_t buf) {
    const size_t       transferCount = 1;
    SPIMaster_Transfer transfers;

    int result = SPIMaster_InitTransfers(&transfers, transferCount);
    if (result != 0) {
        Log_Debug("ERROR : Spi_Write : Error Initialiazing SPI Transfers%d \n", result);
        return -1;
    }

    transfers.flags = SPI_TransferFlags_Write;
    transfers.writeData = &buf;
    transfers.length = 1; //sizeof(buf);    // 1 byte data only 


    GPIO_SetValue(dcfd, GPIO_Value_High); // sending data 

    spi_transferredBytes = SPIMaster_TransferSequential(fd, &transfers, transferCount);
    //Log_Debug("INFO : Spi_Write : No of Bytes Transfered over SPIFD=%d : %d \n", fd, spi_transferredBytes);

    if (spi_transferredBytes == transfers.length) {
        //Log_Debug("INFO : Spi_Write : Leaving SPIFD=%d : %d \n", fd, spi_transferredBytes);
        return spi_transferredBytes;
    }
    Log_Debug("ERROR : Spi_Write_Data : Error sending Data over SPI \n");
    return ExitCode_SPi_Write;
}

/******************************************************************************
function    : Writes 01_Byte of Command to Spi Bus; uses SPI Transfers which means that you cant read and write at same time.
parameter   :
        fd      :   Spi Fd
        buf     :   ONLY ONE BYTE (uint8_t) command max to 0xFF can be sent using this function
return      : No of Spi Bytes Transfered 
******************************************************************************/
int spiWriteCmd(int fd, uint8_t buf) {
    const size_t       transferCount = 1;
    SPIMaster_Transfer transfers;

    int result = SPIMaster_InitTransfers(&transfers, transferCount);
    if (result != 0) {
        Log_Debug("ERROR : Spi_Write : Error Initialiazing SPI Transfers%d \n", result);
        return -1;
    }

    transfers.flags = SPI_TransferFlags_Write;
    transfers.writeData = &buf;
    transfers.length = 1; //sizeof(buf);    // fixed to 1 byte 

    GPIO_SetValue(dcfd, GPIO_Value_Low); // Sending Command 

    spi_transferredBytes = SPIMaster_TransferSequential(fd, &transfers, transferCount);
    //Log_Debug("INFO : Spi_Write : No of Bytes Transfered over SPIFD=%d : %d \n", fd, spi_transferredBytes);


    if (spi_transferredBytes == transfers.length) {
        //Log_Debug("INFO : Spi_Write : Leaving SPIFD=%d : %d \n", fd, spi_transferredBytes);
        return spi_transferredBytes;
    }

    Log_Debug("ERROR : Spi_Write_Cmd : Error sending Data over SPI \n");
    return ExitCode_SPi_Write;
}