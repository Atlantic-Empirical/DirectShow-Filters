class CAdapterProperties : public CBasePropertyPage
{

public:

    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

private:

    INT_PTR OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
    HRESULT OnConnect(IUnknown *pUnknown);
    HRESULT OnDisconnect();
    HRESULT OnDeactivate();
    HRESULT OnActivate();
    HRESULT OnApplyChanges();
    HRESULT SetDirty();

	HRESULT OnTimerTick(); 
    HRESULT SetEditFieldData();
	HRESULT GetControlValues();

    CAdapterProperties(LPUNKNOWN lpunk, HRESULT *phr);

    IKeystone *m_pAdapter;
	IKeystoneQuality * m_pKeyQual;

    IKeystone *pIKeystone() {
        ASSERT(m_pAdapter);
        return m_pAdapter;
    };

	LONGLONG llTargetFR_ATPF;
	float fTargetFR;
	LONGLONG llActualFR_ATPF;
	float fActualFR;
	long lInputDim_W;
	long lInputDim_H;
	long lOutputDim_W;
	long lOutputDim_H;
    bool m_bIsInitialized;      // Used to ignore startup messages
	HWND m_hwnd;
	bool bStartupValuesSet;
	LONGLONG llLastActualOutputATPF;


}; // CAdapterProperties

