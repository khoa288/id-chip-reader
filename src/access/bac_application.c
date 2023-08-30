/**
 * @author Khoa Nguyen
 * @file bac_application.c
 * @brief Source file for Basic Access Control (BAC) application functions.
 *
 * This source file contains the implementation of functions for performing Basic Access Control
 * (BAC) operations on a smart card, such as selecting applications, getting challenges, and reading
 * data groups. The functions in this file are designed to work with a smart card reader and the
 * corresponding smart card that supports BAC protocol.
 */

#include <stdio.h>
#include <string.h>

#include <access/bac_application.h>
#include <access/secure_message.h>
#include <cryptography/des.h>
#include <cryptography/mac3.h>
#include <cryptography/sha1.h>
#include <utils/reader.h>
#include <utils/util.h>

void KeySeedCalculate(unsigned char mrzInformation[], unsigned char mrzKeySeed[16]) {
	// Kseed = first 16 bytes of the hash
	unsigned char mrzInformationDigest[20];
	sha1(mrzInformation, (int)strlen((char*)mrzInformation), mrzInformationDigest);
	memcpy(mrzKeySeed, mrzInformationDigest, 16);
}

void SessionKeyGenerate(unsigned char keySeed[16],
						unsigned char encryptKeyBuf[16],
						unsigned char macKeyBuf[16]) {
	unsigned char d1[20], d2[20], d1digest[20], d2digest[20];

	// D = Kseed || c (c = �00000001� for KEnc and c = �00000002� for KMAC)
	memcpy(d1, keySeed, 16);
	memset(&d1[16], 0x00, 3);
	d1[19] = 0x01;
	memcpy(d2, keySeed, 16);
	memset(&d2[16], 0x00, 3);
	d2[19] = 0x02;

	// 3DES key KEnc and KMAC = first 16 bytes of each hash
	sha1((unsigned char*)d1, 20, d1digest);
	sha1((unsigned char*)d2, 20, d2digest);

	memcpy(encryptKeyBuf, d1digest, 16);
	memcpy(macKeyBuf, d2digest, 16);
}

long InitReader(void) {
	long ret = InitializeReader();
	if (ret != APP_SUCCESS) {
		printf("Fail to Initialize Reader.\n");
		return ret;
	}

	// Anti collision
	Delay(200);
	return APP_SUCCESS;
}

#if USE_NFC
long DetectCard(void) {
	long ret = DetectFeliCaCard();
	if (ret != APP_SUCCESS) {
		printf("Fail to Detect Card.\n");
		return ret;
	}
	// Anti collision
	Delay(200);
	return APP_SUCCESS;
}
#endif	// #if USE_NFC

long SelectApplication(void) {
	// (AID for BAC application)
	// Expected response APDU : 0x90 || 0x00
	const unsigned char selectApplicationCommand[] = {0x00, 0xA4, 0x04, 0x00, 0x07, 0xA0,
													  0x00, 0x00, 0x02, 0x47, 0x10, 0x01};
	unsigned char selectApplicationResponse[2];
	unsigned long selectApplicationResponseLength = sizeof(selectApplicationResponse);
	long ret = TransmitDataToCard(selectApplicationCommand, sizeof(selectApplicationCommand),
								  selectApplicationResponse, &selectApplicationResponseLength);
	if (ret != APP_SUCCESS) {
		printf("Fail to Select Application.\n");
		return ret;
	}
	return APP_SUCCESS;
}

long GetChallenge(unsigned char getChallengeResponse[10], int getChallengeResponseSize) {
	// Expected response APDU: RND.IC (8 bytes) || 0x90 || 0x00
	const unsigned char getChallengeCommand[] = {0x00, 0x84, 0x00, 0x00, 0x08};
	long ret = TransmitDataToCard(getChallengeCommand, sizeof(getChallengeCommand),
								  getChallengeResponse, &getChallengeResponseSize);
	if (ret != APP_SUCCESS) {
		printf("Fail to Get Challenge.\n");
		return ret;
	}
	return APP_SUCCESS;
}

