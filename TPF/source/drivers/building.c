/******************************************************************************
  @file     building.h
  @brief    Building specs and services driver source file
  @author   Grupo Nº5
 ******************************************************************************/

#include "building.h"
#include "matrix.h"

//For indexing purposes
#define COLUMN_STEP  (CANT_COLUMN_PIXELS*2)						   //16
#define FLOOR1_START ((CANT_ROW_PIXELS-1)*(CANT_COLUMN_PIXELS-1))  //49
#define FLOOR2_START (FLOOR1_START+2)
#define FLOOR3_START (FLOOR2_START+2)

//RGB Floor Structures
static colores_t floor_colors[CANT_FLOORS] = {PURPLE, BLUE, WHITE};

//Start index settings
static uint8_t start_index[CANT_FLOORS] = {FLOOR1_START, FLOOR2_START, FLOOR3_START};

//Prototipos
static void clearFloor(floors_t floor);
static uint8_t calculateTopLeft(uint8_t tenants, floors_t floor);
static brightness_t calculateBrightness(uint8_t tenants);

//InitBuilding
void InitBuilding(void)
{
	//Initialize Matrix
	InitMatrix();
}

//setFloor
void setFloor(uint8_t tenants, floors_t floor)
{
	clearFloor(floor);

	//There's at least 1 tenant in the floor
	if(tenants > 0)
	{
		//Turn on desired pixels of the floor
		for(uint8_t i = 1; i <= tenants; i++)
		{
			setSquare(calculateTopLeft(i, floor), floor_colors[floor], calculateBrightness(i));
		}
	}
}

//setExtra
void setExtra(colores_t color, brightness_t bright)
{
	//Set las 2 columns of the matrix
	setColumn(EXTRA1, color, bright);
	setColumn(EXTRA2, color, bright);
}

//calculateTopLeft
static uint8_t calculateTopLeft(uint8_t tenants, floors_t floor)
{
	//Para Floor 1 --> 49, 33, 17, 1
	//Para Floor 2 --> 51, 35, 19, 3
	//Para Floor 3 --> 53, 37, 21, 5

	uint8_t top_left = start_index[floor] - ((tenants - 1) * COLUMN_STEP);
	return top_left;
}

//calculateBrightness
static brightness_t calculateBrightness(uint8_t tenants)
{
	brightness_t bright = OFF;

	//Dependiendo de la cantidad de inquilinos en el piso
	switch(tenants)
	{
		case 0:
			bright = OFF;
			break;
		case 1:
			bright = LVL1;
			break;
		case 2:
			bright = LVL2;
			break;
		case 3:
			bright = LVL3;
			break;
		case MAX_TENANTS:
			bright = LVL4;
			break;
		default:
			bright = OFF;
			break;
	}
	return bright;
}

//clearFloor
static void clearFloor(floors_t floor)
{
	//Clear corresponding columns of the matrix
	if(floor == FLOOR1)
	{
		setColumn(1, floor_colors[floor], OFF);
		setColumn(2, floor_colors[floor], OFF);
	}
	else if(floor == FLOOR2)
	{
		setColumn(3, floor_colors[floor], OFF);
		setColumn(4, floor_colors[floor], OFF);
	}
	else
	{
		setColumn(5, floor_colors[floor], OFF);
		setColumn(6, floor_colors[floor], OFF);
	}
}
