/****************************************************************************************
* uCos-II Multitasking Application Code
*****************************************************************************************
*
* Program Name : Synchronizing two task using Semaphore
* Description  : This program creates total three uCos-II tasks; out of which
*                main task acts as a parent task and creates two child tasks and a Semaphore
*				 The Semaphore Synchronizes the two tasks.
*
* 
* MicroEmbedded Technologies
* Processor : NXP LPC2148
* Board		: Micro-A748
*
****************************************************************************************/

#include <includes.h>
#include "func.h"

/********** Define Task Priorities ***********/
#define  APP_TASK_START_PRIO                   4
#define  APP_TASK0_PRIO                        5
#define  APP_TASK1_PRIO                        6
#define  APP_TASK2_PRIO                        7



/*--------------- AAPLICATION STACKS ---------*/
static  OS_STK       AppTaskStartStk[APP_TASK_STK_SIZE];
static  OS_STK       AppTask0stk[APP_TASK_STK_SIZE];		/* Create the required number of stacks need for every child task*/
static  OS_STK       AppTask1stk[APP_TASK_STK_SIZE];
static  OS_STK       AppTask2stk[APP_TASK_STK_SIZE];



/*-------------LOCAL FUNCTION PROTOTYPES--------------*/

/*--------------- A PARENT TASK (MAIN TASK) ---------*/
static  void  AppTaskStart (void        *p_arg); 			 /* Main(Parent) Task Function */
static  void  AppTaskCreate(void);				  			 /* Separate Function To Create Child Task(s) */

/*--------------- CHILDERN TRASKS --------------*/
static  void  AppTask0   	 (void        *p_arg);			 
static  void  AppTask1     	 (void        *p_arg);			 
static  void  AppTask2  	 (void        *p_arg);	

OS_EVENT     *Sem;                                /* Semaphore  */

		 
/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none
*
* Return(s)   : none
**********************************************************************************************************/

int  main (void)
{
    BSP_IntDisAll();                          /* Disable all interrupts until we are ready to accept them */
    OSInit();                                 /* Initialize "uC/OS-II, The Real-Time Kernel"              */
	
    OSTaskCreate(AppTaskStart,                               /* Create the starting task i.e. Main Task        */
                    (void *)0,
                    (OS_STK *)&AppTaskStartStk[APP_TASK_STK_SIZE - 1],
                    APP_TASK_START_PRIO);

    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)       */
}


/***************************************************************************************
*                                          AppTaskStart()
*
* Description : The startup task. The uC/OS-II ticker should only be initialize once multitasking starts.
*
* Argument(s) : p_arg       Argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this 
*					statement.
*
*               (2) Interrupts are enabled by uCoss-II once the task starts because 
*					main() has disbled it.
****************************************************************************************/

static  void  AppTaskStart (void *p_arg)
{
    p_arg = p_arg;							/*Just to avoid compiler Warning 			*/

    BSP_Init();                             /* Initialize BSP functions 	*/ 
	InitLCD();								/* Initialize LCD	*/
	kbdInit();								/* Initialize Keyboard	*/
	ADCInit();								/* Initialize ADC	*/
	LEDInit();	  							/* Initialize LED	*/
	UartInit(9600);							/* Initialise the UART*/	

	Sem = OSSemCreate(0);					/* Create a Semaphore */

	AppTaskCreate();                        /* Create application tasks  (child tasks)        */

    while(DEF_TRUE)
	{
	 printf(" \r\nMAIN TASK: Created 2 Tasks and Semaphore. Now going to deep sleep...");
	 printf(" \r\n====================================================================\r\n");
	 OSTimeDlyHMSM(1, 0, 0, 0);
	}
}


/*
*********************************************************************************************************
*                                      AppTaskCreate()
*
* Description :  Create the application tasks.
*
* Argument(s) :  none.
*
* Return(s)   :  none.
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
	/* Create User Tasks */
    OSTaskCreate(AppTask0,											 // Name of Task
                 (void *)0,											 // Pointer to arguments for task execution
                 (OS_STK *)&AppTask0stk[APP_TASK_STK_SIZE - 1],		 // Pointer to top-of-stack of the assigned stack
                  APP_TASK0_PRIO );									 // Task priority

    OSTaskCreate(AppTask1,											 // Name of Task
                 (void *)0,											 // Pointer to arguments for task execution
                 (OS_STK *)&AppTask1stk[APP_TASK_STK_SIZE - 1],		 // Pointer to top-of-stack of the assigned stack
                  APP_TASK1_PRIO );									 // Task priority




}

