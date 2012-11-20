#include "StdAfx.h"
#include "PollTask.h"

CPollTask::CPollTask(void)
{
	m_lpQQUser = NULL;
	m_lpQQProtocol = NULL;
	m_lpThreadPool = NULL;
	m_bStop = FALSE;
}

CPollTask::~CPollTask(void)
{
}

int CPollTask::Run()
{
	CBuffer PollResult;
	Json::Reader JsonReader;
	Json::Value JsonValue;
	std::string strText;
	int nRetCode;
	BOOL bRet;

	if (NULL == m_lpQQUser || NULL == m_lpQQProtocol)
	{
		m_bStop = FALSE;
		return 0;
	}

	while (1)
	{
		bRet = m_lpQQProtocol->Poll(m_HttpClient, WEBQQ_CLIENT_ID, 
			m_lpQQUser->m_LoginResult2.m_strPSessionId.c_str(), &PollResult);
		if (bRet && PollResult.GetData() != NULL && PollResult.GetSize() > 0)
		{
			strText = (CHAR *)PollResult.GetData();
			if (JsonReader.parse(strText, JsonValue))
			{
				if (!JsonValue["retcode"].isNull())
					nRetCode = JsonValue["retcode"].asInt();

				if (0 == nRetCode && !JsonValue["result"].isNull()
					&& JsonValue["result"].isArray())
				{
					for (int i = 0; i < JsonValue["result"].size(); i++)
					{
						HandleMsg(JsonValue["result"][i]);
					}
				}
			}
		}

		if (m_bStop)
			break;
	}

	m_bStop = FALSE;

	return 0;
}

int CPollTask::Stop()
{
	m_bStop = TRUE;
	m_HttpClient.SetCancalEvent();
	return 0;
}

void CPollTask::TaskFinish()
{

}

BOOL CPollTask::HandleMsg(Json::Value& JsonValue)
{
	std::string strValue;
	tstring strPollType;

	if (!JsonValue.isNull())
	{
		if (!JsonValue["poll_type"].isNull())
		{
			strValue = JsonValue["poll_type"].asString();
			strPollType = Utf8ToUnicode(strValue);

			if (CBuddyMessage::IsType(strPollType.c_str()))	// 好友消息
			{
				HandleBuddyMsg(JsonValue["value"]);
			}
			else if (CGroupMessage::IsType(strPollType.c_str()))	// 群消息
			{
				HandleGroupMsg(JsonValue["value"]);
			}
			else if (CSessMessage::IsType(strPollType.c_str()))	// 临时会话消息
			{
				HandleSessMsg(JsonValue["value"]);
			}
			else if (CStatusChangeMessage::IsType(strPollType.c_str()))	// 状态改变通知消息
			{
				HandleStatusChangeMsg(JsonValue["value"]);
			}
			else if (CKickMessage::IsType(strPollType.c_str()))	// 被踢下线通知消息
			{
				HandleKickMsg(JsonValue["value"]);
			}
			else if (CSysGroupMessage::IsType(strPollType.c_str()))	// 群系统消息
			{
				HandleSysGroupMsg(JsonValue["value"]);
			}
		}
	}
	return TRUE;
}

BOOL CPollTask::HandleBuddyMsg(Json::Value& JsonValue)
{
	CBuddyMessage * lpBuddyMsg = new CBuddyMessage;
	if (lpBuddyMsg != NULL)
	{
		BOOL bRet = lpBuddyMsg->Parse(JsonValue);
		if (bRet)
		{
			if (IsNeedDownloadPic(lpBuddyMsg->m_arrContent))
			{
				StartGetChatPicTask(OP_TYPE_BUDDY_PIC, lpBuddyMsg, NULL, NULL);
			}
			else
			{
				::PostMessage(m_lpQQUser->m_hCallBackWnd, 
					QQ_MSG_BUDDY_MSG, NULL, (LPARAM)lpBuddyMsg);
			}
		}
	}
	return TRUE;
}

