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