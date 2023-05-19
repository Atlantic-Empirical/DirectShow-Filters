#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	// {84D28780-77FA-411e-8918-8CBAB8F3BBAC}
	DEFINE_GUID(IID_IKeystoneQuality, 0x84d28780, 0x77fa, 0x411e, 0x89, 0x18, 0x8c, 0xba, 0xb8, 0xf3, 0xbb, 0xac);
 
	DECLARE_INTERFACE_(IKeystoneQuality, IUnknown)
    {
        STDMETHOD(get_TargetFR_Out) (THIS_
            float *TargetFR      
        ) PURE;

        STDMETHOD(get_TargetFR_Out_ATPF) (THIS_
            LONGLONG *TargetFR_ATPF      
        ) PURE;

        STDMETHOD(get_TargetFR_In) (THIS_
            float *TargetFR      
        ) PURE;

        STDMETHOD(get_TargetFR_In_ATPF) (THIS_
            LONGLONG *TargetFR_ATPF      
        ) PURE;

        STDMETHOD(get_ActualFR_Out) (THIS_
            float *ActualFR      
        ) PURE;

        STDMETHOD(get_ActualFR_Out_ATPF) (THIS_
            LONGLONG *ActualFR_ATPF      
        ) PURE;

        STDMETHOD(get_ActualFR_In) (THIS_
            float *ActualFR      
        ) PURE;

        STDMETHOD(get_ActualFR_In_ATPF) (THIS_
            LONGLONG *ActualFR_ATPF      
        ) PURE;

        STDMETHOD(get_Jitter_In) (THIS_
            LONGLONG *InputJitter      
        ) PURE;

        STDMETHOD(get_Jitter_Out) (THIS_
            LONGLONG *OutputJitter      
        ) PURE;

	};

#ifdef __cplusplus
}
#endif
