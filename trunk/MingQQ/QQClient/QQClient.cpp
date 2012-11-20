#include "StdAfx.h"
#include "QQClient.h"

CQQClient::CQQClient(void)
{
}

CQQClient::~CQQClient(void)
{
}

// 初始化客户端
BOOL CQQClient::Init()
{
	BOOL bRet;
	bRet = m_ThreadPool.Init(5);	// 初始化线程池
	return bRet;
}

// 反初始化客户端
void CQQClient::UnInit()
{
	m_ThreadPool.UnInit();			// 反初始化线程池
}

// 设置QQ号码和密码
void CQQClient::SetUser(LPCTSTR lpQQNum, LPCTSTR lpQQPwd)
{
	if (NULL == lpQQNum || NULL == lpQQPwd
		|| m_QQUser.m_nStatus != QQ_STATUS_OFFLINE)
		return;

	m_QQUser.m_strQQNum = lpQQNum;
	m_QQUser.m_strQQPwd = lpQQPwd;
	m_QQUser.m_nQQUin = _tcstol(lpQQNum, NULL, 10);
	m_QQUser.m_UserInfo.m_nQQUin = m_QQUser.m_nQQUin;
	m_QQUser.m_UserInfo.m_nQQNum = m_QQUser.m_nQQUin;
	m_QQUser.m_UserInfo.m_bHasQQNum = TRUE;

	m_QQUser.m_strChatPicPath = ZYM::CPath::GetCurDir();
	m_QQUser.m_strChatPicPath += _T("Users\\");
	m_QQUser.m_strChatPicPath += m_QQUser.m_strQQNum;
	m_QQUser.m_strChatPicPath += _T("\\ChatImage\\");

	m_QQUser.m_strHeadPicPath = ZYM::CPath::GetCurDir();
	m_QQUser.m_strHeadPicPath += _T("Users\\");
	m_QQUser.m_strHeadPicPath += m_QQUser.m_strQQNum;
	m_QQUser.m_strHeadPicPath += _T("\\HeadImage\\");

	if (!ZYM::CPath::IsDirectoryExist(m_QQUser.m_strChatPicPath.c_str()))
		ZYM::CPath::CreateDirectory(m_QQUser.m_strChatPicPath.c_str(), NULL);

	if (!ZYM::CPath::IsDirectoryExist(m_QQUser.m_strHeadPicPath.c_str()))
		ZYM::CPath::CreateDirectory(m_QQUser.m_strHeadPicPath.c_str(), NULL);
}

// 设置登录状态
void CQQClient::SetLoginStatus(QQ_STATUS nStatus)
{
	m_QQUser.m_nLoginStatus = nStatus;
}

// 设置回调窗口句柄
void CQQClient::SetCallBackWnd(HWND hCallBackWnd)
{
	m_QQUser.m_hCallBackWnd = hCallBackWnd;
}

// 设置验证码
void CQQClient::SetVerifyCode(LPCTSTR lpVerifyCode)
{
	if (lpVerifyCode != NULL)
		m_QQUser.m_strVerifyCode = lpVerifyCode;
}

// 登录
BOOL CQQClient::Login()
{
	if (m_QQUser.m_strQQNum.empty() || m_QQUser.m_strQQPwd.empty()
		|| m_QQUser.m_nStatus != QQ_STATUS_OFFLINE)
		return FALSE;
	
	if (m_LoginTask.IsRunning())
		return FALSE;

	m_LoginTask.m_lpQQUser = &m_QQUser;
	m_LoginTask.m_lpQQProtocol = &m_QQProtocol;
	m_LoginTask.m_lpThreadPool = &m_ThreadPool;
	m_LoginTask.m_lpPollTask = &m_PollTask;
	m_LoginTask.m_lpSendMsgTask = &m_SendMsgTask;

	return m_ThreadPool.AddTask(&m_LoginTask);
}

// 注销
BOOL CQQClient::Logout()
{
	if (!IsOnline())
		return FALSE;

	m_LogoutTask.m_lpQQUser = &m_QQUser;
	m_LogoutTask.m_lpQQProtocol = &m_QQProtocol;
	return m_ThreadPool.AddTask(&m_LogoutTask);
}

