#include <appincludes.h>

bool CKeystoneVideoOnePin::ShrinkV(LPBYTE dst, int dh, int dstpitch, LPBYTE src, int sh, int srcpitch, int wbytes)
{
	for(int j = 0, y = 0, dy = (sh<<16)/dh; j < dh; j++, y += dy)
	{
		BYTE* s1 = src + (y>>16) * srcpitch;
		BYTE* s2 = s1 + srcpitch;
		BYTE* d1 = dst;
		BYTE* d2 = d1 + dstpitch;

		if(int yfrac = y & 0xffff)
		{
			for(int i = 0; i < wbytes; i++)
			{
				*d1++ = ((*s1++ * (0xffff-yfrac) + *s2++ * yfrac) + 0x7fff) >> 16;
			}
		}
		else
		{
			memcpy(d1, s1, wbytes);
		}

		dst = d2;
	}

	return true;
}

bool CKeystoneVideoOnePin::ShrinkV(LPBYTE dst, int dh, LPBYTE src, int sh, int w, BOOL bInterlaced)
{
	int pitch = w*2;
	int wbytes = w*2;

	if(bInterlaced)
	{
		if((dh&1) || (sh&1)
		|| !ShrinkV(dst, dh/2, pitch*2, src, sh/2, pitch*2, wbytes)
		|| !ShrinkV(dst + pitch, dh/2, pitch*2, src + pitch, sh/2, pitch*2, wbytes))
			return false;
	}
	else
	{
		if(!ShrinkV(dst, dh, pitch, src, sh, pitch, wbytes))
			return false;
	}

	return true;
}