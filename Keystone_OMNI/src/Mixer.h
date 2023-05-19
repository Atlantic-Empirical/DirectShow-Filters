#pragma once
#include "appincludes.h"
#include "CKeystone.h"

class CMixer{
public:

	static HRESULT MixRGB24(LPBYTE TargetImage, LPBYTE MixImage, int Mix_W, int Mix_H, int Mix_X, int Mix_Y)
	{
		//TODO: IANIER
		return S_OK;
	}

	static HRESULT MixRGB32(LPBYTE TargetImage, LPBYTE MixImage, int Mix_W, int Mix_H, int Mix_X, int Mix_Y)
	{
		//TODO: IANIER
		return S_OK;
	}

	static HRESULT MixYUY2_OSD(LPBYTE src, int srcw, int srch, LPBYTE dst, int dstw, int dsth, int x, int y, DWORD KeyColor)
	{ 
		int srcpitch = srcw*2;
		int dstpitch = dstw*2;
		dst += y*dstpitch + x*2;
        
		//The mins crop src to dst
		int xx = min(dstw, srcw + x);
		int yy = min(dsth, srch + y);

		//For each line of source 
		for(int j = y; j < yy; j++, src += srcpitch, dst += dstpitch) 
		{ 
			WORD* s = (WORD*) src; 
			WORD* d = (WORD*) dst;

			//For each pixel on line
			for(int i = x; i < xx; i += 2, d += 2, s += 2) 
			{ 
				//BYTE u = d[0]&0xff; 
				//BYTE y1 = d[0]>>8; 
				//BYTE v = d[1]&0xff; 
				//BYTE y2 = d[1]>>8; 

				BYTE y1 = d[0]&0xff; 
				BYTE u = d[0]>>8; 
				BYTE y2 = d[1]&0xff; 
				BYTE v = d[1]>>8; 

				DWORD yuv1 = ((DWORD)y1<<16)|(u<<8)|v; 
				DWORD yuv2 = ((DWORD)y2<<16)|(u<<8)|v; 
				
				//use mask
				//if(yuv1 == KeyColor) d[0] = s[0]; 
				//if(yuv2 == KeyColor) d[1] = s[1]; 

				//For now mask white fe80
				if(s[0] != 0xfe80) d[0] = s[0]; 
				if(s[1] != 0xfe80) d[1] = s[1]; 

				//don't use mask
				//d[0] = s[0]; 
				//d[1] = s[1]; 
			} 
		}
		return S_OK;
	}

	static HRESULT MixUYVY_OSD(LPBYTE src, int srcw, int srch, LPBYTE dst, int dstw, int dsth, int x, int y, DWORD KeyColor)
	{ 
		int srcpitch = srcw*2;
		int dstpitch = dstw*2;
		dst += y*dstpitch + x*2;
        
		//The mins crop src to dst
		int xx = min(dstw, srcw + x);
		int yy = min(dsth, srch + y);

		//For each line of source 
		for(int j = y; j < yy; j++, src += srcpitch, dst += dstpitch) 
		{ 
			WORD* s = (WORD*) src; 
			WORD* d = (WORD*) dst;

			//For each pixel on line
			for(int i = x; i < xx; i += 2, d += 2, s += 2) 
			{ 
				BYTE u = d[0]&0xff; 
				BYTE y1 = d[0]>>8; 
				BYTE v = d[1]&0xff; 
				BYTE y2 = d[1]>>8; 
				DWORD yuv1 = ((DWORD)y1<<16)|(u<<8)|v; 
				DWORD yuv2 = ((DWORD)y2<<16)|(u<<8)|v; 
				
				////use mask - inop
				//if(yuv1 == KeyColor) d[0] = s[0]; 
				//if(yuv2 == KeyColor) d[1] = s[1]; 

				if(s[0] != ((KeyColor>>16) & 0xFFFF)) d[0] = s[0]; 
				if(s[1] != (KeyColor & 0xFFFF)) d[1] = s[1]; 


				//if (KeyColor == 0xffffff)
				//{
				//	// Mask white fe80
				//	if(s[0] != 0xfe80) d[0] = s[0]; 
				//	if(s[1] != 0xfe80) d[1] = s[1]; 
				//}
				//else
				//{
				//	// Don't use mask
				//	d[0] = s[0]; 
				//	d[1] = s[1]; 
				//}

			} 
		}
		return S_OK;
	}


