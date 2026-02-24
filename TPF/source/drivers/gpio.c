/***************************************************************************//**
  @file     gpio.c
  @brief    GPIO functions
  @author   Grupo 5
 ******************************************************************************/

#include "gpio.h"
#include "MK64F12.h"
#include "hardware.h"

#define PINS_PER_PORT 32
#define CANT_PINS PINS_PER_PORT*5	//5x32

#define PERFORMANCE 1	//No se harán validaciones, el usuario sabe como funciona gpio.c
#define SLOW 2			//Se harán muchas validaciones

//Select desired MODE
#define MODE PERFORMANCE

//Punteros a los PORT_type
static PORT_Type * const port_pointers[5] = PORT_BASE_PTRS;

//Punteros a los GPIO_type
static GPIO_Type * const gpio_pointers[5]  = GPIO_BASE_PTRS;

//Punteros a funcion por eventos irq
static pinIrqFun_t CALLBACKS[CANT_PINS];

static bool state_clk = LOW;
static bool state_irq = LOW;

#if (MODE == PERFORMANCE)

//For brief see gpio.h
void gpioMode (pin_t pin, uint8_t mode)
{
	uint8_t puerto = PIN2PORT(pin);	//Primeros 3 bits
	uint8_t numero_pin = PIN2NUM(pin);	//Ultimo 5 bits

	//Habilito clock para los 5 puertos
	if (state_clk == LOW)
	{
		SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
		SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
		SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
		SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
		SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
		state_clk = HIGH;
	}

	//Set GPIO Mode in MUX field of PCR -> Alternative 1 (Set 001 in MUX field)
	port_pointers[puerto]->PCR[numero_pin] |= PORT_PCR_MUX(HIGH);

	//Configure pin
	if ((mode == INPUT) || (mode == OUTPUT))
	{
		//Casteo a 32b para poder hacer el shift
		gpio_pointers[puerto]->PDDR |= ((uint32_t)(mode))<<(numero_pin);
	}
	else  //INPUT_PULLUP or INPUT_PULLDOWN
	{
		gpio_pointers[puerto]->PDDR |= ((uint32_t)(INPUT))<<(numero_pin);

		//Set 1 in PE of PCR, and set PS value of PCR
		port_pointers[puerto]->PCR[numero_pin] |= PORT_PCR_PE(HIGH);

		if (mode == INPUT_PULLUP)
		{
			port_pointers[puerto]->PCR[numero_pin] |= PORT_PCR_PS(HIGH);
		}
		else  //INPUT_PULLDOWN
		{
			port_pointers[puerto]->PCR[numero_pin] |= PORT_PCR_PS(LOW);
		}
	}
}

//For brief see gpio.h
void gpioIRQ (pin_t pin, uint8_t irqMode, pinIrqFun_t irqFun)
{
	uint8_t puerto = PIN2PORT(pin);	//Primeros 3 bits
	uint8_t numero_pin = PIN2NUM(pin);	//Ultimo 5 bits

	//Habilito interrupciones para los 5 puertos
	if (state_irq == LOW)
	{
		NVIC_EnableIRQ(PORTA_IRQn);
		NVIC_EnableIRQ(PORTB_IRQn);
		NVIC_EnableIRQ(PORTC_IRQn);
		NVIC_EnableIRQ(PORTD_IRQn);
		NVIC_EnableIRQ(PORTE_IRQn);
		state_irq = HIGH;
	}

	//Habilito interrupciones para el pin
	port_pointers[puerto]->PCR[numero_pin] &= ~(PORT_PCR_IRQC_MASK); //Clean IRQC
	port_pointers[puerto]->PCR[numero_pin] |= PORT_PCR_IRQC(irqMode);

	if (irqMode == GPIO_IRQ_MODE_DISABLE)
		NVIC_DisableIRQ(PORTA_IRQn + puerto);

	CALLBACKS[PINS_PER_PORT*puerto + numero_pin] = irqFun;
}

static void IRQHandler(uint32_t puerto)
{
	uint32_t ISFR = port_pointers[puerto]->ISFR;

	for (int pin = 0; pin < PINS_PER_PORT; pin++)
	{
		if ((ISFR>>pin & HIGH) == HIGH)
		{
			port_pointers[puerto]->PCR[pin] |= PORT_PCR_ISF_MASK;  //Clear Flag
			(*CALLBACKS[PINS_PER_PORT * puerto + pin])();
		}
	}
}

