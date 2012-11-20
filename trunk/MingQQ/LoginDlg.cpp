#include "StdAfx.h"
#include "LoginDlg.h"

CLoginDlg::CLoginDlg(void)
{
	m_nStatus = QQ_STATUS_ONLINE;
	m_hDlgIcon = m_hDlgSmallIcon = NULL;
}

CLoginDlg::~CLoginDlg(void)
{
}

BOOL CLoginDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

	CenterWindow(GetParent());

	// set icons
	m_hDlgIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(m_hDlgIcon, TRUE);
	m_hDlgSmallIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(m_hDlgSmallIcon, FALSE);

	CComboBox cboStatus = GetDlgItem(ID_COMBO_STATUS);
	cboStatus.AddString(_T("我在线上"));
	cboStatus.AddString(_T("Q我吧"));
	cboStatus.AddString(_T("离开"));
	cboStatus.AddString(_T("忙碌"));
	cboStatus.AddString(_T("请匆打扰"));
	cboStatus.AddString(_T("隐身"));
	cboStatus.SetCurSel(0);

	return TRUE;
}

void CLoginDlg::OnSysCommand(UINT nID, CPoint pt)
{
	if (nID == SC_MINIMIZE)
	{
		ShowWindow(SW_HIDE);
		return;
	}

	SetMsgHandled(FALSE);
}

void CLoginDlg::OnClose()
{
	EndDialog(IDCANCEL);
}

void CLoginDlg::OnDestroy()
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

void CLoginDlg::OnBtn_Login(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CWindow ctrl;
	CString strStatus;

	ctrl = GetDlgItem(ID_EDIT_USER);
	ctrl.GetWindowText(m_strUser);

	ctrl = GetDlgItem(ID_EDIT_PWD);
	ctrl.GetWindowText(m_strPwd);

	ctrl = GetDlgItem(ID_COMBO_STATUS);
	ctrl.GetWindowText(strStatus);

	if (strStatus == _T("我在线上"))
		m_nStatus = QQ_STATUS_ONLINE;
	else if (strStatus == _T("Q我吧"))
		m_nStatus = QQ_STATUS_CALLME;
	else if (strStatus == _T("离开"))
		m_nStatus = QQ_STATUS_AWAY;
	else if (strStatus == _T("忙碌"))
		m_nStatus = QQ_STATUS_BUSY;
	else if (strStatus == _T("请匆打扰"))
		m_nStatus = QQ_STATUS_SILENT;
	else if (strStatus == _T("隐身"))
		m_nStatus = QQ_STATUS_HIDDEN;

	EndDialog(IDOK);
}

void CLoginDlg::OnBtn_Exit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	EndDialog(IDCANCEL);
}