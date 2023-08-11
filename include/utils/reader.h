/**
 * @file	reader.h
 * @brief	Header file for IC Card Reader Access functions.
 * @author	Copyright 2013 Sony Corporation
 * @date	2013/10/31
 *
 * This header file provides an API for accessing and interacting with an IC Card Reader.
 */

#pragma once
#ifndef UTILS_READER_H_
#define UTILS_READER_H_

#include "config.h"

// Reader Writer related definition
#define UNKOWN_ERROR	  1
#define RCS500_LCLE_ERROR 2
#define RCS500_P1P2_ERROR 3
#define RCS500_INS_ERROR  4
#define RCS500_CLA_ERROR  5
#define RCS500_MAC_ERROR  8

#define SMPL_ESC_KEY	  0x1B

#define APP_ERROR		  -1
#define APP_CANCEL		  -2
#define APP_SUCCESS		  0

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize IC Card Reader.
 *
 * This function initializes the IC Card Reader and prepares it for use.
 *
 * @return APP_SUCCESS if successful, otherwise APP_ERROR.
 */
long InitializeReader(void);

/**
 * @brief Release Reader resources.
 *
 * This function releases the resources associated with the IC Card Reader.
 */
void DisconnectReader(void);

/**
 * @brief Detect FeliCa Card.
 *
 * This function blocks until a FeliCa card has detected or ESC Key has pressed.
 *
 * @return APP_SUCCESS if successful, otherwise APP_ERROR.
 */
long DetectFeliCaCard(void);

/**
 * @brief Cancel Detect Felica Card operation.
 *
 * This function cancels the ongoing detection of a Felica card in case it is needed to stop waiting
 * for a card to be detected.
 */
void CancelDetectFelicaCard(void);

/**
 * @brief Release FeliCa card connection.
 *
 * This function releases the connection with the detected FeliCa card.
 *
 * @return APP_SUCCESS if successful, otherwise APP_ERROR.
 */
long DisconnectFeliCaCard(void);

/**
 * @brief Transmit APDU command packets to FeliCa Card.
 *
 * This function sends the provided command packets to the detected FeliCa card and receives the
 * response in the provided buffer.
 *
 * @param cmdBuf[] Buffer containing the command packets to be sent.
 * @param cmdLen Length of the command buffer.
 * @param resBuf[] Buffer to store the received response from the card.
 * @param resLen Pointer to a variable that will store the length of the received response.
 *
 * @return APP_SUCCESS if successful, otherwise APP_ERROR.
 */
long TransmitDataToCard(unsigned char cmdBuf[],
						unsigned long cmdLen,
						unsigned char resBuf[],
						unsigned long* resLen);

#ifdef __cplusplus
}
#endif

#endif	// #ifndef UTILS_READER_H_