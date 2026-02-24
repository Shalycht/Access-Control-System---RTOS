/******************************************************************************
  @file     display_7s.h
  @brief    Display functions
  @author   Grupo Nº5
 ******************************************************************************/

#include "gpio.h"
#include "display_7s.h"

//Constantes
#define DISP_LEN 4
#define MSG_CHAR_MAX 32
#define MSG_ARRAY_LEN (MSG_CHAR_MAX+2*DISP_LEN)
#define START_MSG_INDEX 4
#define PWM_TH 10
#define TEST LOW	//Test Point para la ISR
#define NO_TEST HIGH
#define MODE NO_TEST
#define PIN_TEST PORTNUM2PIN(PC, 9)

//Arreglo de pines
static pin_t display_pins[7];
static pin_t display_select[2];

//Variables
static uint8_t message_array[MSG_ARRAY_LEN];

//Indexes
static uint8_t display_index, message_index;

//Counters
uint8_t blink_counter, blink_count, spin_counter, spin_count;
int pwm_counter;
int pwm;

//States
int blink, spin;
uint8_t str_len;

bool blink_on;
uint8_t spin_counter;
uint8_t blink_counter;


//increase_display_brightness
void increase_display_brightness(void)
{
	if(pwm>MIN_BRIGHTNESS)
	{
		pwm--;
	}
}

//decrease_display_brightness
void decrease_display_brightness(void)
{
	if(pwm<MAX_BRIGHTNESS)
	{
		pwm++;
	}
}

//display_Init
void display_Init(display_pins_t pins)
{
	//Set pins
	display_pins[0] = pins.pin_a;
	display_pins[1] = pins.pin_b;
	display_pins[2] = pins.pin_c;
	display_pins[3] = pins.pin_d;
	display_pins[4] = pins.pin_e;
	display_pins[5] = pins.pin_f;
	display_pins[6] = pins.pin_g;

	display_select[0] = pins.pin_sel0;
	display_select[1] = pins.pin_sel1;

	//Set Pins as OUTPUT
	for(int i = 0; i < 7; i++)
	{
		gpioMode(display_pins[i], OUTPUT);
	}

	gpioMode(display_select[0], OUTPUT);
	gpioMode(display_select[1], OUTPUT);

	//Lleno los buffers con displays en blanco para el efecto de spin
	for(int i = 0; i < MSG_ARRAY_LEN; i++)
	{
		message_array[i] = 0b00000000;
	}

	//Initial settings
	message_index = START_MSG_INDEX;
	spin_counter = 0;
	pwm_counter=PWM_TH;
	pwm=1;
}

//display_clean
void display_clean(void)
{
	for(int i = 0; i < MSG_ARRAY_LEN; i++)
	{
		message_array[i] = 0;
	}

	message_index = START_MSG_INDEX;
}

