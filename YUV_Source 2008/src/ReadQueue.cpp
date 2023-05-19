#include "stdafx.h"


//***************************************************************************
// CReadQueue - a helper class for CPushPinBitmap.
//***************************************************************************

#define SAFE_RELEASE(x)		{if(x) x->Release(); x = NULL;}
#define CHECK(x)			{HRESULT hr=(x);if(FAILED(hr))return hr;}

// These are our dwParam arguments for CallWorker() and GetRequest().
enum Requests {
	eWorkerInit,
	eWorkerReadNextFrame,
	eWorkerKillThread,
};

CReadQueue::CReadQueue(void) : CAMThread(),
	m_pwszFileName(NULL),
	m_hrLaunch(S_FALSE),
	m_hFile(NULL),
	m_aQueue(NULL),
	m_iFrame(0),
	m_iQueueToggle(0)
{
}

CReadQueue::~CReadQueue(void)
{
	CloseTheFile();

	::CoTaskMemFree(m_aQueue);
	::CoTaskMemFree(m_pwszFileName);
}

HRESULT CReadQueue::CloseTheFile(void)
{
	if ((m_hFile != NULL) || (m_hFile != HANDLE(-1)))
	{
		::CloseHandle(m_hFile);
		m_hFile = NULL;
	}

	return S_OK;
}

//***************************************************************************
// Public
//***************************************************************************
HRESULT CReadQueue::Launch(LPCWSTR pwszFileName)
{
	// Have we been here before?
	if (m_hrLaunch < S_FALSE)
		return m_hrLaunch;

	// Make sure we don't come back.
	m_hrLaunch = E_FAIL;

	// Save file name.
	CHECK(::SHStrDupW(pwszFileName, &m_pwszFileName));

	// Create the thread.
	if (!Create())
		return E_FAIL;

	// Wait for new thread to execute PriviteInit().
	m_hrLaunch = HRESULT(CallWorker(eWorkerInit));

	if (SUCCEEDED(m_hrLaunch))
	{
		SeekToFrameN(0);
	}

	return m_hrLaunch;
}

//***************************************************************************
// Private helper for PriviteInit().
// Called within the context of the new thread.
//***************************************************************************
HRESULT CReadQueue::OpenTheFile(void)
{
	// Just succeed if we are already open.
	if (m_hFile != NULL)
		return S_OK;

	// Verify we have a file name
	if (NULL == m_pwszFileName)
		return E_FAIL;

	// Open the file
	m_hFile = ::CreateFileW(m_pwszFileName,			// file to open
							GENERIC_READ,			// open for reading
							FILE_SHARE_READ,		// share for reading
							NULL,					// default security
							OPEN_EXISTING,			// existing file only
							FILE_ATTRIBUTE_NORMAL,	// normal file
							NULL);					// no attr. template
	
	// Did the file open?
	if (m_hFile == HANDLE(-1))
		HRESULT_FROM_WIN32(::GetLastError());

	// Get the length
	GetFileSizeEx(m_hFile, &m_FileSize);
	m_iFrameCount = UINT(m_FileSize.QuadPart / FRAME_SIZE);

	return S_OK;
}

//***************************************************************************
// Private helper for PriviteInit().
// Called within the context of the new thread.
//***************************************************************************
HRESULT CReadQueue::AllocTheQueue(void)
{
	// Allocate the queue
	if (m_aQueue != NULL)
		return S_OK;

	m_aQueue = (FRAME_BUF*)::CoTaskMemAlloc(QUEUE_SIZE);
	if (m_aQueue == NULL)
		return E_OUTOFMEMORY;

	::ZeroMemory(m_aQueue, QUEUE_SIZE);
	return S_OK;
}

//***************************************************************************
// Initialize the things that need to get initialized.
// Called within the context of the new thread.
//***************************************************************************
HRESULT CReadQueue::PriviteInit(void)
{
	CHECK(AllocTheQueue());
	CHECK(OpenTheFile());

	return S_OK;
}

