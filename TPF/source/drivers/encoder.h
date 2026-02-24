/******************************************************************************
  @file     encoder.h
  @brief    Encoder driver
  @author   Nicolás Pablo Singermann
 ******************************************************************************/

#ifndef ENCODER_H_
#define ENCODER_H_

#include <stdint.h>
#include <stdbool.h>
#include <os.h>

#define OUTS_PERIOD_MS 1  //En ms
#define SW_PERIOD_MS 25   //En ms
#define HOLD_TIME_S 2     //En segs

typedef uint8_t encoder_out;
typedef uint8_t pin_t;

enum {NOTHING_HAPPENED, SW_PRESSED, SW_HOLD, CLOCKWISE, COUNTER_CLOCKWISE };

//Init
void Encoder_Init(pin_t pin_OutA, pin_t pin_OutB, pin_t pin_SW);

/**
 * @brief Encoder servicies
 * @return NOTHING_HAPPENED, SW_PRESSED, SW_HOLD, CLOCKWISE, COUNTER_CLOCKWISE
 */
encoder_out encoder_Read(void);

/**
 * @brief Analiza eventos de giro del encoder. Debe ser llamada cada 1ms.
 */
void timerOuts(OS_SEM *p_sem);

/**
 * @brief Analiza eventos del botón del encoder. Debe ser llamada cada 25ms.
 */
void timerSW(OS_SEM *p_sem);

#endif /* ENCODER_H_ */
