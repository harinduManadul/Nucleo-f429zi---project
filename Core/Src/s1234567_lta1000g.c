/**
 ******************************************************************************
 * @file    s1234567_lta1000g.c
 * @author  s1234567
 * @date    08102026
 * @brief   LTA1000G 10-segment LED bar driver.
 *
 * REFERENCE: Written with assistance from Claude (Anthropic), generative AI
 *            tool, 08/10/2026. Reviewed and understood by author.
 ******************************************************************************
 */

#include "main.h"
#include "s1234567_lta1000g.h"

/* One LED bar segment: GPIO port and pin mask */
typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} LtaSegment;

/* Segment n of the bar, using the CubeMX pin labels from main.h */
static const LtaSegment ltaSegments[LTA1000G_SEGMENT_COUNT] = {
    {LTA_SEG0_GPIO_Port, LTA_SEG0_Pin},
    {LTA_SEG1_GPIO_Port, LTA_SEG1_Pin},
    {LTA_SEG2_GPIO_Port, LTA_SEG2_Pin},
    {LTA_SEG3_GPIO_Port, LTA_SEG3_Pin},
    {LTA_SEG4_GPIO_Port, LTA_SEG4_Pin},
    {LTA_SEG5_GPIO_Port, LTA_SEG5_Pin},
    {LTA_SEG6_GPIO_Port, LTA_SEG6_Pin},
    {LTA_SEG7_GPIO_Port, LTA_SEG7_Pin},
    {LTA_SEG8_GPIO_Port, LTA_SEG8_Pin},
    {LTA_SEG9_GPIO_Port, LTA_SEG9_Pin},
};

void s1234567_lta1000g_init(void) {

    s1234567_lta1000g_write(0);
}

void s1234567_lta1000g_write(uint16_t value) {

    uint8_t i;

    for (i = 0; i < LTA1000G_SEGMENT_COUNT; i++) {
        /* BSRR: lower 16 bits set the pin, upper 16 bits reset it */
        if (value & (1U << i)) {
            ltaSegments[i].port->BSRR = ltaSegments[i].pin;
        } else {
            ltaSegments[i].port->BSRR = (uint32_t)ltaSegments[i].pin << 16;
        }
    }
}
