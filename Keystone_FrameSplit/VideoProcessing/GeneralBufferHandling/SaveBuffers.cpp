#include "../../Utility/appincludes.h"

HRESULT CKeystone::SaveBufferToFile(const char * FileName, const char * Mode, LPBYTE buf, int bufsize, int w, int h)
{
	FILE * Out = fopen(FileName, Mode);
	int pitch = bufsize / h;
	for (int i = 0; i < h; i++)
	{
		fwrite(buf, pitch, 1, Out);
		buf += pitch;
	}
	fclose(Out);
	return S_OK;
}

HRESULT CKeystone::SaveBufferToFile_A(const char * FileName, const char * Mode, LPBYTE buf, int length)
{
	FILE * Out = fopen(FileName, Mode);
	fwrite(buf, length, 1, Out);
	fclose(Out);
	return S_OK;
}

HRESULT CKeystone::TellPhoenixToGetFrameGrab(IMediaSample * pSample, int w, int h, int UYVY)
{
	LPBYTE tmp = NULL;
	pSample->GetPointer(&tmp);
	LPBYTE ConversionTarget = 0;
	ConversionTarget = (LPBYTE)malloc(w*h*3);
	if (UYVY==1)
	{
		CSCManual::UYVYToRGB24(tmp, w, h, ConversionTarget);
	}
	else
	{
		CSCManual::YUY2ToRGB24(tmp, w, h, ConversionTarget);
	}
	NotifyEvent(EC_KEYSTONE_FORCEFRAMEGRAB, (LONG_PTR)ConversionTarget, w*h*3);

	//Currently we are not freeing ConversionTarget so it's a leak.
	//Just don't use this function too much.

	return S_OK;
}


HRESULT CKeystone::SaveBitmapRasterData(LPBYTE src)
{
	CRefTime crtST;
	StreamTime(crtST);

	TCHAR buff[256];
	_stprintf(buff, _T("F:\\Temp\\%I64d.bin"), crtST.Millisecs());

	LONG TargSize = 6220800;
	LPBYTE pRGB = (LPBYTE)malloc(TargSize);
	CSCManual::UYVYToRGB24(src, 1920, 1080, pRGB);
	SaveBufferToFile_A(buff, "wb", pRGB, TargSize);
	free(pRGB);
	return S_OK;
}
