
#ifndef __IL21G__
#define __IL21G__

#ifdef __cplusplus
extern "C" {
#endif

	// {A231C86D-F2B5-406b-990A-6A235C5342DA}
	DEFINE_GUID(IID_IL21G, 
	0xa231c86d, 0xf2b5, 0x406b, 0x99, 0xa, 0x6a, 0x23, 0x5c, 0x53, 0x42, 0xda);

    DECLARE_INTERFACE_(IL21G, IUnknown)
    {

		STDMETHOD(GetBuffer) (THIS_
			LPBYTE *pSample, 
			long *BufferSize
		) PURE;

	};

#ifdef __cplusplus
}
#endif

#endif // __IL21G__

