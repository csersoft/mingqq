#include "StdAfx.h"
#include "SendMsgTask.h"

CMsgItem::CMsgItem(void)
{
	m_nType = QQ_MSG_TYPE_BUDDY;
	m_lpMsg = NULL;
}

CMsgItem::~CMsgItem(void)
{
	if (m_lpMsg != NULL)
	{
		switch (m_nType)
		{
		case QQ_MSG_TYPE_BUDDY:
			delete (CBuddyMessage *)m_lpMsg;
			break;
		case QQ_MSG_TYPE_GROUP:
			delete (CGroupMessage *)m_lpMsg;
			break;
		case QQ_MSG_TYPE_SESS:
			delete (CSessMessage *)m_lpMsg;
			break;
		}
	}
}

CSendMsgTask::CSendMsgTask(void)
{
	m_lpQQUser = NULL;
	m_lpQQProtocol = NULL;
	m_nMsgId = 1100001;
	memset(&m_csItem, 0, sizeof(CRITICAL_SECTION));
	::InitializeCriticalSection(&m_csItem);
	m_hSemaphore = ::CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL);
	m_hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
}

CSendMsgTask::~CSendMsgTask(void)
{
	for (int i = 0; i < (int)m_arrItem.size(); i++)
	{
		CMsgItem * lpMsgItem = m_arrItem[i];
		if (lpMsgItem != NULL)
			delete lpMsgItem;
	}
	m_arrItem.clear();

	::DeleteCriticalSection(&m_csItem);

	if (m_hSemaphore != NULL)
	{
		::CloseHandle(m_hSemaphore);
		m_hSemaphore = NULL;
	}

	if (m_hEvent != NULL)
	{
		::CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
}

int CSendMsgTask::Run()
{
	CSendBuddyMsgResult SendBuddyMsgResult;
	CSendGroupMsgResult SendGroupMsgResult;
	CSendSessMsgResult SendSessMsgResult;
	HANDLE hWaitEvent[2];
	DWORD dwIndex;
	CMsgItem * lpMsgItem;
	BOOL bRet;

	if (NULL == m_lpQQUser || NULL == m_lpQQProtocol)
		return 0;

	hWaitEvent[0] = m_hEvent;
	hWaitEvent[1] = m_hSemaphore;

	while (1)
	{
		dwIndex = ::WaitForMultipleObjects(2, hWaitEvent, FALSE, INFINITE);

		if (dwIndex == WAIT_OBJECT_0)
			break;

		lpMsgItem = NULL;

		::EnterCriticalSection(&m_csItem);
		if (m_arrItem.size() > 0)
		{
			lpMsgItem = m_arrItem[0];
			m_arrItem.erase(m_arrItem.begin() + 0);
		}
		::LeaveCriticalSection(&m_csItem);

		if (lpMsgItem != NULL)
		{
			if (lpMsgItem->m_lpMsg != NULL)
			{
				switch (lpMsgItem->m_nType)
				{
				case QQ_MSG_TYPE_BUDDY:
					{
						CBuddyMessage * lpBuddyMsg = (CBuddyMessage *)lpMsgItem->m_lpMsg;
						bRet = m_lpQQProtocol->SendBuddyMsg(m_HttpClient, lpBuddyMsg, 
							WEBQQ_CLIENT_ID, m_lpQQUser->m_LoginResult2.m_strPSessionId.c_str(), 
							&SendBuddyMsgResult);
					}
					break;
				case QQ_MSG_TYPE_GROUP:
					{
						CGroupMessage * lpGroupMsg = (CGroupMessage *)lpMsgItem->m_lpMsg;
						bRet = m_lpQQProtocol->SendGroupMsg(m_HttpClient, lpGroupMsg, 
							WEBQQ_CLIENT_ID, m_lpQQUser->m_LoginResult2.m_strPSessionId.c_str(), 
							&SendGroupMsgResult);
					}
					break;
				case QQ_MSG_TYPE_SESS:
					{
						CSessMessage * lpSessMsg = (CSessMessage *)lpMsgItem->m_lpMsg;
						bRet = m_lpQQProtocol->SendSessMsg(m_HttpClient, lpSessMsg, 
							lpMsgItem->m_strGroupSig.c_str(), WEBQQ_CLIENT_ID, 
							m_lpQQUser->m_LoginResult2.m_strPSessionId.c_str(), &SendSessMsgResult);
					}
					break;
				}
			}
			delete lpMsgItem;
		}
	}

	return 0;
}

int CSendMsgTask::Stop()
{
	m_HttpClient.SetCancalEvent();
	::SetEvent(m_hEvent);
	return 0;
}

void CSendMsgTask::TaskFinish()
{
	
}

BOOL CSendMsgTask::AddBuddyMsg(UINT nToUin, int nFace, LPCTSTR lpMsg, 
						   LPCTSTR lpFontName, int nFontSize, COLORREF clrText, 
						   BOOL bBold, BOOL bItalic, BOOL bUnderLine)
{
	if (NULL == lpMsg || NULL == *lpMsg)
		return FALSE;

	CMsgItem * lpMsgItem = new CMsgItem;
	if (NULL == lpMsgItem)
		return FALSE;

	CBuddyMessage * lpBuddyMsg = new CBuddyMessage;
	if (NULL == lpBuddyMsg)
	{
		delete lpMsgItem;
		return FALSE;
	}

	m_nMsgId++;
	lpBuddyMsg->m_nMsgId = m_nMsgId;
	lpBuddyMsg->m_nToUin = nToUin;
	
	tstring strTemp;
	CContent * lpContent;
	int nLen;

	nLen = (int)_tcslen(lpMsg);
	for (int i = 0; i < nLen; i++)
	{
		if (lpMsg[i] == _T('/'))
		{
			if (lpMsg[i+1] == _T('/'))
			{
				strTemp += lpMsg[i];
				i++;
				continue;
			}
			else if (i < (nLen - 4))
			{
				if (lpMsg[i+1] == _T('f') && lpMsg[i+2] == _T('[') && lpMsg[i+3] != _T(']'))
				{
					const TCHAR * lpStart = &lpMsg[i+2];
					const TCHAR * lpEnd = _tcschr(&lpMsg[i+3], _T(']'));
					if (NULL == lpEnd)
					{
						strTemp += lpMsg[i];
						continue;
					}

					int nBufLen = lpEnd - lpStart + 1;
					if (nBufLen >= 1024)
					{
						strTemp += lpMsg[i];
						continue;
					}

					if (!strTemp.empty())
					{
						lpContent = new CContent;
						if (lpContent != NULL)
						{
							lpContent->m_nType = CONTENT_TYPE_TEXT;
							lpContent->m_strText = strTemp;
							lpBuddyMsg->m_arrContent.push_back(lpContent);
						}
						strTemp = _T("");
					}

					TCHAR cBuf[1024] = {0};
					_tcsncpy(cBuf, lpStart, nBufLen);

					LPCTSTR lpFaceFmt = _T("[%d]");
					int nFaceId = 0;

					_stscanf(cBuf, lpFaceFmt, &nFaceId);

					lpContent = new CContent;
					if (lpContent != NULL)
					{
						lpContent->m_nType = CONTENT_TYPE_FACE;
						lpContent->m_nFaceId = nFaceId;
						lpBuddyMsg->m_arrContent.push_back(lpContent);
					}

					i += (nBufLen + 1);
					continue;
				}
			}
		}

		strTemp += lpMsg[i];
	}

	if (!strTemp.empty())
	{
		lpContent = new CContent;
		if (lpContent != NULL)
		{
			lpContent->m_nType = CONTENT_TYPE_TEXT;
			lpContent->m_strText = strTemp;
			lpBuddyMsg->m_arrContent.push_back(lpContent);
		}
		strTemp = _T("");
	}

	lpContent = new CContent;
	if (lpContent != NULL)
	{
		lpContent->m_nType = CONTENT_TYPE_FONT_INFO;
		lpContent->m_FontInfo.m_strName = lpFontName;
		lpContent->m_FontInfo.m_nSize = nFontSize;
		lpContent->m_FontInfo.m_clrText = clrText;
		lpContent->m_FontInfo.m_bBold = bBold;
		lpContent->m_FontInfo.m_bItalic = bItalic;
		lpContent->m_FontInfo.m_bUnderLine = bUnderLine;
		lpBuddyMsg->m_arrContent.push_back(lpContent);
	}

	lpMsgItem->m_nType = QQ_MSG_TYPE_BUDDY;
	lpMsgItem->m_lpMsg = (void *)lpBuddyMsg;

	::EnterCriticalSection(&m_csItem);
	m_arrItem.push_back(lpMsgItem);
	::LeaveCriticalSection(&m_csItem);

	::ReleaseSemaphore(m_hSemaphore, 1, NULL);

	return TRUE;
}

BOOL CSendMsgTask::AddGroupMsg(UINT nToUin, LPCTSTR lpMsg, LPCTSTR lpFontName, 
						   int nFontSize, COLORREF clrText, BOOL bBold, 
						   BOOL bItalic, BOOL bUnderLine)
{
	if (NULL == lpMsg || NULL == *lpMsg)
		return FALSE;

	CMsgItem * lpMsgItem = new CMsgItem;
	if (NULL == lpMsgItem)
		return FALSE;

	CGroupMessage * lpGroupMsg = new CGroupMessage;
	if (NULL == lpGroupMsg)
	{
		delete lpMsgItem;
		return FALSE;
	}

	m_nMsgId++;
	lpGroupMsg->m_nMsgId = m_nMsgId;
	lpGroupMsg->m_nToUin = nToUin;

	tstring strTemp;
	CContent * lpContent;
	int nLen;

	nLen = (int)_tcslen(lpMsg);
	for (int i = 0; i < nLen; i++)
	{
		if (lpMsg[i] == _T('/'))
		{
			if (lpMsg[i+1] == _T('/'))
			{
				strTemp += lpMsg[i];
				i++;
				continue;
			}
			else if (i < (nLen - 4))
			{
				if (lpMsg[i+1] == _T('f') && lpMsg[i+2] == _T('[') && lpMsg[i+3] != _T(']'))
				{
					const TCHAR * lpStart = &lpMsg[i+2];
					const TCHAR * lpEnd = _tcschr(&lpMsg[i+3], _T(']'));
					if (NULL == lpEnd)
					{
						strTemp += lpMsg[i];
						continue;
					}

					int nBufLen = lpEnd - lpStart + 1;
					if (nBufLen >= 1024)
					{
						strTemp += lpMsg[i];
						continue;
					}

					if (!strTemp.empty())
					{
						lpContent = new CContent;
						if (lpContent != NULL)
						{
							lpContent->m_nType = CONTENT_TYPE_TEXT;
							lpContent->m_strText = strTemp;
							lpGroupMsg->m_arrContent.push_back(lpContent);
						}
						strTemp = _T("");
					}

					TCHAR cBuf[1024] = {0};
					_tcsncpy(cBuf, lpStart, nBufLen);

					LPCTSTR lpFaceFmt = _T("[%d]");
					int nFaceId = 0;

					_stscanf(cBuf, lpFaceFmt, &nFaceId);

					lpContent = new CContent;
					if (lpContent != NULL)
					{
						lpContent->m_nType = CONTENT_TYPE_FACE;
						lpContent->m_nFaceId = nFaceId;
						lpGroupMsg->m_arrContent.push_back(lpContent);
					}

					i += (nBufLen + 1);
					continue;
				}
			}
		}

		strTemp += lpMsg[i];
	}

	if (!strTemp.empty())
	{
		lpContent = new CContent;
		if (lpContent != NULL)
		{
			lpContent->m_nType = CONTENT_TYPE_TEXT;
			lpContent->m_strText = strTemp;
			lpGroupMsg->m_arrContent.push_back(lpContent);
		}
		strTemp = _T("");
	}

	lpContent = new CContent;
	if (lpContent != NULL)
	{
		lpContent->m_nType = CONTENT_TYPE_FONT_INFO;
		lpContent->m_FontInfo.m_strName = lpFontName;
		lpContent->m_FontInfo.m_nSize = nFontSize;
		lpContent->m_FontInfo.m_clrText = clrText;
		lpContent->m_FontInfo.m_bBold = bBold;
		lpContent->m_FontInfo.m_bItalic = bItalic;
		lpContent->m_FontInfo.m_bUnderLine = bUnderLine;
		lpGroupMsg->m_arrContent.push_back(lpContent);
	}

	lpMsgItem->m_nType = QQ_MSG_TYPE_GROUP;
	lpMsgItem->m_lpMsg = (void *)lpGroupMsg;

	::EnterCriticalSection(&m_csItem);
	m_arrItem.push_back(lpMsgItem);
	::LeaveCriticalSection(&m_csItem);

	::ReleaseSemaphore(m_hSemaphore, 1, NULL);

	return TRUE;
}

BOOL CSendMsgTask::AddSessMsg(UINT nToUin, int nFace, LPCTSTR lpGroupSig,
				LPCTSTR lpMsg, LPCTSTR lpFontName, int nFontSize, 
				COLORREF clrText, BOOL bBold, BOOL bItalic, BOOL bUnderLine)
{
	if (NULL == lpMsg || NULL == *lpMsg || NULL == lpGroupSig || NULL == *lpGroupSig)
		return FALSE;

	CMsgItem * lpMsgItem = new CMsgItem;
	if (NULL == lpMsgItem)
		return FALSE;

	CSessMessage * lpSessMsg = new CSessMessage;
	if (NULL == lpSessMsg)
	{
		delete lpMsgItem;
		return FALSE;
	}

	m_nMsgId++;
	lpSessMsg->m_nMsgId = m_nMsgId;
	lpSessMsg->m_nToUin = nToUin;

	tstring strTemp;
	CContent * lpContent;
	int nLen;

	nLen = (int)_tcslen(lpMsg);
	for (int i = 0; i < nLen; i++)
	{
		if (lpMsg[i] == _T('/'))
		{
			if (lpMsg[i+1] == _T('/'))
			{
				strTemp += lpMsg[i];
				i++;
				continue;
			}
			else if (i < (nLen - 4))
			{
				if (lpMsg[i+1] == _T('f') && lpMsg[i+2] == _T('[') && lpMsg[i+3] != _T(']'))
				{
					const TCHAR * lpStart = &lpMsg[i+2];
					const TCHAR * lpEnd = _tcschr(&lpMsg[i+3], _T(']'));
					if (NULL == lpEnd)
					{
						strTemp += lpMsg[i];
						continue;
					}

					int nBufLen = lpEnd - lpStart + 1;
					if (nBufLen >= 1024)
					{
						strTemp += lpMsg[i];
						continue;
					}

					if (!strTemp.empty())
					{
						lpContent = new CContent;
						if (lpContent != NULL)
						{
							lpContent->m_nType = CONTENT_TYPE_TEXT;
							lpContent->m_strText = strTemp;
							lpSessMsg->m_arrContent.push_back(lpContent);
						}
						strTemp = _T("");
					}

					TCHAR cBuf[1024] = {0};
					_tcsncpy(cBuf, lpStart, nBufLen);

					LPCTSTR lpFaceFmt = _T("[%d]");
					int nFaceId = 0;

					_stscanf(cBuf, lpFaceFmt, &nFaceId);

					lpContent = new CContent;
					if (lpContent != NULL)
					{
						lpContent->m_nType = CONTENT_TYPE_FACE;
						lpContent->m_nFaceId = nFaceId;
						lpSessMsg->m_arrContent.push_back(lpContent);
					}

					i += (nBufLen + 1);
					continue;
				}
			}
		}

		strTemp += lpMsg[i];
	}

	if (!strTemp.empty())
	{
		lpContent = new CContent;
		if (lpContent != NULL)
		{
			lpContent->m_nType = CONTENT_TYPE_TEXT;
			lpContent->m_strText = strTemp;
			lpSessMsg->m_arrContent.push_back(lpContent);
		}
		strTemp = _T("");
	}

	lpContent = new CContent;
	if (lpContent != NULL)
	{
		lpContent->m_nType = CONTENT_TYPE_FONT_INFO;
		lpContent->m_FontInfo.m_strName = lpFontName;
		lpContent->m_FontInfo.m_nSize = nFontSize;
		lpContent->m_FontInfo.m_clrText = clrText;
		lpContent->m_FontInfo.m_bBold = bBold;
		lpContent->m_FontInfo.m_bItalic = bItalic;
		lpContent->m_FontInfo.m_bUnderLine = bUnderLine;
		lpSessMsg->m_arrContent.push_back(lpContent);
	}

	lpMsgItem->m_nType = QQ_MSG_TYPE_SESS;
	lpMsgItem->m_lpMsg = (void *)lpSessMsg;
	lpMsgItem->m_strGroupSig = lpGroupSig;

	::EnterCriticalSection(&m_csItem);
	m_arrItem.push_back(lpMsgItem);
	::LeaveCriticalSection(&m_csItem);

	::ReleaseSemaphore(m_hSemaphore, 1, NULL);

	return TRUE;
}