#include "appincludes.h"
#include "Mixer.h"

HRESULT CKeystone::OSD_MixOnScreenDisplay(LPBYTE dst, int w, int h)
{
	HRESULT hr = S_OK;
	if (USEROPTION_DoOSDMix != true) goto SkipOSDMix;
	if ((int)OSD_ActiveMixData.pImage == 0xcdcdcdcd) goto SkipOSDMix;
	if (OSD_ActiveMixData.pImage)
	{
		if (OSD_Ticker < OSD_TickGoal)
		{
			hr = CMixer::MixUYVY_OSD(OSD_ActiveMixData.pImage, OSD_ActiveMixData.nW, OSD_ActiveMixData.nH, dst, w, h, OSD_ActiveMixData.nX, OSD_ActiveMixData.nY, OSD_ActiveMixData.KeyColor); 
			if (FAILED(hr)) return hr;
			OSD_Ticker += 1;
			//DbgLog((LOG_TRACE, 0, TEXT("Keystone: OSD Ticker. %d"), OSDTicker));
		}
		else
		{
			//DbgLog((LOG_TRACE, 0, TEXT("Keystone: OSD Ticker Maxedout")));
			USEROPTION_DoOSDMix = false;
		}
	}
SkipOSDMix:
	return hr;
}

