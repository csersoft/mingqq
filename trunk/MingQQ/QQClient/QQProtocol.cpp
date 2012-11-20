#include "StdAfx.h"
#include "QQProtocol.h"

CQQProtocol::CQQProtocol(void)
{
}

CQQProtocol::~CQQProtocol(void)
{
}

// 检测是否需要输入验证码
BOOL CQQProtocol::CheckVerifyCode(CHttpClient& HttpClient, LPCTSTR lpQQNum, 
								  LPCTSTR lpAppId, CVerifyCodeInfo * lpVCInfo)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nReferer: http://ui.ptlogin2.qq.com/cgi-bin/login?target=self&style=5&mibao_css=m_webqq&appid=1003903&enable_qlogin=0&no_verifyimg=1&s_url=http%3A%2F%2Fweb.qq.com%2Floginproxy.html&f_url=loginerroralert&strong_login=1&login_state=10&t=20120201001\r\nAccept-Language: zh-cn\r\n");
	LPCTSTR lpFmt = _T("http://check.ptlogin2.qq.com/check?uin=%s&appid=%s&r=0.23017793586962337");
	TCHAR szUrl[1024] = {0};
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpQQNum || NULL == lpAppId || NULL == lpVCInfo)
		return FALSE;

	wsprintf(szUrl, lpFmt, lpQQNum, lpAppId);

	bRet = HttpGet(HttpClient, szUrl, lpszReqHeaders, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	bRet = lpVCInfo->Parse(&RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 获取验证码图片
BOOL CQQProtocol::GetVerifyCodePic(CHttpClient& HttpClient, LPCTSTR lpAppId, 
								   LPCTSTR lpQQNum, LPCTSTR lpVCType, CBuffer * lpVerifyCodePic)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nReferer: http://ui.ptlogin2.qq.com/cgi-bin/login?target=self&style=5&mibao_css=m_webqq&appid=1003903&enable_qlogin=0&no_verifyimg=1&s_url=http%3A%2F%2Fweb.qq.com%2Floginproxy.html&f_url=loginerroralert&strong_login=1&login_state=10&t=20120201001\r\nAccept-Language: zh-cn\r\n");
	LPCTSTR lpFmt = _T("http://captcha.qq.com/getimage?aid=%s&uin=%s&vc_type=%s");
	TCHAR szUrl[1024] = {0};
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpAppId || NULL == lpQQNum 
		|| NULL == lpVCType || NULL == lpVerifyCodePic)
		return FALSE;

	wsprintf(szUrl, lpFmt, lpAppId, lpQQNum, lpVCType);

	bRet = HttpGet(HttpClient, szUrl, lpszReqHeaders, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	lpVerifyCodePic->Release();
	bRet = lpVerifyCodePic->Add(RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 第一次登录
BOOL CQQProtocol::Login1(CHttpClient& HttpClient, LPCTSTR lpQQNum, LPCTSTR lpQQPwd,	
						 LPCTSTR lpVerifyCode, const CHAR * lpPtUin, int nPtUin, 
						 LPCTSTR lpAppId, CLoginResult_1 * lpLoginResult1)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nReferer: http://ui.ptlogin2.qq.com/cgi-bin/login?target=self&style=5&mibao_css=m_webqq&appid=1003903&enable_qlogin=0&no_verifyimg=1&s_url=http%3A%2F%2Fweb.qq.com%2Floginproxy.html&f_url=loginerroralert&strong_login=1&login_state=10&t=20120504001\r\nAccept-Language: zh-cn\r\n");
// 	LPCTSTR lpFmt = _T("http://ptlogin2.qq.com/login?u=%s&p=%s&verifycode=%s&\
// 		webqq_type=10&remember_uin=1&login2qq=1&aid=%s&u1=http%%3A%%2F%%2Fweb.qq.com\
// 		%%2Floginproxy.html%%3Flogin2qq%%3D1%%26webqq_type%%3D10&h=1&ptredirect=0&ptlang\
// 		=2052&from_ui=1&pttype=1&dumy=&fp=loginerroralert&action=3-10-15594&mibao_css=m_webqq");
	LPCTSTR lpFmt = _T("http://ptlogin2.qq.com/login?u=%s&p=%s&verifycode=%s\
		&webqq_type=10&remember_uin=0&login2qq=1&aid=%s&u1=http%%3A%%2F%%2Fweb.qq.com\
		%%2Floginproxy.html%%3Flogin2qq%%3D1%%26webqq_type%%3D10&h=1&ptredirect=0&ptlang\
		=2052&from_ui=1&pttype=1&dumy=&fp=loginerroralert&action=1-8-4593&mibao_css=m_webqq&t=1&g=1");
	TCHAR cPwdHash[64] = {0};
	TCHAR szUrl[1024] = {0};
	DWORD dwRespCode;
	std::vector<tstring> arrRespHeader;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpQQNum || NULL == lpQQPwd || NULL == lpVerifyCode 
		|| NULL == lpPtUin || nPtUin <= 0 || NULL == lpAppId || NULL == lpLoginResult1)
		return FALSE;
	
	CalcPwdHash(lpQQPwd, lpVerifyCode, lpPtUin, nPtUin, cPwdHash, sizeof(cPwdHash) / sizeof(TCHAR));

	wsprintf(szUrl, lpFmt, lpQQNum, cPwdHash, lpVerifyCode, lpAppId);

	bRet = HttpGet(HttpClient, szUrl, lpszReqHeaders, dwRespCode, &arrRespHeader, RespData);
	if (!bRet || dwRespCode != 200)
	{
		arrRespHeader.clear();
		return FALSE;
	}

	bRet = lpLoginResult1->Parse(&RespData, &arrRespHeader);
	if (!bRet)
	{
		arrRespHeader.clear();
		return FALSE;
	}

	arrRespHeader.clear();

	return TRUE;
}

// 第二次登录
BOOL CQQProtocol::Login2(CHttpClient& HttpClient, QQ_STATUS nQQStatus, 
						 LPCTSTR lpPtWebQq, LPCTSTR lpClientId, 
						 CLoginResult_2 * lpLoginResult2)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nAccept-Language: zh-cn\r\nReferer: http://d.web2.qq.com/proxy.html?v=20110331002&callback=2\r\nContent-Type: application/x-www-form-urlencoded\r\n");
	LPCTSTR lpszUrl = _T("http://d.web2.qq.com/channel/login2");
	LPCTSTR lpFmt1 = _T("{\"status\":\"%s\",\"ptwebqq\":\"%s\",\"passwd_sig\":\"\",\"clientid\":\"%s\",\"psessionid\":null}");
	LPCSTR lpFmt2 = "r=%s&clientid=%s&psessionid=null";
	LPCTSTR lpStatusStr;
	TCHAR cRData[2048] = {0};
	std::string strRData, strClientId;
	CHAR cPostData[4096] = {0};
	int nPostDataLen;
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpPtWebQq || NULL == lpClientId || NULL == lpLoginResult2)
		return FALSE;

	lpStatusStr = ConvertToQQStatusStr(nQQStatus);

	wsprintf(cRData, lpFmt1, lpStatusStr, lpPtWebQq, lpClientId);
	strRData = EncodeData(cRData, _tcslen(cRData));

	strClientId = EncodeData(lpClientId, _tcslen(lpClientId));

	sprintf(cPostData, lpFmt2, strRData.c_str(), strClientId.c_str());
	nPostDataLen = strlen(cPostData);

	bRet = HttpPost(HttpClient, lpszUrl, lpszReqHeaders, cPostData, 
		nPostDataLen, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	bRet = lpLoginResult2->Parse(&RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 注销
BOOL CQQProtocol::Logout(CHttpClient& HttpClient, LPCTSTR lpClientId, 
						 LPCTSTR lpPSessionId, CLogoutResult * lpLogoutResult)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nReferer: http://d.web2.qq.com/proxy.html?v=20110331002&callback=2\r\nAccept-Language: zh-cn\r\n");
	LPCTSTR lpFmt = _T("http://d.web2.qq.com/channel/logout2?ids=12916&clientid=%s&psessionid=%s&t=%u");
	TCHAR szUrl[1024] = {0};
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpClientId || NULL == lpPSessionId || NULL == lpLogoutResult)
		return FALSE;

	time_t t;
	t = time(NULL);

	wsprintf(szUrl, lpFmt, lpClientId, lpPSessionId, t);

	bRet = HttpGet(HttpClient, szUrl, lpszReqHeaders, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	bRet = lpLogoutResult->Parse(&RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 获取好友列表
BOOL CQQProtocol::GetBuddyList(CHttpClient& HttpClient, LPCTSTR lpVfWebQq, 
							   CBuddyListResult * lpBuddyListResult)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nAccept-Language: zh-cn\r\nReferer: http://s.web2.qq.com/proxy.html?v=20110412001&callback=1&id=2\r\nContent-Type: application/x-www-form-urlencoded\r\n");
	LPCTSTR lpszUrl = _T("http://s.web2.qq.com/api/get_user_friends2");
	LPCTSTR lpFmt1 = _T("{\"h\":\"hello\",\"vfwebqq\":\"%s\"}");
	LPCSTR lpFmt2 = "r=%s";
	TCHAR cRData[2048] = {0};
	std::string strRData;
	CHAR cPostData[4096] = {0};
	int nPostDataLen;
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpVfWebQq || NULL == lpBuddyListResult)
		return FALSE;

	wsprintf(cRData, lpFmt1, lpVfWebQq);
	strRData = EncodeData(cRData, _tcslen(cRData));

	sprintf(cPostData, lpFmt2, strRData.c_str());
	nPostDataLen = strlen(cPostData);

	bRet = HttpPost(HttpClient, lpszUrl, lpszReqHeaders, cPostData, 
		nPostDataLen, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	bRet = lpBuddyListResult->Parse(&RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 获取在线好友列表
BOOL CQQProtocol::GetOnlineBuddyList(CHttpClient& HttpClient, LPCTSTR lpClientId, 
									 LPCTSTR lpPSessionId, COnlineBuddyListResult * lpOnlineBuddyListResult)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nReferer: http://d.web2.qq.com/proxy.html?v=20110331002&callback=2\r\nAccept-Language: zh-cn\r\n");
	LPCTSTR lpFmt = _T("http://d.web2.qq.com/channel/get_online_buddies2?clientid=%s&psessionid=%s&t=%u");
	TCHAR szUrl[1024] = {0};
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpClientId || NULL == lpPSessionId || NULL == lpOnlineBuddyListResult)
		return FALSE;

	time_t t;
	t = time(NULL);

	wsprintf(szUrl, lpFmt, lpClientId, lpPSessionId, t);

	bRet = HttpGet(HttpClient, szUrl, lpszReqHeaders, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	bRet = lpOnlineBuddyListResult->Parse(&RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 获取群列表
BOOL CQQProtocol::GetGroupList(CHttpClient& HttpClient, LPCTSTR lpVfWebQq, 
							   CGroupListResult * lpGroupListResult)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nAccept-Language: zh-cn\r\nReferer: http://s.web2.qq.com/proxy.html?v=20110412001&callback=1&id=2\r\nContent-Type: application/x-www-form-urlencoded\r\n");
	LPCTSTR lpszUrl = _T("http://s.web2.qq.com/api/get_group_name_list_mask2");
	LPCTSTR lpFmt1 = _T("{\"vfwebqq\":\"%s\"}");
	LPCSTR lpFmt2 = "r=%s";
	TCHAR cRData[2048] = {0};
	std::string strRData;
	CHAR cPostData[4096] = {0};
	int nPostDataLen;
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpVfWebQq || NULL == lpGroupListResult)
		return FALSE;

	wsprintf(cRData, lpFmt1, lpVfWebQq);
	strRData = EncodeData(cRData, _tcslen(cRData));

	sprintf(cPostData, lpFmt2, strRData.c_str());
	nPostDataLen = strlen(cPostData);

	bRet = HttpPost(HttpClient, lpszUrl, lpszReqHeaders, cPostData, 
		nPostDataLen, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	bRet = lpGroupListResult->Parse(&RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 获取最近联系人列表
BOOL CQQProtocol::GetRecentList(CHttpClient& HttpClient, LPCTSTR lpVfWebQq, 
								LPCTSTR lpClientId, LPCTSTR lpPSessionId, 
								CRecentListResult * lpRecentListResult)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nAccept-Language: zh-cn\r\nReferer: http://d.web2.qq.com/proxy.html?v=20110331002&callback=2\r\nContent-Type: application/x-www-form-urlencoded\r\n");
	LPCTSTR lpszUrl = _T("http://d.web2.qq.com/channel/get_recent_list2");
	LPCTSTR lpFmt1 = _T("{\"vfwebqq\":\"%s\",\"clientid\":\"%s\",\"psessionid\":\"%s\"}");
	LPCSTR lpFmt2 = "r=%s&clientid=%s&psessionid=%s";
	TCHAR cRData[2048] = {0};
	std::string strRData, strClientId, strPSessionId;
	CHAR cPostData[4096] = {0};
	int nPostDataLen;
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpVfWebQq || NULL == lpClientId || 
		NULL == lpPSessionId || NULL == lpRecentListResult)
		return FALSE;

	wsprintf(cRData, lpFmt1, lpVfWebQq, lpClientId, lpPSessionId);
	strRData = EncodeData(cRData, _tcslen(cRData));

	strClientId = EncodeData(lpClientId, _tcslen(lpClientId));
	strPSessionId = EncodeData(lpPSessionId, _tcslen(lpPSessionId));
	
	sprintf(cPostData, lpFmt2, strRData.c_str(), strClientId.c_str(), strPSessionId.c_str());
	nPostDataLen = strlen(cPostData);

	bRet = HttpPost(HttpClient, lpszUrl, lpszReqHeaders, cPostData, 
		nPostDataLen, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	bRet = lpRecentListResult->Parse(&RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 轮询消息
BOOL CQQProtocol::Poll(CHttpClient& HttpClient, LPCTSTR lpClientId,	
					   LPCTSTR lpPSessionId, CBuffer * lpPollResult)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nAccept-Language: zh-cn\r\nReferer: http://d.web2.qq.com/proxy.html?v=20110331002&callback=2\r\nContent-Type: application/x-www-form-urlencoded\r\n");
	LPCTSTR lpszUrl = _T("http://d.web2.qq.com/channel/poll2");
	LPCTSTR lpFmt1 = _T("{\"clientid\":\"%s\",\"psessionid\":\"%s\",\"key\":0,\"ids\":[]}");
	LPCSTR lpFmt2 = "r=%s&clientid=%s&psessionid=%s";
	TCHAR cRData[2048] = {0};
	std::string strRData, strClientId, strPSessionId;
	CHAR cPostData[4096] = {0};
	int nPostDataLen;
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpClientId || NULL == lpPSessionId || NULL == lpPollResult)
		return FALSE;

	wsprintf(cRData, lpFmt1, lpClientId, lpPSessionId);
	strRData = EncodeData(cRData, _tcslen(cRData));

	strClientId = EncodeData(lpClientId, _tcslen(lpClientId));
	strPSessionId = EncodeData(lpPSessionId, _tcslen(lpPSessionId));
	
	sprintf(cPostData, lpFmt2, strRData.c_str(), strClientId.c_str(), strPSessionId.c_str());
	nPostDataLen = strlen(cPostData);

	bRet = HttpPost(HttpClient, lpszUrl, lpszReqHeaders, cPostData, 
		nPostDataLen, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	lpPollResult->Release();
	bRet = lpPollResult->Add(RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 获取好友信息
BOOL CQQProtocol::GetBuddyInfo(CHttpClient& HttpClient, UINT nQQUin, 
							   LPCTSTR lpVfWebQq, CBuddyInfoResult * lpBuddyInfoResult)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nReferer: http://s.web2.qq.com/proxy.html?v=20110412001&callback=1&id=2\r\nAccept-Language: zh-cn\r\n");
	LPCTSTR lpFmt = _T("http://s.web2.qq.com/api/get_friend_info2?tuin=%u&verifysession=&code=&vfwebqq=%s&t=%u");
	TCHAR szUrl[1024] = {0};
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpVfWebQq || NULL == lpBuddyInfoResult)
		return FALSE;

	time_t t;
	t = time(NULL);

	wsprintf(szUrl, lpFmt, nQQUin, lpVfWebQq, t);

	bRet = HttpGet(HttpClient, szUrl, lpszReqHeaders, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	bRet = lpBuddyInfoResult->Parse(&RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 获取陌生人信息
BOOL CQQProtocol::GetStrangerInfo(CHttpClient& HttpClient, UINT nQQUin,	
								  LPCTSTR lpVfWebQq, CBuddyInfoResult * lpBuddyInfoResult)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nReferer: http://s.web2.qq.com/proxy.html?v=20110412001&callback=1&id=2\r\nAccept-Language: zh-cn\r\n");
	LPCTSTR lpFmt = _T("http://s.web2.qq.com/api/get_stranger_info2?tuin=%u&verifysession=&gid=0&code=&vfwebqq=%s&t=%u");
	TCHAR szUrl[1024] = {0};
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpVfWebQq || NULL == lpBuddyInfoResult)
		return FALSE;

	time_t t;
	t = time(NULL);

	wsprintf(szUrl, lpFmt, nQQUin, lpVfWebQq, t);

	bRet = HttpGet(HttpClient, szUrl, lpszReqHeaders, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	bRet = lpBuddyInfoResult->Parse(&RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 获取群信息
BOOL CQQProtocol::GetGroupInfo(CHttpClient& HttpClient, UINT nGroupCode,
							   LPCTSTR lpVfWebQq, CGroupInfoResult * lpGroupInfoResult)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nReferer: http://s.web2.qq.com/proxy.html?v=20110412001&callback=1&id=2\r\nAccept-Language: zh-cn\r\n");
	LPCTSTR lpFmt = _T("http://s.web2.qq.com/api/get_group_info_ext2?gcode=%u&vfwebqq=%s&t=%u");
	TCHAR szUrl[1024] = {0};
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpVfWebQq || NULL == lpGroupInfoResult)
		return FALSE;

	time_t t;
	t = time(NULL);

	wsprintf(szUrl, lpFmt, nGroupCode, lpVfWebQq, t);

	bRet = HttpGet(HttpClient, szUrl, lpszReqHeaders, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	bRet = lpGroupInfoResult->Parse(&RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 获取好友、群成员或群号码
BOOL CQQProtocol::GetQQNum(CHttpClient& HttpClient, BOOL bIsBuddy, UINT nQQUin, 
						   LPCTSTR lpVfWebQq, CGetQQNumResult * lpGetQQNumResult)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nReferer: http://s.web2.qq.com/proxy.html?v=20110412001&callback=1&id=2\r\nAccept-Language: zh-cn\r\n");
	LPCTSTR lpFmt = _T("http://s.web2.qq.com/api/get_friend_uin2?tuin=%u&verifysession=&type=%d&code=&vfwebqq=%s&t=%u");
	TCHAR szUrl[1024] = {0};
	DWORD dwRespCode;
	CBuffer RespData;
	int nType;
	time_t t;
	BOOL bRet;

	if (NULL == lpVfWebQq || NULL == lpGetQQNumResult)
		return FALSE;

	nType = bIsBuddy ? 1 : 4;
	t = time(NULL);

	wsprintf(szUrl, lpFmt, nQQUin, nType, lpVfWebQq, t);

	bRet = HttpGet(HttpClient, szUrl, lpszReqHeaders, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	bRet = lpGetQQNumResult->Parse(&RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 获取QQ个性签名
BOOL CQQProtocol::GetQQSign(CHttpClient& HttpClient, UINT nQQUin,
							LPCTSTR lpVfWebQq, CGetSignResult * lpGetSignResult)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nReferer: http://s.web2.qq.com/proxy.html?v=20110412001&callback=1&id=2\r\nAccept-Language: zh-cn\r\n");
	LPCTSTR lpFmt = _T("http://s.web2.qq.com/api/get_single_long_nick2?tuin=%u&vfwebqq=%s&t=%u");
	TCHAR szUrl[1024] = {0};
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpVfWebQq || NULL == lpGetSignResult)
		return FALSE;

	time_t t;
	t = time(NULL);

	wsprintf(szUrl, lpFmt, nQQUin, lpVfWebQq, t);

	bRet = HttpGet(HttpClient, szUrl, lpszReqHeaders, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	bRet = lpGetSignResult->Parse(&RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 设置QQ个性签名
BOOL CQQProtocol::SetQQSign(CHttpClient& HttpClient, LPCTSTR lpSign, 
							LPCTSTR lpVfWebQq, CSetSignResult * lpSetSignResult)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nAccept-Language: zh-cn\r\nReferer: http://s.web2.qq.com/proxy.html?v=20110412001&callback=1&id=2\r\nContent-Type: application/x-www-form-urlencoded\r\n");
	LPCTSTR lpszUrl = _T("http://s.web2.qq.com/api/set_long_nick2");
	LPCTSTR lpFmt1 = _T("{\"nlk\":\"%s\",\"vfwebqq\":\"%s\"}");
	LPCSTR lpFmt2 = "r=%s";
	TCHAR cRData[2048] = {0};
	std::string strRData;
	CHAR cPostData[4096] = {0};
	int nPostDataLen;
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpSign || NULL == lpVfWebQq || NULL == lpSetSignResult)
		return FALSE;

	wsprintf(cRData, lpFmt1, UnicodeToHexStr(lpSign, FALSE).c_str(), lpVfWebQq);
	strRData = EncodeData(cRData, _tcslen(cRData));

	sprintf(cPostData, lpFmt2, strRData.c_str());
	nPostDataLen = strlen(cPostData);

	bRet = HttpPost(HttpClient, lpszUrl, lpszReqHeaders, cPostData, 
		nPostDataLen, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	bRet = lpSetSignResult->Parse(&RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 发送好友消息
BOOL CQQProtocol::SendBuddyMsg(CHttpClient& HttpClient, CBuddyMessage * lpBuddyMsg,	
							   LPCTSTR lpClientId, LPCTSTR lpPSessionId, 
							   CSendBuddyMsgResult * lpSendBuddyMsgResult)
{
 	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nAccept-Language: zh-cn\r\nReferer: http://d.web2.qq.com/proxy.html?v=20110331002&callback=2\r\nContent-Type: application/x-www-form-urlencoded\r\n");
 	LPCTSTR lpszUrl = _T("http://d.web2.qq.com/channel/send_buddy_msg2");
	LPCTSTR lpFontFmt = _T("[\\\"font\\\",{\\\"name\\\":\\\"%s\\\",\\\"size\\\":\\\"%d\\\",\\\"style\\\":[%d,%d,%d],\\\"color\\\":\\\"%s\\\"}]");
 	LPCSTR lpFmt2 = "r=%s&clientid=%s&psessionid=%s";
 	TCHAR cRData[2048] = {0};
 	std::string strRData, strClientId, strPSessionId;
 	CHAR cPostData[4096] = {0};
 	int nPostDataLen;
 	DWORD dwRespCode;
 	CBuffer RespData;
 	BOOL bRet;
 
 	if (NULL == lpBuddyMsg || NULL == lpClientId
		|| NULL == lpPSessionId || NULL == lpSendBuddyMsgResult)
 		return FALSE;
 
	strClientId = EncodeData(lpClientId, _tcslen(lpClientId));
	strPSessionId = EncodeData(lpPSessionId, _tcslen(lpPSessionId));

	std::wstring strData, strContent;
	std::wstring strFontName, strColor;
	WCHAR cBuf[1024] = {0};

	for (int i = 0; i < (int)lpBuddyMsg->m_arrContent.size(); i++)
	{
		CContent * lpContent = lpBuddyMsg->m_arrContent[i];
		if (lpContent != NULL)
		{
			if (lpContent->m_nType == CONTENT_TYPE_TEXT)
			{
				strContent += _T("\\\"");
				strContent += UnicodeToHexStr(lpContent->m_strText.c_str(), TRUE);
				strContent += _T("\\\",");
			}
			else if (lpContent->m_nType == CONTENT_TYPE_FONT_INFO)
			{
				strFontName = UnicodeToHexStr(lpContent->m_FontInfo.m_strName.c_str(), TRUE);
				strColor = RGBToHexStr(lpContent->m_FontInfo.m_clrText);

				memset(cBuf, 0, sizeof(cBuf));
				wsprintf(cBuf, lpFontFmt, strFontName.c_str(), lpContent->m_FontInfo.m_nSize,
					lpContent->m_FontInfo.m_bBold, lpContent->m_FontInfo.m_bItalic,
					lpContent->m_FontInfo.m_bUnderLine, strColor.c_str());
				strContent += cBuf;
			}
			else if (lpContent->m_nType == CONTENT_TYPE_FACE)
			{
				memset(cBuf, 0, sizeof(cBuf));
				wsprintf(cBuf, _T("[\\\"face\\\",%d],"), lpContent->m_nFaceId);
				strContent += cBuf;
			}
		}
	}

	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("{\"to\":%u,"), lpBuddyMsg->m_nToUin);
	strData += cBuf;
	
	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("\"face\":%d,"), 0);
	strData += cBuf;
	
	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("\"content\":\"[%s]\","), strContent.c_str());
	strData += cBuf;

	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("\"msg_id\":%u,"), lpBuddyMsg->m_nMsgId);
	strData += cBuf;

	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("\"clientid\":\"%s\","), lpClientId);
	strData += cBuf;

	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("\"psessionid\":\"%s\"}"), lpPSessionId);
	strData += cBuf;

 	strRData = EncodeData(strData.c_str(), strData.size());
  	
 	sprintf(cPostData, lpFmt2, strRData.c_str(), strClientId.c_str(), strPSessionId.c_str());
 	nPostDataLen = strlen(cPostData);
 
 	bRet = HttpPost(HttpClient, lpszUrl, lpszReqHeaders, cPostData, 
 		nPostDataLen, dwRespCode, NULL, RespData);
 	if (!bRet || dwRespCode != 200)
 		return FALSE;
 
 	bRet = lpSendBuddyMsgResult->Parse(&RespData);
 	if (!bRet)
 		return FALSE;

	return TRUE;
}

// 发送群消息
BOOL CQQProtocol::SendGroupMsg(CHttpClient& HttpClient, CGroupMessage * lpGroupMsg,	
							   LPCTSTR lpClientId, LPCTSTR lpPSessionId, 
							   CSendGroupMsgResult * lpSendGroupMsgResult)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nAccept-Language: zh-cn\r\nReferer: http://d.web2.qq.com/proxy.html?v=20110331002&callback=2\r\nContent-Type: application/x-www-form-urlencoded\r\n");
 	LPCTSTR lpszUrl = _T("http://d.web2.qq.com/channel/send_qun_msg2");
	LPCTSTR lpFontFmt = _T("[\\\"font\\\",{\\\"name\\\":\\\"%s\\\",\\\"size\\\":\\\"%d\\\",\\\"style\\\":[%d,%d,%d],\\\"color\\\":\\\"%s\\\"}]");
 	LPCSTR lpFmt2 = "r=%s&clientid=%s&psessionid=%s";
 	TCHAR cRData[2048] = {0};
 	std::string strRData, strClientId, strPSessionId;
 	CHAR cPostData[4096] = {0};
 	int nPostDataLen;
 	DWORD dwRespCode;
 	CBuffer RespData;
 	BOOL bRet;
 
 	if (NULL == lpGroupMsg || NULL == lpClientId
		|| NULL == lpPSessionId || NULL == lpSendGroupMsgResult)
 		return FALSE;
 
	strClientId = EncodeData(lpClientId, _tcslen(lpClientId));
	strPSessionId = EncodeData(lpPSessionId, _tcslen(lpPSessionId));

	std::wstring strData, strContent;
	std::wstring strFontName, strColor;
	WCHAR cBuf[1024] = {0};

	for (int i = 0; i < (int)lpGroupMsg->m_arrContent.size(); i++)
	{
		CContent * lpContent = lpGroupMsg->m_arrContent[i];
		if (lpContent != NULL)
		{
			if (lpContent->m_nType == CONTENT_TYPE_TEXT)
			{
				strContent += _T("\\\"");
				strContent += UnicodeToHexStr(lpContent->m_strText.c_str(), TRUE);
				strContent += _T("\\\",");
			}
			else if (lpContent->m_nType == CONTENT_TYPE_FONT_INFO)
			{
				strFontName = UnicodeToHexStr(lpContent->m_FontInfo.m_strName.c_str(), TRUE);
				strColor = RGBToHexStr(lpContent->m_FontInfo.m_clrText);

				memset(cBuf, 0, sizeof(cBuf));
				wsprintf(cBuf, lpFontFmt, strFontName.c_str(), lpContent->m_FontInfo.m_nSize,
					lpContent->m_FontInfo.m_bBold, lpContent->m_FontInfo.m_bItalic,
					lpContent->m_FontInfo.m_bUnderLine, strColor.c_str());
				strContent += cBuf;
			}
			else if (lpContent->m_nType == CONTENT_TYPE_FACE)
			{
				memset(cBuf, 0, sizeof(cBuf));
				wsprintf(cBuf, _T("[\\\"face\\\",%d],"), lpContent->m_nFaceId);
				strContent += cBuf;
			}
		}
	}

	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("{\"group_uin\":%u,"), lpGroupMsg->m_nToUin);
	strData += cBuf;
		
	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("\"content\":\"[%s]\","), strContent.c_str());
	strData += cBuf;

	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("\"msg_id\":%u,"), lpGroupMsg->m_nMsgId);
	strData += cBuf;

	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("\"clientid\":\"%s\","), lpClientId);
	strData += cBuf;

	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("\"psessionid\":\"%s\"}"), lpPSessionId);
	strData += cBuf;

 	strRData = EncodeData(strData.c_str(), strData.size());
  	
 	sprintf(cPostData, lpFmt2, strRData.c_str(), strClientId.c_str(), strPSessionId.c_str());
 	nPostDataLen = strlen(cPostData);
 
 	bRet = HttpPost(HttpClient, lpszUrl, lpszReqHeaders, cPostData, 
 		nPostDataLen, dwRespCode, NULL, RespData);
 	if (!bRet || dwRespCode != 200)
 		return FALSE;
 
 	bRet = lpSendGroupMsgResult->Parse(&RespData);
 	if (!bRet)
 		return FALSE;

	return TRUE;
}

// 发送临时会话消息
BOOL CQQProtocol::SendSessMsg(CHttpClient& HttpClient, CSessMessage * lpSessMsg,
							  LPCTSTR lpGroupSig, LPCTSTR lpClientId, LPCTSTR lpPSessionId, 
							  CSendSessMsgResult * lpSendSessMsgResult)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nAccept-Language: zh-cn\r\nReferer: http://d.web2.qq.com/proxy.html?v=20110331002&callback=2\r\nContent-Type: application/x-www-form-urlencoded\r\n");
 	LPCTSTR lpszUrl = _T("http://d.web2.qq.com/channel/send_sess_msg2");
	LPCTSTR lpFontFmt = _T("[\\\"font\\\",{\\\"name\\\":\\\"%s\\\",\\\"size\\\":\\\"%d\\\",\\\"style\\\":[%d,%d,%d],\\\"color\\\":\\\"%s\\\"}]");
 	LPCSTR lpFmt2 = "r=%s&clientid=%s&psessionid=%s";
 	TCHAR cRData[2048] = {0};
 	std::string strRData, strClientId, strPSessionId;
 	CHAR cPostData[4096] = {0};
 	int nPostDataLen;
 	DWORD dwRespCode;
 	CBuffer RespData;
 	BOOL bRet;
 
 	if (NULL == lpSessMsg || NULL == lpGroupSig || NULL == lpClientId
		|| NULL == lpPSessionId || NULL == lpSendSessMsgResult)
 		return FALSE;
 
	strClientId = EncodeData(lpClientId, _tcslen(lpClientId));
	strPSessionId = EncodeData(lpPSessionId, _tcslen(lpPSessionId));

	std::wstring strData, strContent;
	std::wstring strFontName, strColor;
	WCHAR cBuf[1024] = {0};

	for (int i = 0; i < (int)lpSessMsg->m_arrContent.size(); i++)
	{
		CContent * lpContent = lpSessMsg->m_arrContent[i];
		if (lpContent != NULL)
		{
			if (lpContent->m_nType == CONTENT_TYPE_TEXT)
			{
				strContent += _T("\\\"");
				strContent += UnicodeToHexStr(lpContent->m_strText.c_str(), TRUE);
				strContent += _T("\\\",");
			}
			else if (lpContent->m_nType == CONTENT_TYPE_FONT_INFO)
			{
				strFontName = UnicodeToHexStr(lpContent->m_FontInfo.m_strName.c_str(), TRUE);
				strColor = RGBToHexStr(lpContent->m_FontInfo.m_clrText);

				memset(cBuf, 0, sizeof(cBuf));
				wsprintf(cBuf, lpFontFmt, strFontName.c_str(), lpContent->m_FontInfo.m_nSize,
					lpContent->m_FontInfo.m_bBold, lpContent->m_FontInfo.m_bItalic,
					lpContent->m_FontInfo.m_bUnderLine, strColor.c_str());
				strContent += cBuf;
			}
			else if (lpContent->m_nType == CONTENT_TYPE_FACE)
			{
				memset(cBuf, 0, sizeof(cBuf));
				wsprintf(cBuf, _T("[\\\"face\\\",%d],"), lpContent->m_nFaceId);
				strContent += cBuf;
			}
		}
	}

	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("{\"to\":%u,"), lpSessMsg->m_nToUin);
	strData += cBuf;
	
	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("\"group_sig\":\"%s\","), lpGroupSig);
	strData += cBuf;

	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("\"face\":%d,"), 0);
	strData += cBuf;
	
	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("\"content\":\"[%s]\","), strContent.c_str());
	strData += cBuf;

	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("\"msg_id\":%u,"), lpSessMsg->m_nMsgId);
	strData += cBuf;

	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("\"service_type\":%u,"), 0);
	strData += cBuf;

	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("\"clientid\":\"%s\","), lpClientId);
	strData += cBuf;

	memset(cBuf, 0, sizeof(cBuf));
	wsprintf(cBuf, _T("\"psessionid\":\"%s\"}"), lpPSessionId);
	strData += cBuf;

 	strRData = EncodeData(strData.c_str(), strData.size());
  	
 	sprintf(cPostData, lpFmt2, strRData.c_str(), strClientId.c_str(), strPSessionId.c_str());
 	nPostDataLen = strlen(cPostData);
 
 	bRet = HttpPost(HttpClient, lpszUrl, lpszReqHeaders, cPostData, 
 		nPostDataLen, dwRespCode, NULL, RespData);
 	if (!bRet || dwRespCode != 200)
 		return FALSE;
 
 	bRet = lpSendSessMsgResult->Parse(&RespData);
 	if (!bRet)
 		return FALSE;

	return TRUE;
}

