/**
 * @author Khoa Nguyen
 * @file secure_message.h
 * @brief Secure messaging implementation for communication with a smart card.
 *
 * This file provides the interface for secure messaging between an application
 * and a smart card. It implements protected APDU commands for SELECT and READ BINARY operations,
 * using encryption and MAC calculation to ensure confidentiality and integrity of the
 * communication.
 */

#pragma once
#ifndef ACCESS_SECURE_MESSAGE_H_
#define ACCESS_SECURE_MESSAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Sends a protected SELECT APDU command to the smart card.
 *
 * This function sends a protected SELECT APDU command to the smart card using encryption
 * and MAC calculation to ensure confidentiality and integrity of the communication.
 *
 * @param cmdData File identifier (2 bytes) to be selected by this command.
 * @param sendSequenceCounter Pointer to an 8-byte array representing the current Send Sequence
 * Counter (SSC).
 * @param encryptSessionKey Pointer to a 16-byte array containing the encryption session key
 * (KS_Enc).
 * @param macSessionKey Pointer to a 16-byte array containing the MAC session key (KS_MAC).
 *
 * @return APP_SUCCESS if successful; otherwise, an error code indicating failure reason.
 */
int ProtectedSelectAPDU(unsigned char cmdData[2],
						unsigned char* sendSequenceCounter,
						unsigned char encryptSessionKey[16],
						unsigned char macSessionKey[16]);

/**
 * @brief Sends a protected READ BINARY APDU command to the smart card.
 *
 * This function sends a protected READ BINARY APDU command to the smart card using encryption
 * and MAC calculation to ensure confidentiality and integrity of the communication. The response
 * data will be decrypted and stored in the provided buffer.
 *
 * @param cmdHeader Pointer to a 4-byte array representing the command header for the READ BINARY
 * operation.
 * @param resLen Length of expected response data in bytes.
 * @param responseBuf Pointer to a buffer where the decrypted response data will be stored.
 * @param sendSequenceCounter Pointer to an 8-byte array representing the current Send Sequence
 * Counter (SSC).
 * @param encryptSessionKey Pointer to a 16-byte array containing the encryption session key
 * (KS_Enc).
 * @param macSessionKey Pointer to a 16-byte array containing the MAC session key (KS_MAC).
 *
 * @return APP_SUCCESS if successful; otherwise, an error code indicating failure reason.
 */
int ProtectedReadBinaryAPDU(unsigned char cmdHeader[4],
							unsigned char resLen,
							unsigned char* responseBuf,
							unsigned char* sendSequenceCounter,
							unsigned char encryptSessionKey[16],
							unsigned char macSessionKey[16]);

#ifdef __cplusplus
}
#endif

#endif	// #ifndef ACCESS_SECURE_MESSAGE_H_