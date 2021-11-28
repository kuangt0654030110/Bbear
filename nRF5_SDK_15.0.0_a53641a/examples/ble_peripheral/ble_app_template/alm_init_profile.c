
/*********************************************************************
 * INCLUDES
 */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble_srv_common.h"
#include "alm_init_profile.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#if NRF_MODULE_ENABLED(BLE_ALMINITPROFILE)

static void almInitProfile_writeAttrCallback(AlmInitProfile_t *pAlmInitProfile, ble_evt_t const *pBleEvent);

/*********************************************************************
 * LOCAL VARIABLES
 */
BLE_ALMINITPROFILE_DEF(m_almInitProfile);
 
/*********************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 @brief 添加ALM Init Profile服务
 @param 无
 @return NRF_SUCCESS - 成功；其他值 - 失败
*/
uint32 AlmInitProfile_AddService(AlmInitProfileCharWriteHandler_t func)
{	
	AlmInitProfileCallback_t almInitProfileCallback = {0};
	almInitProfileCallback.almInitProfileCharWriteHandler = func;	// 调用外部函数：应用层处理特征值

	return AlmInitProfile_RegisterAppCallback(&m_almInitProfile, &almInitProfileCallback);
}

/**
 @brief 注册应用程序回调函数。只调用这个函数一次
 @param pAlmInitProfile -[out] ALM初始化服务结构体
 @param pAppCallback -[in] 指向应用程序的回调
 @return NRF_SUCCESS - 成功；其他值 - 失败
*/
uint32 AlmInitProfile_RegisterAppCallback(AlmInitProfile_t *pAlmInitProfile, const AlmInitProfileCallback_t *pAppCallback)
{
    uint32 errCode;
    ble_uuid_t bleUuid;
    ble_add_char_params_t addCharParams;
		uint8 init_data[20] = {0};
    // 初始化服务结构体
    pAlmInitProfile->almInitProfileCharWriteHandler = pAppCallback->almInitProfileCharWriteHandler;

    /*--------------------- 服务 ---------------------*/
    ble_uuid128_t baseUuid = {ALMINITPROFILE_UUID_BASE};
    errCode = sd_ble_uuid_vs_add(&baseUuid, &pAlmInitProfile->uuidType);
    VERIFY_SUCCESS(errCode);

    bleUuid.type = pAlmInitProfile->uuidType;
    bleUuid.uuid = ALMINITPROFILE_UUID_SERVICE;

    errCode = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &bleUuid, &pAlmInitProfile->serviceHandle);
    VERIFY_SUCCESS(errCode);

    /*--------------------- 特征1 ---------------------*/
    memset(&addCharParams, 0, sizeof(addCharParams));
    addCharParams.uuid              = ALMINITPROFILE_UUID_CHAR1;
    addCharParams.uuid_type         = pAlmInitProfile->uuidType;
    addCharParams.init_len          = ALMINITPROFILE_CHAR1_LEN;
		addCharParams.p_init_value      = init_data;
    addCharParams.max_len           = ALMINITPROFILE_CHAR1_LEN;								// 特征长度
    addCharParams.char_props.read   = 1;													// 可读
		//addCharParams.char_props.write  = 1;													// 可写
    addCharParams.char_props.notify = 1;													// 可通知								

    addCharParams.read_access       = SEC_OPEN;
		addCharParams.write_access      = SEC_OPEN;
    addCharParams.cccd_write_access = SEC_OPEN;

    errCode = characteristic_add(pAlmInitProfile->serviceHandle, &addCharParams, &pAlmInitProfile->char1Handle);
    if(errCode != NRF_SUCCESS) {
        return errCode;
    }

    /*--------------------- 特征2 ---------------------*/
    memset(&addCharParams, 0, sizeof(addCharParams));
    addCharParams.uuid             = ALMINITPROFILE_UUID_CHAR2;
    addCharParams.uuid_type        = pAlmInitProfile->uuidType;
    addCharParams.init_len         = ALMINITPROFILE_CHAR2_LEN;
		addCharParams.p_init_value     = init_data;
    addCharParams.max_len          = ALMINITPROFILE_CHAR2_LEN;								// 特征长度
