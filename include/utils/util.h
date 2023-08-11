/**
 * @author Khoa Nguyen
 * @file util.h
 * @brief Header file for utility functions.
 */

#pragma once
#ifndef UTILS_UTIL_H_
#define UTILS_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Delays the execution of the program for a specified number of milliseconds.
 * @param millisecond The number of milliseconds to delay the program.
 */
void Delay(int millisecond);

/**
 * @brief Generates a random nonce and stores it in the provided buffer.
 * @param buffer Pointer to an unsigned char array where the generated nonce will be stored.
 * @param len Length of the buffer, determines how many bytes of nonce will be generated.
 */
void RandomNonceGenerate(unsigned char* buffer, int len);

/**
 * @brief Pads a byte array with zeros from a specified starting position to the end.
 * @param byteArray Pointer to an unsigned char array that needs to be padded with zeros.
 * @param startPosition The position in the byteArray from which padding should start.
 */
void PadByteArray(unsigned char* byteArray, int startPosition);

#ifdef __cplusplus
}
#endif

#endif	// #ifndef UTILS_UTIL_H_