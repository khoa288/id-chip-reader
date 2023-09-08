/**
 * @author Khoa Nguyen
 * @file secure_message.c
 * @brief Secure messaging implementation for communication with a smart card.
 *
 * This file provides the implementation for secure messaging between an application
 * and a smart card. It implements protected APDU commands for SELECT and READ BINARY operations,
 * using encryption and MAC calculation and integrity of the communication.
 */

#include <stdio.h>
#include <string.h>

#include <cryptography/des.h>
#include <cryptography/mac3.h>
#include <utils/reader.h>
#include <utils/util.h>

inline void IncreaseUnsignedCharByOne(unsigned char* hexArray, int len) {
	int lastIndex = len - 1;
	while (hexArray[lastIndex] == 255) {
		hexArray[lastIndex] = 0;
		lastIndex -= 1;
	}
	hexArray[lastIndex] += 1;
}

int ProtectedSelectAPDU(unsigned char cmdData[2],
						unsigned char* sendSequenceCounter,
						unsigned char encryptSessionKey[16],
						unsigned char macSessionKey[16]) {
	// Padding CmdHeader
	unsigned char selectCmdHeader[4] = {0x0C, 0xA4, 0x02, 0x0C};
	unsigned char cmdHeader[8];
	memcpy(cmdHeader, selectCmdHeader, 4);
	PadByteArray(cmdHeader, 4);

	// Padding Data
	unsigned char padData[8];
	memcpy(padData, cmdData, 2);
	PadByteArray(padData, 2);

	// Encrypt data with KS_Enc
	unsigned char encryptData[8];
	des3_cbc_encrypt(encryptData, padData, 8, encryptSessionKey, 16, 0);

	// Build DO'87'
	unsigned char dataObject87[11] = {0x87, 0x09, 0x01};
	memcpy(&dataObject87[3], encryptData, 8);

	// M = CmdHeader || DO'87'
	unsigned char concatM[19];
	memcpy(concatM, cmdHeader, 8);
	memcpy(&concatM[8], dataObject87, 11);

	// Increment SSC with 1
	IncreaseUnsignedCharByOne(sendSequenceCounter, 8);

	// N = SSC || M || Padding
	unsigned char concatN[32];
	memcpy(concatN, sendSequenceCounter, 8);
	memcpy(&concatN[8], concatM, 19);
	PadByteArray(concatN, 27);

	// Compute MAC of M
	unsigned char mac[8];  // CC
	des_mac3_checksum(32, mac, concatN, macSessionKey);

	// Build DO'8E'
	unsigned char dataObject8E[10] = {0x8E, 0x08};
	memcpy(&dataObject8E[2], mac, 8);

	// Construct protected APDU
	unsigned char protectedAPDU[27];
	memcpy(protectedAPDU, cmdHeader, 4);  // Header
	protectedAPDU[4] = 0x15;			  // Lc'
	protectedAPDU[5] = 0x87;
	protectedAPDU[6] = 0x09;
	protectedAPDU[7] = 0x01;					   // '87' L '01'
	memcpy(&protectedAPDU[8], encryptData, 8);	   // Encrypted data
	memcpy(&protectedAPDU[16], dataObject8E, 10);  // DO'8E'
	protectedAPDU[26] = 0x00;

	// Send protected APDU
	unsigned char protectedResponse[16];  // RAPDU
	unsigned long protectedResponseLength = sizeof(protectedResponse);
	int ret = TransmitDataToCard(protectedAPDU, sizeof(protectedAPDU), protectedResponse,
								 &protectedResponseLength);
	if (ret != APP_SUCCESS) {
		printf("Fail to Send protected APDU.\n");
		return ret;
	}

	// Verify RAPDU CC by computing MAC of DO'99'
	// Increment SSC with 1
	IncreaseUnsignedCharByOne(sendSequenceCounter, 8);

	// Concatenate SSC and DO'99' and add padding
	unsigned char concatK[16];	// K
	memcpy(concatK, sendSequenceCounter, 8);
	memcpy(&concatK[8], protectedResponse, 4);
	PadByteArray(concatK, 12);

	// Compute MAC with KS_MAC
	unsigned char macCheck[8];	// CC'
	des_mac3_checksum(16, macCheck, concatK, macSessionKey);

	// Compare CC' with data of DO'8E' of RAPDU
	if (memcmp(macCheck, &protectedResponse[6], 8)) {
		printf("Invalid Response APDU.\n");
		return APP_ERROR;
	}
	return APP_SUCCESS;
}

