#include "drivers.h"

/*
* AD_Value[0]: 光照数据
* AD_Value[1]:  温度数据
* AD_Value[2]: 湿度数据
*/

/*事件位表示*/
/*
* 5 6 电机位
* 0 1 LED位
* 2 3 4 峰鸣器位
*/
EventGroupHandle_t open_device_event_group =NULL;
EventGroupHandle_t disable_device_event_group =NULL;


void Sensor_Task(void *arg)
{
    uint16_t light;
    uint16_t temperature;
    uint16_t humidity;


    while(1)
    {
        // Serial_SendString("Sensor_Task\r\n");
        xSemaphoreTake(ADC_Data_semaphore, (TickType_t)portMAX_DELAY);        //进入基本临界区
        light = AD_Value[0];
        temperature = AD_Value[1];
        humidity = AD_Value[2];
        xSemaphoreGive(ADC_Data_semaphore);        //退出基本临界区
        
        /*传感器阈值处理*/
        if(light > 3000) {xEventGroupSetBits(open_device_event_group,0x01 << 1);xEventGroupSetBits(open_device_event_group,0x01 << 2);}
        else             {xEventGroupSetBits(disable_device_event_group,0x01 << 1);xEventGroupSetBits(disable_device_event_group,0x01 << 2);}   

        if(temperature > 1950) {xEventGroupSetBits(open_device_event_group,0x01 << 5);xEventGroupSetBits(open_device_event_group,0x01 << 3); }
        else                  {xEventGroupSetBits(disable_device_event_group,0x01 << 5);xEventGroupSetBits(disable_device_event_group,0x01 << 3);}   

        if(humidity > 3000) {xEventGroupSetBits(open_device_event_group,0x01 << 6);xEventGroupSetBits(open_device_event_group,0x01 << 4);}    
        else              {xEventGroupSetBits(disable_device_event_group,0x01 << 6);xEventGroupSetBits(disable_device_event_group,0x01 << 4);}   
        vTaskDelay(pdMS_TO_TICKS(3000));
    }

}

void Sensor_Init(void)
{
    open_device_event_group = xEventGroupCreate();	
    disable_device_event_group = xEventGroupCreate();	

    xTaskCreate(Sensor_Task, "Sensor_Task", 256, NULL, 10, NULL);
}
