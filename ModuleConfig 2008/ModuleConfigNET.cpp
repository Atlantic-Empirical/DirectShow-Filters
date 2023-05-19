/*************************************************************************
	created:	2006/3/23
	file name:	ModuleConfigNET.cpp
*************************************************************************

    Copyright (c) 2005-2006 Elecard Ltd.
    All rights are reserved.  Reproduction in whole or in part is prohibited
    without the written consent of the copyright owner.

    Elecard Ltd. reserves the right to make changes without
    notice at any time. Elecard Ltd. makes no warranty, expressed,
    implied or statutory, including but not limited to any implied
    warranty of merchantability of fitness for any particular purpose,
    or that the use will not infringe any third party patent, copyright
    or trademark.

    Elecard Ltd. must not be liable for any loss or damage arising
    from its use.

*************************************************************************

	author:		Alexander Ivanov
	
	purpose:	ModuleConfig and pending classes implementation

*************************************************************************/

#include "stdafx.h"
#include "unknwn.h"
#include "ModuleConfigNET.h"

//////////////////////////////////////////////////////////////////////////
DEFINE_GUID(GUID_NULL,
0x00000000L, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
//////////////////////////////////////////////////////////////////////////


using namespace System::Runtime::InteropServices;
using namespace System::IO;


namespace Elecard
{
	namespace ModuleConfigInterface
	{
		//////////////////////////////////////////////////////////////////////////
		//
		//	 ModuleConfigAdapter  methods implementation
		//
		//////////////////////////////////////////////////////////////////////////

		ModuleConfig* ModuleConfigAdapter::GetConfigInterface(IntPtr filter)
		{
			HRESULT hr = S_OK;
			IModuleConfig* pModuleConfig = NULL;
			IUnknown* pUnk = (IUnknown*)(void*)filter;
			hr = pUnk->QueryInterface(IID_IModuleConfig, (void**)&pModuleConfig);

			if (FAILED(hr) || !pModuleConfig) {
				return NULL;
			}
			InternalModuleConfig* internalModuleConfig = new InternalModuleConfig(pModuleConfig);
			return static_cast<ModuleConfig*>(internalModuleConfig);
		}

		//////////////////////////////////////////////////////////////////////////
		//
		//	 InternalParamConfig  methods implementation
		//
		//////////////////////////////////////////////////////////////////////////

		Boolean InternalParamConfig::SetValue(Object *value,Boolean commit)
		{
			VARIANT variant = {0};
			Marshal::GetNativeVariantForObject(value,&variant);

			return SUCCEEDED(m_pParamConfig->SetValue(&variant,commit));
		}

		Object* InternalParamConfig::GetValue(Boolean commited)
		{
			VARIANT variant = {0};
			if (FAILED(m_pParamConfig->GetValue(&variant,commited))) {
				return NULL;
			}

			return Marshal::GetObjectForNativeVariant(&variant);
		}

		Boolean InternalParamConfig::SetVisible(Boolean visible)
		{
			return SUCCEEDED(m_pParamConfig->SetVisible(visible));
		}

		Boolean InternalParamConfig::IsVisible()
		{
			BOOL visible = FALSE;
			return (SUCCEEDED(m_pParamConfig->GetVisible(&visible)) && visible);
		}

		Guid* InternalParamConfig::GetParamID()
		{
			HRESULT hr = S_OK;
			GUID propID = GUID_NULL;
			if (FAILED(m_pParamConfig->GetParamID(&propID))) {
				return NULL;
			}

			return (__box(Guid(propID.Data1,propID.Data2,propID.Data3, 
				propID.Data4[0],propID.Data4[1],propID.Data4[2],propID.Data4[3],
				propID.Data4[4],propID.Data4[5],propID.Data4[6],propID.Data4[7])));

		}

		String* InternalParamConfig::GetName()
		{
			BSTR bstr = NULL;
			if (FAILED(m_pParamConfig->GetName(&bstr)) || !bstr) {
				return NULL;
			}

			String* retStr = new String(bstr);
			SysFreeString(bstr);

			return retStr;
		}

		Boolean InternalParamConfig::IsReadOnly()
		{
			BOOL readOnly = FALSE;
			return (SUCCEEDED(m_pParamConfig->GetReadOnly(&readOnly)) && readOnly);

		}

		Object* InternalParamConfig::GetDefValue()
		{
			HRESULT hr = S_OK;
			VARIANT variant = {0};

			hr = m_pParamConfig->GetDefValue(&variant);
			if (FAILED(hr)) {
				return NULL;
			}

			return Marshal::GetObjectForNativeVariant(&variant);

		}

		Boolean InternalParamConfig::GetValidRange(Object* minValue,Object* maxValue,Object* delta)
		{
			return FALSE;
		}

		Boolean InternalParamConfig::EnumValidValues(Int32 *pNumValidValues,Object* pValidValues,String* pValueNames)
		{
			return FALSE;
		}

		String* InternalParamConfig::ValueToMeaning(Object* value)
		{
			VARIANT variant = {0};
			BSTR bstr = NULL;

			Marshal::GetNativeVariantForObject(value,&variant);

			if (FAILED(m_pParamConfig->ValueToMeaning(&variant,&bstr))) {
				return NULL;
			}

			String* retStr = new String(bstr);
			SysFreeString(bstr);

			return retStr;
		}

		Object* InternalParamConfig::MeaningToValue(String* meaning)
		{

			HRESULT hr = S_OK;
			VARIANT variant = {0};

			LPWSTR pwsMeaning = (LPWSTR)Marshal::StringToCoTaskMemUni(meaning).ToPointer();
			BSTR bstr = SysAllocString(pwsMeaning);

			hr = m_pParamConfig->MeaningToValue(bstr,&variant);

			SysFreeString(bstr);
			Marshal::FreeCoTaskMem(pwsMeaning);

			if (FAILED(hr)) {
				return NULL;
			}

			return Marshal::GetObjectForNativeVariant(&variant);

		}

		//////////////////////////////////////////////////////////////////////////
		//
		//	 InternalModuleConfig  methods implementation
		//
		//////////////////////////////////////////////////////////////////////////

		InternalModuleConfig::InternalModuleConfig(IModuleConfig* moduleConfig):
		bDone(false),
		bDisposed(false)
		{
			m_hCallbackEvent = CreateEvent(NULL,FALSE,FALSE,TEXT("ModuleConfig Event Callback"));

			m_pModuleCallback = new ModuleCallback(moduleConfig, m_hCallbackEvent);
			m_pModuleCallback->RegisterForNotifies();
			if (moduleConfig) {
				moduleConfig->AddRef();
				SAFE_RELEASE(m_pModuleConfig);
				m_pModuleConfig = moduleConfig;
			}

			//workerThread = new Thread(new ThreadStart(this,&InternalModuleConfig::CheckForEvent));
			//workerThread->Start();
		}

		InternalModuleConfig::~InternalModuleConfig()
		{
			Dispose();
		}

		void InternalModuleConfig::Dispose()
		{
			if (!bDisposed) {
				bDone = true;

				m_pModuleCallback->UnRegisterFromNotifies();
				//		workerThread->Join();

				if (m_pModuleCallback) {
					//IModuleCallback* pModuleCallback = m_pModuleCallback;
					//m_pModuleCallback = NULL;
					//delete pModuleCallback;

					delete m_pModuleCallback;
					m_pModuleCallback = NULL;
				}

				SAFE_RELEASE(m_pModuleConfig);

				CloseHandle(m_hCallbackEvent);

				bDisposed = true;
			}
		}

		Boolean InternalModuleConfig::SetParamValue(Guid* paramID, Object* value)
		{
			VARIANT variant = {0};
			Marshal::GetNativeVariantForObject(value,&variant);
			LPWSTR lpwstrGuid = (LPWSTR)Marshal::StringToCoTaskMemUni(paramID->ToString(S"B")).ToPointer();
			GUID guid = GUID_NULL;
			CLSIDFromString(lpwstrGuid,&guid);
			Marshal::FreeCoTaskMem(lpwstrGuid);
			return SUCCEEDED(m_pModuleConfig->SetValue(&guid,&variant));
		}

		Object* InternalModuleConfig::GetParamValue(Guid* paramID)
		{
			LPWSTR lpwstrGuid = (LPWSTR)Marshal::StringToCoTaskMemUni(paramID->ToString(S"B")).ToPointer();
			GUID guid = GUID_NULL;
			CLSIDFromString(lpwstrGuid,&guid);
			Marshal::FreeCoTaskMem(lpwstrGuid);
			HRESULT hr = S_OK;
			VARIANT variant = {0};
			if (SUCCEEDED(hr = m_pModuleConfig->GetValue(&guid, &variant))) {
				Object* object;
				switch(variant.vt) {
					case VT_UI8:
						variant.vt = VT_I8;
						break;
					case VT_UI4:
						variant.vt = VT_I4;
						break;
					case VT_UI2:
						variant.vt = VT_I2;
						break;
					case VT_UI1:
						variant.vt = VT_I1;
						break;
				}
				object = Marshal::GetObjectForNativeVariant(&variant);
				VariantClear(&variant);

				return object;
			}
			return NULL;
		}

		ParamConfig* InternalModuleConfig::GetParamConfig(Guid* paramID)
		{
			LPWSTR lpwstrGuid = (LPWSTR)Marshal::StringToCoTaskMemUni(paramID->ToString(S"B")).ToPointer();
			GUID guid = GUID_NULL;
			CLSIDFromString(lpwstrGuid,&guid);
			Marshal::FreeCoTaskMem(lpwstrGuid);
			IParamConfig* pParamConfig = NULL;

			if (SUCCEEDED(m_pModuleConfig->GetParamConfig(&guid,&pParamConfig))) {
				ParamConfig* paramConfig = new InternalParamConfig(pParamConfig);
				return paramConfig;
			}

			return NULL;
		}

		Boolean InternalModuleConfig::IsParamSupported(Guid* paramID)
		{
			LPWSTR lpwstrGuid = (LPWSTR)Marshal::StringToCoTaskMemUni(paramID->ToString(S"B")).ToPointer();
			GUID guid = GUID_NULL;
			CLSIDFromString(lpwstrGuid,&guid);
			Marshal::FreeCoTaskMem(lpwstrGuid);
			return SUCCEEDED(m_pModuleConfig->IsSupported(&guid));
		}

		Boolean InternalModuleConfig::SetDefState()
		{
			return SUCCEEDED(m_pModuleConfig->SetDefState());
		}

		ArrayList* InternalModuleConfig::EnumParams()
		{
			ArrayList* guidList = new ArrayList();
			GUID* pParamIDs;
			long lParams = 0;
			if (SUCCEEDED(m_pModuleConfig->EnumParams(&lParams,NULL)) && lParams) {
				pParamIDs = new GUID[lParams];
			};

			if (!pParamIDs) {
				return NULL;
			}

			if (SUCCEEDED(m_pModuleConfig->EnumParams(&lParams,pParamIDs))) {
				for (int i = 0; i < lParams; i++) {

					guidList->Add(__box(Guid(pParamIDs[i].Data1,pParamIDs[i].Data2,pParamIDs[i].Data3, 
						pParamIDs[i].Data4[0],pParamIDs[i].Data4[1],pParamIDs[i].Data4[2],pParamIDs[i].Data4[3],
						pParamIDs[i].Data4[4],pParamIDs[i].Data4[5],pParamIDs[i].Data4[6],pParamIDs[i].Data4[7])));
				}
			}

			if (pParamIDs) {
				delete [] pParamIDs;
			}

			return guidList;
		}

		Object* InternalModuleConfig::CommitChanges()
		{
			VARIANT variant = {0};
			HRESULT hr = m_pModuleConfig->CommitChanges(&variant);

			if (FAILED(hr)) {
				VariantClear(&variant);
				return Marshal::GetObjectForNativeVariant(&variant);
			}
			return NULL;
		}

		Boolean InternalModuleConfig::DeclineChanges()
		{
			return SUCCEEDED(m_pModuleConfig->DeclineChanges());
		}

		Boolean InternalModuleConfig::SaveToRegistry(RegistryKey* keyRoot, String* keyName, Boolean preferReadable)
		{
			HRESULT hr = S_OK;
			LPWSTR pwsKeyName = (LPWSTR)Marshal::StringToCoTaskMemUni(keyName).ToPointer();
			BSTR bstr = SysAllocString(pwsKeyName);
			HKEY hKeyRoot = 0;

			if (keyRoot == Registry::ClassesRoot) {
				hKeyRoot = HKEY_CLASSES_ROOT;
			} else if(keyRoot == Registry::CurrentUser) {
				hKeyRoot = HKEY_CURRENT_USER; 
			} else if(keyRoot == Registry::LocalMachine) {
				hKeyRoot = HKEY_LOCAL_MACHINE; 
			} else if(keyRoot == Registry::Users) {
				hKeyRoot = HKEY_USERS; 
			} else {
				return FALSE;
			}

			hr = m_pModuleConfig->SaveToRegistry(reinterpret_cast<DWORD>(hKeyRoot), bstr, preferReadable);
			SysFreeString(bstr);
			Marshal::FreeCoTaskMem(pwsKeyName);
			return SUCCEEDED(hr);
		}

		Boolean InternalModuleConfig::LoadFromRegistry(RegistryKey* keyRoot, String* keyName, Boolean preferReadable)
		{
			HRESULT hr = S_OK;
			LPWSTR pwsKeyName = (LPWSTR)Marshal::StringToCoTaskMemUni(keyName).ToPointer();
			BSTR bstr = SysAllocString(pwsKeyName);
			HKEY hKeyRoot = 0;

			if (keyRoot == Registry::ClassesRoot) {
				hKeyRoot = HKEY_CLASSES_ROOT;
			} else if(keyRoot == Registry::CurrentUser) {
				hKeyRoot = HKEY_CURRENT_USER; 
			} else if(keyRoot == Registry::LocalMachine) {
				hKeyRoot = HKEY_LOCAL_MACHINE; 
			} else if(keyRoot == Registry::Users) {
				hKeyRoot = HKEY_USERS; 
			} else {
				return FALSE;
			}

			hr = m_pModuleConfig->LoadFromRegistry(reinterpret_cast<DWORD>(hKeyRoot), bstr, preferReadable);
			SysFreeString(bstr);
			Marshal::FreeCoTaskMem(pwsKeyName);
			return SUCCEEDED(hr);
		}

		void InternalModuleConfig::CheckForEvent()
		{
			while ((!bDone) && (m_pModuleCallback))
			{
				if (WaitForSingleObject(m_hCallbackEvent, 100) == WAIT_TIMEOUT) {
					continue;
				}

				m_pModuleCallback->EnterCriticalSection();
				ArrayList* guidList = new ArrayList();
				for(int i = 0; i < (int)m_pModuleCallback->cGuidArray; i++)
				{
					guidList->Add(__box((Guid(m_pModuleCallback->pGuidArray[i].Data1,m_pModuleCallback->pGuidArray[i].Data2,
						m_pModuleCallback->pGuidArray[i].Data3,	m_pModuleCallback->pGuidArray[i].Data4[0],
						m_pModuleCallback->pGuidArray[i].Data4[1],m_pModuleCallback->pGuidArray[i].Data4[2],
						m_pModuleCallback->pGuidArray[i].Data4[3],m_pModuleCallback->pGuidArray[i].Data4[4],
						m_pModuleCallback->pGuidArray[i].Data4[5],m_pModuleCallback->pGuidArray[i].Data4[6],
						m_pModuleCallback->pGuidArray[i].Data4[7]))));
				}

				//ModuleNotify(guidList);		
				m_pModuleCallback->LeaveCriticalSection();	

			}
		}

		ModuleCallback::ModuleCallback(IModuleConfig* pModuleConfig, HANDLE hEvent):
		pGuidArray(NULL),
		cGuidArray(0),
		m_pModuleConfig(pModuleConfig),
		m_hEvent(hEvent)
		{
			InitializeCriticalSection(&m_CriticalSection);
		}

		ModuleCallback::~ModuleCallback()
		{
			EnterCriticalSection();

			cGuidArray = 0;
			if (pGuidArray) {
				delete[] pGuidArray;
				pGuidArray = NULL;
			}

			LeaveCriticalSection();

			DeleteCriticalSection(&m_CriticalSection);
		}

		void ModuleCallback::EnterCriticalSection()
		{
			::EnterCriticalSection(&m_CriticalSection);
		}

		void ModuleCallback::LeaveCriticalSection()
		{
			::LeaveCriticalSection(&m_CriticalSection);
		}

		void ModuleCallback::RegisterForNotifies()
		{
			if (m_pModuleConfig) {
				m_pModuleConfig->RegisterForNotifies(this);
			}
		}

		void ModuleCallback::UnRegisterFromNotifies()
		{
			if (m_pModuleConfig) {
				m_pModuleConfig->UnregisterFromNotifies(this);
			}

			EnterCriticalSection();

			cGuidArray = 0;
			if (pGuidArray) {
				delete[] pGuidArray;
				pGuidArray = NULL;
			}

			SetEvent(m_hEvent);
			LeaveCriticalSection();
		}

		HRESULT ModuleCallback::OnModuleNotify(const long cParams, GUID* pParamIDs)
		{
			if (cParams > 0)
			{
				EnterCriticalSection();

				if (pGuidArray) {
					delete[] pGuidArray;
					pGuidArray = NULL;
				}

				pGuidArray = __nogc new GUID[cParams];
				cGuidArray = cParams;
				memcpy( pGuidArray, pParamIDs, sizeof(GUID)*cParams);
				SetEvent(m_hEvent);
				LeaveCriticalSection();		

			}
			return S_OK;
		}
	} // namespace ModuleConfigInterface
} // namespace ModuleConfig 