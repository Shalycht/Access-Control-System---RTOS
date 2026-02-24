/******************************************************************************
  @file     PWM.h
  @brief    Generates PWM signal (PTC1) using K64's FTM Module (FTM0, Channel 0)
  @author   Grupo Nº5
 ******************************************************************************/

#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>
#include <stdbool.h>

//Prescaler Options
enum{ PSC_x1, PSC_x2, PSC_x4, PSC_x8, PSC_x16, PSC_x32, PSC_x64, PSC_x128 };

//Callback
typedef void (*callback_t)(void);

/**
 * @brief Initialize FTM0 to generate PWM signal
 * @param prescaler: Set Clock Prescaler according to enum
 * @param mod: Set FTM0 Counter value to reach
 */
void PWM_Init(uint8_t prescaler, uint16_t mod);

/**
 * @brief Sets desired Duty Cycle for the PWM Signal
 */
void PWM_Set_DC(uint16_t duty);

/**
 * @brief Starts generating PWM Signal
 */
void PWM_Start(void);

/**
 * @brief Stops generating PWM Signal
 */
void PWM_Stop(void);

/**
 * @brief Set Callback to be called at the beginning of each signal period
 */
void PWM_Set_Callback(callback_t fun);

#endif /* PWM_H_ */
