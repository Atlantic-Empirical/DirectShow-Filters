#ifndef __DEINTERLACEFILTER_H__
#define __DEINTERLACEFILTER_H__

#include "Deinterlace.h"
#include "DI.h"

#define MAX_FRAMES_IN_HISTORY	2
#define MAX_INPUT_LINES_PER_FIELD   2048

class CDeinterlacePlugin;

class CDeinterlaceFilter : public CTransformFilter,
		 public IDeinterlace,
		 public ISpecifyPropertyPages,
		 public CPersistStream
{

public:

    DECLARE_IUNKNOWN;
    static CUnknown*  WINAPI CreateInstance(LPUNKNOWN punk, HRESULT* phr);

    // Reveals IDeinterlace, ISpecifyPropertyPages, IPersistStream
    STDMETHOD(NonDelegatingQueryInterface)(REFIID riid, void**  ppv);

    // CPersistStream stuff
    HRESULT WriteToStream(IStream* pStream);
    HRESULT ReadFromStream(IStream* pStream);
    STDMETHOD(GetClassID)(CLSID* pClsid);

	CBasePin* GetPin(int n);

	//
	HRESULT CopyProperties(IMediaSample* pSource, IMediaSample* pDest);

    // Overrriden from CTransformFilter base class
	HRESULT Receive(IMediaSample* pSample);
    HRESULT CheckInputType(const CMediaType* mtIn);
    HRESULT CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut);
    HRESULT DecideBufferSize(IMemAllocator* pAlloc, ALLOCATOR_PROPERTIES* pProperties);
    HRESULT GetMediaType(int iPosition, CMediaType* pMediaType);
    HRESULT StartStreaming();
    HRESULT StopStreaming();

	// These implement the custom IIPEffect interface
    STDMETHOD(get_DeinterlaceType)(long* IPEffect);
    STDMETHOD(put_DeinterlaceType)(long IPEffect);
    STDMETHOD(get_IsOddFieldFirst)(VARIANT_BOOL* OddFirst);
    STDMETHOD(put_IsOddFieldFirst)(VARIANT_BOOL OddFirst);
    STDMETHOD(get_DScalerPluginName)(BSTR* OddFirst);
    STDMETHOD(put_DScalerPluginName)(BSTR OddFirst);

    // ISpecifyPropertyPages interface
    STDMETHOD(GetPages)(CAUUID* pPages);


private:
    // Constructor
    CDeinterlaceFilter(TCHAR* tszName, LPUNKNOWN punk, HRESULT* phr);
    BOOL CanPerformDeinterlace(const CMediaType* pMediaType) const;
	HRESULT GetOutputSampleBuffer(IMediaSample* pSource,IMediaSample** ppOutput);
    HRESULT Deinterlace(IMediaSample* pIn);
	void CallDeinterlaceMethod(DEINTERLACE_INFO* pInfo) const;


    CCritSec	m_DeinterlaceLock;  // Private play critical section
    int         m_DeinterlaceType;  // Which type of deinterlacing shall we do
	CComPtr<IMediaSample> m_pInputHistory[MAX_FRAMES_IN_HISTORY];
	CDeinterlacePlugin* m_pDeinterlacePlugin;
    DEINTERLACE_INFO m_Info;
    short* m_OddLines[2][MAX_INPUT_LINES_PER_FIELD];
	short* m_EvenLines[2][MAX_INPUT_LINES_PER_FIELD];
    int m_History;
    REFERENCE_TIME m_LastStop;
    BOOL m_bIsOddFieldFirst;
    CComBSTR m_PlugInName;
};

#endif