//    addCharParams.char_props.read  = 1;														// 可读
    addCharParams.char_props.write = 1;														// 可写

    addCharParams.read_access  = SEC_OPEN;
    addCharParams.write_access = SEC_OPEN;

    errCode = characteristic_add(pAlmInitProfile->serviceHandle, &addCharParams, &pAlmInitProfile->char2Handle);
		if(errCode != NRF_SUCCESS) {
        return errCode;
    }
	
	/*--------------------- 特征3 ---------------------*/
    memset(&addCharParams, 0, sizeof(addCharParams));
    addCharParams.uuid             = ALMINITPROFILE_UUID_CHAR3;
    addCharParams.uuid_type        = pAlmInitProfile->uuidType;
    addCharParams.init_len         = ALMINITPROFILE_CHAR3_LEN;
		addCharParams.p_init_value     = init_data;
    addCharParams.max_len          = ALMINITPROFILE_CHAR3_LEN;		// 特征长度
    addCharParams.char_props.read  = 1;														// 可读
    //addCharParams.char_props.write = 1;														// 可写

    addCharParams.read_access  = SEC_OPEN;
    addCharParams.write_access = SEC_OPEN;

    errCode = characteristic_add(pAlmInitProfile->serviceHandle, &addCharParams, &pAlmInitProfile->char3Handle);
		if(errCode != NRF_SUCCESS) {
        return errCode;
    }
	
	/*--------------------- 特征4 ---------------------*/
    memset(&addCharParams, 0, sizeof(addCharParams));
    addCharParams.uuid             = ALMINITPROFILE_UUID_CHAR4;
    addCharParams.uuid_type        = pAlmInitProfile->uuidType;
    addCharParams.init_len         = ALMINITPROFILE_CHAR4_LEN;
		addCharParams.p_init_value     = init_data;
    addCharParams.max_len          = ALMINITPROFILE_CHAR4_LEN;								// 特征长度
    addCharParams.char_props.read  = 1;														// 可读
    //addCharParams.char_props.write = 1;														// 可写

    addCharParams.read_access  = SEC_OPEN;
    addCharParams.write_access = SEC_OPEN;

    errCode = characteristic_add(pAlmInitProfile->serviceHandle, &addCharParams, &pAlmInitProfile->char4Handle);
		if(errCode != NRF_SUCCESS) {
        return errCode;
    }
	
	return errCode;
}
 
/**
 @brief 处理来自蓝牙协议栈的应用事件
 @param pBleEvent -[in] 来自蓝牙协议栈的事件
 @param pContext -[in] ALM初始化服务结构体
 @return 无
*/
void HandleAlmInitProfileOnBleStackEvent(ble_evt_t const *pBleEvent, void *pContext)
{
    AlmInitProfile_t *pAlmInitProfile = (AlmInitProfile_t *)pContext;

    switch(pBleEvent->header.evt_id)
    {
	case BLE_GATTS_EVT_WRITE:
		almInitProfile_writeAttrCallback(pAlmInitProfile, pBleEvent);
		break;
	
	default:
		// No implementation needed.
		break;
    }
}

/**
 @brief 推送通知数据
 @param connHandle -[in] 连接句柄
 @param pAlmInitProfile -[in] ALM初始化服务结构体
 @param pData -[in] 通知内容
 @param dataLen -[in] 通知内容长度
 @return NRF_SUCCESS - 成功；其他值 - 失败
*/
uint32 AlmInitProfile_PushNotifyData(uint16 connHandle, AlmInitProfile_t *pAlmInitProfile, uint8 *pData, uint16 dataLen)
{
    ble_gatts_hvx_params_t params;
	
		if (pAlmInitProfile == NULL || pData == NULL || dataLen == 0) {
				return NRF_ERROR_NULL;
		}
		if(connHandle == BLE_CONN_HANDLE_INVALID)
    {
        return NRF_ERROR_NOT_FOUND;
    }
		if(dataLen > ALMINITPROFILE_CHAR1_LEN)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    memset(&params, 0, sizeof(params));
    params.type   = BLE_GATT_HVX_NOTIFICATION;
    params.handle = pAlmInitProfile->char1Handle.value_handle;
    params.p_data = pData;
    params.p_len  = &dataLen;

    return sd_ble_gatts_hvx(connHandle, &params);
}

