/**
 * @file   vcom.c
 * @author Ricard Bitriá Ribes (https://github.com/dracir9)
 * Created Date: 05-02-2025
 * -----
 * Last Modified: 12-02-2025
 * Modified By: Ricard Bitriá Ribes
 * -----
 */

#include "vcom.h"
#include "tusb.h"

static char RxBuffer[VCOM_RX_BUF_SIZE] = {0};
static uint8_t strReceived = 0;
static uint16_t strLen = 0;

inline void VCOM_Init()
{
  tud_init(BOARD_TUD_RHPORT);

  strReceived = 0;
  strLen = 0;
}

inline void VCOM_Task()
{
  tud_task();    
}

inline uint16_t VCOM_GetData(uint8_t *buf, uint16_t len)
{
  if (strLen == 0) return 0;

  len = strLen < len ? strLen : len;

  strReceived = 0;
  strLen = 0;

  strncpy(buf, RxBuffer, len);
  return len;
}

uint16_t VCOM_GetStr(char *str, uint16_t maxLen)
{
  if (!strReceived) return 0;

  // Read the maximum number of chars possible
  maxLen = strLen < maxLen ? strLen : maxLen;

  strReceived = 0;
  strLen = 0;

  strncpy(str, RxBuffer, maxLen);
  return maxLen;
}

inline uint16_t VCOM_BytesAvailable()
{
  return strLen;
}

uint8_t VCOM_isStrAvailable()
{
  return strReceived;
}

void VCOM_flush()
{
  tud_cdc_write_flush();
  strLen = 0;
  strReceived = 0;
}

void VCOM_putc(uint8_t c)
{
  tud_cdc_write(&c, 1);
}

inline void VCOM_puts(char s[])
{
  tud_cdc_write_str(s);
}

inline void VCOM_SendData(uint8_t *buf, uint16_t len)
{
  tud_cdc_write(buf, len);
}

//--------------------------------------------------------------------+
// Interrupts and callbacks
//--------------------------------------------------------------------+

void tud_cdc_rx_cb(uint8_t itf)
{
  if (strReceived)
  {// If there is a string pending to read, then discard new data
    tud_cdc_n_read_flush(itf);
    return;
  }

  // Read data
  uint32_t count = tud_cdc_n_read(itf, &RxBuffer[strLen], VCOM_RX_BUF_SIZE - strLen);

  // Check for end of line characters
  for (uint32_t i = 0; i < count; i++)
  {
    if (RxBuffer[strLen] < 32)
    {
      RxBuffer[strLen] = '\0';
		  strReceived = SET;
		  break;
    }
    strLen++;
  }
}

void OTG_FS_IRQHandler(void)
{
  tud_int_handler(0);
}
