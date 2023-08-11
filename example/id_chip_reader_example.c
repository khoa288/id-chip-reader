/**
 * @file id_chip_reader_example.c
 * @brief Example of using this library to read data from an ID card chip.
 * @author Khoa Nguyen
 */

#include <chip_reader.h>

int main() {
	unsigned char mrzInformation[] = "<MRZ_INFORMATION>";
	unsigned char imageFilePath[]  = "<IMAGE_FILE_PATH>";

	long res = ReadIdCardChip(mrzInformation, imageFilePath);

	return res;
}