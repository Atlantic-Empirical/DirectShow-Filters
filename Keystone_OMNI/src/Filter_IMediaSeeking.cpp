#include "appincludes.h"

//typedef enum AM_SEEKING_SeekingFlags
//{
//    AM_SEEKING_NoPositioning        = 0x00,     // No change
//    AM_SEEKING_AbsolutePositioning  = 0x01,     // Position is supplied and is absolute
//    AM_SEEKING_RelativePositioning  = 0x02,     // Position is supplied and is relative
//    AM_SEEKING_IncrementalPositioning   = 0x03, // (Stop) position relative to current
//                                                // Useful for seeking when paused (use +1)
//    AM_SEEKING_PositioningBitsMask  = 0x03,     // Useful mask
//    AM_SEEKING_SeekToKeyFrame       = 0x04,     // Just seek to key frame (performance gain)
//    AM_SEEKING_ReturnTime           = 0x08,     // Plug the media time equivalents back into the supplied LONGLONGs
//
//    AM_SEEKING_Segment              = 0x10,     // At end just do EC_ENDOFSEGMENT,
//                                                // don't do EndOfStream
//    AM_SEEKING_NoFlush              = 0x20      // Don't flush
//} AM_SEEKING_SEEKING_FLAGS;
//
//typedef enum AM_SEEKING_SeekingCapabilities
//{
//    AM_SEEKING_CanSeekAbsolute     = 0x001,
//    AM_SEEKING_CanSeekForwards     = 0x002,
//    AM_SEEKING_CanSeekBackwards    = 0x004,
//    AM_SEEKING_CanGetCurrentPos    = 0x008,
//    AM_SEEKING_CanGetStopPos       = 0x010,
//    AM_SEEKING_CanGetDuration      = 0x020,
//    AM_SEEKING_CanPlayBackwards    = 0x040,
//    AM_SEEKING_CanDoSegments       = 0x080,
//    AM_SEEKING_Source              = 0x100  // Doesn't pass thru used to
//                                            // count segment ends
//} AM_SEEKING_SEEKING_CAPABILITIES;

// Returns the capability flags
STDMETHODIMP CKeystone::GetCapabilities(DWORD * pCapabilities)
{
	//*pCapabilities = AM_SEEKING_CanSeekAbsolute | AM_SEEKING_CanSeekForwards | AM_SEEKING_CanSeekBackwards | AM_SEEKING_CanPlayBackwards | AM_SEEKING_CanDoSegments;
	*pCapabilities = 199;
	return S_OK;
};

// And's the capabilities flag with the capabilities requested.
// Returns S_OK if all are present, S_FALSE if some are present, E_FAIL if none.
// *pCababilities is always updated with the result of the 'and'ing and can be
// checked in the case of an S_FALSE return code.
//HRESULT CheckCapabilities( [in,out] DWORD * pCapabilities );
STDMETHODIMP CKeystone::CheckCapabilities(DWORD * pCapabilities)
{
	return S_OK;
};

// returns S_OK if mode is supported, S_FALSE otherwise
//HRESULT IsFormatSupported([in] const GUID * pFormat);
STDMETHODIMP CKeystone::IsFormatSupported(const GUID * pFormat)
{
	return S_OK;
};

//HRESULT QueryPreferredFormat([out] GUID * pFormat);
STDMETHODIMP CKeystone::QueryPreferredFormat(GUID * pFormat)
{
	*pFormat = TIME_FORMAT_MEDIA_TIME;
	return S_OK;
};

//HRESULT GetTimeFormat([out] GUID *pFormat);
STDMETHODIMP CKeystone::GetTimeFormat(GUID * pFormat)
{
	*pFormat = CurrentTimeFormat;
	return S_OK;
};

// Returns S_OK if *pFormat is the current time format, otherwise S_FALSE
// This may be used instead of the above and will save the copying of the GUID
//HRESULT IsUsingTimeFormat([in] const GUID * pFormat);
STDMETHODIMP CKeystone::IsUsingTimeFormat(const GUID * pFormat)
{
	if (*pFormat == CurrentTimeFormat)
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
};

// (may return VFE_E_WRONG_STATE if graph is stopped)
//HRESULT SetTimeFormat([in] const GUID * pFormat);
STDMETHODIMP CKeystone::SetTimeFormat(const GUID * pFormat)
{
	CurrentTimeFormat = *pFormat;
	return S_OK;
};

// return current properties
//HRESULT GetDuration([out] LONGLONG *pDuration);
STDMETHODIMP CKeystone::GetDuration(LONGLONG *pDuration)
{
	*pDuration = 500000000;
	return S_FALSE;
};

//HRESULT GetStopPosition([out] LONGLONG *pStop);
STDMETHODIMP CKeystone::GetStopPosition(LONGLONG *pStop)
{
	return S_OK;
};

//HRESULT GetCurrentPosition([out] LONGLONG *pCurrent);
STDMETHODIMP CKeystone::GetCurrentPosition(LONGLONG *pCurrent)
{
	return S_OK;
};

// Convert time from one format to another.
// We must be able to convert between all of the formats that we say we support.
// (However, we can use intermediate formats (e.g. MEDIA_TIME).)
// If a pointer to a format is null, it implies the currently selected format.
//HRESULT ConvertTimeFormat([out] LONGLONG * pTarget, [in] const GUID * pTargetFormat,
//                          [in]  LONGLONG    Source, [in] const GUID * pSourceFormat );
STDMETHODIMP CKeystone::ConvertTimeFormat(LONGLONG * pTarget, const GUID * pTargetFormat, LONGLONG Source, const GUID * pSourceFormat)
{
	return S_OK;
};

// Set current and end positions in one operation
// Either pointer may be null, implying no change
//HRESULT SetPositions( [in,out] LONGLONG * pCurrent, [in] DWORD dwCurrentFlags
//        , [in,out] LONGLONG * pStop, [in] DWORD dwStopFlags );
STDMETHODIMP CKeystone::SetPositions(LONGLONG * pCurrent, DWORD dwCurrentFlags, LONGLONG * pStop, DWORD dwStopFlags)
{
	return S_OK;
};

// Get CurrentPosition & StopTime
// Either pointer may be null, implying not interested
//HRESULT GetPositions( [out] LONGLONG * pCurrent,
//                      [out] LONGLONG * pStop );
STDMETHODIMP CKeystone::GetPositions(LONGLONG * pCurrent, LONGLONG * pStop)
{
	return S_OK;
};

// Get earliest / latest times to which we can currently seek "efficiently".
// This method is intended to help with graphs where the source filter has
// a very high latency.  Seeking within the returned limits should just
// result in a re-pushing of already cached data.  Seeking beyond these
// limits may result in extended delays while the data is fetched (e.g.
// across a slow network).
// (NULL pointer is OK, means caller isn't interested.)
//HRESULT GetAvailable( [out] LONGLONG * pEarliest, [out] LONGLONG * pLatest );
STDMETHODIMP CKeystone::GetAvailable(LONGLONG * pEarliest, LONGLONG * pLatest)
{
	return S_OK;
};

// Rate stuff
//HRESULT SetRate([in]  double dRate);
STDMETHODIMP CKeystone::SetRate(double dRate)
{
	return S_OK;
};

//HRESULT GetRate([out] double * pdRate);
STDMETHODIMP CKeystone::GetRate(double * pdRate)
{
	return S_OK;
};

// Preroll
//HRESULT GetPreroll([out] LONGLONG * pllPreroll);
STDMETHODIMP CKeystone::GetPreroll(LONGLONG * pllPreroll)
{
	return S_OK;
};
