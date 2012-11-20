#pragma once

#include "ThreadPool.h"
#include "QQUser.h"
#include "QQProtocol.h"

class CMsgItem
{
public:
	CMsgItem(void);
	~CMsgItem(void);

public:
	QQ_MSG_TYPE m_nType;
	void * m_lpMsg;
	tstring m_strGroupSig;
};

class CSendMsgTask : public CThreadPoolTask
{
public:
	CSendMsgTask(void);
	~CSendMsgTask(void);

public:
	virtual int Run();
	virtual int Stop();
	virtual void TaskFinish();
	
	BOOL AddBuddyMsg(UINT nToUin, int nFace, LPCTSTR lpMsg, 
		LPCTSTR lpFontName, int nFontSize, COLORREF clrText, 
		BOOL bBold, BOOL bItalic, BOOL bUnderLine);
	BOOL AddGroupMsg(UINT nToUin, LPCTSTR lpMsg, 
		LPCTSTR lpFontName, int nFontSize, COLORREF clrText, 
		BOOL bBold, BOOL bItalic, BOOL bUnderLine);
	BOOL AddSessMsg(UINT nToUin, int nFace, LPCTSTR lpGroupSig,
		LPCTSTR lpMsg, LPCTSTR lpFontName, int nFontSize, 
		COLORREF clrText, BOOL bBold, BOOL bItalic, BOOL bUnderLine);

public:
	CQQUser * m_lpQQUser;
	CQQProtocol * m_lpQQProtocol;

private:
	CHttpClient m_HttpClient;
	UINT m_nMsgId;
	CRITICAL_SECTION m_csItem;
	HANDLE m_hSemaphore;
	HANDLE m_hEvent;
	std::vector<CMsgItem *> m_arrItem;
};
