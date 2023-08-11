/**
 * @file des.h
 * @brief Triple DES (3DES) encryption and decryption library header file
 * @author Khoa Nguyen
 *
 * This header file provides APIs for Triple DES (3DES) encryption and decryption operations.
 * The library supports both DES and 3DES algorithms, as well as Electronic Codebook (ECB)
 * and Cipher Block Chaining (CBC) modes.
 *
 * Supported algorithms and modes:
 * - DES-ECB
 * - DES-CBC
 * - 3DES-ECB
 * - 3DES-CBC
 */

#pragma once
#include "typedef.h"

#ifndef CRYPTOGRAPHY_DES3_H_
#define CRYPTOGRAPHY_DES3_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MBEDTLS_DES_ENCRYPT					 1
#define MBEDTLS_DES_DECRYPT					 0
#define MBEDTLS_ERR_DES_INVALID_INPUT_LENGTH 0

#define USE_DES_EN							 1	// Use DES
#define USE_DES_ECB_EN						 1	// ECB mode using DES
#define USE_DES_CBC_EN						 0	// CBC mode using DES

#define USE_3DES_EN							 1	// Use 3DES
#define USE_3DES_ECB_EN						 0	// ECB mode using 3DES
#define USE_3DES_CBC_EN						 1	// CBC mode using 3DES

#define DES_INPUT_LENGTH					 -0x0002  // The data input has an invalid length

#define MBEDTLS_DES_KEY_SIZE				 8
#define DES_KEY_SIZE						 (8)
#define DES3_KEY2_SIZE						 (16)
#define DES3_KEY3_SIZE						 (24)

#if USE_DES_EN
#if USE_DES_ECB_EN
// DES-EBC buffer encryption API
unsigned int des_ecb_encrypt(unsigned char* pout,
							 unsigned char* pdata,
							 unsigned int nlen,
							 unsigned char* pkey);

// DES-EBC buffer decryption API
unsigned int des_ecb_decrypt(unsigned char* pout,
							 unsigned char* pdata,
							 unsigned int nlen,
							 unsigned char* pkey);
#endif	// #if USE_DES_ECB_EN

#if USE_DES_EN
typedef struct {
	uint32_t sk[32];  // DES subkeys
} des_context;
#endif

#if USE_DES_CBC_EN
int des_crypt_cbc(des_context* ctx,
				  int mode,
				  size_t length,
				  unsigned char iv[8],
				  const unsigned char* input,
				  unsigned char* output);
unsigned int des_cbc_encrypt(unsigned char* pout,
							 unsigned char* pdata,
							 unsigned int nlen,
							 unsigned char* pkey,
							 unsigned char* piv);
#endif	// #if USE_DES_CBC_EN
#endif	// #if USE_DES_EN

#if USE_3DES_EN
#if USE_3DES_ECB_EN
// 3DES-ECB buffer encryption API
unsigned int des3_ecb_encrypt(unsigned char* pout,
							  unsigned char* pdata,
							  unsigned int nlen,
							  unsigned char* pkey,
							  unsigned int klen);

// 3DES-ECB buffer decryption API
unsigned int des3_ecb_decrypt(unsigned char* pout,
							  unsigned char* pdata,
							  unsigned int nlen,
							  unsigned char* pkey,
							  unsigned int klen);
#endif	// #if USE_3DES_ECB_EN

#if USE_3DES_CBC_EN
// 3DES-CBC buffer encryption API
unsigned int des3_cbc_encrypt(unsigned char* pout,
							  unsigned char* pdata,
							  unsigned int nlen,
							  unsigned char* pkey,
							  unsigned int klen,
							  unsigned char* piv);

// 3DES-CBC buffer decryption API
unsigned int des3_cbc_decrypt(unsigned char* pout,
							  unsigned char* pdata,
							  unsigned int nlen,
							  unsigned char* pkey,
							  unsigned int klen,
							  unsigned char* piv);
#endif	// #if USE_3DES_CBC_EN
#endif	// #if USE_3DES_EN

#ifdef __cplusplus
}
#endif

#endif	// #ifndef CRYPTOGRAPHY_DES3_H_