long ExternalAuthenticate(unsigned char getChallengeResponse[10],
						  unsigned char encryptKey[16],
						  unsigned char macKey[16],
						  unsigned char sessionKeyEncrypt[16],
						  unsigned char sessionKeyMac[16],
						  unsigned char sendSequenceCounter[8]) {
	// Compute data for EXTERNAL AUTHENTICATE
	// RND.IC
	unsigned char challenge[8];
	memcpy(challenge, getChallengeResponse, 8);

	// RND.IFD
	unsigned char randomNonceIFD[8];
	RandomNonceGenerate((unsigned char*)randomNonceIFD, 8);

	// K_IFD
	unsigned char keyIFD[16];
	RandomNonceGenerate((unsigned char*)keyIFD, 16);

	// S = RND.IFD || RND.IC || K_IFD
	unsigned char concatS[32];
	memcpy(concatS, randomNonceIFD, 8);
	memcpy(&concatS[8], challenge, 8);
	memcpy(&concatS[16], keyIFD, 16);

	// E_IFD = encrypt S with 3DES key K_Enc
	unsigned char encryptIFD[32];
	des3_cbc_encrypt(encryptIFD, concatS, 32, encryptKey, 16, 0);

	// M_IFD = (MAC algorithm 3, DES cipher, Padding method 2) of E_IFD
	unsigned char macIFD[8];
	unsigned char paddedEncryptIFD[40];
	memcpy(paddedEncryptIFD, encryptIFD, 32);
	PadByteArray(paddedEncryptIFD, 32);
	des_mac3_checksum(40, macIFD, paddedEncryptIFD, macKey);

	// cmd_data = E_IFD || M_IFD
	unsigned char externalAuthenticateCommandData[40];
	memcpy(externalAuthenticateCommandData, encryptIFD, 32);
	memcpy(&externalAuthenticateCommandData[32], macIFD, 8);

	// External Authenticate APDU
	unsigned char externalAuthenticateResponse[42];
	unsigned long externalAuthenticateResponseLength = sizeof(externalAuthenticateResponse);

	unsigned char externalAuthenticateCommand[46] = {0x00, 0x82, 0x00, 0x00, 0x28};
	memcpy(&externalAuthenticateCommand[5], externalAuthenticateCommandData, 40);
	externalAuthenticateCommand[45] = 0x28;

	long ret =
		TransmitDataToCard(externalAuthenticateCommand, sizeof(externalAuthenticateCommand),
						   externalAuthenticateResponse, &externalAuthenticateResponseLength);
	if (ret != APP_SUCCESS) {
		printf("Fail to External Authenticate.\n");
		return ret;
	}

	// Verify response
	unsigned char encryptIC[32], macIC[8];	// E_IC, M_IC
	memcpy(encryptIC, externalAuthenticateResponse, 32);
	memcpy(macIC, &externalAuthenticateResponse[32], 8);

	// Verify E_IC with M_IC
	unsigned char macCheckIC[8];
	unsigned char paddedMacCheckIC[40];
	memcpy(paddedMacCheckIC, encryptIC, 32);
	PadByteArray(paddedMacCheckIC, 32);
	des_mac3_checksum(40, macCheckIC, paddedMacCheckIC, macKey);

	if (memcmp(macIC, macCheckIC, 8)) {
		printf("Invalid External Authenticate response.\n");
		return APP_ERROR;
	}

	// Decrypt E_IC to get R = RND.IC || RND.IFD || K.IC
	unsigned char concatR[32];
	des3_cbc_decrypt(concatR, encryptIC, 32, encryptKey, 16, 0);

	// Compare received RND.IFD with generated RND.IFD
	unsigned char randomNonceIFDCheck[8];
	memcpy(randomNonceIFDCheck, &concatR[8], 8);
	if (memcmp(randomNonceIFD, randomNonceIFDCheck, 8)) {
		printf("Invalid Random Nonce Data received.\n");
		return APP_ERROR;
	}

	// K_seed = K_IFD XOR K_IC
	unsigned char keyIC[16], sessionKeySeed[16];
	memcpy(keyIC, &concatR[16], 16);
	for (int i = 0; i < 16; i++) {
		sessionKeySeed[i] = keyIFD[i] ^ keyIC[i];
	}

	// KS_Enc, KS_MAC
	SessionKeyGenerate(sessionKeySeed, sessionKeyEncrypt, sessionKeyMac);

	// SSC = RND.IC (4 least significant bytes) || RND.IFD (4 least significant bytes)
	// SSC += 1 every time before a command or response APDU is generated
	memcpy(sendSequenceCounter, &challenge[4], 4);
	memcpy(&sendSequenceCounter[4], &randomNonceIFD[4], 4);

	return APP_SUCCESS;
}

