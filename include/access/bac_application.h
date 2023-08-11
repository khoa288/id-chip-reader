/**
 * @author Khoa Nguyen
 * @file bac_application.h
 * @brief Header file for Basic Access Control (BAC) application functions.
 *
 * This header file contains function declarations for performing Basic Access Control (BAC)
 * operations on a smart card, such as selecting applications, getting challenges, and reading
 * data groups.
 */

#pragma once
#ifndef ACCESS_BAC_APPLICATION_H_
#define ACCESS_BAC_APPLICATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

/**
 * @brief Calculate Key Seed for generating Session Key.
 *
 * Given MRZ information, this function calculates the key seed used to generate the session key.
 *
 * @param[in] mrzInformation The MRZ information as an array of unsigned chars.
 * @param[out] mrzKeySeed The calculated key seed as an array of 16 unsigned chars.
 */
void KeySeedCalculate(unsigned char mrzInformation[], unsigned char mrzKeySeed[16]);

/**
 * @brief Generate Session Key from Key Seed.
 *
 * Given a key seed, this function generates the session encryption and MAC keys.
 *
 * @param[in] keySeed The key seed as an array of 16 unsigned chars.
 * @param[out] encryptKeyBuf The generated session encryption key as an array of 16 unsigned chars.
 * @param[out] macKeyBuf The generated session MAC key as an array of 16 unsigned chars.
 */
void SessionKeyGenerate(unsigned char keySeed[16],
						unsigned char encryptKeyBuf[16],
						unsigned char macKeyBuf[16]);

/**
 * @brief Find, connect and start session on reader.
 *
 * Initializes the reader by finding it, connecting to it, and starting a session. Returns a status
 * code indicating success or failure in initializing the reader.
 *
 * @return A long value representing the status code. APP_SUCCESS indicates successful
 initialization, otherwise an error code is returned.
 */
long InitReader(void);

#if USE_NFC
/**
 * @brief Detect card tapped on the reader and connect.
 *
 * Detects the card tapped on the reader and connects to it. Returns a status code indicating
 * success or failure in detecting and connecting to the card.
 *
 * @return A long value representing the status code. APP_SUCCESS indicates successful detection,
		   otherwise an error code is returned.
 */
long DetectCard(void);
#endif

/**
 * @brief Select Application for Basic Access Control.
 *
 * Sends a command to select the BAC application on the smart card. Returns a status code
 * indicating success or failure in selecting the application.
 *
 * @return A long value representing the status code. APP_SUCCESS indicates successful selection,
		   otherwise an error code is returned.
 */
long SelectApplication(void);

/**
 * @brief Get Challenge for Basic Access Control.
 *
 * Requests a challenge from the smart card for BAC authentication. Returns a status code indicating
 * success or failure in getting the challenge * @param[out] getChallengeResponse The received
 challenge as an array of 10 unsigned chars.
 * @param[in] getChallenge of getChallengeResponse array (should be 10).
 *
 * @return A long value representing the status code. APP_SUCCESS indicates successful retrieval,
		   otherwise an error code is returned.
 */
long GetChallenge(unsigned char getChallengeResponse[10], int getChallengeResponseSize);

/**
 * @brief Performs the EXTERNAL AUTHENTICATE operation with the smart card.
 *
 * This function sends an EXTERNAL AUTHENTICATE command to the smart card, which is used for mutual
 * authentication between the card and the application. It computes necessary data, encrypts and
 * decrypts information, verifies received data, and generates session keys (KS_Enc and KS_MAC) as
 * well as the Send Sequence Counter (SSC).
 *
 * @param[in] getChallengeResponse Pointer to a 10-byte array containing the response from a
 * previous GET CHALLENGE command.
 * @param[in] encryptKey Pointer to a 16-byte array containing the encryption key (K_Enc).
 * @param[in] macKey Pointer to a 16-byte array containing the MAC key (K_MAC).
 * @param[out] sessionKeyEncrypt Pointer to a 16-byte array where the generated encryption session
 * key (KS_Enc) will be stored.
 * @param[out] sessionKeyMac Pointer to a 16-byte array where the generated MAC session key (KS_MAC)
 * will be stored.
 * @param[out] sendSequenceCounter Pointer to an 8-byte array where the initialized Send Sequence
 * Counter (SSC) will be stored.
 *
 * @return A long value representing the status code. APP_SUCCESS indicates successful reading,
		   otherwise an error code is returned.
 */
