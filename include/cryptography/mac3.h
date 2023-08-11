/**
 * @author Khoa Nguyen
 * @file mac3.h
 * @brief Header file for ISO 9797 MAC algorithm 3 using DES encryption.
 *
 * This header file provides an API for calculating the checksum using the ISO 9797 MAC algorithm 3
 * with DES encryption.
 */

#pragma once
#ifndef CRYPTOGRAPHY_MAC3_H_
#define CRYPTOGRAPHY_MAC3_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Calculates ISO 9797 MAC algorithm 3 using DES encryption with Padding Method 2, Initial
 * Transformation 1, and Output transformation 3.
 *
 * @param length Length of the input data.
 * @param buff Buffer to store the calculated checksum (8 bytes).
 * @param data Input data for which the checksum is to be calculated.
 * @param key Encryption key used in the calculation (16 bytes).
 */
void des_mac3_checksum(int length, unsigned char buff[8], unsigned char* data, unsigned char* key);

#ifdef __cplusplus
}
#endif

#endif	// #ifndef CRYPTOGRAPHY_MAC3_H_