/**
 * @file   vcom.c
 * @author Ricard Bitriá Ribes (https://github.com/dracir9)
 * Created Date: 05-02-2025
 * -----
 * Last Modified: 13-02-2025
 * Modified By: Ricard Bitriá Ribes
 * -----
 */

#include "vcom.h"
#include "usbd_cdc_if.h"

/* Received data over USB are stored in this buffer      */
extern uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

/* Data to send over USB CDC are stored in this buffer   */
extern uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

/* User interface callback functions                     */
extern USBD_CDC_ItfTypeDef USBD_Interface_fops_FS;

/* USB Device handle */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* VCOM internal state variables */
static char RxBuffer[VCOM_RX_BUF_SIZE] = {0};
static uint8_t strReceived = RESET;
static uint16_t strLen = 0;
static uint8_t vcomOpen = RESET;
uint8_t RTSstatus, DTRstatus;


/* Private function declaration */
static int8_t VCOM_Init_FS(void);
static int8_t VCOM_DeInit_FS(void);
static int8_t VCOM_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t VCOM_Receive_FS(uint8_t* pbuf, uint32_t *Len);
static uint8_t VCOM_Transmit_FS(uint8_t* Buf, uint16_t Len);
static int8_t VCOM_TransmitCplt_FS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

inline void VCOM_Init()
{
  // Inject custom functions
  USBD_Interface_fops_FS.Init =         VCOM_Init_FS;
  USBD_Interface_fops_FS.DeInit =       VCOM_DeInit_FS;
  USBD_Interface_fops_FS.Control =      VCOM_Control_FS;
  USBD_Interface_fops_FS.Receive =      VCOM_Receive_FS;
  USBD_Interface_fops_FS.TransmitCplt = VCOM_TransmitCplt_FS;

  strReceived = RESET;
  strLen = 0;
}

inline void VCOM_Task()
{
  return;  
}

inline uint8_t VCOM_IsConnected()
{
  return vcomOpen;
}

inline uint16_t VCOM_GetData(uint8_t *buf, uint16_t len)
{
  if (strLen == 0) return 0;

  len = strLen < len ? strLen : len;

  strReceived = RESET;
  strLen = 0;

  memcpy(buf, RxBuffer, len);
  return len;
}

uint16_t VCOM_GetStr(char *str, uint16_t maxLen)
{
  if (!strReceived) return 0;

  // Read the maximum number of chars possible
  maxLen = strLen < maxLen ? strLen : maxLen;

  strReceived = RESET;
  strLen = 0;

  memcpy(str, RxBuffer, maxLen);

  RxBuffer[maxLen-1] = '\0'; // Ensure string is null terminated

  return maxLen-1; // Return actual string length, without the terminating null character
}

inline uint16_t VCOM_BytesAvailable()
{
  return strLen;
}

uint8_t VCOM_IsStrAvailable()
{
  return strReceived;
}

void VCOM_Flush()
{
  fflush(stdout);
}

void VCOM_Discard()
{
  strLen = 0;
  strReceived = RESET;
}

void VCOM_Putc(uint8_t c)
{
  VCOM_Transmit_FS(&c, 1);
}

inline void VCOM_Puts(char s[])
{
  VCOM_Transmit_FS((uint8_t *)s, strlen(s));
}

inline void VCOM_SendData(uint8_t *buf, uint16_t len)
{
  VCOM_Transmit_FS(buf, len);
}

//--------------------------------------------------------------------+
// Private Functions
//--------------------------------------------------------------------+

/**
  * @brief  Initializes the CDC media low layer over the FS USB IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t VCOM_Init_FS(void)
{
  /* Set Application Buffers */
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
  return (USBD_OK);
}
 
/**
 * @brief  DeInitializes the CDC media low layer
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t VCOM_DeInit_FS(void)
{
  return (USBD_OK);
}

/**
 * @brief  Manage the CDC class requests
 * @param  cmd: Command code
 * @param  pbuf: Buffer containing command data (request parameters)
 * @param  length: Number of data to be sent (in bytes)
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t VCOM_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  USBD_SetupReqTypedef* req = (USBD_SetupReqTypedef*)pbuf;
  switch(cmd)
  {
    case CDC_SEND_ENCAPSULATED_COMMAND:
      break;

    case CDC_GET_ENCAPSULATED_RESPONSE:
      break;

    case CDC_SET_COMM_FEATURE:
      break;

    case CDC_GET_COMM_FEATURE:
      break;

    case CDC_CLEAR_COMM_FEATURE:
      break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
    case CDC_SET_LINE_CODING:
      break;

    case CDC_GET_LINE_CODING:
      break;

    case CDC_SET_CONTROL_LINE_STATE:
      RTSstatus = (req ->wValue & 0x02) > 0 ? SET : RESET; // host set/reset RTS
      DTRstatus = (req->wValue & 0x01) > 0 ? SET : RESET; // host set/reset DTR

      vcomOpen = DTRstatus;
      break;

    case CDC_SEND_BREAK:
      break;

    default:
      break;
  }

  return (USBD_OK);
}

/**
 * @brief  Data received over USB OUT endpoint are sent over CDC interface
 *         through this function.
 *
 *         @note
 *         This function will issue a NAK packet on any OUT packet received on
 *         USB endpoint until exiting this function. If you exit this function
 *         before transfer is complete on CDC interface (ie. using DMA controller)
 *         it will result in receiving more data while previous ones are still
 *         not sent.
 *
 * @param  Buf: Buffer of data to be received
 * @param  Len: Number of data received (in bytes)
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t VCOM_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  // If there is a string pending to read, then discard new data
  if (strReceived == SET) return USBD_OK;

  uint32_t i = 0;
  while(i < *Len && i < VCOM_RX_BUF_SIZE)
  {
	  if (Buf[i] < 32) // If it is a control character
	  {
		  RxBuffer[strLen++] = '\0';
		  strReceived = SET;
		  break;
	  }

	  RxBuffer[strLen++] = Buf[i++];
  }

  // Prepare for next data
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);

  return (USBD_OK);
}

/**
 * @brief  CDC_Transmit_FS
 *         Data to send over USB IN endpoint are sent over CDC interface
 *         through this function.
 *         @note
 *
 *
 * @param  Buf: Buffer of data to be sent
 * @param  Len: Number of data to be sent (in bytes)
 * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
 */
uint8_t VCOM_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;

  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);

  return result;
}

/**
 * @brief  CDC_TransmitCplt_FS
 *         Data transmitted callback
 *
 *         @note
 *         This function is IN transfer complete callback used to inform user that
 *         the submitted Data is successfully sent over USB.
 *
 * @param  Buf: Buffer of data to be received
 * @param  Len: Number of data received (in bytes)
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t VCOM_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  uint8_t result = USBD_OK;

  UNUSED(Buf);
  UNUSED(Len);
  UNUSED(epnum);

  return result;
}

//--------------------------------------------------------------------+
// Callbacks
//--------------------------------------------------------------------+
// Override built-in _write function. This gets called when data is written to stdout.
int _write(int file, char *ptr, int len)
{
  (void)file;

  while (VCOM_IsConnected() && CDC_Transmit_FS((uint8_t*)ptr, (uint16_t)len) == USBD_BUSY);
  return len;
}
