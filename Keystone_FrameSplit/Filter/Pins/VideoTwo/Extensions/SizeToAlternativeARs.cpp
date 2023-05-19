#include <appincludes.h>


//DWORD version
HRESULT CKeystoneVideoTwoPin::SizeToPanScan(LPBYTE orig, int h, int w)
{
	//this only works for 720 2bytes per pixel
	//takes the middle 540 pixels from an anamorphic 720 image and 'expands' them to 720, thus
	//creating 4x3 in correct proportions.

	int pitch = 1440;
	LPBYTE tmp = (LPBYTE)malloc(h * pitch);
	m_pTransformFilter->CopyVidRect(orig, tmp, pitch, w, h);

	//DEBUGGING
	//memset(orig, 0, h*pitch); //set the bg to some other color - DEBUGGING
	//for (LPBYTE tOrig = orig; tOrig < (LPBYTE)orig + (h*pitch); tOrig+=4)
	//{
	//	memset(tOrig	, 0x00, 1);	//U: Blue - Luma
	//	memset(tOrig + 1, 0x80, 1);	//Y: Luma
	//	memset(tOrig + 2, 0x00, 1);	//V: Red - Luma
	//	memset(tOrig + 3, 0x80, 1);	//Y: Luma
	//}
	//DEBUGGING

	LPBYTE TmpRead = tmp;
	LPBYTE TmpWrite = orig;

	int PixelsToRemoveFromEachSide = (w-((w/((double)16/9))*((double)4/3))) / 2;

	//each DWORD is two pixels

	for(int j = 0; j < h; j++, TmpWrite += pitch, TmpRead += pitch) 
	{ 
		DWORD* re = (DWORD*) TmpRead + (PixelsToRemoveFromEachSide/2); //go to 90 px into the line 
		DWORD* wr = (DWORD*) TmpWrite;
		int cnt = 0;

		for(int i = PixelsToRemoveFromEachSide; i < w - PixelsToRemoveFromEachSide; i+=2, re++, wr++) 
		{
			*wr = *re; cnt++;
			if (cnt == 3)
			{
				wr++;

				//Current DWORD and Pixels
				BYTE a_y1 = *re; 
				BYTE a_u = *re>>8; 
				BYTE a_y2 = *re>>16; 
				BYTE a_v = *re>>24; 
			
				//px1 = a_y1 , a_u, a_v
				//px2 = a_y2, a_u, a_v

				//Next DWORD and pixels
				BYTE b_y1 = re[1]; 
				BYTE b_u = re[1]>>8; 
				BYTE b_y2 = re[1]>>16; 
				BYTE b_v = re[1]>>24; 

				//px1 = b_y1 , b_u, b_v
				//px2 = b_y2, b_u, b_v

				//Out DWORD and pixels
				BYTE o_u = (a_u + b_u)/2;
				BYTE o_y1 = (a_y1 + b_y1)/2;
				BYTE o_v = (a_v + b_v)/2;
				BYTE o_y2 = (a_y2 + b_y2)/2;

				*wr = (o_u<<24) | (o_y1<<16) | (o_v<<8) | o_y2;

				//*wr = *re; //just use the previous pixel
				//*wr = (*re + (*re+1)) / 2; //dumb average of previous and next pixels
				cnt=0;
			}
		}
	}
	free(tmp);
	return S_OK;
}

