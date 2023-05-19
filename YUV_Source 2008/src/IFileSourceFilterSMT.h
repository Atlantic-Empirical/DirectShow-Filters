#ifndef __IFileSourceFilterSMT__
#define __IFileSourceFilterSMT__

#ifdef __cplusplus
extern "C" {
#endif

	// {5AA8CFff-9235-4b4d-A7B2-8C2BA21D0104}
	DEFINE_GUID(IID_IFileSourceFilterSMT, 0x5aa8cfff, 0x9235, 0x4b4d, 0xa7, 0xb2, 0x8c, 0x2b, 0xa2, 0x1d, 0x1, 0x4);

	DECLARE_INTERFACE_(IFileSourceFilterSMT, IUnknown)
    {
		STDMETHOD(Load) (THIS_ 
            /* [in] */ LPCOLESTR pszFileName,
            /* [unique][in] */ const AM_MEDIA_TYPE *pmt
		);

		STDMETHOD(GetCurFile) (THIS_ 
            /* [out] */ LPOLESTR *ppszFileName,
            /* [out] */ AM_MEDIA_TYPE *pmt
		);

		STDMETHOD(SetATPF) (THIS_ 
            REFERENCE_TIME TargetATPF
		);

	};

#ifdef __cplusplus
}
#endif

#endif // __IFileSourceFilterSMT__
