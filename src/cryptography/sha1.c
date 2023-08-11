/**
 * @author Khoa Nguyen
 * @file sha1.c
 * @brief Source file for SHA-1 hash algorithm.
 */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <string.h>
#define rol(x, y) ((x << y) | (x >> (32 - y)))	// Loop left shift

// One cycle process, STR is a portion of the filled data or part in the data
void round(unsigned char str[64], unsigned int h[5]) {
	unsigned int a, b, c, d, e, tmp, w[80];
	unsigned int i;
	for (i = 0; i < 16; i++) {
		w[i] = ((unsigned int)str[i * 4] << 24) | (((unsigned int)str[i * 4 + 1]) << 16) |
			   (((unsigned int)str[i * 4 + 2]) << 8) | (((unsigned int)str[i * 4 + 3]) << 0);
	}
	for (i = 16; i < 80; i++) {
		unsigned int tmp = w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16];
		w[i]			 = rol(tmp, 1);
	}

	a = h[0];
	b = h[1];
	c = h[2];
	d = h[3];
	e = h[4];
	for (i = 0; i < 80; i++) {
		switch (i / 20) {
			case 0:
				tmp = rol(a, 5) + ((b & c) | (d & ~b)) + e + w[i] + 0x5a827999;
				break;
			case 1:
				tmp = rol(a, 5) + (b ^ c ^ d) + e + w[i] + 0x6ed9eba1;
				break;
			case 2:
				tmp = rol(a, 5) + ((b & c) | (b & d) | (c & d)) + e + w[i] + 0x8f1bbcdc;
				break;
			case 3:
				tmp = rol(a, 5) + (b ^ c ^ d) + e + w[i] + 0xca62c1d6;
				break;
		}
		e = d;
		d = c;
		c = rol(b, 30);
		b = a;
		a = tmp;
	}
	h[0] += a;
	h[1] += b;
	h[2] += c;
	h[3] += d;
	h[4] += e;
}

// SHA-1 algorithm
void sha1(unsigned char* input, long long len, unsigned char* output) {
	unsigned char temp[64];
	unsigned int h[5] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0};
	unsigned int i, tmp;
	long long n = len;
	while (n >= 64) {
		memcpy(temp, input + len - n, 64);
		round(temp, h);
		n -= 64;
	}

	if (n >= 56) {
		memset(temp, 0, 64);
		memcpy(temp, input + len - n, n);
		temp[n] = 128;
		round(temp, h);
		memset(temp, 0, 64);
		for (i = 56; i < 64; i++)
			temp[i] = ((len * 8) >> (63 - i) * 8) & 0xff;
		round(temp, h);
	} else {
		memset(temp, 0, 64);
		memcpy(temp, input + len - n, n);
		temp[n] = 128;
		for (i = 56; i < 64; i++)
			temp[i] = ((len * 8) >> (63 - i) * 8) & 0xff;
		round(temp, h);
	}

	for (i = 0; i < 20; i++) {
		tmp		  = (h[i / 4] >> ((3 - i % 4) * 8)) & 0xff;
		output[i] = tmp;
	}
}
