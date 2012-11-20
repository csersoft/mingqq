#pragma once

#include "QQUser.h"
#include "QQProtocol.h"
#include "ThreadPool.h"

struct GHPT_PARAM
{
	UINT nGroupCode;
	UINT nQQUin;
	UINT nGroupNum;
	UINT nQQNum;
};

class CGetHeadPicTask : public CThreadPoolTask
{
public:
	CGetHeadPicTask(void);
	~CGetHeadPicTask(void);

public:
	virtual int Run();
	virtual int Stop();
	virtual void TaskFinish();

	BOOL AddParam(UINT nGroupCode, UINT nQQUin, UINT nGroupNum, UINT nQQNum);
	void DelAllParams();

private:
	BOOL SavePic(UINT nGroupNum, UINT nQQNum, const BYTE * lpData, DWORD dwSize);

public:
	CQQUser * m_lpQQUser;
	CQQProtocol * m_lpQQProtocol;
	std::vector<GHPT_PARAM> m_arrParam;

private:
	BOOL m_bStop;
	CHttpClient m_HttpClient;
};
