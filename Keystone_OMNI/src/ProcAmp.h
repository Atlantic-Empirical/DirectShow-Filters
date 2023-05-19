#pragma once

#pragma warning ( disable : 4244 ) 

class CProcAmp
{
	// Brightness: -255.0 to 255.0, default 0.0
	// Contrast: 0.0 to 10.0, default 1.0
	// Hue: -180.0 to +180.0, default 0.0
	// Saturation: 0.0 to 10.0, default 1.0

	double m_bright, m_cont, m_hue, m_sat;
	BYTE m_YTbl[256], m_UTbl[256*256], m_VTbl[256*256];
	CCritSec m_csProps;

public:
	CProcAmp()
	{
		SetBrightness(0.0);
		SetContrast(1.0);
		SetHue(0.0);
		SetSaturation(1.0);
	}

private:
	void CalcBrCont(BYTE* YTbl, double bright, double cont)
	{
		int Cont = (int)(cont * 512);
		int Bright = (int)bright;

		for(int i = 0; i < 256; i++)
		{
			int y = ((Cont * (i - 16)) >> 9) + Bright + 16;
			YTbl[i] = min(max(y, 0), 255);
			// YTbl[i] = min(max(y, 16), 235);
		}
	}

	void CalcHueSat(BYTE* UTbl, BYTE* VTbl, double hue, double sat)
	{
		int Sat = (int)(sat * 512);
		double Hue = (hue * 3.1415926) / 180.0;
		int Sin = (int)(sin(Hue) * 4096);
		int Cos = (int)(cos(Hue) * 4096);

		for(int y = 0; y < 256; y++)
		{
			for(int x = 0; x < 256; x++)
			{
				int u = x - 128; 
				int v = y - 128;
				int ux = (u * Cos + v * Sin) >> 12;
				v = (v * Cos - u * Sin) >> 12;
				u = ((ux * Sat) >> 9) + 128;
				v = ((v * Sat) >> 9) + 128;
				u = min(max(u, 16), 235);
				v = min(max(v, 16), 235);
				UTbl[(y << 8) | x] = u;
				VTbl[(y << 8) | x] = v;
			}
		}
	}

public:
	//this works only for yv12 and i420
	void ApplyBrContHueSat_YUV(BYTE* srcy, BYTE* srcu, BYTE* srcv, int w, int h, int pitch)
	{
		CAutoLock cAutoLock(&m_csProps);

		static const double EPSILON = 1e-4;

		if(fabs(m_bright-0.0) > EPSILON || fabs(m_cont-1.0) > EPSILON)
		{
			for(int size = pitch*h; size > 0; size--)
			{
				*srcy++ = m_YTbl[*srcy];
			}
		}

		pitch /= 2;
		w /= 2;
		h /= 2;

		if(fabs(m_hue-0.0) > EPSILON || fabs(m_sat-1.0) > EPSILON)
		{
			for(int size = pitch*h; size > 0; size--)
			{
				WORD uv = (*srcv<<8)|*srcu;
				*srcu++ = m_UTbl[uv];
				*srcv++ = m_VTbl[uv];
			}
		}
	}

	//TF Attempt Half Frame
	void ApplyBrContHueSat_UYVY(BYTE* src, int w, int h, int pitch, bool HalfFrame)
	{
		CAutoLock cAutoLock(&m_csProps);

		static const double EPSILON = 1e-4;

		BYTE* srcy = src+1;

		if(fabs(m_bright-0.0) > EPSILON || fabs(m_cont-1.0) > EPSILON)
		{
			for(int j = 0; j < h; j++, srcy += pitch)
			{
				BYTE* cur = srcy;
				BYTE* end = 0;
				if (HalfFrame)
				{
					end = srcy + w;
				}
				else
				{
					end = srcy + w*2;
				}
                
				for(; cur < end; cur += 2)
				{
					*cur = m_YTbl[*cur];
				}
			}
		}

		BYTE* srcuv = src;

		if(fabs(m_hue-0.0) > EPSILON || fabs(m_sat-1.0) > EPSILON)
		{
			for(int j = 0; j < h; j++, srcuv += pitch)
			{
				BYTE* cur = srcuv;
				BYTE* end = 0;
				if (HalfFrame)
				{
					end = srcuv + w;
				}
				else
				{
					end = srcuv + w*2;
				}
                
				for(; cur < end; cur += 4)
				{
					WORD uv = (cur[2]<<8) | cur[0];
					cur[0] = m_UTbl[uv];
					cur[2] = m_VTbl[uv];
				}
			}
		}
	}

	STDMETHODIMP SetBrightness(double bright)
	{
		CAutoLock cAutoLock(&m_csProps);
		CalcBrCont(m_YTbl, m_bright = bright, m_cont);
		return S_OK;
	}

	STDMETHODIMP SetContrast(double cont)
	{
		CAutoLock cAutoLock(&m_csProps);
		CalcBrCont(m_YTbl, m_bright, m_cont = cont);
		return S_OK;
	}

	STDMETHODIMP SetHue(double hue)
	{
		CAutoLock cAutoLock(&m_csProps);
		CalcHueSat(m_UTbl, m_VTbl, m_hue = hue, m_sat);
		return S_OK;
	}

	STDMETHODIMP SetSaturation(double sat)
	{
		CAutoLock cAutoLock(&m_csProps);
		CalcHueSat(m_UTbl, m_VTbl, m_hue, m_sat = sat);
		return S_OK;
	}

	STDMETHODIMP_(double) GetBrightness()
	{
		CAutoLock cAutoLock(&m_csProps);
		return m_bright;
	}

	STDMETHODIMP_(double) GetContrast()
	{
		CAutoLock cAutoLock(&m_csProps);
		return m_cont;
	}

	STDMETHODIMP_(double) GetHue()
	{
		CAutoLock cAutoLock(&m_csProps);
		return m_hue;
	}

	STDMETHODIMP_(double) GetSaturation()
	{
		CAutoLock cAutoLock(&m_csProps);
		return m_sat;
	}
};
