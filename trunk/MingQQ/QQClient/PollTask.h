#pragma once

#include "ThreadPool.h"
#include "QQUser.h"
#include "QQProtocol.h"
#include "GetChatPicTask.h"

class CPollTask : public CThreadPoolTask
{
public:
	CPollTask(void);
	~CPollTask(void);

public:
	virtual int Run();
	virtual int Stop();
	virtual void TaskFinish();

private:
	BOOL HandleMsg(Json::Value& JsonValue);
	BOOL HandleBuddyMsg(Json::Value& JsonValue);
	BOOL HandleGroupMsg(Json::Value& JsonValue);
	BOOL HandleSessMsg(Json::Value& JsonValue);
	BOOL HandleStatusChangeMsg(Json::Value& JsonValue);
	BOOL HandleKickMsg(Json::Value& JsonValue);
	BOOL HandleSysGroupMsg(Json::Value& JsonValue);
	BOOL IsNeedDownloadPic(std::vector<CContent *>& arrContent);
	BOOL StartGetChatPicTask(GETCHATPIC_OP_TYPE nType, CBuddyMessage * lpBuddyMsg, 
		CGroupMessage * lpGroupMsg, CSessMessage * lpSessMsg);
	
public:
	CQQUser * m_lpQQUser;
	CQQProtocol * m_lpQQProtocol;
	CThreadPool * m_lpThreadPool;

private:
	BOOL m_bStop;
	CHttpClient m_HttpClient;
};
