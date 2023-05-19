#include "../Utility/appincludes.h"
//#include "../Utility/stdafx.h"
//#include <windows.h>
#include <atlimage.h>
//#include <initguid.h>
// #include <GdiplusImaging.h>

//DEFINE_GUID(ImageFormatBMP, 0xb96b3cab,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e);

typedef __int64 REFERENCE_TIME;

void CKeystone::RTToHMSF(REFERENCE_TIME rt, double fps, int& h, int& m, int& s, int& f)
{
	f = ((rt % 10000000) * fps + 5000000) / 10000000; rt /= 10000000;
	s = rt % 60; rt /= 60;
	m = rt % 60; rt /= 60;
	h = rt;
}

void CKeystone::RTtoBMP(HDC hDC, HBITMAP hBM, HFONT hFont, REFERENCE_TIME rt, double fps, DWORD fgcolor, DWORD bgcolor)
{
	int h, m, s, f;
	RTToHMSF(rt, fps, h, m, s, f);

	TCHAR buff[256];
	//_stprintf_s(buff, sizeof(buff)/sizeof(buff[0]), _T("%02d:%02d:%02d.%02d\n"), h, m, s, f);
	_stprintf(buff, _T("%02d:%02d:%02d.%02d"), h, m, s, f);

	SetBkMode(hDC, OPAQUE);
	SetBkColor(hDC, bgcolor);
	SetTextColor(hDC, fgcolor);

	HGDIOBJ hBMOld = SelectObject(hDC, hBM);
	HGDIOBJ hOldFont = SelectObject(hDC, hFont);

	BITMAP bm;
	GetObject(hBM, sizeof(bm), &bm);

	SIZE size;
	::GetTextExtentPoint32(hDC, buff, _tcslen(buff), &size);

	TextOut(hDC, (bm.bmWidth - size.cx) / 2, (abs(bm.bmHeight) - size.cy) / 2, buff, _tcslen(buff));

	SelectObject(hDC, hOldFont);
	SelectObject(hDC, hBMOld);

	GdiFlush();
}

void CKeystone::TextToBMP(HDC hDC, HBITMAP hBM, HFONT hFont, TCHAR buff[256], double fps, DWORD fgcolor, DWORD bgcolor)
{
	SetBkMode(hDC, OPAQUE);
	SetBkColor(hDC, bgcolor);
	SetTextColor(hDC, fgcolor);

	HGDIOBJ hBMOld = SelectObject(hDC, hBM);
	HGDIOBJ hOldFont = SelectObject(hDC, hFont);

	BITMAP bm;
	GetObject(hBM, sizeof(bm), &bm);

	SIZE size;
	::GetTextExtentPoint32(hDC, buff, _tcslen(buff), &size);

	TextOut(hDC, (bm.bmWidth - size.cx) / 2, (abs(bm.bmHeight) - size.cy) / 2, buff, _tcslen(buff));

	SelectObject(hDC, hOldFont);
	SelectObject(hDC, hBMOld);

	GdiFlush();
}
void CKeystone::FreeGOPTC(HDC& hDC, HBITMAP& hBM, HFONT& hFont)
{
	if(hFont) {DeleteObject(hFont); hFont = NULL;}
	if(hBM) {DeleteObject(hBM); hBM = NULL;}
	if(hDC) {DeleteDC(hDC); hDC = NULL;}
}

bool CKeystone::InitGOPTC(HDC& hDC, HBITMAP& hBM, HFONT& hFont, int w, int h)
{
	if(HDC hDCScreen = GetDC(0))
	{
		if(hDC = CreateCompatibleDC(hDCScreen))
		{
			struct {BITMAPINFOHEADER bmiHeader; DWORD bmiColors[256];} bmi;
			memset(&bmi, 0, sizeof(bmi));
			bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
			bmi.bmiHeader.biWidth = w;
			bmi.bmiHeader.biHeight = -h;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_BITFIELDS;
			bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth* abs(bmi.bmiHeader.biHeight) *bmi.bmiHeader.biBitCount/8;
			bmi.bmiColors[0] = 0xff0000;
			bmi.bmiColors[1] = 0x00ff00;
			bmi.bmiColors[2] = 0x0000ff;
			void* pvBits = NULL;
			hBM = ::CreateDIBSection(hDC, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
		}

		ReleaseDC(0, hDCScreen);
	}

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	lf.lfHeight = 20;
	lf.lfWeight = FW_BOLD;
	_tcscpy(lf.lfFaceName, _T("Arial"));
	hFont = CreateFontIndirect(&lf);

	if(hDC && hBM && hFont)
		return true;

	FreeGOPTC(hDC, hBM, hFont);

	return false;
}

int CKeystone::_tmain(int argc, _TCHAR* argv[])
{
	HDC hDC;
	HBITMAP hBM;
	HFONT hFont;
	if(!InitGOPTC(hDC, hBM, hFont, 100, 16))
		return 0;
	
	RTtoBMP(hDC, hBM, hFont, 1234567890i64, 23.976);

	//CImage img;
	//img.Attach(hBM);
	//img.Save(_T("c:\\bm.bmp"), ImageFormatBMP);
	//img.Detach();

	FreeGOPTC(hDC, hBM, hFont);

	return 0;
}
