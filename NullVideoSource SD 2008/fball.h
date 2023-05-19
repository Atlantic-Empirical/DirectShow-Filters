//------------------------------------------------------------------------------
// File: FBall.h
//
// Desc: DirectShow sample code - main header file for the bouncing ball
//       source filter.  For more information refer to Ball.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Define GUIDS used in this sample
//------------------------------------------------------------------------------
// { fd501075-8ebe-11ce-8183-00aa00577da1 }
DEFINE_GUID(CLSID_BouncingBall,
0xfd501075, 0x8ebe, 0x11ce, 0x81, 0x83, 0x00, 0xaa, 0x00, 0x57, 0x7d, 0xa1);

//------------------------------------------------------------------------------
// Forward Declarations
//------------------------------------------------------------------------------
// The class managing the output pin
class CBallStream;


//------------------------------------------------------------------------------
// Class CBouncingBall
//
// This is the main class for the bouncing ball filter. It inherits from
// CSource, the DirectShow base class for source filters.
//------------------------------------------------------------------------------
class CBouncingBall : public CSource
{
public:

    // The only allowed way to create Bouncing balls!
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

private:

    // It is only allowed to to create these objects with CreateInstance
    CBouncingBall(LPUNKNOWN lpunk, HRESULT *phr);

}; // CBouncingBall


//------------------------------------------------------------------------------
// Class CBallStream
//
// This class implements the stream which is used to output the bouncing ball
// data from the source filter. It inherits from DirectShows's base
// CSourceStream class.
//------------------------------------------------------------------------------
class CBallStream : public CSourceStream
{

public:

    CBallStream(HRESULT *phr, CBouncingBall *pParent, LPCWSTR pPinName);
    ~CBallStream();

    // plots a ball into the supplied video frame
    HRESULT FillBuffer(IMediaSample *pms);

    // Ask for buffers of the size appropriate to the agreed media type
    HRESULT DecideBufferSize(IMemAllocator *pIMemAlloc, ALLOCATOR_PROPERTIES *pProperties);

    // Set the agreed media type, and set up the necessary ball parameters
    HRESULT SetMediaType(const CMediaType *pMediaType);

    // Because we calculate the ball there is no reason why we
    // can't calculate it in any one of a set of formats...
    HRESULT CheckMediaType(const CMediaType *pMediaType);
    HRESULT GetMediaType(int iPosition, CMediaType *pmt);

    // Resets the stream time to zero
    HRESULT OnThreadCreate(void);

    // Quality control notifications sent to us
    STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);

private:

    int m_iImageHeight;                 // The current image height
    int m_iImageWidth;                  // And current image width
    int m_iRepeatTime;                  // Time in msec between frames
    const int m_iDefaultRepeatTime;     // Initial m_iRepeatTime

	HRESULT SetFullBufferBGColorYUY2(LPBYTE dst, int dstpitch, int w, int h);

    CCritSec m_cSharedState;            // Lock on m_rtSampleTime and m_Ball
    CRefTime m_rtSampleTime;            // The time stamp for each sample

    // set up the palette appropriately
    enum Colour {Red, Blue, Green, Yellow, Black};
    HRESULT SetPaletteEntries(Colour colour);

}; // CBallStream
    
