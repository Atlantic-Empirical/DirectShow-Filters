#include "appincludes.h"

ULONG STDMETHODCALLTYPE CKeystone::GetMiscFlags()
{
	return AM_FILTER_MISC_FLAGS_IS_RENDERER;
}