//**************************************************************************
// Overridden from CAMThread.
// Called within the context of the new thread.
//**************************************************************************
DWORD CReadQueue::ThreadProc(void)
{
	HRESULT hr = PriviteInit();

	// GetRequest() blocks until CallWorker() is called.
	// Our first request is a dummy (eWorkerInit) for synchronization.
	DWORD dwRequest = GetRequest();
	Reply(hr);

	// If PrivateInit() fails then just fall through and terminate thread. 
	if (SUCCEEDED(hr))
	{
		// This is the main loop for our worker thread.
		while (dwRequest != eWorkerKillThread)
		{
			dwRequest = GetRequest();
			switch (dwRequest)
			{
				case eWorkerReadNextFrame:
					Reply(0);
					ReadNextFrame();
					break;

				case eWorkerKillThread:
					// OnKillThread();
					Reply(0);
					break;

				default:
					break;
			}
		}
	}
	// When this method returns, the thread exits.
	return DWORD(0);		// return value is ignored
}


//***************************************************************************
// Public
//***************************************************************************
HRESULT CReadQueue::SeekToFrameN(UINT iFrame)
{
	// Wait for any outstanding file activity to complete.
	CAutoLock lock(&m_FileLock);

	LARGE_INTEGER TargetFrame;
	TargetFrame.QuadPart = iFrame;

	LARGE_INTEGER FrameSize;
	FrameSize.QuadPart = FRAME_SIZE;

	LARGE_INTEGER li;
	li.QuadPart = TargetFrame.QuadPart * FrameSize.QuadPart;

	li.LowPart = SetFilePointer(m_hFile, 
								li.LowPart, 
								&li.HighPart, 
								FILE_BEGIN);

	if (li.LowPart == INVALID_SET_FILE_POINTER)
	{
		HRESULT hr = HRESULT_FROM_WIN32(::GetLastError());
		return hr;
	}

	return ReadNextFrame();
}

//***************************************************************************
// Private helper for ThreadProc().
// Called within the context of the new thread.
// This version is synchronous because CAMThread does not work well with
// WaitForMultipleObjects() or ReadFileEx().
//***************************************************************************
HRESULT CReadQueue::ReadNextFrame(void)
{
	// Wait for any outstanding file activity to complete.
	CAutoLock lock(&m_FileLock);

	if (NULL == m_aQueue)
		return E_FAIL;

	PVOID pvDest = m_aQueue[(m_iQueueToggle) & 0x01];

	HRESULT hr = S_OK;

	DWORD cb = FRAME_SIZE;
	DWORD dwBytesRead = 0;
	if (!::ReadFile(m_hFile, pvDest, cb, &dwBytesRead, NULL))
		return HRESULT_FROM_WIN32(::GetLastError());

	if (dwBytesRead != cb)
		return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

	m_iFrame++;
	return S_OK;		// just for debugging June 1, 2006
/*
	// Set current frame number
	LARGE_INTEGER CurrentPosition;
	CurrentPosition.QuadPart = 0;
	CurrentPosition.LowPart = SetFilePointer (m_hFile, 
												CurrentPosition.LowPart, 
												&CurrentPosition.HighPart, 
												FILE_CURRENT);
	LARGE_INTEGER asdf;
	asdf.QuadPart = CurrentPosition.QuadPart / FRAME_SIZE;
	m_iFrame = asdf.LowPart;
	//DbgLog((LOG_TRACE, 0, TEXT("YUV SOURCE: Frame: %d"), m_iFrame));
	//m_iFrame++;

	// Support rate control
	if (m_pPlayRate < -32) goto SkipRateChange;
	if (m_pPlayRate > 1 || m_pPlayRate < -1)
	{
		CurrentPosition.QuadPart += ((m_pPlayRate - 1) * FRAME_SIZE);  //i think this is correct

		CurrentPosition.LowPart = SetFilePointer (m_hFile, 
									CurrentPosition.LowPart, 
									&CurrentPosition.HighPart, 
									FILE_BEGIN);

		if (CurrentPosition.LowPart == INVALID_SET_FILE_POINTER)
		{
			HRESULT hr = HRESULT_FROM_WIN32(::GetLastError());
			return hr;
		}
	}
SkipRateChange:

	return hr;
*/
}

//***************************************************************************
// Public
// Called by external thread.
// Returns NULL if we have reached end of stream.
//***************************************************************************
PBYTE CReadQueue::LockFrameBuffer()
{
	// Wait for any outstanding file activity to complete.
	CAutoLock lock(&m_FileLock);
	//CallWorker(eWorkerReadNextFrame);
	return PBYTE(m_aQueue[(m_iQueueToggle++) & 0x01]);
}

//***************************************************************************
// Public
// Called by external thread.
//***************************************************************************
void CReadQueue::UnlockFrameBuffer()
{
	CallWorker(eWorkerReadNextFrame);
}



