#include "StdAfx.h"
#include "GroupInfoDlg.h"

CGroupInfoDlg::CGroupInfoDlg(void)
{
	m_lpQQClient = NULL;
	m_hMainDlg = NULL;
	m_nGroupCode = 0;
	m_hDlgIcon = m_hDlgSmallIcon = NULL;
}

CGroupInfoDlg::~CGroupInfoDlg(void)
{
}

BOOL CGroupInfoDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	CenterWindow();

	// set icons
	m_hDlgIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(m_hDlgIcon, TRUE);
	m_hDlgSmallIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(m_hDlgSmallIcon, FALSE);

	CGroupInfo * lpGroupInfo = GetGroupInfoPtr();
	if (lpGroupInfo != NULL)
	{
		if (!lpGroupInfo->IsHasGroupInfo())
			m_lpQQClient->UpdateGroupInfo(m_nGroupCode);
		if (!lpGroupInfo->IsHasGroupNumber())
			m_lpQQClient->UpdateGroupNum(m_nGroupCode);

		CString strTitle;
		strTitle.Format(_T("群设置 - %s"), lpGroupInfo->m_strName.c_str());
		SetWindowText(strTitle);
	}

	UpdateCtrls();

	return TRUE;
}

void CGroupInfoDlg::OnClose()
{
	::PostMessage(m_hMainDlg, WM_CLOSE_GROUPINFODLG, m_nGroupCode, NULL);
}

void CGroupInfoDlg::OnDestroy()
{
	SetMsgHandled(FALSE);

	if (m_hDlgIcon != NULL)
	{
		::DestroyIcon(m_hDlgIcon);
		m_hDlgIcon = NULL;
	}

	if (m_hDlgSmallIcon != NULL)
	{
		::DestroyIcon(m_hDlgSmallIcon);
		m_hDlgSmallIcon = NULL;
	}
}

// “确定”按钮
void CGroupInfoDlg::OnBtn_Ok(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	SendMessage(WM_CLOSE);
}

// “取消”按钮
void CGroupInfoDlg::OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	SendMessage(WM_CLOSE);
}

// 更新群信息
void CGroupInfoDlg::OnUpdateGroupInfo()
{
	UpdateCtrls();
}

// 更新群号码
void CGroupInfoDlg::OnUpdateGroupNumber()
{
	UpdateCtrls();
}

CGroupInfo * CGroupInfoDlg::GetGroupInfoPtr()
{
	if (NULL == m_lpQQClient)
		return NULL;

	CGroupList * lpGroupList = m_lpQQClient->GetGroupList();
	if (lpGroupList != NULL)
		return lpGroupList->GetGroupByCode(m_nGroupCode);
	else
		return NULL;
}

void CGroupInfoDlg::UpdateCtrls()
{
	CString strText;

	CGroupInfo * lpGroupInfo = GetGroupInfoPtr();
	if (lpGroupInfo != NULL)
	{
		SetDlgItemText(ID_EDIT_NAME, lpGroupInfo->m_strName.c_str());
		strText.Format(_T("%u"), lpGroupInfo->m_nGroupNumber);
		SetDlgItemText(ID_EDIT_NUMBER, strText);
		CBuddyInfo * lpBuddyInfo = lpGroupInfo->GetMemberByUin(lpGroupInfo->m_nOwnerUin);
		if (lpBuddyInfo != NULL)
			SetDlgItemText(ID_EDIT_CREATER, lpBuddyInfo->m_strNickName.c_str());
		strText = FormatTime(lpGroupInfo->m_nCreateTime, _T("%Y-%m-%d"));
		SetDlgItemText(ID_EDIT_CREATETIME, strText);
		strText.Format(_T("%u"), lpGroupInfo->m_nClass);
		SetDlgItemText(ID_EDIT_CLASS, strText);
		SetDlgItemText(ID_EDIT_REMARK, _T(""));
		SetDlgItemText(ID_EDIT_MEMO, lpGroupInfo->m_strMemo.c_str());
		SetDlgItemText(ID_EDIT_FINGERMEMO, lpGroupInfo->m_strFingerMemo.c_str());

		lpBuddyInfo = m_lpQQClient->GetUserInfo();
		if (lpBuddyInfo != NULL)
		{
			lpBuddyInfo = lpGroupInfo->GetMemberByUin(lpBuddyInfo->m_nQQUin);
			if (lpBuddyInfo != NULL)
			{
				if (!lpBuddyInfo->m_strGroupCard.empty())
					SetDlgItemText(ID_EDIT_CARDNAME, lpBuddyInfo->m_strGroupCard.c_str());
				else
					SetDlgItemText(ID_EDIT_CARDNAME, lpBuddyInfo->m_strNickName.c_str());
				SetDlgItemText(ID_EDIT_GENDER, lpBuddyInfo->GetDisplayGender().c_str());
				SetDlgItemText(ID_EDIT_PHONE, lpBuddyInfo->m_strPhone.c_str());
				SetDlgItemText(ID_EDIT_EMAIL, lpBuddyInfo->m_strEmail.c_str());
				SetDlgItemText(ID_EDIT_REMARK2, _T(""));
			}
		}
	}
}