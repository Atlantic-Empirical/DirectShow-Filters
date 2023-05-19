#include "appincludes.h"

//int CKeystone::UTMT_GetRectWidth(const RECT& r)
//{
//	return r.right - r.left;
//}

//int CKeystone::UTMT_GetRectHeight(const RECT& r)
//{
//	return r.bottom - r.top;
//}

void CKeystone::UTMT_TEXTtoBMP(HDC hDC, HBITMAP hBM, HFONT hFont, const char * str, DWORD fgcolor, DWORD bgcolor)
{
	TCHAR buff[256];
	_stprintf(buff, str);

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

void CKeystone::UTMT_REFERENCE_TIMEtoHMSF(REFERENCE_TIME rt, double fps, int& h, int& m, int& s, int& f)
{
	f = ((rt % 10000000) * fps + 5000000) / 10000000; rt /= 10000000;
	s = rt % 60; rt /= 60;
	m = rt % 60; rt /= 60;
	h = rt;
}
