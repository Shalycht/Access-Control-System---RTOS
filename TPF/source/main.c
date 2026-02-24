/******************************************************************************
  @file     main.c
  @brief    Trabajo Práctico Nº4 - RTOS
  @author   Grupo Nº5
 ******************************************************************************/

//********************************************
//	 		INCLUDE HEADER FILES
//********************************************

#include <os.h>
#include "hardware.h"
#include "settings.h"
#include "drivers/building.h"

//********************************************
//	 		 Application Variables
//********************************************

// Building variables

// keeps track of how many tenants are on each floor
static uint8_t floor_users [MAX_TENANTS];
// Keeps track of which tenant logged in
static bool active_tenants [(MAX_TENANTS*3)+1];

// User variables
static uint8_t users [MAX_USERS + 1] [ID_LEN]; // All 12 tenants + the admin (0)
static uint8_t passwords [MAX_USERS+1] [MAX_PSW_LEN];
static uint8_t user_count = STARTING_USERS;
static bool queue [QUEUE_LEN];
static uint8_t mistakes_count = 0;

// Index used to save password when adding users
static int login_user = 0;

// Events variables
static bool double_timer = false;
static uint8_t current_ID[ID_LEN];

// Login variables
static uint8_t login_ID[ID_LEN];
static uint8_t login_pass[MAX_PSW_LEN];
// log in input lenght index (to know where am I in the entry array)
static int login_idx = 0;
// log in character selection index
static int login_char = 0;
// flag to ensure it has finished inputting data
static bool data_pushed = false;
// flag to distinguish login inputs (between password (true) and ID (false))
static bool login_pwd = false;
// flag to diferenciate between short and long password
static bool short_pwd = false;
// flag to distinguish between login and adding users
static bool login_add = false;
static errors_t error = ERR_NO_ERROR;
// flag to signal successful adding user attempt
static bool login_add_success = false;

// User variable
// flag to signal the user wants to change the password
static bool chg_pass = false;

// Main flow variables
static int current_state = ST_INIT;
static int main_idx = MAIN_BRI;
static int admin_idx = ADM_DEL;

// Aux state for message display
static int next_state = ST_LOGIN;

int brightness;
static char msg_half[4];
static char msg_ID[8];

// Building related
// User count by floor (registered users)
static uint8_t building [FLOORS];
// Logged user count by floor (for thingspeak usage)
static uint8_t logged_users [FLOORS];
// Flag to signal if a tenant has logged in or out of the building
static bool tenants [MAX_USERS+1];
// Counter to add or delete users from a certain floor
static int adm_floor;

// Color for the matrix. Used for a toggle function
static colores_t alert_color = RED;
// Brightness for the matrix. Used for a toggle function
static brightness_t alert_bri = LVL1;
// Type of alert. 0 = Unlock, 1 = Error, 2 = Admin
static bool alert_type [3];
// Flag to prevet a timer reset each time the program enters the task. Resets when all alerts are down
static bool alert_rst = false;

// Delete menu variables
// As it's being called continuously, in order not to reset or overwrite this they need to be global
// Tenants on current selected floor
static int del_floor_cnt = 0;
// User of said floor indexed
static int del_user_n = 0;

//********************************************
//	 			RTOS Task Configs
//********************************************
//Task Start
#define TASKSTART_STK_SIZE 		512u
#define TASKSTART_PRIO 			3u
static OS_TCB TaskStartTCB;
static CPU_STK TaskStartStk[TASKSTART_STK_SIZE];

//Matrix Task
#define TASK_MATRIX_STK_SIZE		256u
#define TASK_MATRIX_STK_SIZE_LIMIT	(TASK_MATRIX_STK_SIZE / 10u)
#define TASK_MATRIX_PRIO           3u
static OS_TCB TaskMatrixTCB;
static CPU_STK TaskMatrixStk[TASK_MATRIX_STK_SIZE];

//********************************************
//	 			Objetos RTOS
//********************************************
//Semaforo Encoder
static OS_SEM semEncoder;

//Semaforo Lectora
static OS_SEM semLectora;

#define CANT_SEM 2
OS_PEND_DATA pend_data_tbl[CANT_SEM];

//OS
OS_ERR os_err;

//********************************************
//	 			 Timers RTOS
//********************************************
//Timers para el Encoder
OS_TMR timerOuts_Encoder;
OS_TMR timerSW_Encoder;

//Callback timerOuts_Encoder
void timerOutsCallback(OS_TMR *p_tmr, void *p_arg)
{
	timerOuts(&semEncoder);
}

//Callback timerSW_Encoder
void timerSWCallback(OS_TMR *p_tmr, void *p_arg)
{
	timerSW(&semEncoder);
}

//Timers para el Display
OS_TMR timerDisplay;
OS_TMR timerDisplayMSG;

//Callback timerDisplay
void timerDisplayCallback(OS_TMR *p_tmr, void *p_arg)
{
	Timer_Display_Handler();
}

//Callback timerDisplayMSG
void timerDisplayMSGCallback(OS_TMR *p_tmr, void *p_arg)
{
	Timer_Message_Handler();
}

//Timers para la APP
OS_TMR timerDouble;
OS_TMR timerError;
OS_TMR timerUnlock;
OS_TMR timerMessage;
OS_TMR timerMatrixToggle;
OS_TMR timerID;

//Callback timerDouble
void timerDoubleCallback(OS_TMR *p_tmr, void *p_arg)
{
	event_double_tap();
}

//Callback timerError
void timerErrorCallback(OS_TMR *p_tmr, void *p_arg)
{
	event_error();
}

//Callback timerUnlock
void timerUnlockCallback(OS_TMR *p_tmr, void *p_arg)
{
	event_unlock();
}

//Callback timerMessage
void timerEventMessageCallback(OS_TMR *p_tmr, void *p_arg)
{
	event_msg();
}

//Callback timerMatrix
void timerEventMatrixCallback(OS_TMR *p_tmr, void *p_arg)
{
	event_matrix_toggle();
}

void timerIDOKCallback(OS_TMR *p_tmr, void *p_arg)
{
	event_matrix_toggle();
}


// Old variables. Do not delete
#define MSG_LENGTH 5
//Message received from Message Queue
char msg[MSG_LENGTH];
//Parse of Message received from Message Queue
char floor;
char msg_lsb;
char msg_msb;



//********************************************
//	 			Matrix Task
//********************************************
static void TaskMatrix (void *p_arg) {
	(void)p_arg;
    OS_ERR os_err;



    while (1) {
    	
   
        // Update floors

        for (int floor_idx = 0; floor_idx < FLOORS; floor_idx++)
        {
            setFloor(floor_users[floor_idx], floor_idx);
        }

    	
        // Check the blinking of 4th Column
        if (alert_type[0] == true)
        {
            alert_color = GREEN;
            alert_bri = LVL1;
            if (!alert_rst)
            {
                alert_rst = true;
                OSTmrStart(&timerMatrixToggle, &os_err);
            }
        }
        else if (alert_type[1] == true)
        {
            alert_color = RED;
            alert_bri = LVL1;
            if (!alert_rst)
            {
                alert_rst = true;
                OSTmrStart(&timerMatrixToggle, &os_err);
            }
        }
        else if (alert_type[2] == true)
        {
            alert_color = YELLOW;
            alert_bri = LVL1;
            if (!alert_rst)
            {
                alert_rst = true;
                OSTmrStart(&timerMatrixToggle, &os_err);
            }
        }
        else
        {
            alert_rst = false;
            alert_bri = OFF;
            OSTmrStop(&timerMatrixToggle, (OS_OPT) OS_OPT_TMR_PERIODIC, (void*) NULL, &os_err);
        }
         
                        

    	//Wait 1sec until next update
    	OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);
    }
}

