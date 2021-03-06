/*
	Audio File Library
	Copyright (C) 1998-1999, Michael Pruett <michael@68k.org>

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Library General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Library General Public License for more details.

	You should have received a copy of the GNU Library General Public
	License along with this library; if not, write to the
	Free Software Foundation, Inc., 59 Temple Place - Suite 330,
	Boston, MA  02111-1307  USA.
*/

/*
	byteorder.h

	This file declares functions useful for dealing with byte
	swapping.
*/

#ifndef BYTEORDER_H
#define BYTEORDER_H

#include <config.h>

#include <stdint.h>

#if WORDS_BIGENDIAN
	#define _AF_BYTEORDER_NATIVE (AF_BYTEORDER_BIGENDIAN)
#else
	#define _AF_BYTEORDER_NATIVE (AF_BYTEORDER_LITTLEENDIAN)
#endif

#if !WORDS_BIGENDIAN

#define HOST_TO_LENDIAN_INT16(x)		((uint16_t) (x))
#define HOST_TO_LENDIAN_INT32(x)		((uint32_t) (x))
#define HOST_TO_LENDIAN_FLOAT32(x)		((float) (x))
#define HOST_TO_LENDIAN_DOUBLE64(x)		((double) (x))

#define LENDIAN_TO_HOST_INT16(x)		((uint16_t) (x))
#define LENDIAN_TO_HOST_INT32(x)		((uint32_t) (x))
#define LENDIAN_TO_HOST_FLOAT32(x)		((float) (x))
#define LENDIAN_TO_HOST_DOUBLE64(x)		((double) (x))

#else

#define HOST_TO_LENDIAN_INT16(x)		_af_byteswap_int16(x)
#define HOST_TO_LENDIAN_INT32(x)		_af_byteswap_int32(x)
#define HOST_TO_LENDIAN_FLOAT32(x)		_af_byteswap_float32(x)
#define HOST_TO_LENDIAN_DOUBLE64(x)		_af_byteswap_double64(x)

#define LENDIAN_TO_HOST_INT16(x)		_af_byteswap_int16(x)
#define LENDIAN_TO_HOST_INT32(x)		_af_byteswap_int32(x)
#define LENDIAN_TO_HOST_FLOAT32(x)		_af_byteswap_float32(x)
#define LENDIAN_TO_HOST_DOUBLE64(x)		_af_byteswap_double64(x)

#endif

#if WORDS_BIGENDIAN

#define HOST_TO_BENDIAN_INT16(x)		((uint16_t) (x))
#define HOST_TO_BENDIAN_INT32(x)		((uint32_t) (x))
#define HOST_TO_BENDIAN_FLOAT32(x)		((float) (x))
#define HOST_TO_BENDIAN_DOUBLE64(x)		((double) (x))

#define BENDIAN_TO_HOST_INT16(x)		((uint16_t) (x))
#define BENDIAN_TO_HOST_INT32(x)		((uint32_t) (x))
#define BENDIAN_TO_HOST_FLOAT32(x)		((float) (x))
#define BENDIAN_TO_HOST_DOUBLE64(x)		((double) (x))

#else

#define HOST_TO_BENDIAN_INT16(x)		_af_byteswap_int16(x)
#define HOST_TO_BENDIAN_INT32(x)		_af_byteswap_int32(x)
#define HOST_TO_BENDIAN_FLOAT32(x)		_af_byteswap_float32(x)
#define HOST_TO_BENDIAN_DOUBLE64(x)		_af_byteswap_double64(x)

#define BENDIAN_TO_HOST_INT16(x)		_af_byteswap_int16(x)
#define BENDIAN_TO_HOST_INT32(x)		_af_byteswap_int32(x)
#define BENDIAN_TO_HOST_FLOAT32(x)		_af_byteswap_float32(x)
#define BENDIAN_TO_HOST_DOUBLE64(x)		_af_byteswap_double64(x)

#endif

uint16_t _af_byteswap_int16 (uint16_t x);
uint32_t _af_byteswap_int32 (uint32_t x);
float _af_byteswap_float32 (float x);
double _af_byteswap_double64 (double x);

#endif
