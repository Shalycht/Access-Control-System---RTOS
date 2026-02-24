/*
 * system.c
 *
 *  Created on: May 1, 2015
 *      Author: Juan Pablo VEGA - Laboratorio de Microprocesadores
 */

#include "hardware.h"

static uint32_t __LDM_interruptDisableCount = 0;

void hw_Init (void)
{

	SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2)); /* set CP10, CP11 for Full Access to the FPU*/

	//WDOG->UNLOCK  = WDOG_UNLOCK_WDOGUNLOCK(0xC520); /* Key 1 */
	//WDOG->UNLOCK  = WDOG_UNLOCK_WDOGUNLOCK(0xD928); /* Key 2 */
	//WDOG->STCTRLH = WDOG_STCTRLH_ALLOWUPDATE_MASK | WDOG_STCTRLH_CLKSRC_MASK | 0x0100U; /* Disable WDOG */

	PMC->REGSC |= PMC_REGSC_ACKISO_MASK; /* Release hold with ACKISO:  Only has an effect if recovering from VLLSx.*/

	SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0x00) | SIM_CLKDIV1_OUTDIV2(0x01) | SIM_CLKDIV1_OUTDIV3(0x01) | SIM_CLKDIV1_OUTDIV4(0x03); /* Core-System = 100MHz, Bus = 50MHz, FlexBus = 50MHz, Flash = 25MHz */
	SIM->SOPT1 |= SIM_SOPT1_OSC32KSEL(0x03); /* Set 32 kHz clock source (ERCLK32K) */
	SIM->SOPT2 = SIM_SOPT2_PLLFLLSEL_MASK; /* Set high frequency clock source (PLL) */
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK; /* All PORTs enabled */

	// --- PASO 0: Capacitores y Configuración de Energía ---
    // Importante: Sumar los caps internos si no tienes externos.
    OSC->CR = OSC_CR_SC16P(1) | OSC_CR_SC4P(1);

    // --- PASO 1: De FEI a FBE (External Reference) ---
    // C2: RANGE=2 (High Freq), HGO=0 (Low Power para estabilidad), EREFS=1 (Crystal)
    MCG->C2 = MCG_C2_RANGE(2) | MCG_C2_EREFS(1) | MCG_C2_HGO(0);

    // C1: CLKS=2 (External Ref), FRDIV=4 (Div por 512 para 16MHz), IREFS=0
    MCG->C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(4) | MCG_C1_IREFS(0);

    // Esperas obligatorias del manual:
    while (!(MCG->S & MCG_S_OSCINIT0_MASK));  // 1.c: Cristal inicializado
    while (MCG->S & MCG_S_IREFST_MASK);       // 1.d: Fuente es externa
    while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(2)); // 1.e: MCGOUTCLK es el cristal

    // --- PASO 2 y 3: Configurar PLL y pasar a PBE ---
    // PRDIV=7 (Divide 16MHz por 8 = 2MHz para el PLL)
    MCG->C5 = MCG_C5_PRDIV0(7);

    // VDIV=26 (0x1A) (Multiplica 2MHz * 50 = 100MHz), PLLS=1 (Selecciona PLL)
    MCG->C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(0x1A);

    // Esperas del paso 3:
    while (!(MCG->S & MCG_S_PLLST_MASK));     // 3.d: PLL seleccionado como fuente
    while (!(MCG->S & MCG_S_LOCK0_MASK));      // 3.e: PLL enganchado (Locked)

    // --- PASO 4: De PBE a PEE (PLL engaged) ---
    // C1: CLKS=0 (Seleccionar salida del PLL como reloj del sistema)
    MCG->C1 &= ~MCG_C1_CLKS_MASK;

    while ((MCG->S & MCG_S_CLKST_MASK) != MCG_S_CLKST(3)); // 4.b: Confirmar salida PLL
}

void hw_EnableInterrupts (void)
{
    if (__LDM_interruptDisableCount > 0)
    {
        __LDM_interruptDisableCount--;

        if (__LDM_interruptDisableCount == 0)
            __enable_irq();
    }
}
void hw_DisableInterrupts (void)
{
    __disable_irq();

    __LDM_interruptDisableCount++;
}
