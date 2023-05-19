#include "appincludes.h"
#include <atlimage.h>
#include "Mixer.h"

void CKeystone::TCKS_BurnInTicks(LPBYTE dst, int w, int h)
{
	TCHAR buff[256];
	//_stprintf_s(buff, sizeof(buff)/sizeof(buff[0]), _T("%02d:%02d:%02d.%02d\n"), h, m, s, f);
	_stprintf(buff, _T("%I32d"), GetTickCount());

	SetBkMode(TCKS_BurnIn_hDC, OPAQUE);
	SetBkColor(TCKS_BurnIn_hDC, 0x00ff00);
	SetTextColor(TCKS_BurnIn_hDC, 0x000000);

	HGDIOBJ hBMOld = SelectObject(TCKS_BurnIn_hDC, TCKS_BurnIn_hBM);
	HGDIOBJ hOldFont = SelectObject(TCKS_BurnIn_hDC, TCKS_BurnIn_hFONT);

	BITMAP bm;
	GetObject(TCKS_BurnIn_hBM, sizeof(bm), &bm);

	SIZE size;
	::GetTextExtentPoint32(TCKS_BurnIn_hDC, buff, _tcslen(buff), &size);

	TextOut(TCKS_BurnIn_hDC, (bm.bmWidth - size.cx) / 2, (abs(bm.bmHeight) - size.cy) / 2, buff, _tcslen(buff));

	SelectObject(TCKS_BurnIn_hDC, hOldFont);
	SelectObject(TCKS_BurnIn_hDC, hBMOld);

	GdiFlush();

	//Get a bitmap from the hbitmap
	//BITMAP bm;
	GetObject(TCKS_BurnIn_hBM, sizeof(bm), &bm);

	//CSC
	LPBYTE GOPTC_UYVY = (LPBYTE)malloc(bm.bmHeight * bm.bmWidth * 2); //maybe needed	
	CSCManual::RGB32ToUYVY((LPBYTE)bm.bmBits, bm.bmWidthBytes, bm.bmWidth, bm.bmHeight, GOPTC_UYVY);

	//Mix
	CMixer::MixUYVY_OSD(GOPTC_UYVY, 100, 16, dst, w, h, 540, 30, 0x00000000);
	free(GOPTC_UYVY);
}
