#include "uunicode.h"

THREADLOCAL jmp_buf _uuException;
THREADLOCAL int _uuErrno = UUERR_SUCCESS;

static void _uuSetError(int error)
{
  longjmp(_uuException, error);
}

UTF16 uuUTF16ToLE(UTF16 in)
{
#ifdef __LITTLE_ENDIAN__
  return in;
#else
  return BSWAP_U16(in);
#endif
}

UTF32 uuUTF32ToLE(UTF32 in)
{
#ifdef __LITTLE_ENDIAN__
  return in;
#else
  return BSWAP_U32(in);
#endif
}

UTF16 uuUTF16ToBE(UTF16 in)
{
#ifdef __LITTLE_ENDIAN__
  return BSWAP_U16(in);
#else
  return in;
#endif
}

UTF32 uuUTF32ToBE(UTF32 in)
{
#ifdef __LITTLE_ENDIAN__
  return BSWAP_U32(in);
#else
  return in;
#endif
}

UTF16 uuUTF16LEToNA(UTF16 in)
{
#ifdef __LITTLE_ENDIAN__
  return in;
#else
  return BSWAP_U16(in);
#endif
}

UTF32 uuUTF32LEToNA(UTF32 in)
{
#ifdef __LITTLE_ENDIAN__
  return in;
#else
  return BSWAP_U32(in);
#endif
}

UTF16 uuUTF16BEToNA(UTF16 in)
{
#ifdef __LITTLE_ENDIAN__
  return BSWAP_U16(in);
#else
  return in;
#endif
}

UTF32 uuUTF32BEToNA(UTF32 in)
{
#ifdef __LITTLE_ENDIAN__
  return BSWAP_U32(in);
#else
  return in;
#endif
}

size_t uuUTF32ToUTF8(UTF32 in, UTF8 out[4])
{
  UTF8 *d = out;

  if (in < 0x80) 
  {  
    *d++=  in;
  }
  else 
    if (in < 0x800) 
    { 
      *d++= ((in >> 6) & 0x1F) | 0xC0;
      *d++= ((in >> 0) & 0x3F) | 0x80;
    }
    else 
      if (in < 0x10000) 
      {  
        *d++= ((in >> 12) & 0x0F) | 0xE0;  
        *d++= ((in >> 6) & 0x3F) | 0x80;
        *d++= ((in >> 0) & 0x3F) | 0x80;

      }
      else 
        if (in < 0x200000) 
        {  
          *d++= ((in >> 18) & 0x07) | 0xF0; 
          *d++= ((in >> 12) & 0x3F) | 0x80;
          *d++= ((in >> 6) & 0x3F) | 0x80;
          *d++= ((in >> 0) & 0x3F) | 0x80;
        }
        else
        {
          _uuSetError(UUERR_INVALID_CODEPOINT);
        }

        return (int) (d - out);
}

int uuUTF8ToUTF32(const UTF8 *in, const UTF8 *end, UTF32 *out)
{
  static const UTF8 utf8LengthLookup[256] = 
  {
    /* 0x00 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    /* 0x10 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 0x20 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    /* 0x30 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 0x40 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    /* 0x50 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 0x60 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    /* 0x70 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 0x80 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    /* 0x90 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 0xa0 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    /* 0xb0 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    /* 0xc0 */ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
    /* 0xd0 */ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    /* 0xe0 */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
    /* 0xf0 */ 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 7, 7
  };

  UTF32 ucs;
  UTF8 len = utf8LengthLookup[*in];

  switch (len)
  {
  case 1:
    {
      ucs = *in;
      *out = ucs;
      return 1;
    }
  case 2:
    {
      UTF16 na;

      if (in + 1 > end)
      {
        return UUERR_UNEXPECTED_END_OF_INPUT;
      }

      na = *((UTF16 *) in);

#ifdef __LITTLE_ENDIAN__
      ucs = ((na & 0x1f) << 6) | ((na >> 8) & 0x3f);
#else
      ucs = ((na & 0x1f00) >> 2) | (na & 0x3f);
#endif

      if (ucs < 0x80)
      {
        _uuSetError(UUERR_OVERLONG_UTF8_SEQUENCE);
      }

      *out = ucs;
      return 2;
    }

  case 3:
    {
      UTF32 na;
      if (in + 2 > end)
      {
        _uuSetError(UUERR_UNEXPECTED_END_OF_INPUT);
      }


#ifdef __LITTLE_ENDIAN__
      na = *((UTF32 *) in);
      na |= *((UTF8 *) in + 2) << 16;
      ucs = ((na & 0x0f) << 12) | ((na & 0x3f00) >> 2) | ((na & 0x3f0000) >> 16);
#else
      na = *((UTF32 *) in) << 8;
      na |= *((UTF8 *) in + 2);
      ucs = ((na & 0x0f0000) >> 4) | ((na & 0x3f00) >> 2) | (na & 0x3f);
#endif

      if (ucs < 0x800)
      {
        _uuSetError(UUERR_OVERLONG_UTF8_SEQUENCE);
      }

      *out = ucs;
      return 3;
    }

  case 4:
    {
      UTF32 na;

      if (in + 3 > end)
      {
        _uuSetError(UUERR_UNEXPECTED_END_OF_INPUT);
      }

#ifdef __LITTLE_ENDIAN__
      na = *((UTF32 *) in);
      ucs = ((na & 0x07) << 18) | ((na & 0x3f00) << 4) | ((na & 0x3f0000) >> 10) | ((na & 0x3f000000) >> 24);
#else
      in = *((UTF32 *) in);
      ucs = ((na & 0x07000000) >> 6) | ((na & 0x3f0000) >> 4) | ((na & 0x3f00) >> 2) | (na & 0x3f);
#endif
      if (ucs < 0x10000)
      {
        _uuSetError(UUERR_OVERLONG_UTF8_SEQUENCE);
      }

      *out = ucs;
      return 4;
    }

  //case 0:
  //  _uuSetError(UUERR_SUCCESS);

  case 5:
  case 6:
    _uuSetError(UUERR_INVALID_UTF8_SEQUENCE_LENGTH);

  default:
    _uuSetError(UUERR_BOM_DETECTED);


  }

  // Keep compiler happy
  return 0;
}

