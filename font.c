/*
 * font.c
 *
 *  Created on: May 20, 2014
 *      Author: ahmed
 */

#include <stdio.h>
#include "font.h"
#include "st7565.h"

/**************************************************************************/
/*                                                                        */
/* ----------------------- Private Methods ------------------------------ */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/*!
 @brief  Draws a single bitmap character
 */
/**************************************************************************/
void fontsDrawCharBitmap(const uint8_t xPixel, const uint8_t yPixel,
		uint16_t color, const char *glyph, uint8_t cols, uint8_t rows) {
	uint16_t currentY, currentX, indexIntoGlyph;
	uint16_t _row, _col;

	// set initial current y
	currentY = yPixel;
	currentX = xPixel;

	for (_row = 0; _row < rows; _row++) {
		for (_col = 0; _col < cols; _col++) {
			if (_row == 0)
				indexIntoGlyph = _col;
			else
				indexIntoGlyph = (_row * cols) + _col;

			currentY = yPixel + (_row * 8);
			currentX = xPixel + _col;
			// send the data byte
			if (glyph[indexIntoGlyph] & (0X80))
				st7565_drawPixel(currentX, currentY);
			else
				st7565_clearPixel(currentX, currentY);
			if (glyph[indexIntoGlyph] & (0X40))
				st7565_drawPixel(currentX, currentY + 1);
			else
				st7565_clearPixel(currentX, currentY + 1);
			if (glyph[indexIntoGlyph] & (0X20))
				st7565_drawPixel(currentX, currentY + 2);
			else
				st7565_clearPixel(currentX, currentY + 2);
			if (glyph[indexIntoGlyph] & (0X10))
				st7565_drawPixel(currentX, currentY + 3);
			else
				st7565_clearPixel(currentX, currentY + 3);
			if (glyph[indexIntoGlyph] & (0X08))
				st7565_drawPixel(currentX, currentY + 4);
			else
				st7565_clearPixel(currentX, currentY + 4);
			if (glyph[indexIntoGlyph] & (0X04))
				st7565_drawPixel(currentX, currentY + 5);
			else
				st7565_clearPixel(currentX, currentY + 5);
			if (glyph[indexIntoGlyph] & (0X02))
				st7565_drawPixel(currentX, currentY + 6);
			else
				st7565_clearPixel(currentX, currentY + 6);
			if (glyph[indexIntoGlyph] & (0X01))
				st7565_drawPixel(currentX, currentY + 7);
			else
				st7565_clearPixel(currentX, currentY + 7);
		}
	}
}

/**************************************************************************/
/*                                                                        */
/* ----------------------- Public Methods ------------------------------- */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/*!
 @brief  Draws a string using the supplied font

 @param[in]  x
 Starting x co-ordinate
 @param[in]  y
 Starting y co-ordinate
 @param[in]  color
 Color to use when rendering the font
 @param[in]  fontInfo
 Pointer to the FONT_INFO to use when drawing the string
 @param[in]  str
 The string to render

 @section Example

 @code

 #include "drivers/displays/tft/fonts/dejavusans9.h"

 fontsDrawString(0, 90,  COLOR_BLACK, &dejaVuSans9ptFontInfo, "DejaVu Sans 9");
 fontsDrawString(0, 105, COLOR_BLACK, &dejaVuSans9ptFontInfo, "123456789012345678901234567890");

 @endcode
 */
/**************************************************************************/
void fontsDrawString(uint8_t x, uint8_t y, uint16_t color,
		const FONT_INFO *fontInfo, char *str) {
	uint8_t currentX, charWidth, characterToOutput;
	const FONT_CHAR_INFO *charInfo;
	uint16_t charOffset;

	// set current x, y to that of requested
	currentX = x;

	// while not NULL
	while (*str != '\0') {
		// get character to output
		characterToOutput = *str;

		// get char info
		charInfo = fontInfo->charInfo;

		// some fonts have character descriptors, some don't
		if (charInfo != NULL) {
			// get correct char offset
			charInfo += (characterToOutput - fontInfo->startChar);

			// get width from char info
			charWidth = charInfo->widthBits;

			// get offset from char info
			charOffset = charInfo->offset;
		} else {
			// if no char info, char width is always 5
			charWidth = 5;

			// char offset - assume 5 * letter offset
			charOffset = (characterToOutput - fontInfo->startChar) * 5;
		}

		// Send individual characters
		// We need to manually calculate width in pages since this is screwy with variable width fonts
		//uint8_t heightPages = charWidth % 8 ? charWidth / 8 : charWidth / 8 + 1;
		fontsDrawCharBitmap(currentX, y, color,
				(const char *) (&fontInfo->data[charOffset]), charWidth,
				fontInfo->height);

		// next char X
		currentX += charWidth + 1;

		// next char
		str++;
	}
}

/**************************************************************************/
/*!
 @brief  Returns the width in pixels of a string when it is rendered

 This method can be used to determine whether a string will fit
 inside a specific area, or if it needs to be broken up into multiple
 lines to be properly rendered on the screen.

 This function only applied to bitmap fonts (which can have variable
 widths).  All smallfonts (if available) are fixed width and can
 easily have their width calculated without costly functions like
 this one.

 @param[in]  fontInfo
 Pointer to the FONT_INFO for the font that will be used
 @param[in]  str
 The string that will be rendered
 */
/**************************************************************************/
uint16_t fontsGetStringWidth(const FONT_INFO *fontInfo, char *str) {
	uint16_t width = 0;
	uint32_t currChar;
	uint32_t startChar = fontInfo->startChar;

	// until termination
	for (currChar = *str; currChar; currChar = *(++str)) {
		// if char info exists for the font, use width from there
		if (fontInfo->charInfo != NULL) {
			width += fontInfo->charInfo[currChar - startChar].widthBits + 1;
		} else {
			width += 5 + 1;
		}
	}

	/* return the width */
	return width > 0 ? width - 1 : width;
}