BOOL CPollTask::HandleGroupMsg(Json::Value& JsonValue)
{
	CGroupMessage * lpGroupMsg = new CGroupMessage;
	if (lpGroupMsg != NULL)
	{
		BOOL bRet = lpGroupMsg->Parse(JsonValue);
		if (bRet)
		{
			if (IsNeedDownloadPic(lpGroupMsg->m_arrContent))
			{
				StartGetChatPicTask(OP_TYPE_GROUP_PIC, NULL, lpGroupMsg, NULL);
			}
			else
			{
				::PostMessage(m_lpQQUser->m_hCallBackWnd, 
					QQ_MSG_GROUP_MSG, NULL, (LPARAM)lpGroupMsg);
			}
		}
	}
	return TRUE;
}

BOOL CPollTask::HandleSessMsg(Json::Value& JsonValue)
{
	CSessMessage * lpSessMsg = new CSessMessage;
	if (lpSessMsg != NULL)
	{
		BOOL bRet = lpSessMsg->Parse(JsonValue);
		if (bRet)
		{
			::PostMessage(m_lpQQUser->m_hCallBackWnd, 
				QQ_MSG_SESS_MSG, NULL, (LPARAM)lpSessMsg);
		}
	}
	return TRUE;
}

BOOL CPollTask::HandleStatusChangeMsg(Json::Value& JsonValue)
{
	CStatusChangeMessage * lpStatusChangeMsg = new CStatusChangeMessage;
	if (lpStatusChangeMsg != NULL)
	{
		BOOL bRet = lpStatusChangeMsg->Parse(JsonValue);
		if (bRet)
		{
			::PostMessage(m_lpQQUser->m_hCallBackWnd, 
				QQ_MSG_STATUS_CHANGE_MSG, NULL, (LPARAM)lpStatusChangeMsg);
		}
	}
	return TRUE;
}

BOOL CPollTask::HandleKickMsg(Json::Value& JsonValue)
{
	CKickMessage * lpKickMsg = new CKickMessage;
	if (lpKickMsg != NULL)
	{
		BOOL bRet = lpKickMsg->Parse(JsonValue);
		if (bRet)
		{
			m_lpQQUser->m_nStatus = QQ_STATUS_OFFLINE;
			m_lpThreadPool->RemoveAllTask();
			::PostMessage(m_lpQQUser->m_hCallBackWnd, 
				QQ_MSG_KICK_MSG, NULL, (LPARAM)lpKickMsg);
		}
	}
	m_bStop = TRUE;
	return TRUE;
}

BOOL CPollTask::HandleSysGroupMsg(Json::Value& JsonValue)
{
	CSysGroupMessage * lpSysGroupMsg = new CSysGroupMessage;
	if (lpSysGroupMsg != NULL)
	{
		BOOL bRet = lpSysGroupMsg->Parse(JsonValue);
		if (bRet)
		{
			::PostMessage(m_lpQQUser->m_hCallBackWnd, 
				QQ_MSG_SYS_GROUP_MSG, NULL, (LPARAM)lpSysGroupMsg);
		}
	}
	return TRUE;
}

BOOL CPollTask::IsNeedDownloadPic(std::vector<CContent *>& arrContent)
{
	for (int i = 0; i < (int)arrContent.size(); i++)
	{
		CContent * lpContent = arrContent[i];
		if (lpContent != NULL && CONTENT_TYPE_CUSTOM_FACE == lpContent->m_nType)
		{
			tstring strFullName = m_lpQQUser->GetChatPicFullName(lpContent->m_strCFaceName.c_str());
			if (!ZYM::CPath::IsFileExist(strFullName.c_str()))
				return TRUE;
		}
	}
	return FALSE;
}

BOOL CPollTask::StartGetChatPicTask(GETCHATPIC_OP_TYPE nType, CBuddyMessage * lpBuddyMsg, 
									CGroupMessage * lpGroupMsg, CSessMessage * lpSessMsg)
{
	CGetChatPicTask * lpGetChatPicTask = new CGetChatPicTask;
	if (NULL == lpGetChatPicTask)
		return FALSE;

	lpGetChatPicTask->m_lpQQUser = m_lpQQUser;
	lpGetChatPicTask->m_lpQQProtocol = m_lpQQProtocol;
	lpGetChatPicTask->m_nType = nType;
	lpGetChatPicTask->m_lpBuddyMsg = lpBuddyMsg;
	lpGetChatPicTask->m_lpGroupMsg = lpGroupMsg;
	lpGetChatPicTask->m_lpSessMsg = lpSessMsg;
	return m_lpThreadPool->AddTask(lpGetChatPicTask);
}