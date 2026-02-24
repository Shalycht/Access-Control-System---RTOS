/*******************************************************************************
  @file     dma.c
  @brief    DMA Driver (FTM0 generates DMA requests to the DMA channel 0)
  @author   Grupo Nº5
 ******************************************************************************/

#include "dma.h"
#include "hardware.h"

//FTM0 DMA Request Source
#define FTM0_SRC 20
#define CH0 0
#define BIT_16 1

//DMA_Init
void DMA_Init(void* srcBuffer, uint16_t srcLength, uint32_t srcSize)
{
	//Clock Gating
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

	//Enable DMA Channel 0 and set FTM0 as the Request Source
	DMAMUX->CHCFG[CH0] |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(FTM0_SRC);

	//Clear all the pending events
	NVIC_ClearPendingIRQ(DMA0_IRQn);

	//Enable the DMA interrupts
	NVIC_EnableIRQ(DMA0_IRQn);

	//--------------- DMA Configuration ---------------
	DMA0->TCD[CH0].SADDR = (uint32_t)(srcBuffer);
	DMA0->TCD[CH0].DADDR = (uint32_t)(&(FTM0->CONTROLS[0].CnV));

	//Set Source and Destination offset
	DMA0->TCD[CH0].SOFF =0x02;
	DMA0->TCD[CH0].DOFF =0x00;

	//Set Source and Destination Data Sizes
	DMA0->TCD[CH0].ATTR = DMA_ATTR_SSIZE(BIT_16) | DMA_ATTR_DSIZE(BIT_16);

	//Set number of bytes to send every request
	DMA0->TCD[CH0].NBYTES_MLNO = 0x02;

	//Set Citer and Biter
	DMA0->TCD[CH0].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(srcLength);
	DMA0->TCD[CH0].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(srcLength);

	//Set Slast for wrap around
	DMA0->TCD[CH0].SLAST = -srcSize;

	//Clear Dlast
	 DMA0->TCD[CH0].DLAST_SGA = 0x00;

	//Enable Major interrupt
	DMA0->TCD[CH0].CSR = DMA_CSR_INTMAJOR_MASK;

	//Enable request signal
	DMA0->ERQ = DMA_ERQ_ERQ0_MASK;
}

//DMA Channel 0 IRQ Handler
void DMA0_IRQHandler(void)
{
	//Clear the interrupt flag
	DMA0->CINT |= 0;
}

