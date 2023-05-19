#include "appincludes.h"

void CKeystone::HandleVideoPinConnected(CMediaType* mtIn)
{
	PIVI_MediaType = *mtIn;
	if (PIVI_MediaType.formattype == FORMAT_VideoInfo2)
	{
		PIVI_VIH2 = (VIDEOINFOHEADER2*)PIVI_MediaType.pbFormat;
		PIVI_FormatIsVIH2 = true;
		PIVI_ConnectedHeight = PIVI_VIH2->bmiHeader.biHeight;
		PIVI_ConnectedWidth = PIVI_VIH2->bmiHeader.biWidth;
		PIVI_Pitch = 2 * PIVI_ConnectedWidth;
		PIVI_VIH2->bmiHeader.biSizeImage = (PIVI_VIH2->bmiHeader.biHeight * PIVI_VIH2->bmiHeader.biWidth * 2);
		PIVI_BufferSize = PIVI_VIH2->bmiHeader.biSizeImage; 
		FRRT_TargetFramerate_In_ATPF = PIVI_VIH2->AvgTimePerFrame;
	}
	else
	{
		PIVI_VIH = (VIDEOINFOHEADER*)PIVI_MediaType.pbFormat;
		PIVI_FormatIsVIH2 = false;
		PIVI_ConnectedHeight = PIVI_VIH->bmiHeader.biHeight;
		PIVI_ConnectedWidth = PIVI_VIH->bmiHeader.biWidth;
		PIVI_Pitch = 2 * PIVI_ConnectedWidth;
		PIVI_VIH->bmiHeader.biSizeImage = (PIVI_VIH->bmiHeader.biHeight * PIVI_VIH->bmiHeader.biWidth * 2);
		PIVI_BufferSize = PIVI_VIH->bmiHeader.biSizeImage; 
		FRRT_TargetFramerate_In_ATPF = PIVI_VIH->AvgTimePerFrame;
	}

	//Init Gabor's resizing code
	if (mtIn->formattype == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2 * VIH2 = (VIDEOINFOHEADER2*)mtIn->pbFormat;
		SCLE_ZoomInit(VIH2->bmiHeader.biWidth, VIH2->bmiHeader.biHeight);
	}
	else
	{
		VIDEOINFOHEADER * VIH = (VIDEOINFOHEADER*)mtIn->pbFormat;
		SCLE_ZoomInit(VIH->bmiHeader.biWidth, VIH->bmiHeader.biHeight);
	}

	if (SAGR_LastBufferReceived) free(SAGR_LastBufferReceived);
	if (SAGR_LastBufferReceived_Scaled) free(SAGR_LastBufferReceived_Scaled);

	int h;
	if (PIVI_ConnectedHeight == 480)
	{
		h = 486;
	}
	else
	{
		h = PIVI_ConnectedHeight;
	}

	SAGR_LastBufferReceived = (LPBYTE)malloc(PIVI_ConnectedWidth*h*2);
	SAGR_LastBufferReceived_Scaled = (LPBYTE)malloc(PIVI_ConnectedWidth*h*2);

}
