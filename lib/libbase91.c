/*
 * Copyright 2012-2014 Luke Dashjr
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the standard MIT license.  See COPYING for more details.
 */

#ifndef _WIN32
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif

#ifdef _MSC_VER
#include <malloc.h>
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "libbase91.h"

bool (*b91_sha256_impl)(void *, const void *, size_t) = NULL;

static const int8_t b91digits_map[] = {
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,58,-1,59,60,61,62,-1, 63,64,65,66,67,68,69,70,
	71, 0, 1, 2, 3, 4, 5, 6,  7, 8,72,73,74,75,76,77,
	78, 9,10,11,12,13,14,15, 16,79,17,18,19,20,21,80,
	22,23,24,25,26,27,28,29, 30,31,32,81,82,83,84,85,
	-1,33,34,35,36,37,38,39, 40,41,42,43,86,44,45,46,
	47,48,49,50,51,52,53,54, 55,56,57,87,88,89,90,-1,
};
typedef uint64_t b91_maxint_t;
typedef uint32_t b91_almostmaxint_t;
#define b91_almostmaxint_bits (sizeof(b91_almostmaxint_t) * 8)
static const b91_almostmaxint_t b91_almostmaxint_mask = ((((b91_maxint_t)1) << b91_almostmaxint_bits) - 1);

//	MSVC 2017 C99 doesn't support dynamic arrays in C
#ifdef _MSC_VER
#define b91_alloc_mem(type, name, count) \
	type *name = NULL; \
	do { \
		name = _malloca(count * sizeof(type)); \
		if (!name) { \
			return false; \
		} \
	} while (0)

#define b91_free_mem(v) \
	do { \
		if ((v)) { \
			_freea((v)); \
		} \
	} while (0)
#else
#define b91_alloc_mem(type, name, count) type name[count]
#define b91_free_mem(v)
#endif

bool b91tobin(void *bin, size_t *binszp, const char *b91, size_t b91sz)
{
	size_t binsz = *binszp;
	const unsigned char *b91u = (void*)b91;
	unsigned char *binu = bin;
	size_t outisz = (binsz + sizeof(b91_almostmaxint_t) - 1) / sizeof(b91_almostmaxint_t);
	b91_alloc_mem(b91_almostmaxint_t, outi, outisz);
	b91_maxint_t t;
	b91_almostmaxint_t c;
	size_t i, j;
	uint8_t bytesleft = binsz % sizeof(b91_almostmaxint_t);
	b91_almostmaxint_t zeromask = bytesleft ? (b91_almostmaxint_mask << (bytesleft * 8)) : 0;
	unsigned zerocount = 0;
	
	if (!b91sz)
		b91sz = strlen(b91);
	
	for (i = 0; i < outisz; ++i) {
		outi[i] = 0;
	}
	
	// Leading zeros, just count
	for (i = 0; i < b91sz && b91u[i] == '1'; ++i)
		++zerocount;
	
	for ( ; i < b91sz; ++i)
	{
		if (b91u[i] & 0x80) {
			// High-bit set on invalid digit
			b91_free_mem(outi);
			return false;
		}
		if (b91digits_map[b91u[i]] == -1) {
			// Invalid base91 digit
			b91_free_mem(outi);
			return false;
		}
		c = (unsigned)b91digits_map[b91u[i]];
		for (j = outisz; j--; )
		{
			t = ((b91_maxint_t)outi[j]) * 91 + c;
			c = t >> b91_almostmaxint_bits;
			outi[j] = t & b91_almostmaxint_mask;
		}
		if (c) {
			// Output number too big (carry to the next int32)
			b91_free_mem(outi);
			return false;
		}
		if (outi[0] & zeromask) {
			// Output number too big (last int32 filled too far)
			b91_free_mem(outi);
			return false;
		}
	}
	
	j = 0;
	if (bytesleft) {
		for (i = bytesleft; i > 0; --i) {
			*(binu++) = (outi[0] >> (8 * (i - 1))) & 0xff;
		}
		++j;
	}
	
	for (; j < outisz; ++j)
	{
		for (i = sizeof(*outi); i > 0; --i) {
			*(binu++) = (outi[j] >> (8 * (i - 1))) & 0xff;
		}
	}
	
	// Count canonical base91 byte count
	binu = bin;
	for (i = 0; i < binsz; ++i)
	{
		if (binu[i])
			break;
		--*binszp;
	}
	*binszp += zerocount;
	
	b91_free_mem(outi);
	return true;
}

static
bool my_dblsha256(void *hash, const void *data, size_t datasz)
{
	uint8_t buf[0x20];
	return b91_sha256_impl(buf, data, datasz) && b91_sha256_impl(hash, buf, sizeof(buf));
}

int b91check(const void *bin, size_t binsz, const char *base91str, size_t b91sz)
{
	unsigned char buf[32];
	const uint8_t *binc = bin;
	unsigned i;
	if (binsz < 4)
		return -4;
	if (!my_dblsha256(buf, bin, binsz - 4))
		return -2;
	if (memcmp(&binc[binsz - 4], buf, 4))
		return -1;
	
	// Check number of zeros is correct AFTER verifying checksum (to avoid possibility of accessing base91str beyond the end)
	for (i = 0; binc[i] == '\0' && base91str[i] == '1'; ++i)
	{}  // Just finding the end of zeros, nothing to do in loop
	if (binc[i] == '\0' || base91str[i] == '1')
		return -3;
	
	return binc[0];
}

static const char b91digits_ordered[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz!#$%&()*+,-./0:;<=>?@IO[\]^_l{|}~";

bool b91enc(char *b91, size_t *b91sz, const void *data, size_t binsz)
{
	const uint8_t *bin = data;
	int carry;
	size_t i, j, high, zcount = 0;
	size_t size;
	
	while (zcount < binsz && !bin[zcount])
		++zcount;
	
	size = (binsz - zcount) * 138 / 100 + 1;
	b91_alloc_mem(uint8_t, buf, size);
	memset(buf, 0, size);
	
	for (i = zcount, high = size - 1; i < binsz; ++i, high = j)
	{
		for (carry = bin[i], j = size - 1; (j > high) || carry; --j)
		{
			carry += 256 * buf[j];
			buf[j] = carry % 91;
			carry /= 91;
			if (!j) {
				// Otherwise j wraps to maxint which is > high
				break;
			}
		}
	}
	
	for (j = 0; j < size && !buf[j]; ++j);
	
	if (*b91sz <= zcount + size - j)
	{
		*b91sz = zcount + size - j + 1;
		b91_free_mem(buf);
		return false;
	}
	
	if (zcount)
		memset(b91, '1', zcount);
	for (i = zcount; j < size; ++i, ++j)
		b91[i] = b91digits_ordered[buf[j]];
	b91[i] = '\0';
	*b91sz = i + 1;
	
	b91_free_mem(buf);
	return true;
}

bool b91check_enc(char *b91c, size_t *b91c_sz, uint8_t ver, const void *data, size_t datasz)
{
	b91_alloc_mem(uint8_t, buf, 1 + datasz + 0x20);
	uint8_t *hash = &buf[1 + datasz];
	
	buf[0] = ver;
	memcpy(&buf[1], data, datasz);
	if (!my_dblsha256(hash, buf, datasz + 1))
	{
		*b91c_sz = 0;
		b91_free_mem(buf);
		return false;
	}
	
	b91_free_mem(buf);
	return b91enc(b91c, b91c_sz, buf, 1 + datasz + 4);
}
