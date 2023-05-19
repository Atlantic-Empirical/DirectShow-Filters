#include "appincludes.h"
#include "Mixer.h"

HRESULT CKeystone::GIDE_MixGuides(LPBYTE dst, int w, int h, bool bUYVY)
{
	HRESULT hr = S_OK;
	if (USEROPTION_MixGuides == true)
	{
		if (bUYVY == true)
		{
			hr = CMixer::MixUYVY_Guides(dst, w, h, GIDE_Guide_L, GIDE_Guide_T, GIDE_Guide_R, GIDE_Guide_B, GIDE_GuideColor);		
		}
		else
		{
			hr = CMixer::MixYUY2_Guides(dst, w, h, GIDE_Guide_L, GIDE_Guide_T, GIDE_Guide_R, GIDE_Guide_B, GIDE_GuideColor);		
		}
		if (FAILED(hr)) return hr;
	}

	if (USEROPTION_ShowActionTitleGuides == 1)
	{
		if (bUYVY == true)
		{
			hr = CMixer::MixUYVY_ActionTitleGuides(dst, w, h, GIDE_ActionTitleSafeColor);		
		}
		else
		{
			hr = CMixer::MixYUY2_ActionTitleGuides(dst, w, h, GIDE_ActionTitleSafeColor);				
		}
		if (FAILED(hr)) return hr;
	}
	return hr;
}

