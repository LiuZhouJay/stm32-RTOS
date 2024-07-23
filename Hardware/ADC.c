#include "drivers.h"                // Device header

uint16_t AD_Value[3];
SemaphoreHandle_t ADC_Data_semaphore  = NULL;

void AD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);
		
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_NbrOfChannel = 3;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; //外设地址，adc外设的地址
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //数据宽度为半字 也就是16位数据，因为adc读取返回的数据是十六位
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    //外设地址不递增
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;  //存储地址
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;  //数据宽度为半字 也就是16位数据，因为adc读取返回的数据是十六位
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;      //储存器地址递增
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //转移方向，从外设到寄存器
	DMA_InitStructure.DMA_BufferSize = 3;   //转移次数
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //自动重装载，当转移四次结束后，自动开始
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //硬件触发DMA
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //优先级
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Channel1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    ADC_Data_semaphore = xSemaphoreCreateMutex();
}
