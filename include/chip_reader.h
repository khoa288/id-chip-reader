/**
 * @author Khoa Nguyen
 * @file chip_reader.h
 * @brief Header file for chip reader functions.
 *
 * This header file contains the function declaration for reading data from an ID card chip using
 * Basic Access Control (BAC) application functions. The ReadIdCardChip function is designed to work
 * with a smart card reader and a corresponding smart card that supports BAC protocol.
 */

#pragma once
#ifndef CHIP_READER_H_
#define CHIP_READER_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reads data from an ID card chip using Basic Access Control (BAC) protocol.
 *
 * Given the MRZ information, this function reads data from an ID card chip using the BAC protocol,
 * which includes initializing the reader, selecting applications, getting challenges, and reading
 * data groups (EF.COM, DG1, and DG2). This function works with a smart card reader and the
 * corresponding smart card that supports BAC protocol.
 *
 * @param[in] mrzInformation The MRZ information as an array of unsigned chars used for BAC
 authentication.
 * @param[out] imageFilePath The file path to the image file that will be created after reading
		   data from the ID card chip.
 *
 * @return A long value representing the status code. APP_SUCCESS indicates successful reading of
		   data from the ID card chip, otherwise an error code is returned.
 */
long ReadIdCardChip(unsigned char mrzInformation[], unsigned char imageFilePath[]);

#ifdef __cplusplus
}
#endif

#endif	// #ifndef CHIP_READER_H_