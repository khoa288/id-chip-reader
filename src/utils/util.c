/**
 * @author Khoa Nguyen
 * @file util.c
 * @brief Source file for utility functions.
 */

#include <Windows.h>
#include <stdlib.h>
#include <time.h>

#include <utils/util.h>

void Delay(int millisecond) {
	Sleep(millisecond);
}

void RandomNonceGenerate(unsigned char* buffer, int length) {
	srand((int)time(NULL));
	for (int i = 0; i < length; i++) {
		buffer[i] = rand() % 256;
	}
}

void PadByteArray(unsigned char* byteArray, int startPosition) {
	byteArray[startPosition] = 0x80;
	memset(&byteArray[startPosition + 1], 0x00, 7 - (startPosition % 8));
}

int CharToInt(const char c) {
	if (c >= '0' && c <= '9') {
		return c - '0';
	} else if (c >= 'A' && c <= 'Z') {
		return c - '9' + 2;
	} else if (c == '<') {
		return 0;
	}
}

char IntToChar(const int i) {
	return i + '0';
}

int CheckDigitCalculate(const unsigned char* data, int length) {
	const int weight[3] = {7, 3, 1};
	int s				= 0;
	for (int i = 0; i < length; i++) {
		s += CharToInt(data[i]) * weight[i % 3];
	}
	return s %= 10;
}

int ExpirationYearCalculate(const int birthYear, const int currentYear) {
	int age = currentYear - birthYear;
	if (age <= 25) {
		return birthYear + 25;
	} else if (age <= 40) {
		return birthYear + 40;
	} else if (age <= 60) {
		return birthYear + 60;
	} else {
		return -1;
	}
}

int CharToYear(const unsigned char year[2], const int currentYear) {
	int yearValue		 = CharToInt(year[0]) * 10 + CharToInt(year[1]);
	int currentYearValue = currentYear % 100;
	if (yearValue < currentYearValue) {
		return 2000 + yearValue;
	} else {
		return 1900 + yearValue;
	}
}

void YearToChar(const int year, unsigned char yearChar[2]) {
	memset(yearChar, IntToChar((year % 100) / 10), 1);
	memset(&yearChar[1], IntToChar(year % 10), 1);
}

void MrzInformationGenerate(const unsigned char documentNumber[9],
							const unsigned char birthDate[4],
							unsigned char mrzInformation[24],
							int currentYear) {
	memset(mrzInformation, 0, 24);
	memcpy(&mrzInformation[0], documentNumber, 9);
	memset(&mrzInformation[9], IntToChar(CheckDigitCalculate(documentNumber, 9)), 1);
	unsigned char birth[6];
	unsigned char birthYear[2];
	memcpy(&birthYear[0], &mrzInformation[1], 2);
	memcpy(&birth[0], &birthYear[0], 2);
	memcpy(&birth[2], &birthDate[0], 4);
	memcpy(&mrzInformation[10], &birth[0], 6);
	memset(&mrzInformation[16], IntToChar(CheckDigitCalculate(birth, 6)), 1);
	unsigned char expiration[6];
	unsigned char expirationYear[2];
	YearToChar(ExpirationYearCalculate(CharToYear(birthYear, currentYear), currentYear),
			   expirationYear);
	memcpy(&expiration[2], &birthDate[0], 4);
	memcpy(&expiration[0], &expirationYear[0], 2);
	memcpy(&expiration[2], &birthDate[0], 4);
	memcpy(&mrzInformation[17], &expiration[0], 6);
	memset(&mrzInformation[23], IntToChar(CheckDigitCalculate(expiration, 6)), 1);
}