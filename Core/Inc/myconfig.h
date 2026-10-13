/*
 ******************************************************************************
 * File: myconfig.h
 * Author: s1234567
 * Date: 08102026
 * Brief: Config file used for the tx radio. Included only by
 *        nrf24l01plus_hal_tx.c (when MYCONFIG is defined), because it
 *        defines the address array.
 *        Placeholder channel/address - replace with the ABOX values from
 *        BlackBoard.
 ******************************************************************************
 */

#ifndef MYCONFIG_H
#define MYCONFIG_H

#include <stdint.h>

#define MYRADIOCHAN 52
uint8_t myradiotxaddr[5] = {0x30, 0x10, 0x00, 0x00, 0x52};

#endif
