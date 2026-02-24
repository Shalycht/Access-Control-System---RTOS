/******************************************************************************
  @file     matrix.h
  @brief    LEDs WS2812 Matrix Driver for K64
  @author   Grupo Nº5
 ******************************************************************************/

#ifndef MATRIX_H_
#define MATRIX_H_

#include "defs.h"

//Matrix Specs
#define CANT_ROW_PIXELS 8
#define CANT_COLUMN_PIXELS 8
#define CANT_PIXELS (CANT_ROW_PIXELS*CANT_COLUMN_PIXELS)

//Matrix Pixels numbers example (8x8)

// Column 1  ... Column 8
// 1  2  3  4  5  6  7  8		Row 1
// 9  10 11 12 13 14 15 16		Row 2
// 17 18 19 20 21 22 23 24		Row 3
// 25 26 27 28 29 30 31 32		Row 4
// 33 34 35 36 37 38 39 40		Row 5
// 41 42 43 44 45 46 47 48		Row 6
// 49 50 51 52 53 54 55 56		Row 7
// 57 58 59 60 61 62 63 64		Row 8

/**
 * @brief Initialize Matrix
 */
void InitMatrix(void);

/**
 * @brief Turn on/off certain pixel
 * @param num: Pixel number (1 - 64)
 * @param color: RGB LED Color according to colores_t
 * @param bright: RGB LED Brightness according to brightness_t
 */
void setPixel(uint8_t num, colores_t color, brightness_t bright);

/**
 * @brief Turn on/off certain row of the Matrix
 * @param row: Matrix row (1 - 8)
 * @param color: RGB LED Color according to colores_t
 * @param bright: RGB LED Brightness according to brightness_t
 */
void setRow(uint8_t row, colores_t color, brightness_t bright);

/**
 * @brief Turn on/off certain column of the Matrix
 * @param col: Matrix column (1 - 8)
 * @param color: RGB LED Color according to colores_t
 * @param bright: RGB LED Brightness according to brightness_t
 */
void setColumn(uint8_t col, colores_t color, brightness_t bright);

/**
 * @brief Turn on/off all Matrix pixels
 * @param color: RGB LED Color according to colores_t
 * @param bright: RGB LED Brightness according to brightness_t
 */
void setAllPixels(colores_t color, brightness_t bright);

/**
 * @brief Turn off all Matrix pixels
 */
void clearMatrix(void);

/**
 * @brief Turn on a 2x2 square in the matrix
 * @param top_left: Top left pixel number
 * @param color: RGB LED Color according to colores_t
 * @param bright: RGB LED Brightness according to brightness_t
 */
void setSquare(uint8_t top_left, colores_t color, brightness_t bright);


#endif /* MATRIX_H_ */
