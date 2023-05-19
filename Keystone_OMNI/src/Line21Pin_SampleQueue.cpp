#include "appincludes.h"
#include "CKeystone.h"

// Pushes a sample onto the Q - input is a populated IMediaSample structure
// returns S_OK or E_FAIL
HRESULT CKeystone::L21_AddBufferToLine21Queue(LPBYTE pL21Buffer)
{
	// So we have all of our data in the structure - let's get the mutex.
	HRESULT hr = L21_GetLine21QueueMutex( L21_SampleQueue_AccessMutex, 2000 );
	if( FAILED( hr ) )
	{
		// TODO: output failure message here
		//delete pstrucCompleteFrame;
		return E_FAIL;
	}
	L21_SampleQueue.push(pL21Buffer);
	ReleaseMutex( L21_SampleQueue_AccessMutex );
	return hr;
}

// Pops a sample from the queue - input is a pointer to an EMPTY IMediaSample
// returns S_OK or E_FAIL
LPBYTE CKeystone::L21_PopSampleFromLine21Queue()
{
	// regardless of the queue depth, we still want this sample.  Let's get it.
	HRESULT hr = L21_GetLine21QueueMutex( L21_SampleQueue_AccessMutex, 2000 );
	if( FAILED( hr ) )
	{
		// TODO: output failure message here
		//return E_FAIL;
		return NULL;
	}
	LPBYTE CurrentFrame = L21_SampleQueue.front();
	L21_SampleQueue.pop();

	ReleaseMutex( L21_SampleQueue_AccessMutex );

	return CurrentFrame;
}

// Gets the mutex.  
bool CKeystone::L21_GetLine21QueueMutex( HANDLE hMutex, DWORD dwMaxWaitMsec )
{
	// Request ownership of mutex.
	DWORD dwWaitResult = WaitForSingleObject( hMutex, dwMaxWaitMsec );
	bool bGotMutex = false;
	bool bDebugMessage = false;
	switch ( dwWaitResult ) 
	{
		// The thread got Mutex ownership.
		case WAIT_OBJECT_0: 
			bGotMutex = true;
			break; 

		// Failed to get Mutex ownership after time-out expired.
		case WAIT_TIMEOUT: 
			bGotMutex = false; 
			break;

		// Got ownership of the abandoned Mutex object.
		case WAIT_ABANDONED: 
			bGotMutex = true;
			break; 

		default:
			bGotMutex = false; 
			break;
	}

	return bGotMutex;
}
