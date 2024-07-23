#include "drivers.h"

EventGroupHandle_t key_event_group =NULL;

TimerHandle_t TIMER;

uint8_t key_flag = 0;


BaseType_t xHigherPriorityTaskWoken = pdFALSE;

void timer_callback(void *arg)
{
    Serial_SendString("timer_callback\r\n"); 
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0)
    {
        OLED_ShowString(75,30,"       ",OLED_8X16);
        if(key_flag == 0)
        {
            Serial_SendString("SetBits\r\n"); 
            OLED_ShowString(75, 25, "Open", OLED_8X16);
            xEventGroupSetBits(key_event_group,0x01 << 1);
            key_flag = 1;
        }
        else
        {
            Serial_SendString("ClearBits\r\n"); 
            OLED_ShowString(75, 25, "Close", OLED_8X16);
            key_flag = 0;
            xEventGroupClearBits(key_event_group,0x01 << 1);
        }
        OLED_Update();
	}
}

void Exit_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource11);
	// GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);

    // Serial_SendString("timer\r\n"); 

    TIMER = xTimerCreate("Timer2", 
                pdMS_TO_TICKS(20),    
                pdFALSE,            //周期执行
                (void *)1,                  //定时器ID
                timer_callback);    //定时器回调函数

    key_event_group = xEventGroupCreate();	
    // xEventGroupSetBits(key_event_group,0x01 << 1);
    // Serial_SendString("timer2\r\n"); 
}

void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line11) == SET)
	{
        Serial_SendString("Exit\r\n"); 
        
        xTimerStartFromISR(TIMER, &xHigherPriorityTaskWoken);
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        
		EXTI_ClearITPendingBit(EXTI_Line11);
	}
}
