#include "StdAfx.h"
#include "GetChatPicTask.h"

CGetChatPicTask::CGetChatPicTask(void)
{
	m_lpQQUser = NULL;
	m_lpQQProtocol = NULL;
	m_nType = OP_TYPE_BUDDY_PIC;
	m_lpBuddyMsg = NULL;
	m_lpGroupMsg = NULL;
	m_lpSessMsg = NULL;
	m_bStop = FALSE;
}

CGetChatPicTask::~CGetChatPicTask(void)
{

}

int CGetChatPicTask::Run()
{
	CBuffer bufPic;
	BOOL bRet;

	if (NULL == m_lpQQUser || NULL == m_lpQQProtocol)
	{
		m_bStop = FALSE;
		return 0;
	}

	if (OP_TYPE_BUDDY_PIC == m_nType && m_lpBuddyMsg != NULL)
	{
		int nCount = (int)m_lpBuddyMsg->m_arrContent.size();
		for (int i = 0; i < nCount; i++)
		{
			CContent * lpContent = m_lpBuddyMsg->m_arrContent[i];
			if (lpContent != NULL && CONTENT_TYPE_CUSTOM_FACE == lpContent->m_nType)
			{
				bRet = m_lpQQProtocol->GetBuddyChatPic(m_HttpClient, m_lpBuddyMsg->m_nMsgId,
					lpContent->m_strCFaceName.c_str(), m_lpBuddyMsg->m_nFromUin, 
					WEBQQ_CLIENT_ID, m_lpQQUser->m_LoginResult2.m_strPSessionId.c_str(), &bufPic);
				if (bRet)
					SavePic(lpContent->m_strCFaceName.c_str(), bufPic.GetData(), bufPic.GetSize());
			}
		}
		::PostMessage(m_lpQQUser->m_hCallBackWnd, 
			QQ_MSG_BUDDY_MSG, NULL, (LPARAM)m_lpBuddyMsg);
	}
	else if (OP_TYPE_GROUP_PIC == m_nType && m_lpGroupMsg != NULL)
	{
		int nCount = (int)m_lpGroupMsg->m_arrContent.size();
		for (int i = 0; i < nCount; i++)
		{
			CContent * lpContent = m_lpGroupMsg->m_arrContent[i];
			if (lpContent != NULL && CONTENT_TYPE_CUSTOM_FACE == lpContent->m_nType)
			{
				LPCTSTR lpFmt = _T("%[^:]:%d");
				TCHAR cServer[1024] = {0};
				int nPort = 0;

				_stscanf(lpContent->m_strCFaceServer.c_str(), lpFmt, cServer, &nPort);

				bRet = m_lpQQProtocol->GetGroupChatPic(m_HttpClient, m_lpGroupMsg->m_nGroupCode,
					m_lpGroupMsg->m_nSendUin, cServer, nPort, lpContent->m_nCFaceFileId, 
					lpContent->m_strCFaceName.c_str(), m_lpQQUser->m_LoginResult2.m_strVfWebQq.c_str(), &bufPic);
				if (bRet)
					SavePic(lpContent->m_strCFaceName.c_str(), bufPic.GetData(), bufPic.GetSize());
			}
		}
		::PostMessage(m_lpQQUser->m_hCallBackWnd, 
			QQ_MSG_GROUP_MSG, NULL, (LPARAM)m_lpGroupMsg);
	}
	else if (OP_TYPE_SESS_PIC == m_nType && m_lpSessMsg != NULL)
	{
		// Œ¥ µœ÷
	}

	m_bStop = FALSE;

	return 0;
}

int CGetChatPicTask::Stop()
{
	m_bStop = TRUE;
	m_HttpClient.SetCancalEvent();
	return 0;
}

void CGetChatPicTask::TaskFinish()
{
	delete this;
}

BOOL CGetChatPicTask::SavePic(LPCTSTR lpszFileName, const BYTE * lpData, DWORD dwSize)
{
	if (NULL == lpszFileName || NULL == *lpszFileName
		|| NULL == lpData || dwSize <= 0)
		return FALSE;

	tstring strFullName = m_lpQQUser->GetChatPicFullName(lpszFileName);

	FILE * fp = _tfopen(strFullName.c_str(), _T("wb"));
	if (fp != NULL)
	{
		fwrite(lpData, dwSize, 1, fp);
		fclose(fp);
		return TRUE;
	}
	else
		return FALSE;
}