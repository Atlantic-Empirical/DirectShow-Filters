#pragma once

#define SAMPLE_SIZE_MAX	691200	//Make buffer that should be 699840 - you probably already have this, but it's for the structure below.
								//should be 691200 for NTSC and 829440 for PAL

// This is a structure we're going to use with our queue.  Basically, it's all the information we need 
// to take from the IMediaSample, so we can create a new one when we need it.
struct strucCompleteFrame
{
	REFERENCE_TIME		rtTimeStart;								// start reference time
	REFERENCE_TIME		rtTimeEnd;									// end reference time
	byte				bSampleBuffer[ SAMPLE_SIZE_MAX ];			// the buffer.
	bool				bIsDiscontinuity;							// quick discontinuity flag
	bool				bIsSyncPoint;								// sync point flag
	LPBYTE				pSampleAddress;								// address of a image buffer
};

//Structure for sample meta data
typedef struct  _SampleMetaData {
	
	//AM_MEDIA_TYPE
	GUID      MT_majortype;
	GUID      MT_subtype;
	BOOL      MT_bFixedSizeSamples;
	BOOL      MT_bTemporalCompression;
	ULONG     MT_lSampleSize;
	GUID      MT_formattype;
	IUnknown  *MT_pUnk;
	ULONG     MT_cbFormat;
	BYTE *MT_pbFormat;

	//VIDEOINFOHEADER
	RECT                VIH_rcSource;
	RECT                VIH_rcTarget;
	DWORD               VIH_dwBitRate;
	DWORD               VIH_dwBitErrorRate;
	REFERENCE_TIME      VIH_AvgTimePerFrame;
	BITMAPINFOHEADER    VIH_bmiHeader;

	//VIDEOINFOHEADER2
	DWORD               VIH2_dwInterlaceFlags;
	DWORD               VIH2_dwCopyProtectFlags;
	DWORD               VIH2_dwPictAspectRatioX; 
	DWORD               VIH2_dwPictAspectRatioY; 
	DWORD				VIH2_dwControlFlags;
	
	//BITMAPINFOHEADER
	DWORD  BMI_biSize;
	LONG   BMI_biWidth;
	LONG   BMI_biHeight;
	WORD   BMI_biPlanes;
	WORD   BMI_biBitCount;
	DWORD  BMI_biCompression;
	DWORD  BMI_biSizeImage;
	LONG   BMI_biXPelsPerMeter;
	LONG   BMI_biYPelsPerMeter;
	DWORD  BMI_biClrUsed;
	DWORD  BMI_biClrImportant;

	//AM_SAMPLE2_PROPERTIES
	DWORD           S2P_cbData;
	DWORD           S2P_dwTypeSpecificFlags;
	DWORD           S2P_dwSampleFlags;
	LONG            S2P_lActual;
	REFERENCE_TIME  S2P_tStart;
	REFERENCE_TIME  S2P_tStop;
	DWORD           S2P_dwStreamId;
	AM_MEDIA_TYPE   *S2P_pMediaType;
	BYTE            *S2P_pbBuffer;
	LONG            S2P_cbBuffer;

	//AM_SAMPLE_PROPERTY_FLAGS
	BYTE SPLICEPOINT;
	BYTE PREROLL;
	BYTE DATADISCONTINUITY;
	BYTE TYPECHANGED;
	BYTE TIMEVALID;
	BYTE TIMEDISCONTINUITY;
	BYTE FLUSH_ON_PAUSE;
	BYTE STOPVALID;
	BYTE ENDOFSTREAM;
	BYTE STREAM_MEDIA;
	BYTE STREAM_CONTROL;

	//TYPE_SPECIFIC_FLAGS
	BYTE FIELD_MASK;
	BYTE INTERLEAVED_FRAME;
	BYTE FIELD1;
	BYTE FIELD2;
	BYTE FIELD1FIRST;
	BYTE WEAVE;
	BYTE REPEAT_FIELD;

} SMT_SAMPLE_METADATA;

struct stMixData
{
	LPBYTE pImage;
	int nW;
	int nH;
	int nX;
	int nY;
	int nImage_Format;
	RGBQUAD * Palette;
	COLORREF KeyColor;
};

typedef enum _ImageFormat
{
	IF_RGB24 =  0,
	IF_RGB32 = 1,
	IF_YUY2 = 2,
	IF_UYVY = 3,
	IF_ARGB4444 = 4,
	IF_RGB8 = 5
} ImageFormat;

typedef enum _eSampleFrom
{
	VideoIn =  0,
	SubpictureIn = 1,
	Line21In = 2,
	Output = 3,
} eSampleFrom;