////WORD version
//HRESULT CKeystoneVideoTwoPin::SizeToPanScan(LPBYTE orig, int h, int w)
//{
//	//this only works for 720w 2bytes per pixel
//	//takes the middle 540 pixels from an anamorphic 720 image and 'expands' them to 720, thus
//	//creating 4x3 in correct proportions.
//
//	int pitch = 1440;
//	LPBYTE tmp = (LPBYTE)malloc(h * pitch);
//	m_pTransformFilter->CopyVidRect(orig, tmp, pitch, w, h);
//
//	//DEBUGGING
//	memset(orig, 16, h*pitch); //set the bg to some other color
//	//for (LPBYTE tOrig = orig; tOrig < (LPBYTE)orig + (h*pitch); tOrig+=4)
//	//{
//	//	memset(tOrig	, 0x10, 1);	//U: Blue - Luma
//	//	memset(tOrig + 1, 0x80, 1);	//Y: Luma
//	//	memset(tOrig + 2, 0x10, 1);	//V: Red - Luma
//	//	memset(tOrig + 3, 0x80, 1);	//Y: Luma
//	//}
//	//DEBUGGING
//
//	LPBYTE TmpRead = tmp;
//	LPBYTE TmpWrite = orig;
//
//	int PixelsToRemoveFromEachSide = (w-((w/((double)16/9))*((double)4/3))) / 2;
//
//	for(int j = 0; j < h; j++, TmpWrite += pitch, TmpRead += pitch) 
//	{ 
//		WORD* re = (WORD*) TmpRead + PixelsToRemoveFromEachSide; //go to 90 px into the line 
//		WORD* wr = (WORD*) TmpWrite;
//		int cnt = 0;
//
//		for(int i = PixelsToRemoveFromEachSide; i < w - PixelsToRemoveFromEachSide; i++, re++, wr++) 
//		{
//			*wr = *re; cnt++;
//			if (cnt == 3)
//			{
//				wr++; 
//				*wr = re[2];
//
//				//wr++; //move the write cursor up one pixel (word)
//				//BYTE cA = re[0]&0xff;	//current word byte A
//				//BYTE cB = re[0]>>8;		//current word byte B
//				////one or the other of these should be 2 to go two bytes ahead to get the match u or v
//				//BYTE nA = re[1]&0xff;	//next word byte A
//				//BYTE nB = re[2]>>8;		//next word byte B
//				//BYTE oA = (cA + nA) /2; //out word byte A
//				//BYTE oB = (cB + nB) /2; //out word byte B
//				//*wr = (oB<<8) | oA;
//				cnt=0;
//			}
//		}
//	}
//	free(tmp);
//	return S_OK;
//}

////DWORD version
//HRESULT CKeystoneVideoTwoPin::SizeToPanScan(LPBYTE orig, int h, int w)
//{
//	//this only works for 720 2bytes per pixel
//	//takes the middle 540 pixels from an anamorphic 720 image and 'expands' them to 720, thus
//	//creating 4x3 in correct proportions.
//
//	int pitch = 1440;
//	LPBYTE tmp = (LPBYTE)malloc(h * pitch);
//	m_pTransformFilter->CopyVidRect(orig, tmp, pitch, w, h);
//
//	//DEBUGGING
//	//memset(orig, 0, h*pitch); //set the bg to some other color - DEBUGGING
//	//for (LPBYTE tOrig = orig; tOrig < (LPBYTE)orig + (h*pitch); tOrig+=4)
//	//{
//	//	memset(tOrig	, 0x00, 1);	//U: Blue - Luma
//	//	memset(tOrig + 1, 0x80, 1);	//Y: Luma
//	//	memset(tOrig + 2, 0x00, 1);	//V: Red - Luma
//	//	memset(tOrig + 3, 0x80, 1);	//Y: Luma
//	//}
//	//DEBUGGING
//
//	LPBYTE TmpRead = tmp;
//	LPBYTE TmpWrite = orig;
//
//	int PixelsToRemoveFromEachSide = (w-((w/((double)16/9))*((double)4/3))) / 2;
//
//	for(int j = 0; j < h; j++, TmpWrite += pitch, TmpRead += pitch) 
//	{ 
//		DWORD* re = (DWORD*) TmpRead + (PixelsToRemoveFromEachSide/2); //go to 90 px into the line 
//		DWORD* wr = (DWORD*) TmpWrite;
//		int cnt = 0;
//
//		for(int i = PixelsToRemoveFromEachSide; i < w - PixelsToRemoveFromEachSide; i+=2, re++, wr++) 
//		{
//			*wr = *re; cnt++;
//			if (cnt == 3)
//			{
//				wr++;
//
//				BYTE a_y1 = *re; 
//				BYTE a_u = *re>>8; 
//				BYTE a_y2 = *re>>16; 
//				BYTE a_v = *re>>24; 
//			
//				BYTE b_y1 = re[1]; 
//				BYTE b_u = re[1]>>8; 
//				BYTE b_y2 = re[1]>>16; 
//				BYTE b_v = re[1]>>24; 
//
//				BYTE o_u = (a_u + b_u)/2;
//				BYTE o_y1 = (a_y1 + b_y1)/2;
//				BYTE o_v = (a_v + b_v)/2;
//				BYTE o_y2 = (a_y2 + b_y2)/2;
//
//				*wr = (o_u<<24) | (o_y1<<16) | (o_v<<8) | o_y2;
//
//				//*wr = *re; //just use the previous pixel
//				//*wr = (*re + (*re+1)) / 2; //dumb average of previous and next pixels
//				cnt=0;
//			}
//		}
//	}
//
//	//delete tmp;
//	free(tmp);
//	return S_OK;
//}

