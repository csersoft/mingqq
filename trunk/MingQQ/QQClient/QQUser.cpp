#include "StdAfx.h"
#include "QQUser.h"

CQQUser::CQQUser(void)
{
	m_hCallBackWnd = NULL;
	m_strQQNum = _T("");
	m_strQQPwd = _T("");
	m_nQQUin = 0;
	m_nLoginStatus = QQ_STATUS_ONLINE;
	m_nStatus = QQ_STATUS_OFFLINE;
	m_strVerifyCode = _T("");
	m_strChatPicPath = _T("");
	m_strHeadPicPath = _T("");
}

CQQUser::~CQQUser(void)
{
}

tstring CQQUser::GetHeadPicFullName(UINT nGroupNum, UINT nQQNum)
{
	TCHAR szFullName[1024] = {0};

	if (nGroupNum != 0 && nQQNum != 0)	// 群成员
		wsprintf(szFullName, _T("%sGroup\\%u\\%u.png"), m_strHeadPicPath.c_str(), nGroupNum, nQQNum);
	else if (nQQNum != 0)				// 好友
		wsprintf(szFullName, _T("%sBuddy\\%u.png"), m_strHeadPicPath.c_str(), nQQNum);
	else if (nGroupNum != 0)			// 群
		wsprintf(szFullName, _T("%sGroup\\%u.png"), m_strHeadPicPath.c_str(), nGroupNum);

	return szFullName;
}

tstring CQQUser::GetChatPicFullName(LPCTSTR lpszFileName)
{
	tstring strFileName, strExtName, strNewFileName;

	if (NULL == lpszFileName || NULL == *lpszFileName)
		return _T("");

	strFileName = ZYM::CPath::GetFileNameWithoutExtension(lpszFileName);
	strExtName = ZYM::CPath::GetExtension(lpszFileName);

	for (int i = 0; i < (int)strFileName.size(); i++)
	{
		if (isalnum(strFileName[i]))
			strNewFileName += strFileName[i];
	}

	strNewFileName += _T(".");
	strNewFileName += strExtName;
	strNewFileName = m_strChatPicPath + strNewFileName;

	return strNewFileName;
}