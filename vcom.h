/**
 * @file   vcom.h
 * @author Ricard Bitriá Ribes (https://github.com/dracir9)
 * Created Date: 05-02-2025
 * -----
 * Last Modified: 22-09-2025
 * Modified By: Ricard Bitriá Ribes
 * -----
 */

#ifndef USB_VCOM_VCOM_H_
#define USB_VCOM_VCOM_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include "stdint.h"

// Include configuration header if exists
// This allows to override default configuration values
#if defined __has_include
    #if __has_include ("config.h")
        #include "config.h"
    #endif
#endif

// Load default configuration values
// Defines can be overridden by the config.h file

/**
 * @brief  Size of the input buffer for the USB virtual COM
 * 
 */
#ifndef VCOM_RX_BUF_SIZE
    #define VCOM_RX_BUF_SIZE 128
#endif

/**
 * @brief  Redirect printf output to the USB virtual COM
 *        If set to 1, printf will be redirected to the USB virtual COM
 *        If set to 0, printf will not be redirected
 */
#ifndef VCOM_LINK_PRINTF
    #define VCOM_LINK_PRINTF 0
#endif

/**
 * @brief  
 * 
 */
void VCOM_Init();

void VCOM_Task();

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
uint8_t VCOM_IsStrAvailable();

/**
 * @brief  Check the status of the USB virtual COM
 * 
 * @return uint8_t          Returns 1 if the communication is active, returns 0 otherwise
 */
uint8_t VCOM_IsConnected();

/**
 * @brief  Check if the USB virtual COM is busy sending data
 * 
 * @return uint8_t          Returns USBD_BUSY if busy, USBD_OK otherwise
 */
uint8_t VCOM_IsBusy();

/**
 * @brief  Force sending data
 * 
 */
void VCOM_Flush();

/**
 * @brief  Clear input buffers
 * 
 */
void VCOM_Discard();

/**
 * @brief  Send a character over USB virtual COM
 * 
 * @param  c                Character to send
 */
void VCOM_Putc(uint8_t c);

/**
 * @brief  Send string over USB virtual COM
 *         String MUST be null terminated
 * 
 * @param  s                String pointer
 */
void VCOM_Puts(char s[]);

/**
 * @brief  Send data over USB virtual COM
 * 
 * @param  buf              Buffer pointer containing the data to send
 * @param  len              Number of bytes to send
 */
void VCOM_SendData(uint8_t *buf, uint16_t len);

/**
 * @brief  Print formatted data over USB virtual COM
 *         This function uses the same format as printf
 * 
 * @param  format           Format string
 * @param  ...              Variable arguments
 */
void VCOM_printf(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* USB_VCOM_VCOM_H_ */
