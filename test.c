#include "includes.h"
#include <time.h>

#define TASK_STK_SIZE 512
#define NUMBER_OF_TASKS 6
#define NUMBER_OF_MESSAGES 1000

OS_STK TaskStk[NUMBER_OF_TASKS][TASK_STK_SIZE];
OS_EVENT* callWashing;
OS_EVENT* callSoaking;
OS_EVENT* callRinsing;
OS_EVENT* callDrying;
OS_EVENT* callUserinput;
OS_EVENT* callTubWashing;

OS_EVENT* messageQueue;

void* messageArray[NUMBER_OF_MESSAGES];

void Task_Washing(void* data);
void Task_Soaking(void* data);
void Task_Rinsing(void* data);
void Task_Drying(void* data);
void Task_TubWashing(void* data);
void Task_UserInput(void* data);

int main(void)
{
	OSInit();

	//create semaphores
	callWashing = OSSemCreate(0);
	callSoaking = OSSemCreate(0);
	callRinsing = OSSemCreate(0);
	callDrying = OSSemCreate(0);
	callUserinput = OSSemCreate(0);
	callTubWashing = OSSemCreate(0);

	//create message queue
	messageQueue = OSQCreate(messageArray, (INT16U)NUMBER_OF_MESSAGES);

	//create tasks
	OSTaskCreate(Task_Washing, (void*)0, &TaskStk[0][TASK_STK_SIZE - 1], 20);
	OSTaskCreate(Task_Soaking, (void*)0, &TaskStk[1][TASK_STK_SIZE - 1], 25);
	OSTaskCreate(Task_Rinsing, (void*)0, &TaskStk[2][TASK_STK_SIZE - 1], 15);
	OSTaskCreate(Task_Drying, (void*)0, &TaskStk[3][TASK_STK_SIZE - 1], 10);
	OSTaskCreate(Task_TubWashing, (void*)0, &TaskStk[4][TASK_STK_SIZE - 1], 5);
	OSTaskCreate(Task_UserInput, (void*)0, &TaskStk[5][TASK_STK_SIZE - 1], 50);

	OSStart();

	return 0;
}

