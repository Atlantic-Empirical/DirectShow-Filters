#include "stdafx.h"

STDMETHODIMP CPushPinBitmap::Load(LPCOLESTR pszFileName, const AM_MEDIA_TYPE *pmt)
{
	HRESULT hr = m_QueueThread.Launch(pszFileName);

	if (FAILED(hr))
	{
		DbgLog((LOG_TRACE, 0, TEXT("File opened.")));
	}
	else
	{
		DbgLog((LOG_TRACE, 0, TEXT("File did not open.")));
	}

	return hr;
}

STDMETHODIMP CPushPinBitmap::GetCurFile(LPOLESTR *ppszFileName, AM_MEDIA_TYPE *pmt)
{
	//*ppszFileName = A2W(this->m_pPin->FilePath);
    return S_OK;
}

STDMETHODIMP CPushPinBitmap::SetATPF(REFERENCE_TIME TargetATPF)
{
	//*ppszFileName = A2W(this->m_pPin->FilePath);
	m_rtFrameLength = TargetATPF;
	rtLastEnd = m_rtFrameLength;
	return S_OK;
}

		//STDMETHOD Load(
		//	[in] LPCOLESTR pszFileName,     // Pointer to absolute path of file to open
		//	[in, unique] const AM_MEDIA_TYPE *pmt   // Media type of file - can be NULL
		//);
		//// Get the currently loaded file name
		//STDMETHOD GetCurFile(
		//	[out] LPOLESTR *ppszFileName,   // Pointer to the path for the current file
		//	[out] AM_MEDIA_TYPE *pmt        // Pointer to the media type
		//);