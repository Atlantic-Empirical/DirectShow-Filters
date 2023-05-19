#include "appincludes.h"

HRESULT CKeystone::BUSA_SaveBufferToFile(const char * FileName, const char * Mode, LPBYTE buf, int bufsize, int w, int h)
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

HRESULT CKeystone::BUSA_SaveBufferToFile_A(const char * FileName, const char * Mode, LPBYTE buf, int length)
{
	FILE * Out = fopen(FileName, Mode);
	fwrite(buf, length, 1, Out);
	fclose(Out);
	return S_OK;
}

HRESULT CKeystone::BUSA_SaveBitmapRasterData(LPBYTE src)
{
	CRefTime crtST;
	StreamTime(crtST);

	TCHAR buff[256];
	_stprintf(buff, _T("C:\\Temp\\%I64d.bin"), crtST.Millisecs());

	LONG TargSize = 1049760;
	LPBYTE pRGB = (LPBYTE)malloc(TargSize);
	CSCManual::UYVYToRGB24(src, 720, 486, pRGB);
	BUSA_SaveBufferToFile_A(buff, "wb", pRGB, TargSize);
	free(pRGB);
	return S_OK;
}