	//	static HRESULT MixUYVY_OSD(LPBYTE src, int srcw, int srch, LPBYTE dst, int dstw, int dsth, int x, int y, DWORD KeyColor)
//	{ 
//		srch -= 2; //For some reason the bottom two lines are coming out pink. This is a cheat fix. I think it's a problem with RGB24ToUYVY.  It is not a problem in Phoenix.
//		int srcpitch = srcw*2;
//		int dstpitch = dstw*2;
//		dst += y*dstpitch + x*2; 
//        
//		//The mins crop src to dst
//		int xx = min(dstw, srcw + x);
//		int yy = min(dsth, srch + y);
//
//		//For each line
//		for(int j = 0; j < dsth; j++) 
//		{ 
//			if ((j < y) || (j > yy))
//			{
//				dst += dstpitch;
//				goto RunNextLine; //skip this line if there's no src data to mix here.
//			}
//
//			WORD* s = (WORD*) src; 
//			WORD* d = (WORD*) dst;
//
//			//For each pixel on line
//			for(int i = 0; i < dstw; i += 2, d += 2) 
//			{ 
//				if ((i < x) || (i > xx)) goto RunNextPixel; //skip this pixel if there's no data to mix here.
//
//				BYTE u = d[0]&0xff; 
//				BYTE y1 = d[0]>>8; 
//				BYTE v = d[1]&0xff; 
//				BYTE y2 = d[1]>>8; 
//				DWORD yuv1 = ((DWORD)y1<<16)|(u<<8)|v; 
//				DWORD yuv2 = ((DWORD)y2<<16)|(u<<8)|v; 
//				
//				//use mask
//				//if(yuv1 == KeyColor) d[0] = s[0]; 
//				//if(yuv2 == KeyColor) d[1] = s[1]; 
//
//				//For now mask white fe80
//				if(s[0] != 0xfe80) d[0] = s[0]; 
//				if(s[1] != 0xfe80) d[1] = s[1]; 
//
//				//don't use mask
//				//d[0] = s[0]; 
//				//d[1] = s[1]; 
//
//				s += 2;  //only increment src when we've used it
//RunNextPixel:;
//			} 
//			src += srcpitch;
//			dst += dstpitch;
//RunNextLine:;
//		} 
//		return S_OK;
//	}

	static HRESULT MixUYVY(LPBYTE src, int srcw, int srch, LPBYTE dst, int dstw, int dsth, int x, int y, DWORD KeyColor)
	{ 
		int h = min(srch, dsth);
		int srcpitch = srcw*2;
		int dstpitch = dstw*2;
		dst += y*dstpitch + x*2; 
		int xx = min(srcw, dstw - x);
		int yy = h - y; 
		for(int j = 0; j < yy; j++, src += srcpitch, dst += dstpitch) 
		{ 
			WORD* s = (WORD*) src; 
			WORD* d = (WORD*) dst; 
			for(int i = 0; i < xx; i += 2, s += 2, d += 2) 
			{ 
				BYTE u = d[0]&0xff; 
				BYTE y1 = d[0]>>8; 
				BYTE v = d[1]&0xff; 
				BYTE y2 = d[1]>>8; 
				DWORD yuv1 = ((DWORD)y1<<16)|(u<<8)|v; 
				DWORD yuv2 = ((DWORD)y2<<16)|(u<<8)|v; 
				
				//use mask
				//if(yuv1 == KeyColor) d[0] = s[0]; 
				//if(yuv2 == KeyColor) d[1] = s[1]; 

				//don't use mask
				d[0] = s[0]; 
				d[1] = s[1]; 
			} 
		} 
		return S_OK;
	}

