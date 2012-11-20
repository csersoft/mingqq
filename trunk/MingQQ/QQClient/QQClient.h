#pragma once

#include "QQUser.h"
#include "QQProtocol.h"
#include "LoginTask.h"
#include "LogoutTask.h"
#include "PollTask.h"
#include "SendMsgTask.h"
#include "InfoTask.h"
#include "GetC2CMsgSigTask.h"
#include "ChangeStatusTask.h"

class CQQClient
{
public:
	CQQClient(void);
	~CQQClient(void);

public:
	BOOL Init();										// 初始化客户端
	void UnInit();										// 反初始化客户端

	void SetUser(LPCTSTR lpQQNum, LPCTSTR lpQQPwd);		// 设置QQ号码和密码
	void SetLoginStatus(QQ_STATUS nStatus);				// 设置登录状态
	void SetCallBackWnd(HWND hCallBackWnd);				// 设置回调窗口句柄
	void SetVerifyCode(LPCTSTR lpVerifyCode);			// 设置验证码

	BOOL Login();										// 登录
	BOOL Logout();										// 注销

	void CancelLogin();									// 取消登录
	
	void ChangeStatus(QQ_STATUS nStatus);				// 改变在线状态
	void UpdateBuddyList();								// 更新好友列表
	void UpdateGroupList();								// 更新群列表
	void UpdateRecentList();							// 更新最近联系人列表
	void UpdateBuddyInfo(UINT nQQUin);					// 更新好友信息
	void UpdateGroupMemberInfo(UINT nGroupCode, UINT nQQUin);	// 更新群成员信息
	void UpdateGroupInfo(UINT nGroupCode);				// 更新群信息
	void UpdateBuddyNum(UINT nQQUin);					// 更新好友号码
	void UpdateGroupMemberNum(UINT nGroupCode, UINT nQQUin);	// 更新群成员号码
	void UpdateGroupMemberNum(UINT nGroupCode, std::vector<UINT> * arrQQUin);	// 更新群成员号码
	void UpdateGroupNum(UINT nGroupCode);				// 更新群号码
	void UpdateBuddySign(UINT nQQUin);					// 更新好友个性签名
	void UpdateGroupMemberSign(UINT nGroupCode, UINT nQQUin);	// 更新群成员个性签名
	void ModifyQQSign(LPCTSTR lpSign);					// 修改QQ个性签名
	void UpdateBuddyHeadPic(UINT nQQUin, UINT nQQNum);			// 更新好友头像
	void UpdateGroupMemberHeadPic(UINT nGroupCode, UINT nQQUin, UINT nGroupNum, UINT nQQNum);// 更新群成员头像
	void UpdateGroupHeadPic(UINT nGroupCode, UINT nGroupNum);	// 更新群头像
	void UpdateC2CMsgSignal(UINT nGroupId, UINT nQQUin);// 更新临时会话信令
	BOOL SendBuddyMsg(UINT nToUin, int nFace, LPCTSTR lpMsg,// 发送好友消息
		LPCTSTR lpFontName, int nFontSize, COLORREF clrText, 
		BOOL bBold, BOOL bItalic, BOOL bUnderLine);
	BOOL SendGroupMsg(UINT nToUin, LPCTSTR lpMsg,			// 发送群消息
		LPCTSTR lpFontName,	int nFontSize, COLORREF clrText, 
		BOOL bBold, BOOL bItalic, BOOL bUnderLine);
	BOOL SendSessMsg(UINT nToUin, int nFace, LPCTSTR lpGroupSig, // 发送临时会话消息
		LPCTSTR lpMsg, LPCTSTR lpFontName, int nFontSize, 
		COLORREF clrText, BOOL bBold, BOOL bItalic, BOOL bUnderLine);

	BOOL IsOnline();									// 是否在线状态
	QQ_STATUS GetStatus();								// 获取在线状态
	BOOL GetVerifyCodePic(const BYTE *& lpData, DWORD& dwSize);	// 获取验证码图片

	CBuddyInfo * GetUserInfo();							// 获取用户信息
	CBuddyList * GetBuddyList();						// 获取好友列表
	CGroupList * GetGroupList();						// 获取群列表
	CRecentList * GetRecentList();						// 获取最近联系人列表
	CMessageList * GetMessageList();					// 获取消息列表

	tstring GetHeadPicFullName(UINT nGroupNum, UINT nQQNum);
	tstring GetChatPicFullName(LPCTSTR lpszFileName);

	tstring GetBuddyHeadPicPath();						// 获取好友头像图片存放路径
	tstring GetGroupHeadPicPath();						// 获取群头像图片存放路径
	tstring GetChatPicPath();							// 获取聊天图片存放路径
	void SetChatPicPath(LPCTSTR lpPath);				// 设置聊天图片存放路径

	void OnUpdateUserInfo(CBuddyInfoResult * lpBuddyInfoResult);
	void OnUpdateBuddyList(CBuddyListResult * lpBuddyListResult);
	void OnUpdateGroupList(CGroupListResult * lpGroupListResult);
	void OnUpdateRecentList(CRecentListResult * lpRecentListResult);
	void OnBuddyMsg(CBuddyMessage * lpBuddyMsg);
	void OnGroupMsg(CGroupMessage * lpGroupMsg);
	void OnSessMsg(CSessMessage * lpSessMsg);
	void OnSysGroupMsg(CSysGroupMessage * lpSysGroupMsg);
	void OnStatusChangeMsg(CStatusChangeMessage * lpStatusChangeMsg);
	void OnUpdateBuddyNumber(CGetQQNumResult * lpGetQQNumResult);
	void OnUpdateGMemberNumber(UINT nGroupCode, CGetQQNumResult * lpGetQQNumResult);
	void OnUpdateGroupNumber(UINT nGroupCode, CGetQQNumResult * lpGetQQNumResult);
	void OnUpdateBuddySign(CGetSignResult * lpGetSignResult);
	void OnUpdateGMemberSign(UINT nGroupCode, CGetSignResult * lpGetSignResult);
	void OnUpdateBuddyInfo(CBuddyInfoResult * lpBuddyInfoResult);
	void OnUpdateGMemberInfo(UINT nGroupCode, CBuddyInfoResult * lpBuddyInfoResult);
	void OnUpdateGroupInfo(UINT nGroupCode, CGroupInfoResult * lpGroupInfoResult);
	void OnUpdateC2CMsgSig(CGetC2CMsgSigResult * lpGetC2CMsgSigResult);
	void OnChangeStatusResult(BOOL bSuccess, QQ_STATUS nNewStatus);

	BOOL CalcPwdHash(LPCTSTR lpQQPwd, LPCTSTR lpVerifyCode, 
		const CHAR * lpPtUin, int nPtUinLen, TCHAR * lpPwdHash, int nLen)
	{
		return m_QQProtocol.CalcPwdHash(lpQQPwd, lpVerifyCode, lpPtUin, nPtUinLen, lpPwdHash, nLen);
	}

private:
	CQQUser m_QQUser;
	CQQProtocol m_QQProtocol;
	CThreadPool m_ThreadPool;
	CLoginTask m_LoginTask;
	CLogoutTask m_LogoutTask;
	CPollTask m_PollTask;
	CSendMsgTask m_SendMsgTask;
};
