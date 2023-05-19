#include "../Utility/appincludes.h"
#include "resource.h"
//#include "Keystone_prop.h"

//
// GetPages
//
// This is the sole member of ISpecifyPropertyPages
// Returns the clsid's of the property pages we support
//
STDMETHODIMP CKeystone::GetPages(CAUUID *pPages)
{
    CheckPointer(pPages, E_POINTER);

	pPages->cElems = 1;
    pPages->pElems = (GUID*)CoTaskMemAlloc(sizeof(GUID)*pPages->cElems);

	if(pPages->pElems == NULL) return E_OUTOFMEMORY;

	int i = 0;
    pPages->pElems[i++] = CLSID_AdapterPropertyPage;

    return NOERROR;
}


//
// CreateInstance
//
// This goes in the factory template table to create new filter instances
//
CUnknown * WINAPI CAdapterProperties::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
    ASSERT(phr);

    CUnknown *punk = new CAdapterProperties(lpunk, phr);

    if(punk == NULL)
    {
        if (phr)
            *phr = E_OUTOFMEMORY;
    }

    return punk;

} // CreateInstance


//
// Constructor
//
CAdapterProperties::CAdapterProperties(LPUNKNOWN pUnk, HRESULT *phr) :
    CBasePropertyPage(NAME("Adapter Property Page"), pUnk,
                      IDD_AdapterPROP, IDS_TITLE),
    m_pAdapter(NULL)
{
    InitCommonControls();
} // (Constructor)


// SetDirty
//
// Sets m_bDirty and notifies the property page site of the change
//
HRESULT CAdapterProperties::SetDirty()
{
    m_bDirty = TRUE;

    if(m_pPageSite)
    {
        m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
    }
	return S_OK;
} // SetDirty


//
// OnReceiveMessage
//
// Virtual method called by base class with Window messages
//
INT_PTR CAdapterProperties::OnReceiveMessage(HWND hwnd,
                                           UINT uMsg,
                                           WPARAM wParam,
                                           LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
			m_hwnd = hwnd;
			SetTimer(hwnd,				// handle to main window 
				IDT_TIMER1,				// timer identifier 
				1000,					// 1-second interval 
				(TIMERPROC) NULL);		// no timer callback 
			return (LRESULT) 1;
        }

        case WM_VSCROLL:
        {
            //return (LRESULT) 1;
        }

        case WM_COMMAND:
        {
            if(LOWORD(wParam) == IDC_BTN_GET_DATA)
            {
				//The GetData button was pushed.
				SetEditFieldData();
            }
			if(LOWORD(wParam) == IDC_FEEDBACK_CLICKS)
			{
				SetDirty();
			}
            if(LOWORD(wParam) == IDC_BTN_SCRAP)
            {
				//LPBYTE Samp = NULL;
				//long Size = 0;
				//long W = 0;
				//long H = 0;
				//m_pAdapter->GrabSample(3, &Samp, &Size, &W, &H);
				//DbgLog((LOG_TRACE, 0, TEXT("Sample Size: %d"), Size));
			}
			return (LRESULT) 1;
		}

        case WM_DESTROY:
        {
			KillTimer(hwnd, IDT_TIMER1);
            return (LRESULT) 1;
        }

		case WM_TIMER: 
			switch (wParam) 
			{ 
				case IDT_TIMER1: 
					// process the 1-second timer 
					SetEditFieldData();
					return (LRESULT) 1;
			} 
	}

    return CBasePropertyPage::OnReceiveMessage(hwnd,uMsg,wParam,lParam);
} // OnReceiveMessage


//
// OnConnect
//
// Called when the property page connects to a filter
//
HRESULT CAdapterProperties::OnConnect(IUnknown *pUnknown)
{
    ASSERT(m_pAdapter == NULL);
    CheckPointer(pUnknown,E_POINTER);

    HRESULT hr = pUnknown->QueryInterface(IID_IKeystone, (void **) &m_pAdapter);
    if(FAILED(hr))
    {
        return E_NOINTERFACE;
    }

    ASSERT(m_pAdapter);

    hr = pUnknown->QueryInterface(IID_IKeystoneQuality, (void **) &m_pKeyQual);
    if(FAILED(hr))
    {
        return E_NOINTERFACE;
    }

    ASSERT(m_pAdapter);
	

	//TODO: 1) Get a timer working here.
	//Start the timer
	//SetTimer(ID_CLOCK_TIMER, 1000, NULL);

    return NOERROR;

} // OnConnect


//
// OnDisconnect
//
// Called when we're disconnected from a filter
//
HRESULT CAdapterProperties::OnDisconnect()
{
    // Release of Interface after setting the appropriate Adapter value
    if (!m_pAdapter)
        return E_UNEXPECTED;

    m_pAdapter->Release();
    m_pAdapter = NULL;

	//Kill the timer
	//KillTimer(ID_COUNT_TIMER);

    return NOERROR;

} // OnDisconnect


