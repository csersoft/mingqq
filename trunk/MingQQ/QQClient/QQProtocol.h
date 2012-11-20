#pragma once

#include <vector>
#include <time.h>
#include "HttpClient.h"
#include "Buffer.h"
#include "QQUtils.h"
#include "MD5.h"
#include "QQProtocolData.h"

class CQQProtocol
{
public:
	CQQProtocol(void);
	~CQQProtocol(void);

public:
	BOOL CheckVerifyCode(CHttpClient& HttpClient, LPCTSTR lpQQNum,	// 检测是否需要输入验证码
		LPCTSTR lpAppId, CVerifyCodeInfo * lpVCInfo);
	BOOL GetVerifyCodePic(CHttpClient& HttpClient, LPCTSTR lpAppId,	// 获取验证码图片
		LPCTSTR lpQQNum, LPCTSTR lpVCType, CBuffer * lpVerifyCodePic);
	BOOL Login1(CHttpClient& HttpClient, LPCTSTR lpQQNum, LPCTSTR lpQQPwd,	// 第一次登录
		LPCTSTR lpVerifyCode, const CHAR * lpPtUin, int nPtUin, 
		LPCTSTR lpAppId, CLoginResult_1 * lpLoginResult1);
	BOOL Login2(CHttpClient& HttpClient, QQ_STATUS nQQStatus,		// 第二次登录
		LPCTSTR lpPtWebQq, LPCTSTR lpClientId, CLoginResult_2 * lpLoginResult2);
	BOOL Logout(CHttpClient& HttpClient, LPCTSTR lpClientId,		// 注销
		LPCTSTR lpPSessionId, CLogoutResult * lpLogoutResult);
	BOOL GetBuddyList(CHttpClient& HttpClient,						// 获取好友列表
		LPCTSTR lpVfWebQq, CBuddyListResult * lpBuddyListResult);
	BOOL GetOnlineBuddyList(CHttpClient& HttpClient, LPCTSTR lpClientId,	// 获取在线好友列表
		LPCTSTR lpPSessionId, COnlineBuddyListResult * lpOnlineBuddyListResult);
	BOOL GetGroupList(CHttpClient& HttpClient,						// 获取群列表
		LPCTSTR lpVfWebQq, CGroupListResult * lpGroupListResult);
	BOOL GetRecentList(CHttpClient& HttpClient, LPCTSTR lpVfWebQq,	// 获取最近联系人列表
		LPCTSTR lpClientId, LPCTSTR lpPSessionId, 
		CRecentListResult * lpRecentListResult);
	BOOL Poll(CHttpClient& HttpClient, LPCTSTR lpClientId,			// 轮询消息
		LPCTSTR lpPSessionId, CBuffer * lpPollResult);
	BOOL GetBuddyInfo(CHttpClient& HttpClient, UINT nQQUin,			// 获取好友信息
		LPCTSTR lpVfWebQq, CBuddyInfoResult * lpBuddyInfoResult);
	BOOL GetStrangerInfo(CHttpClient& HttpClient, UINT nQQUin,		// 获取陌生人信息
		LPCTSTR lpVfWebQq, CBuddyInfoResult * lpBuddyInfoResult);
	BOOL GetGroupInfo(CHttpClient& HttpClient, UINT nGroupCode,		// 获取群信息
		LPCTSTR lpVfWebQq, CGroupInfoResult * lpGroupInfoResult);
	BOOL GetQQNum(CHttpClient& HttpClient, BOOL bIsBuddy, UINT nQQUin,	// 获取好友、群成员或群号码
		LPCTSTR lpVfWebQq, CGetQQNumResult * lpGetQQNumResult);
	BOOL GetQQSign(CHttpClient& HttpClient, UINT nQQUin,			// 获取QQ个性签名
		LPCTSTR lpVfWebQq, CGetSignResult * lpGetSignResult);
	BOOL SetQQSign(CHttpClient& HttpClient, LPCTSTR lpSign,			// 设置QQ个性签名
		LPCTSTR lpVfWebQq, CSetSignResult * lpSetSignResult);
	BOOL SendBuddyMsg(CHttpClient& HttpClient, CBuddyMessage * lpBuddyMsg,	// 发送好友消息
		LPCTSTR lpClientId, LPCTSTR lpPSessionId, CSendBuddyMsgResult * lpSendBuddyMsgResult);
	BOOL SendGroupMsg(CHttpClient& HttpClient, CGroupMessage * lpGroupMsg,	// 发送群消息
		LPCTSTR lpClientId, LPCTSTR lpPSessionId, CSendGroupMsgResult * lpSendGroupMsgResult);
	BOOL SendSessMsg(CHttpClient& HttpClient, CSessMessage * lpSessMsg,	// 发送临时会话消息
		LPCTSTR lpGroupSig, LPCTSTR lpClientId, LPCTSTR lpPSessionId, 
		CSendSessMsgResult * lpSendSessMsgResult);
	BOOL GetHeadPic(CHttpClient& HttpClient, BOOL bIsBuddy,			// 获取头像图片
		UINT nQQUin, LPCTSTR lpVfWebQq, CBuffer * lpFacePic);
	BOOL GetBuddyChatPic(CHttpClient& HttpClient, UINT nMsgId,		// 获取好友聊天图片
		LPCTSTR lpFileName, UINT nQQUin, LPCTSTR lpClientId,
		LPCTSTR lpPSessionId, CBuffer * lpBuddyPic);
	BOOL GetGroupChatPic(CHttpClient& HttpClient, UINT nGroupId,	// 获取群聊天图片
		UINT nQQUin, LPCTSTR lpServer, int nPort, UINT nFileId, 
		LPCTSTR lpFileName, LPCTSTR lpVfWebQq, CBuffer * lpGroupPic);
	BOOL ChangeStatus(CHttpClient& HttpClient, QQ_STATUS nStatus,	// 改变状态
		LPCTSTR lpClientId, LPCTSTR lpPSessionId, CChangeStatusResult * lpChangeStatusResult);
	BOOL GetC2CMsgSignal(CHttpClient& HttpClient, UINT nGroupId,	// 获取临时会话信令
		UINT nToUin, LPCTSTR lpClientId, LPCTSTR lpPSessionId, 
		CGetC2CMsgSigResult * lpGetC2CMsgSigResult);

	BOOL CalcPwdHash(LPCTSTR lpQQPwd, LPCTSTR lpVerifyCode, 
		const CHAR * lpPtUin, int nPtUinLen, TCHAR * lpPwdHash, int nLen);

private:
	BOOL HttpReq(CHttpClient& HttpClient, LPCTSTR lpszUrl, 
		LPCTSTR lpszReqHeaders, const CHAR * lpPostData, DWORD dwPostDataLen, 
		DWORD& dwRespCode, std::vector<tstring>* arrRespHeader, CBuffer& RespData);
	BOOL HttpGet(CHttpClient& HttpClient, LPCTSTR lpszUrl, 
		LPCTSTR lpszReqHeaders, DWORD& dwRespCode, 
		std::vector<tstring>* arrRespHeader, CBuffer& RespData);
	BOOL HttpPost(CHttpClient& HttpClient, LPCTSTR lpszUrl, 
		LPCTSTR lpszReqHeaders, const CHAR * lpPostData, DWORD dwPostDataLen, 
		DWORD& dwRespCode, std::vector<tstring>* arrRespHeader, CBuffer& RespData);
	
	BOOL my_isalnum(unsigned char cChar);
	std::string EncodeData(const WCHAR * lpData, int nLen);
	std::wstring UnicodeToHexStr(const WCHAR * lpStr, BOOL bDblSlash);
};