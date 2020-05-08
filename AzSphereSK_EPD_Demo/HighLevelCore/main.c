/*****************************************************************************
* | File      	:  	main.c
* | Author      :   GS Gill (gsgill112.github.io) @ TechNervers
* | Function    :   Main program to demonstrate the capb of EPD displays
* | Info        :   View the Demo on youtube 
*                   Link : 
*----------------
* |	This version:   V1.0
* | Date        :   2020-05-08
* | Info        :   -   -   -
* -----------------------------------------------------------------------------
* Change Log V0.4dbg (2020-05-04):
* 1. Added : Support for Waveshare/ Good Display 2.9" Bi-Colour EPD pannels with Display driver IL3820 
*            Datasheet and reference materials can be found in Reference Section 
* 2. Support for 128x296 Image Works, with the standard (orignal) LUT Provided by Waveshare
* 3. Both Standard and Partial Mode works
* 4. EPD Clear function can be called to avoid permanent damage if not using Sleep Mode. 
* 5. Waveshare's Gui Paint included as GFX Lib 
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

// GLobal includes 
#include <stdbool.h>
#include <errno.h>
#include <string.h>
//#include <time.h>
#include <stdlib.h>

#include "applibs_versions.h"

#include <applibs/log.h>
#include <applibs/spi.h>
#include "hw/avnet_mt3620_sk.h"
#include "../EPD/AzSphere_Interface.h"
#include "../Tools/Fonts/fonts.h"
#include "../EPD/ImageData.h"
#include "../Tools/GFX/Gfx.h"
//#include "../Tools/QRcode/qrcode.h"


// Spi File Discripter that will be used universally accross the Lib 
// (Ideally set it to -1 during inititialiazation in main())
int spiFd;

// Main Function 
int main(void)
{
    // Setting Spi File Discripter to -1
    spiFd = -1;

    // Using a Image Buffer that will be reused to construct any image data
    uint8_t* BlackImage;

    uint16_t WidthByte = EPD_WidthByte;
    uint16_t HeightByte = EPD_HeightByte;

    uint16_t Imagesize = WidthByte * HeightByte;
    BlackImage = (uint8_t*)malloc(Imagesize);
    //Creating a new Paint Object 
    Paint_NewImage(BlackImage, EPD_2in9_WIDTH, EPD_2in9_HEIGHT, 90, WHITE);
    // Selecting the Above Defined image 
    Paint_SelectImage(BlackImage);

    // Using Avnet SK Blue LED as a simple Blinky 
    int Ledfd = GPIO_OpenAsOutput(AVNET_MT3620_SK_USER_LED_BLUE, GPIO_OutputMode_PushPull, GPIO_Value_High);

    // Initialiazing Spi and returns the given File Discripter.
    spiFd = AzSphere_Spi_Init(spiFd);

    // Initialiazing the Display 
    Display_Init_2in9(spiFd, WaveShare_2in9_EPD_FULL);

    //Clearing the display 
    EPD_Clear_2in9(spiFd);
    delay_ms(1800);

    // GFX Example
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

    Paint_DrawPoint(230, 10, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    Paint_DrawPoint(230, 30, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    Paint_DrawPoint(260, 20, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);

    Paint_DrawLine(170, 70, 220, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(220, 70, 170, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);

    Paint_DrawRectangle(170, 70, 220, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(230, 70, 280, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    Paint_DrawCircle(195, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(255, 95, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    Paint_DrawLine(230, 95, 280, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(255, 75, 255, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);

    Paint_DrawString_EN(10, 0, "Hello From", &Font24, WHITE, BLACK);
    Paint_DrawString_EN(10, 30, "Tech", &Font24, WHITE, BLACK);
    Paint_DrawString_EN(80, 30, "NERVERS", &Font24, BLACK, WHITE);

    Paint_DrawNum(10, 60, 123456789, &Font8, BLACK, WHITE);
    Paint_DrawNum(10, 70, 987654321, &Font12, WHITE, BLACK);
    Paint_DrawNum(10, 85, 123456789, &Font16, BLACK, WHITE);
    Paint_DrawNum(10, 100, 987654321, &Font20, WHITE, BLACK);
    Log_Debug("GFX Demo\n");

    EPD_Display_Image_2in9(spiFd, BlackImage);
    delay_ms(1800);

    uint16_t  x, y;
    uint32_t Addr = 0;
    
    // Image 1 : WaveShare Logo 
    
    // This step is necessary for conversion of 8 pixels into the corrent format as defined by the EPD Init function.
    // Not Necessory if you are using Image2Lcd for converting your own images on the display as they are already 8 bits wide.  
    for (y = 0; y < HeightByte; y++) {
        for (x = 0; x < WidthByte; x++) {//8 pixel =  1 byte
            Addr = x + y * WidthByte;
            BlackImage[Addr] = WaveShareLogo_2in9[Addr]; //(Addr%5 == 0) ? 0xFF : 0x00;
        }
    }
    
    // Displays the Image 
    Log_Debug("WaveShare Logo Demo\n");
    EPD_Display_Image_2in9(spiFd, BlackImage);
    delay_ms(1800);

    //Initialiazing the Display in Partial Mode TEST 1
    EPD_Init_2in9(spiFd, WaveShare_2in9_EPD_PART);

    // Clearing the Display 
    // It seems that BUSY Pin is quite unpredictible and is confusing as the WaveShare Datasheet states it to be active Low 
    // and the WaveShare example Codes defines as active High ?? EPD_Clear or DisplayImage Functions may brake due to inadequate 
    // time for the display to update itself. SO, use delay_ms(1800); after both those functions.
    EPD_Clear_2in9(spiFd);
    delay_ms(1800);
    Log_Debug("Compleated Image Clear process\n");
    Paint_ClearWindows(0, 0, EPD_2in9_HEIGHT, EPD_2in9_WIDTH, WHITE); // This is to fill the buffer with All white pixels as the EPD will use its RAM Buffer 
    //On Screen Clock using Partial Display 
    PAINT_TIME sPaint_time;
    sPaint_time.Hour = 12;
    sPaint_time.Min = 34;
    sPaint_time.Sec = 56;
    uint8_t num = 20;
    for (;;) {
        sPaint_time.Sec = sPaint_time.Sec + 1;
        if (sPaint_time.Sec == 60) {
            sPaint_time.Min = sPaint_time.Min + 1;
            sPaint_time.Sec = 0;
            if (sPaint_time.Min == 60) {
                sPaint_time.Hour = sPaint_time.Hour + 1;
                sPaint_time.Min = 0;
                if (sPaint_time.Hour == 24) {
                    sPaint_time.Hour = 0;
                    sPaint_time.Min = 0;
                    sPaint_time.Sec = 0;
                }
            }
        }
        Paint_ClearWindows(100, 50, 100 + Font24.Width * 7, 50 + Font24.Height, WHITE);
        Paint_DrawTime(100, 50, &sPaint_time, &Font24, WHITE, BLACK);

        num = num - 1;
        if (num == 0) {
            break;
        }
        EPD_Display_Image_2in9(spiFd, BlackImage);
        delay_ms(500);//Analog clock 500ms
        Log_Debug("Partial Refresh\n");
    }
    
    //clearing the display 
    Paint_ClearWindows(0, 0, EPD_2in9_HEIGHT, EPD_2in9_WIDTH, WHITE);
    
    // Horse Animation Trial 
    // Image is 200x125 Px 
    // Because of the image size & the fact that the image background is white 
    // I am looping to fill the remaining bytes with 0xFF; to avoid the ghost of the image from the 
    // EPD Buffer.
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    for (y = 0; y < HeightByte; y++) {
        for (x = 0; x < WidthByte; x++) {//8 pixel =  1 byte
            Addr = x + y * WidthByte;
            if(Addr < 3200)
                BlackImage[Addr] = gImage_1[Addr]; //(Addr%5 == 0) ? 0xFF : 0x00;
            if (Addr >= 3200)
                BlackImage[Addr] = 0xFF;
        }
    }
    EPD_Display_Image_2in9(spiFd, BlackImage);
    //delay_ms(50);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    for (y = 0; y < HeightByte; y++) {
        for (x = 0; x < WidthByte; x++) {//8 pixel =  1 byte
            Addr = x + y * WidthByte;
            if (Addr < 3200)
                BlackImage[Addr] = gImage_2[Addr]; //(Addr%5 == 0) ? 0xFF : 0x00;
            if (Addr >= 3200)
                BlackImage[Addr] = 0xFF;
        }
    }
    EPD_Display_Image_2in9(spiFd, BlackImage);
    //delay_ms(50);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    for (y = 0; y < HeightByte; y++) {
        for (x = 0; x < WidthByte; x++) {//8 pixel =  1 byte
            Addr = x + y * WidthByte;
            if (Addr < 3200)
                BlackImage[Addr] = gImage_3[Addr]; //(Addr%5 == 0) ? 0xFF : 0x00;
            if (Addr >= 3200)
                BlackImage[Addr] = 0xFF;
        }
    }
    EPD_Display_Image_2in9(spiFd, BlackImage);
    //delay_ms(50);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    for (y = 0; y < HeightByte; y++) {
        for (x = 0; x < WidthByte; x++) {//8 pixel =  1 byte
            Addr = x + y * WidthByte;
            if (Addr < 3200)
                BlackImage[Addr] = gImage_4[Addr]; //(Addr%5 == 0) ? 0xFF : 0x00;
            if (Addr >= 3200)
                BlackImage[Addr] = 0xFF;
        }
    }
    EPD_Display_Image_2in9(spiFd, BlackImage);
    //delay_ms(50);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    for (y = 0; y < HeightByte; y++) {
        for (x = 0; x < WidthByte; x++) {//8 pixel =  1 byte
            Addr = x + y * WidthByte;
            if (Addr < 3200)
                BlackImage[Addr] = gImage_5[Addr]; //(Addr%5 == 0) ? 0xFF : 0x00;
            if (Addr >= 3200)
                BlackImage[Addr] = 0xFF;
        }
    }
    EPD_Display_Image_2in9(spiFd, BlackImage);
    //delay_ms(50);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    for (y = 0; y < HeightByte; y++) {
        for (x = 0; x < WidthByte; x++) {//8 pixel =  1 byte
            Addr = x + y * WidthByte;
            if (Addr < 3200)
                BlackImage[Addr] = gImage_6[Addr]; //(Addr%5 == 0) ? 0xFF : 0x00;
            if (Addr >= 3200)
                BlackImage[Addr] = 0xFF;
        }
    }
    EPD_Display_Image_2in9(spiFd, BlackImage);
    //delay_ms(50);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    for (y = 0; y < HeightByte; y++) {
        for (x = 0; x < WidthByte; x++) {//8 pixel =  1 byte
            Addr = x + y * WidthByte;
            if (Addr < 3200)
                BlackImage[Addr] = gImage_7[Addr]; //(Addr%5 == 0) ? 0xFF : 0x00;
            if (Addr >= 3200)
                BlackImage[Addr] = 0xFF;
        }
    }
    EPD_Display_Image_2in9(spiFd, BlackImage);
    //delay_ms(50);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    for (y = 0; y < HeightByte; y++) {
        for (x = 0; x < WidthByte; x++) {//8 pixel =  1 byte
            Addr = x + y * WidthByte;
            if (Addr < 3200)
                BlackImage[Addr] = gImage_8[Addr]; //(Addr%5 == 0) ? 0xFF : 0x00;
            if (Addr >= 3200)
                BlackImage[Addr] = 0xFF;
        }
    }
    EPD_Display_Image_2in9(spiFd, BlackImage);
    //delay_ms(50);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    for (y = 0; y < HeightByte; y++) {
        for (x = 0; x < WidthByte; x++) {//8 pixel =  1 byte
            Addr = x + y * WidthByte;
            if (Addr < 3200)
                BlackImage[Addr] = gImage_9[Addr]; //(Addr%5 == 0) ? 0xFF : 0x00;
            if (Addr >= 3200)
                BlackImage[Addr] = 0xFF;
        }
    }
    EPD_Display_Image_2in9(spiFd, BlackImage);
    //delay_ms(50);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    for (y = 0; y < HeightByte; y++) {
        for (x = 0; x < WidthByte; x++) {//8 pixel =  1 byte
            Addr = x + y * WidthByte;
            if (Addr < 3200)
                BlackImage[Addr] = gImage_10[Addr]; //(Addr%5 == 0) ? 0xFF : 0x00;
            if (Addr >= 3200)
                BlackImage[Addr] = 0xFF;
        }
    }
    EPD_Display_Image_2in9(spiFd, BlackImage);
    //delay_ms(50);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    for (y = 0; y < HeightByte; y++) {
        for (x = 0; x < WidthByte; x++) {//8 pixel =  1 byte
            Addr = x + y * WidthByte;
            if (Addr < 3200)
                BlackImage[Addr] = gImage_11[Addr]; //(Addr%5 == 0) ? 0xFF : 0x00;
            if (Addr >= 3200)
                BlackImage[Addr] = 0xFF;
        }
    }
    EPD_Display_Image_2in9(spiFd, BlackImage);
    //delay_ms(50);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    for (y = 0; y < HeightByte; y++) {
        for (x = 0; x < WidthByte; x++) {//8 pixel =  1 byte
            Addr = x + y * WidthByte;
            if (Addr < 3200)
                BlackImage[Addr] = gImage_12[Addr]; //(Addr%5 == 0) ? 0xFF : 0x00;
            if (Addr >= 3200)
                BlackImage[Addr] = 0xFF;
        }
    }
    EPD_Display_Image_2in9(spiFd, BlackImage);
    //delay_ms(50);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    Paint_ClearWindows(0, 0, 205, EPD_2in9_WIDTH, WHITE);
    for (y = 0; y < HeightByte; y++) {
        for (x = 0; x < WidthByte; x++) {//8 pixel =  1 byte
            Addr = x + y * WidthByte;
            if (Addr < 3200)
                BlackImage[Addr] = gImage_13[Addr]; //(Addr%5 == 0) ? 0xFF : 0x00;
            if (Addr >= 3200)
                BlackImage[Addr] = 0xFF;
        }
    }
    EPD_Display_Image_2in9(spiFd, BlackImage);
    //delay_ms(50);

    EPD_Init_2in9(spiFd, WaveShare_2in9_EPD_FULL);
    EPD_Clear_2in9(spiFd);

    // Image 2 My Logo 

    EPD_Display_Image_2in9(spiFd, TechNerversLogo_2in9);
    delay_ms(1800);

    Log_Debug("Image update process Compleated, Freeing the Memory \n");
    free(BlackImage);

    // Gets the EPD in Sleep Mode 
    // I have to check this but apparently on calling the Wake function the EPD Does Not Wake ? ? 
    // So do we have to re Init the EPD ??, but the Sleep works as expected. 
    EPD_Sleep_2in9(spiFd);

    while (true) {
        // We Love Blinky or as the Experts like to call them : HelloWorld :) 
        GPIO_SetValue(Ledfd, GPIO_Value_Low);
        delay_ms(500);
        GPIO_SetValue(Ledfd, GPIO_Value_High);
        delay_ms(500);
    }
}
