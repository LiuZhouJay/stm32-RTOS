#include "drivers.h"

SemaphoreHandle_t beep_alarm_semaphore;
void beep_on(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}

void beep_off(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_8);
}

void beep_turn(void)
{
    if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_8) == 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_8);
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_8);
	}
}

void beep_alarm(void)
{
    beep_on();
    xSemaphoreGive(beep_alarm_semaphore);
}

void beep_alarm_task(void *arg)
{
    static uint8_t i = 0;
    while(1)
    {
        // Serial_SendString("alarms\r\n");
        xSemaphoreTake(beep_alarm_semaphore, (TickType_t)portMAX_DELAY);
        while(i < 2)
        {
            beep_on();
            vTaskDelay(pdMS_TO_TICKS(500));
            beep_off();
            vTaskDelay(pdMS_TO_TICKS(500));
            i++;
        }
        i = 0;
    }
}

void beep_open_task(void *pvParameters)
{
    while (1)
    {
        // Serial_SendString("beep_open_task\r\n"); 
        xEventGroupWaitBits(key_event_group,  /* 事件对象句柄 */
                                  (0x01 << 1),/* 接收线程感兴趣的事件 */
                                  pdFALSE,   /* 退出时不清除事件位 */
                                  pdTRUE,   /* 满足感兴趣的其中一个事件 */
                                  (TickType_t)portMAX_DELAY);/* 指定超时事件,一直等 */

        xEventGroupWaitBits(open_device_event_group,  /* 事件对象句柄 */
                                  (0x01 << 2) | (0x01 << 3) | (0x01 << 4),/* 接收线程感兴趣的事件 */
                                  pdTRUE,   /* 退出时清除事件位 */
                                  pdFALSE,   /* 满足感兴趣的其中一个事件 */
                                  (TickType_t)portMAX_DELAY);/* 指定超时事件,一直等 */
        beep_alarm();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void beep_disable_task(void *pvParameters)
{
    while (1)
    {
        // Serial_SendString("beep_disable\r\n");
        xEventGroupWaitBits(disable_device_event_group,  /* 事件对象句柄 */
                                  (0x01 << 2) | (0x01 << 3) | (0x01 << 4),/* 接收线程感兴趣的事件 */
                                  pdTRUE,   /* 退出时清除事件位 */
                                  pdTRUE,   /* 满足感兴趣的所有事件 */
                                  (TickType_t)portMAX_DELAY);/* 指定超时事件,一直等 */
        beep_off();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void beep_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_8);

    beep_alarm_semaphore = xSemaphoreCreateBinary();

    xTaskCreate(beep_open_task, "beep_open_task", 128, NULL, 10, NULL);
    xTaskCreate(beep_disable_task, "beep_disable_task", 128, NULL, 10, NULL);
    xTaskCreate(beep_alarm_task, "beep_alarm_task", 128, NULL, 10, NULL);

}


