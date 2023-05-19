#pragma once

#define FPS_TO_RT(fps) REFERENCE_TIME((double)UNITS / (fps))

//class CMediaTypeUtils
//{
//public:
//	static void CreateVideoMediaType(int width, int height, WORD bpp, DWORD format, REFERENCE_TIME timePerFrame, CMediaType* pMediaType)
//	{
//		VIDEOINFO* info = (VIDEOINFO*)pMediaType->AllocFormatBuffer(sizeof VIDEOINFO);
//		ZeroMemory(info, sizeof VIDEOINFO);
//
//		info->bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
//		info->bmiHeader.biWidth        = width;
//		info->bmiHeader.biHeight       = height;
//		info->bmiHeader.biPlanes       = 1;
//		info->bmiHeader.biCompression  = format;
//		info->bmiHeader.biBitCount     = bpp;
//		info->bmiHeader.biSizeImage    = GetBitmapSize(&info->bmiHeader);
//		info->bmiHeader.biClrImportant = 0;
//
//		SetRectEmpty(&(info->rcSource)); // we want the whole image area rendered.
//		SetRectEmpty(&(info->rcTarget)); // no particular destination rectangle
//
//		info->AvgTimePerFrame = timePerFrame;
//		if (timePerFrame > 0)
//			info->dwBitRate = info->bmiHeader.biSizeImage * DWORD(UNITS / timePerFrame) * 8;
//
//		info->rcSource.right = width;
//		info->rcSource.bottom = abs(height);
//		info->rcTarget.right = width;
//		info->rcTarget.bottom = abs(height);
//
//		pMediaType->SetType(&MEDIATYPE_Video);
//		pMediaType->SetFormatType(&FORMAT_VideoInfo);
//		pMediaType->SetTemporalCompression(FALSE);
//
//		// Work out the GUID for the subtype from the header info.
//		const GUID SubTypeGUID = GetBitmapSubtype(&info->bmiHeader);
//		pMediaType->SetSubtype(&SubTypeGUID);
//		pMediaType->SetSampleSize(info->bmiHeader.biSizeImage);
//	}
//};

//class CFilterUtils
//{
//public:
//	static void GetPinFilter(IPin* pin, IBaseFilter** filter)
//	{
//		PIN_INFO info;
//		pin->QueryPinInfo(&info);
//		*filter = info.pFilter;
//	}
//	static void GetOutputPin(IBaseFilter* filter, IPin** pin)
//	{
//		CComPtr<IEnumPins> en;
//		if (SUCCEEDED(filter->EnumPins(&en)))
//		{
//			CComPtr<IPin> res;
//			CComPtr<IPin> p;
//			ULONG got;
//			while((res == NULL) && (en->Next(1, &p, &got) == S_OK))
//			{
//				PIN_DIRECTION dir;
//				p->QueryDirection(&dir);
//				if (dir == PINDIR_OUTPUT)
//				{
//					CComPtr<IPin> dest;
//					p->ConnectedTo(&dest);
//					if (dest != NULL)
//						res = p;
//				}
//				p.Release();
//			}
//			*pin = res.Detach();
//		}
//	}
//
//	static void SetNullClockInPrev(IPin* _pin)
//	{
//		CComPtr<IPin> pin;
//		_pin->ConnectedTo(&pin);
//		CComPtr<IBaseFilter> flt;
//		GetPinFilter(pin, &flt);
//		CComQIPtr<IMediaFilter> mf = flt;
//		if (mf != NULL)
//            mf->SetSyncSource(NULL);
//	}
//
//	static void SetNullClockInRenderer(IPin* _pin)
//	{
//		CComPtr<IPin> pin;
//		_pin->ConnectedTo(&pin);
//		while (pin != NULL)
//		{
//			CComPtr<IBaseFilter> flt;
//			GetPinFilter(pin, &flt);
//			CComPtr<IPin> out;
//			GetOutputPin(flt, &out);
//			pin.Release();
//			if (out != NULL) // we found the renderer
//			{
//				out->ConnectedTo(&pin);
//			}
//			else
//			{
//				CComQIPtr<IMediaFilter> mf = flt;
//				if (mf != NULL)
//				{
//					HRESULT hr = mf->SetSyncSource(NULL);
//					if (FAILED(hr))
//						Beep(1000, 10);
//				}
//			}
//			flt.Release();
//		}
//
//
//		// find last filter in the chain
//
//	}
//};
//
//class CAutoLog
//{
//public:
//	CAutoLog(LPCSTR name) :
//	  m_Name(name)
//	{
//		OutputDebugStr("Enter: " + m_Name + "\n");
//	}
//	~CAutoLog()
//	{
//		OutputDebugStr("Leave: " + m_Name + "\n");
//	}
//private:
//	CString m_Name;
//};