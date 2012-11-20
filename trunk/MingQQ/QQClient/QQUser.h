#pragma once

#include <string>
#include "QQProtocolData.h"
#include "Path.h"

#if defined(UNICODE) || defined(_UNICODE)
	typedef std::wstring tstring;
#else
	typedef std::string tstring;
#endif

class CQQUser
{
public:
	CQQUser(void);
	~CQQUser(void);

public:
	tstring GetHeadPicFullName(UINT nGroupNum, UINT nQQNum);
	tstring GetChatPicFullName(LPCTSTR lpszFileName);
	
public:
	tstring m_strQQNum;
	tstring m_strQQPwd;
	UINT m_nQQUin;
	HWND m_hCallBackWnd;
	QQ_STATUS m_nLoginStatus;
	QQ_STATUS m_nStatus;
	CBuffer m_VerifyCodePic;
	tstring m_strVerifyCode;
	CVerifyCodeInfo m_VerifyCodeInfo;
	CLoginResult_1 m_LoginResult1;
	CLoginResult_2 m_LoginResult2;
	tstring m_strHeadPicPath;
	tstring m_strChatPicPath;
	CBuddyInfo m_UserInfo;
	CBuddyList m_BuddyList;
	CGroupList m_GroupList;
	CRecentList m_RecentList;
	CMessageList m_MsgList;
};
