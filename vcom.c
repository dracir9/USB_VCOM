/**
 * @file   vcom.c
 * @author Ricard Bitriá Ribes (https://github.com/dracir9)
 * Created Date: 05-02-2025
 * -----
 * Last Modified: 11-02-2025
 * Modified By: Ricard Bitriá Ribes
 * -----
 */

#include "vcom.h"
#include "tusb.h"

static uint8_t strReceived = 0;
static uint16_t strLen = 0;

inline void VCOM_Init()
{
  tud_init(BOARD_TUD_RHPORT);

  strReceived = 0;
  strLen = 0;

  // Set the line ending character to new line
  tud_cdc_set_wanted_char('\n');
}

inline void VCOM_Task ()
{
  tud_task();    
}

inline uint16_t VCOM_GetData(uint8_t *buf, uint16_t len)
{
  strReceived = 0;
  strLen = 0;
  return tud_cdc_read(buf, len);
}

uint16_t VCOM_GetStr(char *str, uint16_t maxLen)
{
  if (strReceived)
  {
    // Read the maximum number of chars possible
    maxLen = strLen < maxLen ? strLen : maxLen;
    uint32_t count = tud_cdc_read(str, maxLen);

    // If the full string has been read
    if (count >= strLen)
    {
      uint32_t i = count;
      while(str[--i] < 32);
      str[count - i] = '\0';
      tud_cdc_read_flush();
      strReceived = 0;
      strLen = 0;
    }
    else
      strLen -= count;

    return count;
  }
  return 0;
}

inline uint16_t VCOM_BytesAvailable()
{
  return tud_cdc_available();
}

uint8_t VCOM_isStrAvailable()
{
  return strReceived;
}

void VCOM_flush()
{
  tud_cdc_write_flush();
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

void tud_cdc_rx_wanted_cb(uint8_t itf, char wanted_char)
{
  if (!strReceived) 
  {
    strLen = tud_cdc_n_available(itf);
    strReceived = 1;
  }
}

void OTG_FS_IRQHandler(void)
{
  tud_int_handler(0);
}
