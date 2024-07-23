#include "drivers.h"

static uint16_t sensor_data[3] = {0, 0, 0};
void sensor_show_task(void *arg)
{
    OLED_ShowString(80, 0, "KEY", OLED_8X16);
    OLED_ShowString(0, 0, "L:", OLED_8X16);
    Serial_SendString("oled\r\n"); 
    OLED_ShowString(0, 25, "T:", OLED_8X16);
    OLED_ShowString(0, 50, "H:", OLED_8X16);

    OLED_DrawLine(0, 21, 60, 21);
    OLED_DrawLine(0, 45, 60, 45);

    OLED_DrawLine(60, 0, 60, 63);
    OLED_DrawLine(60, 63, 127, 63);
    
    while(1)
    {
        xSemaphoreTake(ADC_Data_semaphore, (TickType_t)portMAX_DELAY);        //进入基本临界区
        // taskENTER_CRITICAL();        //进入基本临界区
        sensor_data[0] = AD_Value[0];
        sensor_data[1] = AD_Value[1];
        sensor_data[2] = AD_Value[2];
        xSemaphoreGive(ADC_Data_semaphore);        //退出基本临界区
        // taskEXIT_CRITICAL();        //退出基本临界区

        OLED_ShowNum(20, 0, sensor_data[0], 4, OLED_8X16);
        OLED_ShowNum(20, 25, sensor_data[1], 4, OLED_8X16);
        OLED_ShowNum(20, 50, sensor_data[2], 4, OLED_8X16);
        vTaskDelay(pdMS_TO_TICKS(20));
        OLED_Update();

        // Serial_SendString("oled22\r\n"); 

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void OLED_show_init(void)
{
    OLED_Init();
    xTaskCreate(sensor_show_task, "sensor_show_task", 128, NULL, 10, NULL);
}
