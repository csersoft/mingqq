#include "StdAfx.h"
#include "GetHeadPicTask.h"

CGetHeadPicTask::CGetHeadPicTask(void)
{
	m_lpQQUser = NULL;
	m_lpQQProtocol = NULL;
	m_bStop = FALSE;
}

CGetHeadPicTask::~CGetHeadPicTask(void)
{
	DelAllParams();
}

int CGetHeadPicTask::Run()
{
	GHPT_PARAM stParam;
	CBuffer bufPic;
	BOOL bRet;

	if (NULL == m_lpQQUser || NULL == m_lpQQProtocol)
	{
		DelAllParams();
		m_bStop = FALSE;
		return 0;
	}

	for (int i = 0; i < (int)m_arrParam.size(); i++)
	{
		stParam = m_arrParam[i];

		BOOL bIsBuddy = TRUE;
		UINT nQQUin = stParam.nQQUin;
		if (stParam.nQQUin == 0 && stParam.nGroupCode != 0)	// 群
		{
			bIsBuddy = FALSE;
			nQQUin = stParam.nGroupCode;
		}

		bRet = m_lpQQProtocol->GetHeadPic(m_HttpClient, bIsBuddy, nQQUin, 
			m_lpQQUser->m_LoginResult2.m_strVfWebQq.c_str(), &bufPic);
		if (bRet)
			SavePic(stParam.nGroupNum, stParam.nQQNum, bufPic.GetData(), bufPic.GetSize());

		if (m_bStop)
			break;

		if (stParam.nQQUin != 0 && stParam.nGroupCode == 0)			// 好友
			::PostMessage(m_lpQQUser->m_hCallBackWnd, QQ_MSG_UPDATE_BUDDY_HEADPIC, 0, stParam.nQQUin);
		else if (stParam.nQQUin == 0 && stParam.nGroupCode != 0)	// 群
			::PostMessage(m_lpQQUser->m_hCallBackWnd, QQ_MSG_UPDATE_GROUP_HEADPIC, stParam.nGroupCode, 0);
		else if (stParam.nQQUin != 0 && stParam.nGroupCode != 0)	// 群成员
			::PostMessage(m_lpQQUser->m_hCallBackWnd, QQ_MSG_UPDATE_GMEMBER_HEADPIC, stParam.nGroupCode, stParam.nQQUin);
	}

	DelAllParams();
	m_bStop = FALSE;

	return 0;
}

int CGetHeadPicTask::Stop()
{
	m_bStop = TRUE;
	m_HttpClient.SetCancalEvent();
	return 0;
}

void CGetHeadPicTask::TaskFinish()
{
	delete this;
}

BOOL CGetHeadPicTask::AddParam(UINT nGroupCode, UINT nQQUin, UINT nGroupNum, UINT nQQNum)
{
	GHPT_PARAM stParam;
	stParam.nGroupCode = nGroupCode;
	stParam.nQQUin = nQQUin;
	stParam.nGroupNum = nGroupNum;
	stParam.nQQNum = nQQNum;
	m_arrParam.push_back(stParam);
	return TRUE;
}

void CGetHeadPicTask::DelAllParams()
{
	m_arrParam.clear();
}

BOOL CGetHeadPicTask::SavePic(UINT nGroupNum, UINT nQQNum, const BYTE * lpData, DWORD dwSize)
{
	if (NULL == lpData || dwSize <= 0)
		return FALSE;

	tstring strFullName = m_lpQQUser->GetHeadPicFullName(nGroupNum, nQQNum);

	tstring strPath = ZYM::CPath::GetDirectoryName(strFullName.c_str());
	if (!ZYM::CPath::IsDirectoryExist(strPath.c_str()))
		ZYM::CPath::CreateDirectory(strPath.c_str(), NULL);

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