/****************************************************************************************
* uCos-II Multitasking Application Code
*****************************************************************************************
*
* Program Name : Memory Management in uC/OS-II
* Description  : This program creates total three uCos-II tasks; out of which
*                main task acts as a parent task and create two child tasks.
*				 Task0 allocates a memory block from the memory partition, accepts some data
*				 from the user, then posts a pointer to the memory block through mailbox.
*				 Task1 waits for a message through mailbox, and on recieving the memory block
*				 pointer, displays the content of the memory block on the terminal.
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


OS_EVENT *TxMbox;			  	/*	Mailbox	*/
OS_MEM *PartitionPtr;			/*	Pointer to partition	*/
INT8U Partition[100][32];		/*	Allocate a partition	*/

	 
/**********************************************************************************************************
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
	unsigned char err;
    p_arg = p_arg;							/*Just to avoid compiler Warning 			*/

    BSP_Init();                             /* Initialize BSP functions 	*/ 
	InitLCD();								/* Initialize LCD	*/
	kbdInit();								/* Initialize Keyboard	*/
	ADCInit();								/* Initialize ADC	*/
	LEDInit();	  							/* Initialize LED	*/
	UartInit(9600);							/* Initialise the UART*/			

	TxMbox = OSMboxCreate((void*)0);		/*	Create a mailbox	*/
	PartitionPtr = OSMemCreate(Partition,100,32, &err);	   	/*	Create a partition	*/

	AppTaskCreate();                        /* Create application tasks  (child tasks)        */

    while(DEF_TRUE)
	{
	 printf(" \r\nMAIN TASK: Created 3 Tasks. Now going to deep sleep...");
	 printf(" \r\n======================================================");
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
	INT8U *pblock;
	unsigned char temp, err;
	unsigned int i;
   p_arg = p_arg;									   /*Just to avoid compiler Warning 			*/

    while(DEF_TRUE)
    {     
	 /* User Code Here */
	 	printf("\r\nTASK0: Obtaining a memory block from the partition\r\n");
		pblock = OSMemGet(PartitionPtr, &err);				  /*	get a memory block from the partition	*/
		if(err != OS_NO_ERR)
		{
			printf("TASK0: Error obtaining memory block\r\n");
			OSTimeDlyHMSM(0,0,5,0);
		} 
		else
		{
	 		printf("TASK0: Enter 10 digits from keypad to be stored in the memory block: ");
			for(i=0;i<10;i++)						   	/*	accept 10 digits from the user	*/
			{
				while(!(temp = GetKey()));				/*	wait till you get a valid key value from the keypad	*/
				UART_PutChar(temp);						/*	echo the key to the terminal	*/
				pblock[i] = temp;						/*	store the key in the memory block	*/
				OSTimeDlyHMSM(0,0,0,200);
			}
			printf("\r\nTask0: Posting the memory block to the MailBox\r\n");
			OSMboxPost(TxMbox,(void*)pblock);			 /*	post the pointer to the memory block through the mailbox	*/
			OSTimeDlyHMSM(0,0,1,0);
			printf("Task0: Returning the memory block to the partition\r\n");
			OSMemPut(PartitionPtr, pblock);				 /*	return the memory block to the partition	*/
			OSTimeDlyHMSM(0,0,5,0);
		} 	
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
	INT8U *msg;
	unsigned char err;
	unsigned int i;
   p_arg = p_arg;											 /* Just to avoid compiler Warning 			*/

   while(DEF_TRUE)
   {
	 /* User Code Here */
	   msg = (INT8U*)OSMboxPend(TxMbox,0,&err);				 /*	wait till a message is recieved in the mailbox	*/
	   printf("Task1: Memory block recieved: ");
	   for(i=0;i<10;i++)									 /*	display the contents of the memory block	*/
	   {
	   	  	printf("%c",msg[i]);
	   }
	   printf("\r\n");
	   OSTimeDlyHMSM(0,0,2,0);
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