HRESULT CKeystoneVideoTwoPin::SizeToLetterbox(LPBYTE orig, int h, int w, DWORD* LBColor)
{
	int pitch = 1440;
	LPBYTE tmp = (LPBYTE)malloc(h * pitch);
	m_pTransformFilter->CopyVidRect(orig, tmp, pitch, w, h);

	//set bg color for desired lb color
	char* from = (char*)LBColor;
	char dest[] = "0000";
	swab(from, dest, 4);

	LPBYTE tOrig = orig;
	for(int j = 0; j < h; j++, tOrig += pitch) 
	{ 
		WORD* s = (WORD*) tOrig; 
		for(int i = 0; i < w; i += 2, s += 2) 
		{
			s[0] = (WORD) ((WORD)(dest[3]<<8) | (BYTE)(dest[2])); 
			s[1] = (WORD) ((WORD)(dest[1]<<8) | (BYTE)(dest[0]));
		} 
	}

	////Set bg to black for letterboxes
	//for (WORD*tOrig=(WORD*)orig; (LPBYTE)tOrig < orig + (h*pitch); tOrig+=2)
	//{
	//	//memset(tOrig	, 0x00, 1);	//U: Blue - Luma
	//	//memset(tOrig + 1, 0x00, 1);	//Y: Luma
	//	//memset(tOrig + 2, 0x00, 1);	//V: Red - Luma
	//	//memset(tOrig + 3, 0x00, 1);	//Y: Luma
	//	//memset(tOrig	, 0x10, 1);	//U: Blue - Luma
	//	//memset(tOrig + 1, 0x80, 1);	//Y: Luma
	//	//memset(tOrig + 2, 0x10, 1);	//V: Red - Luma
	//	//memset(tOrig + 3, 0x80, 1);	//Y: Luma
	//}

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
	LPBYTE TmpWrite = orig + (adj * pitch);
	LPBYTE TmpRead = tmp;

	int cnt = 0;
	for(int j = 0; j < h; j++, TmpRead += pitch) 
	{ 
		if (cnt == 3)
		{
			//skip this line
			cnt = 0;
			goto NextLine;
		}
		WORD* re = (WORD*) TmpRead;
		WORD* wr = (WORD*) TmpWrite;
		for(int i = 0; i < w; i++, re++, wr++) 
		{
			*wr = *re;
		}
		cnt++;
		TmpWrite += pitch; //only move up write cursor when we've actually written something
NextLine:;
	}
	free(tmp);
	return S_OK;
}


				////move write cursor up one spot
				//wr++;

				////Tom's hail mary at averaging
				//BYTE a_y1 = re[0]&0xff; 
				//BYTE a_u = re[0]>>8; 
				//BYTE a_y2 = re[1]&0xff; 
				//BYTE a_v = re[1]>>8; 
				//
				//BYTE b_y1 = re[2]&0xff; 
				//BYTE b_u = re[2]>>8; 
				//BYTE b_y2 = re[3]&0xff; 
				//BYTE b_v = re[3]>>8; 

				//BYTE o_u = (a_u + b_u)/2;
				//BYTE o_y1 = (a_y1 + b_y1)/2;
				//BYTE o_v = (a_v + b_v)/2;
				//BYTE o_y2 = (a_y2 + b_y2)/2;

				////wr[1] = (o_u<<24) | (o_y1<<16) | (o_v<<8) | o_y2;
				//wr[0] = (o_u<<8) | o_y1;
				//wr[1] = (o_v<<8) | o_y2;
    //            
				////End Hail Mary
