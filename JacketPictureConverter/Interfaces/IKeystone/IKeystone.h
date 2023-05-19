#ifndef __IKeystone__
#define __IKeystone__

#ifdef __cplusplus
extern "C" {
#endif

    // {fd5010a3-8ebe-11ce-8183-00aa00577da1}
    DEFINE_GUID(IID_IKeystone, 0xfd5010a3, 0x8ebe, 0x11ce, 0x81, 0x83, 0x00, 0xaa, 0x00, 0x57, 0x7d, 0xa1);

    DECLARE_INTERFACE_(IKeystone, IUnknown)
    {
        STDMETHOD(get_InputMediaType) (THIS_
            CMediaType *InputMediaType      
        ) PURE;

        STDMETHOD(get_OutputMediaType) (THIS_
            CMediaType *OutputMediaType      
        ) PURE;

        STDMETHOD(get_InputWidth) (THIS_
            long *InputWidth
        ) PURE;

		STDMETHOD(get_InputHeight) (THIS_
            long *InputHeight
        ) PURE;

        STDMETHOD(get_OutputWidth) (THIS_
            long *OutputWidth
        ) PURE;

		STDMETHOD(get_OutputHeight) (THIS_
            long *OutputHeight
        ) PURE;

		STDMETHOD(GrabSample) (THIS_
			int in_SampleWhat, 
			LPBYTE * out_pSample, 
			long *out_lSampleSize, 
			long *out_Width, 
			long *out_Height
		) PURE;

		STDMETHOD(UnlockFilter) (THIS_
            GUID *FilterKey
        ) PURE;

		STDMETHOD(put_FeedbackClicks) (THIS_
            bool DoClicks
        ) PURE;

		STDMETHOD(get_FeedbackClicks) (THIS_
            bool *DoClicks
        ) PURE;

        STDMETHOD ( Set32Status ) (THIS_ 
            int Do32
		);

		STDMETHOD ( FrameStep ) (THIS_ 
            bool bForward
		);

		STDMETHOD ( QuitFrameStepping ) (THIS_ );

		STDMETHOD ( ActivateVarispeed ) (THIS_ 
			double Speed	
		);

		STDMETHOD ( DeactivateVarispeed ) (THIS_ );

		STDMETHOD ( SetL21State ) (THIS_ 
            bool bL21Active
		);

		STDMETHOD ( ShowJacketPicture ) (THIS_ 
            LPCTSTR stJPPath,
			int X,
			int Y,
			int W,
			int H
		);

		STDMETHOD ( ShowBitmap ) (THIS_ 
            LPBYTE pBMP,
			int W,
			int H,
			int X,
			int Y,
			int Format
		);

		STDMETHOD ( SaveNextXFrames ) (THIS_ 
            int Count,
			LPCTSTR stDumpPath
		);

		STDMETHOD ( ForceOutputConnectSize ) (THIS_ 
			int W,
			int H
		);

		STDMETHOD ( ShowUYVYFile ) (THIS_ 
			LPCTSTR stYUVPath, 
			int X, 
			int Y, 
			int W, 
			int H		
		);

		STDMETHOD ( ClearUYVYFile ) (THIS_ );

		STDMETHOD (ShowUYVYBuffer) (THIS_
			LPBYTE pUYVY, 
			int W, 
			int H, 
			int X, 
			int Y			
		);

		STDMETHOD ( ClearUYVYFile_A ) (THIS_ );

		STDMETHOD (Pause) (THIS_ 
			int Pause);

		STDMETHOD ( ActivateFFRW ) (THIS_ );

		STDMETHOD ( DeactivateFFRW ) (THIS_ );

		STDMETHOD ( ResendLastSamp ) (THIS_ 
			int Unconditional);

		STDMETHOD ( SetNULLTimestamps ) (THIS_ );

		STDMETHOD ( ConvertJacketPicture ) (THIS_ 
            LPCTSTR stJPPath,
			int W,
			int H,
			LPBYTE * out_pData
		);

	};

#ifdef __cplusplus
}
#endif

#endif // __IKeystone__

