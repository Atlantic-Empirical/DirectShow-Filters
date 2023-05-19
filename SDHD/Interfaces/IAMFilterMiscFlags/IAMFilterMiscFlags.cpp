#include "../../utility/appincludes.h"
//#include <vmr9.h>

ULONG STDMETHODCALLTYPE CKeystone::GetMiscFlags()
{
	return AM_FILTER_MISC_FLAGS_IS_RENDERER;
}

