#include "appincludes.h"

HRESULT CKeystone::SetBufferColor_YUY2(LPBYTE dst, int dstpitch, int w, int h)
{
	for (LPBYTE i = dst; i < dst + (dstpitch * h); i += 4)
	{
		//Truer black
		memset(i	, 0x00, 1);	//U: Blue - Luma
		memset(i + 1, 0x80, 1);	//Y: Luma
		memset(i + 2, 0x00, 1);	//V: Red - Luma
		memset(i + 3, 0x80, 1);	//Y: Luma

		////green
		//memset(i	, 0, 1);	//U: Blue - Luma
		//memset(i + 1, 0, 1);	//Y: Luma
		//memset(i + 2, 0, 1);	//V: Red - Luma
		//memset(i + 3, 0, 1);	//Y: Luma
	}

	return S_OK;
}