//********************************************
//	 			Start Task
//********************************************
static void TaskStart(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;

    //CPU Init
    CPU_Init();

#if OS_CFG_STAT_TASK_EN > 0u
    /* (optional) Compute CPU capacity with no task running */
    OSStatTaskCPUUsageInit(&os_err);
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    //Create TaskMatrix
    OSTaskCreate(&TaskMatrixTCB, 				//tcb
                 "Task Matrix",				//name
                  TaskMatrix,					//func
                  0u,							//arg
                  TASK_MATRIX_PRIO,			//prio
                 &TaskMatrixStk[0u],			//stack
                  TASK_MATRIX_STK_SIZE_LIMIT,	//stack limit
                  TASK_MATRIX_STK_SIZE,		//stack size
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_err);

    //Setting para el OSPendMulti
    pend_data_tbl[0].PendObjPtr = (OS_PEND_OBJ*) &semEncoder;
    pend_data_tbl[1].PendObjPtr = (OS_PEND_OBJ*) &semLectora;



    //Semaforo Encoder
    OSSemCreate(&semEncoder, "Sem Encoder", 0u, &os_err);

    //Semaforo Lectora
    OSSemCreate(&semLectora, "Sem Lectora", 0u, &os_err);

    //TimerOuts_Encoder
    OSTmrCreate((OS_TMR*) &timerOuts_Encoder,
    			(CPU_CHAR*) "TimerOuts Encoder",
				(OS_TICK) 0,
				(OS_TICK) OUTS_PERIOD_MS,
				(OS_OPT) OS_OPT_TMR_PERIODIC,
				(OS_TMR_CALLBACK_PTR) timerOutsCallback,
				(void*) NULL,
				(OS_ERR*) &os_err);

    //TimerSW_Encoder
    OSTmrCreate((OS_TMR*) &timerSW_Encoder,
    			(CPU_CHAR*) "TimerSW Encoder",
				(OS_TICK) 0,
				(OS_TICK) SW_PERIOD_MS,
				(OS_OPT) OS_OPT_TMR_PERIODIC,
				(OS_TMR_CALLBACK_PTR) timerSWCallback,
				(void*) NULL,
				(OS_ERR*) &os_err);

    //TimerDisplay
    OSTmrCreate((OS_TMR*) &timerDisplay,
    			(CPU_CHAR*) "Timer Display",
				(OS_TICK) 0,
				(OS_TICK) DISPLAY_TICKS,
				(OS_OPT) OS_OPT_TMR_PERIODIC,
				(OS_TMR_CALLBACK_PTR) timerDisplayCallback,
				(void*) NULL,
				(OS_ERR*) &os_err);

    //TimerDisplayMSG
    OSTmrCreate((OS_TMR*) &timerDisplayMSG,
    			(CPU_CHAR*) "Timer Display MSG",
				(OS_TICK) 0,
				(OS_TICK) DISPLAY_MSG_TICKS,
				(OS_OPT) OS_OPT_TMR_PERIODIC,
				(OS_TMR_CALLBACK_PTR) timerDisplayMSGCallback,
				(void*) NULL,
				(OS_ERR*) &os_err);

    //TimerDouble
    OSTmrCreate((OS_TMR*) &timerDouble,
    			(CPU_CHAR*) "Timer Double",
				(OS_TICK) DOUBLE_TICKS,
				(OS_TICK) 0,
				(OS_OPT) OS_OPT_TMR_ONE_SHOT,
				(OS_TMR_CALLBACK_PTR) timerDoubleCallback,
				(void*) NULL,
				(OS_ERR*) &os_err);

    //TimerError
    OSTmrCreate((OS_TMR*) &timerError,
    			(CPU_CHAR*) "Timer Error",
				(OS_TICK) ERROR_TICKS,
				(OS_TICK) 0,
				(OS_OPT) OS_OPT_TMR_ONE_SHOT,
				(OS_TMR_CALLBACK_PTR) timerErrorCallback,
				(void*) NULL,
				(OS_ERR*) &os_err);

    //TimerUnlock
    OSTmrCreate((OS_TMR*) &timerUnlock,
    			(CPU_CHAR*) "Timer Unlock",
				(OS_TICK) UNLOCK_TICKS,
				(OS_TICK) 0,
				(OS_OPT) OS_OPT_TMR_ONE_SHOT,
				(OS_TMR_CALLBACK_PTR) timerUnlockCallback,
				(void*) NULL,
				(OS_ERR*) &os_err);

    //TimerMessage
    OSTmrCreate((OS_TMR*) &timerMessage,
    			(CPU_CHAR*) "Timer Event Message",
				(OS_TICK) EVENT_MSG_TICKS,
				(OS_TICK) 0,
				(OS_OPT) OS_OPT_TMR_ONE_SHOT,
				(OS_TMR_CALLBACK_PTR) timerEventMessageCallback,
				(void*) NULL,
				(OS_ERR*) &os_err);

    //TimerMatrixToggle
    OSTmrCreate((OS_TMR*) &timerMatrixToggle,
    			(CPU_CHAR*) "Timer Matrix Toggle",
				(OS_TICK) MATRIX_TOGGLE_TICKS,
				(OS_TICK) MATRIX_TOGGLE_TICKS,
				(OS_OPT) OS_OPT_TMR_PERIODIC,
				(OS_TMR_CALLBACK_PTR) timerEventMatrixCallback,
				(void*) NULL,
				(OS_ERR*) &os_err);

     //TimerIDOK
    OSTmrCreate((OS_TMR*) &timerID,
    			(CPU_CHAR*) "Timer ID OK",
				(OS_TICK) MATRIX_TOGGLE_TICKS,
				(OS_TICK) 0,
				(OS_OPT) OS_OPT_TMR_ONE_SHOT,
				(OS_TMR_CALLBACK_PTR) timerIDOKCallback,
				(void*) NULL,
				(OS_ERR*) &os_err);

    //TimerOuts_Encoder Start
    OSTmrStart(&timerOuts_Encoder, &os_err);

    //TimerSW_Encoder Start
    OSTmrStart(&timerSW_Encoder, &os_err);

    //Timer Display Start
    OSTmrStart(&timerDisplay, &os_err);

    //Timer Display MSG Start
    OSTmrStart(&timerDisplayMSG, &os_err);

    while (1) {
    	App_Run();
    }
}