// 取消登录
void CQQClient::CancelLogin()
{
	m_ThreadPool.RemoveAllTask();
}

// 改变在线状态
void CQQClient::ChangeStatus(QQ_STATUS nStatus)
{
	if (!IsOnline())
		return;

	CChangeStatusTask * lpChangeStatusTask = new CChangeStatusTask;
	if (lpChangeStatusTask != NULL)
	{
		lpChangeStatusTask->m_lpQQUser = &m_QQUser;
		lpChangeStatusTask->m_lpQQProtocol = &m_QQProtocol;
		lpChangeStatusTask->m_nStatus = nStatus;
		m_ThreadPool.AddTask(lpChangeStatusTask);
	}
}

// 更新好友列表
void CQQClient::UpdateBuddyList()
{
	if (!IsOnline())
		return;
}

// 更新群列表
void CQQClient::UpdateGroupList()
{
	if (!IsOnline())
		return;
}

// 更新最近联系人列表
void CQQClient::UpdateRecentList()
{
	if (!IsOnline())
		return;
}

// 更新好友信息
void CQQClient::UpdateBuddyInfo(UINT nQQUin)
{
	if (!IsOnline())
		return;

	CInfoTask * lpInfoTask = new CInfoTask;
	if (lpInfoTask != NULL)
	{
		lpInfoTask->m_lpQQUser = &m_QQUser;
		lpInfoTask->m_lpQQProtocol = &m_QQProtocol;
		lpInfoTask->m_nType = OP_TYPE_GETBUDDYINFO;
		lpInfoTask->m_nGroupCode = 0;
		lpInfoTask->m_nQQUin = nQQUin;
		m_ThreadPool.AddTask(lpInfoTask);
	}
}

// 更新群成员信息
void CQQClient::UpdateGroupMemberInfo(UINT nGroupCode, UINT nQQUin)
{
	if (!IsOnline())
		return;

	CInfoTask * lpInfoTask = new CInfoTask;
	if (lpInfoTask != NULL)
	{
		lpInfoTask->m_lpQQUser = &m_QQUser;
		lpInfoTask->m_lpQQProtocol = &m_QQProtocol;
		lpInfoTask->m_nType = OP_TYPE_GETGMEMBERINFO;
		lpInfoTask->m_nGroupCode = nGroupCode;
		lpInfoTask->m_nQQUin = nQQUin;
		m_ThreadPool.AddTask(lpInfoTask);
	}
}

// 更新群信息
void CQQClient::UpdateGroupInfo(UINT nGroupCode)
{
	if (!IsOnline())
		return;

	CInfoTask * lpInfoTask = new CInfoTask;
	if (lpInfoTask != NULL)
	{
		lpInfoTask->m_lpQQUser = &m_QQUser;
		lpInfoTask->m_lpQQProtocol = &m_QQProtocol;
		lpInfoTask->m_nType = OP_TYPE_GETGROUPINFO;
		lpInfoTask->m_nGroupCode = nGroupCode;
		lpInfoTask->m_nQQUin = 0;
		m_ThreadPool.AddTask(lpInfoTask);
	}
}

// 更新好友号码
void CQQClient::UpdateBuddyNum(UINT nQQUin)
{
	if (!IsOnline())
		return;

	CQQNumTask * lpQQNumTask = new CQQNumTask;
	if (lpQQNumTask != NULL)
	{
		lpQQNumTask->m_lpQQUser = &m_QQUser;
		lpQQNumTask->m_lpQQProtocol = &m_QQProtocol;
		lpQQNumTask->AddParam(OP_TYPE_GETBUDDYNUM, 0, nQQUin);
		m_ThreadPool.AddTask(lpQQNumTask);
	}
}

// 更新群成员号码
void CQQClient::UpdateGroupMemberNum(UINT nGroupCode, UINT nQQUin)
{
	if (!IsOnline())
		return;

	CQQNumTask * lpQQNumTask = new CQQNumTask;
	if (lpQQNumTask != NULL)
	{
		lpQQNumTask->m_lpQQUser = &m_QQUser;
		lpQQNumTask->m_lpQQProtocol = &m_QQProtocol;
		lpQQNumTask->AddParam(OP_TYPE_GETGMEMBERNUM, nGroupCode, nQQUin);
		m_ThreadPool.AddTask(lpQQNumTask);
	}
}

