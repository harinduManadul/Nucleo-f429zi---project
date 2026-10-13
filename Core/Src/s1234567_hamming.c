/**
  ******************************************************************************
  * @file    s1234567_hamming.c
  * @author  MDS & KB (encoder functions, CSSE3010 sourcelib
  *          common/sxxxxxxx_hamming.c), s1234567 (buffer encoder)
  * @date    08102026
  * @brief   Hamming encoder functions.
  *
  * REFERENCE: hamming_hbyte_encoder and hamming_byte_encoder are provided by
  *            CSSE3010 teaching staff (sourcelib/common/sxxxxxxx_hamming.c).
  *            hamming_buffer_encode written with assistance from Claude
  *            (Anthropic), generative AI tool, 08/10/2026.
  ******************************************************************************
  */

#include "s1234567_hamming.h"

/**
  * Implement Hamming Code + parity checking
  * Hamming code is based on the following generator and parity check matrices
  * G = [ 0 1 1 | 1 0 0 0 ;
  *       1 0 1 | 0 1 0 0 ;
  *       1 1 0 | 0 0 1 0 ;
  *       1 1 1 | 0 0 0 1 ;
  *
  * hence H =
  * [ 1 0 0 | 0 1 1 1 ;
  *   0 1 0 | 1 0 1 1 ;
  *   0 0 1 | 1 1 0 1 ];
  *
  * y = x * G, syn = H * y'
  *
  *
  * NOTE: !! is used to get 1 out of non zeros
  */
uint8_t hamming_hbyte_encoder(uint8_t in) {

	uint8_t d0, d1, d2, d3;
	uint8_t p0 = 0, h0, h1, h2;
	uint8_t z;
	uint8_t out;

	/* extract bits */
	d0 = !!(in & 0x1);
	d1 = !!(in & 0x2);
	d2 = !!(in & 0x4);
	d3 = !!(in & 0x8);

	/* calculate hamming parity bits */
	h0 = d1 ^ d2 ^ d3;
	h1 = d0 ^ d2 ^ d3;
	h2 = d0 ^ d1 ^ d3;

	/* generate out byte without parity bit P0 */
	out = (h0 << 1) | (h1 << 2) | (h2 << 3) |
		(d0 << 4) | (d1 << 5) | (d2 << 6) | (d3 << 7);

	/* calculate even parity bit */
	for (z = 1; z<8; z++)
		p0 = p0 ^ !!(out & (1 << z));

	out |= p0;

	return(out);

}

/*
 * Implement Hamming Code on a full byte of input
 * This means that 16-bits out output is needed
 */
uint16_t hamming_byte_encoder(uint8_t input) {

	uint16_t out;

	/* first encode D0..D3 (first 4 bits),
	 * then D4..D7 (second 4 bits).
	 */
	out = hamming_hbyte_encoder(input & 0xF) |
		(hamming_hbyte_encoder(input >> 4) << 8);

	return(out);

}

/*
 * Encode a buffer of bytes. Each input byte becomes two output bytes:
 * the low nibble's code word first, then the high nibble's.
 */
void hamming_buffer_encode(const uint8_t *raw, uint8_t rawSize,
        uint8_t *encoded) {

	uint8_t i;
	uint16_t word;

	for (i = 0; i < rawSize; i++) {
		word = hamming_byte_encoder(raw[i]);
		encoded[2 * i] = (uint8_t)(word & 0xFF);
		encoded[(2 * i) + 1] = (uint8_t)(word >> 8);
	}
}
