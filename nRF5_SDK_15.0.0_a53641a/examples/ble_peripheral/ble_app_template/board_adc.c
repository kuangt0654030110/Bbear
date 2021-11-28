/*********************************************************************
 * INCLUDES
 */
#include "nrf_drv_saadc.h"
#include "app_error.h" 

#include "board_adc.h"
#include "nrf_log.h"
#include "alm_init_profile.h"

#if NRF_MODULE_ENABLED(NTN_AND_BAT_ADC)

#define SAADC_OVERSAMPLE NRF_SAADC_OVERSAMPLE_DISABLED
#define BAT_ADC   NRF_SAADC_INPUT_AIN0
#define NTN_ADC   NRF_SAADC_INPUT_AIN1

nrf_saadc_value_t Battery_data     = 0;
nrf_saadc_value_t temperature_data = 0;
static void adcCallbackFunc(nrf_drv_saadc_evt_t const *pEvent);

/*********************************************************************
 * LOCAL VARIABLES
 */
static nrf_saadc_value_t m_bufferPool[2][SAMPLES_IN_BUFFER];

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 @brief ADC的初始化函数
 @param 无
 @return 无
*/
uint32_t ADC_Init(void)
{
		ret_code_t err_code;
	
    err_code = nrf_drv_saadc_init(NULL, adcCallbackFunc);
    APP_ERROR_CHECK(err_code);
	
	
	  nrf_saadc_channel_config_t bat_channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(BAT_ADC);
		nrf_saadc_channel_config_t ntn_channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NTN_ADC);
		
    err_code = nrf_drv_saadc_channel_init(0, &bat_channel_config);
    APP_ERROR_CHECK(err_code);

		err_code = nrf_drv_saadc_channel_init(1, &ntn_channel_config);
    APP_ERROR_CHECK(err_code);
	
//    err_code = nrf_drv_saadc_buffer_convert(m_bufferPool[0], SAMPLES_IN_BUFFER);
//    APP_ERROR_CHECK(err_code);
		return 0;
}

/**
 @brief ADC读取
 @param 无
 @return 结果在回调函数的缓冲区中
*/
void ADC_Read(void)
{
		ret_code_t errCode;
		errCode = nrf_drv_saadc_sample();
		APP_ERROR_CHECK(errCode);
}

/**
 @brief 禁用ADC
 @param 无
 @return 无
*/
void ADC_Disable(void)
{
	nrfx_saadc_uninit();
}


/*********************************************************************
 * LOCAL FUNCTIONS
 */
/**
 @brief ADC中断处理回调函数
 @param 无
 @return 无
*/
static void adcCallbackFunc(nrf_drv_saadc_evt_t const *pEvent)
{
		ret_code_t errCode;
	
		if(pEvent->type == NRF_DRV_SAADC_EVT_DONE) // 采样完成
		{
// 				nrf_saadc_value_t BatAdcResult;
//				nrf_saadc_value_t NTNAdcResult;
//					
//				for (int i = 0; i < pEvent->data.done.size; i++) {
//						if(i == 0) {
//							BatAdcResult = pEvent->data.done.p_buffer[i];
//							Battery_data = ADC_RESULT_IN_MILLI_VOLTS(BatAdcResult);
//							AlmInitProfile_SetData(NULL, ALMINITPROFILE_CHAR3, (uint8_t*)&Battery_data, sizeof(Battery_data));
//						}
//						if(i == 1) {
//							NTNAdcResult = pEvent->data.done.p_buffer[i];
//							temperature_data = ADC_RESULT_IN_MILLI_VOLTS(NTNAdcResult);
//							AlmInitProfile_SetData(NULL, ALMINITPROFILE_CHAR4, (uint8_t*)&temperature_data, sizeof(temperature_data));
//						}
// 				}  
				// 设置好缓存，为下次转换缓冲做准备，并且把导入到缓冲的值提取出来
				errCode = nrf_drv_saadc_buffer_convert(m_bufferPool[0], SAMPLES_IN_BUFFER);					
				APP_ERROR_CHECK(errCode);
				
		} 
}

uint32_t ADC_BlockRead(uint8_t channel, int16_t *value)
{
		if (value == NULL || channel >= NRF_SAADC_CHANNEL_COUNT) {
				return (uint32_t) -1;
		}
		return nrf_drv_saadc_sample_convert(channel, value);
}
#endif