	static HRESULT MixUYVY_Line21(LPBYTE src, int srcw, int srch, LPBYTE dst, int dstw, int dsth, int x, int y)
	{ 
		int h = min(srch, dsth);
		int srcpitch = srcw*2;
		int dstpitch = dstw*2;

		////old way - L21
		//dst += y*dstpitch + x*2; 
		//int xx = min(srcw, dstw - x);
		//int yy = h - y; 

		//old way - SP
		//dst += y*dstpitch + x*2;
		//int xx = min(srcw, dstw - x);
		//int yy = h - y; 

		//new way to support negative x and y
		if (y<0)
		{
			src += abs(y) * srcpitch;
		}
		else
		{
			dst += y*dstpitch;
		}
		if (x<0)
		{
			src += abs(x) * 4;
		}
		else
		{
			dst += x*2;
		}

		int xx = min(srcw, dstw - abs(x));
		int yy = h - abs(y); 
		//end new way

		for(int j = 0; j < yy; j++, src += srcpitch, dst += dstpitch) 
		{ 
			WORD* s = (WORD*) src; 
			WORD* d = (WORD*) dst; 
			for(int i = 0; i < xx; i += 2, s += 2, d += 2) 
			{ 
				BYTE u = d[0]&0xff; 
				BYTE y1 = d[0]>>8; 
				BYTE v = d[1]&0xff; 
				BYTE y2 = d[1]>>8; 
				DWORD yuv1 = ((DWORD)y1<<16)|(u<<8)|v; 
				DWORD yuv2 = ((DWORD)y2<<16)|(u<<8)|v; 

				////Use DWORD mask
				//if(yuv1 == KeyColor) d[0] = s[0]; 
				//if(yuv2 == KeyColor) d[1] = s[1]; 

				//Use WORD mask
				if ((s[0] != 0x68d4) && (s[0] != 0x68aa) && (s[0] != 0x00aa)) d[0] = s[0];
				if ((s[1] != 0x68eb) && (s[1] != 0x00b5) && (s[1] != 0x68b5)) d[1] = s[1];

				////Debugging
				//if ((s[0] != 0x68d4) && (s[0] != 0x68aa) && (s[0] != 0x0080) && (s[0] != 0x00aa) && (s[0] != 0xfe80))
				//{
				//	DbgLog((LOG_TRACE, 0, TEXT("Keystone: L21 A: %d"), s[0]));
				//}

				//if ((s[1] != 0x68eb) && (s[1] != 0x00b5) && (s[1] != 0x0080) && (s[1] != 0x68b5) && (s[1] != 0xfe80))
				//{
				//	DbgLog((LOG_TRACE, 0, TEXT("Keystone: L21 B: %d"), s[1]));
				//}

				////Don't use the mask
				//d[0] = s[0]; 
				//d[1] = s[1]; 
			} 
		} 
		return S_OK;
	}