void Task_Washing(void* data)
{
	INT8U err;
	void* message;
	int integerMessage = 0;

	while (1)
	{
		OSSemPend(callWashing, 0, &err);						//wait semaphore signal from user input task either saking task

		printf("getSem\n");

		message = OSQPend(messageQueue, 0, &err);
		integerMessage = (int)message;
		printf("*** Now in Washing, Please Wait ***\n%d seconds required\n", (integerMessage + 1) * 2);
		
		OSTimeDly((integerMessage + 1) * 2);

		OSQPost(messageQueue, message);					//forwarding message to next task
		OSSemPost(callRinsing);	
	}
}
void Task_Soaking(void* data)
{
	INT8U err;
	void* message;
	int integerMessage = 0;

	while (1)
	{
		OSSemPend(callSoaking, 0, &err);

		printf("getSem\n");

		message = OSQPend(messageQueue, 0, &err);
		integerMessage = (int)message;
		printf("*** Now in Soaking, Please Wait ***\n%d seconds required\n", (integerMessage + 1) * 3);

		OSTimeDly((integerMessage + 1) * 3);

		OSQPost(messageQueue, message);					//forwarding message to next task
		OSSemPost(callWashing);
	}
}
void Task_Rinsing(void* data)
{
	INT8U err;
	void* message;
	int integerMessage = 0;

	while (1)
	{
		OSSemPend(callRinsing, 0, &err);

		printf("getSem\n");

		message = OSQPend(messageQueue, 0, &err);
		integerMessage = (int)message;
		printf("*** Now in Rinsing, Please Wait ***\n%d seconds required\n", integerMessage + 1);

		OSTimeDly(integerMessage + 1);

		OSQPost(messageQueue, message);					//forwarding message to next task
		OSSemPost(callDrying);
	}
}
void Task_Drying(void* data)
{
	INT8U err;
	void* message;
	int integerMessage = 0;

	while (1)
	{
		OSSemPend(callDrying, 0, &err);

		printf("getSem\n");

		message = OSQPend(messageQueue, 0, &err);
		integerMessage = (int)message;
		printf("*** Now in Drying, Please Wait ***\n%d seconds required\n", (integerMessage + 1) * 3);

		OSTimeDly((integerMessage + 1) * 3);
			
		OSSemPost(callUserinput);
	}
}
void Task_TubWashing(void* data)
{
	INT8U err;
	void* message;
	int integerMessage = 0;

	while (1)
	{
		OSSemPend(callTubWashing, 0, &err);

		printf("getSem\n");

		message = OSQPend(messageQueue, 0, &err);				//동작 횟수 전달 받아서 많이 동작했으면 시간 증가, 최대 10까지
		integerMessage = (int)message;

		//if (integerMessage == 0)
			//printf("*** ERROR: Dish-washer is clean... TubWashing not needed ***");
		//else
		//{
			if (integerMessage > 5)
				integerMessage = 5;								//now the dish washer is the dertiest condition

			printf("*** Now in TubWashing, Please Wait ***\n%d seconds required\n", integerMessage);
			OSTimeDly(integerMessage);
		//}

		OSSemPost(callUserinput);
	}
}
void Task_UserInput(void* data)
{
	srand(time((unsigned int*)0));										//set seed to use rand() function for generating random amount of dishes							

	int Number_Of_Operations = 0;										
	int amount_of_dishes = 0;											//suppose random amount of dishes 
	int course = 0;														//suppose the user use a random course
	INT8U err;

	while (1)
	{
		course = rand() % 5;
		amount_of_dishes = rand() % 5;

		system("cls");													//clear console
		printf("========== Dish-washer Control Program ==========\n\n\n* %d Times run\n", Number_Of_Operations);
		if (Number_Of_Operations > 5)
			printf("Pollution level MAX... TubWashing Needed...\n");
		
		printf("\n*** Select Course *** \ncourse >> ");
		/*switch (course)
		{
		case 0:
			printf("SOAKING\n%d of Total capacity loaded\n\n", amount_of_dishes * 20 + 20);
			err = OSQPost(messageQueue, (void*)amount_of_dishes);
			while (err != OS_NO_ERR)
				err = OSQPost(messageQueue, (void*)amount_of_dishes);
			OSSemPost(callSoaking);
			break;
		case 1:
			printf("WASHING\n%d of Total capacity loaded\n\n", amount_of_dishes * 20 + 20);
			err = OSQPost(messageQueue, (void*)amount_of_dishes);
			while (err != OS_NO_ERR)
				err = OSQPost(messageQueue, (void*)amount_of_dishes);
			OSSemPost(callWashing);
			break;
		case 2:
			printf("RINSING\n%d of Total capacity loaded\n\n", amount_of_dishes * 20 + 20);
			err = OSQPost(messageQueue, (void*)amount_of_dishes);
			while (err != OS_NO_ERR)
				err = OSQPost(messageQueue, (void*)amount_of_dishes);
			OSSemPost(callRinsing);
			break;
		case 3:
			printf("DRYING\n%d of Total capacity loaded\n\n", amount_of_dishes * 20 + 20);
			err = OSQPost(messageQueue, (void*)amount_of_dishes);
			while (err != OS_NO_ERR)
				err = OSQPost(messageQueue, (void*)amount_of_dishes);
			OSSemPost(callDrying);
			break;
		case 4:
			printf("TUBWASHING\npollution level : %d\n", Number_Of_Operations * 10);
			err = OSQPost(messageQueue, (void*)Number_Of_Operations);
			while (err != OS_NO_ERR)
				err = OSQPost(messageQueue, (void*)Number_Of_Operations);
			OSSemPost(callTubWashing);
			break;
		}*/

		if (course == 0)
		{
			printf("SOAKING\n%d of Total capacity loaded\n\n", amount_of_dishes * 20 + 20);
			err = OSQPost(messageQueue, (void*)amount_of_dishes);
			printf("%d\n", err);
			while (err != OS_NO_ERR)
			{
				printf("re do queing%d\n", err);
				//OSQFlush(messageQueue);
				err = OSQPost(messageQueue, (void*)amount_of_dishes);
			}
			printf("Qinit\n");
			OSSemPost(callSoaking);
		}
		else if (course == 1)
		{
			printf("WASHING\n%d of Total capacity loaded\n\n", amount_of_dishes * 20 + 20);
			err = OSQPost(messageQueue, (void*)amount_of_dishes);
			printf("%d\n", err);
			while (err != OS_NO_ERR)
			{
				printf("re do queing%d\n", err);
				//OSQFlush(messageQueue);
				err = OSQPost(messageQueue, (void*)amount_of_dishes);
			}
			printf("Qinit\n");
			OSSemPost(callWashing);
		}
		else if (course == 2)
		{
			printf("RINSING\n%d of Total capacity loaded\n\n", amount_of_dishes * 20 + 20);
			err = OSQPost(messageQueue, (void*)amount_of_dishes);
			printf("%d\n", err);
			while (err != OS_NO_ERR)
			{
				printf("re do queing%d\n", err);
				//OSQFlush(messageQueue);
				err = OSQPost(messageQueue, (void*)amount_of_dishes);
			}
			printf("Qinit\n");
			OSSemPost(callRinsing);
		}
		else if (course == 3)
		{
			printf("DRYING\n%d of Total capacity loaded\n\n", amount_of_dishes * 20 + 20);
			err = OSQPost(messageQueue, (void*)amount_of_dishes); 
			printf("%d\n", err);
			while (err != OS_NO_ERR)
			{
				printf("re do queing%d\n", err);
				//OSQFlush(messageQueue);
				err = OSQPost(messageQueue, (void*)amount_of_dishes);
			}
			printf("Qinit\n");
			OSSemPost(callDrying);
		}
		else if (course == 4)
		{
			printf("TUBWASHING\npollution level : %d\n", Number_Of_Operations * 20);
			err = OSQPost(messageQueue, (void*)Number_Of_Operations);
			printf("%d\n", err);
			while (err != OS_NO_ERR)
			{
				printf("re do queing %d\n", err);
				//OSQFlush(messageQueue);
				err = OSQPost(messageQueue, (void*)amount_of_dishes);
			}
			printf("Qinit\n");
			OSSemPost(callTubWashing);
		}

		Number_Of_Operations++;
		
		OSSemPend(callUserinput, 0, &err);
		printf("\a\n\n*** JOB DONE!! ***\n");
		OSTimeDly(1);
		//OSQFlush(messageQueue);
	}
}