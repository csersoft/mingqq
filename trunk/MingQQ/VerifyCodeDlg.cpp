#include "StdAfx.h"
#include "VerifyCodeDlg.h"

CVerifyCodeDlg::CVerifyCodeDlg(void)
{
}

CVerifyCodeDlg::~CVerifyCodeDlg(void)
{
}

BOOL CVerifyCodeDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	CenterWindow(GetParent());
	return TRUE;
}

void CVerifyCodeDlg::OnPaint(CDCHandle dc)
{
	CPaintDC PaintDC(m_hWnd);

	if (!m_Image.IsNull())
		m_Image.Draw(PaintDC.m_hDC, 0, 0);
}

void CVerifyCodeDlg::OnClose()
{
	EndDialog(IDCANCEL);
}

void CVerifyCodeDlg::OnDestroy()
{
	SetMsgHandled(FALSE);

	m_Image.Destroy();
}

void CVerifyCodeDlg::OnBtn_Ok(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CWindow ctrl;

	ctrl = GetDlgItem(ID_EDIT_VERIFY_CODE);
	ctrl.GetWindowText(m_strVerifyCode);

	EndDialog(IDOK);
}

void CVerifyCodeDlg::OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	EndDialog(IDCANCEL);
}