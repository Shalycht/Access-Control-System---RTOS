/***************************************************************************//**
  @file     CRead.c
  @brief    Card Reader Driver
  @author   Pedro Dalla Lasta
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "CRead.h"
#include "board.h"
#include "gpio.h"
#include <stddef.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

 //Pins from the card reader
#define PIN_CR_DATA      PTC10     //Card reader data entry, LS bit comes out first
#define PIN_CR_CLOCK     PTC11     //Card reader clock entry, data changes on positive edge
#define PIN_CR_ENABLE    PTB11     //Card reader enable, low while card is sliding

//Characters from the card
#define SS ';'
#define FS '='
#define ES '?'
#define BASE '0'

#define MAX_BITS 4
#define MAX_DATA 200
#define MAX_TRACK 40
#define MAX_ID 8
#define PAN 17
#define AD_DD 37
#define SPECIAL_CHAR 1

#define CARD_PASS_TIMEOUT 5000

#define TEST LOW	//Test Point para la ISR
#define NO_TEST HIGH
#define MODE TEST
#define PIN_TEST PORTNUM2PIN(PC, 9)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct 
{
  uint8_t data    :4;   // Char bits
  uint8_t parity  :1;   // Odd parity bit
  uint8_t count   :3;   // Total amount of 1s in data
} data_t;


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
//OS
OS_ERR os_err_lectora;
OS_SEM *semaforoLectora;

// Data binary array
static uint8_t data[MAX_DATA];
// Data char array
static char track[MAX_TRACK];

static char ID[MAX_ID];

static bool data_arrived = false;
static int data_index = 0;
static bool data_stored = false;
static bool enable_reading = false;
static CReadErrors errors = CR_NoError;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
// IRQ called during either the beggining or the end of data storing
void pin_enable_IRQ (void);
// IRQ called on each rising flank to store new data
void pin_clk_IRQ (void);

/**
 * @brief Creates a character from the data array based on an index
 * @param reference index for the data array
 * @return a single character
 */
data_t create_data (int reference);

/**
 * @brief Checks the odd parity of a char data
 * @param data_char data character that needs to be analized
 * @return true if the parity is valid
 */
bool data_check_parity (data_t data_char);


