#include <appincludes.h>

static BYTE* s_yuv[3];
static SIZE s_yuvsize;

void CKeystoneVideoPin::ZoomDeInit()
{
	for(int i = 0; i < 3; i++)
	{
		delete [] s_yuv[i];
		s_yuv[i] = NULL;
	}

	s_yuvsize.cx = 0;
	s_yuvsize.cy = 0;
}

bool CKeystoneVideoPin::ZoomInit(int w, int h)
{
	ZoomDeInit();
	if(w&3) return false;
	s_yuvsize.cx = w;
	s_yuvsize.cy = h;
	s_yuv[0] = new BYTE[(w+1)*(h+1)];
	s_yuv[1] = new BYTE[(w/2+1)*(h+1)];
	s_yuv[2] = new BYTE[(w/2+1)*(h+1)];
	if(!s_yuv[0] || !s_yuv[1] || !s_yuv[2]) {ZoomDeInit(); return false;}
	return true;
}

void CKeystoneVideoPin::Resize1D(BYTE* d, int dw, BYTE* s, int sw, int step)
{
	if(step == 1)
	{
		for(int i = 0, j = 0, dj = (sw<<16)/dw; i < dw-1; i++, d++, j += dj)
		{
			BYTE* p = &s[j>>16];
			int jfrac = j&0xffff;
			*d = ((p[0]*(0xffff-jfrac) + p[1]*jfrac) + 0x7fff) >> 16;
		}

		*d = s[sw-1];
	}
	else
	{
		for(int i = 0, j = 0, dj = (sw<<16)/dw; i < dw-1; i++, d += step, j += dj)
		{
			BYTE* p = &s[step*(j>>16)];
			int jfrac = j&0xffff;
			*d = ((p[0]*(0xffff-jfrac) + p[step]*jfrac) + 0x7fff) >> 16;
		}

		*d = s[step*(sw-1)];
	}
}

bool CKeystoneVideoPin::ZoomYUY2(LPBYTE src, int w, int h, RECT r)
{
	//DbgLog((LOG_TRACE, 0, TEXT("KEYSTONE: ZoomYUY2")));

	int sw = r.right - r.left;
	int sh = r.bottom - r.top;

	if((w&1) || (r.left&1) || (r.right&1)
	|| sw < 4 || sh < 2 || sw > w || sh > h
	|| (sw & 2) || (sh & 1)
	|| !s_yuv[0] || !s_yuv[1] || !s_yuv[2])
		return false;

	if(sw == w && sh == h)
		return true;

	BYTE* s = src + r.top*w*2 + r.left*2;
	BYTE* y = s_yuv[0] + w+1 + 1;
	BYTE* u = s_yuv[1] + w/2+1 + 1;
	BYTE* v = s_yuv[2] + w/2+1 + 1;

	for(int j = 0; j < sh; j++)
	{
		for(int i = 0; i < sw; i += 2)
		{
			*y++ = *s++;
			*v++ = *s++;
			*y++ = *s++;
			*u++ = *s++;
		}

		s += (w - sw)*2;
		y += (w - sw)+1;
		u += (w - sw)/2+1;
		v += (w - sw)/2+1;
	}

	y = s_yuv[0] + 1;
	u = s_yuv[1] + 1;
	v = s_yuv[2] + 1;

	for(int j = 0; j < sh; j++, y += w+1) Resize1D(y, w, y + w+1, sw, 1);
	for(int j = 0; j < sh; j++, u += w/2+1) Resize1D(u, w/2, u + w/2+1, sw/2, 1);
	for(int j = 0; j < sh; j++, v += w/2+1) Resize1D(v, w/2, v + w/2+1, sw/2, 1);

	if(sh < h)
	{
		y = s_yuv[0];
		u = s_yuv[1];
		v = s_yuv[2];

		for(int i = 0; i < w; i++, y++) Resize1D(y, h, y + 1, sh, w+1);
		for(int i = 0; i < w/2; i++, u++) Resize1D(u, h, u + 1, sh, w/2+1);
		for(int i = 0; i < w/2; i++, v++) Resize1D(v, h, v + 1, sh, w/2+1);

		y = s_yuv[0];
		u = s_yuv[1];
		v = s_yuv[2];
	}
	else
	{
		y = s_yuv[0] + 1;
		u = s_yuv[1] + 1;
		v = s_yuv[2] + 1;
	}

	s = src;

	for(int j = 0; j < h; j++)
	{
		for(int i = 0; i < w; i += 2)
		{
			*s++ = *y++;
			*s++ = *v++;
			*s++ = *y++;
			*s++ = *u++;
		}

		y++;
		u++;
		v++;
	}

	return true;
}