uint32 AlmInitProfile_SetData(AlmInitProfile_t *pAlmInitProfile, uint16_t characteristicId, uint8 *pData, uint16 dataLen)
{       
		ret_code_t         err_code = NRF_SUCCESS;
    ble_gatts_value_t  gatts_value;
		uint32 data_max_len = ALMINITPROFILE_CHAR1_LEN;
 		uint16_t value_handle;
		if (pAlmInitProfile == NULL) {
				pAlmInitProfile = &m_almInitProfile;
		}
		if (pAlmInitProfile == NULL || pData == NULL || dataLen == 0) {
				return NRF_ERROR_NULL;
		}
		if (characteristicId == ALMINITPROFILE_CHAR1) {
				data_max_len = ALMINITPROFILE_CHAR1_LEN;
				value_handle = pAlmInitProfile->char1Handle.value_handle;
		} else if (characteristicId == ALMINITPROFILE_CHAR2) {
				data_max_len = ALMINITPROFILE_CHAR2_LEN;
				value_handle = pAlmInitProfile->char2Handle.value_handle;
		} else if (characteristicId == ALMINITPROFILE_CHAR3) {
				data_max_len = ALMINITPROFILE_CHAR3_LEN;
				value_handle = pAlmInitProfile->char3Handle.value_handle;
		} else if (characteristicId == ALMINITPROFILE_CHAR4) {
				data_max_len = ALMINITPROFILE_CHAR4_LEN;
				value_handle = pAlmInitProfile->char4Handle.value_handle;
		} else {
				return NRF_ERROR_NOT_FOUND;
		}
		if(dataLen > data_max_len) {
        return NRF_ERROR_INVALID_PARAM;
    }
		// Initialize value struct.
		memset(&gatts_value, 0, sizeof(gatts_value));
		
		gatts_value.len     = dataLen;
		gatts_value.offset  = 0;
		gatts_value.p_value = pData;
		
		// Update database.
		err_code = sd_ble_gatts_value_set(BLE_CONN_HANDLE_INVALID, value_handle, &gatts_value);
		return err_code;
}

/*********************************************************************
 * LOCAL FUNCTIONS
 */
/**
 @brief 写属性，在写入之前验证属性数据
 @param pAlmInitProfile -[in] ALM初始化服务结构体
 @param pBleEvent -[in] 来自蓝牙协议栈的事件
 @return 无
*/
static void almInitProfile_writeAttrCallback(AlmInitProfile_t *pAlmInitProfile, ble_evt_t const *pBleEvent)
{
    ble_gatts_evt_write_t const *pEventWrite = &pBleEvent->evt.gatts_evt.params.write;
	uint8 characteristicId;

	/*--------------------- 特征1 ---------------------*/
	if ((pEventWrite->handle == pAlmInitProfile->char1Handle.value_handle)
		&& ((pEventWrite->len > 0) && (pEventWrite->len <= ALMINITPROFILE_CHAR1_LEN))	
		&& (pAlmInitProfile->almInitProfileCharWriteHandler != NULL)) {
				characteristicId = ALMINITPROFILE_CHAR1;
        pAlmInitProfile->almInitProfileCharWriteHandler(pBleEvent->evt.gap_evt.conn_handle, characteristicId,
														pAlmInitProfile, pEventWrite->data, pEventWrite->len);
    }	
	/*--------------------- 特征2 ---------------------*/
	else if ((pEventWrite->handle == pAlmInitProfile->char2Handle.value_handle)
		&& ((pEventWrite->len > 0) && (pEventWrite->len <= ALMINITPROFILE_CHAR2_LEN))	
		&& (pAlmInitProfile->almInitProfileCharWriteHandler != NULL)) {
				characteristicId = ALMINITPROFILE_CHAR2;
        pAlmInitProfile->almInitProfileCharWriteHandler(pBleEvent->evt.gap_evt.conn_handle, characteristicId,
														pAlmInitProfile, pEventWrite->data, pEventWrite->len);
    }
	/*--------------------- 特征3 ---------------------*/
	else if ((pEventWrite->handle == pAlmInitProfile->char3Handle.value_handle)
		&& ((pEventWrite->len > 0) && (pEventWrite->len <= ALMINITPROFILE_CHAR3_LEN))	
		&& (pAlmInitProfile->almInitProfileCharWriteHandler != NULL)) {
				characteristicId = ALMINITPROFILE_CHAR3;
        pAlmInitProfile->almInitProfileCharWriteHandler(pBleEvent->evt.gap_evt.conn_handle, characteristicId,
														pAlmInitProfile, pEventWrite->data, pEventWrite->len);
    }
	/*--------------------- 特征4 ---------------------*/
	else if ((pEventWrite->handle == pAlmInitProfile->char4Handle.value_handle)
		&& ((pEventWrite->len > 0) && (pEventWrite->len <= ALMINITPROFILE_CHAR4_LEN))	
		&& (pAlmInitProfile->almInitProfileCharWriteHandler != NULL)) {
				characteristicId = ALMINITPROFILE_CHAR4;
        pAlmInitProfile->almInitProfileCharWriteHandler(pBleEvent->evt.gap_evt.conn_handle, characteristicId,
														pAlmInitProfile, pEventWrite->data, pEventWrite->len);
    }
}

#endif // NRF_MODULE_ENABLED(BLE_ALMINITPROFILE)

/****************************************************END OF FILE****************************************************/
