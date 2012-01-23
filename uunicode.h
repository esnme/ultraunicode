/*
UltraUnicode 1.0.0 -
A simplistic and efficient library for dealing with Unicode, UTF-8, UTF-16, UTF-32 and byte orders.

Copyright (c) 2012, Jonas Tarnstrom and ESN Social Software AB
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by ESN Social Software AB (www.esn.me).
4. Neither the name of the ESN Social Software AB nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ESN SOCIAL SOFTWARE AB ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ESN SOCIAL SOFTWARE AB BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __UUNICODE_H__
#define __UUNICODE_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <intrin.h>

typedef UINT8 UTF8;
typedef UINT16 UTF16;
typedef UINT32 UTF32;
typedef SSIZE_T ssize_t;

#define BSWAP_U16(_in) \
  _byteswap_ushort((_in))
#define BSWAP_U32(_in) \
  _byteswap_ulong((_in))

#define THREADLOCAL __declspec(thread)

#else
#include <sys/types.h>
#include <byteswap.h>
typedef u_int8_t UTF8;
typedef u_int16_t UTF16;
typedef u_int32_t UTF32;

#define BSWAP_U16(_in) \
  bswap_16((_in))
#define BSWAP_U32(_in) \
  bswap_32((_in))

#define THREADLOCAL __thread

#endif
  
#include <wchar.h>
#include <setjmp.h>

enum UUERRORS
{
  UUERR_SUCCESS = 0,
  UUERR_INVALID_CODEPOINT = -1,
  UUERR_UNEXPECTED_END_OF_INPUT = -2,
  UUERR_OVERLONG_UTF8_SEQUENCE = -3,
  UUERR_INVALID_UTF8_SEQUENCE_LENGTH = -4,
  UUERR_UNMATCHED_LEAD_SURROGATE = -5,
  UUERR_BOM_DETECTED = -6,
	UUERR_OUTPUT_OVERFLOW = -7
};

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define __LITTLE_ENDIAN__
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define __BIG_ENDIAN__
#else
#error "Unknown byte order"
#endif

extern THREADLOCAL jmp_buf _uuException;
extern THREADLOCAL int _uuErrno;

#define UUNICODE_TRY() if (!(_uuErrno = setjmp(_uuException)))
#define UUNICODE_CATCH_SUCCESS() else if (_uuErrno == UUERR_SUCCESS)
#define UUNICODE_CATCH_ERROR() else
#define UUNICODE_ERRNO() (_uuErrno)

/* Native byte order to Little Endian */

/*
Converts Native UTF16 to Little Endian */
UTF16 uuUTF16ToLE(UTF16 in);

/*
Converts Native UTF32 to Little Endian */
UTF32 uuUTF32ToLE(UTF32 in);

/* Native byte order to Big Endian*/

/*
Converts Native UTF16 to Big Endian */
UTF16 uuUTF16ToBE(UTF16 in);

/*
Converts Native UTF32 to Big Endian */
UTF32 uuUTF32ToBE(UTF32 in);

/* Little Endian byte order to Native */

/*
Converts UTF16LE to Native */
UTF16 uuUTF16LEToNA(UTF16 in);

/*
Converts UTF32LE to Native */
UTF32 uuUTF32LEToNA(UTF32 in);

/* Big Endian byte order to Native */

/*
Converts UTF16BE to Native */
UTF16 uuUTF16BEToNA(UTF16 in);

/*
Converts UTF32BE to Native */
UTF32 uuUTF32BEToNA(UTF32 in);

/*
Converts a Unicode character to UTF-8. 
Returns the number of bytes written to out is returned. Native byte order */
size_t uuUTF32ToUTF8(UTF32 in, UTF8 out[4]);

/*
Converts a UTF-8 sequence to a Unicode character. 
Return value on error is less than 0. On success the number of bytes read from in is returned. 
Native byte order

Handling of special octets in input:
0x00 NULL => UUERR_SUCCESS is set
0xfe BOM  => UUERR_BOM_DETECTED is set
0xff BOM  => UUERR_BOM_DETECTED is set
*/
int uuUTF8ToUTF32(const UTF8 *in, const UTF8 *end, UTF32 *out);

/*
Converts a UTF-16 sequence to a Unicode character. 
Returns the number of words (16-bit) read from in is returned. Native byte order */
int uuUTF16ToUTF32(const UTF16 *in, const UTF16 *end, UTF32 *out);

/*
Converts a Unicode character to a UTF-16 sequence. 
Returns the number of words (16-bit) written to out is returned. Native byte order */
int uuUTF32ToUTF16(UTF32 in, UTF16 out[2]);

/*
Converts a suplimentary plane Unicode code point into a surrogate UTF-16 pair 
Function will produce undefined output input is not in the suplimentary plane */
void uuUTF32ToUTF16Sup(UTF32 in, UTF16 out[2]);

/*
Converts an input UTF-8 octet stream into a wide character output stream. 
Depending on the underlying implementation wchar_t is either UTF-32 (OSX, Posix, Linux) or UTF-16 (Windows) 

Upon completion cchInOut is set to the number of individual wchar_t values written to output.
NOTE: Surrogate pairs (win32) are counted as two characters. */
void uuUTF8ToWide(const UTF8 *in, const UTF8 *end, wchar_t *out, size_t *cchOut);

/*
Converts a wide character stream to UTF-8. 
Upon completion cbInOut is set to the number of bytes written to output. */
void uuWideToUTF8(const wchar_t *in, const wchar_t *end, UTF8 *out, size_t *cbInOut);

#ifdef __cplusplus
}
#endif

#endif