//IRQ Handlers
__ISR__ PORTA_IRQHandler(void)
{
	IRQHandler(PA);
}

__ISR__ PORTB_IRQHandler(void)
{
	IRQHandler(PB);
}

__ISR__ PORTC_IRQHandler(void)
{
	IRQHandler(PC);
}

__ISR__ PORTD_IRQHandler(void)
{
	IRQHandler(PD);
}

__ISR__ PORTE_IRQHandler(void)
{
	IRQHandler(PE);
}

//For brief see gpio.h
void gpioWrite (pin_t pin, bool value)
{
	uint8_t puerto = PIN2PORT(pin);	//Primeros 3 bits
	uint8_t numero_pin = PIN2NUM(pin);	//Ultimo 5 bits

	uint32_t aux = gpio_pointers[puerto]->PDDR & (1U<<numero_pin);

	//Set desired value to pin
	aux = gpio_pointers[puerto]->PDOR;
	gpio_pointers[puerto]->PDOR = (aux & ~(1U<<numero_pin)) | (((uint32_t)(value))<<numero_pin);
}

//For brief see gpio.h
void gpioToggle (pin_t pin)
{
	uint8_t puerto = PIN2PORT(pin);	//Primeros 3 bits
	uint8_t numero_pin = PIN2NUM(pin);	//Ultimo 5 bits

	uint32_t aux = gpio_pointers[puerto]->PDDR & (1U<<numero_pin);

	//Set desired value to pin
	aux = gpio_pointers[puerto]->PTOR;
	gpio_pointers[puerto]->PTOR = (aux & ~(1U<<numero_pin)) | (1U<<numero_pin);
}

//For brief see gpio.h
bool gpioRead (pin_t pin)
{
	uint8_t puerto = PIN2PORT(pin);	//Primeros 3 bits
	uint8_t numero_pin = PIN2NUM(pin);	//Ultimo 5 bits

	uint32_t aux = gpio_pointers[puerto]->PDDR & (1U<<numero_pin);

	//Read pin value
	aux = (gpio_pointers[puerto]->PDIR) & (1U<<numero_pin);
	if (aux == 0)
		return LOW;
	else
		return HIGH;
}

#endif	//MODE == PERFORMANCE


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


#if (MODE == SLOW)

void gpioMode (pin_t pin, uint8_t mode)
{
	uint8_t puerto = PIN2PORT(pin);	//Primeros 3 bits
	uint8_t numero_pin = PIN2NUM(pin);	//Ultimo 5 bits

	//Habilito clock para los 5 puertos
	if (state_clk == LOW)
	{
		SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
		SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
		SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
		SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
		SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
		state_clk = HIGH;
	}

	//Set GPIO Mode in MUX field of PCR -> Alternative 1 (Set 001 in MUX field)
	port_pointers[puerto]->PCR[numero_pin] |= PORT_PCR_MUX(HIGH);

	//Configure pin
	if ((mode == INPUT) || (mode == OUTPUT))
	{
		//Casteo a 32b para poder hacer el shift
		gpio_pointers[puerto]->PDDR |= ((uint32_t)(mode))<<(numero_pin);
	}
	else  //INPUT_PULLUP or INPUT_PULLDOWN
	{
		gpio_pointers[puerto]->PDDR |= ((uint32_t)(INPUT))<<(numero_pin);

		//Set 1 in PE of PCR, and set PS value of PCR
		port_pointers[puerto]->PCR[numero_pin] |= PORT_PCR_PE(HIGH);

		if (mode == INPUT_PULLUP)
		{
			port_pointers[puerto]->PCR[numero_pin] |= PORT_PCR_PS(HIGH);
		}
		else  //INPUT_PULLDOWN
		{
			port_pointers[puerto]->PCR[numero_pin] |= PORT_PCR_PS(LOW);
		}
	}
}

