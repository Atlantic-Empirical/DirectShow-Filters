#include "appincludes.h"

void CKeystone::HandleOutputPinConnected(CMediaType* mtIn)
{
	PIVO_MediaType = *mtIn;
	if (PIVO_MediaType.formattype == FORMAT_VideoInfo2)
	{
		PIVO_VIH2 = (VIDEOINFOHEADER2*)PIVO_MediaType.pbFormat;
		PIVO_FormatIsVIH2 = true;
		PIVO_ConnectedHeight = PIVO_VIH2->bmiHeader.biHeight;
		PIVO_ConnectedWidth = PIVO_VIH2->bmiHeader.biWidth;
		PIVO_Pitch = 2 * PIVO_ConnectedWidth;
		PIVO_VIH2->bmiHeader.biSizeImage = (PIVO_VIH2->bmiHeader.biHeight * PIVO_VIH2->bmiHeader.biWidth * 2);
		PIVO_BufferSize = abs((long)PIVO_VIH2->bmiHeader.biSizeImage);
		PIVO_ATPF = PIVO_VIH2->AvgTimePerFrame;
		FRRT_TargetFramerate_Out_ATPF = PIVO_VIH2->AvgTimePerFrame;
	}
	else
	{
		PIVO_VIH = (VIDEOINFOHEADER*)PIVO_MediaType.pbFormat;
		PIVO_FormatIsVIH2 = false;
		PIVO_ConnectedHeight = PIVO_VIH->bmiHeader.biHeight;
		PIVO_ConnectedWidth = PIVO_VIH->bmiHeader.biWidth;
		PIVO_Pitch = 2 * PIVO_ConnectedWidth;
		PIVO_VIH->bmiHeader.biSizeImage = (PIVO_VIH->bmiHeader.biHeight * PIVO_VIH->bmiHeader.biWidth * 2);
		PIVO_BufferSize = abs((long)PIVO_VIH->bmiHeader.biSizeImage); 
		PIVO_ATPF = PIVO_VIH->AvgTimePerFrame;
		FRRT_TargetFramerate_Out_ATPF = PIVO_VIH->AvgTimePerFrame;
	}

	if (SAGR_LastBufferOutput) free(SAGR_LastBufferOutput);
	SAGR_LastBufferOutput = (LPBYTE)malloc(PIVO_ConnectedWidth*PIVO_ConnectedHeight*2);

}
