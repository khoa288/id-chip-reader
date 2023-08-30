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

/**
 * @brief Converts a character to an integer.
 * @param c The character to be converted.
 * @return The integer value of the character.
 */
int CharToInt(const char c);

/**
 * @brief Converts an integer to a character.
 * @param i The integer to be converted.
 * @return The character value of the integer.
 */
char IntToChar(const int i);

/**
 * @brief Calculates the check digit of a string of data.
 * @param data Pointer to an unsigned char array containing the data.
 * @param length Length of the data.
 * @return The check digit of the data.
 */
int CheckDigitCalculate(const unsigned char* data, int length);

/**
 * @brief Calculates the expiration year of a document.
 * @param birthYear The birth year of the document holder.
 * @param currentYear The current year.
 * @return The expiration year of the document.
 */
int ExpirationYearCalculate(const int birthYear, const int currentYear);

/**
 * @brief Converts a string of 2 characters to an integer representing a year.
 * @param year Pointer to an unsigned char array containing the year.
 * @param currentYear The current year.
 * @return The year represented by the string.
 */
int CharToYear(const unsigned char year[2], const int currentYear);

/**
 * @brief Generates the MRZ information of a document.
 * @param documentNumber Pointer to an unsigned char array containing the document number.
 * @param birthDate Pointer to an unsigned char array containing the birth date in YYMMDD format.
 * @param mrzInformation Pointer to an unsigned char array where the MRZ information will be stored.
 * @param currentYear The current year.
 */
void MrzInformationGenerate(const unsigned char documentNumber[9],
							const unsigned char birthDate[4],
							unsigned char mrzInformation[24],
							int currentYear);

#ifdef __cplusplus
}
#endif

#endif	// #ifndef UTILS_UTIL_H_