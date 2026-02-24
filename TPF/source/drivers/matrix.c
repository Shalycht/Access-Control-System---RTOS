/******************************************************************************
  @file     matrix.c
  @brief    LEDs WS2812 Matrix Driver Functions
  @author   Grupo Nº5
 ******************************************************************************/

#include "hardware.h"
#include "matrix.h"
#include "PWM.h"
#include "dma.h"

//WS2812 Matrix related constants
#define BITS_LED 8
#define BITS_PIXEL BITS_LED*3
#define CANT_BITS_TOTAL (CANT_PIXELS*BITS_PIXEL)

//WS2812 PWM data signal realted constants
#define PWM_MOD 62
#define RESET_COUNT 80	//Para cumplir con los 50us del Reset
#define PWM_DC_0 1		//Para el Reset
#define PWM_DC_30 19	//Corresponde a un 0 lógico
#define PWM_DC_70 43	//Corresponde a un 1 lógico

//Más Constantes
#define CANT_DUTYS (CANT_BITS_TOTAL + RESET_COUNT)
#define LSB_MASK 0x01

//Dutys Array
uint16_t bits_dutys[CANT_DUTYS];

//RGB Colors intensity
static uint8_t red;
static uint8_t green;
static uint8_t blue;

//Prototipos
static void InitDutys(void);
static void setColor(colores_t color, brightness_t bright);

//InitMatrix
void InitMatrix(void)
{
	//Init desired PWM for the WS2812 data signal
	PWM_Init(PSC_x1, PWM_MOD);

	//Init Duty values to logic's 0 (All pixels off)
	InitDutys();

	//Init DMA
	DMA_Init(bits_dutys, CANT_DUTYS, sizeof(bits_dutys));
}

//InitDutys
static void InitDutys(void)
{
	//Initial Duty values correspond to logic's 0 (All pixels off)
	for(uint16_t i = 0; i < CANT_BITS_TOTAL; i++)
	{
		bits_dutys[i] = PWM_DC_30;
	}

	//Dutys according to the reset command
	for(uint16_t i = CANT_BITS_TOTAL; i < CANT_DUTYS; i++)
	{
		bits_dutys[i] = PWM_DC_0;
	}
}

//setPixel
void setPixel(uint8_t num, colores_t color, brightness_t bright)
{
	setColor(color, bright);

	uint16_t index = (num-1) * BITS_PIXEL;

	//First set Green bits (starting with the MSB)
	for(int i = 0; i < BITS_LED; i++)
	{
		bits_dutys[index+i] = (((green>>(BITS_LED-1-i)) & LSB_MASK) == 1) ? PWM_DC_70 : PWM_DC_30;
	}

	//Then set Red bits (starting with the MSB)
	for(int i = 0; i < BITS_LED; i++)
	{
		bits_dutys[(index + BITS_LED) + i] = (((red>>(BITS_LED-1-i)) & LSB_MASK) == 1) ? PWM_DC_70 : PWM_DC_30;
	}

	//Finally set Blue bits (starting with the MSB)
	for(int i = 0; i < BITS_LED; i++)
	{
		bits_dutys[(index + BITS_LED*2) + i] = (((blue>>(BITS_LED-1-i)) & LSB_MASK) == 1) ? PWM_DC_70 : PWM_DC_30;
	}
}

//setRow
void setRow(uint8_t row, colores_t color, brightness_t bright)
{
	//First pixel's of desired row number
	uint8_t pixel_index = ((row-1)*CANT_ROW_PIXELS) + 1;

	//Set all pixels of according row
	for(int i = 0; i < CANT_COLUMN_PIXELS; i++)
	{
		setPixel(pixel_index+i, color, bright);
	}
}

//setColumn
void setColumn(uint8_t col, colores_t color, brightness_t bright)
{
	//First pixel's of desired column number
	uint8_t pixel_index = col;

	//Set all pixels of according column
	for(int i = 0; i < CANT_ROW_PIXELS; i++)
	{
		setPixel(pixel_index + (i*CANT_COLUMN_PIXELS), color, bright);
	}
}

//setAllPixels
void setAllPixels(colores_t color, brightness_t bright)
{
	//Set all pixels of the Matrix as desired
	for(int i = 1; i <= CANT_PIXELS; i++)
	{
		setPixel(i, color, bright);
	}
}

//clearMatrix
void clearMatrix(void)
{
	//Turn off al the pixels of the Matrix
	InitDutys();
}

//setSquare
void setSquare(uint8_t top_left, colores_t color, brightness_t bright)
{
	//Máximo valor que puede tomar top left
	uint8_t max_top_left = (CANT_COLUMN_PIXELS*(CANT_ROW_PIXELS-1)) - 1;

	//Validacion del indice top left
	if((top_left <= max_top_left) && ((top_left % 8) != 0))
	{
		//Set 2x2 Square
		setPixel(top_left, color, bright);
		setPixel(top_left + 1, color, bright);
		setPixel(top_left + CANT_COLUMN_PIXELS, color, bright);
		setPixel(top_left + CANT_COLUMN_PIXELS + 1, color, bright);
	}
}

//setColor
static void setColor(colores_t color, brightness_t bright)
{
    switch(color)
    {
        case RED:
        	red = 3 * bright;
        	green = 0;
        	blue = 0;
        	break;
        case GREEN:
        	red = 0;
        	green = 3 * bright;
        	blue = 0;
        	break;
        case BLUE:
        	red = 0;
        	green = 0;
        	blue = 3 * bright;
        	break;
        case WHITE:
        	red = 3 * bright;
        	green = 3 * bright;
        	blue = 3 * bright;
        	break;
        case YELLOW:
        	red = 3 * bright;
        	green = 3 * bright;
        	blue = 0;
        	break;
        case PURPLE:
        	red = 4 * bright;
        	green = 0;
        	blue = 10 * bright;
        	break;
        default:
        	red = 0;
        	green = 0;
        	blue = 3;
        	break;
    }
}
