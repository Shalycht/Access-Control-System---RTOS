/******************************************************************************
  @file     building.h
  @brief    Building specs and services driver header file
  @author   Grupo Nº5
 ******************************************************************************/

#ifndef BUILDING_H_
#define BUILDING_H_

#include "defs.h"

//Building Specs
#define CANT_FLOORS  3
#define MAX_TENANTS 4

//Indexes for Extra
#define EXTRA1 7
#define EXTRA2 8

//Building floors
typedef enum {FLOOR1, FLOOR2, FLOOR3} floors_t;

/**
 * @brief Initialize Building
 */
void InitBuilding(void);

/**
 * @brief Set pixels according the amount of tenants in the floor
 * @param tenants: Amount of tenatns in certain floor (0, 1, 2, 3, 4)
 * @param floor: Corresponding floor
 */
void setFloor(uint8_t tenants, floors_t floor);

/**
 * @brief Turn on/off last 2 columns
 * @param color: RED, GREEN, YELLOW
 * @param bright: Brightness according to brightness_t
 */
void setExtra(colores_t color, brightness_t bright);


#endif /* BUILDING_H_ */
