/**
 * @author Khoa Nguyen
 * @file mac3.c
 * @brief source file for ISO 9797 MAC algorithm 3 using DES encryption.
 *
 * This source file implements the function for calculating the checksum using the ISO 9797 MAC
 * algorithm 3 with DES encryption.
 */

#include <string.h>

#include <cryptography/des.h>

void des_mac3_checksum(int length, unsigned char buff[8], unsigned char* data, unsigned char* key) {
	// Separate Key
	unsigned char key1[8], key2[8];
	memcpy(key1, key, 8);
	memcpy(key2, &key[8], 8);

	// Initialize
	unsigned char blockh[400];
	des_ecb_encrypt(blockh, data, 8, key1);
	for (int i = 0; i < 8; i++) {
		data[i + 8] = data[i + 8] ^ blockh[i];
	}

	// Iteration
	for (int i = 0; i < length / 8 - 2; i++) {
		des_ecb_encrypt(&blockh[8 * (i + 1)], &data[8 * (i + 1)], 8, key1);
		for (int j = 0; j < 8; j++) {
			data[8 * (i + 2) + j] = data[8 * (i + 2) + j] ^ blockh[8 * (i + 1) + j];
		}
	}

	// Output Transformation 3
	des3_cbc_encrypt(buff, &data[length - 8], 8, key, 16, 0);
}