void cr_timeout_mark (void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/********************************************************************************
 * Brief idea of the driver:                                                    *
 * The card reader has 3 channels: enable, clock and data. Enable indicates a   *
 * card is passing, thus data should be stored. Clock indicates the flow of     *
 * said data, and data is a flow of 1s and 0s that starts at 1 with the SS char.*
 *                                                                              *
 * Steps to follow :                                                            *
 *                                                                              *
 * 1) At the init, enable interrupts from the PIN_CR_ENABLE at both edges. It's *
 * importante to note that the falling edge (an interrupt followed by a confirm *
 * that the pin reads 0) indicates data should be stored, whereas a rising edge *
 * followed by a 1 marks the end of the data storing (or reaching the max track,*
 * whatever happens first).                                                     *
 *                                                                              *
 * 2) When the enable falling edge interrupted is trigger, it should enable     *
 * clock interrupts at rising edge. This interrupts need to be disabled once    *
 * the data is stored successfully.                                             *
 *                                                                              *
 * 3) The processing of the data is later done by the main app, the driver only *
 * stores incoming information during its interrupts. That data must be first   *
 * verified using partity check for each char.                                  *
 *                                                                              *
 * 4) Once the 40 Chars array is complete, the information should be divided so *
 * the app can get the necessary info. No trash or irrelevant info should go    *
 * outside of this driver.                                                      *
 *******************************************************************************/

void cread_init (OS_SEM *p_sem)
{
  //Setea Semaforo
  semaforoLectora = p_sem;

  // Initializes the 3 data pins
  gpioMode(PIN_CR_DATA, INPUT);
  gpioMode(PIN_CR_CLOCK, INPUT);
  gpioMode(PIN_CR_ENABLE, INPUT);

  // Enables only the enable pin interrupts, as the data should only be allowed at certain times
  gpioIRQ(PIN_CR_ENABLE, GPIO_IRQ_MODE_BOTH_EDGES, pin_enable_IRQ);
  gpioIRQ(PIN_CR_CLOCK, GPIO_IRQ_MODE_FALLING_EDGE, pin_clk_IRQ);
}

void cread_reset (void)
{
  data_arrived = false;
  data_index = 0;
  data_stored = false;
  enable_reading = false;
  errors = CR_NoError;

  int index;
  for (index = 0; index < MAX_DATA; index++)
  {
    data[index] = 0;
  }
  for (index = 0; index < MAX_TRACK; index++)
  {
    track[index] = '0';
  }
  for (index = 0; index < MAX_ID; index++)
  {
    ID[index] = '0';
  }
}


bool cread_askData (void)
{
  return data_stored;
}

uint8_t* cread_getData (void)
{
  return &data[0];
}


char* cread_getTrack (void)
{
  return &track[0];
}

char* cread_getID (void)
{
  if (check_track())
  {
	  for (int i = 0; i < MAX_ID; i++)
    {
      ID[i] = track [i+9];
    }
    
  }
  else
  {
    errors = CR_Invalid_ID;
  }
  return &ID[0];
}

CReadErrors cread_getError (void)
{
  return errors;
}

data_t create_data (int reference)
{
  data_t new_char;
  new_char.data = 0b0000;
  new_char.count = 0b000;
  new_char.parity = 0b0;
  int new_index;
  int new_count = 0;
  for (new_index = 0; new_index < MAX_BITS; new_index++ )
  {
    new_char.data |= (data[reference+new_index]<<new_index);
    if (data[reference+new_index] == 1)
    {
      new_count++;
    }
  }
  new_char.count = new_count;
  new_char.parity = data[reference+MAX_BITS];

  return new_char;
}

bool data_check_parity (data_t data_char)
{
  int total_bits = data_char.count + data_char.parity;
  if ((total_bits % 2) == 1)
    return true;
  else
    return false;
}

bool check_track (void)
{
  int trk_idx;
  for (trk_idx = 0; trk_idx < MAX_TRACK; trk_idx++)
  {
    switch (trk_idx)
    {
    case 0:
      if (track[trk_idx] != SS)
      {
        errors = CR_MissingSS;
        return false;
      }
      break;
    case (MAX_TRACK-2):
      if (track[trk_idx] != ES)
      {
        errors = CR_MissingES;
        return false;
      }
      break;
    case PAN:
      if (track[trk_idx] != FS)
      {
        errors = CR_MissingFS;
        return false;
      }
      break;
    default:                                // LOOK INTO THIS ONE, TEST MAY BE INVALID
      if ((trk_idx<PAN) && (trk_idx >0))   // Remember that ID is located in PAN
      {
        if ((track[trk_idx] < BASE) || (track[trk_idx] > (BASE+'9')))
        {
          errors = CR_Invalid_Track_Data;
          return false;
        }
      }
      break;
    }
  }
  return true;
}

void cread_processData (void)
{
  int idx;
  int track_idx = 0;
  data_t tmp_char;
  for (idx = 0; idx < MAX_DATA; idx += 5)
  {
    // Takes designed character
    tmp_char = create_data (idx);
    // Checks the parity
    if (data_check_parity (tmp_char))
    {
      // Coverts data to ASCII and saves it into the track
      track[track_idx] = BASE + tmp_char.data;
      track_idx++;
    }
    else
    {
      errors = CR_Invalid_Parity;
    }

  }
}


void pin_enable_IRQ (void)
{
	#if (MODE == TEST)
	gpioWrite(PIN_TEST, HIGH);
	#endif
  bool pin_en;
  pin_en = gpioRead(PIN_CR_ENABLE);
  // Data is incomming when enable is LOW
  if ((!pin_en) && (!data_stored))
  {
    // Clears flags
    cread_reset ();
    enable_reading = true;
    // gpioIRQ(PIN_CR_CLOCK, GPIO_IRQ_MODE_FALLING_EDGE, pin_clk_IRQ);
  }
  else if (pin_en && data_stored)
  {
    enable_reading = false;
    // gpioIRQ(PIN_CR_CLOCK, GPIO_IRQ_MODE_DISABLE, pin_clk_IRQ);
  }
  
  else
  {
    enable_reading = false;
    //gpioIRQ(PIN_CR_CLOCK, GPIO_IRQ_MODE_DISABLE, pin_clk_IRQ);
  }
	#if (MODE == TEST)
	gpioWrite(PIN_TEST, LOW);
	#endif

}

void pin_clk_IRQ (void)
{
	#if (MODE == TEST)
	gpioWrite(PIN_TEST, HIGH);
	#endif
  if(enable_reading && (!data_stored))
  {
    uint8_t pin_data;
    pin_data = !gpioRead(PIN_CR_DATA);
    // Waits for the first 1 to arrive to start saving data
    if (!data_arrived)
    {
      if (pin_data)
      {
        data_arrived = true;
        data[data_index] = pin_data;
        data_index++;
      }
      else
      {
        // Do nothing
      }
    }
    else
    {
      // Saves up to 200 bits of information before deciding no more is needed.
      if (data_index < MAX_DATA)
      {
        data[data_index] = pin_data;
        data_index++;
      }
      else
      {
        data_stored = true;

		//Semaforo en Verde (Se paso la tarjeta)
		OSSemPost(semaforoLectora, OS_OPT_POST_1, &os_err_lectora);
      }
    }
  }
	#if (MODE == TEST)
	gpioWrite(PIN_TEST, LOW);
	#endif

}


void cr_timeout_mark (void)
{
  cread_reset();
}