// 更新群成员号码
void CQQClient::UpdateGroupMemberNum(UINT nGroupCode, std::vector<UINT> * arrQQUin)
{
	if (QQ_STATUS_OFFLINE == m_QQUser.m_nStatus
		|| NULL == arrQQUin || (int)arrQQUin->size() <= 0)
		return;

	CQQNumTask * lpQQNumTask = new CQQNumTask;
	if (lpQQNumTask != NULL)
	{
		lpQQNumTask->m_lpQQUser = &m_QQUser;
		lpQQNumTask->m_lpQQProtocol = &m_QQProtocol;
		for (int i = 0; i < (int)arrQQUin->size(); i++)
		{
			lpQQNumTask->AddParam(OP_TYPE_GETGMEMBERNUM, nGroupCode, arrQQUin->at(i));
		}
		m_ThreadPool.AddTask(lpQQNumTask);
	}
}

// 更新群号码
void CQQClient::UpdateGroupNum(UINT nGroupCode)
{
	if (!IsOnline())
		return;

	CQQNumTask * lpQQNumTask = new CQQNumTask;
	if (lpQQNumTask != NULL)
	{
		lpQQNumTask->m_lpQQUser = &m_QQUser;
		lpQQNumTask->m_lpQQProtocol = &m_QQProtocol;
		lpQQNumTask->AddParam(OP_TYPE_GETGROUPNUM, nGroupCode, 0);
		m_ThreadPool.AddTask(lpQQNumTask);
	}
}

// 更新好友个性签名
void CQQClient::UpdateBuddySign(UINT nQQUin)
{
	if (!IsOnline())
		return;

	CQQSignTask * lpQQSignTask = new CQQSignTask;
	if (NULL == lpQQSignTask)
		return;

	lpQQSignTask->m_lpQQUser = &m_QQUser;
	lpQQSignTask->m_lpQQProtocol = &m_QQProtocol;
	lpQQSignTask->m_nType = QQSIGN_OP_TYPE_GET;
	lpQQSignTask->AddGetParam(FALSE, 0, nQQUin);
	m_ThreadPool.AddTask(lpQQSignTask);
}

// 更新群成员个性签名
void CQQClient::UpdateGroupMemberSign(UINT nGroupCode, UINT nQQUin)
{
	if (!IsOnline())
		return;

	CQQSignTask * lpQQSignTask = new CQQSignTask;
	if (NULL == lpQQSignTask)
		return;

	lpQQSignTask->m_lpQQUser = &m_QQUser;
	lpQQSignTask->m_lpQQProtocol = &m_QQProtocol;
	lpQQSignTask->m_nType = QQSIGN_OP_TYPE_GET;
	lpQQSignTask->AddGetParam(TRUE, nGroupCode, nQQUin);
	m_ThreadPool.AddTask(lpQQSignTask);
}

// 修改QQ个性签名
void CQQClient::ModifyQQSign(LPCTSTR lpSign)
{
	if (!IsOnline())
		return;

	if (NULL == lpSign)
		return;

	CQQSignTask * lpQQSignTask = new CQQSignTask;
	if (NULL == lpQQSignTask)
		return;

	lpQQSignTask->m_lpQQUser = &m_QQUser;
	lpQQSignTask->m_lpQQProtocol = &m_QQProtocol;
	lpQQSignTask->m_nType = QQSIGN_OP_TYPE_SET;
	lpQQSignTask->AddSetParam(lpSign);
	m_ThreadPool.AddTask(lpQQSignTask);
}

// 更新好友头像
void CQQClient::UpdateBuddyHeadPic(UINT nQQUin, UINT nQQNum)
{
	if (!IsOnline())
		return;

	CGetHeadPicTask * lpGetHeadPicTask = new CGetHeadPicTask;
	if (NULL == lpGetHeadPicTask)
		return;

	lpGetHeadPicTask->m_lpQQUser = &m_QQUser;
	lpGetHeadPicTask->m_lpQQProtocol = &m_QQProtocol;
	lpGetHeadPicTask->AddParam(0, nQQUin, 0, nQQNum);
	m_ThreadPool.AddTask(lpGetHeadPicTask);
}

