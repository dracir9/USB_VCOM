/**
 * @file   vcom.c
 * @author Ricard Bitriá Ribes (https://github.com/dracir9)
 * Created Date: 05-02-2025
 * -----
 * Last Modified: 05-02-2025
 * Modified By: Ricard Bitriá Ribes
 * -----
 */

#include "vcom.h"

/* Received data over USB are stored in this buffer      */
extern uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

/* Data to send over USB CDC are stored in this buffer   */
extern uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

/* Buffer for received string */
static char strBuffer[VCOM_BUF_SIZE];
static uint16_t RxDataLen = 0;
static uint8_t isLineComplete = SET;
static uint8_t vcomOpen = RESET;
static uint8_t RTSstatus, DTRstatus;

