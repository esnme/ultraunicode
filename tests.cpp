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

#include "uunicode.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

void testAssert(const char *expr, int line, const char *file)
{
	fprintf (stderr, "%s:%d: %s\n", file, line, expr);
	getchar();
	abort();
}

#define ASSERT(_x) if (!(_x)) testAssert(#_x, __LINE__, __FILE__);

void test_UTF32ToUTF8()
{
  UTF8 utf8[5];
  UTF32 utf32;
  uuUTF32ToUTF8(0xc5, utf8);
  uuUTF8ToUTF32(utf8, utf8 + 4, &utf32);
  ASSERT (utf32 == 0xc5);
}

void test_UTF32ToUTF16()
{
  UTF16 utf16[2];
  UTF32 utf32 = 0xc5;
  uuUTF32ToUTF16(utf32, utf16);
  uuUTF16ToUTF32(utf16, utf16 + 2, &utf32);
  ASSERT (utf32 == 0xc5);
}

void test_UTF32ToUTF8Supplementary()
{
  UTF8 utf8[5];
  UTF32 utf32;
  uuUTF32ToUTF8(0x2070e, utf8);
  uuUTF8ToUTF32(utf8, utf8 + 4, &utf32);
  ASSERT (utf32 == 0x2070e);
}

void test_UTF16Surrogate()
{
  UTF16 utf16[2];
  UTF32 utf32 = 0x2070e;
  uuUTF32ToUTF16(utf32, utf16);
  uuUTF16ToUTF32(utf16, utf16 + 2, &utf32);
  ASSERT (utf32 == 0x2070e);
}

void test_DetectBOM()
{
  UTF8 utf8[5];
  UTF32 utf32;
  utf8[0] = 0xfe;
  int hasError = 0;

  UUNICODE_TRY()
  {
    uuUTF8ToUTF32(utf8, utf8+1, &utf32);
  }
  UUNICODE_CATCH_SUCCESS()
  {
  }
  UUNICODE_CATCH_ERROR()
  {
    hasError = UUNICODE_ERRNO();   
  }
  ASSERT (hasError == UUERR_BOM_DETECTED);
}


void test_UTF8ToWide()
{
  UTF8 utf8[5];
  wchar_t wideStr[2];
  size_t wideLen = 2;

  utf8[0] = 0xf0;
  utf8[1] = 0xa0;
  utf8[2] = 0x9c;
  utf8[3] = 0x8e;

  uuUTF8ToWide(utf8, utf8 + 4, wideStr, &wideLen);

#if WCHAR_MAX == 0xffff
  ASSERT (wideLen == 2);
#else
  ASSERT (wideLen == 1);
#endif

}

int main (int argc, char **argv)
{
  test_UTF32ToUTF8();
  test_UTF32ToUTF16();
  test_UTF32ToUTF8Supplementary();
  test_UTF16Surrogate();
  test_DetectBOM();
  test_UTF8ToWide();

  fprintf (stderr, "All tests successful!\n");

  return 0;
}
