/******************************************************************************
  @file     PWM.h
  @brief    Generates PWM signal using K64's FTM Module
  @author   Grupo Nº5
 ******************************************************************************/

#include "hardware.h"
#include "PWM.h"

//Constantes
#define INITIAL_CNT_VALUE 0x00
#define INITIAL_CNV_VALUE 0x01
#define CH0 0x00
#define ALT4 0x04

//Variable de Estado
static bool callback_setted = false;
static callback_t callback;

//PWM_Init
void PWM_Init(uint8_t prescaler, uint16_t mod)
{
	//FTM Module Clock Gating
	SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;

	//Enable Interrupts
	NVIC_EnableIRQ(FTM0_IRQn);

	//Enable FTM PWM Load
	FTM0->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;

	//Configure PTC1 Pin to be the PWM signal output
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;	//PORT C Clock Gating

	//Set PTC1 MUX to FTM0 Channel 0, Enable Drive Strength
	//Disable interal pull up/down and Disable GPIO Interrupts
	PORTC->PCR[1] =  PORT_PCR_PE(0) | PORT_PCR_PS(0) | PORT_PCR_DSE(1) |
			 	 	 PORT_PCR_MUX(ALT4) | PORT_PCR_IRQC(0);

	//Set FTM Prescaler
	FTM0->SC = (FTM0->SC & ~FTM_SC_PS_MASK) | FTM_SC_PS(prescaler);

	//Enable Channel Interrupts
	FTM0->CONTROLS[CH0].CnSC = (FTM0->CONTROLS[CH0].CnSC & ~FTM_CnSC_CHIE_MASK) | FTM_CnSC_CHIE(true);

	//Set Working Mode to Edge Aligned PWM
	FTM0->CONTROLS[CH0].CnSC = (FTM0->CONTROLS[CH0].CnSC & ~FTM_CnSC_MSB_MASK)| FTM_CnSC_MSB(true);

	//Set Falling edges as detected edges
	FTM0->CONTROLS[CH0].CnSC = (FTM0->CONTROLS[CH0].CnSC & ~FTM_CnSC_ELSB_MASK)| FTM_CnSC_ELSB(true);

	//Set FTM Inital Counter value and MOD
	FTM0->CNTIN = INITIAL_CNT_VALUE;
	FTM0->CNT = INITIAL_CNT_VALUE;
	FTM0->MOD = FTM_MOD_MOD(mod);

	//Set Initial Duty Cycle
	FTM0->CONTROLS[CH0].CnV = FTM_CnV_VAL(INITIAL_CNV_VALUE);

	//Enable DMA
	FTM0->CONTROLS[CH0].CnSC = (FTM0->CONTROLS[CH0].CnSC & ~FTM_CnSC_DMA_MASK) | FTM_CnSC_DMA(true);

	//Start PWM signal
	PWM_Start();
}

//PWM_Set_DC
void PWM_Set_DC(uint16_t duty)
{
	FTM0->CONTROLS[CH0].CnV = FTM_CnV_VAL(duty);
}

//PWM_Start
void PWM_Start(void)
{
	//Start Clock (System Clock --> 50MHz)
	FTM0->SC |= FTM_SC_CLKS(0x01);
}

//PWM_Stop
void PWM_Stop(void)
{
	//Stop Clock
	FTM0->SC &= ~FTM_SC_CLKS(0x01);
}

//PWM_Set_Callback
void PWM_Set_Callback(callback_t fun)
{
	callback = fun;
	callback_setted = true;
}

//FTM0 Interrupt Handler
__ISR__ FTM0_IRQHandler(void)
{
	if(callback_setted == true)
	{
		callback();	//Do Something ...
	}
}

