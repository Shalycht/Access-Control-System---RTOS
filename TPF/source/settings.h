/******************************************************************************
  @file     settings.h
  @brief    Certain settings for the application
  @author   Grupo Nº5
 ******************************************************************************/

#ifndef SETTINGS_H_
#define SETTINGS_H_

//***************************************************
// 				INCLUDE HEADER FILES
//***************************************************
#include <stddef.h>
#include "drivers/board.h"
#include "drivers/CRead.h"
#include "drivers/display_7s.h"
#include "drivers/encoder.h"

//***************************************************
// 			CONSTANT AND MACRO DEFINITIONS
//***************************************************
#define MAX_USERS 12
#define ID_LEN 8
#define MAX_PSW_LEN 5
#define MIN_USER 0
#define STARTING_USERS 6
#define QUEUE_LEN 6

#define LOGIN_KEY 1
#define BRIGHT_KEY 2
#define SUPER_KEY 3

#define FLOORS 3
#define FLOOR_USERS 4

#define DOUBLE_TICKS 1000
#define ERROR_TICKS 5000
#define UNLOCK_TICKS 3000
#define EVENT_MSG_TICKS 5000
#define MATRIX_TOGGLE_TICKS 2000

//***************************************************
// 		ENUMERATIONS AND STRUCTURES AND TYPEDEFS
//***************************************************
typedef enum
{
    ST_MAIN,
    ST_LOGIN,
    ST_INT,
    ST_ADMIN,
    ST_UNLOCK,
    ST_ERROR,
    ST_DELETE,
	ST_SUCCESS,
    ST_MSG,
    ST_INIT
} state_t;

enum
{
    MAIN_BRI,
    MAIN_PASS,
    MAIN_LOG,
    MAIN_SUB_BRI,
    MAIN_SUB_PASS
};

enum
{
    ADM_DEL,
    ADM_ADD,
    ADM_SUB_FLR,
    ADM_SUB_DEL,
    ADM_SUB_ADD
};

typedef enum
{
    EV_CARD,
    EV_EN_RIGHT,
    EV_EN_LEFT,
    EV_EN_PUSH,
    EV_EN_DOUBLE,
    EV_EN_HOLD
} event_t;

typedef enum
{
    ERR_NO_ERROR,
    ERR_CARD,
    ERR_ENCODER,
    ERR_WRONG_ACCESS,
    ERR_MAX_USERS_ACHIEVED,
    ERR_NO_USERS_TO_ERASE,
    ERR_USER_REGISTERED,
    ERR_TENANT
} errors_t;

//***************************************************
// 				FUNCTION PROTOTYPES
//***************************************************
// Callback for double push timer (on timer expired is a single push)
void event_double_tap (void);
// Callback for an error in access
void event_error (void);
// Callback for an unlock event
void event_unlock (void);
// Callback to display a message
void event_msg (void);
// Callback to blink the matrix
void event_matrix_toggle (void);
// Function to check all the active event flags
void update_queue (void);
// Clean all the encoder events and save the last one
void clean_encoder (int event);
// Process the card data and resets its flags
void save_card_data (void);
// Initializes admin users
void init_users (void);
// Initializes floors and tenants for the matrix
void floor_init (void);
//Para ingreso de datos
void show_display (bool state);
//Clean msg
void clean_msg_half (void);
// Corrects users array upon deletion
void correct_users (int index);


// App_Run menus
void main_menu(void);
void admin_menu(void);
void login_menu(void);
void delete_menu(void);
void error_menu (void);
void unlock_menu (void);
void init_menu(void);

//App Init
void App_Init(void);

//App Run
void App_Run(void);


#endif /* SETTINGS_H_ */
