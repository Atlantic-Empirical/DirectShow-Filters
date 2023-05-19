/*************************************************************************
	created:	2006/3/23
	file name:	ModuleConfigNET.h
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
	
	purpose:	ModuleConfig and pending classes declaration

*************************************************************************/
#pragma once

using namespace System;
using namespace System::Threading;
using namespace System::Collections;
using namespace Microsoft::Win32;

#define SAFE_RELEASE(_p) if (_p != NULL) { _p->Release(); _p = NULL; }

#include "ModuleConfig.h"

namespace Elecard
{
	namespace ModuleConfigInterface
	{
		public __gc interface ParamConfig
		{
			virtual Boolean SetValue(Object *value,Boolean commit) = 0;
			virtual Object* GetValue(Boolean commited) = 0;
			virtual Boolean SetVisible(Boolean visible) = 0;
			virtual Boolean IsVisible() = 0;
			virtual Guid*   GetParamID() = 0;
			virtual String* GetName() = 0;
			virtual Boolean IsReadOnly() = 0;
			virtual Object* GetDefValue() = 0;
			virtual Boolean GetValidRange(Object* minValue,Object* maxValue,Object* delta) = 0;
			virtual Boolean EnumValidValues(Int32 *pNumValidValues,Object* pValidValues,String* pValueNames) = 0;
			virtual String* ValueToMeaning(Object* value) = 0;
			virtual Object* MeaningToValue(String* meaning) = 0;
		};

		public __gc interface ModuleConfig: public IDisposable
		{
			virtual void Dispose() = 0;
			virtual Boolean SetParamValue(Guid* paramID, Object* value) = 0;
			virtual Object* GetParamValue(Guid* paramID) = 0;
			virtual ParamConfig* GetParamConfig(Guid* paramID) = 0;
			virtual Boolean IsParamSupported(Guid* paramID) = 0;
			virtual Boolean SetDefState() = 0;
			virtual ArrayList* EnumParams() = 0;
			virtual Object* CommitChanges() = 0;
			virtual Boolean DeclineChanges() = 0;
			virtual Boolean SaveToRegistry(RegistryKey* keyRoot, String* keyName, Boolean preferReadable) = 0;
			virtual Boolean LoadFromRegistry(RegistryKey* keyRoot, String* keyName, Boolean preferReadable) = 0;

			__delegate void ModuleNotifyEventHandler(ArrayList* changedParameters);
			__event ModuleNotifyEventHandler*	ModuleNotify;
		};

		public __gc class ModuleConfigAdapter
		{
		public:
			static ModuleConfig* GetConfigInterface(IntPtr filter);
		};

		private __gc class InternalParamConfig: public ParamConfig
		{
			IParamConfig* m_pParamConfig;
		public:
			InternalParamConfig(IParamConfig* pParamConfig):
			m_pParamConfig(pParamConfig)
			{};

			Boolean SetValue(Object *value,Boolean commit);
			Object* GetValue(Boolean commited);
			Boolean SetVisible(Boolean visible);
			Boolean IsVisible();
			Guid*   GetParamID();
			String* GetName();
			Boolean IsReadOnly();
			Object* GetDefValue();
			Boolean GetValidRange(Object* minValue,Object* maxValue,Object* delta);
			Boolean EnumValidValues(Int32 *pNumValidValues,Object* pValidValues,String* pValueNames);
			String* ValueToMeaning(Object* value);
			Object* MeaningToValue(String* meaning);
		};

		private __gc class InternalModuleConfig;

		__nogc class ModuleCallback: public IModuleCallback
		{
			HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,void **ppvObject) { return S_OK; }
			ULONG STDMETHODCALLTYPE AddRef() { return 0; }
			ULONG STDMETHODCALLTYPE Release() { return 0; }

			IModuleConfig* m_pModuleConfig;
			HANDLE m_hEvent;
			CRITICAL_SECTION m_CriticalSection;

		public:
			GUID __nogc* pGuidArray;
			UINT cGuidArray;

			ModuleCallback(IModuleConfig* pModuleConfig, HANDLE hEvent);
			virtual ~ModuleCallback();

			void EnterCriticalSection();
			void LeaveCriticalSection();
			void RegisterForNotifies();
			void UnRegisterFromNotifies();


			HRESULT STDMETHODCALLTYPE OnModuleNotify(const long cParams, GUID* pParamIDs);

		};


		private __gc class InternalModuleConfig: public ModuleConfig
		{
			IModuleConfig* m_pModuleConfig;
			ModuleCallback* m_pModuleCallback;
			Thread* workerThread;
			HANDLE m_hCallbackEvent;
			Boolean bDone;
			Boolean bDisposed;

			void CheckForEvent();

		public:
			InternalModuleConfig(IModuleConfig* moduleConfig);
			~InternalModuleConfig();

			void Dispose();
			Boolean SetParamValue(Guid* paramID, Object* value);
			Object* GetParamValue(Guid* paramID);
			ParamConfig* GetParamConfig(Guid* paramID);
			Boolean IsParamSupported(Guid* paramID);
			Boolean SetDefState();
			ArrayList* EnumParams();
			Object* CommitChanges();
			Boolean DeclineChanges();

			Boolean SaveToRegistry(RegistryKey* keyRoot, String* keyName, Boolean preferReadable);
			Boolean LoadFromRegistry(RegistryKey* keyRoot, String* keyName, Boolean preferReadable);
		};
	} // namespace ModuleConfigInterface
} // namespace ModuleConfig