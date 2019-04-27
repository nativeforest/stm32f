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

 void init_app_adc(void)
{
	HAL_ADC_Start_IT(&hadc1);
	memset( adc_values, 0, sizeof (adc_values));
	adc_sample_cnt = 0;
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
		}
		else{
			HAL_ADC_Stop_IT(hadc);
		}
	}
}
