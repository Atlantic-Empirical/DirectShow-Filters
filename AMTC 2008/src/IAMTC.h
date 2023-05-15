#ifndef __IAMTC__
#define __IAMTC__

#ifdef __cplusplus
extern "C" {
#endif

	// {B388D24F-8CD8-4cc2-BCC3-12274BABCD24}
	DEFINE_GUID(IID_IAMTC, 
	0xb388d24f, 0x8cd8, 0x4cc2, 0xbc, 0xc3, 0x12, 0x27, 0x4b, 0xab, 0xcd, 0x24);


    DECLARE_INTERFACE_(IAMTC, IUnknown)
    {

		STDMETHOD(GetBuffer) (THIS_
			LPBYTE *pSample, 
			long *BufferSize
		) PURE;

		STDMETHOD(StoreBuffers) (THIS_
			bool bUserWantsBuffersStored
		) PURE;

		STDMETHOD ( FrameStep ) (THIS_ 
            bool bForward
		);

		STDMETHOD ( QuitFrameStepping ) (THIS_ );

		STDMETHOD ( SetNULLTimestamps ) (THIS_ );

	};

#ifdef __cplusplus
}
#endif

#endif // __IAMTC__

