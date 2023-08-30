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
#include <utils/util.h>

long ReadIdCardChip(unsigned char mrzInformation[], unsigned char imageFilePath[]) {
	long res = InitReader();
	if (res != APP_SUCCESS) {
		goto end;
	}

#if USE_NFC
	res = DetectCard();
	if (res != APP_SUCCESS) {
		goto end;
	}
#endif	// #if USE_NFC

	res = SelectApplication();
	if (res != APP_SUCCESS) {
		goto end;
	}

	unsigned char getChallengeResponse[10];
	res = GetChallenge(getChallengeResponse, sizeof(getChallengeResponse));
	if (res != APP_SUCCESS) {
		goto end;
	}

	unsigned char mrzKeySeed[16];
	KeySeedCalculate(mrzInformation, mrzKeySeed);

	unsigned char encryptKey[16], macKey[16];
	SessionKeyGenerate(mrzKeySeed, encryptKey, macKey);

	unsigned char sessionKeyEncrypt[16], sessionKeyMac[16], sendSequenceCounter[8];
	res = ExternalAuthenticate(getChallengeResponse, encryptKey, macKey, sessionKeyEncrypt,
							   sessionKeyMac, sendSequenceCounter);
	if (res != APP_SUCCESS) {
		goto end;
	}

	res = ReadEFCOM(sessionKeyEncrypt, sessionKeyMac, sendSequenceCounter);
	if (res != APP_SUCCESS) {
		goto end;
	}

	res = ReadDG1(sessionKeyEncrypt, sessionKeyMac, sendSequenceCounter);
	if (res != APP_SUCCESS) {
		goto end;
	}

	res = ReadDG2(sessionKeyEncrypt, sessionKeyMac, sendSequenceCounter, imageFilePath);
	if (res != APP_SUCCESS) {
		goto end;
	}

end:
	DisconnectFeliCaCard();
	DisconnectReader();
	return res;
}

long ReadIdCardChipWithDocumentNumber(unsigned char documentNumber[9],
									  unsigned char imageFilePath[]) {
	long res = InitReader();
	if (res != APP_SUCCESS) {
		goto end;
	}

#if USE_NFC
	res = DetectCard();
	if (res != APP_SUCCESS) {
		goto end;
	}
#endif	// #if USE_NFC

	res = SelectApplication();
	if (res != APP_SUCCESS) {
		goto end;
	}

	for (int month = 8; month <= 8; month++) {
		for (int day = 1; day <= 31; day++) {
			unsigned char getChallengeResponse[10];
			GetChallenge(getChallengeResponse, sizeof(getChallengeResponse));
			unsigned char mrzInformation[24];
			unsigned char birthDate[4] = {IntToChar(month / 10), IntToChar(month % 10),
										  IntToChar(day / 10), IntToChar(day % 10)};
			MrzInformationGenerate(documentNumber, birthDate, mrzInformation, 2023);

			unsigned char mrzKeySeed[16];
			KeySeedCalculate(mrzInformation, mrzKeySeed);

			unsigned char encryptKey[16], macKey[16];
			SessionKeyGenerate(mrzKeySeed, encryptKey, macKey);

			unsigned char sessionKeyEncrypt[16], sessionKeyMac[16], sendSenquenceCounter[8];
			int res = ExternalAuthenticate(getChallengeResponse, encryptKey, macKey,
										   sessionKeyEncrypt, sessionKeyMac, sendSenquenceCounter);
			if (res == 0) {
				ReadEFCOM(sessionKeyEncrypt, sessionKeyMac, sendSenquenceCounter);
				ReadDG1(sessionKeyEncrypt, sessionKeyMac, sendSenquenceCounter);
				ReadDG2(sessionKeyEncrypt, sessionKeyMac, sendSenquenceCounter, imageFilePath);
				break;
			}
		}
	}
end:
	DisconnectFeliCaCard();
	DisconnectReader();
	return res;
}