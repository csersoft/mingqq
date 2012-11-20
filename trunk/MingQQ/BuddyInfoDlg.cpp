#include "StdAfx.h"
#include "BuddyInfoDlg.h"

CBuddyInfoDlg::CBuddyInfoDlg(void)
{
	m_lpQQClient = NULL;
	m_hMainDlg = NULL;
	m_nQQUin = 0;
	m_bIsGMember = FALSE;
	m_nGroupCode = 0;
	m_hDlgIcon = m_hDlgSmallIcon = NULL;
}

CBuddyInfoDlg::~CBuddyInfoDlg(void)
{
}

BOOL CBuddyInfoDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	CenterWindow();

	// set icons
	m_hDlgIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(m_hDlgIcon, TRUE);
	m_hDlgSmallIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(m_hDlgSmallIcon, FALSE);

	CBuddyInfo * lpBuddyInfo = GetBuddyInfoPtr();
	if (NULL == lpBuddyInfo)
		return TRUE;

	CString strTitle;
	if (m_bIsGMember || lpBuddyInfo->m_strMarkName.empty())
		strTitle.Format(_T("%s的资料"), lpBuddyInfo->m_strNickName.c_str());
	else
		strTitle.Format(_T("%s的资料"), lpBuddyInfo->m_strMarkName.c_str());
	SetWindowText(strTitle);

	if (m_lpQQClient != NULL)
	{
		if (!lpBuddyInfo->IsHasBuddyInfo())
		{
			if (!m_bIsGMember)
				m_lpQQClient->UpdateBuddyInfo(m_nQQUin);
			else
				m_lpQQClient->UpdateGroupMemberInfo(m_nGroupCode, m_nQQUin);
		}
		if (!lpBuddyInfo->IsHasQQNum())
		{
			if (!m_bIsGMember)
				m_lpQQClient->UpdateBuddyNum(m_nQQUin);
			else
				m_lpQQClient->UpdateGroupMemberNum(m_nGroupCode, m_nQQUin);
		}
		if (!lpBuddyInfo->IsHasQQSign())
		{
			if (!m_bIsGMember)
				m_lpQQClient->UpdateBuddySign(m_nQQUin);
			else
				m_lpQQClient->UpdateGroupMemberSign(m_nGroupCode, m_nQQUin);
		}
	}
	
	UpdateCtrls();
	
	return TRUE;
}

void CBuddyInfoDlg::OnClose()
{
	if (!m_bIsGMember)
		::PostMessage(m_hMainDlg, WM_CLOSE_BUDDYINFODLG, NULL, m_nQQUin);
	else
		::PostMessage(m_hMainDlg, WM_CLOSE_GMEMBERINFODLG, m_nGroupCode, m_nQQUin);
}

void CBuddyInfoDlg::OnDestroy()
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

// “更新”按钮
void CBuddyInfoDlg::OnBtn_Update(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (m_lpQQClient != NULL)
	{
		if (!m_bIsGMember)
		{
			m_lpQQClient->UpdateBuddyInfo(m_nQQUin);
			m_lpQQClient->UpdateBuddyNum(m_nQQUin);
			m_lpQQClient->UpdateBuddySign(m_nQQUin);
		}
		else
		{
			m_lpQQClient->UpdateGroupMemberInfo(m_nGroupCode, m_nQQUin);
			m_lpQQClient->UpdateGroupMemberNum(m_nGroupCode, m_nQQUin);
			m_lpQQClient->UpdateGroupMemberSign(m_nGroupCode, m_nQQUin);
		}
	}
}

// “关闭”按钮
void CBuddyInfoDlg::OnBtn_Close(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	SendMessage(WM_CLOSE);
}

void CBuddyInfoDlg::OnUpdateBuddyInfo()
{
	UpdateCtrls();
}

void CBuddyInfoDlg::OnUpdateBuddyNumber()
{
	UpdateCtrls();
}

void CBuddyInfoDlg::OnUpdateBuddySign()
{
	UpdateCtrls();
}

CBuddyInfo * CBuddyInfoDlg::GetBuddyInfoPtr()
{
	if (NULL == m_lpQQClient)
		return NULL;

	if (!m_bIsGMember)
	{
		CBuddyInfo * lpUserInfo = m_lpQQClient->GetUserInfo();
		if (lpUserInfo != NULL && lpUserInfo->m_nQQUin == m_nQQUin)
		{
			return lpUserInfo;
		}
		else
		{
			CBuddyList * lpBuddyList = m_lpQQClient->GetBuddyList();
			if (lpBuddyList != NULL)
				return lpBuddyList->GetBuddy(m_nQQUin);
		}
	}
	else
	{
		CGroupList * lpGroupList = m_lpQQClient->GetGroupList();
		if (lpGroupList != NULL)
			return lpGroupList->GetGroupMember(m_nGroupCode, m_nQQUin);
	}

	return NULL;
}

void CBuddyInfoDlg::UpdateCtrls()
{
	CString strText;
	CBuddyInfo * lpBuddyInfo = GetBuddyInfoPtr();
	if (lpBuddyInfo != NULL)
	{
		SetDlgItemText(ID_EDIT_NICKNAME, lpBuddyInfo->m_strNickName.c_str());
		SetDlgItemText(ID_EDIT_MARKNAME, lpBuddyInfo->m_strMarkName.c_str());
		strText.Format(_T("%u"), lpBuddyInfo->m_nQQNum);
		SetDlgItemText(ID_EDIT_NUMBER, strText);
		SetDlgItemText(ID_EDIT_SIGN, lpBuddyInfo->m_strSign.c_str());

		SetDlgItemText(ID_EDIT_GENDER, lpBuddyInfo->GetDisplayGender().c_str());
		SetDlgItemText(ID_EDIT_SHENGXIAO, lpBuddyInfo->GetDisplayShengXiao().c_str());
		SetDlgItemText(ID_EDIT_CONSTEL, lpBuddyInfo->GetDisplayConstel().c_str());
		SetDlgItemText(ID_EDIT_BLOOD, lpBuddyInfo->GetDisplayBlood().c_str());
		SetDlgItemText(ID_EDIT_BIRTHDAY, lpBuddyInfo->m_strBirthday.c_str());
		SetDlgItemText(ID_EDIT_COUNTRY, lpBuddyInfo->m_strCountry.c_str());
		SetDlgItemText(ID_EDIT_PROVINCE, lpBuddyInfo->m_strProvince.c_str());
		SetDlgItemText(ID_EDIT_CITY, lpBuddyInfo->m_strCity.c_str());
		SetDlgItemText(ID_EDIT_PHONE, lpBuddyInfo->m_strPhone.c_str());
		SetDlgItemText(ID_EDIT_MOBILE, lpBuddyInfo->m_strMobile.c_str());
		SetDlgItemText(ID_EDIT_EMAIL, lpBuddyInfo->m_strEmail.c_str());
		SetDlgItemText(ID_EDIT_OCCUPATION, lpBuddyInfo->m_strOccupation.c_str());
		SetDlgItemText(ID_EDIT_COLLEGE, lpBuddyInfo->m_strCollege.c_str());
		SetDlgItemText(ID_EDIT_HOMEPAGE, lpBuddyInfo->m_strHomepage.c_str());
		SetDlgItemText(ID_EDIT_PERSONAL, lpBuddyInfo->m_strPersonal.c_str());
	}
}