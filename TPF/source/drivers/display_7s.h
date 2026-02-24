/******************************************************************************
  @file     display_7s.h
  @brief    Display driver
  @author   Grupo Nº5
 ******************************************************************************/

#ifndef _DISPLAY7S_H_
#define _DISPLAY7S_H_

#include <stdint.h>
#include <stdbool.h>

#define DISPLAY_TICKS	   1
#define DISPLAY_MSG_TICKS  2

#define MIN_BRIGHTNESS 1
#define MAX_BRIGHTNESS 8

typedef uint8_t pin_t;

//Tipos de Blink
enum{ NO_BLINK, BLINK_1, BLINK_2, BLINK_3, BLINK_4, BLINK_ALL };

//Spin
enum{ NO_SPIN, SPIN };

//Estructura para los pines
typedef struct{
	pin_t pin_a;	//Pins del Display de 7 segmentos
	pin_t pin_b;
	pin_t pin_c;
	pin_t pin_d;
	pin_t pin_e;
	pin_t pin_f;
	pin_t pin_g;
	pin_t pin_dp;
	pin_t pin_sel0;	//Pins para seleccionar Display
	pin_t pin_sel1;
}display_pins_t;

//Estructura de configuración del Display
typedef struct{
	char *msg;				//Mensaje a mostrar
	uint8_t length;			//Cantidad de caracteres del mensaje
	uint8_t bright_level;	//Intensidad del brillo deseado
	bool spin;				//Spin, segun el enum
	uint8_t spin_delay;		//Velocidad del spin
	int8_t blink;			//Blink de cierto Display, o todos, segun el enum
	uint8_t blink_delay;	//Velocidad del blink
}display_config_t;

/**
 * @brief Inicializa Display
 * @param pins: Estructura con los pines del Display
 */
void display_Init(display_pins_t pins);

/**
 * @brief Muestra mensaje en el display
 * @param config: Configuración del Display
 */
void display_msg(display_config_t config);

/**
 * @brief Limpia el Display
 */
void display_clean(void);

/**
 * @brief Aumenta brillo display
 */
void increase_display_brightness(void);

/**
 * @brief Disminuye birllo display
 */
void decrease_display_brightness(void);

/**
 * @brief Actualiza mensaje en el display. Debe ser llamada cada 2ms.
 */
void Timer_Message_Handler(void);

/**
 * @brief Refresco de los displays. Debe ser llamada cada 1ms.
 */
void Timer_Display_Handler(void);


#endif /* _DISPLAY7S_H_ */