long ReadEFCOM(unsigned char sessionKeyEncrypt[16],
			   unsigned char sessionKeyMac[16],
			   unsigned char sendSequenceCounter[8]) {
	// Construct protected APDU command to Select EF.COM
	// Unprotected command: 0x00, 0xA4, 0x02, 0x0C, 0x02, 0x01, 0x1E
	const unsigned char selectEFCOMCmdData[2] = {0x01, 0x1E};
	int ret = ProtectedSelectAPDU(selectEFCOMCmdData, sendSequenceCounter, sessionKeyEncrypt,
								  sessionKeyMac);
	if (ret != APP_SUCCESS) {
		printf("Fail to Select EF.COM.\n");
		return ret;
	}

	// Read Binary 26 bytes of EF.COM
	// Unprotected command APDU: 0x00, 0xB0, 0x00, 0x00, 0x04
	const unsigned char readBinaryEFCOMCmdHeader[4] = {0x0C, 0xB0, 0x00, 0x00};
	unsigned char readBinaryEFCOMResponse[32];
	ret = ProtectedReadBinaryAPDU(readBinaryEFCOMCmdHeader, 0x1A, readBinaryEFCOMResponse,
								  sendSequenceCounter, sessionKeyEncrypt, sessionKeyMac);
	if (ret != APP_SUCCESS) {
		printf("Fail to Read Binary of EF.COM.\n");
		return ret;
	}

#if DEBUG
	printf("EF.COM: ");
	for (int i = 0; i < 32; i++) {
		printf("%02X ", readBinaryEFCOMResponse[i]);
	}
	printf("\n");
#endif

	printf("\n> LDS Version number: ");
	for (int i = 0; i < 4; i++) {
		printf("%c", readBinaryEFCOMResponse[i + 5]);
	}
	printf("\n> Unicode Version: ");
	for (int i = 0; i < 6; i++) {
		printf("%c", readBinaryEFCOMResponse[i + 12]);
	}
	printf("\n> Tag list: ");
	for (int i = 0; i < readBinaryEFCOMResponse[19]; i++) {
		printf("%02X ", readBinaryEFCOMResponse[i + 20]);
	}
	printf("\n");

	return APP_SUCCESS;
}

long ReadDG1(unsigned char sessionKeyEncrypt[16],
			 unsigned char sessionKeyMac[16],
			 unsigned char sendSequenceCounter[8]) {
	// Construct protected APDU command to Select DG1
	// Unprotected command: 0x00, 0xA4, 0x02, 0x0C, 0x02, 0x01, 0x01
	const unsigned char selectDataGroup1CmdData[2] = {0x01, 0x01};
	int ret = ProtectedSelectAPDU(selectDataGroup1CmdData, sendSequenceCounter, sessionKeyEncrypt,
								  sessionKeyMac);
	if (ret != APP_SUCCESS) {
		printf("Fail to Select DG1.\n");
		return ret;
	}

	// Read Binary of 95 bytes of DG1
	// Unprotected command APDU: 0x00, 0xB0, 0x00, 0x00, 0x04
	const unsigned char readBinaryDataGroup1CmdHeader[4] = {0x0C, 0xB0, 0x00, 0x00};
	unsigned char readBinaryDataGroup1Response[96];	 // 95 bytes with padding
	int ret =
		ProtectedReadBinaryAPDU(readBinaryDataGroup1CmdHeader, 0x5F, readBinaryDataGroup1Response,
								sendSequenceCounter, sessionKeyEncrypt, sessionKeyMac);
	if (ret != APP_SUCCESS) {
		printf("Fail to Read Binary of DG1.\n");
		return ret;
	}

#if DEBUG
	printf("DG1: ");
	for (int i = 0; i < 96; i++) {
		printf("%02X ", readBinaryDataGroup1Response[i]);
	}
	printf("\n");
#endif	// #if DEBUG

	printf("\n> Document code: ");
	for (int i = 0; i < 2; i++) {
		printf("%c", readBinaryDataGroup1Response[i + 5]);
	}
	printf("\n> Issuing State or Organization: ");
	for (int i = 0; i < 3; i++) {
		printf("%c", readBinaryDataGroup1Response[i + 7]);
	}
	printf("\n> Document number: ");
	for (int i = 0; i < 9; i++) {
		printf("%c", readBinaryDataGroup1Response[i + 10]);
	}
	// Optional data and /or in the case of a Document Number exceeding nine characters, least
	// significant characters of document number plus document number check digit plus filler
	// character
	printf("\n> Remaining of Document number: ");
	for (int i = 0; i < 15; i++) {
		printf("%c", readBinaryDataGroup1Response[i + 20]);
	}
	printf("\n> Date of birth: ");
	for (int i = 0; i < 6; i++) {
		printf("%c", readBinaryDataGroup1Response[i + 35]);
	}
	printf("\n> Sex: ");
	for (int i = 0; i < 1; i++) {
		printf("%c", readBinaryDataGroup1Response[i + 42]);
	}
	printf("\n> Date of Expiry: ");
	for (int i = 0; i < 6; i++) {
		printf("%c", readBinaryDataGroup1Response[i + 43]);
	}
	printf("\n> Nationality: ");
	for (int i = 0; i < 3; i++) {
		printf("%c", readBinaryDataGroup1Response[i + 50]);
	}
	printf("\n> Name of holder: ");
	for (int i = 0; i < 30; i++) {
		printf("%c", readBinaryDataGroup1Response[i + 65]);
	}
	printf("\n");

	return APP_SUCCESS;
}

