#include "drivers.h"

TimerHandle_t Motor_TIMER;

void Motor_timer_callback(void *arg)
{
    Motor_SetSpeed(0);
}
void PWM_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;		//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;		//CCR
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
}
void Motor_SetSpeed(int8_t Speed)
{
	if (Speed >= 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);
		PWM_SetCompare(Speed);
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		GPIO_SetBits(GPIOA, GPIO_Pin_5);
		PWM_SetCompare(-Speed);
	}
}


void PWM_SetCompare(uint16_t Compare)
{
	TIM_SetCompare4(TIM2, Compare);
}

void Motor_open_Task(void *pvParameters)
{
    while (1)
    {
        // Serial_SendString("Motor_wait_Task\r\n");
        xEventGroupWaitBits(key_event_group,  /* 事件对象句柄 */
                                  (0x01 << 1),/* 接收线程感兴趣的事件 */
                                  pdFALSE,   /* 退出时不清除事件位 */
                                  pdTRUE,   /* 满足感兴趣的其中一个事件 */
                                  (TickType_t)portMAX_DELAY);/* 指定超时事件,一直等 */
        Serial_SendString("Motor_open_Task\r\n"); 
		
        xEventGroupWaitBits(open_device_event_group,  /* 事件对象句柄 */
                                  (0x01 << 5) | (0x01 << 6),/* 接收线程感兴趣的事件 */
                                  pdTRUE,   /* 退出时清除事件位 */
                                  pdFALSE,   /* 满足感兴趣的其中一个事件 */
                                  (TickType_t)portMAX_DELAY);/* 指定超时事件,一直等 */
        Motor_SetSpeed(20);
        xTimerStart(Motor_TIMER,0);
        // Serial_SendString("motor_open\r\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void Motor_disable_Task(void *pvParameters)
{
    while (1)
    {
        xEventGroupWaitBits(disable_device_event_group,  /* 事件对象句柄 */
                                  (0x01 << 5) | (0x01 << 6),/* 接收线程感兴趣的事件 */
                                  pdTRUE,   /* 退出时清除事件位 */
                                  pdTRUE,   /* 满足感兴趣的所有事件 */
                                  (TickType_t)portMAX_DELAY);/* 指定超时事件,一直等 */
        Motor_SetSpeed(0);
        // Serial_SendString("motor_disable\r\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	PWM_Init();

    Motor_TIMER = xTimerCreate("Motor_TIMER", 
                pdMS_TO_TICKS(3000),    
                pdFALSE,            //单次执行
                (void *)1,                  //定时器ID
                Motor_timer_callback);    //定时器回调函数

    xTaskCreate(Motor_open_Task, "Motor_open_Task", 128, NULL, 10, NULL);
    xTaskCreate(Motor_disable_Task, "Motor_disable_Task", 128, NULL, 10, NULL);
}