long ExternalAuthenticate(unsigned char getChallengeResponse[10],
						  unsigned char encryptKey[16],
						  unsigned char macKey[16],
						  unsigned char sessionKeyEncrypt[16],
						  unsigned char sessionKeyMac[16],
						  unsigned char sendSequenceCounter[8]);

/**
* @brief External Authenticate to get Session Encrypt Key, MAC Key, and Send Sequence Counter for
Secure Messaging.
*
* Performs external authentication with the smart indicating success or failure in performing
external authentication.
*
* @param[in] getChallengeResponse The received challenge as an array of 10 unsigned chars used in
authentication.
* @param[in] encryptKey The encryption key as an array of 16 unsigned chars used in authentication.
* @param[in] macKey The MAC key as an array of 16 unsigned chars used in authentication get basic
chip's information.
 *
 * Reads the EF.COM data group from card.
 *
 * @return A long value representing the status code. APP_SUCCESS indicates successful reading,
		   otherwise an error code is returned.
 */
long ReadEFCOM(unsigned char sessionKeyEncrypt[16],
			   unsigned char sessionKeyMac[16],
			   unsigned char sendSequenceCounter[8]);

/**
 * @brief Read DG1.COM to get basic holder's information.
 *
 * Reads the DG1 data group from the smart card, which contains basic information about the holder,
 * using secure messaging with provided session keys and send sequence counter. Returns a status
 code
 * indicating success or failure in reading DG1 data group.
 *
 * @param[in] sessionKeyEncrypt The session encryption key as an array of 16 unsigned chars for
 secure messaging.
 * @param[in] sessionKeyMac The session MAC key as an array of 16 unsigned chars for secure
 messaging.
 * @param[in,out] sendSequenceCounter The send sequence counter as an array of 8 unsigned chars for
 secure messaging, updated after each command/response exchange with the smart card.
 *
 * @return A long value representing the status code. APP_SUCCESS indicates successful reading,
		   otherwise an error code is returned.
 */
long ReadDG1(unsigned char sessionKeyEncrypt[16],
			 unsigned char sessionKeyMac[16],
			 unsigned char sendSequenceCounter[8]);

/**
* @brief Read DG2.COM to get holder's image.
*
* Reads the DG2 data group from the smart card, which contains the holder's image (in JPEG format),
* using secure messaging with provided session keys and send sequence counter. Saves the retrieved
* image as a JPEG file named "face_image.jpeg" in the current working directory. Returns a status
* code indicating success or failure in reading DG2
* data group and saving the image file.
*
* @param[in] sessionKeyEncrypt The session encryption key as an array of 16 unsigned chars for
secure messaging.
* @param[in] sessionKeyMac The session MAC key as an array of 16 unsigned chars for secure
messaging.
* @param[in,out] sendSequenceCounter The send sequence counter as an array of 8 unsigned chars for
secure messaging, updated after each command/response exchange with the smart card.
* @param[in] imageFilePath The path to the image file to be saved.
*
* @return A long value representing the status code. APP_SUCCESS indicates successful reading and
saving of the image, otherwise an error code is returned.
*/
long ReadDG2(unsigned char sessionKeyEncrypt[16],
			 unsigned char sessionKeyMac[16],
			 unsigned char sendSequenceCounter[8],
			 unsigned char imageFilePath[]);

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ACCESS_BAC_APPLICATION_H_