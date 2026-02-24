/***************************************************************************//**
  @file     encoder.c
  @brief    Encoder functions
  @author   Nicolás Pablo Singermann
 ******************************************************************************/

#include "encoder.h"
#include "gpio.h"

#define TEST LOW	//Test Point para la ISR
#define NO_TEST HIGH
#define MODE NO_TEST
#define PIN_TEST PORTNUM2PIN(PC, 9)

#define HOLD_TIME_COUNTER ((HOLD_TIME_S*1000)/(SW_PERIOD_MS))

//OS
OS_ERR os_err_encoder;

//Pins
static pin_t pinOutA, pinOutB, pinSW;

//States
static bool OutA = HIGH, OutA_prev = HIGH;	//Son Active Low
static bool OutB = HIGH, OutB_prev = HIGH;
static bool sw = HIGH, sw_prev = HIGH;
static bool state;
static bool state_horario;
static bool state_antihorario;

//Flags
static bool flag_sw, flag_sw_hold, flag_horario, flag_antihorario;

//Counters
static uint8_t cont_hold = LOW;

//Recibe pines correspondientes a Out A, Out B y SW
void Encoder_Init(pin_t pin_OutA, pin_t pin_OutB, pin_t pin_SW)
{
	pinOutA = pin_OutA;
	pinOutB = pin_OutB;
	pinSW = pin_SW;

	//Set pins as INPUT
	gpioMode(pinOutA, INPUT_PULLUP);
	gpioMode(pinOutB, INPUT_PULLUP);
	gpioMode(pinSW, INPUT_PULLUP);
}

//Servicio del driver
encoder_out encoder_Read(void)
{
	if (flag_sw == HIGH)
	{
		flag_sw = LOW;
		return SW_PRESSED;	//Se pulso el sw
	}
	else if (flag_sw_hold == HIGH)
	{
		flag_sw_hold = LOW;
		return SW_HOLD;  //Se mantuvo el sw
	}
	else if (flag_horario == HIGH)
	{
		flag_horario = LOW;
		return CLOCKWISE;  //Rotación horaria
	}
	else if(flag_antihorario == HIGH)
	{
		flag_antihorario = LOW;
		return COUNTER_CLOCKWISE;  //Rotación antihoraria
	}
	else
		return NOTHING_HAPPENED;  //No ocurrió nada
}

//Callback timerOuts
void timerOuts(OS_SEM *p_sem) //Se llama cada 1ms
{
	// Giro horario --> Primero se baja OutB, luego OutA
	// Giro antihorario --> Primero se baja OutA, luego OutB

	#if (MODE == TEST)
	gpioWrite(PIN_TEST, HIGH);
	#endif

	OutA_prev = OutA;
	OutA = gpioRead(pinOutA);

	OutB_prev = OutB;
	OutB = gpioRead(pinOutB);

	if((OutA == LOW) && (OutA_prev == HIGH))  //Flanco descendente en OutA
	{
		if (OutB == LOW)	//Tuve previamente flanco descendente en OutB
		{
			state_horario = HIGH;	//Corresponde a rotación horaria
		}
	}
	else if((OutB == LOW) && (OutB_prev == HIGH))  //Flanco descendente en OutB
	{
		if (OutA == LOW)	//Tuve previamente flanco descendente en OutB
		{
			state_antihorario = HIGH;	//Corresponde a rotación antihoraria
		}
	}
	else	//Una vez se tenga el flanco ascendente, se levanta el flag
	{
		if((state_horario == HIGH) && (OutA != OutA_prev))
		{
			flag_horario = HIGH;
			state_horario = LOW;

			//Semaforo en Verde (Se giro hacia la derecha el Encoder)
			OSSemPost(p_sem, OS_OPT_POST_1, &os_err_encoder);
		}
		else if((state_antihorario == HIGH) && (OutB != OutB_prev))
		{
			flag_antihorario = HIGH;
			state_antihorario = LOW;

			//Semaforo en Verde (Se giro hacia la izquierda el Encoder)
			OSSemPost(p_sem, OS_OPT_POST_1, &os_err_encoder);
		}
	}

	#if (MODE == TEST)
	gpioWrite(PIN_TEST, LOW);
	#endif
}

//Callback timerSW
void timerSW(OS_SEM *p_sem) //Se llama cada 25ms
{

	#if (MODE == TEST)
		gpioWrite(PIN_TEST, HIGH);
	#endif

	sw_prev = sw;
	sw = gpioRead(pinSW);

	if ((sw == LOW) && (sw_prev == HIGH))  //Se tuvo un flanco descendente
	{
		state = HIGH;
	}
	else  //No se tuvo flanco descendente
	{
		if ((state == HIGH) && (sw != sw_prev))  //Se pulso el botón una vez
		{
			flag_sw = HIGH;
			state = LOW; //Flanco ascendente cancela el descendente
			cont_hold = LOW;

			//Semaforo en Verde (Se puslo el Encoder)
			OSSemPost(p_sem, OS_OPT_POST_1, &os_err_encoder);
		}
		else if ((sw == LOW) && (sw_prev == LOW))  //Se sigue presionando
		{
			cont_hold++;
		}

		if (cont_hold == HOLD_TIME_COUNTER)
		{
			flag_sw_hold = HIGH;
			cont_hold = LOW;
			state = LOW; //Evento de hold cancela flanco descendente

			//Semaforo en Verde (Se mantuvo el Encoder)
			OSSemPost(p_sem, OS_OPT_POST_1, &os_err_encoder);
		}
	}

	#if (MODE == TEST)
		gpioWrite(PIN_TEST, LOW);
	#endif
}



