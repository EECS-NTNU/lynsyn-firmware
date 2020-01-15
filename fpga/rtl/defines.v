/******************************************************************************
 *
 *  This file is part of the Lynsyn PMU firmware.
 *
 *  Copyright 2018 Asbjørn Djupdal, NTNU, TULIPP EU Project
 *
 *****************************************************************************/

`define JTAG_EXT  0
`define JTAG_INT  1
`define JTAG_TEST 2

`define MAGIC 8'had

// SPI commands

`define SPI_CMD_STATUS          0 // CMD/status - 0/status
`define SPI_CMD_MAGIC           1 // CMD/status - 0/data
`define SPI_CMD_JTAG_SEL        2 // CMD/status - sel/0
`define SPI_CMD_WR_SEQ          3 // CMD/status - size/0 - (tdidata/0 tmsdata/0)* 0/0
`define SPI_CMD_RDWR_SEQ        4 // CMD/status - size/0 - (tdidata/0 tmsdata/0)* 0/0
`define SPI_CMD_GET_DATA        5 // CMD/status - (0/data)* (ff/data)

`define SPI_CMD_STORE_SEQ       6 // CMD/status - sizel/0 sizeh/0 (tdidata/0 tmsdata/0)* 0/0
`define SPI_CMD_STORE_PROG      7 // CMD/status - size/0 (read/0 initposl/0 initposh/0 loopposl/0 loopposh/0 ackposl/0 ackposh/0 endposl/0 endposh/0)* 0/0

`define SPI_CMD_EXECUTE_SEQ     8 // CMD/status - 0/0
`define SPI_CMD_JTAG_TEST       9 // CMD/status - 0/data
`define SPI_CMD_OSC_TEST       10 // CMD/status - 0/data

// SPI -> JTAG controller commands

`define FIFO_CMD_WR         5'h0
`define FIFO_CMD_STORE_SEQ  5'h1
`define FIFO_CMD_STORE_PROG 5'h2
`define FIFO_CMD_EXECUTE    5'h3
`define FIFO_CMD_FLUSH      5'h4
