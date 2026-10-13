/**
 ******************************************************************************
 * @file    abox_config.h
 * @author  s1234567
 * @date    08102026
 * @brief   User-adjustable configuration for the ABOX controller.
 *          Every value that may need changing in the lab (student ID,
 *          drawing area, pen heights, timing) is collected here.
 *
 * REFERENCE: Structure written with assistance from Claude (Anthropic),
 *            generative AI tool, 08/10/2026. Reviewed and understood by author.
 ******************************************************************************
 */

#ifndef ABOX_CONFIG_H
#define ABOX_CONFIG_H

/* Sender address: student ID written as BCD, e.g. 45678910 -> 0x45678910.
 * Placeholder value - replace with your own student number. */
#define ABOX_STUDENT_ID         0x12345678U

/* Byte order of the sender address within the packet.
 * 1: most significant byte first (0x12, 0x34, 0x56, 0x78), as in the spec
 *    example. 0: least significant byte first. */
#define ABOX_ADDR_MSB_FIRST     1

/* Drawing area limits in mm (origin is the bottom-left corner) */
#define ABOX_X_MAX_MM           200
#define ABOX_Y_MAX_MM           200
#define ABOX_Z_MAX_MM           99      /* Z is sent as 2 digits */

/* Pen heights in mm */
#define ABOX_PEN_DOWN_Z         0       /* marker touching the whiteboard */
#define ABOX_PEN_UP_Z           10      /* marker lifted off the whiteboard */

/* Line and shape commands use VALUE in multiples of this distance */
#define ABOX_GRID_STEP_MM       20

/* Number of straight segments used to approximate a circle */
#define ABOX_CIRCLE_SEGMENTS    16

/* Naught shape: 1 draws a circle, 0 draws a square */
#define ABOX_NAUGHT_CIRCLE      1

/* Cross shape: 1 draws an X (diagonals), 0 draws a + */
#define ABOX_CROSS_DIAGONAL     1

/* Time allowed for the head to finish a move before the next packet */
#define ABOX_MOVE_MS_PER_MM     20
#define ABOX_MOVE_MIN_MS        300

/* Pushbutton debounce window */
#define ABOX_DEBOUNCE_MS        50

/* Seven segment display wiring: 1 for common anode, 0 for common cathode */
#define SEVENSEG_COMMON_ANODE   0

/* Debug output: add ABOX_DEBUG to the build's preprocessor symbols to print
 * button events and packet hex dumps on the ST-LINK virtual COM port. */

#endif /* ABOX_CONFIG_H */
