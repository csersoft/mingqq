#pragma once

#include <WinInet.h>
#include <string>
#include <vector>

#pragma comment(lib, "wininet.lib")

// IE8 on Windows 7
#define		IE8_USER_AGENT	 _T("Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; Trident/4.0)")

enum HTTP_REQ_METHOD	// HTTP请求方法
{
	REQ_METHOD_GET,
	REQ_METHOD_POST,
};

enum HTTP_STATUS_EVENT	// HTTP状态事件标识
{
	CONNECTED_EVENT,
	REQUEST_OPENED_EVENT,
	REQUEST_COMPLETE_EVENT,
	USER_CANCEL_EVENT
};

class CHttpClient
{
public:
	CHttpClient(void);
	~CHttpClient(void);

public:
	BOOL OpenRequest(LPCTSTR lpszUrl, HTTP_REQ_METHOD nReqMethod = REQ_METHOD_GET);	// 打开HTTP请求函数
	BOOL AddReqHeaders(LPCTSTR lpHeaders);	// 添加一个或多个HTTP请求头函数
	BOOL SendRequest(const CHAR * lpData, DWORD dwLen);	// 发送HTTP请求函数
	BOOL SendRequestEx(DWORD dwLen);	// 开始发送HTTP请求函数
	BOOL SendReqBodyData(CHAR * lpBuf, DWORD dwLen, DWORD& dwSendLen);	// 发送HTTP请求消息体数据函数
	BOOL EndSendRequest();	// 结束发送HTTP请求函数
	DWORD GetRespCode();	// 获取HTTP响应码函数
	std::wstring GetRespHeader();	// 获取全部HTTP头
	std::wstring GetRespHeader(LPCTSTR lpszName, int nIndex = 0);	// 获取HTTP头指定字段名称的值
	int GetRespHeaderByInt(LPCTSTR lpszName, int nIndex = 0);	// 获取HTTP头指定字段名称的值(Int)
	BOOL GetRespBodyData(CHAR * lpBuf, DWORD dwLen, DWORD& dwRecvLen);	// 获取HTTP响应消息体数据函数
	void CloseRequest();	// 关闭HTTP请求函数

	void SetConnectTimeOut(DWORD dwTimeOut);	// 设置连接超时(单位：毫秒)
	void SetCancalEvent();	// 设置取消事件函数
	void SetProxy(LPCTSTR lpszServer, WORD nPort, 
		LPCTSTR lpszUser = NULL, LPCTSTR lpszPwd = NULL);	// 设置HTTP代理服务器

private:
	static void __stdcall StatusCallback(HINTERNET hInternet, DWORD dwContext,	// 状态回调函数
		DWORD dwInternetStatus, LPVOID lpStatusInfo, DWORD dwStatusInfoLen);

	BOOL ParseURL(LPCTSTR lpszUrl, LPTSTR lpszScheme, DWORD dwSchemeLength,
		LPTSTR lpszHostName, DWORD dwHostNameLength, WORD& nPort, 
		LPTSTR lpszUrlPath, DWORD dwUrlPathLength);	// 解析Url各个部分函数
	BOOL WaitForEvent(HTTP_STATUS_EVENT nEvent, DWORD dwTimeOut);	// 等待事件函数

	DWORD __GetRespHeaderLen();	// 获取HTTP响应头长度
	BOOL __GetRespHeader();	// 获取HTTP响应头，按行保存在m_arrRespHeader数组

private:
	HINTERNET m_hInternet;
	HINTERNET m_hConnect;
	HINTERNET m_hRequest;

	HANDLE m_hConnectedEvent;
	HANDLE m_hRequestOpenedEvent;
	HANDLE m_hRequestCompleteEvent;
	HANDLE m_hCancelEvent;

	DWORD m_dwConnectTimeOut;
	DWORD m_dwContext;

	std::vector<std::wstring> m_arrRespHeader;

	std::wstring m_strProxy;
	std::wstring m_strUser, m_strPwd;
};
