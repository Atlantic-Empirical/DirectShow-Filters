#pragma once
#include "appincludes.h"

#pragma warning (disable: 4244) //conversion from into to char
#pragma warning (disable: 4309) //truncation of constant value

class CSCManual
{
public:

	typedef enum
	{
		NO_STEREO_DECODING=0,
		STEREO_DECODING_INTERLACED,
		STEREO_DECODING_FIELD
	} stereo_decoding_t;

	#define CLIP(in, out)\
	in = in < 0 ? 0 : in;\
	in = in > 255 ? 255 : in;\
	out=in;

	// color conversion functions from Bart Nabbe.
	// corrected by Damien: bad coeficients in YUV2RGB
	//TF: also see fourcc.org for more discussion of YUV/RGB conversion

	//Original - Don't mess this up - seems to be working
	#define YUV2RGB(y, u, v, r, g, b)\
	r = y + ((v*1436) >> 10);\
	g = y - ((u*352 + v*731) >> 10);\
	b = y + ((u*1814) >> 10);\
	r = r < 0 ? 0 : r;\
	g = g < 0 ? 0 : g;\
	b = b < 0 ? 0 : b;\
	r = r > 255 ? 255 : r;\
	g = g > 255 ? 255 : g;\
	b = b > 255 ? 255 : b

	#define RGB2YUV(r, g, b, y, u, v)\
	y = (306*r + 601*g + 117*b)  >> 10;\
	u = ((-172*r - 340*g + 512*b) >> 10)  + 128;\
	v = ((512*r - 429*g - 83*b) >> 10) + 128;\
	y = y < 0 ? 0 : y;\
	u = u < 0 ? 0 : u;\
	v = v < 0 ? 0 : v;\
	y = y > 255 ? 255 : y;\
	u = u > 255 ? 255 : u;\
	v = v > 255 ? 255 : v

	#define REPLPIX(im, pix, index)\
	im[index]=pix[0];\
	im[index+1]=pix[1];\
	im[index+2]=pix[2];\
	im[index+3]=pix[3]

	#define INVPIX(im, index)\
	im[index]=255-im[index];\
	im[index+1]=255-im[index+1];\
	im[index+2]=255-im[index+2];\
	im[index+3]=255-im[index+3]

	//TO RGB24
	static HRESULT RGB32ToRGB24(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT UYVYToRGB24(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT YUY2ToRGB24(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT ARGB4444ToRGB24(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT RGB8ToRGB24(LPBYTE src, int w, int h, LPBYTE dst, DWORD * pal);

	//TO RGB32
	static HRESULT RGB24ToRGB32(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT UYVYToRGB32(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT YUY2ToRGB32(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT ARGB4444ToRGB32(LPBYTE src, int W, int H, LPBYTE dst);

	//TO UYVY
	static HRESULT RGB24ToUYVY(LPBYTE src, int W, int H, LPBYTE dst);
	//static HRESULT RGB32ToUYVY(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT RGB32ToUYVY(LPBYTE src, int srcpitch, int w, int h, LPBYTE dst);
	//static HRESULT YUY2ToUYVY(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT YUY2ToUYVY(LPBYTE buf, int W, int H);
	static HRESULT ARGB4444ToUYVY(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT RGB8ToUYVY(LPBYTE src, int W, int H, LPBYTE dst, DWORD* pal);

	//TOAUVAVY
	static HRESULT ARGB4444ToAAUYVY(LPBYTE src, int W, int H, LPBYTE dst);
	
	//TO YUY2
	static HRESULT RGB24ToYUY2(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT RGB32ToYUY2(LPBYTE src, int W, int H, LPBYTE dst);
	//static HRESULT UYVYToYUY2(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT UYVYToYUY2(LPBYTE buf, int W, int H);
	static HRESULT ARGB4444ToYUY2(LPBYTE src, int W, int H, LPBYTE dst);

	//TO ARGB4444
	static HRESULT RGB24ToARGB4444(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT RGB32ToARGB4444(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT UYVYToARGB4444(LPBYTE src, int W, int H, LPBYTE dst);
	static HRESULT YUY2ToARGB4444(LPBYTE src, int W, int H, LPBYTE dst);

	// change a 16bit stereo image (8bit/channel) into two 8bit images on top of each other
	static HRESULT StereoDecode( LPBYTE src, int W, int H, LPBYTE * dst);


	//ETC
	static HRESULT SaveBufferToFile_A(const char * FileName, const char * Mode, LPBYTE buf, int length);
	
	//GABOR STUFF
	/*
		Y = R *  .299 + G *  .587 + B *  .114;
		U = R * -.169 + G * -.332 + B *  .500 + 128;
		V = R *  .500 + G * -.419 + B * -.0813 + 128;
	*/
	static void InitCC();
	static DWORD RGBToYUV(BYTE r, BYTE g, BYTE b);

};
