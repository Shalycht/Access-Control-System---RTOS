/*******************************************************************************
  @file     dma.c
  @brief    DMA Driver (FTM0 generates DMA requeststo the DMA channel 0)
  @author   Grupo Nº5
 ******************************************************************************/

#ifndef DMA_H_
#define DMA_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief DMA Configuration
 * @param srcBuffer: Arreglo con los DC
 * @param srcLength: Cantidad de elementos del arreglo
 * @param srcSize: sizeof del arreglo
 */
void DMA_Init(void* srcBuffer, uint16_t srcLength, uint32_t srcSize);


#endif /* DMA_H_ */