	static HRESULT MixSP_AAUYVY(LPBYTE src, int srcw, int srch, LPBYTE dst, int dstw, int dsth, int x, int y)
	{ 
		int h = min(srch, dsth);
		int srcpitch = srcw*3;
		int dstpitch = dstw*2;
		
		//old way
		//dst += y*dstpitch + x*2;
		//int xx = min(srcw, dstw - x);
		//int yy = h - y; 

		//new way to support negative x and y
		if (y<0)
		{
			src += abs(y) * srcpitch;
		}
		else
		{
			dst += y*dstpitch;
		}
		if (x<0)
		{
			src += abs(x) * 6;
		}
		else
		{
			dst += x*2;
		}

		int xx = min(srcw, dstw - abs(x));
		int yy = h - abs(y); 
		//end new way

		//BYTE alphaA, alphaB;

		for(int j = 0; j < yy; j++, src += srcpitch, dst += dstpitch) 
		{ 
			//New Gabor version
			BYTE* s = (BYTE*)src; 
			BYTE* d = (BYTE*)dst; 
			for(int i = 0; i < xx; i += 2, d += 4, s += 6) 
			{
				//d[0] = (s[2] * s[0] + d[0] * (0xf - s[0]) + 7) >> 4;
				//d[1] = (s[3] * s[0] + d[1] * (0xf - s[0]) + 7) >> 4;
				//d[2] = (s[4] * s[1] + d[2] * (0xf - s[1]) + 7) >> 4;
				//d[3] = (s[5] * s[1] + d[3] * (0xf - s[1]) + 7) >> 4;

				//d[0] = ((((int)s[2]-0x80) * s[0] + ((int)d[0]-0x80) * (0xf - s[0]) + 7) >> 4) + 0x80;
				//d[1] = (s[3] * s[0] + d[1] * (0xf - s[0]) + 7) >> 4;
				//d[2] = ((((int)s[4]-0x80) * s[1] + ((int)d[2]-0x80) * (0xf - s[1]) + 7) >> 4) + 0x80;
				//d[3] = (s[5] * s[1] + d[3] * (0xf - s[1]) + 7) >> 4;

				d[0] = ((int)d[0]-0x80) + ((((int)s[2]-0x80) - ((int)d[0]-0x80)) * s[0] >> 4) + 0x80;
				d[1] = d[1] + ((s[3] - d[1]) * s[0] >> 4);
				d[2] = ((int)d[2]-0x80) + ((((int)s[4]-0x80) - ((int)d[2]-0x80)) * s[1] >> 4) + 0x80;
				d[3] = d[3] + ((s[5] - d[3]) * s[1] >> 4);

				//d[0] = s[2];
				//d[1] = s[3];
				//d[2] = s[4];
				//d[3] = s[5];
			} 
	
			//My Version
			//WORD* s = (WORD*) src; 
			//WORD* d = (WORD*) dst; 
			//for(int i = 0; i < xx; i += 2, d+=2, s+=3) 
			//{
			//	//Get Alphas
			//	alphaA = 0;
			//	alphaB = 0;
			//	if (s[0] != 0x0000)
			//	{
			//		alphaA = s[0]>>8;
			//		alphaB = s[0];
			//	}

			//	//Black is transparent
			//	//if not src pixel is black use the src pixel to overwrite dst pixel
			//	if ((s[1] != 0x0080) && (alphaA != 0))
			//	{
			//		d[0] = s[1]; 
			//	}
			//	if ((s[2] != 0x0080) && (alphaB != 0))
			//	{
			//		d[1] = s[2];
			//	}

			//	//DEBUGGING
			//	//if (s[1] != 0x0080) d[0] = s[1];
			//	//if (s[2] != 0x0080) d[1] = s[2];
			//	//DEBUGGING
			//} 
		} 
		return S_OK;
	}

	//This one does the CSC from ARGB4444 right here
	//static HRESULT MixUYVY_SP_New(LPBYTE src, int srcw, int srch, LPBYTE dst, int dstw, int dsth, int x, int y)
	//{ 
	//	int h = min(srch, dsth);
	//	int srcpitch = srcw*2;
	//	int dstpitch = dstw*2;
	//	dst += y*dstpitch + x*2; 
	//	int xx = min(srcw, dstw - x);
	//	int yy = h - y; 

	//	BYTE a; //,r,g,b;
	//	double alphaA, alphaB;