//********************************************
//	 				Main
//********************************************
int main(void) {
    OS_ERR err;

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_ERR  cpu_err;
#endif

	hw_Init();
    hw_DisableInterrupts();

    //Clear floors
    floor_init();
    // Init the led matrix
    InitBuilding();
    //Init App
    App_Init();

	hw_EnableInterrupts();

	//Init Leds
	gpioMode(PIN_LED_RED, OUTPUT);
	gpioMode(PIN_LED_GREEN, OUTPUT);
	gpioMode(PIN_LED_BLUE, OUTPUT);

	gpioWrite(PIN_LED_RED, HIGH);
	gpioWrite(PIN_LED_GREEN, HIGH);
	gpioWrite(PIN_LED_BLUE, HIGH);

	//Init OS
    OSInit(&err);

 #if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
     //Enable Round Robin
	 OSSchedRoundRobinCfg((CPU_BOOLEAN)1, 0, &err);
 #endif
    OS_CPU_SysTickInit(SystemCoreClock / (uint32_t)OSCfg_TickRate_Hz);

    OSTaskCreate(&TaskStartTCB,
                 "App Task Start",
                  TaskStart,
                  0u,
                  TASKSTART_PRIO,
                 &TaskStartStk[0u],
                 (TASKSTART_STK_SIZE / 10u),
                  TASKSTART_STK_SIZE,
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 &err);

    OSStart(&err);

	//Should Never Get Here
    while (1) {

    }
}


//********************************************
//	 		  Application Init
//********************************************
void App_Init(void)
{
    //Init Card Reader
    cread_init(&semLectora);

    //Init Display
    display_pins_t pins;
    pins.pin_a = PIN_a;
    pins.pin_b = PIN_b;
    pins.pin_c = PIN_c;
    pins.pin_d = PIN_d;
    pins.pin_e = PIN_e;
    pins.pin_f = PIN_f;
    pins.pin_g = PIN_g;
    pins.pin_dp = PIN_DP;
    pins.pin_sel0 = PIN_SEL_0;
	pins.pin_sel1 = PIN_SEL_1;
	display_Init(pins);

    //Init Encoder
    Encoder_Init(PTB3, PTB2, PTD1);

    //Suggested brightness level
    brightness = 6;
    gpioMode(PTC9,OUTPUT);

    //Init Building
    init_users();
}

//********************************************
//	 			Application
//********************************************
void App_Run(void)
{
	//Chequeo si hubo evento
	OSPendMulti((OS_PEND_DATA*)&pend_data_tbl[0],
	            (OS_OBJ_QTY) CANT_SEM,
	            (OS_TICK) 10,
	            (OS_OPT) OS_OPT_PEND_BLOCKING,
	            (OS_ERR*) &os_err);

	if(os_err == OS_ERR_TIMEOUT)
	{
		//Do Nothing
	}
	else
	{
	    update_queue();
	}

    switch (current_state)
    {
        // First state. Either loggin in as a user or admin
        case ST_LOGIN:
            // when successful goes into UNLOCK, and saves in login_user which user entered
            login_menu();
            break;
        // Intermediate state to indicate success after a log in
        case ST_UNLOCK:
            // 3s displaying welcome message
            unlock_menu();
            break;
        // Intermediate state to signal an error that needs to be displayed
        case ST_ERROR:
            error_menu();
            break;
        // First destination: when login_user = 0 (admin) it can erase or add users to the building
        case ST_ADMIN:
            admin_menu();
            break;
        case ST_MAIN:
            main_menu();
            break;
        case ST_INIT:
            init_menu();
            break;

    }
}

//********************************************
//	 			Local Functions
//********************************************
//init_users for all floors. Need to be expanded up to 3 per floor
void init_users(void)
{
	// Admin user
	char temp_ID0 [] = "00000007";
	for (int i=0;i< ID_LEN; i++)
	{
		users[0][i] = temp_ID0[i];
	}
	char temp_pass0 [] = "00086";
	for (int i=0;i< MAX_PSW_LEN; i++)
	{
		passwords[0][i] = temp_pass0[i];
	}

    // Floor 1 User 1
	char temp_ID1 [] = "39372604";
	for (int i=0;i< ID_LEN; i++)
	{
		users[1][i] = temp_ID1[i];
	}
	char temp_pass1 [] = "72854";
	for (int i=0;i< MAX_PSW_LEN; i++)
	{
		passwords[1][i] = temp_pass1[i];
	}
    // Floor 1 User 2
    char temp_ID2 [] = "06799370";
	for (int i=0;i< ID_LEN; i++)
	{
		users[2][i] = temp_ID2[i];
	}
	char temp_pass2 [] = "6288";
	for (int i=0;i< MAX_PSW_LEN-1; i++)
	{
		passwords[2][i] = temp_pass2[i];
	}
	passwords[2][4] = '0' + 10;
	// Floor 1 User 3
	char temp_ID7 [] = "00019741";
	for (int i=0;i< ID_LEN; i++)
	{
		users[3][i] = temp_ID7[i];
	}
	char temp_pass7 [] = "00000";
	for (int i=0;i< MAX_PSW_LEN; i++)
	{
		passwords[3][i] = temp_pass7[i];
	}
    // Floor 2 User 1
	char temp_ID3 [] = "25969879";
	for (int i=0;i< ID_LEN; i++)
	{
		users[5][i] = temp_ID3[i];
	}
	char temp_pass3 [] = "12783";
	for (int i=0;i< MAX_PSW_LEN; i++)
	{
		passwords[5][i] = temp_pass3[i];
	}
    // Floor 2 User 2
    char temp_ID4 [] = "05056431";
	for (int i=0;i< ID_LEN; i++)
	{
		users[6][i] = temp_ID4[i];
	}
	char temp_pass4 [] = "4437";
	for (int i=0;i< MAX_PSW_LEN-1; i++)
	{
		passwords[6][i] = temp_pass4[i];
	}
	passwords[6][4] = '0' + 10;

    // Floor 3 User 1
	char temp_ID5 [] = "23537157";
	for (int i=0;i< ID_LEN; i++)
	{
		users[9][i] = temp_ID5[i];
	}
	char temp_pass5 [] = "37730";
	for (int i=0;i< MAX_PSW_LEN; i++)
	{
		passwords[9][i] = temp_pass5[i];
	}
    // Floor 3 User 2
    char temp_ID6 [] = "07091945";
	for (int i=0;i< ID_LEN; i++)
	{
		users[10][i] = temp_ID6[i];
	}
	char temp_pass6 [] = "3773";
	for (int i=0;i< MAX_PSW_LEN-1; i++)
	{
		passwords[10][i] = temp_pass6[i];
	}
	passwords[10][4] = '0' + 10;

    // Initialize building users by floor
    for (int i=0;i< 4; i++)
	{
		building[i] = 3;
	}
    for (int i=0;i< 4; i++)
	{
		logged_users[i] = 0;
	}
    for (int i=0;i< 12; i++)
	{
		tenants[i] = false;
	}
}

//update_queue
void update_queue(void)
{
	if(pend_data_tbl[0].RdyObjPtr != NULL)
	{
		//Hubo evento de Encoder
	    switch (encoder_Read())
	    {
	        case NOTHING_HAPPENED:
	            // do nothing
	            break;
	        case SW_PRESSED:
	            // Start timer to confirm simple push
	            if (!double_timer)
	            {
	                //Timer Double
	                OSTmrStart(&timerDouble, &os_err);
	                double_timer = true;
	            }
	            // Arrives another tap before the timer expires
	            else
	            {
	            	//Timer Double
	            	OSTmrStop(&timerDouble, OS_OPT_TMR_NONE,NULL, &os_err);
	                clean_encoder(EV_EN_DOUBLE);
	                double_timer=false;
	            }
	            break;
	        case SW_HOLD:
	            clean_encoder(EV_EN_HOLD);
	            break;
	        case CLOCKWISE:
	            clean_encoder(EV_EN_RIGHT);
	            break;
	        case COUNTER_CLOCKWISE:
	            clean_encoder(EV_EN_LEFT);
	            break;
	        default:
	            break;
	    }
	}

	if(pend_data_tbl[1].RdyObjPtr != NULL)
	{
		//Hubo evento Lectora
	    queue[EV_CARD] = true;
	    save_card_data();
	}
}

