#include "drivers.h"

#define LED_PIN                     GPIO_Pin_12
#define LED_GPIO                    GPIOB
#define LED_GPIO_CLOCK              RCC_APB2Periph_GPIOB


TimerHandle_t LED_TIMER;


void LED_ON(void)
{
    GPIO_ResetBits(LED_GPIO, LED_PIN);
}
void LED_OFF(void)
{
    GPIO_SetBits(LED_GPIO, LED_PIN);
}

void LED_Turn(void)
{
	if (GPIO_ReadOutputDataBit(LED_GPIO, LED_PIN) == 0)
	{
		GPIO_SetBits(LED_GPIO, LED_PIN);
	}
	else
	{
		GPIO_ResetBits(LED_GPIO, LED_PIN);
	}
}
void LED_timer_callback(void *arg)
{
    LED_OFF();
}


void LED_open_Task(void *arg)
{
    while (1)
    {
        xEventGroupWaitBits(key_event_group,  /* 事件对象句柄 */
                                  (0x01 << 1),/* 接收线程感兴趣的事件 */
                                  pdFALSE,   /* 退出时不清除事件位 */
                                  pdTRUE,   /* 满足感兴趣的其中一个事件 */
                                  (TickType_t)portMAX_DELAY);/* 指定超时事件,一直等 */

        xEventGroupWaitBits(open_device_event_group,  /* 事件对象句柄 */
                                  (0x01 << 1),/* 接收线程感兴趣的事件 */
                                  pdTRUE,   /* 退出时清除事件位 */
                                  pdTRUE,   /* 满足感兴趣的其中一个事件 */
                                  (TickType_t)portMAX_DELAY);/* 指定超时事件,一直等 */
        LED_ON();
        xTimerStart(LED_TIMER,0);
        // Serial_SendString("led_open\r\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
void LED_disable_Task(void *arg)
{
    while (1)
    {
        xEventGroupWaitBits(disable_device_event_group,  /* 事件对象句柄 */
                                  (0x01 << 1),/* 接收线程感兴趣的事件 */
                                  pdTRUE,   /* 退出时清除事件位 */
                                  pdTRUE,   /* 满足感兴趣的其中一个事件 */
                                  (TickType_t)portMAX_DELAY);/* 指定超时事件,一直等 */
        LED_OFF();
        // Serial_SendString("led_disable\r\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void LED_Init(void)
{
    RCC_APB2PeriphClockCmd(LED_GPIO_CLOCK, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = LED_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_GPIO, &GPIO_InitStructure);
	GPIO_SetBits(LED_GPIO, LED_PIN);

    LED_TIMER = xTimerCreate("LED_TIMER", 
                pdMS_TO_TICKS(3000),    
                pdFALSE,            //单次执行
                (void *)1,                  //定时器ID
                LED_timer_callback);    //定时器回调函数

    xTaskCreate(LED_open_Task, "LED_open_Task", 128, NULL, 10, NULL);
    xTaskCreate(LED_disable_Task, "LED_disable_Task", 128, NULL, 10, NULL);
}
