#include "../../Utility/appincludes.h"

HRESULT CKeystone::SetBufferBGColor(LPBYTE dst, int dstpitch, DWORD w, DWORD h)
{
	//dst is a pointer to the first byte of the target for the image processing
	//we just want to set the last six lines to black so we need to start the for
	//at the address at the end of the actual image. To get this address we start from
	//the dst then go forward by the number of bytes per line (pitch) times the height
	//of the actual image. This usually will give us 691200 (1440 * 480, for 2byte per pixel formats)
	//Then we for our way through the buffer up to the last byte setting every other byte to 0.
	dst += dstpitch * h;
	//for (LPBYTE i = dst; i < dst + (dstpitch * 6); i += 2)
	//{
	//	//Each time through we're handling one UY or VY packet.
	//	//TODO: (Later) Tinker with this to get a 'truer' black.
	//	memset(i, 128, 1);		//Why this has to be 128 I have no idea. But it definately doesn't work with higher or lower values.
	//	memset(i + 1, 0, 1);
	//}
	for (LPBYTE i = dst; i < dst + (dstpitch * 6); i += 4)
	{
		//Truer black
		memset(i	, 0x80, 1);	//U: Blue - Luma
		memset(i + 1, 0x00, 1);	//Y: Luma
		memset(i + 2, 0x80, 1);	//V: Red - Luma
		memset(i + 3, 0x00, 1);	//Y: Luma

		//Each time through we're handling one UY or VY packet.
		//TODO: (Later) Tinker with this to get a 'truer' black.
		//memset(i	, 128, 1);	//U: Blue - Luma
		//memset(i + 1, 16, 1);	//Y: Luma
		//memset(i + 2, 128, 1);	//V: Red - Luma
		//memset(i + 3, 16, 1);	//Y: Luma

		////To set it green
		//memset(i	, 0, 1);	//U: Blue - Luma
		//memset(i + 1, 0, 1);	//Y: Luma
		//memset(i + 2, 0, 1);	//V: Red - Luma
		//memset(i + 3, 0, 1);	//Y: Luma
	}

	return S_OK;
}

HRESULT CKeystone::SetFullBufferBGColorYUY2(LPBYTE dst, int dstpitch, int w, int h)
{
	for (LPBYTE i = dst; i < dst + (dstpitch * h); i += 4)
	{
		//Truer black
		memset(i	, 0x80, 1);	//U: Blue - Luma
		memset(i + 1, 0x00, 1);	//Y: Luma
		memset(i + 2, 0x80, 1);	//V: Red - Luma
		memset(i + 3, 0x00, 1);	//Y: Luma

		////green
		//memset(i	, 0, 1);	//U: Blue - Luma
		//memset(i + 1, 0, 1);	//Y: Luma
		//memset(i + 2, 0, 1);	//V: Red - Luma
		//memset(i + 3, 0, 1);	//Y: Luma
	}

	return S_OK;
}