// On app time saves on current_ID the data of the last card used
void save_card_data(void)
{
    // Process data in app time
    cread_processData();
    // Checks for error during process and a valid track
    // if ((cread_getError() == CR_NoError) && (check_track()))              // <- NEED TO VALIDATE THIS ONE
    if (cread_getError() == CR_NoError)                                      // <- PUT ONLY FOR TESTING THE PROGRAM RATHER THAN THE CARD READER
    {
        // Saves the ID into a temporary array
        char* id_ptr = cread_getID();

        for (int i = 0; i < 8; i++)
        {
            current_ID[i] = id_ptr[i];
        }
    }
    // Skip card reading errors in order to avoid 'false' log ins
    else
    {
        queue[EV_CARD] = false;
    }
    // Clear the card flags should a new card arrive
    cread_reset();
}

//clean_msg_half
void clean_msg_half(void)
{
    for (int i = 0; i <(ID_LEN/2); i++)
    {
        msg_half[i] = ' ';
    }
}

//clean_encoder
void clean_encoder(int event)
{
    // clean all the encoder events
    for (int i = EV_EN_RIGHT; i <= EV_EN_HOLD; i++)
    {
        queue[i] = false;
    }
    // Saves only the last encoder event
    queue[event] = true;
}

//show_display
void show_display(bool state)
{

    if(login_idx < ID_LEN/2)    //Primera mitad
    {
        clean_msg_half();
        for(int i = 0; i < login_idx; i++)
        {
            if (state == false)    //ID
            {
                //Muestra en display todos los anteriores a login_idx
                msg_half[i] = login_ID[i];
                display_config_t msg = {msg_half, 4, brightness, NO_SPIN, 0, NO_BLINK, 0};
                display_msg(msg);
            }
            else    //PIN
            {
                //Valores anteriores se ven como un - si se esta en pass
                msg_half[i] = '-';
                display_config_t msg = {msg_half, 4, brightness, NO_SPIN, 0, NO_BLINK, 0};
                display_msg(msg);
            }
        }

        //Titilando el login char del login_idx
        msg_half[login_idx] = '0' + login_char;
        display_config_t msg = {msg_half, 4, brightness, NO_SPIN, 0, NO_BLINK, 0};
        display_msg(msg);
    }
    else if ((login_idx >= ID_LEN/2) && (login_idx < ID_LEN))    //Segunda mitad
    {
        clean_msg_half();
        for(int i = (ID_LEN/2); i < login_idx; i++)
        {
            if (state == false)    //ID
            {
                //Muestra en display todos los anteriores a login_idx
                msg_half[i-ID_LEN/2] = login_ID[i];
                display_config_t msg = {msg_half, 4, brightness, NO_SPIN, 0, NO_BLINK, 0};
                display_msg(msg);
            }
        }
        if ((state == true) && (login_char == 10))    //5to digito del PIN
        {
            //Muestra n para indicar que no se usa el 5to digito
            msg_half[login_idx-ID_LEN/2] = 'n';
            display_config_t msg = {msg_half, 4, brightness, NO_SPIN, 0, NO_BLINK, 0};
            display_msg(msg);
        }
        else
        {
            //Titilando el login char del login_idx
            msg_half[login_idx-ID_LEN/2] = '0' + login_char;
            display_config_t msg = {msg_half, 4, brightness, NO_SPIN, 0, NO_BLINK, 0};
            display_msg(msg);
        }
    }
}

