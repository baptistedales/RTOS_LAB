/****************************************************************************
 * Copyright (C) 2020 by Fabrice Muller                                     *
 *                                                                          *
 * This file is useful for ESP32 Design course.                             *
 *                                                                          *
 ****************************************************************************/

/**
 * @file lab3-1_main.c
 * @author Fabrice Muller
 * @date 20 Oct. 2020
 * @brief File containing the lab3-1 of Part 3.
 *
 * @see https://github.com/fmuller-pns/esp32-vscode-project-template
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include "esp_log.h"

/* FreeRTOS.org includes. */
#include "freertos/FreeRTOSConfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "my_helper_fct.h"

static const char* TAG = "SEM";

/* Application constants */
#define STACK_SIZE     4096
#define TABLE_SIZE     400

/* Task Priority */
const uint32_t TIMER_TASK_PRIORITY = 5;
const uint32_t INC_TABLE_TASK_PRIORITY = 3;
const uint32_t DEC_TABLE_TASK_PRIORITY = 4;

/* Communications */
SemaphoreHandle_t xSemClk_Inc;
SemaphoreHandle_t xSemClk_Dec;


/* Tasks */
void vTaskTimer(void *pvParameters);
void vTaskIncTable(void *pvParameters);
void vTaskDecTable(void *pvParameters);

/* Datas */
int Table[TABLE_SIZE];

/*-----------------------------------------------------------*/


void vTaskTimer(void *pvParameters) {
	while(1){
		vTaskDelay(pdMS_TO_TICKS(250));
		COMPUTE_IN_TIME_MS(20);
		DISPLAY("Task timer : Give Sem");
		/*Using Semaphore */
		xSemaphoreGive(xSemClk_Inc);
		xSemaphoreGive(xSemClk_Dec);
		
	}
}

void vTaskIncTable(void *pvParameters) {
	uint32_t activationNumber =0;
	while (1){
		if(xSemaphoreTake(xSemClk_Inc, portMAX_DELAY)){
			DISPLAY("IncTable");
			if (activationNumber==0){
				for (int i=0; i<TABLE_SIZE-1; i++){
					Table[i]+=5;
				}
				COMPUTE_IN_TIME_MS(50);
				activationNumber=4;
			}

			else activationNumber-=1;
		}
	}
}

void vTaskDecTable(void *pvParameters) {
	while (1){
		if(xSemaphoreTake(xSemClk_Dec, portMAX_DELAY)){
			DISPLAY("DecTable");
			for (int i=0; i<TABLE_SIZE-1; i++){
				Table[i]-=1;
			}
			COMPUTE_IN_TIME_MS(50);

		}
	}
}



/* Main function */
void app_main(void) {

	/* Init Table */
	memset(Table, 0, TABLE_SIZE*sizeof(int));

	/* Create Binary semaphore */
	xSemClk_Inc = xSemaphoreCreateBinary();
	xSemClk_Dec = xSemaphoreCreateBinary();


	/* Stop scheduler */
	vTaskSuspendAll();

	/* Create Tasks */
	xTaskCreatePinnedToCore(vTaskTimer,	"vTaskTimer", STACK_SIZE,	(void*)"vTaskTimer", TIMER_TASK_PRIORITY,	NULL,CORE_0);					
	xTaskCreatePinnedToCore(vTaskIncTable,	"vTaskIncTable", STACK_SIZE,	(void*)"vTaskIncTable", INC_TABLE_TASK_PRIORITY,	NULL,CORE_0);					
	xTaskCreatePinnedToCore(vTaskDecTable,	"vTaskDecTable", STACK_SIZE,	(void*)"vTaskDecTable", DEC_TABLE_TASK_PRIORITY,	NULL,CORE_1);					


	/* Continue scheduler */
	xTaskResumeAll();

	/* to ensure its exit is clean */
	vTaskDelete(NULL);
}
