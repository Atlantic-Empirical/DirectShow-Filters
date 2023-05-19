#include <appincludes.h>

HRESULT CKeystoneVideoTwoPin::ReverseFieldOrder(LPBYTE src, int h, int w, int pitch)
{
	LPBYTE tmp = (LPBYTE)malloc(h * pitch);
	m_pTransformFilter->CopyVidRect(src, tmp, pitch, w, h);

	LPBYTE ReadAddress = tmp;
	LPBYTE WriteAddress = src + pitch; //start by writing the second line in the target

	int ThreePitch = pitch*3;

	for (int nScanLines = 0; nScanLines < h; nScanLines += 2)
	{
		//memcpy(ReadAddress, WriteAddress, pitch);
		memcpy(WriteAddress, ReadAddress, pitch);
		WriteAddress -= pitch;
		ReadAddress += pitch;
		//memcpy(ReadAddress, WriteAddress, pitch);
		memcpy(WriteAddress, ReadAddress, pitch);
		WriteAddress += ThreePitch;
		ReadAddress += pitch;
	}
	delete tmp;
	//free(tmp);
	return S_OK;
}

HRESULT CKeystoneVideoTwoPin::BumpFieldsDown(LPBYTE src, int h, int w, int pitch, int Cnt, bool SetLinesBlack)
{
	LPBYTE tmp = (LPBYTE)malloc(h * pitch);
	m_pTransformFilter->CopyVidRect(src, tmp, pitch, w, h);

	LPBYTE ReadAddress = tmp; 
	LPBYTE WriteAddress = src + (pitch*Cnt); //start by reading the nth line in the target

	for (int nScanLines = 0; nScanLines < h-1; nScanLines++)
	{
		memcpy(WriteAddress, ReadAddress, pitch);
		WriteAddress += pitch;
		ReadAddress += pitch;
	}

	if (SetLinesBlack)
	{
		for (LPBYTE i = src; i < src + (pitch * Cnt); i += 4)
		{
			//this is currently YUY2 byte order
			memset(i	, 0x00, 1);	//U: Blue - Luma
			memset(i + 1, 0x80, 1);	//Y: Luma
			memset(i + 2, 0x00, 1);	//V: Red - Luma
			memset(i + 3, 0x80, 1);	//Y: Luma
		}
	}

	free(tmp);
	return S_OK;
}

//HRESULT CKeystoneVideoTwoPin::BumpFieldsDown(LPBYTE src, int h, int w, int pitch)
//{
//	LPBYTE tmp = (LPBYTE)malloc(h * pitch);
//	m_pTransformFilter->CopyVidRect(src, tmp, pitch, w, h);
//
//	LPBYTE ReadAddress = tmp + pitch; //start by reading the second line in the target
//	LPBYTE WriteAddress = src; 
//
//	for (int nScanLines = 0; nScanLines < h-1; nScanLines++)
//	{
//		memcpy(WriteAddress, ReadAddress, pitch);
//		WriteAddress += pitch;
//		ReadAddress += pitch;
//	}
//
//	//DEBUGGING
//	for (LPBYTE i = src; i < src + 1440; i += 4)
//	{
//		memset(i	, 0xC0, 1);	//U: Blue - Luma
//		memset(i + 1, 0x00, 1);	//Y: Luma
//		memset(i + 2, 0xC0, 1);	//V: Red - Luma
//		memset(i + 3, 0x00, 1);	//Y: Luma
//	}
//	//DEBUGGING
//
//	free(tmp);
//	return S_OK;
//}

//HRESULT CKeystoneVideoTwoPin::BumpFieldsDown(LPBYTE src, int h, int w, int pitch)
//{
//	LPBYTE tmp = (LPBYTE)malloc(h * pitch);
//	m_pTransformFilter->CopyVidRect(src, tmp, pitch, w, h);
//
//	//for (LPBYTE i = src; i < src + pitch; i += 4)
//	//{
//	//	memset(i	, 0x80, 1);	//U: Blue - Luma
//	//	memset(i + 1, 0x00, 1);	//Y: Luma
//	//	memset(i + 2, 0x80, 1);	//V: Red - Luma
//	//	memset(i + 3, 0x00, 1);	//Y: Luma
//
//	//	//memset(i	, 0, 1);	//U: Blue - Luma
//	//	//memset(i + 1, 0, 1);	//Y: Luma
//	//	//memset(i + 2, 0, 1);	//V: Red - Luma
//	//	//memset(i + 3, 0, 1);	//Y: Luma
//
//	//}
//
//	LPBYTE ReadAddress = tmp;
//	LPBYTE WriteAddress = src + pitch; //start by writing the second line in the target
//
//	for (int nScanLines = 0; nScanLines < h-1; nScanLines++)
//	{
//		memcpy(WriteAddress, ReadAddress, pitch);
//		WriteAddress += pitch;
//		ReadAddress += pitch;
//	}
//	free(tmp);
//	return S_OK;
//}