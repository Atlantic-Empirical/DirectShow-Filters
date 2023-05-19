#include "appincludes.h"
#include <atlimage.h>
#include "Mixer.h"

bool CKeystone::TRWA_InitTrialWarningMixing(HDC& hDC, HBITMAP& hBM, HFONT& hFont, int w, int h)
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
	lf.lfHeight = w/3; //280;
	lf.lfWeight = FW_BOLD;
	_tcscpy(lf.lfFaceName, _T("Arial"));
	hFont = CreateFontIndirect(&lf);

	if(hDC && hBM && hFont)
		return true;

	GPTC_Free(hDC, hBM, hFont);

	return false;
}

//bool CKeystone::TRWA_InitTrialWarningMixing(HDC& hDC, HBITMAP& hBM, HFONT& hFont, int w, int h)
//{
//	if(HDC hDCScreen = GetDC(0))
//	{
//		if(hDC = CreateCompatibleDC(hDCScreen))
//		{
//			struct {BITMAPINFOHEADER bmiHeader; DWORD bmiColors[256];} bmi;
//			memset(&bmi, 0, sizeof(bmi));
//			bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
//			bmi.bmiHeader.biWidth = w;
//			bmi.bmiHeader.biHeight = -h;
//			bmi.bmiHeader.biPlanes = 1;
//			bmi.bmiHeader.biBitCount = 32;
//			bmi.bmiHeader.biCompression = BI_BITFIELDS;
//			bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth* abs(bmi.bmiHeader.biHeight) *bmi.bmiHeader.biBitCount/8;
//			bmi.bmiColors[0] = 0xff0000;
//			bmi.bmiColors[1] = 0x00ff00;
//			bmi.bmiColors[2] = 0x0000ff;
//			void* pvBits = NULL;
//			hBM = ::CreateDIBSection(hDC, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
//		}
//
//		ReleaseDC(0, hDCScreen);
//	}
//
//	LOGFONT lf;
//	memset(&lf, 0, sizeof(lf));
//	lf.lfHeight = 80;
//	lf.lfWeight = FW_BOLD;
//	_tcscpy(lf.lfFaceName, _T("Arial"));
//	hFont = CreateFontIndirect(&lf);
//
//	if(hDC && hBM && hFont)
//		return true;
//
//	GPTC_Free(hDC, hBM, hFont);
//
//	return false;
//}
//
void CKeystone::TRWA_FreeTrialWarningMixing(HDC& hDC, HBITMAP& hBM, HFONT& hFont)
{
	if(hFont) {DeleteObject(hFont); hFont = NULL;}
	if(hBM) {DeleteObject(hBM); hBM = NULL;}
	if(hDC) {DeleteDC(hDC); hDC = NULL;}
}

HRESULT CKeystone::TRWA_MixTrialWarning(LPBYTE dst, IMediaSample * pOut, int w, int h)
{
	if (((int)&*TRWA_BurnIn_hDC == 0xcdcdcdcd) || ((int)&*TRWA_BurnIn_hDC == 0))
	{
		TRWA_InitTrialWarningMixing(TRWA_BurnIn_hDC, TRWA_BurnIn_hBM, TRWA_BurnIn_hFONT, w, h);
		//TRWA_InitTrialWarningMixing(TRWA_BurnIn_hDC, TRWA_BurnIn_hBM, TRWA_BurnIn_hFONT, PIVI_ConnectedWidth/3, PIVI_ConnectedHeight/3);
	}

	HRESULT hr = S_OK;

	UTMT_TEXTtoBMP(TRWA_BurnIn_hDC, TRWA_BurnIn_hBM, TRWA_BurnIn_hFONT, _T("TRIAL"), 0x222222, 0x000000);

	//Get a bitmap from the hbitmap
	BITMAP bm;
	GetObject(TRWA_BurnIn_hBM, sizeof(bm), &bm);

	//CSC
	LPBYTE GOPTC_UYVY = (LPBYTE)malloc(bm.bmHeight * bm.bmWidth * 2);
	CSCManual::RGB32ToUYVY((LPBYTE)bm.bmBits, bm.bmWidthBytes, bm.bmWidth, bm.bmHeight, GOPTC_UYVY);

	//Mix
	CMixer::MixUYVY_OSD(GOPTC_UYVY, bm.bmWidth, bm.bmHeight, dst, w, h, 0, 0, 0x00800080);
	free(GOPTC_UYVY);

	return hr;
}

