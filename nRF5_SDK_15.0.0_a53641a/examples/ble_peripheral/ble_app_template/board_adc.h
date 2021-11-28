#ifndef _BOARD_ADC_H_
#define _BOARD_ADC_H_

/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stdbool.h>
#include "nrfx_saadc.h"
#include "nrf_drv_saadc.h"

#define uint16 uint16_t
#define uint8  uint8_t
#define uint32 uint32_t

/*********************************************************************
 * DEFINITIONS
 */
#define ADC_REF_VOLTAGE_IN_MILLIVOLTS   600					/**< Reference voltage (in milli volts) used by ADC while doing conversion. */
#define ADC_PRE_SCALING_COMPENSATION    6					/**< The ADC is configured to use VDD with 1/3 prescaling as input. And hence the result of conversion is to be multiplied by 3 to get the actual value of the battery voltage.*/
#define DIODE_FWD_VOLT_DROP_MILLIVOLTS  1000					/**< Typical forward voltage drop of the diode . */
#define ADC_RES_10BIT                   1024				/**< Maximum digital value for 10-bit ADC conversion. */

// VP = (RESULT * REFERENCE / 2^10) * 6
#define ADC_RESULT_IN_MILLI_VOLTS(ADC_VALUE)\
        ((((ADC_VALUE) * ADC_REF_VOLTAGE_IN_MILLIVOLTS) / ADC_RES_10BIT) * ADC_PRE_SCALING_COMPENSATION) 
 
#define SAMPLES_IN_BUFFER				2

extern nrf_saadc_value_t Battery_data;
extern nrf_saadc_value_t temperature_data;

/*********************************************************************
 * API FUNCTIONS
 */
void ADC_Read(void);
uint32_t ADC_Init(void);
void ADC_Disable(void);

uint32_t ADC_BlockRead(uint8_t channel, int16_t *value);
#endif /* _BOARD_ADC_H_ */