	//	for(int j = 0; j < yy; j++, src += srcpitch, dst += dstpitch) 
	//	{ 
	//		WORD* s = (WORD*) src; 
	//		WORD* d = (WORD*) dst; 
	//		for(int i = 0; i < xx; i += 2) 
	//		{ 
	//			DWORD yuv1 = 0;
	//			if ((s[0] & 0xf000) == 0)
	//			{
	//				//alpha is zero,set to black
	//				alphaA = 0;
	//				yuv1 = CSCManual::RGBToYUV(0,0,0);
	//			}
	//			else
	//			{
	//				a = (*s>>8)&0xf0;
	//				alphaA = (double) a/255;
	//				yuv1 = CSCManual::RGBToYUV((*s>>4)&0xf0, *s&0xf0, (*s<<4)&0xf0);
	//			}
	//			s++;
	//			DWORD yuv2 = 0;
	//			if ((s[0] & 0xf000) == 0)
	//			{
	//				//alpha is zero, set to black
	//				alphaB = 0;
	//				yuv2 = CSCManual::RGBToYUV(0,0,0);
	//			}
	//			else
	//			{
	//				a = (*s>>8)&0xf0;
	//				alphaB = (double) a/255;
	//				yuv2 = CSCManual::RGBToYUV((*s>>4)&0xf0, *s&0xf0, (*s<<4)&0xf0);
	//			}
	//			s++;
	//			
	//			WORD outA = (WORD)((yuv1&0xff00) | (((yuv1&0xff)+(yuv2&0xff)+1)>>1));
	//			WORD outB = (WORD)((yuv2&0xff00) | (((yuv1&0xff0000)+(yuv2&0xff0000)+1)>>17));

	//			//Mix
	//			//Black is transparent
	//			//if not src pixel is black use the src pixel to overwrite dst pixel
	//			if ((alphaA > 0) || (outA != 0x0080))
	//			{
	//				d[0] = outA; 
	//			}
	//			if ((alphaB > 0) || (outB != 0x0080))
	//			{
	//				d[1] = outB;
	//			}

	//			d++;
	//			d++;
	//		} 
	//	} 
	//	return S_OK;
	//}

	static HRESULT MixUYVY_SP(LPBYTE src, int srcw, int srch, LPBYTE dst, int dstw, int dsth, int x, int y)
	{ 
		int h = min(srch, dsth);
		int srcpitch = srcw*2;
		int dstpitch = dstw*2;
		dst += y*dstpitch + x*2; 
		int xx = min(srcw, dstw - x);
		int yy = h - y; 
		for(int j = 0; j < yy; j++, src += srcpitch, dst += dstpitch) 
		{ 
			WORD* s = (WORD*) src; 
			WORD* d = (WORD*) dst; 
			for(int i = 0; i < xx; i += 2, s += 2, d += 2) 
			{ 
				//Black is transparent
				if (s[0] != 0x0080) d[0] = s[0];
				if (s[1] != 0x0080) d[1] = s[1];

				//if ((s[0] != 0x0080) && (s[0] != 0x0f80) && (s[0] != 0xef80)) d[0] = s[0];
				//if ((s[1] != 0x0080) && (s[1] != 0x0f80) && (s[1] != 0xef80)) d[1] = s[1];

				//d[0] = s[0]; 
				//d[1] = s[1]; 


				//BYTE u = d[0]&0xff; 
				//BYTE y1 = d[0]>>8; 
				//BYTE v = d[1]&0xff; 
				//BYTE y2 = d[1]>>8; 
				//DWORD yuv1 = ((DWORD)y1<<16)|(u<<8)|v; 
				//DWORD yuv2 = ((DWORD)y2<<16)|(u<<8)|v; 

				////Use DWORD mask
				//if(yuv1 == KeyColor) d[0] = s[0]; 
				//if(yuv2 == KeyColor) d[1] = s[1]; 

				////Use WORD mask
				//if ((s[0] != 0x68d4) && (s[0] != 0x68aa) && (s[0] != 0x00aa)) d[0] = s[0];
				//if ((s[1] != 0x68eb) && (s[1] != 0x00b5) && (s[1] != 0x68b5)) d[1] = s[1];

				////Debugging
				//if ((s[0] != 0x68d4) && (s[0] != 0x68aa) && (s[0] != 0x0080) && (s[0] != 0x00aa) && (s[0] != 0xfe80))
				//{
				//	DbgLog((LOG_TRACE, 0, TEXT("Keystone: L21 A: %d"), s[0]));
				//}

				//if ((s[1] != 0x68eb) && (s[1] != 0x00b5) && (s[1] != 0x0080) && (s[1] != 0x68b5) && (s[1] != 0xfe80))
				//{
				//	DbgLog((LOG_TRACE, 0, TEXT("Keystone: L21 B: %d"), s[1]));
				//}

				//Don't use the mask
				//d[0] = s[0]; 
				//d[1] = s[1]; 
			} 
		} 
		return S_OK;
	}

