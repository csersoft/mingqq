#pragma once

#include "QQClient/QQClient.h"
#include "Utils.h"

class CBuddyInfoDlg : public CDialogImpl<CBuddyInfoDlg>
{
public:
	CBuddyInfoDlg(void);
	~CBuddyInfoDlg(void);

	enum { IDD = IDD_BUDDYINFODLG };

	BEGIN_MSG_MAP_EX(CBuddyInfoDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_HANDLER_EX(ID_BTN_UPDATE, BN_CLICKED, OnBtn_Update)
		COMMAND_HANDLER_EX(ID_BTN_CLOSE, BN_CLICKED, OnBtn_Close)
	END_MSG_MAP()

public:
	void OnUpdateBuddyInfo();
	void OnUpdateBuddyNumber();
	void OnUpdateBuddySign();

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();
	void OnBtn_Update(UINT uNotifyCode, int nID, CWindow wndCtl);	// “更新”按钮
	void OnBtn_Close(UINT uNotifyCode, int nID, CWindow wndCtl);	// “关闭”按钮

	CBuddyInfo * GetBuddyInfoPtr();
	void UpdateCtrls();

public:
	CQQClient * m_lpQQClient;
	HWND m_hMainDlg;
	UINT m_nQQUin;
	BOOL m_bIsGMember;
	UINT m_nGroupCode;
	HICON m_hDlgIcon, m_hDlgSmallIcon;
};
