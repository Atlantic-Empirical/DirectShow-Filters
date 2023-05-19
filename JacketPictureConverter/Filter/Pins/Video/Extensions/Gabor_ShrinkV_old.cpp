#include <appincludes.h>

bool CKeystoneVideoPin::ShrinkV(LPBYTE dst, int dh, LPBYTE src, int sh, int w)
{
	int pitch = w*2;

	for(int j = 0, y = 0, dy = (sh<<16)/dh; j < dh; j++, y += dy)
	{
		BYTE* s1 = src + (y>>16) * pitch;
		BYTE* s2 = s1 + pitch;
		BYTE* d1 = dst;
		BYTE* d2 = d1 + pitch;

		if(int yfrac = y & 0xffff)
		{
			while(d1 < d2)
			{
				*d1++ = ((*s1++ * (0xffff-yfrac) + *s2++ * yfrac) + 0x7fff) >> 16;
			}
		}
		else
		{
			memcpy(d1, s1, pitch);
		}

		dst = d2;
	}

	return true;
}