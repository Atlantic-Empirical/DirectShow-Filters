#include "../Utility/appincludes.h"

template <class T>
static CUnknown* WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT* phr)
{
	//new
	*phr = S_OK;
    CUnknown* punk = new T(NAME("Javelin JacketPictureConverter"), lpunk, CLSID_JacketPictureConverter);
    if(punk == NULL) *phr = E_OUTOFMEMORY;
	return punk;
	

	//old
	//*phr = S_OK;
 //   CUnknown* punk = new T(lpunk, phr);
 //   if(punk == NULL) *phr = E_OUTOFMEMORY;
	//return punk;
}

STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2(TRUE);

} // DllRegisterServer

STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2(FALSE);

} // DllUnregisterServer


static HINSTANCE g_Instance = NULL;

HINSTANCE GetModuleInstance()
{
	return g_Instance;
}

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);
BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  dwReason, 
                      LPVOID lpReserved)
{
	g_Instance = (HINSTANCE)hModule;
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}

const AMOVIESETUP_MEDIATYPE sudInPinTypes[] =
{
	{&MEDIATYPE_Video, &GUID_NULL}
};

const AMOVIESETUP_MEDIATYPE sudOutPinTypes[] =
{
	{&MEDIATYPE_Video, &GUID_NULL}
};

const AMOVIESETUP_PIN sudPins[] =
{
	{L"Input", FALSE, FALSE, FALSE, FALSE, &CLSID_NULL, L"Output", 1, sudInPinTypes},
	/*{L"Input", FALSE, FALSE, FALSE, FALSE, &CLSID_NULL, L"Output", 1, sudInPinTypes},
	{L"Input", FALSE, FALSE, FALSE, FALSE, &CLSID_NULL, L"Output", 1, sudInPinTypes},*/
	{L"Output", FALSE, TRUE, FALSE, FALSE, &CLSID_NULL, L"Input", 1, sudOutPinTypes}
};

const AMOVIESETUP_FILTER sudAdapter[] =
{
	{&CLSID_JacketPictureConverter, L"Javelin JacketPictureConverter", MERIT_DO_NOT_USE, 2, sudPins}
};

CFactoryTemplate g_Templates[1] = 
{
	{
		sudAdapter[0].strName, 
		sudAdapter[0].clsID, 
		CreateInstance<CKeystone>, 
		NULL, 
		&sudAdapter[0]
	},
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);
