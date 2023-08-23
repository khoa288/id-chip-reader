/**
 * @author Khoa Nguyen
 * @file chip_reader.c
 * @brief Source file for chip reader functions.
 *
 * This source file contains the implementation of a function for reading data from an ID card chip
 * using Basic Access Control (BAC) application functions. The ReadIdCardChip function is designed
 * to work with a smart card reader and a corresponding smart card that supports BAC protocol.
 */

#include <access/bac_application.h>
#include <chip_reader.h>
#include <utils/reader.h>

long ReadIdCardChip(unsigned char mrzInformation[], unsigned char imageFilePath[]) {
	long res = InitReader();
	if (res != APP_SUCCESS) {
		return res;
	}

#if USE_NFC
	res = DetectCard();
	if (res != APP_SUCCESS) {
		return res;
	}
#endif	// #if USE_NFC

	res = SelectApplication();
	if (res != APP_SUCCESS) {
		return res;
	}

	unsigned char getChallengeResponse[10];
	res = GetChallenge(getChallengeResponse, sizeof(getChallengeResponse));
	if (res != APP_SUCCESS) {
		return res;
	}

	unsigned char mrzKeySeed[16];
	KeySeedCalculate(mrzInformation, mrzKeySeed);

	unsigned char encryptKey[16], macKey[16];
	SessionKeyGenerate(mrzKeySeed, encryptKey, macKey);

	unsigned char sessionKeyEncrypt[16], sessionKeyMac[16], sendSequenceCounter[8];
	res = ExternalAuthenticate(getChallengeResponse, encryptKey, macKey, sessionKeyEncrypt,
							   sessionKeyMac, sendSequenceCounter);
	if (res != APP_SUCCESS) {
		return res;
	}

	res = ReadEFCOM(sessionKeyEncrypt, sessionKeyMac, sendSequenceCounter);
	if (res != APP_SUCCESS) {
		return res;
	}

	res = ReadDG1(sessionKeyEncrypt, sessionKeyMac, sendSequenceCounter);
	if (res != APP_SUCCESS) {
		return res;
	}

	res = ReadDG2(sessionKeyEncrypt, sessionKeyMac, sendSequenceCounter, imageFilePath);

	return res;
}