	static HRESULT MixUYVY_Guides(LPBYTE src, int srcw, int srch, int l, int t, int r, int b, DWORD LineColor)
	{ 
		int srcpitch = srcw*2;
		for(int j = 0; j < srch; j++, src += srcpitch) 
		{ 
			WORD* s = (WORD*) src; 
			for(int i = 0; i < srcw; i += 2, s += 2) 
			{
				//this is a pixel on the top or bottom line, make the line 3px thick
				if ((j==t) || (j==b) || (j==t-1) || (j==b+1) ||(j==t-2) || (j==b+2))
				{
					s[0] = (WORD)(LineColor>>16); 
					s[1] = (WORD)LineColor; 
				}
				//this is a pixel on the left or right line, make the line 3px thick
				if ((i==l) || (i==r) || (i==l-1) || (i==r+1) ||(i==l-2) || (i==r+2))
				{
					s[0] = (WORD)(LineColor>>16); 
					s[1] = (WORD)LineColor; 
				}
			} 
		} 
		return S_OK;
	}

	static HRESULT MixYUY2_Guides(LPBYTE src, int srcw, int srch, int l, int t, int r, int b, DWORD LineColor)
	{ 
		DWORD NewLineColor = 0;
		_swab((char*)&LineColor, (char*)&NewLineColor, 4);
		int srcpitch = srcw*2;
		for(int j = 0; j < srch; j++, src += srcpitch) 
		{ 
			WORD* s = (WORD*) src; 
			for(int i = 0; i < srcw; i += 2, s += 2) 
			{
				//this is a pixel on the top or bottom line, make the line 3px thick
				if ((j==t) || (j==b) || (j==t-1) || (j==b+1) ||(j==t-2) || (j==b+2))
				{
					s[0] = (WORD)(NewLineColor>>16);
					s[1] = (WORD)NewLineColor;
				}
				//this is a pixel on the left or right line, make the line 3px thick
				if ((i==l) || (i==r) || (i==l-1) || (i==r+1) ||(i==l-2) || (i==r+2))
				{
					s[0] = (WORD)(NewLineColor>>16);
					s[1] = (WORD)NewLineColor;  
				}
			} 
		} 
		return S_OK;
	}

	static HRESULT MixUYVY_ActionTitleGuides(LPBYTE src, int srcw, int srch, DWORD LineColor)
	{ 
		int al,at,ar,ab;
		al = srcw * 0.06;
		at = srch * 0.06;
		ar = srcw - (0.06 * srcw);
		ab = srch - (0.06 * srch);

		int tl,tt,tr,tb;
		tl = srcw * 0.1;
		tt = srch * 0.1;
		tr = srcw - (0.1 * srcw);
		tb = srch - (0.1 * srch);

		int srcpitch = srcw*2;
		for(int j = 0; j < srch; j++, src += srcpitch) 
		{ 
			WORD* s = (WORD*) src; 
			for(int i = 0; i < srcw; i += 2, s += 2) 
			{
				//this is a pixel on the top or bottom line, make the line 2px thick
				//if ((j==at) || (j==ab) || (j==at+1) || (j==ab+1) ||(j==at-1) || (j==ab-1)    || (j==tt) || (j==tb) || (j==tt+1) || (j==tb+1) ||(j==tt-1) || (j==tb-1))
				if ((j==at) || (j==ab) ||(j==at-1) || (j==ab+1)    || (j==tt) || (j==tb) || (j==tt-1) || (j==tb+1))
				{
					s[0] = (WORD)(LineColor>>16); 
					s[1] = (WORD)LineColor; 
				}
				//this is a pixel on the left or right line, make the line 3px thick
				//if ((i==al) || (i==ar) || (i==al+1) || (i==ar+1) ||(i==al-1) || (i==ar-1)    || (i==tl) || (i==tr) || (i==tl+1) || (i==tr+1) ||(i==tl-1) || (i==tr-1))
				if ((i==al) || (i==ar) ||(i==al-1) || (i==ar+1)    || (i==tl) || (i==tr) || (i==tl-1) || (i==tr+1))
				{
					s[0] = (WORD)(LineColor>>16); 
					s[1] = (WORD)LineColor; 
				}
			} 
		} 
		return S_OK;
	}