int uuUTF16ToUTF32(const UTF16 *in, const UTF16 *end, UTF32 *out)
{
  int iSur = 0;

  if ((*in & 0xfc00) == 0xd800)
  {
    UTF16 first;
    if (in + 1 >= end)
    {
      _uuSetError(UUERR_UNEXPECTED_END_OF_INPUT);
    }

    first = *in;
    in ++;
    if ((*in & 0xfc00) != 0xdc00)
    {
      _uuSetError(UUERR_UNMATCHED_LEAD_SURROGATE);
    }


    *out = 0x10000 + (((first - 0xd800) << 10) | (*in - 0xdc00));
    return 2;
  }
  else
  {
    *out = *in;
    return 1;
  }
}

void uuUTF32ToUTF16Sup(UTF32 in, UTF16 out[2])
{
  in -= 0x10000;
  out[0] = (in >> 10) + 0xd800;
  out[1] = (in & 0x3ff) + 0xdc00;
}

int uuUTF32ToUTF16(UTF32 in, UTF16 out[2])
{
  if (in > 0x10000)
  {
    uuUTF32ToUTF16Sup(in, out);
    return 2;
  }
  else
  {
    out[0] = in;
    return 1;
  }
}

void uuWideToUTF8(const wchar_t *in, const wchar_t *end, UTF8 *out, size_t *cbInOut)
{
  UTF32 chr;
  UTF8 *outOrg = out;

  while (in < end)
  {

#if WCHAR_MAX == 0xffff
    in += uuUTF16ToUTF32((UTF16 *)in, (UTF16 *)end, &chr);
#else
    chr = *in;
    in ++;
#endif
    
    out += uuUTF32ToUTF8(chr, out);
  }

  *cbInOut = out - outOrg;
}

void uuUTF8ToWide(const UTF8 *in, const UTF8 *end, wchar_t *out, size_t *cchInOut)
{
  int utf8len;
  UTF32 chr;
  wchar_t *outEnd = out + *cchInOut;
  wchar_t *outOrg = out;

  while (in < end)
  {
    utf8len = uuUTF8ToUTF32(in, end, &chr);

#if WCHAR_MAX == 0xffff
    if (chr > 0x10000)
    {
      if (out + 2 > outEnd)
      {
        _uuSetError(UUERR_OUTPUT_OVERFLOW);
      }

      uuUTF32ToUTF16Sup(chr, (UTF16 *) out);
      out += 2;
    }
    else
    {
      if (out + 1 >= outEnd)
      {
        _uuSetError(UUERR_OUTPUT_OVERFLOW);
      }

      *out = (wchar_t) chr;
      out ++;
    }
#else
    if (out + 1 >= outEnd)
    {
      _uuSetError(UUERR_OUTPUT_OVERFLOW);
    }

    *out = (wchar_t) chr;
    out ++;
#endif
    in += utf8len;
  }

  *cchInOut = out - outOrg;
}

