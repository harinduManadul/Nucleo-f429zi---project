/**
 ******************************************************************************
 * @file    abox_sevenseg.h
 * @author  s1234567
 * @date    08102026
 * @brief   Multiplexed 3-digit seven segment display helpers used by
 *          abox_coordinateTask. Segment pins SEG_A..SEG_DP and digit select
 *          pins SEG_DIG0..SEG_DIG2 are configured by CubeMX. Header only,
 *          because the spec does not allow extra .c files.
 *
 * REFERENCE: Written with assistance from Claude (Anthropic), generative AI
 *            tool, 08/10/2026. Reviewed and understood by author.
 ******************************************************************************
 */

#ifndef ABOX_SEVENSEG_H
#define ABOX_SEVENSEG_H

#include <stdint.h>
#include "main.h"
#include "abox_config.h"

#define SEVENSEG_DIGIT_COUNT    3
#define SEVENSEG_SEGMENT_COUNT  8
#define SEVENSEG_MAX_VALUE      999

/* GPIO port and pin of one display line */
typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} SevenSegPin;

/* Segment lines a, b, c, d, e, f, g, dp */
static const SevenSegPin sevenSegSegments[SEVENSEG_SEGMENT_COUNT] = {
    {SEG_A_GPIO_Port, SEG_A_Pin},
    {SEG_B_GPIO_Port, SEG_B_Pin},
    {SEG_C_GPIO_Port, SEG_C_Pin},
    {SEG_D_GPIO_Port, SEG_D_Pin},
    {SEG_E_GPIO_Port, SEG_E_Pin},
    {SEG_F_GPIO_Port, SEG_F_Pin},
    {SEG_G_GPIO_Port, SEG_G_Pin},
    {SEG_DP_GPIO_Port, SEG_DP_Pin},
};

/* Digit select lines, index 0 is the units digit */
static const SevenSegPin sevenSegDigits[SEVENSEG_DIGIT_COUNT] = {
    {SEG_DIG0_GPIO_Port, SEG_DIG0_Pin},
    {SEG_DIG1_GPIO_Port, SEG_DIG1_Pin},
    {SEG_DIG2_GPIO_Port, SEG_DIG2_Pin},
};

/* Segment patterns for 0-9, bit 0 = a ... bit 6 = g (lit = 1) */
static const uint8_t sevenSegNumerals[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

/**
 * @brief  Drive one display line on or off, applying the common anode
 *         inversion when configured.
 * @param  line Pin to drive.
 * @param  on   1 to light the segment / enable the digit.
 */
static inline void sevenseg_drive(const SevenSegPin *line, uint8_t on) {

#if SEVENSEG_COMMON_ANODE
    on = !on;
#endif
    if (on) {
        line->port->BSRR = line->pin;
    } else {
        line->port->BSRR = (uint32_t)line->pin << 16;
    }
}

/**
 * @brief  Show one digit of a value; call repeatedly with digit 0..2 to
 *         multiplex the whole number. Leading zeros are blanked.
 * @param  value Number to show (0-999).
 * @param  digit Digit position to light, 0 is the units digit.
 */
static inline void sevenseg_show_digit(uint16_t value, uint8_t digit) {

    uint8_t i;
    uint8_t pattern = 0;
    uint16_t divisor = 1;

    for (i = 0; i < digit; i++) {
        divisor *= 10;
    }

    /* Blank leading zeros, but always show the units digit */
    if ((digit == 0) || (value >= divisor)) {
        pattern = sevenSegNumerals[(value / divisor) % 10];
    }

    /* Turn all digits off while the segments change to avoid ghosting */
    for (i = 0; i < SEVENSEG_DIGIT_COUNT; i++) {
        sevenseg_drive(&sevenSegDigits[i], 0);
    }

    for (i = 0; i < SEVENSEG_SEGMENT_COUNT; i++) {
        sevenseg_drive(&sevenSegSegments[i], (pattern >> i) & 0x01);
    }

    sevenseg_drive(&sevenSegDigits[digit], 1);
}

#endif /* ABOX_SEVENSEG_H */
