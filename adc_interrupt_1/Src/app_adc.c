/*!
   \file app_adc.c
   \brief Contains all the adc related routines
   \author Mauricio R
   \date 27/04/2019
*/
#include "app_adc.h"
#include "stm32f0xx_hal.h"
#include <string.h>
/* public variables-----------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

/* Private macros-------------------------------------------------------------*/
/*using MAX_ADC_SAMPLES macro we can :
	-avoid magic numbers
	-improve readability
	-avoid bugs since the sizeof the array is used to stop the samples
	-fast modification, lets say you want only 100 samples then you just
	modify the macro
*/
#define MAX_ADC_SAMPLES 500
/* Private variables ---------------------------------------------------------*/
/*ADC is usually 12bits, so no need to use 32bits uint32_t --> uint16_t*/
static uint16_t adc_values[MAX_ADC_SAMPLES];  /* variables names are lowecase */
static uint16_t adc_sample_cnt ;             /* recommended to add proper naming*/

/*!
   \brief Initialize adc app resources
   \param none
   \pre none
   \post adc variables are cleared isr enabled
   \return none
*/

 void adc1_init(void)
{
	HAL_ADCEx_Calibration_Start(&hadc);

	memset( adc_values, 0, sizeof (adc_values));
	adc_sample_cnt = 0;
	/*
	interrupst shall be the last  thing to enabled, in this case
	we are using only a single module, so there is not problem

	but if we have more initialization we would trigger an interrupt
	before reaching the next function e.g

	NOT RECOMMENDED:
	PROBLEM DUE TO INTERRUPT TRIGGERING BEFORE SYSTEM CONFIGURATION
	Here are enabling the ISR in adc1 and timer init. before reaching
	the while loop

	adc1_init() --> would trigger an interrupt
	timer_init() --> would trigger an interrup
	uart_init() -->

	while(1)
	{
	......
	}
	RECOMMENDED: removing the ISR enable from the init and enabling rigth
	before the infinite while loop

	adc1_init() --> would NOT trigger  interrupt
	timer_init() --> would NOT trigger  interrup
	uart_init()

	also take into account ISR enabling order :)
	adc1_enable_isr()
	timer_enable_isr()
	uart_enable_isr()
	while(1)
	{
	......
	}

	*/
	HAL_ADC_Start_IT(&hadc1);
}

/*!
   \brif stops adc isr and autoreload counter
   \param none
   \pre none
   \post adc isr is stopped
   \return
*/
void stop_app_adc(void)
{
#ifdef INFINITE_ADC_SAMPLING
	adc_sample_cnt = 0;
#endif
	HAL_ADC_Stop_IT(&hadc1);
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	/*
		Check that your adc insterrupt is triggered by the adc that you
		selected, this mcu has only one adc, but others have several
		ADC1, ADC2...etc, that's why  hadc->Instance is used here

		The hals recieves a pointer to the handle, so here you dont
		need to use &hadc, because hadc is already a pointer

		but if you use hadc1, you need to use &hadc1 because hadc1 is
		nota pointer

		ADC_HandleTypeDef* hadc  pointer
		ADC_HandleTypeDef hadc1;  non a pointer :)


	*/
	if (hadc->Instance == ADC1) {
		if ( adc_sample_cnt < MAX_ADC_SAMPLES ) {
			adc_values[adc_sample_cnt] = HAL_ADC_GetValue(hadc);
			adc_sample_cnt++;
			HAL_ADC_Start_IT(&hadc1);
			/*
			Never forget about flag clearing, in this case (ADC) is
			not necesary to clear the flag, because it's cleared
			automatically by reading the ADC register
			so , calling HAL_ADC_GetValue CLEAR the ADC_FLAG_EOC bit
			*/
//			__HAL_ADC_CLEAR_FLAG(&hadc1, (ADC_FLAG_EOC | ADC_FLAG_EOS | ADC_FLAG_OVR));

		}
		else{
			/*In the last sample we clear the flag directly
			because we discard the 500+1rst  convertion
			*/
			__HAL_ADC_CLEAR_FLAG(&hadc1, (ADC_FLAG_EOC | ADC_FLAG_EOS | ADC_FLAG_OVR));
			HAL_ADC_Stop_IT(hadc);
		}


	}
}
