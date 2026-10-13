# CSSE3010 Project – ABOX O/X Controller (Nucleo-F429ZI, FreeRTOS)

## Files
| File | Purpose |
|---|---|
| `Core/Src/main.c` | CubeMX peripheral + OS initialisation only |
| `Core/Src/stm32f4xx_it.c` | ISRs; they only signal tasks |
| `Core/Src/freertos.c` | All ABOX tasks: `abox_inTask`, `abox_sysTask`, `abox_txTask`, `abox_coordinateTask`, `abox_positionTask` |
| `Core/Src/s1234567_hamming.c/.h` | Hamming encoder (staff encoder + buffer helper) |
| `Core/Src/s1234567_lta1000g.c/.h` | LED bar driver |
| `Core/Inc/myconfig.h` | Radio channel / ABOX address |
| `Core/Inc/abox_config.h` | Student ID, drawing area, pen heights, timing |
| `Core/Inc/abox.h` | Opcodes, packet types, inter-task message types |
| `Core/Inc/abox_sevenseg.h` | Seven segment helpers (header only) |
| `pins.txt` | Pin usage |

Provided libraries (CSSE3010 sourcelib): `board_printf`, `nrf24l01plus_hal_tx`, `oled_pixel`, `oled_string`, `fonts`.

## Changing the placeholder student ID
1. `Core/Inc/abox_config.h`: set `ABOX_STUDENT_ID` (BCD, e.g. 45678910 → `0x45678910U`).
2. Rename `s1234567_hamming.*` and `s1234567_lta1000g.*` to your number and update the `#include` lines and the `s1234567_lta1000g_*` function names.
3. Update the `@author` tags.

## Assumptions (edge conditions, spec §12)
- Origin [0,0,0] is the bottom-left corner; all coordinates are absolute and in mm.
- Z = 0 mm is pen down (drawing), Z = 10 mm is pen up.
- Drawing area is taken as 200 × 200 mm (`ABOX_X_MAX_MM`, `ABOX_Y_MAX_MM`); moves are clamped to it.
- Sender address is sent most significant byte first (0x45, 0x67, 0x89, 0x10 for 45678910), following the spec example. Set `ABOX_ADDR_MSB_FIRST` to 0 for least significant byte first.
- X/Y/Z commands move that axis to VALUE mm (absolute); the other axes are unchanged.
- Line moves with the pen down from the current position to (VALUE[1:0]×20, VALUE[3:2]×20).
- Naught and Cross are drawn centred on the current head position; size is VALUE[2:0]×20 mm. The circle is a 16-segment polygon. The pen is lifted for travel and after each shape, and the head returns to the centre.
- Packets other than JOIN are not sent until JOIN (opcode 0x0F) has been executed.
- The system waits `ABOX_MOVE_MS_PER_MM` per mm moved (minimum `ABOX_MOVE_MIN_MS`) before sending the next point, because the radio link is one-way.
- Opcodes without a design-task function (0x08, 0x09, 0x0A–0x0E until challenges are added) do nothing.
- The OLED is 128×32, so the position marker is scaled from the drawing area to 128×32 pixels.

## Generative AI referencing
Files written with help from Claude (Anthropic) carry a `REFERENCE:` comment in their header.
