#include "drivers.h"

int main(void)
{
	Serial_Init();
    AD_Init();
    OLED_show_init();
    Sensor_Init();
    Motor_Init();
    LED_Init();
    beep_init();
    Exit_init();

    vTaskStartScheduler();

    return 0;
}
