/*****************************************************************************
* | File      	:  	IamgeData.h
* | Author      :   GS Gill (gsgill112.github.io) @ TechNervers
* | Function    :   Comtains static image data from Waveshare Reference code as well as My own created images.
*                   Supports EPD Display by WaveShare electronics and Good Display EPD Pannels
* | Info        :   can be used to test the display
* | NOTE        :   FOR custom Images use image to lcd from links below, and tutorial for how to use this will be on my blog
*					technervers.blogspot.in and I will do a short Youtube Video on the same check that out on my youtube. 
*					Links will be added shortly
*					Main Link : https://www.waveshare.com/wiki/File:Image2Lcd.7z
*					Alternate :	https://www.electronoobs.com/images/Arduino/tut_57/image2LCD.zip
*
*----------------
* |	This version:   V0.4dbg
* | Date        :   2020-05-04
* | Info        :
* -----------------------------------------------------------------------------
* Change Log V0.4dbg (2020-05-04):
* 1. Added : TechNERVERS and Waveshare Logos for 2.9" EPS in b/w bi-colour format (you need to convert this to 8 bit format before sending this to EPD check reference main.c code
*            TechNERVERS and Waveshare Logos for 4.2" EPS in b/w bi-colour format (you need to convert this to 8 bit format before sending this to EPD check reference main.c code
*			 TechNERVERS Logos for 4.2" EPD in 4-colour grey image format for b/w bi-colour EPD
*			 02x Waveshare Logos for 4.92" EPD in 4-colour grey image format for b/w bi-colour EPD
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
#ifndef ImageData_H
#define ImageData_H

// Image Data Dump for waveshare Displays 
// Supported displays as of rev0.4dbg are
// 2.9" bicolour display (can be hacked to get Grey Scale 
// 4.2" bicolour display, capable of displaying grey scale

extern const unsigned char TechNerversLogo_2in9[]; //4736
extern const unsigned char WaveShareLogo_2in9[]; //4736
extern const unsigned char gImage_4in2[];
extern const unsigned char gImage_4in2_4Gray[];
extern const unsigned char gImage_4in2_4Gray1[];
extern const unsigned char TechnerversProjectLogo_4in2[8700];
extern const unsigned char TechnerversLogo_4in2_grey[17100];
extern const unsigned char gImage_1[];
extern const unsigned char gImage_2[];
extern const unsigned char gImage_3[];
extern const unsigned char gImage_4[];
extern const unsigned char gImage_5[];
extern const unsigned char gImage_6[];
extern const unsigned char gImage_7[];
extern const unsigned char gImage_8[];
extern const unsigned char gImage_9[];
extern const unsigned char gImage_10[];
extern const unsigned char gImage_11[];
extern const unsigned char gImage_12[];
extern const unsigned char gImage_13[];

#endif