	static HRESULT MixYUY2_ActionTitleGuides(LPBYTE src, int srcw, int srch, DWORD LineColor)
	{ 
		DWORD NewLineColor = 0;
		_swab((char*)&LineColor, (char*)&NewLineColor, 4);

		int al,at,ar,ab;
		al = srcw * 0.06;
		at = srch * 0.06;
		ar = srcw - (0.06 * srcw);
		ab = srch - (0.06 * srch);

		int tl,tt,tr,tb;
		tl = srcw * 0.1;
		tt = srch * 0.1;
		tr = srcw - (0.1 * srcw);
		tb = srch - (0.1 * srch);

		int srcpitch = srcw*2;
		for(int j = 0; j < srch; j++, src += srcpitch) 
		{ 
			WORD* s = (WORD*) src; 
			for(int i = 0; i < srcw; i += 2, s += 2) 
			{
				//this is a pixel on the top or bottom line, make the line 2px thick
				//if ((j==at) || (j==ab) || (j==at+1) || (j==ab+1) ||(j==at-1) || (j==ab-1)    || (j==tt) || (j==tb) || (j==tt+1) || (j==tb+1) ||(j==tt-1) || (j==tb-1))
				if ((j==at) || (j==ab) ||(j==at-1) || (j==ab+1)    || (j==tt) || (j==tb) || (j==tt-1) || (j==tb+1))
				{
					s[0] = (WORD)NewLineColor;
					s[1] = (WORD)(NewLineColor>>16); 
				}
				//this is a pixel on the left or right line, make the line 3px thick
				//if ((i==al) || (i==ar) || (i==al+1) || (i==ar+1) ||(i==al-1) || (i==ar-1)    || (i==tl) || (i==tr) || (i==tl+1) || (i==tr+1) ||(i==tl-1) || (i==tr-1))
				if ((i==al) || (i==ar) ||(i==al-1) || (i==ar+1)    || (i==tl) || (i==tr) || (i==tl-1) || (i==tr+1))
				{
					s[0] = (WORD)NewLineColor;
					s[1] = (WORD)(NewLineColor>>16);  
				}
			} 
		} 
		return S_OK;
	}

	static HRESULT MixYUY2(LPBYTE TargetImage, LPBYTE MixImage, int Mix_W, int Mix_H, int Mix_X, int Mix_Y)
	{
		//TODO: IANIER
		return S_OK;
	}

	static HRESULT MixARGB4444(LPBYTE TargetImage, LPBYTE MixImage, int Mix_W, int Mix_H, int Mix_X, int Mix_Y)
	{
		//TODO: IANIER
		return S_OK;
	}

