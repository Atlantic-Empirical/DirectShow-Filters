#pragma once

//#include "../../Utility/appincludes.h"
//#include <ippi.h>

class CColorSpaceConversionsIPP
{
public:

	//TO RGB24
	static HRESULT RGB32ToRGB24(LPBYTE src, int W, int H, LPBYTE dst)
	{
		////TODO: IANIER
		////Create the RGB24 target buffer
		//LPBYTE RGB24Buffer = NULL;
		//RGB24Buffer = (LPBYTE)malloc(H * W * 4);

		//dst = RGB24Buffer;
		dst = src;
		return S_OK;
	}

	static HRESULT UYVYToRGB24(LPBYTE src, int W, int H, LPBYTE* dst)
	{
		//TODO: IANIER - Priority A - For frame capture of output image.
		*dst = src;
		return S_OK;
	};

	static HRESULT YUY2ToRGB24(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER
		dst = src;
		return S_OK;
	};

	static HRESULT ARGB4444ToRGB24(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER
		dst = src;
		return S_OK;
	}

	//TO RGB32
	static HRESULT RGB24ToRGB32(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER	
		return S_OK;
	};

	static HRESULT UYVYToRGB32(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER	
		return S_OK;
	}

	static HRESULT YUY2ToRGB32(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER	
		return S_OK;
	};

	static HRESULT ARGB4444ToRGB32(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER
		return S_OK;
	}

	//TO UYVY
	static HRESULT RGB24ToUYVY(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER	
		return S_OK;
	};

	static HRESULT RGB32ToUYVY(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER	
		return S_OK;
	};

	static HRESULT YUY2ToUYVY(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER	
		return S_OK;
	}

	static HRESULT ARGB4444ToUYVY(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER
		return S_OK;
	}

	//TO YUY2
	static HRESULT RGB24ToYUY2(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER - Priority 1
		return S_OK;
	}

	static HRESULT RGB32ToYUY2(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER	
		return S_OK;
	}

	static HRESULT UYVYToYUY2(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER	
		return S_OK;
	}

	static HRESULT ARGB4444ToYUY2(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER - Priority 1
		return S_OK;
	}

	//TO ARGB4444
	static HRESULT RGB24ToARGB4444(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER
		return S_OK;
	}

	static HRESULT RGB32ToARGB4444(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER	
		return S_OK;
	}

	static HRESULT UYVYToARGB4444(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER	
		return S_OK;
	}

	static HRESULT YUY2ToARGB4444(LPBYTE src, int W, int H, LPBYTE dst)
	{
		//TODO: IANIER
		return S_OK;
	}
};