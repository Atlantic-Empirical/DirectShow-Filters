#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	// {5AA8CFEE-9235-4b4d-A7B2-8C2BA21D0104}
	DEFINE_GUID(IID_IKeystoneMixer, 0x5aa8cfee, 0x9235, 0x4b4d, 0xa7, 0xb2, 0x8c, 0x2b, 0xa2, 0x1d, 0x1, 0x4);

	DECLARE_INTERFACE_(IKeystoneMixer, IUnknown)
    {
		STDMETHOD(put_OSD) (THIS_ 
            LPBYTE pMixImage,
            int W,
            int H,
            int X,
            int Y,
            int Format,
			COLORREF KeyColor,
			int DurationSecs
		);
        
        STDMETHOD (ClearOSD)( THIS_ );

        STDMETHOD (GrabLastL21)( THIS_ 
			LPBYTE * pL21Sample
		);

        STDMETHOD (GrabLastSubpicture)( THIS_ 
			LPBYTE * pSubpictureSample
		);

        STDMETHOD (SetGuides)( THIS_ 
			int Left,
			int Top, 
			int Right,
			int Bottom, 
			int Red, 
			int Blue, 
			int Green
		);

		STDMETHOD (ClearGuides)( THIS_  );

		STDMETHOD (SetSPPlacement)( THIS_ 
			int X,
			int Y
		);

		STDMETHOD (SetResizeMode)( THIS_ 
			int Mode
		);

		STDMETHOD (SetLBColor)( THIS_ 
			int Red,
			int Green,
			int Blue
		);

		STDMETHOD (SetL21Placement)( THIS_ 
			int X,
			int Y
		);

		STDMETHOD (ReverseFieldOrder)( THIS_ 
			bool bReverseIt
		);

		STDMETHOD (SetJacketPicMode)( THIS_ 
			bool bJackPicMode
		);

		STDMETHOD (BumpFieldsDown)( THIS_ 
			int bBumpFields
		);

		STDMETHOD (BurnGOPTCs)( THIS_ 
			int iBurnGOPTCs, 
			int iBlinkRed
		);

		STDMETHOD (SetActionTitleGuides)( THIS_ 
			int iShowGuides,
			int Red, 
			int Blue, 
			int Green
		);

		STDMETHOD (FieldSplit)( THIS_ 
			int DoFieldSplit
		);

		STDMETHOD (HighContrastSP)( THIS_ 
			int DoHighContrastSP
		);

		STDMETHOD (SetARFlags)( THIS_ 
			int PS, 
			int LB
		);

		STDMETHOD (SetActionTitleSafeColor)( THIS_ 
			int Red,
			int Green,
			int Blue
		);

		STDMETHOD (BarDataConfig)( THIS_ 
			int DetectBarData,
			int BurnGuides,
			int Luma_Tolerance,
			int Chroma_Tolerance
		);

		STDMETHOD (BarDataReset)( THIS_ );

	};

#ifdef __cplusplus
}
#endif
