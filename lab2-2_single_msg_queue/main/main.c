#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "esp_log.h"

/* FreeRTOS.org includes. */
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "my_helper_fct.h"

static const uint32_t T1_PRIO = 2;
static const uint32_t T2_PRIO = 2;
static const uint32_t T3_PRIO = 3;

/* Default stack size for tasks */
static const uint32_t STACK_SIZE = 4000;

void vTaskFunction_1(void *pvParameters);
void vTaskFunction_2(void *pvParameters);
void vTaskFunction_3(void *pvParameters);

QueueHandle_t xQueue1 = NULL;

uint32_t msg_to_send = 0;
/**
 * @brief Starting point function
 * 
 */

void app_main(void) {

	xQueue1 = xQueueCreate(5,sizeof(uint32_t));

	xTaskCreatePinnedToCore(vTaskFunction_1,	"Task 1", STACK_SIZE,	(void*)"Task 1", T1_PRIO,	NULL,CORE_1);
	xTaskCreatePinnedToCore(vTaskFunction_2,	"Task 2", STACK_SIZE,	(void*)"Task 2", T2_PRIO,	NULL,CORE_0);
	xTaskCreatePinnedToCore(vTaskFunction_3,	"Task 3", STACK_SIZE,	(void*)"Task 3", T3_PRIO,	NULL,CORE_0);

	vTaskDelete(NULL);
}

void vTaskFunction_1(void *pvParameters){
	DISPLAY("Debut Task 1");
	TickType_t xLastWakeTime = xTaskGetTickCount();

	for(;;){
		if(xQueue1 != 0){
			int result = xQueueSend(xQueue1,(void *) &msg_to_send,0);
			if(result != pdPASS){
				DISPLAY(" error to send value to queue");
			}
			else{
				DISPLAY("Value send");
			}
		}
		else
		{
			DISPLAY("QUEUE not created");
		}	
		//Compute time : 40 ms
		COMPUTE_IN_TIME_MS(40);
		//Block periodically : 500ms
		vTaskDelayUntil(&xLastWakeTime,pdMS_TO_TICKS(500));
		DISPLAY("fin task 1");
	}
	vTaskDelete(NULL);
}

void vTaskFunction_2(void *pvParameters){
	DISPLAY("Debut Task2");
	uint32_t *receivedMessage = NULL;
	
	for(;;){
		if(xQueueReceive(xQueue1, &receivedMessage,300)== pdPASS){
			DISPLAYI(LOG_COLOR_GREEN,"Task 2 : Received %d from the queue1",(int) receivedMessage);
		}
		else
		{
			DISPLAYE(LOG_COLOR_RED,"Task 2 : Problem reception");
		}
		COMPUTE_IN_TIME_MS(30);
		DISPLAY("Fin TASK2");
	}
	vTaskDelete(NULL);
}

void vTaskFunction_3(void *pvParameters){
	for(;;){
		// Task blocked during 100ms
		DISPLAY("debut task3");
		vTaskDelay(pdMS_TO_TICKS(100));
		DISPLAY("Task 3 start computing");
		// Compute time : 20 ms 
		COMPUTE_IN_TIME_MS(20);
	}
	vTaskDelete(NULL);	
}