// 更新群成员头像
void CQQClient::UpdateGroupMemberHeadPic(UINT nGroupCode, UINT nQQUin, UINT nGroupNum, UINT nQQNum)
{
	if (!IsOnline())
		return;

	CGetHeadPicTask * lpGetHeadPicTask = new CGetHeadPicTask;
	if (NULL == lpGetHeadPicTask)
		return;

	lpGetHeadPicTask->m_lpQQUser = &m_QQUser;
	lpGetHeadPicTask->m_lpQQProtocol = &m_QQProtocol;
	lpGetHeadPicTask->AddParam(nGroupCode, nQQUin, nGroupNum, nQQNum);
	m_ThreadPool.AddTask(lpGetHeadPicTask);
}

// 更新群头像
void CQQClient::UpdateGroupHeadPic(UINT nGroupCode, UINT nGroupNum)
{
	if (!IsOnline())
		return;

	CGetHeadPicTask * lpGetHeadPicTask = new CGetHeadPicTask;
	if (NULL == lpGetHeadPicTask)
		return;

	lpGetHeadPicTask->m_lpQQUser = &m_QQUser;
	lpGetHeadPicTask->m_lpQQProtocol = &m_QQProtocol;
	lpGetHeadPicTask->AddParam(nGroupCode, 0, nGroupNum, 0);
	m_ThreadPool.AddTask(lpGetHeadPicTask);
}

// 更新临时会话信令
void CQQClient::UpdateC2CMsgSignal(UINT nGroupId, UINT nQQUin)
{
	if (!IsOnline())
		return;

	CGetC2CMsgSigTask * lpGetC2CMsgSigTask = new CGetC2CMsgSigTask;
	if (NULL == lpGetC2CMsgSigTask)
		return;

	lpGetC2CMsgSigTask->m_lpQQUser = &m_QQUser;
	lpGetC2CMsgSigTask->m_lpQQProtocol = &m_QQProtocol;
	lpGetC2CMsgSigTask->m_nGroupId = nGroupId;
	lpGetC2CMsgSigTask->m_nQQUin = nQQUin;
	m_ThreadPool.AddTask(lpGetC2CMsgSigTask);
}

// 发送好友消息
BOOL CQQClient::SendBuddyMsg(UINT nToUin, int nFace, LPCTSTR lpMsg, 
							 LPCTSTR lpFontName, int nFontSize, COLORREF clrText, 
							 BOOL bBold, BOOL bItalic, BOOL bUnderLine)
{
	if (!IsOnline())
		return FALSE;

	return m_SendMsgTask.AddBuddyMsg(nToUin, nFace, lpMsg, lpFontName, 
		nFontSize, clrText, bBold, bItalic, bUnderLine);
}

// 发送群消息
BOOL CQQClient::SendGroupMsg(UINT nToUin, LPCTSTR lpMsg, LPCTSTR lpFontName, 
							 int nFontSize, COLORREF clrText, BOOL bBold, 
							 BOOL bItalic, BOOL bUnderLine)
{
	if (!IsOnline())
		return FALSE;

	return m_SendMsgTask.AddGroupMsg(nToUin, lpMsg, lpFontName, 
		nFontSize, clrText, bBold, bItalic, bUnderLine);
}

// 发送临时会话消息
BOOL CQQClient::SendSessMsg(UINT nToUin, int nFace, LPCTSTR lpGroupSig, 
							LPCTSTR lpMsg, LPCTSTR lpFontName, int nFontSize, 
							COLORREF clrText, BOOL bBold, BOOL bItalic, BOOL bUnderLine)
{
	if (!IsOnline())
		return FALSE;

	return m_SendMsgTask.AddSessMsg(nToUin, nFace, lpGroupSig, lpMsg, 
		lpFontName, nFontSize, clrText, bBold, bItalic, bUnderLine);
}

// 是否在线状态
BOOL CQQClient::IsOnline()
{
	return (QQ_STATUS_OFFLINE == m_QQUser.m_nStatus) ? FALSE : TRUE;
}

// 获取在线状态
QQ_STATUS CQQClient::GetStatus()
{
	return m_QQUser.m_nStatus;
}