//display_msg
void display_msg(display_config_t config)
{
	//Set values
	blink = config.blink;
	spin = config.spin;
	str_len = config.length;
	spin_count = config.spin_delay;
	blink_count = config.blink_delay;
	pwm = config.bright_level;

	for(int i_char = 0; i_char < str_len+DISP_LEN;i_char++)
	{
		if(i_char<str_len)
		{
		switch(config.msg[i_char])
		{
			case 'a':
			case 'A':
				message_array[i_char+DISP_LEN] = 0b01110111; // a
				break;
			case 'b':
			case 'B':
				message_array[i_char+DISP_LEN] = 0b01111100; // b
				break;
			case 'c':
			case 'C':
				message_array[i_char+DISP_LEN] = 0b00111001; // c
				break;
			case 'd':
			case 'D':
				message_array[i_char+DISP_LEN] = 0b01011110; // d
				break;
			case 'e':
			case 'E':
				message_array[i_char+DISP_LEN] = 0b01111001; // E
				break;
			case 'f':
			case 'F':
				message_array[i_char+DISP_LEN] = 0b01110001; // f
				break;
			case 'g':
			case 'G':
				message_array[i_char+DISP_LEN] = 0b01101111; // g
				break;
			case 'h':
			case 'H':
				message_array[i_char+DISP_LEN] = 0b01110100; // h
				break;
			case 'i':
			case 'I':
				message_array[i_char+DISP_LEN] = 0b00000100; // i
				break;
			case 'j':
			case 'J':
				message_array[i_char+DISP_LEN] = 0b00011110; // j
				break;
			case 'l':
			case 'L':
				message_array[i_char+DISP_LEN] = 0b00111000; // l
				break;
			case 'n':
			case 'N':
				message_array[i_char+DISP_LEN] = 0b01010100; // n
				break;
			case 'o':
			case 'O':
				message_array[i_char+DISP_LEN] = 0b00111111; // o
				break;
			case 'p':
			case 'P':
				message_array[i_char+DISP_LEN] = 0b01110011; // p
				break;
			case 'q':
			case 'Q':
				message_array[i_char+DISP_LEN] = 0b01100111; // q
				break;
			case 'r':
			case 'R':
				message_array[i_char+DISP_LEN] = 0b01010000; // r
				break;
			case 's':
			case 'S':
				message_array[i_char+DISP_LEN] = 0b01101101; // s
				break;
			case 't':
			case 'T':
				message_array[i_char+DISP_LEN] = 0b11111000; // t
				break;
			case 'u':
			case 'U':
				message_array[i_char+DISP_LEN] = 0b00111110; // u
				break;
			case 'v':
			case 'V':
				message_array[i_char+DISP_LEN] = 0b00111110; // v
				break;
			case 'y':
			case 'Y':
				message_array[i_char+DISP_LEN] = 0b01101110; // y
				break;
			case 'z':
			case 'Z':
				message_array[i_char+DISP_LEN] = 0b11111111; // y
				break;
			case '0':
				message_array[i_char+DISP_LEN]= 0b00111111; // 0
				break;
			case '1':
				message_array[i_char+DISP_LEN] = 0b00000110; // 1
				break;
			case '2':
				message_array[i_char+DISP_LEN] = 0b01011011; // 2
				break;
			case '3':
				message_array[i_char+DISP_LEN]= 0b01001111; // 3
				break;
			case '4':
				message_array[i_char+DISP_LEN] = 0b01100110; // 4
				break;
			case '5':
				message_array[i_char+DISP_LEN] = 0b01101101; // 5
				break;
			case '6':
				message_array[i_char+DISP_LEN] = 0b01111101; // 6
				break;
			case '7':
				message_array[i_char+DISP_LEN] = 0b00000111; // 7
				break;
			case '8':
				message_array[i_char+DISP_LEN] = 0b01111111; // 8
				break;
			case '9':
				message_array[i_char+DISP_LEN] = 0b01101111; // 9
				break;
			case '-':
				message_array[i_char+DISP_LEN] = 0b01000000; // -
				break;
			case '=':
				message_array[i_char+DISP_LEN] = 0b01001000;
			default:
				message_array[i_char+DISP_LEN] = 0b00000000;
				break;
		}
		}else
		{
			message_array[i_char+DISP_LEN] = 0b00000000;

		}
	}
}

//Timer_Message_Handler
void Timer_Message_Handler(void)
{
	#if (MODE == TEST)
	gpioWrite(PIN_TEST, HIGH);
	#endif

	if(blink!=0)
	{
		if(--blink_counter<=0)
		{
			blink_counter=blink_count;
			blink_on = !blink_on;
		}
	}
	if(spin)
	{
		if(--spin_counter<=0)
		{
			if(message_index>str_len+4)
			{
				message_index=0;
			}
			else
			{
				message_index++;
			}
		}

	}
	else
	{
		message_index=4;
	}

	#if (MODE == TEST)
	gpioWrite(PIN_TEST, LOW);
	#endif

}

//Timer_Display_Handler
void Timer_Display_Handler(void)
{
	#if (MODE == TEST)
	gpioWrite(PIN_TEST, HIGH);
	#endif

	if(++display_index>=4)
	{
		display_index=0;
	}
	if(display_index == 0)
	{
		gpioWrite(display_select[0], LOW);
		gpioWrite(display_select[1], LOW);
	}
	else if(display_index == 1)
	{
		gpioWrite(display_select[0], HIGH);
		gpioWrite(display_select[1], LOW);
	}
	else if(display_index == 2)
	{
		gpioWrite(display_select[0], LOW);
		gpioWrite(display_select[1], HIGH);
	}
	else if(display_index == 3)
	{
		gpioWrite(display_select[0], HIGH);
		gpioWrite(display_select[0], HIGH);
	}

	if(--pwm_counter>=pwm)
	{



			for(int i=0;i<7;i++)
			{
				if(!blink_on && ((display_index==(blink-1)) || (blink==BLINK_ALL)))
				{
					gpioWrite(display_pins[i],0);
				}
				else
				{
					gpioWrite(display_pins[i],(message_array[message_index+display_index]&((uint8_t)(0b0000001))<<(i))>=1);
				}
			}

		/*
		else //if(display_index == blink-1 || blink == -1)
		{
			for(int i=0;i<7;i++)
			{
					gpioWrite(display_pins[i],0);
			}
		}*/


	}
	else
	{
		if(pwm_counter<=0)
		{
			pwm_counter = PWM_TH;
		}
		for(int i=0;i<7;i++)
		{
			gpioWrite(display_pins[i],0);
		}
	}
#if (MODE == TEST)
	gpioWrite(PIN_TEST, LOW);
	#endif
}
