/**
 * @file	reader.c
 * @brief	Source file for IC Card Reader Access functions.
 * @author	Copyright 2013 Sony Corporation
 * @date	2013/10/31
 *
 * This source file implements functions for accessing and interacting with an IC Card Reader.
 */

#pragma comment(lib, "winscard.lib")

#include <conio.h>
#include <stdio.h>
#include <winscard.h>

#include <utils/reader.h>

// PC/SC connection const
static const unsigned char PASORI_PCSC_NO_ERROR_HEADER[5] = {0xC0, 0x03, 0x00, 0x90, 0x00};

// Reader name of PaSoRi
static wchar_t CARD_IF_PASORI[] = L"Sony FeliCa Port/PaSoRi 3.0 0";
static int isCancelDetected		= 0;
// Reader name of Contact Card reader (depends on driver version)
// static wchar_t SAM_IF_GEM[] = L"Gemalto USB Smart Card Reader 0";
static wchar_t SAM_IF_GEM[] = L"Gemalto USB SmartCard Reader 0";
// static wchar_t SAM_IF_GEM[] = L"Gemplus USB SmartCard Reader 0";

// PC/SC context
SCARDCONTEXT hContext;
SCARDHANDLE hCardFeliCa;
SCARDHANDLE hCardSAM;

// Buffer to store PC/SC Reader Names
LPTSTR mszReaders;

long InitializeReader(void) {
	long _ret				  = 0;
	unsigned long pcchReaders = SCARD_AUTOALLOCATE;
	LPTSTR pReader;

#if USE_SAM
	unsigned long dwActProtocolSAM;
#endif	// #if USE_SAM

	printf("Initialize Reader\n");

	// Establish Context
	printf("Establish Context\n");
	_ret = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &hContext);
	if (_ret != SCARD_S_SUCCESS) {
		printf(" -> Error\n");
		return APP_ERROR;
	}

	// List All Readers
	printf("List All Readers\n");
	_ret = SCardListReaders(hContext, NULL, (LPTSTR)&mszReaders, &pcchReaders);
	if (_ret != SCARD_S_SUCCESS) {
		printf(" -> Error\n");
		return APP_ERROR;
	}
	pReader = mszReaders;
	while ('\0' != *pReader) {
		printf(" %TS\n", pReader);
		pReader = pReader + wcslen((wchar_t*)pReader) + 1;
	}

	SCardFreeMemory(hContext, mszReaders);

#if USE_SAM && !USE_NFC
	// Connect to SAM interface
	printf("Connect SAM\n");
	_ret = SCardConnect(hContext, SAM_IF_GEM, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T1, &hCardSAM,
						&dwActProtocolSAM);
#endif	// #if USE_SAM && !USE_NFC

	if (_ret != SCARD_S_SUCCESS) {
		printf(" -> Error\n");
		return APP_ERROR;
	}

	return APP_SUCCESS;
}

void DisconnectReader(void) {
	SCardDisconnect(hCardFeliCa, SCARD_UNPOWER_CARD);
	SCardDisconnect(hCardSAM, SCARD_UNPOWER_CARD);

	SCardReleaseContext(hContext);
}

long DetectFeliCaCard(void) {
	long _ret;

	unsigned long readerLen, ATRLen, dwState;
	unsigned char ATRVal[262];
	unsigned long dwActProtocol;

	unsigned char _send_buf[262], receiveBuf[262];
	unsigned long _send_len, receiveLen;
	static char CARD_IF_PASORI[] = "Sony FeliCa Port/PaSoRi 3.0 0";

	printf("\nTap FeliCa Card\n");
	printf("<Press ESC Key to cancel>\n");

	while (isCancelDetected == 0) {
		// Hit Esc to cancel operation
		if ((0 != _kbhit()) && (SMPL_ESC_KEY == _getch())) {
			printf("  -> Canceled\n");
			return APP_CANCEL;
		}

		_ret = SCardConnect(hContext, CARD_IF_PASORI, SCARD_SHARE_SHARED,
							SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCardFeliCa, &dwActProtocol);

		if (_ret != SCARD_S_SUCCESS) {
			continue;
		}

		// Get ATR value to check status
		readerLen = 100;
		ATRLen	  = 32;
		_ret	  = SCardStatus(hCardFeliCa, CARD_IF_PASORI, &readerLen, &dwState, &dwActProtocol,
								ATRVal, &ATRLen);

		if (_ret != SCARD_S_SUCCESS) {
			return APP_ERROR;
		}

		// Start Transparent Session of PC/SC on Pasori
		_send_buf[0] = 0xFF;
		_send_buf[1] = 0xC2;
		_send_buf[2] = 0x00;
		_send_buf[3] = 0x00;
		_send_buf[4] = 0x02;
		_send_buf[5] = 0x81;
		_send_buf[6] = 0x00;
		_send_len	 = 7;
		receiveLen	 = 7;
		_ret =
			SCardTransmit(hCardFeliCa, NULL, _send_buf, _send_len, NULL, receiveBuf, &receiveLen);

		if (_ret != SCARD_S_SUCCESS) {
			return APP_ERROR;
		}

		// Response of PC/SC Start Transparent Session with no error contains 7
		// bytes
		//  C0 03 00 90 00  90 00
		if (receiveLen != 7) {
			return APP_ERROR;
		}
		if (memcmp(receiveBuf, PASORI_PCSC_NO_ERROR_HEADER, 5) != 0) {
			return APP_ERROR;
		}

		break;
	}

	if (isCancelDetected) {
		isCancelDetected = 0;
		return APP_CANCEL;
	}
	return APP_SUCCESS;
}