// 获取验证码图片
BOOL CQQClient::GetVerifyCodePic(const BYTE *& lpData, DWORD& dwSize)
{
	lpData = m_QQUser.m_VerifyCodePic.GetData();
	dwSize = m_QQUser.m_VerifyCodePic.GetSize();

	if (lpData != NULL && dwSize > 0)
	{
		return TRUE;
	}
	else
	{
		lpData = NULL;
		dwSize = 0;
		return FALSE;
	}
}

// 获取用户信息
CBuddyInfo * CQQClient::GetUserInfo()
{
	return &m_QQUser.m_UserInfo;
}

// 获取好友列表
CBuddyList * CQQClient::GetBuddyList()
{
	return &m_QQUser.m_BuddyList;
}

// 获取群列表
CGroupList * CQQClient::GetGroupList()
{
	return &m_QQUser.m_GroupList;
}

// 获取最近联系人列表
CRecentList * CQQClient::GetRecentList()
{
	return &m_QQUser.m_RecentList;
}

// 获取消息列表
CMessageList * CQQClient::GetMessageList()
{
	return &m_QQUser.m_MsgList;
}

tstring CQQClient::GetHeadPicFullName(UINT nGroupNum, UINT nQQNum)
{
	return m_QQUser.GetHeadPicFullName(nGroupNum, nQQNum);
}

tstring CQQClient::GetChatPicFullName(LPCTSTR lpszFileName)
{
	return m_QQUser.GetChatPicFullName(lpszFileName);
}

// 获取好友头像图片存放路径
tstring CQQClient::GetBuddyHeadPicPath()
{
	return m_QQUser.m_strHeadPicPath + _T("Buddy\\");
}

// 获取群头像图片存放路径
tstring CQQClient::GetGroupHeadPicPath()
{
	return m_QQUser.m_strHeadPicPath + _T("Group\\");
}

// 获取聊天图片存放路径
tstring CQQClient::GetChatPicPath()
{
	return m_QQUser.m_strChatPicPath;
}

// 设置聊天图片存放路径
void CQQClient::SetChatPicPath(LPCTSTR lpPath)
{
	if (lpPath != NULL)
		m_QQUser.m_strChatPicPath = lpPath;
}

void CQQClient::OnUpdateUserInfo(CBuddyInfoResult * lpBuddyInfoResult)
{
	if (NULL == lpBuddyInfoResult)
		return;

	m_QQUser.m_UserInfo.SetBuddyInfo(lpBuddyInfoResult);
}

void CQQClient::OnUpdateBuddyList(CBuddyListResult * lpBuddyListResult)
{
	if (NULL == lpBuddyListResult)
		return;

	for (int i = 0; i < (int)lpBuddyListResult->m_arrBuddyTeamInfo.size(); i++)
	{
		CBuddyTeamInfo * lpBuddyTeamInfo = lpBuddyListResult->m_arrBuddyTeamInfo[i];
		if (lpBuddyTeamInfo != NULL)
			m_QQUser.m_BuddyList.AddBuddyTeam(lpBuddyTeamInfo);
	}
	lpBuddyListResult->m_arrBuddyTeamInfo.clear();
}

void CQQClient::OnUpdateGroupList(CGroupListResult * lpGroupListResult)
{
	if (NULL == lpGroupListResult)
		return;

	for (int i = 0; i < (int)lpGroupListResult->m_arrGroupInfo.size(); i++)
	{
		CGroupInfo * lpGroupInfo = lpGroupListResult->m_arrGroupInfo[i];
		if (lpGroupInfo != NULL)
			m_QQUser.m_GroupList.AddGroup(lpGroupInfo);
	}
	lpGroupListResult->m_arrGroupInfo.clear();
}

void CQQClient::OnUpdateRecentList(CRecentListResult * lpRecentListResult)
{
	if (NULL == lpRecentListResult)
		return;

	for (int i = 0; i < (int)lpRecentListResult->m_arrRecentInfo.size(); i++)
	{
		CRecentInfo * lpRecentInfo = lpRecentListResult->m_arrRecentInfo[i];
		if (lpRecentInfo != NULL)
			m_QQUser.m_RecentList.AddRecent(lpRecentInfo);
	}
	lpRecentListResult->m_arrRecentInfo.clear();
}

