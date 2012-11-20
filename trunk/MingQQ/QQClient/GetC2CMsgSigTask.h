#pragma once

#include "QQUser.h"
#include "QQProtocol.h"
#include "ThreadPool.h"

class CGetC2CMsgSigTask : public CThreadPoolTask
{
public:
	CGetC2CMsgSigTask(void);
	~CGetC2CMsgSigTask(void);

public:
	virtual int Run();
	virtual int Stop();
	virtual void TaskFinish();

public:
	CQQUser * m_lpQQUser;
	CQQProtocol * m_lpQQProtocol;
	UINT m_nGroupId;		// »∫Id
	UINT m_nQQUin;			// ∫√”—Uin

private:
	BOOL m_bStop;
	CHttpClient m_HttpClient;
};
