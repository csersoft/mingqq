#pragma once

#include "resource.h"
#include "ImageEx.h"

class CVerifyCodeDlg : public CDialogImpl<CVerifyCodeDlg>
{
public:
	CVerifyCodeDlg(void);
	~CVerifyCodeDlg(void);

	enum { IDD = IDD_VERIFYCODEDLG };

	BEGIN_MSG_MAP_EX(CVerifyCodeDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_HANDLER_EX(ID_BTN_OK, BN_CLICKED, OnBtn_Ok)
		COMMAND_HANDLER_EX(ID_BTN_CANCEL, BN_CLICKED, OnBtn_Cancel)
		END_MSG_MAP()

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnPaint(CDCHandle dc);
	void OnClose();
	void OnDestroy();
	void OnBtn_Ok(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl);

public:
	CImageEx m_Image;
	CString m_strVerifyCode;
};