// 获取头像图片
BOOL CQQProtocol::GetHeadPic(CHttpClient& HttpClient, BOOL bIsBuddy,
						  UINT nQQUin, LPCTSTR lpVfWebQq, CBuffer * lpFacePic)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nReferer: http://web.qq.com/?ADTAG=DESKTOP\r\nAccept-Language: zh-cn\r\n");
	LPCTSTR lpFmt = _T("http://face%d.qun.qq.com/cgi/svr/face/getface?cache=0&type=%d&fid=0&uin=%u&vfwebqq=%s");
	TCHAR szUrl[1024] = {0};
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpVfWebQq || NULL == lpFacePic)
		return FALSE;

	int nType = bIsBuddy ? 1 : 4;

	srand(time(NULL));
	int nRandom = rand() % 9 + 1;

	wsprintf(szUrl, lpFmt, nRandom, nType, nQQUin, lpVfWebQq);

	bRet = HttpGet(HttpClient, szUrl, lpszReqHeaders, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	lpFacePic->Release();
	bRet = lpFacePic->Add(RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 获取好友聊天图片
BOOL CQQProtocol::GetBuddyChatPic(CHttpClient& HttpClient, UINT nMsgId,
							  LPCTSTR lpFileName, UINT nQQUin, LPCTSTR lpClientId,
							  LPCTSTR lpPSessionId, CBuffer * lpBuddyPic)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nReferer: http://web.qq.com/?ADTAG=DESKTOP\r\nAccept-Language: zh-cn\r\n");
	LPCTSTR lpFmt = _T("http://d.web2.qq.com/channel/get_cface2?lcid=%u&guid=%s&to=%u&count=5&time=1&clientid=%s&psessionid=%s");
	TCHAR szUrl[1024] = {0};
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpFileName || NULL == lpClientId 
		|| NULL == lpPSessionId || NULL == lpBuddyPic)
		return FALSE;

	wsprintf(szUrl, lpFmt, nMsgId, lpFileName, nQQUin, lpClientId, lpPSessionId);

	bRet = HttpGet(HttpClient, szUrl, lpszReqHeaders, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	lpBuddyPic->Release();
	bRet = lpBuddyPic->Add(RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 获取群聊天图片
BOOL CQQProtocol::GetGroupChatPic(CHttpClient& HttpClient, UINT nGroupId,
							  UINT nQQUin, LPCTSTR lpServer, int nPort, UINT nFileId, 
							  LPCTSTR lpFileName, LPCTSTR lpVfWebQq, CBuffer * lpGroupPic)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nReferer: http://web.qq.com/?ADTAG=DESKTOP\r\nAccept-Language: zh-cn\r\n");
	LPCTSTR lpFmt = _T("http://web.qq.com/cgi-bin/get_group_pic?type=0&gid=%u&uin=%u&rip=%s&rport=%d&fid=%u&pic=%s&vfwebqq=%s&t=%u");
	TCHAR szUrl[1024] = {0};
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpServer || NULL == lpFileName 
		|| NULL == lpVfWebQq || NULL == lpGroupPic)
		return FALSE;

	wsprintf(szUrl, lpFmt, nGroupId, nQQUin, 
		lpServer, nPort, nFileId, lpFileName, lpVfWebQq);

	bRet = HttpGet(HttpClient, szUrl, lpszReqHeaders, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	lpGroupPic->Release();
	bRet = lpGroupPic->Add(RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 改变状态
BOOL CQQProtocol::ChangeStatus(CHttpClient& HttpClient, QQ_STATUS nStatus,
							   LPCTSTR lpClientId, LPCTSTR lpPSessionId, 
							   CChangeStatusResult * lpChangeStatusResult)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nReferer: http://d.web2.qq.com/proxy.html?v=20110331002&callback=2\r\nAccept-Language: zh-cn\r\n");
	LPCTSTR lpFmt = _T("http://d.web2.qq.com/channel/change_status2?newstatus=%s&clientid=%s&psessionid=%s&t=%u");
	TCHAR szUrl[1024] = {0};
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpClientId || NULL == lpPSessionId 
		|| NULL == lpChangeStatusResult)
		return FALSE;

	time_t t;
	t = time(NULL);

	LPCTSTR lpStatusStr = ConvertToQQStatusStr(nStatus);

	wsprintf(szUrl, lpFmt, lpStatusStr, lpClientId, lpPSessionId, t);

	bRet = HttpGet(HttpClient, szUrl, lpszReqHeaders, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	bRet = lpChangeStatusResult->Parse(&RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

// 获取临时会话信令
BOOL CQQProtocol::GetC2CMsgSignal(CHttpClient& HttpClient, UINT nGroupId,
								  UINT nToUin, LPCTSTR lpClientId, LPCTSTR lpPSessionId, 
								  CGetC2CMsgSigResult * lpGetC2CMsgSigResult)
{
	LPCTSTR lpszReqHeaders = _T("Accept: */*\r\nReferer: http://d.web2.qq.com/proxy.html?v=20110331002&callback=2\r\nAccept-Language: zh-cn\r\n");
	LPCTSTR lpFmt = _T("http://d.web2.qq.com/channel/get_c2cmsg_sig2?id=%u&to_uin=%u&service_type=0&clientid=%s&psessionid=%s&t=%u");
	TCHAR szUrl[1024] = {0};
	DWORD dwRespCode;
	CBuffer RespData;
	BOOL bRet;

	if (NULL == lpClientId || NULL == lpPSessionId 
		|| NULL == lpGetC2CMsgSigResult)
		return FALSE;

	time_t t;
	t = time(NULL);

	wsprintf(szUrl, lpFmt, nGroupId, nToUin, lpClientId, lpPSessionId, t);

	bRet = HttpGet(HttpClient, szUrl, lpszReqHeaders, dwRespCode, NULL, RespData);
	if (!bRet || dwRespCode != 200)
		return FALSE;

	bRet = lpGetC2CMsgSigResult->Parse(&RespData);
	if (!bRet)
		return FALSE;

	return TRUE;
}

BOOL CQQProtocol::HttpReq(CHttpClient& HttpClient, LPCTSTR lpszUrl, 
			 LPCTSTR lpszReqHeaders, const CHAR * lpPostData, DWORD dwPostDataLen, 
			 DWORD& dwRespCode, std::vector<tstring>* arrRespHeader, CBuffer& RespData)
{
	HTTP_REQ_METHOD nReqMethod;
	CHAR cBuf[1024] = {0};
	DWORD dwBufLen = 1024;
	DWORD dwRecvLen;
	BOOL bRet;

	dwRespCode = 0;
	if (arrRespHeader != NULL)
		arrRespHeader->clear();

	if (lpPostData != NULL && dwPostDataLen > 0)
		nReqMethod = REQ_METHOD_POST;
	else
		nReqMethod = REQ_METHOD_GET;

	bRet = HttpClient.OpenRequest(lpszUrl, nReqMethod);
	if (!bRet)
		return FALSE;

	if (lpszReqHeaders != NULL)
		HttpClient.AddReqHeaders(lpszReqHeaders);
	
	bRet = HttpClient.SendRequest(lpPostData, dwPostDataLen);
	if (!bRet)
	{
		HttpClient.CloseRequest();
		return FALSE;
	}

	dwRespCode = HttpClient.GetRespCode();
	if (dwRespCode != 200)
	{
		HttpClient.CloseRequest();
		return FALSE;
	}

	do
	{
		bRet = HttpClient.GetRespBodyData(cBuf, dwBufLen, dwRecvLen);
		if (!bRet)
		{
			HttpClient.CloseRequest();
			return FALSE;
		}

		if (dwRecvLen == 0)
		{
			break;
		}
		else
		{
			RespData.Add((const BYTE *)cBuf, dwRecvLen);
		}
	} while (1);

	if (arrRespHeader != NULL)
	{
		tstring strRespHeader = HttpClient.GetRespHeader();
		tstring strLine;
		int nStart = 0;

		std::wstring::size_type nPos = strRespHeader.find(_T("\r\n"), nStart);
		while (nPos != std::wstring::npos)
		{
			strLine = strRespHeader.substr(nStart, nPos - nStart);
			if (strLine != _T(""))
				arrRespHeader->push_back(strLine);
			nStart = nPos + 2;
			nPos = strRespHeader.find(_T("\r\n"), nStart);
		}
	}

	HttpClient.CloseRequest();

	return TRUE;
}

BOOL CQQProtocol::HttpGet(CHttpClient& HttpClient, LPCTSTR lpszUrl, 
			 LPCTSTR lpszReqHeaders, DWORD& dwRespCode, 
			 std::vector<tstring>* arrRespHeader, CBuffer& RespData)
{
	return HttpReq(HttpClient, lpszUrl, lpszReqHeaders, 
		NULL, 0, dwRespCode, arrRespHeader, RespData);
}

BOOL CQQProtocol::HttpPost(CHttpClient& HttpClient, LPCTSTR lpszUrl, 
			  LPCTSTR lpszReqHeaders, const CHAR * lpPostData, DWORD dwPostDataLen, 
			  DWORD& dwRespCode, std::vector<tstring>* arrRespHeader, CBuffer& RespData)
{
	return HttpReq(HttpClient, lpszUrl, lpszReqHeaders, 
		lpPostData, dwPostDataLen, dwRespCode, arrRespHeader, RespData);
}

// BOOL CQQProtocol::CalcPwdHash(LPCTSTR lpQQPwd, LPCTSTR lpVerifyCode, TCHAR * lpPwdHash, int nLen)
// {
// 	CHAR cQQPwd[128] = {0};
// 	CHAR cVerifyCode[128] = {0};
// 	CHAR cHex[36] = {0};
// 	CHAR cTemp[256] = {0};
// 	const byte * lpDigest;
// 	MD5 md5;
// 
// 	//UPPER(HEX(MD5(UPPER(HEX(MD5(MD5(MD5(密码))))) + UPPER(验证码))));
// 
// 	if (NULL == lpQQPwd || NULL == lpVerifyCode
// 		|| NULL == lpPwdHash || nLen <= 0)
// 		return FALSE;
// 
// 	UnicodeToUtf8(lpQQPwd, cQQPwd, sizeof(cQQPwd));
// 	UnicodeToUtf8(lpVerifyCode, cVerifyCode, sizeof(cVerifyCode));
// 
// 	md5.reset();
// 	md5.update((const void *)cQQPwd, strlen(cQQPwd));
// 	lpDigest = md5.digest();
// 
// 	md5.reset();
// 	md5.update((const void *)lpDigest, 16);
// 	lpDigest = md5.digest();
// 
// 	md5.reset();
// 	md5.update((const void *)lpDigest, 16);
// 	lpDigest = md5.digest();
// 
// 	ToHexStr((const CHAR *)lpDigest, 16, cHex, sizeof(cHex));
// 
// 	_strupr(cHex);
// 	_strupr(cVerifyCode);
// 	strcpy(cTemp, cHex);
// 	strcat(cTemp, cVerifyCode);
// 
// 	md5.reset();
// 	md5.update((const void *)cTemp, strlen(cTemp));
// 	lpDigest = md5.digest();
// 
// 	memset(cHex, 0, sizeof(cHex));
// 	ToHexStr((const CHAR *)lpDigest, 16, cHex, sizeof(cHex));
// 	_strupr(cHex);
// 
// 	Utf8ToUnicode(cHex, lpPwdHash, nLen);
// 
// 	return TRUE;
// }

BOOL CQQProtocol::CalcPwdHash(LPCTSTR lpQQPwd, LPCTSTR lpVerifyCode, 
							  const CHAR * lpPtUin, int nPtUinLen, TCHAR * lpPwdHash, int nLen)
{
	CHAR cQQPwd[128] = {0};
	CHAR cVerifyCode[128] = {0};
	CHAR cHex[36] = {0};
	CHAR cTemp[256] = {0};
	const byte * lpDigest;
	MD5 md5;

	// UPPER(HEX(MD5(UPPER(HEX(MD5(MD5(密码) + PtUin))) + UPPER(验证码))))

	if (NULL == lpQQPwd || NULL == lpVerifyCode
		|| NULL == lpPtUin || nPtUinLen <= 0
		|| NULL == lpPwdHash || nLen <= 0)
		return FALSE;

	UnicodeToUtf8(lpQQPwd, cQQPwd, sizeof(cQQPwd));
	UnicodeToUtf8(lpVerifyCode, cVerifyCode, sizeof(cVerifyCode));

	md5.reset();
	md5.update((const void *)cQQPwd, strlen(cQQPwd));
	lpDigest = md5.digest();

	memset(cTemp, 0, sizeof(cTemp));
	memcpy(cTemp, lpDigest, 16);
	memcpy(&cTemp[16], lpPtUin, nPtUinLen);

	md5.reset();
	md5.update((const void *)cTemp, 16 + nPtUinLen);
	lpDigest = md5.digest();

	ToHexStr((const CHAR *)lpDigest, 16, cHex, sizeof(cHex));
	_strupr(cHex);

	int nHexLen = strlen(cHex);
	int nVerifyCodeLen = strlen(cVerifyCode);
	memset(cTemp, 0, sizeof(cTemp));
	memcpy(cTemp, cHex, nHexLen);
	memcpy(&cTemp[nHexLen], cVerifyCode, nVerifyCodeLen);

	md5.reset();
	md5.update((const void *)cTemp, nHexLen + nVerifyCodeLen);
	lpDigest = md5.digest();

	ToHexStr((const CHAR *)lpDigest, 16, cHex, sizeof(cHex));
	_strupr(cHex);

	Utf8ToUnicode(cHex, lpPwdHash, nLen);

	return TRUE;
}

// 检测指定字符是否是字母(A-Z，a-z)或数字(0-9)
BOOL CQQProtocol::my_isalnum (unsigned char cChar)
{
	return ((cChar | 0x20) - _T('a')) < 26u  || ( cChar - _T('0')) < 10u;
}

std::string CQQProtocol::EncodeData(const WCHAR * lpData, int nLen)
{
	CHAR szBuf[16];
	CHAR * lpUtf8Data;
	std::string strUtf8Data;

	lpUtf8Data = UnicodeToUtf8(lpData);
	if (NULL == lpUtf8Data)
		return "";

	for (int i = 0; i < nLen; i++)
	{
		if (this->my_isalnum(lpUtf8Data[i])/* || '_' == lpUtf8Data[i]*/)
		{
			strUtf8Data += lpUtf8Data[i];
		}
		else
		{
			sprintf(szBuf, "%%%02X", (unsigned char)lpUtf8Data[i]);
			strUtf8Data += szBuf;
		}
	}

	delete []lpUtf8Data;

	return strUtf8Data;
}

std::wstring CQQProtocol::UnicodeToHexStr(const WCHAR * lpStr, BOOL bDblSlash)
{
	static const WCHAR * lpHexStr = _T("0123456789abcdef");
	std::wstring strRet = _T("");
	WCHAR * lpSlash;

	if (NULL == lpStr || NULL == *lpStr)
		return strRet;

	lpSlash = (bDblSlash ? _T("\\\\u") : _T("\\u"));

	for (int i = 0; i < (int)wcslen(lpStr); i++)
	{
		if (((lpStr[i] | 0x20) - _T('a')) < 26u  || ( lpStr[i] - _T('0')) < 10u)
		{
			strRet += lpStr[i];
		}
		else
		{
			CHAR * lpChar = (CHAR *)&lpStr[i];

			strRet += lpSlash;
			strRet += lpHexStr[((unsigned char)(*(lpChar+1)) >> 4) & 0x0f];
			strRet += lpHexStr[(unsigned char)(*(lpChar+1)) & 0x0f];
			strRet += lpHexStr[((unsigned char)(*lpChar) >> 4) & 0x0f];
			strRet += lpHexStr[(unsigned char)(*lpChar) & 0x0f];
		}
	}

	return strRet;
}