//login_menu
void login_menu(void)
{
    if(login_pwd == false)    //Esta ingresando ID
    {
        show_display(false);
    }
    else    //Esta ingresando PIN
    {
        show_display(true);
    }

    // Checks for a card event
	if(queue[EV_CARD] == true)
	{
        // Turn off the queue flag
		queue[EV_CARD] = false;

        // Only a valid action if the card is enter without prior numbers entered via encoder
		if (login_idx == 0)
		{
            // Since login works both for password and user, checks it's entered during the user step
			if (!login_pwd)
			{
                // Load the login ID
				for (int i = 0; i < ID_LEN; i++)
				{
					login_ID[i] = current_ID [i];
				}
                data_pushed = true;

                alert_color = GREEN;
                alert_bri = LVL1;
                event_matrix_toggle();
                OSTmrStart(&timerID, &os_err);
			}
		}

	}
    // Confirm current number
	else if (queue[EV_EN_PUSH] == true)
    {
    	queue[EV_EN_PUSH]=false;
        // Orders to first check for ID then for password
        if (!login_pwd)
        {
             if(login_idx < ID_LEN-1)
            {
                login_ID[login_idx] = '0' + login_char;
                login_idx++;
                login_char = 0;
            }
            // After the last digit starts checking
            else if (login_idx == ID_LEN-1)
            {
                login_ID[login_idx] = '0' + login_char;

                data_pushed = true;
                
            }
        }
        else
        {
            if(login_idx < (MAX_PSW_LEN-1))
            {
                login_pass[login_idx] = '0' + login_char;
                login_idx++;
                login_char = 0;
            }
            // Checks if it's a long or short password
            else if (login_idx == 4)
            {
                login_pass[login_idx] = '0' + login_char;
                // A 4 length password
                if (login_char  == 10)
                {
                    short_pwd = true;
                }
                // A five length password
                else
                {
                    short_pwd = false;

                }
                data_pushed = true;
            }
        }
    }
    // Deletes the last saved ID char (or allows to change the input user)
    else if (queue[EV_EN_DOUBLE] == true)
    {
    	queue[EV_EN_DOUBLE]=false;
        if(!login_pwd)
        {
            if (login_idx > 0)
            {
                login_ID[login_idx] = '0';
                login_idx--;
                login_char = 0;
            }
            // In case its adding a user and wants to quit
            else if (login_add)
            {
                login_add = false;
                admin_idx = ADM_DEL;
            }
            else
            {
                // It's assume a double push on 0 is a mistake rather than going back
            }
        }
        else
        {
            if (login_idx > 0)
            {
                login_pass[login_idx] = '0';
                login_idx--;
                login_char = 0;
            }
            else if (!chg_pass)
            {
                // Goes back to change the user
                login_pwd = false;
                login_idx = 0;
                login_char = 0;
            }
            else if (chg_pass)
            {
                // Goes back to the main if its changing a password
                main_idx = MAIN_BRI;
                chg_pass = false;
                login_idx = 0;
                login_char = 0;
            }
        }
    }
    // Moves between 9 or 10 digits (depends if input or output)
    else if (queue[EV_EN_RIGHT] == true)
    {
    	queue[EV_EN_RIGHT]=false;
        login_char++;
        if (login_pwd)
        {
            // only for the 5th slot the char 10 is available, otherwise same as ID
            if (login_idx == 4)
            {
                if (login_char > 10)
                {
                    login_char = 0;
                }
            }
            else
            {
                if (login_char >= 10)
                {
                    login_char = 0;
                }
            }
        }
        else
        {
            if (login_char >= 10)
            {
                login_char = 0;
            }
        }

    }
    else if (queue[EV_EN_LEFT] == true)
    {
    	queue[EV_EN_LEFT]=false;
        login_char--;
        if (login_pwd)
        {
            // only for the 5th slot the char 10 is available, otherwise same as ID
            if (login_idx == 4)
            {
                if (login_char < 0)
                {
                    login_char = 10;
                }
            }
            else
            {
                if (login_char < 0)
                {
                    login_char = 9;
                }
            }
        }
        else
        {
            if (login_char < 0)
            {
                login_char = 9;
            }
        }
    }
    // Instant quit from any part of the process
    else if (queue[EV_EN_HOLD] == true)
    {
        queue[EV_EN_HOLD] = false;
        // Back to admin_menu()
        if (login_add)
        {
            admin_idx = ADM_DEL;
            login_add = false;
            data_pushed = false;
        }
        // Back to main_menu()
        else if (chg_pass)
        {
            main_idx = MAIN_BRI;
            chg_pass = false;
            data_pushed = false;
        }
        else
        {
            login_pwd = false;
            login_idx = 0;
            login_char = 0;
            current_state = ST_INIT;
        }
    }

    if (data_pushed == true)
    {
        // Login ID check
        if (!login_pwd)
        {
        
            if (!login_add)
            {
                // Travels through the user database in order to check for a match
                for (int uc_i = 0; uc_i <= MAX_USERS; uc_i++)
                {
                    bool match_found = true;
                    for (int u_i = 0; u_i < ID_LEN; u_i++)
                    {
                        if (login_ID[u_i] != users[uc_i][u_i])
                        {
                            match_found = false;
                            break;
                        }
                    }
                    // In case the match is found marks to proceed to password
                    if (match_found)
                    {
                        login_user = uc_i;
                        login_pwd = true;
                        login_idx = 0;
                        login_char = 0;
                        // Reset mistaker counter
                        mistakes_count = 0;     
                        alert_color = GREEN;
                        alert_bri = LVL1;
                        event_matrix_toggle();
                        OSTmrStart(&timerID, &os_err); 
                    }
                }
                // In case the user is invalid after all the database is revised, increase the errors
                if (!login_pwd)
                {
                    mistakes_count++;
                    login_idx = 0;
                    login_char = 0;
                    alert_color = RED;
                    alert_bri = LVL1;
                    event_matrix_toggle();
                    OSTmrStart(&timerID, &os_err);
                    // More than 3 errors not tolerated
                    if (mistakes_count >= 3)
                    {
                        login_idx = 0;
                        login_char = 0;
                        error = ERR_WRONG_ACCESS;
                        current_state = ST_ERROR;

                        //Timer Error
                        OSTmrStart(&timerError, &os_err);
                        alert_type[1] = true;
                        alert_type[0] = false;
                        alert_type[2] = false;
                    }
                }
            }
            // Adding a user
            else
            {
                // First it's needed to check the user isn't already registered
                int existing_user = -1;
                // Travels through the user database in order to check for a match
                for (int uc_i = 0; uc_i <= MAX_USERS; uc_i++)
                {
                    bool match_found = true;
                    for (int u_i = 0; u_i < ID_LEN; u_i++)
                    {
                        if (login_ID[u_i] != users[uc_i][u_i])
                        {
                            match_found = false;
                            break;
                        }
                    }
                    // In case the match is found signals there's indeed a taken ID
                    if (match_found)
                    {
                        existing_user = uc_i;

                    }
                }
                // Available ID
                if (existing_user < 0)
                {
                    // users by floor
                    int floor_cnt = building[adm_floor];
                    // index = users by floor + (max floor users * (floor - 1)) + 1
                    // Example = 2 users on flor 2. adding the 3d should be 7th on index
                    // index = 2 + (4 * (2 - 1)) + 1 =  2 + 5 = 7
                    int user_n = MIN_USER + floor_cnt + (4 * adm_floor)+1;

                    for (int u_i = 0; u_i < ID_LEN; u_i++)
                    {
                        users[user_n][u_i] = login_ID[u_i];
                    }

                    login_pwd = true;
                    login_idx = 0;
                    login_char = 0;
                }
                // Taken ID
                else
                {
                    login_idx = 0;
                    login_char = 0;
                    error = ERR_USER_REGISTERED;
                    current_state = ST_ERROR;

                    //Timer Double
                    OSTmrStart(&timerError, &os_err);
                    alert_type[1] = true;
                    alert_type[0] = false;
                    alert_type[2] = false;
                }
            }

        }
        // Password check
        else
        {
            int length;
            if (short_pwd == true)
            {
                length = MAX_PSW_LEN-1;
            }
            else
            {
                length = MAX_PSW_LEN;
            }
            // Normal password
            if ((!login_add) && (!chg_pass))
            {
                // Checks if the password matches the user
                bool match_found = true;
                for (int u_i = 0; u_i < length; u_i++)
                {
                    if (login_pass[u_i] != passwords[login_user][u_i])
                    {
                        match_found = false;
                        break;
                    }
                }
                // In case the match is found marks to proceed to password
                if (match_found)
                {
                    login_idx = 0;
                    login_char = 0;
                    current_state = ST_UNLOCK;
                    // Reset mistaker counter
                    mistakes_count = 0;

                    // Local flag, admin does not count as a tenant
                    bool admin_logging = false;

                    // Identify which floor is the tenant from
                    int floor_idx;
                    if (login_user == 0)
                    {
                        admin_logging = true;
                    }
                    
                    // Only tenants modify floor_users
                    if (!admin_logging)
                    {
                        // Unlock alert
                        alert_type[0] = true;
                        alert_type[1] = false;
                        alert_type[2] = false;
                    }
                    else
                    {
                        // Prepare to blink yellow for admin
                        alert_type[0] = false;
                        alert_type[1] = false;
                        alert_type[2] = true;
                    }

                    //Timer Unlock
                    OSTmrStart(&timerUnlock, &os_err);
                }
                // In case the user is invalid after all the database is revised, increase the errors
                else
                {
                    login_idx = 0;
                    login_char = 0;
                    mistakes_count++;
                    // More than 3 errors not tolerated
                    if (mistakes_count >= 3)
                    {
                        login_idx = 0;
                        login_char = 0;
                        error = ERR_WRONG_ACCESS;
                        current_state = ST_ERROR;

                        //Timer Error
                        OSTmrStart(&timerError, &os_err);
                        alert_type[1] = true;
                        alert_type[0] = false;
                        alert_type[2] = false;
                    }
                }
            }
            // Adding a password
            else if (login_add)
            {
                int floor_cnt = building[adm_floor];
                int user_n = MIN_USER + floor_cnt + (4 * adm_floor)+1;

                for (int u_i = 0; u_i < length; u_i++)
                {
                    passwords[user_n][u_i] = login_pass[u_i];
                }

                login_idx = 0;
                login_char = 0;

                // Increment registered users count
                building[adm_floor]++;
                user_count++;

                admin_idx = ADM_DEL;
                display_config_t msg = {"success", 7, brightness, SPIN, 0, NO_BLINK, 0};
                display_msg(msg);
                current_state = ST_MSG;
                next_state = ST_ADMIN;
                

                //Timer Message
                OSTmrStart(&timerMessage, &os_err);

            }
            else if (chg_pass)
            {
                for (int u_i = 0; u_i < length; u_i++)
                {
                    passwords[login_user][u_i] = login_pass[u_i];
                }

                login_idx = 0;
                login_char = 0;
                main_idx = MAIN_BRI;
                chg_pass = false;
                display_config_t msg = {"success", 7, brightness, SPIN, 0, NO_BLINK, 0};
                display_msg(msg);
                current_state = ST_MSG;
                next_state = ST_MAIN;

                //Timer Message
                OSTmrStart(&timerMessage, &os_err);
            }

        }
        data_pushed = false;
    }

}

