#ifndef __DRIVERS_H
#define __DRIVERS_H

#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Serial.h"
#include <stdarg.h>
#include <stdio.h>
#include "LED.h"
#include "OLED.h"
#include "Key.h"
#include "ADC.h"
#include "OLED_show.h"
#include "Motor.h"
#include "Sensor.h"
#include "beep.h"
#include "EXIT.h"

#include "FreeRTOS.h"					///FreeRTOS使用	 
#include "task.h" 
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"

extern SemaphoreHandle_t ADC_Data_semaphore;
extern EventGroupHandle_t open_device_event_group;
extern EventGroupHandle_t disable_device_event_group;
extern EventGroupHandle_t key_event_group;

#endif