void CQQClient::OnBuddyMsg(CBuddyMessage * lpBuddyMsg)
{
	if (NULL == lpBuddyMsg)
		return;

	m_QQUser.m_MsgList.AddMsg(QQ_MSG_TYPE_BUDDY, 
		lpBuddyMsg->m_nFromUin, 0, (void *)lpBuddyMsg);
}

void CQQClient::OnGroupMsg(CGroupMessage * lpGroupMsg)
{
	if (NULL == lpGroupMsg)
		return;

	m_QQUser.m_MsgList.AddMsg(QQ_MSG_TYPE_GROUP, 
		lpGroupMsg->m_nGroupCode, lpGroupMsg->m_nGroupCode, (void *)lpGroupMsg);
}

void CQQClient::OnSessMsg(CSessMessage * lpSessMsg)
{
	if (NULL == lpSessMsg)
		return;

	UINT nGroupCode = 0;

	CGroupInfo * lpGroupInfo = m_QQUser.m_GroupList.GetGroupById(lpSessMsg->m_nGroupId);
	if (lpGroupInfo != NULL)
	{
		nGroupCode = lpGroupInfo->m_nGroupCode;
		CBuddyInfo * lpBuddyInfo = lpGroupInfo->GetMemberByUin(lpSessMsg->m_nFromUin);
		if (NULL == lpBuddyInfo)
		{
			lpBuddyInfo = new CBuddyInfo;
			if (lpBuddyInfo != NULL)
			{
				lpBuddyInfo->Reset();
				lpBuddyInfo->m_nQQUin = lpSessMsg->m_nFromUin;
				lpBuddyInfo->m_nQQNum = lpSessMsg->m_nQQNum;
				lpGroupInfo->m_arrMember.push_back(lpBuddyInfo);
			}
			UpdateGroupMemberInfo(nGroupCode, lpSessMsg->m_nFromUin);
		}
	}

	m_QQUser.m_MsgList.AddMsg(QQ_MSG_TYPE_SESS, 
		lpSessMsg->m_nFromUin, nGroupCode, (void *)lpSessMsg);
}

void CQQClient::OnSysGroupMsg(CSysGroupMessage * lpSysGroupMsg)
{
	if (NULL == lpSysGroupMsg)
		return;

	m_QQUser.m_MsgList.AddMsg(QQ_MSG_TYPE_SYSGROUP, lpSysGroupMsg->m_nGroupCode, 
		lpSysGroupMsg->m_nGroupCode, (void *)lpSysGroupMsg);
}

void CQQClient::OnStatusChangeMsg(CStatusChangeMessage * lpStatusChangeMsg)
{
	if (NULL == lpStatusChangeMsg)
		return;

	CBuddyInfo * lpBuddyInfo = m_QQUser.m_BuddyList.GetBuddy(lpStatusChangeMsg->m_nQQUin);
	if (lpBuddyInfo != NULL)
	{
		lpBuddyInfo->m_nStatus = lpStatusChangeMsg->m_nStatus;
		lpBuddyInfo->m_nClientType = lpStatusChangeMsg->m_nClientType;
		CBuddyTeamInfo * lpBuddyTeamInfo = m_QQUser.m_BuddyList.GetBuddyTeam(lpBuddyInfo->m_nTeamIndex);
		if (lpBuddyTeamInfo != NULL)
			lpBuddyTeamInfo->Sort();
	}
}

void CQQClient::OnUpdateBuddyNumber(CGetQQNumResult * lpGetQQNumResult)
{
	if (NULL == lpGetQQNumResult)
		return;

	CBuddyInfo * lpBuddyInfo = m_QQUser.m_BuddyList.GetBuddy(lpGetQQNumResult->m_nQQUin);
	if (lpBuddyInfo != NULL)
		lpBuddyInfo->SetQQNum(lpGetQQNumResult);
}

void CQQClient::OnUpdateGMemberNumber(UINT nGroupCode, CGetQQNumResult * lpGetQQNumResult)
{
	if (NULL == lpGetQQNumResult)
		return;

	CBuddyInfo * lpBuddyInfo = m_QQUser.m_GroupList.GetGroupMember(nGroupCode, lpGetQQNumResult->m_nQQUin);
	if (lpBuddyInfo != NULL)
		lpBuddyInfo->SetQQNum(lpGetQQNumResult);
}

