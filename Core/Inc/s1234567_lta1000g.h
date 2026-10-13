/**
 ******************************************************************************
 * @file    s1234567_lta1000g.h
 * @author  s1234567
 * @date    08102026
 * @brief   LTA1000G 10-segment LED bar driver. Pins are configured by
 *          CubeMX (labels LTA_SEG0 .. LTA_SEG9).
 ******************************************************************************
 */

#ifndef S1234567_LTA1000G_H
#define S1234567_LTA1000G_H

#include <stdint.h>

#define LTA1000G_SEGMENT_COUNT  10

/**
 * @brief  Turn every segment off. GPIO clocks/modes are set by MX_GPIO_Init.
 */
void s1234567_lta1000g_init(void);

/**
 * @brief  Show a value on the LED bar; bit n of value drives segment n.
 * @param  value Bit pattern (bits 9:0 used).
 */
void s1234567_lta1000g_write(uint16_t value);

#endif /* S1234567_LTA1000G_H */