void gpioIRQ (pin_t pin, uint8_t irqMode, pinIrqFun_t irqFun)
{
	uint8_t puerto = PIN2PORT(pin);	//Primeros 3 bits
	uint8_t numero_pin = PIN2NUM(pin);	//Ultimo 5 bits

	//Habilito interrupciones para los 5 puertos
	if (state_irq == LOW)
	{
		NVIC_EnableIRQ(PORTA_IRQn);
		NVIC_EnableIRQ(PORTB_IRQn);
		NVIC_EnableIRQ(PORTC_IRQn);
		NVIC_EnableIRQ(PORTD_IRQn);
		NVIC_EnableIRQ(PORTE_IRQn);
		state_irq = HIGH;
	}

	//Habilito interrupciones para el pin
	port_pointers[puerto]->PCR[numero_pin] &= ~(PORT_PCR_IRQC_MASK); //Clean IRQC
	port_pointers[puerto]->PCR[numero_pin] |= PORT_PCR_IRQC(irqMode);

	if (irqMode == GPIO_IRQ_MODE_DISABLE)
		NVIC_DisableIRQ(PORTA_IRQn + puerto);

	CALLBACKS[PINS_PER_PORT*puerto + numero_pin] = irqFun;
}

static void IRQHandler(uint32_t puerto)
{
	uint32_t ISFR = port_pointers[puerto]->ISFR;

	for(int pin = 0; pin < PINS_PER_PORT; pin++)
	{
		if ((ISFR>>pin & HIGH) == HIGH)
		{
			port_pointers[puerto]->PCR[pin] |= PORT_PCR_ISF_MASK;  //Clear Flag
			(*CALLBACKS[PINS_PER_PORT * puerto + pin])();
		}
	}
}

//IRQ Handlers
__ISR__ PORTA_IRQHandler(void)
{
	IRQHandler(PA);
}

__ISR__ PORTB_IRQHandler(void)
{
	IRQHandler(PB);
}

__ISR__ PORTC_IRQHandler(void)
{
	IRQHandler(PC);
}

__ISR__ PORTD_IRQHandler(void)
{
	IRQHandler(PD);
}

__ISR__ PORTE_IRQHandler(void)
{
	IRQHandler(PE);
}


void gpioWrite (pin_t pin, bool value)
{
	uint8_t puerto = PIN2PORT(pin);	//Primeros 3 bits
	uint8_t numero_pin = PIN2NUM(pin);	//Ultimo 5 bits

	uint32_t aux = gpio_pointers[puerto]->PDDR & (1U<<numero_pin);

	if (state_clk == HIGH)	//Clock is enabled
	{
		if(aux != LOW)  //Pin is set as OUTPUT
		{
			//Set desired value to pin
			aux = gpio_pointers[puerto]->PDOR;
			gpio_pointers[puerto]->PDOR = (aux & ~(1U<<numero_pin)) | (((uint32_t)(value))<<numero_pin);
		}
		else  //Pin is set as INPUT
		{
			//Devuelve error
		}
	}
	else  //Clock isn't enabled
	{
		//Devuelve error
	}
}

void gpioToggle (pin_t pin)
{
	uint8_t puerto = PIN2PORT(pin);	//Primeros 3 bits
	uint8_t numero_pin = PIN2NUM(pin);	//Ultimo 5 bits

	uint32_t aux = gpio_pointers[puerto]->PDDR & (1U<<numero_pin);

	if (state_clk == HIGH)	//Clock is enabled
	{
		if(aux != LOW)  //Pin is set as OUTPUT
		{
			//Set desired value to pin
			aux = gpio_pointers[puerto]->PTOR;
			gpio_pointers[puerto]->PTOR = (aux & ~(1U<<numero_pin)) | (1U<<numero_pin);
		}
		else  //Pin is set as INPUT
		{
			//Devuelve error
		}
	}
	else  //Clock isn't enabled
	{
		//Devuelve error
	}
}

bool gpioRead (pin_t pin)
{
	uint8_t puerto = PIN2PORT(pin);	//Primeros 3 bits
	uint8_t numero_pin = PIN2NUM(pin);	//Ultimo 5 bits

	uint32_t aux = gpio_pointers[puerto]->PDDR & (1U<<numero_pin);

	bool value = LOW;

	if (state_clk == HIGH)	//Clock is enabled
	{
		if(aux == LOW)  //Pin is set as INPUT
		{
			//Read pin value
			aux = (gpio_pointers[puerto]->PDIR) & (1U<<numero_pin);
			if (aux == 0)
			{
				value = LOW;
			}
			else
			{
				value = HIGH;
			}
		}
		else  //Pin is set as OUTPUT
		{
			//Devuelve error
		}
	}
	else  //Clock isn't enabled22
	{
		//Devuelve error
	}

	return value;
}

#endif	//MODE == SLOW