long ReadDG2(unsigned char sessionKeyEncrypt[16],
			 unsigned char sessionKeyMac[16],
			 unsigned char sendSequenceCounter[8],
			 unsigned char imageFilePath[]) {
	// Construct protected APDU command to Select DG2
	// Unprotected command: 0x00, 0xA4, 0x02, 0x0C, 0x02, 0x01, 0x02
	const unsigned char selectDataGroup2CmdData[2] = {0x01, 0x02};
	int ret = ProtectedSelectAPDU(selectDataGroup2CmdData, sendSequenceCounter, sessionKeyEncrypt,
								  sessionKeyMac);
	if (ret != APP_SUCCESS) {
		printf("Fail to Select DG2.\n");
		return ret;
	}

	// Open Image file
	FILE* ptr;
	fopen_s(&ptr, imageFilePath, "wb");
	if (ptr == NULL) {
		printf("Error opening file!\n");
		return APP_ERROR;
	}

	// Read Binary First 256 bytes of DG2
	unsigned char readBinaryDataGroup2CmdHeader[4] = {0x0C, 0xB0, 0x00, 0x00};
	unsigned char tempDataGroup2Buffer[264],
		dataGroup2Buffer[4];  // DG2 length = dataGroup2Buffer[2] * 16^2 + dataGroup2Buffer[3]
	ret = ProtectedReadBinaryAPDU(readBinaryDataGroup2CmdHeader, (unsigned char)256,
								  tempDataGroup2Buffer, sendSequenceCounter, sessionKeyEncrypt,
								  sessionKeyMac);
	if (ret != APP_SUCCESS) {
		printf("Fail to Read Binary of DG2.\n");
		fclose(ptr);
		return ret;
	}

	// Find jpeg header
	int jpegHeader = 0;
	for (int i = 0; i < 256; i++) {
		if (tempDataGroup2Buffer[i] == 0xFF && tempDataGroup2Buffer[i + 1] == 0xD8 &&
			tempDataGroup2Buffer[i + 2] == 0xFF && tempDataGroup2Buffer[i + 3] == 0xE0) {
			break;
		}
		jpegHeader++;
	}
	fwrite(&tempDataGroup2Buffer[jpegHeader], 256 - jpegHeader, 1, ptr);
	memcpy(dataGroup2Buffer, tempDataGroup2Buffer, 4);

	// Read Binary remaining bytes of DG2
	int indexDG2;
	for (indexDG2 = 1; indexDG2 < dataGroup2Buffer[2]; indexDG2++) {
		readBinaryDataGroup2CmdHeader[2] = indexDG2;
		ret = ProtectedReadBinaryAPDU(readBinaryDataGroup2CmdHeader, (unsigned char)256,
									  tempDataGroup2Buffer, sendSequenceCounter, sessionKeyEncrypt,
									  sessionKeyMac);
		if (ret != APP_SUCCESS) {
			printf("Fail to Read Binary of DG2.\n");
			fclose(ptr);
			return ret;
		}
		fwrite(&tempDataGroup2Buffer[0], 256, 1, ptr);
	}

	// Read Binary last bytes of DG2
	readBinaryDataGroup2CmdHeader[2] = indexDG2;
	ret = ProtectedReadBinaryAPDU(readBinaryDataGroup2CmdHeader, dataGroup2Buffer[3],
								  tempDataGroup2Buffer, sendSequenceCounter, sessionKeyEncrypt,
								  sessionKeyMac);
	if (ret != APP_SUCCESS) {
		printf("Fail to Read Binary of DG2.\n");
		fclose(ptr);
		return ret;
	}
	fwrite(&tempDataGroup2Buffer[0], dataGroup2Buffer[3], 1, ptr);

	// Close Image file
	fclose(ptr);

	return APP_SUCCESS;
}