void CancelDetectFelicaCard(void) {
	isCancelDetected = 1;
}

long DisconnectFeliCaCard(void) {
	long _ret;
	unsigned char _send_buf[262];
	unsigned char receiveBuf[262];
	unsigned long _send_len, receiveLen;

	// Turn off RF Power
	_send_buf[0] = 0xFF;
	_send_buf[1] = 0xC2;
	_send_buf[2] = 0x00;
	_send_buf[3] = 0x00;
	_send_buf[4] = 0x02;
	_send_buf[5] = 0x83;
	_send_buf[6] = 0x00;
	_send_len	 = 7;
	receiveLen	 = 7;
	_ret = SCardTransmit(hCardFeliCa, NULL, _send_buf, _send_len, NULL, receiveBuf, &receiveLen);

	if (_ret != SCARD_S_SUCCESS) {
		return APP_ERROR;
	}

	// End Transparent Session of PC/SC on Pasori
	_send_buf[0] = 0xFF;
	_send_buf[1] = 0xC2;
	_send_buf[2] = 0x00;
	_send_buf[3] = 0x00;
	_send_buf[4] = 0x02;
	_send_buf[5] = 0x82;
	_send_buf[6] = 0x00;
	_send_len	 = 7;
	receiveLen	 = 7;
	_ret = SCardTransmit(hCardFeliCa, NULL, _send_buf, _send_len, NULL, receiveBuf, &receiveLen);

	if (_ret != SCARD_S_SUCCESS) {
		return APP_ERROR;
	}

	// Response of PC/SC Start Transparent Session with no error contains 7
	// bytes
	//  C0 03 00 90 00  90 00
	if (receiveLen != 7) {
		return APP_ERROR;
	}
	if (memcmp(receiveBuf, PASORI_PCSC_NO_ERROR_HEADER, 5) != 0) {
		return APP_ERROR;
	}

	return APP_SUCCESS;
}

inline void PrintHexArray(char* header, unsigned long len, unsigned char byte_array[]) {
	unsigned int i;

	printf("%s", header);

	for (i = 0; i < len; i++) {
		printf("%02X ", byte_array[i]);
	}
	printf("\n");
}

long TransmitDataToCard(unsigned char cmdBuf[],
						unsigned long cmdLen,
						unsigned char resBuf[],
						unsigned long* resLen) {
#if DEBUG
	PrintHexArray("\nPC->CARD: ", cmdLen, cmdBuf);
#endif	// #if DEBUG

	DWORD dwResLen = *resLen;

#if USE_SAM
	long _ret = SCardTransmit(hCardSAM, NULL, cmdBuf, cmdLen, NULL, resBuf, &dwResLen);
#elif USE_NFC
	long _ret = SCardTransmit(hCardFeliCa, NULL, cmdBuf, cmdLen, NULL, resBuf, &dwResLen);
#endif	// #if USE_SAM

	*resLen = dwResLen;

	if (_ret != SCARD_S_SUCCESS) {
		printf("SCardTransmit Error\n");
		return _ret;
	}

#if DEBUG
	PrintHexArray("CARD->PC: ", *resLen, resBuf);
#endif	// #if DEBUG

	return APP_SUCCESS;
}