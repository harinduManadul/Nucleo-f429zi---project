/**
 ******************************************************************************
 * @file    s1234567_hamming.h
 * @author  s1234567
 * @date    08102026
 * @brief   Hamming (7,4) + even parity encoder for radio packets.
 ******************************************************************************
 */

#ifndef S1234567_HAMMING_H
#define S1234567_HAMMING_H

#include <stdint.h>

/**
 * @brief  Encode a 4-bit half byte into an 8-bit Hamming + parity byte.
 * @param  in Half byte to encode (only bits 3:0 are used).
 * @retval Encoded byte.
 */
uint8_t hamming_hbyte_encoder(uint8_t in);

/**
 * @brief  Encode a full byte; low nibble goes in the low output byte.
 * @param  input Byte to encode.
 * @retval 16-bit encoded value.
 */
uint16_t hamming_byte_encoder(uint8_t input);

/**
 * @brief  Encode a buffer, writing two encoded bytes per input byte
 *         (low nibble's code first).
 * @param  raw       Input bytes.
 * @param  rawSize   Number of input bytes.
 * @param  encoded   Output buffer, at least 2 * rawSize bytes.
 */
void hamming_buffer_encode(const uint8_t *raw, uint8_t rawSize,
        uint8_t *encoded);

#endif /* S1234567_HAMMING_H */