//admin_menu
void admin_menu(void)
{
    // Main admin flow. Switches between adding or deleting users
    if (admin_idx < ADM_SUB_FLR)
    {
        if(admin_idx==ADM_ADD)
        {
            display_config_t msg = {"add ", 4, brightness, NO_SPIN, 0, NO_BLINK, 0};
            display_msg(msg);
        }
        else if(admin_idx==ADM_DEL)
        {
            display_config_t msg = {"del ", 4, brightness, NO_SPIN, 0, NO_BLINK, 0};
            display_msg(msg);
        }

        if (queue[EV_EN_PUSH] == true)
        {
            queue[EV_EN_PUSH]=false;
            switch (admin_idx)
            {
            case ADM_ADD:
                if (user_count <= MAX_USERS)
                {
                    // Set flag for login_menu()
                    login_add = true;

                    login_idx = 0;
                    login_char = 0;
                    login_pwd = false;

                    // Sets the floor index on the first available floor
                    if (building[0] < FLOOR_USERS)
                    {
                        adm_floor = 0;
                    }
                    else if ((building[1] < FLOOR_USERS))
                    {
                        adm_floor = 1;
                    }
                    else if ((building[2] < FLOOR_USERS))
                    {
                        adm_floor = 2;
                    }
                    else
                    {
                        error = ERR_MAX_USERS_ACHIEVED;
                        admin_idx = ADM_DEL;
                        current_state = ST_ERROR;

                        //Timer Error
                        OSTmrStart(&timerError, &os_err);
                        alert_type[1] = true;
                        alert_type[0] = false;
                        alert_type[2] = false;
                    }

                    admin_idx = ADM_SUB_FLR;
                }
                break;

            case ADM_DEL:
                if (user_count > MIN_USER)
                {
                    login_add = false;
                    // Sets the floor index on the first available floor
                    if (building[0] > 0)
                    {
                        adm_floor = 0;
                    }
                    else if ((building[1] > 0))
                    {
                        adm_floor = 1;
                    }
                    else if ((building[2] > 0))
                    {
                        adm_floor = 2;
                    }
                    else
                    {
                        error = ERR_NO_USERS_TO_ERASE;
                        admin_idx = ADM_DEL;
                        current_state = ST_ERROR;

                        //Timer Error
                        OSTmrStart(&timerError, &os_err);
                        alert_type[1] = true;
                        alert_type[0] = false;
                        alert_type[2] = false;
                    }

                    admin_idx = ADM_SUB_FLR;
                }
                else
                {
                    error = ERR_NO_USERS_TO_ERASE;
                    admin_idx = 0;
                    current_state = ST_ERROR;

                    //Timer Error
                    OSTmrStart(&timerError, &os_err);
                    alert_type[1] = true;
                    alert_type[0] = false;
                    alert_type[2] = false;
                }
                break;
            default:
                break;
            }
        }
        // Exit the admin screen
        else if (queue[EV_EN_HOLD] == true)
        {
            queue[EV_EN_HOLD]=false;
            current_state = ST_INIT;
            alert_type[2] = false;
            brightness = OFF;
            event_matrix_toggle();

        }
        // If there's no advance then it switches between the 2 possible options
        else if (queue[EV_EN_RIGHT] == true)
        {
            queue[EV_EN_RIGHT]=false;
            admin_idx++;
            if (admin_idx > ADM_ADD)
            {
                admin_idx = ADM_DEL;
            }
        }
        else if (queue[EV_EN_LEFT] == true)
        {
            queue[EV_EN_LEFT]=false;
            admin_idx--;
            if (admin_idx < 0)
            {
                admin_idx = ADM_ADD;
            }
        }
    }
    else if (admin_idx == ADM_SUB_FLR)
    {
        switch (adm_floor)
        {
            case 0:
                display_config_t msg0 = {"F 1 ", 4, brightness, NO_SPIN, 0, NO_BLINK, 0};
                display_msg(msg0);
                break;
            case 1:
                display_config_t msg1 = {"F 2 ", 4, brightness, NO_SPIN, 0, NO_BLINK, 0};
                display_msg(msg1);
                break;
            case 2:
                display_config_t msg2 = {"F 3 ", 4, brightness, NO_SPIN, 0, NO_BLINK, 0};
                display_msg(msg2);
                break;
            default:
                break;
        }
        // Select which floor you want to interact with
         if (queue[EV_EN_PUSH] == true)
        {
            queue[EV_EN_PUSH]=false;
            if (login_add)
            {
                admin_idx = ADM_SUB_ADD;
            }
            else
            {
                admin_idx = ADM_SUB_DEL;

                // Update delete menu variables BEFORE jumping into it. Crucial to do it here otherwise they'll overwrite on each loop
                // gets how many users are on the floor
                del_floor_cnt = building[adm_floor];
                // user index (for the floor) is (max floor users * (floor - 1)) + 1
                // This gets you the FIRST user on that floor. Then you can rotate between each
                // Example = flor 2. Users on this floor are 5 to 8 included.
                // index = (4 * (2 - 1)) + 1 =  5 (starting index for floor 2)
                del_user_n = (FLOOR_USERS * (adm_floor - 1)) + 1;
            }


        }
        // Back to admin screen
        else if (queue[EV_EN_HOLD] == true)
        {
            queue[EV_EN_HOLD]=false;
            admin_idx = ADM_DEL;
            alert_bri = OFF;
            // Turn lights off
            event_matrix_toggle();
        }
        // If there's no advance then it switches between the 2 possible options
        else if (queue[EV_EN_RIGHT] == true)
        {
            queue[EV_EN_RIGHT]=false;

            // Remember: 0 < adm_floor < 3 (Floor quantity, not maximum user per floor)

            adm_floor++;
            if (adm_floor == 3)
            {
                adm_floor = 0;
            }
            // While adding users, you want to stay away from full floors
            if (login_add)
            {
                if (building[adm_floor]==FLOOR_USERS)
                {
                    adm_floor++;
                    if (adm_floor == 3)
                    {
                        adm_floor = 0;
                    }
                }
                if (building[adm_floor]==FLOOR_USERS)
                {
                    adm_floor++;
                    if (adm_floor == 3)
                    {
                        adm_floor = 0;
                    }
                }
            }
            // While deleting users, avoid empty floors
            else
            {
                if (building[adm_floor]==MIN_USER)
                {
                    adm_floor++;
                    if (adm_floor == 3)
                    {
                        adm_floor = 0;
                    }
                }
                if (building[adm_floor]==MIN_USER)
                {
                    adm_floor++;
                    if (adm_floor == 3)
                    {
                        adm_floor = 0;
                    }
                }
            }

        }
        else if (queue[EV_EN_LEFT] == true)
        {
            queue[EV_EN_LEFT]=false;

            adm_floor--;
            if (adm_floor < 0)
            {
                adm_floor = 2;
            }
            // While adding users, you want to stay away from full floors
            if (login_add)
            {
                if (building[adm_floor]==4)
                {
                    adm_floor--;
                    if (adm_floor < 0)
                    {
                        adm_floor = 2;
                    }
                }
                if (building[adm_floor]==4)
                {
                    adm_floor--;
                    if (adm_floor < 0)
                    {
                        adm_floor = 2;
                    }
                }
            }
             // While deleting users, avoid empty floors
            else
            {
                if (building[adm_floor]==MIN_USER)
                {
                    adm_floor--;
                    if (adm_floor < 0)
                    {
                        adm_floor = 2;
                    }
                }
                if (building[adm_floor]==MIN_USER)
                {
                    adm_floor--;
                    if (adm_floor < 0)
                    {
                        adm_floor = 2;
                    }
                }
            }

        }

    }

    else if (admin_idx == ADM_SUB_ADD)
    {
        login_menu();
    }

    else if (admin_idx == ADM_SUB_DEL)
    {
        delete_menu();
    }

    // Executing commands takes
    if (login_add_success)
    {
        login_add_success = false;
        // Add "successful" msg display function
    }

}

