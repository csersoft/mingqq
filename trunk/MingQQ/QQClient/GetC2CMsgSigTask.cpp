#include "StdAfx.h"
#include "GetC2CMsgSigTask.h"

CGetC2CMsgSigTask::CGetC2CMsgSigTask(void)
{
}

CGetC2CMsgSigTask::~CGetC2CMsgSigTask(void)
{
}

int CGetC2CMsgSigTask::Run()
{
	if (NULL == m_lpQQUser || NULL == m_lpQQProtocol)
	{
		m_bStop = FALSE;
		return 0;
	}

	CGetC2CMsgSigResult * lpGetC2CMsgSigResult = new CGetC2CMsgSigResult;
	if (lpGetC2CMsgSigResult != NULL)
	{
		BOOL bRet = m_lpQQProtocol->GetC2CMsgSignal(m_HttpClient, m_nGroupId,
			m_nQQUin, WEBQQ_CLIENT_ID, m_lpQQUser->m_LoginResult2.m_strPSessionId.c_str(),
			lpGetC2CMsgSigResult);
		if (!bRet || lpGetC2CMsgSigResult->m_nRetCode != 0)
		{
			delete lpGetC2CMsgSigResult;
			lpGetC2CMsgSigResult = NULL;
		}
		lpGetC2CMsgSigResult->m_nGroupId = m_nGroupId;
		lpGetC2CMsgSigResult->m_nQQUin = m_nQQUin;
		::PostMessage(m_lpQQUser->m_hCallBackWnd, 
			QQ_MSG_UPDATE_C2CMSGSIG, 0, (LPARAM)lpGetC2CMsgSigResult);
	}
	
	m_bStop = FALSE;
	return 0;
}

int CGetC2CMsgSigTask::Stop()
{
	m_bStop = TRUE;
	m_HttpClient.SetCancalEvent();
	return 0;
}

void CGetC2CMsgSigTask::TaskFinish()
{
	delete this;
}