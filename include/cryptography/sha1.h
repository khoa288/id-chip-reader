/**
 * @author Khoa Nguyen
 * @file sha1.h
 * @brief Header file for SHA-1 hash algorithm.
 */

#pragma once
#ifndef CRYPTOGRAPHY_SHA1_H_
#define CRYPTOGRAPHY_SHA1_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Computes the SHA-1 hash of the input data and stores the result in the output buffer.
 * @param input Pointer to an unsigned char array containing the data to be hashed.
 * @param length Length of the input data in bytes.
 * @param output Pointer to an unsigned char array where the resulting hash will be stored (should
 * be 40 bytes long).
 */
void sha1(unsigned char* input, long long length, unsigned char* output);

#ifdef __cplusplus
}
#endif

#endif	// #ifndef CRYPTOGRAPHY_SHA1_H_