#include "appincludes.h"

//Just copies identical data. No change.
void CKeystone::CopyBuffer_Rect(LPBYTE src, LPBYTE dst, int pitch, int w, int h)
{
	for (int i = 0; i < h; i++)
	{
		memcpy(dst, src, pitch);
		src += pitch;
		dst += pitch;
	}
}

//void CKeystone::YUVSquare(LPBYTE dst, int x, int y, int dstpitch, int w, int h)
//{
//	dst += x * 2 + y * dstpitch;
//	memset(dst, 255, w * 2);
//	for (int i = 0; i < h; i++)
//	{
//		dst[0] = 255;
//		dst[1] = 255;
//		dst[(w - 1) * 2] = 255;
//		dst[(w - 1) * 2 + 1] = 255;
//		dst += dstpitch;
//	}
//	memset(dst, 255, w * 2);
//}

void CKeystone::CopyBuffer_Rect_UYVY(LPBYTE src, int srcx, int srcy, int srcpitch, LPBYTE dst, int dstx, int dsty, int dstpitch, int w, int h)
{
	src += srcx * 2 + srcy * srcpitch;
	dst += dstx * 2 + dsty * dstpitch;
	for (int i = 0; i < h; i++)
	{
		memcpy(dst, src, w * 2);
		src += srcpitch;
		dst += dstpitch;
	}
}

void CKeystone::CopyBuffer_Rect_YUY2toUYVY(const BYTE* src, int srcx, int srcy, int srcpitch, LPBYTE dst, int dstx, int dsty, int dstpitch, int w, int h)
{
	src += srcx * 2 + srcy * srcpitch;
	dst += dstx * 2 + dsty * dstpitch;
	for (int i = 0; i < h; i++)
	{
		const BYTE* src1 = src;
		LPBYTE dst1 = dst;
		// copy the line
		for(int j = 0; j < w; j++)
		{
			dst1[0] = src1[1];
			dst1[1] = src1[0];
			src1 += 2;
			dst1 += 2;
		}
		src += srcpitch;
		dst += dstpitch;
	}
}

void CKeystone::CopyBuffer_Line_YUY2toUYVY(const BYTE* src, LPBYTE dst, int w)
{
	const BYTE* src1 = src;
	LPBYTE dst1 = dst;
	for(int j = 0; j < w; j++)
	{
		dst1[0] = src1[1];
		dst1[1] = src1[0];
		src1 += 2;
		dst1 += 2;
	}
}
