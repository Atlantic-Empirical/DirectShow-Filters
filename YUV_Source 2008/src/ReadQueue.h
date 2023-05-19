#pragma once

#define	FRAME_SIZE	(3110400)
#define QUEUE_ELEMS	(2)
#define	QUEUE_SIZE	(FRAME_SIZE * QUEUE_ELEMS)
typedef BYTE		FRAME_BUF[FRAME_SIZE];

class CReadQueue :
	public CAMThread
{
public:
				CReadQueue(void);
public:
	virtual		~CReadQueue(void);
public:
	HRESULT		Launch(LPCWSTR pszFileName);
	HRESULT		SeekToFrameN(UINT iFrame);
	PBYTE		LockFrameBuffer(void);
	void		UnlockFrameBuffer(void);

protected:
	virtual DWORD ThreadProc(void);

private:
	// These are called within the context of the new thread.
	HRESULT		PriviteInit(void);
	HRESULT		OpenTheFile(void);
	HRESULT		CloseTheFile(void);
	HRESULT		AllocTheQueue(void);
	HRESULT		CreateTheEvents(void);

	HRESULT		ReadNextFrame(void);

    CCritSec	m_FileLock;
	HRESULT		m_hrLaunch;
	HANDLE		m_hFile;
	FRAME_BUF*	m_aQueue;
	UINT		m_iQueueToggle;

public:
	UINT			m_iFrame;		// Current frame
	LARGE_INTEGER	m_FileSize;		// Size of the file we're playing in bytes
	UINT			m_iFrameCount;	// total frames in file
	LPWSTR			m_pwszFileName; // File path
	INT				m_pPlayRate;	// Playback rate

};
