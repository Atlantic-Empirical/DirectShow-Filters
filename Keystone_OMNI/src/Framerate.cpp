#include "appincludes.h"

void CKeystone::FRRT_TickInputRate(DWORD dwCurrentTicks)
{
	int i10Diff = abs(short(FRRT_TenSecondInterval_StartTicks_In - dwCurrentTicks));
	if (i10Diff > 10000)
	{
		//Ten secs have gone by
		if (FRRT_FramesPassed_10s_In == 0)
		{
			//FRRT_10sFR_In = 0;
			DbgLog((LOG_TRACE,0,TEXT("Keystone: No frames passed in 10 seconds.")));
			goto Restart10s;
		}
		float fMSPerFrame = (float)i10Diff/FRRT_FramesPassed_10s_In;
		float f10s = (float)1000/fMSPerFrame;

Restart10s:
		FRRT_TenSecondInterval_StartTicks_In = GetTickCount();
		FRRT_FramesPassed_10s_In = 0;
	}
	else
	{
		FRRT_FramesPassed_10s_In++;
	}

	int iTDiff = abs(short(FRRT_OneSecondInterval_StartTicks_In - dwCurrentTicks)); 
	if (iTDiff > 1000)
	{
		FRRT_CalculateInputFramerate(iTDiff, FRRT_FramesPassed_1s_In);
		FRRT_FramesPassed_1s_In = 0;
		FRRT_OneSecondInterval_StartTicks_In = GetTickCount();
	}
	else
	{
		FRRT_FramesPassed_1s_In++;
	}
};

void CKeystone::FRRT_TickOutputRate(DWORD dwCurrentTicks)
{
	//TODO: (IANIER) Calculate ActualFR
	//Please verify that I did this correctly:
	//Main questions: Would this give accurate values? 
	//Does this significantly decrease performace of the filter?
	//The values currently being created seem a little bit low.
	//DWORD dwCurrentTicks = GetTickCount();
	int iTDiff = abs(short(FRRT_OneSecondInterval_StartTicks_Out - dwCurrentTicks)); 
	//if (iTDiff > 2000) goto LeaveCalcActualFR;

	int i10Diff = abs(short(FRRT_TenSecondInterval_StartTicks_Out - dwCurrentTicks));
	if (i10Diff > 10000)
	{
		//Ten secs have gone by
		if (FRRT_FramesPassed_10s_Out == 0)
		{
			//FRRT_10sFR_Out = 0;
			DbgLog((LOG_TRACE,0,TEXT("Keystone: No frames passed in 10 seconds.")));
			goto Restart10s;
		}
		float fMSPerFrame = (float)i10Diff/FRRT_FramesPassed_10s_Out;
		float f10s = (float)1000/fMSPerFrame;
		//if (f10s == 0)
		//{
		//	DbgLog((LOG_TRACE,0,TEXT("Keystone: 10s framerate is zero.")));
		//}
		//else
		//{
		//	DbgLog((LOG_TRACE,0,TEXT("Keystone: 10s output framerate is: %.3f"), f10s));
		//}

Restart10s:
		FRRT_TenSecondInterval_StartTicks_Out = GetTickCount();
		FRRT_FramesPassed_10s_Out = 0;
	}
	else
	{
		FRRT_FramesPassed_10s_Out++;
	}

	if (iTDiff > 1000)
	{
		FRRT_CalcOutputFramerate(iTDiff, FRRT_FramesPassed_1s_Out);
		FRRT_FramesPassed_1s_Out = 0;
		FRRT_OneSecondInterval_StartTicks_Out = GetTickCount();
	}
	else
	{
		FRRT_FramesPassed_1s_Out++;
	}
};


HRESULT CKeystone::FRRT_CalculateInputFramerate(int MSCount, short FrameCount)
{
	if (FrameCount == 0)
	{
		FRRT_ActualFramerate_In = 0;
		return S_OK;
	}
	float fMSPerFrame = (float)MSCount/FrameCount;
	FRRT_ActualFramerate_In = (float)1000/fMSPerFrame;
	if (FRRT_ActualFramerate_In == 0)
	{
		DbgLog((LOG_TRACE,0,TEXT("Keystone: Actual input frame rate is zero.")));
	}
	else
	{
		//DbgLog((LOG_TRACE,0,TEXT("Keystone: Actual input frame rate is: %.3f"), fActualFR_In));
	}
	return S_OK;
}

HRESULT CKeystone::FRRT_CalcOutputFramerate(int MSCount, short FrameCount)
{
	if (FrameCount == 0)
	{
		FRRT_ActualFramerate_Out = 0;
		return S_OK;
	}
	float fMSPerFrame = (float)MSCount/FrameCount;
	FRRT_ActualFramerate_Out = (float)1000/fMSPerFrame;
	if (FRRT_ActualFramerate_Out == 0)
	{
		DbgLog((LOG_TRACE,0,TEXT("Keystone: Actual output frame rate is zero.")));
	}
	else
	{
		//DbgLog((LOG_TRACE,0,TEXT("Keystone: Actual output frame rate is: %.3f"), fActualFR_Out));
	}
	
	return S_OK;
}
