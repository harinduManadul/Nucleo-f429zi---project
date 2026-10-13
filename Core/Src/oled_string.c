/*
 * OLED_pixel.c
 *
 *  Created on: Jan 4, 2021
 *      Author: LyLeo
 */
#include "oled_pixel.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>  // For memcpy
#include "fonts.h"

//
//  Write full character to screenbuffer
//
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color)
{
    ssd1306_WriteChar_int(ch, Font, color);

    // Return written char for validation
    return ch;
}



//
//  Write full string to screenbuffer
//
char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color)
{
   ssd1306_WriteString_int(str, Font,  color);

    // Everything ok
    return *str;
}