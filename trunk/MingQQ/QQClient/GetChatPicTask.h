#pragma once

#include "ThreadPool.h"
#include "QQUser.h"
#include "QQProtocol.h"

enum GETCHATPIC_OP_TYPE
{
	OP_TYPE_BUDDY_PIC,
	OP_TYPE_GROUP_PIC,
	OP_TYPE_SESS_PIC	// WebQQ3.0目前不支持群内临时消息发送图片
};

class CGetChatPicTask : public CThreadPoolTask
{
public:
	CGetChatPicTask(void);
	~CGetChatPicTask(void);

public:
	virtual int Run();
	virtual int Stop();
	virtual void TaskFinish();

private:
	BOOL SavePic(LPCTSTR lpszFileName, const BYTE * lpData, DWORD dwSize);

public:
	CQQUser * m_lpQQUser;
	CQQProtocol * m_lpQQProtocol;
	GETCHATPIC_OP_TYPE m_nType;
	CBuddyMessage * m_lpBuddyMsg;
	CGroupMessage * m_lpGroupMsg;
	CSessMessage * m_lpSessMsg;

private:
	BOOL m_bStop;
	CHttpClient m_HttpClient;
};
