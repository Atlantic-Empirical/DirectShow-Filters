#include "appincludes.h"
#include <atlimage.h>
#include "Mixer.h"

typedef __int64 REFERENCE_TIME;

void CKeystone::GPTC_RTtoBMP(HDC hDC, HBITMAP hBM, HFONT hFont, REFERENCE_TIME rt, double fps, DWORD fgcolor, DWORD bgcolor)
{
	int h, m, s, f;
	UTMT_REFERENCE_TIMEtoHMSF(rt, fps, h, m, s, f);

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

void CKeystone::GPTC_Free(HDC& hDC, HBITMAP& hBM, HFONT& hFont)
{
	if(hFont) {DeleteObject(hFont); hFont = NULL;}
	if(hBM) {DeleteObject(hBM); hBM = NULL;}
	if(hDC) {DeleteDC(hDC); hDC = NULL;}
}

bool CKeystone::GPTC_Init(HDC& hDC, HBITMAP& hBM, HFONT& hFont, int w, int h)
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

	GPTC_Free(hDC, hBM, hFont);

	return false;
}

int CKeystone::_tmain(int argc, _TCHAR* argv[])
{
	HDC hDC;
	HBITMAP hBM;
	HFONT hFont;
	if(!GPTC_Init(hDC, hBM, hFont, 100, 16))
		return 0;
	
	GPTC_RTtoBMP(hDC, hBM, hFont, 1234567890i64, 23.976);

	//CImage img;
	//img.Attach(hBM);
	//img.Save(_T("c:\\bm.bmp"), ImageFormatBMP);
	//img.Detach();

	GPTC_Free(hDC, hBM, hFont);
	return 0;
}

HRESULT CKeystone::GPTC_MixGOPTimecode(LPBYTE dst, IMediaSample * pOut, int w, int h)
{
	HRESULT hr = S_OK;
	//GET GOP TIMECODE
	LONGLONG TStart = 0;
	LONGLONG TEnd = 0;
    pOut->GetMediaTime(&TStart, &TEnd);
	if ((TStart>0) || (TEnd>0)) 
	{
		////DEBUGGING
		//int h, m, s, f;
		//RTToHMSF(TStart, 29.97, h, m, s, f);
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: SH GOP TC: %d:%d:%d;%d"), h, m, s, f));
		////DEBUGGING
		
		if (TStart == GPTC_LastGOPHeaderTimecodeValue)
		{
			//Increment it
			if ((FRRT_TargetFramerate_In_ATPF == 333667) || (FRRT_TargetFramerate_In_ATPF == 333666) || (FRRT_TargetFramerate_In_ATPF == 417084) || (FRRT_TargetFramerate_In_ATPF == 417083))
			{
				GPTC_ATPFSinceLastGOPHeader += 333667;
			}
			else
			{
				GPTC_ATPFSinceLastGOPHeader += FRRT_TargetFramerate_In_ATPF;
			}
			TStart += GPTC_ATPFSinceLastGOPHeader;
		}
		else
		{
			//New MPEG GOP TC
			GPTC_LastGOPHeaderTimecodeValue = TStart;
			GPTC_ATPFSinceLastGOPHeader = 0;
		}

		int FPS = 0;
		double dFPS = 0;
		if ((FRRT_TargetFramerate_In_ATPF == 333667) || (FRRT_TargetFramerate_In_ATPF == 333666) || (FRRT_TargetFramerate_In_ATPF == 417084) || (FRRT_TargetFramerate_In_ATPF == 417083))
		{
			FPS = 2997;
			dFPS = 29.97;
		}
		else if (FRRT_TargetFramerate_In_ATPF == 400000)
		{
			FPS = 25;
			dFPS = 25;
		}

		int h, m, s, f;
		UTMT_REFERENCE_TIMEtoHMSF(TStart, dFPS, h, m, s, f);

		LONG_PTR out = (h<<24) | (m<<16) | (s<<8) | f;

		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: GOP TC: %d - %d"), TStart, TEnd));
		NotifyEvent(EC_KEYSTONE_MPEGTC, out, (LONG_PTR)FPS);
	}
	else
	{
		goto SkipGOPTCMix;
	}

	//Mix GOP Timecodes
	if (USEROPTION_MixGOPTimecode > 0)
	{
		double dOut = 0;
		if ((FRRT_TargetFramerate_In_ATPF == 333667) || (FRRT_TargetFramerate_In_ATPF == 333666) || (FRRT_TargetFramerate_In_ATPF == 417084) || (FRRT_TargetFramerate_In_ATPF == 417083))
		{
			dOut = 29.97;
		}
		else if (FRRT_TargetFramerate_In_ATPF == 400000)
		{
			dOut = 25;
		}

		//Create bmp of ref time
		if ((GPTC_ATPFSinceLastGOPHeader == 0) && (USEROPTION_MixGOPTimecode_RedIFrames == 1))
		{
			//red
			GPTC_RTtoBMP(GPTC_BurnIn_hDC, GPTC_BurnIn_hBM, GPTC_BurnIn_hFONT, (REFERENCE_TIME)TStart, dOut, 0x0000ff, 0x000000);
		}
		else
		{
			//green
			GPTC_RTtoBMP(GPTC_BurnIn_hDC, GPTC_BurnIn_hBM, GPTC_BurnIn_hFONT, (REFERENCE_TIME)TStart, dOut, 0x00ff00, 0x000000);		
		}

		//Get a bitmap from the hbitmap
		BITMAP bm;
		GetObject(GPTC_BurnIn_hBM, sizeof(bm), &bm);

		//CSC
		LPBYTE GOPTC_UYVY = (LPBYTE)malloc(bm.bmHeight * bm.bmWidth * 2); //maybe needed	
		CSCManual::RGB32ToUYVY((LPBYTE)bm.bmBits, bm.bmWidthBytes, bm.bmWidth, bm.bmHeight, GOPTC_UYVY);

		//Mix
		CMixer::MixUYVY_OSD(GOPTC_UYVY, 100, 16, dst, w, h, 540, 30, 0x00000000);
		free(GOPTC_UYVY);
	}
SkipGOPTCMix:
	return hr;
}