	static HRESULT MixImage(LPBYTE TargetImage, int Targ_W, int Targ_H, int Targ_Pitch, int Targ_Format, stMixData MixMe)
	{
		if (Targ_Format != MixMe.nImage_Format)
		{
			LPBYTE ConversionTarget = NULL;
			//Convert MixImage to Targ_Format
			if (Targ_Format == IF_RGB24)
			{
				if (MixMe.nImage_Format == IF_RGB32)
				{
					CSCManual::RGB32ToRGB24(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
				if (MixMe.nImage_Format == IF_UYVY)
				{
 					CSCManual::UYVYToRGB24(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
				if (MixMe.nImage_Format == IF_YUY2)
				{
					CSCManual::YUY2ToRGB24(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
				if (MixMe.nImage_Format == IF_ARGB4444)
				{
					CSCManual::ARGB4444ToRGB24(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
			}

			if (Targ_Format == IF_RGB32)
			{
				if (MixMe.nImage_Format == IF_RGB24)
				{
					CSCManual::RGB24ToRGB32(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
				if (MixMe.nImage_Format == IF_UYVY)
				{
					CSCManual::UYVYToRGB32(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
				if (MixMe.nImage_Format == IF_YUY2)
				{
					CSCManual::YUY2ToRGB32(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
				if (MixMe.nImage_Format == IF_ARGB4444)
				{
					CSCManual::ARGB4444ToRGB32(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
			}

			if (Targ_Format == IF_UYVY)
			{
				if (MixMe.nImage_Format == IF_RGB24)
				{
					CSCManual::RGB24ToUYVY(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
				if (MixMe.nImage_Format == IF_RGB32)
				{
					CSCManual::RGB32ToUYVY(MixMe.pImage, MixMe.nW * 4, MixMe.nW, MixMe.nH, ConversionTarget);
				}
				if (MixMe.nImage_Format == IF_YUY2)
				{
					//CSCManual::YUY2ToUYVY(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
				if (MixMe.nImage_Format == IF_ARGB4444)
				{
					CSCManual::ARGB4444ToUYVY(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
			}

			if (Targ_Format == IF_YUY2)
			{
				if (MixMe.nImage_Format == IF_RGB24)
				{
					CSCManual::RGB24ToYUY2(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
				if (MixMe.nImage_Format == IF_RGB32)
				{
					CSCManual::RGB32ToYUY2(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
				if (MixMe.nImage_Format == IF_UYVY)
				{
					//CSCManual::UYVYToYUY2(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
				if (MixMe.nImage_Format == IF_ARGB4444)
				{
					CSCManual::ARGB4444ToYUY2(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
			}
			if (Targ_Format == IF_ARGB4444)
			{
				if (MixMe.nImage_Format == IF_RGB24)
				{
					CSCManual::RGB24ToARGB4444(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
				if (MixMe.nImage_Format == IF_RGB32)
				{
					CSCManual::RGB32ToARGB4444(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
				if (MixMe.nImage_Format == IF_UYVY)
				{
					CSCManual::UYVYToARGB4444(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
				if (MixMe.nImage_Format == IF_YUY2)
				{
					CSCManual::YUY2ToARGB4444(MixMe.pImage, MixMe.nW, MixMe.nH, ConversionTarget);
				}
			}
		
			if (ConversionTarget != NULL)
			{
				MixMe.pImage = ConversionTarget;
			}

		}

		//Now do the mixing, knowing that both TargetImage and MixImage are the same format.
		switch(Targ_Format)
		{
			case IF_RGB24:
			{
				return MixRGB24(TargetImage, MixMe.pImage, MixMe.nW, MixMe.nH, MixMe.nX, MixMe.nY);
			}
			case IF_RGB32:
			{
				return MixRGB32(TargetImage, MixMe.pImage, MixMe.nW, MixMe.nH, MixMe.nX, MixMe.nY);
			}
			case IF_UYVY:
			{
				return MixUYVY(MixMe.pImage,  MixMe.nW, MixMe.nH, TargetImage, Targ_W, Targ_H,  MixMe.nX, MixMe.nY, 0x80008000);
			}
			case IF_YUY2:
			{
				return MixYUY2(TargetImage, MixMe.pImage, MixMe.nW, MixMe.nH, MixMe.nX, MixMe.nY);
			}
			case IF_ARGB4444:
			{
				return MixARGB4444(TargetImage, MixMe.pImage, MixMe.nW, MixMe.nH, MixMe.nX, MixMe.nY);
			}
			default:
			{
				return E_FAIL;
			}
		}
	}

};