//
// OnDeactivate
//
// We are being deactivated
//
HRESULT CAdapterProperties::OnDeactivate(void)
{
	GetControlValues();
    m_bIsInitialized = false;

    return NOERROR;
} // OnDeactivate


//
// OnApplyChanges
//
// Changes made should be kept.
//
HRESULT CAdapterProperties::OnApplyChanges()
{
	GetControlValues();
    return(NOERROR);
} // OnApplyChanges


HRESULT CAdapterProperties::GetControlValues()
{
	//Get DoClicks value and set it in the filter.
     if (IsDlgButtonChecked(m_Dlg, IDC_FEEDBACK_CLICKS)) 
    {
		m_pAdapter->put_FeedbackClicks(true);
	}
	else
	{
		m_pAdapter->put_FeedbackClicks(false);
	}

	return S_OK;
}

//
// OnActivate
//
// We are being activated
//
HRESULT CAdapterProperties::OnActivate()
{
	SetEditFieldData();
    m_bIsInitialized = TRUE;
	bStartupValuesSet = false;
    return NOERROR;
} // OnActivate

//
// SetEditFieldData
//
// Initialize the property page fields
//
HRESULT CAdapterProperties::SetEditFieldData()
{
	m_pKeyQual->get_TargetFR_Out(&fTargetFR);
	m_pKeyQual->get_TargetFR_Out_ATPF(&llTargetFR_ATPF);
	m_pKeyQual->get_ActualFR_Out(&fActualFR);
	m_pKeyQual->get_ActualFR_Out_ATPF(&llActualFR_ATPF);
	m_pAdapter->get_InputWidth(&lInputDim_W);
	m_pAdapter->get_InputHeight(&lInputDim_H);
	m_pAdapter->get_OutputWidth(&lOutputDim_W);
	m_pAdapter->get_OutputHeight(&lOutputDim_H);

	LONGLONG llTempActualFR_ATPF = llActualFR_ATPF;
	if (llLastActualOutputATPF == llActualFR_ATPF)
	{
		llActualFR_ATPF = 0;
		fActualFR = 0;
	}

    TCHAR buffer[50];

	wsprintf(buffer,TEXT("%d\0"), llTargetFR_ATPF);
    SendDlgItemMessage(m_Dlg, IDC_TARGET_FR_ATPF, WM_SETTEXT, 0, (LPARAM) buffer);

	sprintf(buffer, "%.3f", fTargetFR); 
	SendDlgItemMessage(m_Dlg, IDC_TARGET_FR,   WM_SETTEXT, 0, (LPARAM) buffer);

    sprintf(buffer,"%.3f", fActualFR);
    SendDlgItemMessage(m_Dlg, IDC_ACTUAL_FR,  WM_SETTEXT, 0, (LPARAM) buffer);

    wsprintf(buffer,TEXT("%d\0"), llActualFR_ATPF);
    SendDlgItemMessage(m_Dlg, IDC_ACTUAL_FR_ATPF,  WM_SETTEXT, 0, (LPARAM) buffer);

	wsprintf(buffer,TEXT("%d\0"), lInputDim_W);
    SendDlgItemMessage(m_Dlg, IDS_Width,  WM_SETTEXT, 0, (LPARAM) buffer);

	wsprintf(buffer,TEXT("%d\0"), lInputDim_H);
    SendDlgItemMessage(m_Dlg, IDS_Height,  WM_SETTEXT, 0, (LPARAM) buffer);

	wsprintf(buffer,TEXT("%d\0"), lOutputDim_W);
    SendDlgItemMessage(m_Dlg, IDS_OutWidth,  WM_SETTEXT, 0, (LPARAM) buffer);

	wsprintf(buffer,TEXT("%d\0"), lOutputDim_H);
    SendDlgItemMessage(m_Dlg, IDS_OutHeight,  WM_SETTEXT, 0, (LPARAM) buffer);

	if (bStartupValuesSet == false)
	{
		bool bClicks = 0;
		m_pAdapter->get_FeedbackClicks(&bClicks);
		if (bClicks == true)
		{
			CheckDlgButton(m_Dlg, IDC_FEEDBACK_CLICKS, BST_CHECKED); 
		}
		else
		{
			CheckDlgButton(m_Dlg, IDC_FEEDBACK_CLICKS, BST_UNCHECKED); 
		}
		bStartupValuesSet = true;
	}

	//Do this so that FR will report zero if at a still menu
	llLastActualOutputATPF = llTempActualFR_ATPF;
	return S_OK;

	//Another way to set the values of controls on proppage:
	//_stprintf(sz, TEXT("486"));
	//Static_SetText(GetDlgItem(m_Dlg, IDS_OutHeight), sz);

} // SetEditFieldData


//========================================================================================
//	TIMER RELATED
//========================================================================================

HRESULT CAdapterProperties::OnTimerTick()
{
	SetEditFieldData();
	return S_OK;
}

//========================================================================================
//	END TIMER RELATED
//========================================================================================