//main_menu
void main_menu(void)
{
    //imprimir:log in, bright, modo admin
    if(main_idx==MAIN_BRI)
    {
        display_config_t msg = {"bri ", 4, brightness, NO_SPIN, 0, NO_BLINK, 0};
        display_msg(msg);
    }
    else if(main_idx==MAIN_PASS)
    {
        display_config_t msg = {"pass", 4, brightness, NO_SPIN, 0, NO_BLINK, 0};
        display_msg(msg);
    }
    else if (main_idx==MAIN_LOG)
    {
        display_config_t msg = {"log ", 4, brightness, NO_SPIN, 0, NO_BLINK, 0};
        display_msg(msg);
    }
    else if (main_idx == MAIN_SUB_BRI)
    {
        char msg_bri[4];
        msg_bri[0] = 'L';
        msg_bri[1] = 'V';
        msg_bri[2] = 'L';
        msg_bri[3] = '0' + MAX_BRIGHTNESS - brightness + 1;
        display_config_t msg = {&msg_bri[0], 4, brightness, NO_SPIN, 0, NO_BLINK, 0};
        display_msg(msg);
    }
    else if (main_idx == MAIN_SUB_PASS)
    {
        login_menu();
    }
    // Advancing takes priority
    if (queue[EV_EN_PUSH] == true)
    {
    	queue[EV_EN_PUSH]=false;
        switch (main_idx)
        {
        case MAIN_BRI:
            main_idx = MAIN_SUB_BRI;
            break;
        case MAIN_PASS:
            main_idx = MAIN_SUB_PASS;

            // Set login variables
            login_idx = 0;
            login_char = 0;
            login_pwd = true;
            chg_pass = true;
            break;
        case MAIN_LOG:



            current_state = ST_MSG;
            next_state = ST_INIT;

            int floor_idx;

            if ((login_user < 5) && (login_user > 0))
            {
                floor_idx = 0;
            }
            else if (login_user < 9)
            {
                floor_idx = 1;
            }
            else
            {
                floor_idx = 2;
            }
            // Log in or out the tenant
            if (active_tenants[login_user] == true) // The tenant was on the building
            {
                // log the tenant out
                active_tenants[login_user] = false;
                // lower the floor count for the matrix
                floor_users [floor_idx]--;
            }       
            else    // The tenant is entering the building
            {
                // log the tenant in
                active_tenants[login_user] = true;
                // increase the floor count for the matrix
                floor_users [floor_idx]++;
            }
            // Flag to prevent a user for being deleted while inside
            tenants [login_user] = !tenants [login_user];

            //Timer Message
            OSTmrStart(&timerMessage, &os_err);



            break;
        default:
            break;
        }

    }
    // If there's no advance then it switches between the 3 possible options
    else if (queue[EV_EN_RIGHT] == true)
    {
    	queue[EV_EN_RIGHT]=false;
        // Moving through exit options
        if (main_idx <= MAIN_LOG)
        {
            main_idx++;
            if (main_idx >= MAIN_SUB_BRI)
            {
                main_idx = MAIN_BRI;
            }
        }
        else if (main_idx == MAIN_SUB_BRI)
        {

            brightness--;
            if (brightness <= 0)
            {
                brightness = MIN_BRIGHTNESS;
            }
        }
    }
    else if (queue[EV_EN_LEFT] == true)
    {
    	queue[EV_EN_LEFT]=false;
        if (main_idx <= MAIN_LOG)
        {
            main_idx--;
            if (main_idx < 0)
            {
                main_idx = MAIN_LOG;
            }
        }
        else if (main_idx == MAIN_SUB_BRI)
        {
            brightness++;
            if (brightness >= MAX_BRIGHTNESS)
            {
                brightness = MAX_BRIGHTNESS;
            }
        }
    }
    else if (queue[EV_EN_HOLD])
    {
        queue[EV_EN_HOLD]=false;
        if (main_idx == MAIN_SUB_BRI)
        {
            main_idx = MAIN_BRI;
        }
        else if (main_idx == MAIN_SUB_PASS)
        {
            main_idx = MAIN_BRI;

            // reset login variables
            chg_pass = false;
            login_idx = 0;
            login_char = 0;
            login_pwd = false;
        }
        // Quittin before doing the log in
        else
        {
            current_state = ST_INIT;
            login_idx = 0;
            login_char = 0;
            login_pwd = false;
        }
    }
}

