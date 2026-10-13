/**
 ******************************************************************************
 * @file    abox.h
 * @author  s1234567
 * @date    08102026
 * @brief   Shared definitions for the ABOX FreeRTOS tasks: command opcodes,
 *          radio packet types, inter-task message types and the ISR hooks
 *          used by stm32f4xx_it.c and main.c.
 *
 * REFERENCE: Written with assistance from Claude (Anthropic), generative AI
 *            tool, 08/10/2026. Reviewed and understood by author.
 ******************************************************************************
 */

#ifndef ABOX_H
#define ABOX_H

#include <stdint.h>
#include "abox_config.h"

/* Command opcodes selected with LAB PB0 (Table 12) */
#define ABOX_OP_ORG         0x01
#define ABOX_OP_X           0x02
#define ABOX_OP_Y           0x03
#define ABOX_OP_Z           0x04
#define ABOX_OP_LINE        0x05
#define ABOX_OP_NAUGHT      0x06
#define ABOX_OP_CROSS       0x07
#define ABOX_OP_JOYSTICK    0x0A
#define ABOX_OP_NEW         0x0B
#define ABOX_OP_DEL         0x0C
#define ABOX_OP_SYS         0x0D
#define ABOX_OP_JOIN        0x0F

#define ABOX_OP_FIRST       ABOX_OP_ORG
#define ABOX_OP_LAST        ABOX_OP_JOIN

/* Radio packet types (Table 10) */
#define ABOX_PKT_JOIN       0x20
#define ABOX_PKT_XYZ        0x22

/* Packet layout (Table 9) */
#define ABOX_PKT_TYPE_IDX       0
#define ABOX_PKT_ADDR_IDX       1
#define ABOX_PKT_ADDR_SIZE      4
#define ABOX_PKT_PAYLOAD_IDX    5
#define ABOX_PKT_PAYLOAD_SIZE   11
#define ABOX_PKT_RAW_SIZE       16
#define ABOX_PKT_ENCODED_SIZE   32

/* Axis identifiers used by the coordinate display */
typedef enum {
    ABOX_AXIS_X = 0,
    ABOX_AXIS_Y,
    ABOX_AXIS_Z
} AboxAxis;

/* Lab pushbutton identifiers sent from the EXTI ISR to abox_inTask */
typedef enum {
    ABOX_BTN_CMD = 0,   /* LAB PB0 - select next command */
    ABOX_BTN_EXE        /* LAB PB3 - execute command */
} AboxButton;

/* Command sent from abox_inTask to abox_sysTask */
typedef struct {
    uint8_t opcode;     /* ABOX_OP_* */
    uint8_t value;      /* VALUE[3:0] from the lab switches */
} AboxCommand;

/* Unencoded packet request sent from abox_sysTask to abox_txTask */
typedef struct {
    uint8_t type;                               /* ABOX_PKT_* */
    char payload[ABOX_PKT_PAYLOAD_SIZE];        /* ASCII, zero padded */
} AboxTxPacket;

/* Coordinate update sent from abox_sysTask to abox_coordinateTask */
typedef struct {
    AboxAxis axis;
    uint16_t mm;
} AboxCoordMsg;

/* Current head position in mm */
typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t z;
} AboxPosition;

/**
 * @brief  Called from the lab pushbutton EXTI callback. Only queues the
 *         button event for abox_inTask.
 * @param  button Which lab pushbutton was pressed.
 */
void abox_button_from_isr(AboxButton button);

/**
 * @brief  Called from the 1 Hz TIM3 update callback. Only notifies
 *         abox_positionTask.
 */
void abox_position_tick_from_isr(void);

#endif /* ABOX_H */
