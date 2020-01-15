/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018-2019 Asbjørn Djupdal, NTNU
 *
 *****************************************************************************/

#ifndef USBCONFIG_H
#define USBCONFIG_H

#define USB_DEVICE

#define NUM_EP_USED 3
#define NUM_APP_TIMERS 1

#define CDC_CTRL_INTERFACE_NO   0
#define CDC_DATA_INTERFACE_NO   1

#define CDC_EP_DATA_OUT   0x01
#define CDC_EP_DATA_IN    0x81
#define CDC_EP_NOTIFY     0x82

#define CDC_TIMER_ID              0
#define CDC_UART_TX_DMA_CHANNEL   0
#define CDC_UART_RX_DMA_CHANNEL   1
#define CDC_TX_DMA_SIGNAL         DMAREQ_UART1_TXBL
#define CDC_RX_DMA_SIGNAL         DMAREQ_UART1_RXDATAV
#define CDC_UART                  UART1
#define CDC_UART_CLOCK            cmuClock_UART1
#define CDC_UART_ROUTE            (UART_ROUTE_RXPEN | \
                                   UART_ROUTE_TXPEN | \
                                   UART_ROUTE_LOCATION_LOC2)
#define CDC_UART_TX_PORT          gpioPortB
#define CDC_UART_TX_PIN           9
#define CDC_UART_RX_PORT          gpioPortB
#define CDC_UART_RX_PIN           10
#define CDC_ENABLE_DK_UART_SWITCH() BSP_PeripheralAccess(BSP_RS232_UART, true)

#endif