//delete_menu
void delete_menu(void)
{
    
    // top index for the floor (from ACTIVE users, not floor max users)
    int floor_max = (FLOOR_USERS * (adm_floor - 1)) + 1 + del_floor_cnt;
    // bottom index for the floor
    int floor_min = (FLOOR_USERS * (adm_floor - 1)) + 1;

    if (queue[EV_EN_PUSH] == true)
    {
        queue[EV_EN_PUSH]=false;
        // Makes sure the tenant isn't inside the building befor deleting the user
        if (tenants[del_user_n] == true)
        {
            display_config_t msg = {"Tenant inside", 13, brightness, SPIN, 0, NO_BLINK, 0};
            display_msg(msg);
            current_state = ST_MSG;
            next_state = ST_ADMIN;
            admin_idx = ADM_DEL;

            //Timer Message
            OSTmrStart(&timerMessage, &os_err);
        }
        else
        {
            // Lower the tenant count
            building[adm_floor]--;
            user_count--;
            del_floor_cnt--;
            
            // Clean the user
            for (int i = 0; i < ID_LEN; i++)
            {
                users[del_user_n][i] = '0';
            }
            for (int i = 0; i < MAX_PSW_LEN; i++)
            {
                passwords[del_user_n][i] = '0';
            }
            admin_idx = ADM_DEL;

            for (int idx = 0;  idx < (del_floor_cnt - (del_user_n - floor_min)); idx++)
            {   
                for (int i = 0; i < ID_LEN; i++)
                {
                    users[del_user_n+idx][i] = users[del_user_n+idx+1][i];
                }
                for (int i = 0; i < MAX_PSW_LEN; i++)
                {
                    passwords[del_user_n+idx][i] = passwords[del_user_n+idx+1][i];
                }
            }
            

            // Confirmation message

            display_config_t msg = {"deleted", 7, brightness, SPIN, 0, NO_BLINK, 0};
            display_msg(msg);
            current_state = ST_MSG;
            next_state = ST_ADMIN;

            //Timer Message
            OSTmrStart(&timerMessage, &os_err);
        }

    }
    // Next priority is going back to the main menu
    else if (queue[EV_EN_HOLD] == true)
    {
        queue[EV_EN_HOLD]=false;
        admin_idx = ADM_DEL;
    }
    // Switches between all users, except the first 3 which are admin users and can't be deleted
    else if (queue[EV_EN_RIGHT] == true)
    {
        queue[EV_EN_RIGHT]=false;

        // Can only vary if there are at least 2 registered tenants
        if (del_floor_cnt > 1)
        {
            del_user_n++;
            // compares the new index with the max users registered at the moment
            if (del_user_n  > floor_max)
            {
                // Back to the first index for the floor
                del_user_n = (FLOOR_USERS * (adm_floor - 1)) + 1;
            }
        }

    }
    else if (queue[EV_EN_LEFT] == true)
    {
        queue[EV_EN_LEFT]=false;

        // Can only vary if there are at least 2 registered tenants
        if (del_floor_cnt > 1)
        {
            del_user_n--;
            // makes sure you won't end in other's floor index
            if (del_user_n < floor_min)
            {
                del_user_n = floor_max;
            }
        }

    }
    for(int i = 0; i < ID_LEN; i++)
    {
        msg_ID[i] = users[del_user_n][i];
    }
    //Show user ID
    display_config_t msg = {&msg_ID[0], ID_LEN, brightness, SPIN, 0, NO_BLINK, 0};
    display_msg(msg);


}

//correct_users
void correct_users(int index)
{
    // Move objects on array
    for (int i = index; i < user_count; i++)
    {
        for (int idx = 0; idx < ID_LEN; idx++)
        {
            users[i][idx] = users[i+1][idx];
        }
    }
    for (int i = index; i < user_count; i++)
    {
        for (int idx = 0; idx < MAX_PSW_LEN; idx++)
        {
            passwords[i][idx] = passwords[i+1][idx];
        }
    }
    // Clean last unused user
    for (int idx = 0; idx < ID_LEN; idx++)
    {
        users[user_count][idx] = 0;
    }
    for (int idx = 0; idx < MAX_PSW_LEN; idx++)
    {
        passwords[user_count][idx] = 0;
    }
}

//error_menu
void error_menu(void)
{
    // 30s lock for an error in access
    if (error == ERR_WRONG_ACCESS)
    {
        display_config_t msg = {"incorrect data", 14, brightness, SPIN, 0, NO_BLINK, 0};
        display_msg(msg);
    }
    if (error == ERR_MAX_USERS_ACHIEVED)
    {
        display_config_t msg = {"user limit achieved", 19, brightness, SPIN, 0, NO_BLINK, 0};
        display_msg(msg);
    }
    if (error == ERR_CARD)
    {
        display_config_t msg = {"invalid card", 12, brightness, SPIN, 0, NO_BLINK, 0};
        display_msg(msg);
    }
    if (error == ERR_NO_USERS_TO_ERASE)
    {
        display_config_t msg = {"no users", 8, brightness, SPIN, 0, NO_BLINK, 0};
        display_msg(msg);
    }
    if (error == ERR_USER_REGISTERED)
    {
        display_config_t msg = {"ID in use", 9, brightness, SPIN, 0, NO_BLINK, 0};
        display_msg(msg);
    }
    if (error == ERR_TENANT)
    {
        display_config_t msg = {"Tenant inside", 13, brightness, SPIN, 0, NO_BLINK, 0};
        display_msg(msg);
    }

}

//unlock_menu
void unlock_menu(void)
{
    display_config_t msg = {"buen dia  ", 10, brightness, SPIN, 0, NO_BLINK, 0};
    display_msg(msg);
}

//init_menu
void init_menu(void)
{
    display_config_t msg = {"Log in", 6, brightness, SPIN, 0, NO_BLINK, 0};
    display_msg(msg);

    if (queue[EV_EN_PUSH] == true)
    {
        queue[EV_EN_PUSH]=false;
        current_state = ST_LOGIN;
        login_add = false;
        login_idx = 0;
        login_char = 0;
        login_pwd = false;
        queue[EV_CARD]=false;
    }
}

//********************************************
//	 	Timer Callbacks for Application
//********************************************
//event_double_tap
void event_double_tap(void)
{
    clean_encoder(EV_EN_PUSH);
    double_timer = false;
}

//event_error: Is called once the error display message finishes in order to
//take the flow of the program back where it belongs
void event_error(void)
{
    if ((error == ERR_NO_USERS_TO_ERASE) || (error == ERR_MAX_USERS_ACHIEVED)|| (error == ERR_USER_REGISTERED))
    {
        current_state = ST_ADMIN;
        admin_idx = ADM_DEL;
        login_char = 0;
        login_idx = 0;
    }
    else if (error == ERR_WRONG_ACCESS)
    {
        current_state = ST_LOGIN;
        login_char = 0;
        login_idx = 0;
        mistakes_count = 0;
    }
    else if (error == ERR_CARD)
    {
        current_state = ST_LOGIN;
        login_char = 0;
        login_idx = 0;
    }

    error  = ERR_NO_ERROR;
    alert_type[1] = false;
    alert_bri = OFF;
    // Turn lights off
    event_matrix_toggle();
}

//event_unlock
void event_unlock(void)
{
    // It's the bulding's admin
    if (login_user == 0)
    {
        current_state = ST_ADMIN;
        admin_idx = ADM_DEL;
    }
    else
    {
        current_state = ST_MAIN;
    }
    alert_type[0] = false;
    alert_bri = OFF;
	// Turn lights off
	event_matrix_toggle();
}

//event_msg
void event_msg(void)
{
    current_state = next_state;
}

// depletes all floors from tenants. Clears other related data
void floor_init (void)
{
    // Floor data. This will be used later to modify the matrix
    for (int i=0; i <3; i++)
    {
        floor_users[i] = 0;
    }
    // Tenant data. This will keep track of which tenant logged in
    for (int i=0; i < 14; i++)
    {
        active_tenants[i] = false;
    }
    // Alert data. This will help managing the extra column of the Matrix
    for (int i=0; i < 3; i++)
    {
        alert_type[i] = false;
    }
}

//********************************************
//	 	Timer Callbacks for Matrix
//********************************************

// When started, every two seconds will toggle the matrix
void event_matrix_toggle (void)
{
    setExtra(alert_color, alert_bri);
}
