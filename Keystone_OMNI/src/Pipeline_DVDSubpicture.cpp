#include "appincludes.h"
#include "Mixer.h"

HRESULT CKeystone::Pipeline_Subpicture_ReceiveSubpicture(IMediaSample * pSample)
{
	//DEBUGGING
	//Beep(1000, 1);
	//DbgLog((LOG_TRACE, 0, TEXT("Keystone: ReceiveSubpicture")));
	//TellPhoenixToGetFrameGrab(pSample, 720, 480, 0);
	//DEBUGGING

	if ((SUB_ActiveMixData.pImage) && ((int)SUB_ActiveMixData.pImage != 0xcdcdcdcd)) 	free(SUB_ActiveMixData.pImage);
	CMediaType *mSource = 0;
	VIDEOINFOHEADER2 * VIH2;
	if (S_OK == pSample->GetMediaType((AM_MEDIA_TYPE**)&mSource) && mSource)
	{
		VIH2 = (VIDEOINFOHEADER2*) mSource->Format();
		SUB_ActiveMixData.nH = VIH2->rcTarget.bottom;
		SUB_ActiveMixData.nW = VIH2->rcTarget.right;
		SUB_ActiveMixData.nX = VIH2->rcTarget.left;
		SUB_ActiveMixData.nY = VIH2->rcTarget.top;
		SUB_LastSub_Width = VIH2->rcTarget.right;
		SUB_LastSub_Height = VIH2->rcTarget.bottom;
		SUB_LastSub_X = VIH2->rcTarget.left;
		SUB_LastSub_Y	= VIH2->rcTarget.top;
	}
	else
	{
		SUB_ActiveMixData.nW = SUB_LastSub_Width;
		SUB_ActiveMixData.nH = SUB_LastSub_Height;
		SUB_ActiveMixData.nX = SUB_LastSub_X;
		SUB_ActiveMixData.nY = SUB_LastSub_Y;
	}

	//REFERENCE_TIME pEnd = 0;
	//pSample->GetTime(&rtCurrentSPStart, &pEnd);

	SUB_ActiveMixData.nImage_Format = IF_ARGB4444;

	LPBYTE TempSPBuffer;
	pSample->GetPointer(&TempSPBuffer);

	if (USEROPTION_DoHighContrastSubpicture == 1)
	{
		//LIME
		//WORD n1 = 0xf0fa;
		WORD n1 = 0xfaf0;

		//MAGENTA
		//WORD n2 = 0x0faf;
		WORD n2 = 0xaf0f;

		//CYAN
		//WORD n3 = 0xfff0;
		WORD n3 = 0xf0ff;

		//Pixel signatures
		WORD b1 = 0;
		WORD b2 = 0;
		WORD b3 = 0;

		//For each byte of source 
		LPBYTE src = TempSPBuffer;
		for(int i = 0; i < 1440*SUB_ActiveMixData.nH-1; i+=2, src+=2) 
		{
			if (src[1] >> 4 != 0)
			{
                //Positive Alpha Value, Now determine Color
				WORD * tW = (WORD*) src;
				if (*tW == b1)
				{
					*tW = n1;
				}
				else if (*tW == b2)
				{
					*tW = n2;
				}
				else if (*tW == b3)
				{
					*tW = n3;
				}
				else
				{
					//It's a new combo
					if (b1==0)
					{
						b1 = *tW;
						*tW = n1;
					}
					else if (b2==0)
					{
						b2 = *tW;
						*tW = n2;
					}
					else if (b3==0)
					{
						b3 = *tW;
						*tW = n3;
					}
					else
					{
						//This should never happen
						DbgLog((LOG_TRACE, 0, TEXT("Keystone: One too many.")));
					}
				}
			}
		}
	}

	SUB_ActiveMixData.pImage = (LPBYTE)malloc(SUB_ActiveMixData.nW * SUB_ActiveMixData.nH * 3);
	CSCManual::ARGB4444ToAAUYVY(TempSPBuffer, SUB_ActiveMixData.nW, SUB_ActiveMixData.nH, SUB_ActiveMixData.pImage);

	return S_OK;
}

HRESULT CKeystone::Pipeline_Subpicture_MixSubpicture(LPBYTE dst, int w, int h)
{
	HRESULT hr = S_OK;
	if (!m_pSubpicture->BSubpictureStreamIsActive) goto SkipSPMix;
	if ((int)SUB_ActiveMixData.pImage == 0xcdcdcdcd) goto SkipSPMix;
	if (SUB_ActiveMixData.pImage)
	{
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: SP Mix.")));
		BYTE vOffset = 0;
		if (this->PIVI_ConnectedHeight == 480)
		{
			vOffset = 3;	
		}
		hr = CMixer::MixSP_AAUYVY(SUB_ActiveMixData.pImage, SUB_ActiveMixData.nW, SUB_ActiveMixData.nH, dst, w, h, USEROPTION_SubpictureLocation_X, USEROPTION_SubpictureLocation_Y + vOffset); //CurrentSubpicture.nX, CurrentSubpicture.nY); 
		if (FAILED(hr)) return hr;
	}
SkipSPMix:
	return hr;
}
