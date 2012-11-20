#pragma once

#include "QQClient/QQClient.h"

class CLoginDlg : public CDialogImpl<CLoginDlg>
{
public:
	CLoginDlg(void);
	virtual ~CLoginDlg(void);

	enum { IDD = IDD_LOGINDLG };

	BEGIN_MSG_MAP_EX(CLoginDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SYSCOMMAND(OnSysCommand)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_HANDLER_EX(ID_BTN_LOGIN, BN_CLICKED, OnBtn_Login)
		COMMAND_HANDLER_EX(ID_BTN_EXIT, BN_CLICKED, OnBtn_Exit)
	END_MSG_MAP()

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnSysCommand(UINT nID, CPoint pt);
	void OnClose();
	void OnDestroy();
	void OnBtn_Login(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtn_Exit(UINT uNotifyCode, int nID, CWindow wndCtl);

public:
	CString m_strUser, m_strPwd;
	QQ_STATUS m_nStatus;
	HICON m_hDlgIcon, m_hDlgSmallIcon;
};