int ProtectedReadBinaryAPDU(unsigned char cmdHeader[4],
							unsigned char resLen,
							unsigned char* responseBuf,
							unsigned char* sendSequenceCounter,
							unsigned char encryptSessionKey[16],
							unsigned char macSessionKey[16]) {
	// Padding CmdHeader
	unsigned char padCmdHeader[8];
	memcpy(padCmdHeader, cmdHeader, 4);
	PadByteArray(padCmdHeader, 4);

	// Build DO'97'
	unsigned char dataObject97[3] = {0x97, 0x01, resLen};

	// Concatenate CmdHeader and DO'97'
	unsigned char concatM[11];
	memcpy(concatM, padCmdHeader, 8);
	memcpy(&concatM[8], dataObject97, 3);

	// Increment SSC with 1
	IncreaseUnsignedCharByOne(sendSequenceCounter, 8);

	// N = SSC || M || Padding
	unsigned char concatN[24];
	memcpy(concatN, sendSequenceCounter, 8);
	memcpy(&concatN[8], concatM, 11);
	PadByteArray(concatN, 19);

	// Compute MAC of M
	unsigned char mac[8];  // CC
	des_mac3_checksum(24, mac, concatN, macSessionKey);

	// Build DO'8E'
	unsigned char dataObject8E[10] = {0x8E, 0x08};
	memcpy(&dataObject8E[2], mac, 8);

	// Construct protected APDU
	unsigned char protectedAPDU[19];
	memcpy(protectedAPDU, padCmdHeader, 4);	 // Header
	protectedAPDU[4] = 0x0D;				 // Lc'
	memcpy(&protectedAPDU[5], dataObject97, 3);
	memcpy(&protectedAPDU[8], dataObject8E, 10);
	protectedAPDU[18] = 0x00;

	// Send protected APDU
	unsigned char res[285];
	unsigned long protectedResponseLength = sizeof(res);
	int ret =
		TransmitDataToCard(protectedAPDU, sizeof(protectedAPDU), res, &protectedResponseLength);
	if (ret != APP_SUCCESS) {
		printf("Fail to Send protected APDU.\n");
		return ret;
	}

	// Count
	int responseLen = 0;
	if (res[1] == 0x82) {
		// File can be read more
		responseLen = 285 - 5 - 16;
		des3_cbc_decrypt(responseBuf, &res[5], responseLen, encryptSessionKey, 16, 0);
	} else if (res[2] == 0x01) {
		for (int i = 3; i < 280; i++) {
			if (res[i] == 0x99 && res[i + 1] == 0x02 && res[i + 2] == 0x90) {
				break;
			}
			responseLen++;
		}
		// Decrypt
		des3_cbc_decrypt(responseBuf, &res[3], responseLen, encryptSessionKey, 16, 0);
	} else if (res[3] = 0x01) {
		for (int i = 4; i < 280; i++) {
			if (res[i] == 0x99 && res[i + 1] == 0x02 && res[i + 2] == 0x90) {
				break;
			}
			responseLen++;
		}
		// Decrypt
		des3_cbc_decrypt(responseBuf, &res[4], responseLen, encryptSessionKey, 16, 0);
	}

	// Increment SSC with 1
	IncreaseUnsignedCharByOne(sendSequenceCounter, 8);

	return APP_SUCCESS;
}
