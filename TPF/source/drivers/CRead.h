/***************************************************************************//**
  @file     CRead.h
  @brief    Card Reader Driver
  @author   Pedro Dalla Lasta
 ******************************************************************************/

#ifndef _CREAD_H_
#define _CREAD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <os.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef enum 
{
  CR_NoError,
  CR_Invalid_Parity,
  CR_MissingSS,
  CR_MissingFS,
  CR_MissingES,
  CR_Invalid_Track_Data,
  CR_TimeOut,
  CR_Invalid_ID
} CReadErrors;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initializes card reading driver
 */
void cread_init (OS_SEM *p_sem);

/**
 * @brief Resets card reader flags
 */
void cread_reset (void);

/**
 * @brief Asks if data has arrived
 * @return true if there is data stored
 */
bool cread_askData (void);

/**
 * @brief Asks for the last saved data from the card reader
 * @return data array in binary
 */
uint8_t* cread_getData (void);

/**
 * @brief Asks for the last saved track from the card reader
 * @return data array in char
 */
char* cread_getTrack (void);


/**
 * @brief Asks for the last saved ID
 * @return ID array in char
 */
char* cread_getID (void);

/**
 * @brief Checks if any error has appeard during the reading or processing process
 * @return Error type corresponding to the card reader
 */
CReadErrors cread_getError (void);

/**
 * @brief Begin to run a new timer
 * @param id ID of the timer to start
 * @return data array in chars
 */
void cread_processData (void);

/**
 * @brief Checks the track was correctly saved (validates SS, PAN, FS and ES)
 * @return true if the track is valid
 */
bool check_track (void);

/*******************************************************************************
 ******************************************************************************/

#endif // _CREAD_H_
