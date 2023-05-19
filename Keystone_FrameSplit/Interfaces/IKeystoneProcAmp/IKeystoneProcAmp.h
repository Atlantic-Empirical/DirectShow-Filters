#ifndef __IKeystoneProcAmp__
#define __IKeystoneProcAmp__

#ifdef __cplusplus
extern "C" {
#endif

	// {4F96B59E-11EB-4e02-9A7C-1CA55AB7D7FF}
	DEFINE_GUID(IID_IKeystoneProcAmp, 0x4f96b59e, 0x11eb, 0x4e02, 0x9a, 0x7c, 0x1c, 0xa5, 0x5a, 0xb7, 0xd7, 0xff);

    DECLARE_INTERFACE_(IKeystoneProcAmp, IUnknown)
    {

		STDMETHOD (put_Brightness )(THIS_ 
            double dBrightness
		);
        
        STDMETHOD ( get_Brightness ) (THIS_ 
            double *dBrightness
		);

		STDMETHOD (put_Contrast )(THIS_ 
            double dContrast
		);
        
        STDMETHOD ( get_Contrast ) (THIS_ 
            double *dContrast
		);

		STDMETHOD (put_Hue )(THIS_ 
            double dHue
		);
        
        STDMETHOD ( get_Hue ) (THIS_ 
            double *dHue
		);

		STDMETHOD (put_Saturation )(THIS_ 
            double dSaturation
		);
        
        STDMETHOD ( get_Saturation ) (THIS_ 
            double *dSaturation
		);

		STDMETHOD ( ToggleProcAmp) (THIS_
			bool bToggleProcAmp,
			bool bHalfFrame
		);

		STDMETHOD ( ToggleColorFilter) (THIS_
			bool bDoColorFilter,
			int iUseWhichFilter
		);

	};

#ifdef __cplusplus
}
#endif

#endif // __IKeystoneProcAmp__

