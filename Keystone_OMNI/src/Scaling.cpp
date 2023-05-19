#include "appincludes.h"

HRESULT CKeystone::SCLE_SetupVideoSizing()
{
	if ((SCLE_LB_OK==0) && (SCLE_PS_OK==0))
	{
		SCLE_ActiveResizeMode = 1;
		return S_OK;
	}

	//Check for current AR mode
	//int ResizeMode; //1=anamorphic, 2=PanScan, 3=Letterbox
	DWORD dwCF =  PIVI_VIH2->dwControlFlags;
	int X = PIVI_VIH2->dwPictAspectRatioX;
	int Y = PIVI_VIH2->dwPictAspectRatioY;
	DbgLog((LOG_TRACE, 0, TEXT("Keystone: NewAR. DesResizeMode: %d  ResizeMode: %d  dwCF: %d  X: %d  Y: %d  L: %d  R: %d"), SCLE_DesiredResizeMode, SCLE_ActiveResizeMode, dwCF, X, Y, PIVI_VIH2->rcSource.left, PIVI_VIH2->rcSource.right));

	if (PIVI_VIH2->rcSource.left == 90) X = 16;

    switch(SCLE_DesiredResizeMode)
    {
        case 1: //Anamorphic
        {
			SCLE_ActiveResizeMode = 1;
			DbgLog((LOG_TRACE, 0, TEXT("Keystone: OUTPUT - no change")));
			break;
		}

        case 2: //Panscan
        {
CasePanscan:
			if (X==4) //Source is 4x3
			{
				SCLE_ActiveResizeMode = 1;
				DbgLog((LOG_TRACE, 0, TEXT("Keystone: OUTPUT - no change")));				
				break;
			}
			else
			{
				if (SCLE_PS_OK==1)
				{
					SCLE_ActiveResizeMode = 2;
					DbgLog((LOG_TRACE, 0, TEXT("Keystone: OUTPUT - panscanned")));
					break;
				}
				else
				{
					goto CaseLetterbox;
				}
			}
		}

		case 3: //Letterbox
		{
CaseLetterbox:
			if (X==4) //Source is 4x3
			{
				SCLE_ActiveResizeMode = 1;
				DbgLog((LOG_TRACE, 0, TEXT("Keystone: OUTPUT - no change")));				
				break;
			}
			else
			{
				if (SCLE_LB_OK==1)
				{
					SCLE_ActiveResizeMode = 3;
					DbgLog((LOG_TRACE, 0, TEXT("Keystone: OUTPUT - letterboxed")));
					break;
				}
				else
				{
					goto CasePanscan;
				}
			}
		}
	}

	return S_OK;
}

HRESULT CKeystone::SCLE_Resize(LPBYTE src)
{
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone: Resize: %d"), ResizeMode));

	//ResizeMode = 1; //do nothing

	if (SCLE_ActiveResizeMode == 2)
	{
		//Size 16x9 content to panscan
		SCLE_ZoomYUY2(src, PIVI_VIH2->bmiHeader.biWidth, PIVI_VIH2->bmiHeader.biHeight, PIVI_VIH2->rcTarget);
	}
	else if (SCLE_ActiveResizeMode == 3)
	{
		//size 16x9 content to letterbox
		int h = PIVI_VIH2->bmiHeader.biHeight;
		int w = PIVI_VIH2->bmiHeader.biWidth;
		int pitch = 1440;

		//prep
		LPBYTE tmp = (LPBYTE)malloc(h * pitch);
		CopyBuffer_Rect(src, tmp, pitch, w, h);

		//set bg color for desired lb color
		char* from = (char*)&SCLE_BarColor;
		char dest[] = "0000";
		swab(from, dest, 4);

		LPBYTE tOrig = src;
		for(int j = 0; j < h; j++, tOrig += pitch) 
		{ 
			WORD* s = (WORD*) tOrig; 
			for(int i = 0; i < w; i += 2, s += 2) 
			{
				s[0] = (WORD) ((WORD)(dest[3]<<8) | (BYTE)(dest[2])); 
				s[1] = (WORD) ((WORD)(dest[1]<<8) | (BYTE)(dest[0]));
			} 
		}

		//set write address to appropriate starting position after the top LB
		BYTE adj = 0;
		if (h==576) //PAL
		{
			adj = 72;
		}
		else //NTSC
		{
			adj = 60;
		}
		LPBYTE TmpWrite = src + (adj * pitch);
		LPBYTE TmpRead = tmp;

		SCLE_ShrinkV(TmpWrite, h-(adj*2), TmpRead, h, w, true);
		free(tmp);
	}
	return S_OK;
}
