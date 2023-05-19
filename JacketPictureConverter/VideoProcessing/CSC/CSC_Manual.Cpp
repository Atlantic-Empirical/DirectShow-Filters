#include "../../Utility/appincludes.h"
#include "CSC_Manual.h"
#include <math.h>

	//Gabor Stuff
	#pragma warning(push)
	#pragma warning(disable: 4244)
	static const int cy[3] = {256* 0.299, 256* 0.587, 256* 0.114};
	static const int cu[3] = {256*-0.169, 256*-0.332, 256* 0.500};
	static const int cv[3] = {256* 0.500, 256*-0.419, 256*-0.0813};
	#pragma warning(pop)

	static int cymul[3][256];
	static int cumul[3][256];
	static int cvmul[3][256];
	//End Gabor Stuff

	//===========================================================================================
	//	TO RGB24
	//===========================================================================================

	HRESULT CSCManual::RGB32ToRGB24(LPBYTE src, int W, int H, LPBYTE dst)
	{
		dst = src;
		return S_OK;
	}

	HRESULT CSCManual::UYVYToRGB24(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//WORKS
		int NumPixels = W * H;
		register int i = (NumPixels << 1) - 1;
		register int j = NumPixels + ( NumPixels << 1 ) - 1;
		register int y0, y1, u, v;
		register int r, g, b;

		while (i >= 0) {
			y1 = (unsigned char) src[i--];
			v = (unsigned char) src[i--] - 128;
			y0 = (unsigned char) src[i--];
			u = (unsigned char) src[i--] - 128;
			YUV2RGB (y1, u, v, r, g, b);
			dst[j--] = r;
			dst[j--] = g;
			dst[j--] = b;
			YUV2RGB (y0, u, v, r, g, b);
			dst[j--] = r;
			dst[j--] = g;
			dst[j--] = b;
		}
		return S_OK;
	}

	HRESULT CSCManual::YUY2ToRGB24(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//WORKS
		int NumPixels = W * H;
		register int i = (NumPixels << 1) - 1;
		register int j = NumPixels + ( NumPixels << 1 ) - 1;
		register int y0, y1, u, v;
		register int r, g, b;

		while (i >= 0) {
			v = (unsigned char) src[i--] - 128;
			y1 = (unsigned char) src[i--];
			u = (unsigned char) src[i--] - 128;
			y0 = (unsigned char) src[i--];
			YUV2RGB (y1, u, v, r, g, b);
			dst[j--] = r;
			dst[j--] = g;
			dst[j--] = b;
			YUV2RGB (y0, u, v, r, g, b);
			dst[j--] = r;
			dst[j--] = g;
			dst[j--] = b;
		}
		return S_OK;
	}

	HRESULT CSCManual::ARGB4444ToRGB24(LPBYTE src, int w, int h, LPBYTE dst)
	{
		//Gabor Version
		int srcpitch = 2 * w;
		int dstpitch = 3 * w;
		
		for(; h > 0; h--, src += srcpitch, dst += dstpitch)
		{
			WORD* s = (WORD*)src;
			BYTE* d = (BYTE*)dst;

			for(int i = 0; i < w; i++, s++)
			{
				*d++ = (*s>>4)&0xf0;
				*d++ = *s&0xf0;
				*d++ = (*s<<4)&0xf0;
			}
		}
		return S_OK;
	}

	HRESULT CSCManual::RGB8ToRGB24(LPBYTE src, int w, int h, LPBYTE dst, DWORD * pal)
	{
		//WORKS
		int srcpitch = w;
		int dstpitch = 3 * w;

		//flip vertically
		src += srcpitch*(h-1);
		srcpitch = -srcpitch;

		for(; h > 0; h--, src += srcpitch, dst += dstpitch)
		{
			BYTE* s = (BYTE*)src;
			BYTE* d = (BYTE*)dst;

			for(int i = 0; i < w; i++)
			{
				DWORD rgb = pal[*s++];
				*d++ = ((BYTE)(rgb&0xff));
				*d++ = ((BYTE)((rgb>>8)&0xff));
				*d++ = ((BYTE)((rgb>>16)&0xff));
			}
		}
		return S_OK;	
	}

	//Gabor orig
	//HRESULT CSCManual::RGB8ToRGB24(LPBYTE src, int w, int h, LPBYTE dst, DWORD * pal)
	//{
	//	int srcpitch = w;
	//	int dstpitch = 3 * w;

	//	//flip vertically
	//	src += srcpitch*(h-1);
	//	srcpitch = -srcpitch;

	//	for(; h > 0; h--, src += srcpitch, dst += dstpitch)
	//	{
	//		BYTE* s = (BYTE*)src;
	//		BYTE* d = (BYTE*)dst;

	//		for(int i = 0; i < w; i++)
	//		{
	//			DWORD rgb = pal[*s++];
	//			*d++ = ((BYTE*)rgb)[0];
	//			*d++ = ((BYTE*)rgb)[1];
	//			*d++ = ((BYTE*)rgb)[2];
	//		}
	//	}
	//	return S_OK;	
	//}

	//===========================================================================================
	//	TO RGB32
	//===========================================================================================

	HRESULT CSCManual::RGB24ToRGB32(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER	
		return S_OK;
	}

	HRESULT CSCManual::UYVYToRGB32(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER	
		return S_OK;
	}

	HRESULT CSCManual::YUY2ToRGB32(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//GABOR - Works
		int y0, y1, u, v;
		int r, g, b;

		for(int j = 0; j < H; j++, src += W*2, dst += W*4)
		{
			BYTE* s = src;
			BYTE* d = dst;

			for(int i = 0; i < W; i += 2)
			{
				y0 = s[i*2+0];
				u  = s[i*2+1] - 128;
				y1 = s[i*2+2];
				v  = s[i*2+3] - 128;
				YUV2RGB(y0, u, v, r, g, b);
				d[i*4+0] = b;
				d[i*4+1] = g;
				d[i*4+2] = r;
				d[i*4+3] = 0;
				YUV2RGB(y1, u, v, r, g, b);
				d[i*4+0+4] = b;
				d[i*4+1+4] = g;
				d[i*4+2+4] = r;
				d[i*4+3+4] = 0;
			}
		}
		return S_OK;
	}

	HRESULT CSCManual::ARGB4444ToRGB32(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER
		return S_OK;
	}

	//===========================================================================================
	//	TO UYVY
	//===========================================================================================

	HRESULT CSCManual::RGB24ToUYVY(LPBYTE src, int w, int h, LPBYTE dst)
	{
		//GABOR Version
		//if(w&1) return S_FALSE;

		int srcpitch = 3 * w;
		int dstpitch = 2 * w;

		for(; h > 0; h--, src += srcpitch, dst += dstpitch)
		{
			BYTE* s = (BYTE*)src;
			WORD* d = (WORD*)dst;

			for(int i = 0; i < w; i += 2, s += 6)
			{
				DWORD yuv1 = RGBToYUV(s[2], s[1], s[0]);
				DWORD yuv2 = RGBToYUV(s[5], s[4], s[3]);
				//*d++ = (WORD)((yuv1&0xff00)); 
				*d++ = (WORD)((yuv1&0xff00) | (((yuv1&0xff)+(yuv2&0xff)+1)>>1));
				*d++ = (WORD)((yuv2&0xff00) | (((yuv1&0xff0000)+(yuv2&0xff0000)+1)>>17));
			}
		}
		return S_OK;

		////Not tested. Source unknown.
		//int NumPixels = W * H;
		//register int i = NumPixels + ( NumPixels << 1 )-1;
		//register int j = (NumPixels << 1)-1;
		//register int y0, y1, u0, u1, v0, v1 ;
		//register int r, g, b;
		//while (i >= 0) {
		//	b = ( char) src[i--];
		//	g = ( char) src[i--];
		//	r = ( char) src[i--];
		//	RGB2YUV (r, g, b, y0, u0 , v0);
		//	b = ( char) src[i--];
		//	g = ( char) src[i--];
		//	r = ( char) src[i--];
		//	RGB2YUV (r, g, b, y1, u1 , v1);
		//	*dst[j--] = y0;
		//	*dst[j--] = (v0+v1) >> 1;
		//	*dst[j--] = y1;
		//	*dst[j--] = (u0+u1) >> 1;
		//}
		//return S_OK;
	}

	HRESULT CSCManual::RGB32ToUYVY(LPBYTE src, int srcpitch, int w, int h, LPBYTE dst)
	{
		//GABOR Version
		if(w&1) return S_FALSE;

		int dstpitch = 2 * w;

		for(; h > 0; h--, src += srcpitch, dst += dstpitch)
		{
			BYTE* s = (BYTE*)src;
			WORD* d = (WORD*)dst;

			for(int i = 0; i < w; i += 2, s += 8)
			{
				DWORD yuv1 = RGBToYUV(s[2], s[1], s[0]);
				DWORD yuv2 = RGBToYUV(s[6], s[5], s[4]);
				//*d++ = (WORD)((yuv1&0xff00)); 
				*d++ = (WORD)((yuv1&0xff00) | (((yuv1&0xff)+(yuv2&0xff)+1)>>1));
				*d++ = (WORD)((yuv2&0xff00) | (((yuv1&0xff0000)+(yuv2&0xff0000)+1)>>17));
			}
		}
		return S_OK;
	}

	HRESULT CSCManual::YUY2ToUYVY(LPBYTE buf, int W, int H)
	{
		LPBYTE tmp = (LPBYTE)malloc(H*W*2);
		memcpy(tmp, buf, H*W*2);
		swab((char*)tmp, (char*)buf, H*W*2);	
		free(tmp);
		return S_OK;
	}

	HRESULT CSCManual::SaveBufferToFile_A(const char * FileName, const char * Mode, LPBYTE buf, int length)
	{
		FILE * Out = fopen(FileName, Mode);
		fwrite(buf, length, 1, Out);
		fclose(Out);
		return S_OK;
	}

	HRESULT CSCManual::ARGB4444ToAAUYVY(LPBYTE src, int w, int h, LPBYTE dst)
	{
		//Gabor Version
		if(w&1) return false;

		int srcpitch = 2 * w;
		int dstpitch = 3 * w;

		for(; h > 0; h--, src += srcpitch, dst += dstpitch)
		{
			WORD* s = (WORD*)src;
			WORD* d = (WORD*)dst;

			for(int i = 0; i < w; i += 2)
			{
				//WORD a1 = (*s & 0xf0) << 4;
				//DWORD yuv1 = RGBToYUV((*s<<4)&0xf0, (*s>>8)&0xf0, (*s>>4)&0xf0); 
				//s++;

				//WORD a2 = (*s & 0xf0) >> 4;
				//DWORD yuv2 = yuv2 = RGBToYUV((*s<<4)&0xf0, (*s>>8)&0xf0, (*s>>4)&0xf0); 
				//s++;
				
				WORD a1 = (*s & 0xf000) >> 12;
				DWORD yuv1 = RGBToYUV((*s>>4)&0xf0, *s&0xf0, (*s<<4)&0xf0); 
				s++;

				WORD a2 = (*s & 0xf000) >> 4;
				DWORD yuv2 = RGBToYUV((*s>>4)&0xf0, *s&0xf0, (*s<<4)&0xf0); 
				s++;

				*d++ = (WORD)(a1|a2);
 				*d++ = (WORD)((yuv1&0xff00) | (((yuv1&0xff)+(yuv2&0xff)+1)>>1));
				*d++ = (WORD)((yuv2&0xff00) | (((yuv1&0xff0000)+(yuv2&0xff0000)+1)>>17));
			}
		}

		return S_OK;
	}

	//HRESULT CSCManual::ARGB4444ToAAUYVY(LPBYTE src, int w, int h, LPBYTE dst)
	//{
	//	//Gabor Version
	//	if(w&1) return false;

	//	int srcpitch = 2 * w;
	//	int dstpitch = 3 * w;
	//	 
	//	BYTE a; //,r,g,b;
	//	double alphaA, alphaB;

	//	for(; h > 0; h--, src += srcpitch, dst += dstpitch)
	//	{
	//		WORD* s = (WORD*)src;
	//		WORD* d = (WORD*)dst;

	//		for(int i = 0; i < w; i += 2)
	//		{
	//			DWORD yuv1 = 0;
	//			if ((s[0] & 0xf000) == 0)
	//			{
	//				//alpha is zero,set to black
	//				alphaA = 0;
	//				//yuv1 = RGBToYUV(0,0,0); s++;
	//				yuv1 = 0x00800080; s++;
	//			}
	//			else
	//			{
	//				a = (*s>>8)&0xf0;
	//				alphaA = (double) a/255;
	//				yuv1 = RGBToYUV((*s>>4)&0xf0, *s&0xf0, (*s<<4)&0xf0); s++;
	//			}
	//			
	//			DWORD yuv2 = 0;
	//			if ((s[0] & 0xf000) == 0)
	//			{
	//				//alpha is zero, set to black
	//				alphaB = 0;
	//				//yuv2 = RGBToYUV(0,0,0); s++;
	//				yuv2 = 0x00800080; s++;
	//			}
	//			else
	//			{
	//				a = (*s>>8)&0xf0;
	//				alphaB = (double) a/255;
	//				yuv2 = RGBToYUV((*s>>4)&0xf0, *s&0xf0, (*s<<4)&0xf0); s++;
	//			}

	//			int alA = ceil(alphaA * 100);
	//			int alB = ceil(alphaB * 100);
	//			*d++ = (WORD)((alA<<8) | alB);
 //				*d++ = (WORD)((yuv1&0xff00) | (((yuv1&0xff)+(yuv2&0xff)+1)>>1));
	//			*d++ = (WORD)((yuv2&0xff00) | (((yuv1&0xff0000)+(yuv2&0xff0000)+1)>>17));
	//		}
	//	}
	//	return S_OK;

	//	////Gabor Version - Original - works but not for the SP data which seems to be x,x,A,x
	//	//if(w&1) return false;

	//	//int srcpitch = 2 * w;
	//	//int dstpitch = 2 * w;

	//	//for(; h > 0; h--, src += srcpitch, dst += dstpitch)
	//	//{
	//	//	WORD* s = (WORD*)src;
	//	//	WORD* d = (WORD*)dst;

	//	//	for(int i = 0; i < w; i += 2)
	//	//	{
	//	//		DWORD yuv1 = RGBToYUV((*s<<4)&0xf0, *s&0xf0, (*s>>4)&0xf0); s++;
	//	//		DWORD yuv2 = RGBToYUV((*s<<4)&0xf0, *s&0xf0, (*s>>4)&0xf0); s++;
	//	//		*d++ = (WORD)((yuv1&0xff00) | (((yuv1&0xff)+(yuv2&0xff)+1)>>1));
	//	//		*d++ = (WORD)((yuv2&0xff00) | (((yuv1&0xff0000)+(yuv2&0xff0000)+1)>>17));
	//	//	}
	//	//}
	//	//return S_OK;
	//}

	HRESULT CSCManual::ARGB4444ToUYVY(LPBYTE src, int w, int h, LPBYTE dst)
	{
		//Gabor Version
		if(w&1) return false;

		int srcpitch = 2 * w;
		int dstpitch = 2 * w;
		
		BYTE a,r,g,b;
		double alpha;

		for(; h > 0; h--, src += srcpitch, dst += dstpitch)
		{
			WORD* s = (WORD*)src;
			WORD* d = (WORD*)dst;

			for(int i = 0; i < w; i += 2)
			{
				DWORD yuv1 = 0;
				//yuv1 = RGBToYUV((*s>>4)&0xf0, *s&0xf0, (*s<<4)&0xf0); s++; //Mix all bits
				if ((s[0] & 0xf000) == 0)
				{
					//alpha is zero,set to black
					//yuv1 = RGBToYUV(0,0,0); s++;
					yuv1 = 0x00800080; s++;
				}
				else
				{
					//old way - works
					//yuv1 = RGBToYUV((*s>>4)&0xf0, *s&0xf0, (*s<<4)&0xf0); s++;

					//new way
					a = (*s>>8)&0xf0;
					alpha = (double) a/255;
					g = (*s&0xf0) * alpha;
					b = ((*s<<4)&0xf0) * alpha;
					r = ((*s>>4)&0xf0) * alpha;
					yuv1 = RGBToYUV(r, g, b); s++;
				}
				
				DWORD yuv2 = 0;
				//yuv2 = RGBToYUV((*s>>4)&0xf0, *s&0xf0, (*s<<4)&0xf0); s++;  //Mix all bits
				if ((s[0] & 0xf000) == 0)
				{
					//alpha is zero, set to black
					//yuv2 = RGBToYUV(0,0,0); s++;
					yuv2 = 0x00800080; s++;
				}
				else
				{
					//old way - works
					//yuv2 = RGBToYUV((*s>>4)&0xf0, *s&0xf0, (*s<<4)&0xf0); s++;

					//new way
					a = (*s>>8)&0xf0;
					alpha = (double) a/255;
					g = (*s&0xf0) * alpha;
					b = ((*s<<4)&0xf0) * alpha;
					r = ((*s>>4)&0xf0) * alpha;
					yuv2 = RGBToYUV(r, g, b); s++;
				}

				*d++ = (WORD)((yuv1&0xff00) | (((yuv1&0xff)+(yuv2&0xff)+1)>>1));
				*d++ = (WORD)((yuv2&0xff00) | (((yuv1&0xff0000)+(yuv2&0xff0000)+1)>>17));
			}
		}
		return S_OK;

		////Gabor Version - Original - works but not for the SP data which seems to be x,x,A,x
		//if(w&1) return false;

		//int srcpitch = 2 * w;
		//int dstpitch = 2 * w;

		//for(; h > 0; h--, src += srcpitch, dst += dstpitch)
		//{
		//	WORD* s = (WORD*)src;
		//	WORD* d = (WORD*)dst;

		//	for(int i = 0; i < w; i += 2)
		//	{
		//		DWORD yuv1 = RGBToYUV((*s<<4)&0xf0, *s&0xf0, (*s>>4)&0xf0); s++;
		//		DWORD yuv2 = RGBToYUV((*s<<4)&0xf0, *s&0xf0, (*s>>4)&0xf0); s++;
		//		*d++ = (WORD)((yuv1&0xff00) | (((yuv1&0xff)+(yuv2&0xff)+1)>>1));
		//		*d++ = (WORD)((yuv2&0xff00) | (((yuv1&0xff0000)+(yuv2&0xff0000)+1)>>17));
		//	}
		//}
		//return S_OK;
	}

	HRESULT CSCManual::RGB8ToUYVY(LPBYTE src, int w, int h, LPBYTE dst, DWORD* pal)
	{
		//Gabor Version
		if(w&1) return E_FAIL;

		int srcpitch = w;
		int dstpitch = 2 * w;

		//flip vertically
		src += srcpitch*(h-1);
		srcpitch = -srcpitch;

		for(; h > 0; h--, src += srcpitch, dst += dstpitch)
		{
			BYTE* s = (BYTE*)src;
			WORD* d = (WORD*)dst;

			for(int i = 0; i < w; i += 2)
			{
				DWORD rgb1 = pal[*s++];
				DWORD rgb2 = pal[*s++];
				DWORD yuv1 = RGBToYUV(rgb1&0xff, (rgb1>>8)&0xff, (rgb1>>16)&0xff);
				DWORD yuv2 = RGBToYUV(rgb2&0xff, (rgb2>>8)&0xff, (rgb2>>16)&0xff);
				*d++ = (WORD)((yuv1&0xff00) | (((yuv1&0xff)+(yuv2&0xff)+1)>>1));
				*d++ = (WORD)((yuv2&0xff00) | (((yuv1&0xff0000)+(yuv2&0xff0000)+1)>>17));
			}
		}
		return S_OK;
	}

	//===========================================================================================
	//	TO YUY2
	//===========================================================================================

	HRESULT CSCManual::RGB24ToYUY2(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//Not tested.
		int NumPixels = W * H;
		register int i = NumPixels + ( NumPixels << 1 )-1;
		register int j = (NumPixels << 1)-1;
		register int y0, y1, u0, u1, v0, v1 ;
		register int r, g, b;
		while (i >= 0) {
			b = ( char) src[i--];
			g = ( char) src[i--];
			r = ( char) src[i--];
			RGB2YUV (r, g, b, y0, u0 , v0);
			b = ( char) src[i--];
			g = ( char) src[i--];
			r = ( char) src[i--];
			RGB2YUV (r, g, b, y1, u1 , v1);
			dst[j--] = (v0+v1) >> 1;
			dst[j--] = y0;
			dst[j--] = (u0+u1) >> 1;
			dst[j--] = y1;
		}
		return S_OK;
	}

	HRESULT CSCManual::RGB32ToYUY2(LPBYTE src, int w, int h, LPBYTE dst)
	{
		//Gabor Version, not tested, should work
		if(w&1) return S_FALSE;

		int dstpitch = 2 * w;
		int srcpitch = 4 * w;

		for(; h > 0; h--, src += srcpitch, dst += dstpitch)
		{
			BYTE* s = (BYTE*)src;
			WORD* d = (WORD*)dst;

			for(int i = 0; i < w; i += 2, s += 8)
			{
				DWORD yuv1 = RGBToYUV(s[2], s[1], s[0]);
				DWORD yuv2 = RGBToYUV(s[6], s[5], s[4]);
				*d++ = (WORD)((yuv1&0xff00) | (((yuv1&0xff0000)+(yuv2&0xff0000)+1)>>17));
				*d++ = (WORD)((yuv2&0xff00) | (((yuv1&0xff)+(yuv2&0xff)+1)>>1));
			}
		}
		return S_OK;
	}


	HRESULT CSCManual::UYVYToYUY2(LPBYTE buf, int W, int H)
	{
		LPBYTE tmp = (LPBYTE)malloc(H*W*2);
		memcpy(tmp, buf, H*W*2);
		swab((char*)tmp, (char*)buf, H*W*2);	
		free(tmp);
		return S_OK;
	}

	HRESULT CSCManual::ARGB4444ToYUY2(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER - Priority 1
		return S_OK;
	}

	//===========================================================================================
	//	TO ARGB4444
	//===========================================================================================

	HRESULT CSCManual::RGB24ToARGB4444(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER
		return S_OK;
	}

	HRESULT CSCManual::RGB32ToARGB4444(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER	
		return S_OK;
	}

	HRESULT CSCManual::UYVYToARGB4444(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER	
		return S_OK;
	}

	HRESULT CSCManual::YUY2ToARGB4444(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER
		return S_OK;
	}

	HRESULT CSCManual::StereoDecode( LPBYTE src, int W, int H, LPBYTE * dst)
	{
		int NumPixels = W * H;
		register int i = NumPixels-1;
		register int j = (NumPixels>>1)-1;
		register int k = NumPixels-1;

		while (i >= 0) 
		{
			*dst[k--] = src[i--];
			*dst[j--] = src[i--];
		}
		return S_OK;
	}


	//===========================================================================================
	//GABOR STUFF
	//===========================================================================================

	void CSCManual::InitCC()
	{
		for(int j = 0; j < 3; j++)
		{
			for(int i = 0; i < 256; i++)
			{
				cymul[j][i] = cy[j]*i;
				cumul[j][i] = cu[j]*i;
				cvmul[j][i] = cv[j]*i;
			}
		}
	}

	DWORD CSCManual::RGBToYUV(BYTE r, BYTE g, BYTE b)
	{
		DWORD yuv = 0;

		yuv |= (cumul[0][r] + cumul[1][g] + cumul[2][b] + 0x8000) >> 8;
		yuv |= (cymul[0][r] + cymul[1][g] + cymul[2][b]) & 0xff00;
		yuv |= ((cvmul[0][r] + cvmul[1][g] + cvmul[2][b] + 0x8000) & 0xff00) << 8;

		return yuv;
	}






