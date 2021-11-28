#ifndef _ALM_INIT_PROFILE_H_
#define _ALM_INIT_PROFILE_H_

/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"

#define uint16 uint16_t
#define uint8  uint8_t
#define uint32 uint32_t

/*********************************************************************
 * DEFINITIONS
 */
#ifdef __cplusplus
extern "C" {
#endif

#define BLE_ALMINITPROFILE_DEF(_name)						\
static AlmInitProfile_t _name;								\
NRF_SDH_BLE_OBSERVER(_name ## _obs,							\
                     BLE_ALMINITPROFILE_BLE_OBSERVER_PRIO,	\
                     HandleAlmInitProfileOnBleStackEvent, &_name)

#define ALMINITPROFILE_UUID_BASE		{0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, \
										0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define ALMINITPROFILE_UUID_SERVICE		0xFFE0
#define ALMINITPROFILE_UUID_CHAR1		0xFFE1
#define ALMINITPROFILE_UUID_CHAR2		0xFFE2
#define ALMINITPROFILE_UUID_CHAR3		0xFFE3
#define ALMINITPROFILE_UUID_CHAR4		0xFFE4

// Profile Parameters
#define ALMINITPROFILE_CHAR1			0        						// uint8 - Profile Characteristic 1 value
#define ALMINITPROFILE_CHAR2			1        						// uint8 - Profile Characteristic 2 value
#define ALMINITPROFILE_CHAR3			2        						// uint8 - Profile Characteristic 3 value
#define ALMINITPROFILE_CHAR4			3        						// uint8 - Profile Characteristic 4 value
#define ALMINITPROFILE_CHAR5			4        						// uint8 - Profile Characteristic 5 value
										
// Length of Characteristic in bytes
#define ALMINITPROFILE_CHAR1_LEN		20       						// CHAR1 LEN
#define ALMINITPROFILE_CHAR2_LEN		20       						// CHAR2 LEN
#define ALMINITPROFILE_CHAR3_LEN		20       						// CHAR3 LEN
#define ALMINITPROFILE_CHAR4_LEN		20       						// CHAR4 LEN
#define ALMINITPROFILE_CHAR5_LEN		20       						// CHAR5 LEN										

/*********************************************************************
 * TYPEDEFS
 */
// Forward declaration of the AlmInitProfile_t type.
typedef struct almInitProfileService_s AlmInitProfile_t;

typedef void (*AlmInitProfileCharWriteHandler_t)(uint16 connHandle, uint8 charId, AlmInitProfile_t *pAlmInitProfile,
												const uint8 *pCharValue, uint8 length);

/* 
 ALM Init Service init structure.
 This structure contains all options 
 and data needed for initialization of the service.
*/
typedef struct
{
    AlmInitProfileCharWriteHandler_t almInitProfileCharWriteHandler;	// Event handler to be called when the Characteristic is written.
} AlmInitProfileCallback_t;

/*
 ALM Init Service structure.
 This structure contains various status information for the service.
*/
struct almInitProfileService_s
{
    uint16 serviceHandle;      											// Handle of ALM Init Service (as provided by the BLE stack).
	ble_gatts_char_handles_t char1Handle; 								// Handles related to the Characteristic 1.
    ble_gatts_char_handles_t char2Handle;								// Handles related to the Characteristic 2.
	ble_gatts_char_handles_t char3Handle;								// Handles related to the Characteristic 3.
	ble_gatts_char_handles_t char4Handle;								// Handles related to the Characteristic 4.
    uint8 uuidType;           											// UUID type for the ALM Init Service.
    AlmInitProfileCharWriteHandler_t almInitProfileCharWriteHandler;	// Event handler to be called when the Characteristic is written.
};

/*********************************************************************
 * API FUNCTIONS
 */
uint32 AlmInitProfile_AddService(AlmInitProfileCharWriteHandler_t func);
uint32 AlmInitProfile_RegisterAppCallback(AlmInitProfile_t *pAlmInitProfile, const AlmInitProfileCallback_t *pAppCallback);
void HandleAlmInitProfileOnBleStackEvent(ble_evt_t const *pBleEvent, void *pContext);
uint32 AlmInitProfile_PushNotifyData(uint16 connHandle, AlmInitProfile_t *pAlmInitProfile, uint8 *pData, uint16 dataLen);
uint32 AlmInitProfile_SetData(AlmInitProfile_t *pAlmInitProfile, uint16_t characteristicId, uint8 *pData, uint16 dataLen);

#ifdef __cplusplus
}
#endif

#endif /* _ALM_INIT_PROFILE_H_ */
