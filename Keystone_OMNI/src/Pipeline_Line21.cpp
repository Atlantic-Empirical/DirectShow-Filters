#include "appincludes.h"
#include "Mixer.h"

HRESULT CKeystone::Pipeline_Line21_ReceiveLine21(IMediaSample * pSample)
{
	//DbgLog((LOG_TRACE,0,TEXT("ReceiveLine21() - BEGIN")));
	
	L21_ActiveMixData.nH = 640;
	L21_ActiveMixData.nW = 480;
	L21_ActiveMixData.nImage_Format = IF_RGB8;
	L21_ActiveMixData.nX = 0;
	L21_ActiveMixData.nY = 0;

	//Get palette
	CMediaType *mtL21 = 0;
	if (S_OK != pSample->GetMediaType((AM_MEDIA_TYPE**)&mtL21) && mtL21) 
	{
		return E_FAIL;
	}

	if (mtL21 == 0x00000000) 
	{
		//pSample->GetPointer(&TempL21Buffer);
		//L21MixData.pImage = NULL;
		//return S_OK;
	}
	else
	{
		VIDEOINFO * VI = (VIDEOINFO*) mtL21->Format();
		L21_ActiveMixData.Palette = COLORS(VI);
	}

	if (((unsigned int)L21_ActiveMixData.Palette == 0xcdcdcdcd) || ((unsigned int)L21_ActiveMixData.Palette == 0))
	{
		return S_OK;
	}

	LPBYTE TempL21Buffer;
	pSample->GetPointer(&TempL21Buffer);

	LPBYTE pNewL21Buf = (LPBYTE)malloc(614400);
	HRESULT hr = CSCManual::RGB8ToUYVY(TempL21Buffer, 640, 480, pNewL21Buf, (DWORD*) L21_ActiveMixData.Palette);

	// add item to queue
	L21_AddBufferToLine21Queue(pNewL21Buf);

	return S_OK;
}


HRESULT CKeystone::Pipeline_Line21_MixLine21(LPBYTE dst, int w, int h)
{
	//DbgLog((LOG_TRACE,0,TEXT("MixLine21() - BEGIN")));
	HRESULT hr = S_OK;
	if (!m_pLine21->bL21StreamIsActive) goto SkipL21Mix;

	queue <int>::size_type queuelength;
	queuelength = L21_SampleQueue.size();

	if (queuelength > 0)
	{
		if ((L21_ActiveMixData.pImage) && ((int)L21_ActiveMixData.pImage != 0xcdcdcdcd)) 	free(L21_ActiveMixData.pImage);
		L21_ActiveMixData.pImage = L21_PopSampleFromLine21Queue();
	}
	else
	{
		if ((int)L21_ActiveMixData.pImage == 0xcdcdcdcd) goto SkipL21Mix;
		if ((int)L21_ActiveMixData.pImage == 0) goto SkipL21Mix;
	}
	if (L21_ActiveMixData.pImage)
	{
		//DbgLog((LOG_TRACE, 0, TEXT("Keystone: MixLine21 - 1")));

		hr = CMixer::MixUYVY_Line21(L21_ActiveMixData.pImage, 640, 480, dst, w,  h, USEROPTION_Line21Position_X, USEROPTION_Line21Position_Y); //40, 0); 
		if (FAILED(hr)) return hr;
	}

SkipL21Mix:
	//DbgLog((LOG_TRACE,0,TEXT("MixLine21() - END")));
	return hr;
}