void CQQClient::OnUpdateGroupNumber(UINT nGroupCode, CGetQQNumResult * lpGetQQNumResult)
{
	if (NULL == lpGetQQNumResult)
		return;

	CGroupInfo * lpGroupInfo = m_QQUser.m_GroupList.GetGroupByCode(nGroupCode);
	if (lpGroupInfo != NULL)
		lpGroupInfo->SetGroupNumber(lpGetQQNumResult);
}

void CQQClient::OnUpdateBuddySign(CGetSignResult * lpGetSignResult)
{
	if (NULL == lpGetSignResult)
		return;

	if (m_QQUser.m_UserInfo.m_nQQUin == lpGetSignResult->m_nQQUin)
	{
		m_QQUser.m_UserInfo.SetQQSign(lpGetSignResult);
	}
	else
	{
		CBuddyInfo * lpBuddyInfo = m_QQUser.m_BuddyList.GetBuddy(lpGetSignResult->m_nQQUin);
		if (lpBuddyInfo != NULL)
			lpBuddyInfo->SetQQSign(lpGetSignResult);
	}
}

void CQQClient::OnUpdateGMemberSign(UINT nGroupCode, CGetSignResult * lpGetSignResult)
{
	if (NULL == lpGetSignResult)
		return;

	CBuddyInfo * lpBuddyInfo = m_QQUser.m_GroupList.GetGroupMember(nGroupCode, lpGetSignResult->m_nQQUin);
	if (lpBuddyInfo != NULL)
		lpBuddyInfo->SetQQSign(lpGetSignResult);
}

void CQQClient::OnUpdateBuddyInfo(CBuddyInfoResult * lpBuddyInfoResult)
{
	if (NULL == lpBuddyInfoResult)
		return;

	if (m_QQUser.m_UserInfo.m_nQQUin == lpBuddyInfoResult->m_nQQUin)
	{
		m_QQUser.m_UserInfo.SetBuddyInfo(lpBuddyInfoResult);
	}
	else
	{
		CBuddyInfo * lpBuddyInfo = m_QQUser.m_BuddyList.GetBuddy(lpBuddyInfoResult->m_nQQUin);
		if (lpBuddyInfo != NULL)
			lpBuddyInfo->SetBuddyInfo(lpBuddyInfoResult);
	}
}

void CQQClient::OnUpdateGMemberInfo(UINT nGroupCode, CBuddyInfoResult * lpBuddyInfoResult)
{
	if (NULL == lpBuddyInfoResult)
		return;

	CBuddyInfo * lpBuddyInfo = m_QQUser.m_GroupList.GetGroupMember(nGroupCode, lpBuddyInfoResult->m_nQQUin);
	if (lpBuddyInfo != NULL)
		lpBuddyInfo->SetBuddyInfo(lpBuddyInfoResult);
}

void CQQClient::OnUpdateGroupInfo(UINT nGroupCode, CGroupInfoResult * lpGroupInfoResult)
{
	if (NULL == lpGroupInfoResult)
		return;

	CGroupInfo * lpGroupInfo = m_QQUser.m_GroupList.GetGroupByCode(nGroupCode);
	if (lpGroupInfo != NULL)
		lpGroupInfo->SetGroupInfo(lpGroupInfoResult);
}

void CQQClient::OnUpdateC2CMsgSig(CGetC2CMsgSigResult * lpGetC2CMsgSigResult)
{
	if (NULL == lpGetC2CMsgSigResult)
		return;

	CGroupInfo * lpGroupInfo = m_QQUser.m_GroupList.GetGroupById(lpGetC2CMsgSigResult->m_nGroupId);
	if (lpGroupInfo != NULL)
	{
		CBuddyInfo * lpBuddyInfo = lpGroupInfo->GetMemberByUin(lpGetC2CMsgSigResult->m_nQQUin);
		if (lpBuddyInfo != NULL)
			lpBuddyInfo->m_strGroupSig = lpGetC2CMsgSigResult->m_strValue;
	}
}

void CQQClient::OnChangeStatusResult(BOOL bSuccess, QQ_STATUS nNewStatus)
{
	if (bSuccess)
		m_QQUser.m_nStatus = nNewStatus;
}