//
//
//
///*
// * Copyright (C) 2000-2004 Damien Douxchamps  <ddouxchamps@users.sf.net>
// *							damien@douxchamps.net
// *                         Dan Dennedy  <dan@dennedy.org>
// */
//
//#pragma warning (disable: 4244) //conversion from into to char
//#pragma warning (disable: 4309) //truncation of constant value
//
//#define CLIP(in, out)\
//   in = in < 0 ? 0 : in;\
//   in = in > 255 ? 255 : in;\
//   out=in;
//  
///**********************************************************************
// *
// *  CONVERSION FUNCTIONS TO UYVY 
// *
// **********************************************************************/
//
//void
//yuyv2uyvy(char *src, char *dest, int NumPixels, int byte_order)
//{
//  switch (byte_order) {
//  case OVERLAY_BYTE_ORDER_YUYV:
//    _swab(src, dest, NumPixels << 1);
//    break;
//  case OVERLAY_BYTE_ORDER_UYVY:
//    memcpy(dest,src, NumPixels<<1);
//    break;
//  default:
//    fprintf(stderr,"Invalid overlay byte order\n");
//    break;
//  }
//}
//
//void
//uyvy2yuyv( char *src,  char *dest,   int NumPixels, int byte_order)
//{
//  switch (byte_order) {
//  case OVERLAY_BYTE_ORDER_YUYV:
//    _swab(src, dest, NumPixels << 1);
//    break;
//  case OVERLAY_BYTE_ORDER_UYVY:
//    memcpy(dest,src, NumPixels<<1);
//    break;
//  default:
//    fprintf(stderr,"Invalid overlay byte order\n");
//    break;
//  }
//}
//void
//uyyvyy2uyvy ( char *src,  char *dest,   int NumPixels, int byte_order)
//{
//  register int i=NumPixels + (NumPixels >> 1)-1;
//  register int j=(NumPixels << 1)-1;
//  register int y0, y1, y2, y3, u, v;
//
//  switch (byte_order) {
//  case OVERLAY_BYTE_ORDER_YUYV:
//    while (i >= 0) {
//      y3 = src[i--];
//      y2 = src[i--];
//      v  = src[i--];
//      y1 = src[i--];
//      y0 = src[i--];
//      u  = src[i--];
//
//      dest[j--] = v;
//      dest[j--] = y3;
//      dest[j--] = u;
//      dest[j--] = y2;
//      
//      dest[j--] = v;
//      dest[j--] = y1;
//      dest[j--] = u;
//      dest[j--] = y0;
//    }
//    break;
//  case OVERLAY_BYTE_ORDER_UYVY:
//    while (i >= 0) {
//      y3 = src[i--];
//      y2 = src[i--];
//      v  = src[i--];
//      y1 = src[i--];
//      y0 = src[i--];
//      u  = src[i--];
//
//      dest[j--] = y3;
//      dest[j--] = v;
//      dest[j--] = y2;
//      dest[j--] = u;
//      
//      dest[j--] = y1;
//      dest[j--] = v;
//      dest[j--] = y0;
//      dest[j--] = u;
//    }
//    break;
//  default:
//    fprintf(stderr,"Invalid overlay byte order\n");
//    break;
//  }
//
//}
//
//void
//uyv2uyvy ( char *src,  char *dest,   int NumPixels, int byte_order)
//{
//  register int i = NumPixels + (NumPixels << 1)-1;
//  register int j = (NumPixels << 1)-1;
//  register int y0, y1, u0, u1, v0, v1;
//
//  switch (byte_order) {
//  case OVERLAY_BYTE_ORDER_YUYV:
//    while (i >= 0) {
//      v1 = src[i--];
//      y1 = src[i--];
//      u1 = src[i--];
//      v0 = src[i--];
//      y0 = src[i--];
//      u0 = src[i--];
//
//      dest[j--] = (v0+v1) >> 1;
//      dest[j--] = y1;
//      dest[j--] = (u0+u1) >> 1;
//      dest[j--] = y0;
//    }
//    break;
//  case OVERLAY_BYTE_ORDER_UYVY:
//    while (i >= 0) {
//      v1 = src[i--];
//      y1 = src[i--];
//      u1 = src[i--];
//      v0 = src[i--];
//      y0 = src[i--];
//      u0 = src[i--];
//
//      dest[j--] = y1;
//      dest[j--] = (v0+v1) >> 1;
//      dest[j--] = y0;
//      dest[j--] = (u0+u1) >> 1;
//    }
//    break;
//  default:
//    fprintf(stderr,"Invalid overlay byte order\n");
//    break;
//  }
//}
//
//void
//y2uyvy ( char *src,  char *dest, 
//	 long src_width,  long src_height,
//	 long dest_pitch, int byte_order)
//{
//  if ((src_width*2) == dest_pitch) {
//    // do it the quick way
//    register int i = src_width*src_height - 1;
//    register int j = (src_width*src_height << 1) - 1;
//    register int y0, y1;
//    
//    switch (byte_order) {
//    case OVERLAY_BYTE_ORDER_YUYV:
//      while (i >= 0) {
//	y1 = src[i--];
//	y0 = src[i--];
//	dest[j--] = 128;
//	dest[j--] = y1;
//	dest[j--] = 128;
//	dest[j--] = y0;
//      }
//      break;
//    case OVERLAY_BYTE_ORDER_UYVY:
//      while (i >= 0) {
//	y1 = src[i--];
//	y0 = src[i--];
//	dest[j--] = y1;
//	dest[j--] = 128;
//	dest[j--] = y0;
//	dest[j--] = 128;
//      }
//      break;
//    default:
//      fprintf(stderr,"Invalid overlay byte order\n");
//      break;
//    }
//  } else { // src_width*2 != dest_pitch
//    register int x, y;
//
//    //assert ((dest_pitch - 2*src_width)==1);
//
//    switch (byte_order) {
//    case OVERLAY_BYTE_ORDER_YUYV:
//      y=src_height;
//      while (y--) {
//	x=src_width;
//	while (x--) {
//	  *dest++ = *src++;
//	  *dest++ = 128;
//	}
//	// padding required, duplicate last column
//	*dest++ = *(src-1);
//	*dest++ = 128;
//      }
//      break;
//    case OVERLAY_BYTE_ORDER_UYVY:
//      y=src_height;
//      while (y--) {
//	x=src_width;
//	while (x--) {
//	  *dest++ = 128;
//	  *dest++ = *src++;
//	}
//	// padding required, duplicate last column
//	*dest++ = 128;
//	*dest++ = *(src-1);
//      }
//      break;
//    default:
//      fprintf(stderr,"Invalid overlay byte order\n");
//      break;
//    }
//  }
//}
//
//void
//y162uyvy ( char *src,  char *dest,   int NumPixels, int bits, int byte_order)
//{
//  register int i = (NumPixels << 1)-1;
//  register int j = (NumPixels << 1)-1;
//  register int y0, y1;
//
//  switch (byte_order) {
//  case OVERLAY_BYTE_ORDER_YUYV:
//    while (i >= 0) {
//      y1 = src[i--];
//      y1 = (y1 + (((int)src[i--])<<8))>>(bits-8);
//      y0 = src[i--];
//      y0 = (y0 + (((int)src[i--])<<8))>>(bits-8);
//      dest[j--] = 128;
//      dest[j--] = y1;
//      dest[j--] = 128;
//      dest[j--] = y0;
//    }
//    break;
//  case OVERLAY_BYTE_ORDER_UYVY:
//    while (i >= 0) {
//      y1 = src[i--];
//      y1 = (y1 + (((int)src[i--])<<8))>>(bits-8);
//      y0 = src[i--];
//      y0 = (y0 + (((int)src[i--])<<8))>>(bits-8);
//      dest[j--] = y1;
//      dest[j--] = 128;
//      dest[j--] = y0;
//      dest[j--] = 128;
//    }
//    break;
//  default:
//    fprintf(stderr,"Invalid overlay byte order\n");
//    break;
//  }
//
//}
//
//void
//y162y ( char *src,  char *dest,   int NumPixels, int bits)
//{
//  register int i = (NumPixels<<1)-1;
//  register int j = NumPixels-1;
//  register int y;
//
//  while (i >= 0) {
//    y = src[i--];
//    dest[j--] = (y + (src[i--]<<8))>>(bits-8);
//  }
//}
//
//void
//rgb2uyvy ( char *src,  char *dest,   int NumPixels, int byte_order)
//{
//  register int i = NumPixels + ( NumPixels << 1 )-1;
//  register int j = (NumPixels << 1)-1;
//  register int y0, y1, u0, u1, v0, v1 ;
//  register int r, g, b;
//
//  switch (byte_order) {
//  case OVERLAY_BYTE_ORDER_YUYV:
//    while (i >= 0) {
//      b = ( char) src[i--];
//      g = ( char) src[i--];
//      r = ( char) src[i--];
//      RGB2YUV (r, g, b, y0, u0 , v0);
//      b = ( char) src[i--];
//      g = ( char) src[i--];
//      r = ( char) src[i--];
//      RGB2YUV (r, g, b, y1, u1 , v1);
//      dest[j--] = (v0+v1) >> 1;
//      dest[j--] = y0;
//      dest[j--] = (u0+u1) >> 1;
//      dest[j--] = y1;
//    }
//    break;
//  case OVERLAY_BYTE_ORDER_UYVY:
//    while (i >= 0) {
//      b = ( char) src[i--];
//      g = ( char) src[i--];
//      r = ( char) src[i--];
//      RGB2YUV (r, g, b, y0, u0 , v0);
//      b = ( char) src[i--];
//      g = ( char) src[i--];
//      r = ( char) src[i--];
//      RGB2YUV (r, g, b, y1, u1 , v1);
//      dest[j--] = y0;
//      dest[j--] = (v0+v1) >> 1;
//      dest[j--] = y1;
//      dest[j--] = (u0+u1) >> 1;
//    }
//    break;
//  default:
//    fprintf(stderr,"Invalid overlay byte order\n");
//    break;
//  }
//}
//
//void
//rgb482uyvy ( char *src,  char *dest,   int NumPixels, int byte_order)
//{
//  register int i = ( (NumPixels + ( NumPixels << 1 )) << 1 ) -1;
//  register int j = (NumPixels << 1)-1;
//  register int y0, y1, u0, u1, v0, v1 ;
//  register int r, g, b;
//
//  switch (byte_order) {
//  case OVERLAY_BYTE_ORDER_YUYV:
//    while (i >= 0) {
//      i--;
//      b = ( char) src[i--];
//      i--;
//      g = ( char) src[i--];
//      i--;
//      r = ( char) src[i--];
//      i--;
//      RGB2YUV (r, g, b, y0, u0 , v0);
//      b = ( char) src[i--];
//      i--;
//      g = ( char) src[i--];
//      i--;
//      r = ( char) src[i--];
//      RGB2YUV (r, g, b, y1, u1 , v1);
//      dest[j--] = (v0+v1) >> 1;
//      dest[j--] = y0;
//      dest[j--] = (u0+u1) >> 1;
//      dest[j--] = y1;
//    } 
//    break;
//  case OVERLAY_BYTE_ORDER_UYVY:
//    while (i >= 0) {
//      i--;
//      b = ( char) src[i--];
//      i--;
//      g = ( char) src[i--];
//      i--;
//      r = ( char) src[i--];
//      i--;
//      RGB2YUV (r, g, b, y0, u0 , v0);
//      b = ( char) src[i--];
//      i--;
//      g = ( char) src[i--];
//      i--;
//      r = ( char) src[i--];
//      RGB2YUV (r, g, b, y1, u1 , v1);
//      dest[j--] = y0;
//      dest[j--] = (v0+v1) >> 1;
//      dest[j--] = y1;
//      dest[j--] = (u0+u1) >> 1;
//    }
//    break;
//  default:
//    fprintf(stderr,"Invalid overlay byte order\n");
//    break;
//  }
//}
//
///**********************************************************************
// *
// *  CONVERSION FUNCTIONS TO RGB 24bpp 
// *
// **********************************************************************/
//
//void
//rgb482rgb ( char *src,  char *dest,   int NumPixels)
//{
//  register int i = ((NumPixels + ( NumPixels << 1 )) << 1)-1;
//  register int j = NumPixels + ( NumPixels << 1 ) -1;
//
//  while (i >= 0) {
//    i--;
//    dest[j--]=src[i--];
//    i--;
//    dest[j--]=src[i--];
//    i--;
//    dest[j--]=src[i--];
//  }
//}
//
//
//void
//uyv2rgb ( char *src,  char *dest,   int NumPixels)
//{
//  register int i = NumPixels + ( NumPixels << 1 ) -1;
//  register int j = NumPixels + ( NumPixels << 1 ) -1;
//  register int y, u, v;
//  register int r, g, b;
//
//  while (i >= 0) {
//    v = ( char) src[i--] - 128;
//    y = ( char) src[i--];
//    u = ( char) src[i--] - 128;
//    YUV2RGB (y, u, v, r, g, b);
//    dest[j--] = b;
//    dest[j--] = g;
//    dest[j--] = r;  
//  }
//}
//
////Original - Don't mess this up
////void
////uyvy2rgb ( LPBYTE src,  LPBYTE dest,   int NumPixels)
////{
////  register int i = (NumPixels << 1)-1;
////  register int j = NumPixels + ( NumPixels << 1 ) -1;
////  register int y0, y1, u, v;
////  register int r, g, b;
////
////  while (i >= 0) {
////    y1 = (char) src[i--];
////    v  = (char) src[i--] - 128;
////    y0 = (char) src[i--];
////    u  = (char) src[i--] - 128;
////    YUV2RGB (y1, u, v, r, g, b);
////    dest[j--] = b;
////    dest[j--] = g;
////    dest[j--] = r;
////    YUV2RGB (y0, u, v, r, g, b);
////    dest[j--] = b;
////    dest[j--] = g;
////     dest[j--] = r;
////  }
////}
//
//////JerNor version1
////void
////uyvy2rgb ( LPBYTE src,  LPBYTE dest,   int NumPixels)
////{
////  register int i = (NumPixels << 1)-1;
////  register int j = NumPixels + ( NumPixels << 1 ) -1;
////  register int y0, y1, u, v;
////  register int r, g, b;
////  char cv, cu;
////
////  while (i >= 0) {
////    y1 = (char) src[i--];
////    cv = (unsigned char) src[i--];
////    v = cv - 128;
////    y0 = (char) src[i--];
////	cu = (char) src[i--];
////    u  = cu - 128;
////    YUV2RGB (y1, u, v, r, g, b);
////    dest[j--] = b;
////    dest[j--] = g;
////    dest[j--] = r;
////    YUV2RGB (y0, u, v, r, g, b);
////    dest[j--] = b;
////    dest[j--] = g;
////    dest[j--] = r;
////  }
////}
//
////JerNor version2
//void
//uyvy2rgb ( LPBYTE src,  LPBYTE dest,   int NumPixels)
//{
//  register int i = (NumPixels << 1)-1;
//  register int j = NumPixels + ( NumPixels << 1 ) -1;
//  register int y0, y1, u, v;
//  register int r, g, b;
//
//  while (i >= 0) {
//    y1 = (unsigned char) src[i--];
//    v = (unsigned char) src[i--] - 128;
//    y0 = (unsigned char) src[i--];
//	u = (unsigned char) src[i--] - 128;
//    YUV2RGB (y1, u, v, r, g, b);
//    dest[j--] = r;
//    dest[j--] = g;
//    dest[j--] = b;
//    YUV2RGB (y0, u, v, r, g, b);
//    dest[j--] = r;
//    dest[j--] = g;
//    dest[j--] = b;
//  }
//}
//
////void
////uyvy2rgb ( LPBYTE src,  LPBYTE dest,   int NumPixels)
////{
////  register int i = (NumPixels << 1)-1;
////  register int j = NumPixels + ( NumPixels << 1 ) -1;
////  register int y0, y1, u, v;
////  register int r, g, b;
////
////  while (i >= 0) {
////    y1 = (char) src[i--];
////    v  = (char) src[i--]; // - 128;
////    y0 = (char) src[i--];
////    u  = (char) src[i--]; // - 128;
////
////	//v = abs(v);
////	//u = abs(u);
////
////	//YUV2RGB (y1, u, v, r, g, b);
////	r = (1.164 * (y1 - 16)) + (1.596 * (v - 128));\
////	g = (1.164 * (y1 - 16)) - (.392 * (u - 128)) - (.813 * (v - 128));\
////	b = (1.164 * (y1 - 16)) + (2.017 * (u - 128));\
////	if (r > 255) r = 255;
////	if (r < 0) r = 0;
////	if (b > 255) b = 255;
////	if (b < 0) b = 0;
////	if (g > 255) g = 255;
////	if (g < 0) g = 0;
////	dest[j--] = b;
////    dest[j--] = g;
////    dest[j--] = r;
////
////	//YUV2RGB (y0, u, v, r, g, b);
////	r = (1.164 * (y0 - 16)) + (1.596 * (v - 128));\
////	g = (1.164 * (y0 - 16)) - (.392 * (u - 128)) - (.813 * (v - 128));\
////	b = (1.164 * (y0 - 16)) + (2.017 * (u - 128));\
////	if (r > 255) r = 255;
////	if (r < 0) r = 0;
////	if (b > 255) b = 255;
////	if (b < 0) b = 0;
////	if (g > 255) g = 255;
////	if (g < 0) g = 0;
////	dest[j--] = b;
////    dest[j--] = g;
////    dest[j--] = r;
////  }
////}
//
//void
//uyyvyy2rgb ( char *src,  char *dest,   int NumPixels)
//{
//  register int i = NumPixels + ( NumPixels >> 1 )-1;
//  register int j = NumPixels + ( NumPixels << 1 )-1;
//  register int y0, y1, y2, y3, u, v;
//  register int r, g, b;
//  
//  while (i >= 0) {
//    y3 = ( char) src[i--];
//    y2 = ( char) src[i--];
//    v  = ( char) src[i--] - 128;
//    y1 = ( char) src[i--];
//    y0 = ( char) src[i--];
//    u  = ( char) src[i--] - 128;
//    YUV2RGB (y3, u, v, r, g, b);
//    dest[j--] = b;
//    dest[j--] = g;
//    dest[j--] = r;
//    YUV2RGB (y2, u, v, r, g, b);
//    dest[j--] = b;
//    dest[j--] = g;
//    dest[j--] = r;
//    YUV2RGB (y1, u, v, r, g, b);
//    dest[j--] = b;
//    dest[j--] = g;
//    dest[j--] = r;
//    YUV2RGB (y0, u, v, r, g, b);
//    dest[j--] = b;
//    dest[j--] = g;
//    dest[j--] = r;
//  }
//}
//
//void
//y2rgb ( char *src,  char *dest,   int NumPixels)
//{
//  register int i = NumPixels-1;
//  register int j = NumPixels + ( NumPixels << 1 )-1;
//  register int y;
//
//  while (i >= 0) {
//    y = ( char) src[i--];
//    dest[j--] = y;
//    dest[j--] = y;
//    dest[j--] = y;
//  }
//}
//
//void
//y162rgb ( char *src,  char *dest,   int NumPixels, int bits)
//{
//  register int i = (NumPixels << 1)-1;
//  register int j = NumPixels + ( NumPixels << 1 )-1;
//  register int y;
//
//  while (i > 0) {
//    y = src[i--];
//    y = (y + (src[i--]<<8))>>(bits-8);
//    dest[j--] = y;
//    dest[j--] = y;
//    dest[j--] = y;
//  }
//}
//
//// change a 16bit stereo image (8bit/channel) into two 8bit images on top of each other
//void
//StereoDecode( char *src,  char *dest,   int NumPixels)
//{
//  register int i = NumPixels-1;
//  register int j = (NumPixels>>1)-1;
//  register int k = NumPixels-1;
//
//  while (i >= 0) {
//    dest[k--] = src[i--];
//    dest[j--] = src[i--];
//  }
//}
//
