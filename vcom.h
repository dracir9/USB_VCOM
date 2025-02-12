/**
 * @file   vcom.h
 * @author Ricard Bitriá Ribes (https://github.com/dracir9)
 * Created Date: 05-02-2025
 * -----
 * Last Modified: 12-02-2025
 * Modified By: Ricard Bitriá Ribes
 * -----
 */

#ifndef USB_VCOM_VCOM_H_
#define USB_VCOM_VCOM_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include "stm32f4xx_hal.h"

#define VCOM_RX_BUF_SIZE 128

/**
 * @brief  
 * 
 */
void VCOM_Init();

void VCOM_Task();

//void VCOM_DeviceStop(void);
//void VCOM_DeviceReStart(void);
//uint8_t VCOM_DeviceStatus(void);
//uint8_t VCOM_GetStatus(void);

/**
 * @brief  Get all raw bytes currently stored in the input buffer and remove them.
 *         Data is returned AS IS, without any termination and ignoring the line break characters.
 * 
 * @param  buf              Output buffer where the data will be copied
 * @param  len              Maximum number of bytes to copy
 * @return uint16_t         Return the number of bytes written
 */
uint16_t VCOM_GetData(uint8_t *buf, uint16_t len);

/**
 * @brief  Get last valid string currently stored in the input buffer and remove it.
 *         The string is null terminated and the line break characters have been removed.
 * 
 * @param  str              Param Output buffer where the data will be copied
 * @param  maxLen           Param Maximum number of bytes to copy
 * @return uint16_t         Return the total number of bytes written
 */
uint16_t VCOM_GetStr(char *str, uint16_t maxLen);

/**
 * @brief  Get the number of bytes currently stored in the input buffer
 * 
 * @return uint16_t         Return the number of bytes
 */
uint16_t VCOM_BytesAvailable();

/**
 * @brief  Check whether there is an available string
 * 
 * @return uint8_t          Return 1 if there is an available string, returns 0 otherwise
 */
uint8_t VCOM_isStrAvailable();

/**
 * @brief  Check the status of the USB virtual COM
 * 
 * @return uint8_t          Returns 1 if the communication is active, returns 0 otherwise
 */
uint8_t VCOM_isConnected();

/**
 * @brief  Empty input buffer
 * 
 */
void VCOM_flush();

/**
 * @brief  Send a character over USB virtual COM
 * 
 * @param  c                Character to send
 */
void VCOM_putc(uint8_t c);

/**
 * @brief  Send string over USB virtual COM
 *         String MUST be null terminated
 * 
 * @param  s                String pointer
 */
void VCOM_puts(char s[]);

/**
 * @brief  Send data over USB virtual COM
 * 
 * @param  buf              Buffer pointer containing the data to send
 * @param  len              Number of bytes to send
 */
void VCOM_SendData(uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* USB_VCOM_VCOM_H_ */