/*******************************************************************************************
*                                    TASK-0 : AppTask0()
*
* Description : 
*
* Argument(s) : p_arg       Argument passed to 'AppTask0()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : (1) The first line of code is used to prevent a compiler warning 
*					because 'p_arg' is not used.  The compiler should not generate 
*					any code for this statement.
****************************************************************************************/
static  void  AppTask0 (void *p_arg)
{
	unsigned char err;
   	p_arg = p_arg;									   /*Just to avoid compiler Warning 			*/

    while(DEF_TRUE)
    {     
	 /* User Code Here */ 
		do									   /*	Wait till a key is pressed	*/
		{
			printf("TASK0: Press any key on Keypad to release Semaphore\n\r");
			OSTimeDlyHMSM(0,0,2,0);
		}while(!(GetKey()));
		OSSemPost(Sem);								   /*	Signal Semaphore	*/
		OSTimeDlyHMSM(0,0,0,200);
		do
		{
		 	printf("TASK0: Waiting for Semaphore signal\n\r");
			OSSemPend(Sem,200,&err); 
		}while(err != OS_NO_ERR);						   /*	Wait on Semaphore	*/
		printf("TASK0: Semaphore Acquired.... TASK0 will now perform operations....\n\r");
		OSTimeDlyHMSM(0,0,1,0);
    }
}


/*******************************************************************************************
*                                    TASK-1 : AppTask1()
*
* Description : 
*
* Argument(s) : p_arg       Argument passed to 'AppTask1()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : (1) The first line of code is used to prevent a compiler warning 
*					because 'p_arg' is not used.  The compiler should not generate 
*					any code for this statement.
****************************************************************************************/

static  void  AppTask1 (void *p_arg)
{
	unsigned char err;
   	p_arg = p_arg;											 /* Just to avoid compiler Warning 			*/

   	while(DEF_TRUE)
   	{
	 /* User Code Here */
	 	do
		{
		 	printf("TASK1: Waiting for Semaphore signal\n\r");
			OSSemPend(Sem,200,&err);								 /*	Wait on Semaphore	*/
		}while(err != OS_NO_ERR);
		printf("TASK1: Semaphore Acquired.... TASK1 will now perform operations....\n\r");
		OSTimeDlyHMSM(0,0,1,0);								 
		do												 /*	Wait till a key pressed	*/
		{
			OSTimeDlyHMSM(0,0,2,0);
			printf("TASK1: Press any key on Keypad to release Semaphore\n\r");
		}while(!(GetKey()));
		OSSemPost(Sem);										 /*	Signal Semaphore	*/
		OSTimeDlyHMSM(0,0,0,200);

   	}
}



/*******************************************************************************************
*                                    TASK-2 : AppTask2()
*
* Description : 
*
* Argument(s) : p_arg       Argument passed to 'AppTask2()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : (1) The first line of code is used to prevent a compiler warning 
*					because 'p_arg' is not used.  The compiler should not generate 
*					any code for this statement.
****************************************************************************************/

static  void  AppTask2 (void *p_arg)
{
   p_arg = p_arg;										/* Just to avoid compiler Warning 		*/

    while(DEF_TRUE)
    {
	 /* User Code Here */


   }
}








/*
*********************************************************************************************************
*********************************************************************************************************
**                                         uC/OS-II APP HOOKS
**										(PLEASE IGONRE THE CODE BELOW)
*********************************************************************************************************
*********************************************************************************************************
*/

#if (OS_APP_HOOKS_EN > 0)
/*
*********************************************************************************************************
*                                      TASK CREATION HOOK (APPLICATION)
*
* Description : This function is called when a task is created.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being created.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskCreateHook (OS_TCB *ptcb)
{
#if (OS_VIEW_MODULE > 0)
    OSView_TaskCreateHook(ptcb);
#endif
}

/*
*********************************************************************************************************
*                                    TASK DELETION HOOK (APPLICATION)
*
* Description : This function is called when a task is deleted.
*
* Argument(s) : ptcb   is a pointer to the task control block of the task being deleted.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*********************************************************************************************************
*/

void  App_TaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}

/*
*********************************************************************************************************
*                                      IDLE TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskIdleHook(), which is called by the idle task.  This hook
*               has been added to allow you to do such things as STOP the CPU to conserve power.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are enabled during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{
}
#endif

/*
*********************************************************************************************************
*                                        STATISTIC TASK HOOK (APPLICATION)
*
* Description : This function is called by OSTaskStatHook(), which is called every second by uC/OS-II's
*               statistics task.  This allows your application to add functionality to the statistics task.
*
* Argument(s) : none.
*********************************************************************************************************
*/

void  App_TaskStatHook (void)
{
}

/*
*********************************************************************************************************
*                                        TASK SWITCH HOOK (APPLICATION)
*
* Description : This function is called when a task switch is performed.  This allows you to perform other
*               operations during a context switch.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts are disabled during this call.
*
*               (2) It is assumed that the global pointer 'OSTCBHighRdy' points to the TCB of the task that
*                   will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCur' points to the
*                  task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{
#if (OS_VIEW_MODULE > 0)
    OSView_TaskSwHook();
#endif
}
#endif

/*
*********************************************************************************************************
*                                     OS_TCBInit() HOOK (APPLICATION)
*
* Description : This function is called by OSTCBInitHook(), which is called by OS_TCBInit() after setting
*               up most of the TCB.
*
* Argument(s) : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif

/*
*********************************************************************************************************
*                                        TICK HOOK (APPLICATION)
*
* Description : This function is called every tick.
*
* Argument(s) : none.
*
* Note(s)     : (1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/

#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
#if (OS_VIEW_MODULE > 0)
    OSView_TickHook();
#endif
}
#endif
#endif
