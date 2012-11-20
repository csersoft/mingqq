#pragma once

#include "resource.h"
#include "QQClient/QQClient.h"
#include "QQClient/Path.h"
#include "ImageEx.h"
#include "Utils.h"
#include "HeadImageList.h"

class CMsgTipDlg : public CDialogImpl<CMsgTipDlg>
{
public:
	CMsgTipDlg(void);
	~CMsgTipDlg(void);

	enum { IDD = IDD_MSGTIPDLG };

	BEGIN_MSG_MAP_EX(CMsgTipDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_SIZE(OnSize)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		NOTIFY_HANDLER_EX(IDC_LIST1, NM_CLICK, OnList_Click)
		//NOTIFY_HANDLER_EX(IDC_LIST1, NM_DBLCLK, OnList_DblClick)
		COMMAND_HANDLER_EX(ID_LINK_CANCEL_FLASH, BN_CLICKED, OnLnk_CancelFlash)
		COMMAND_HANDLER_EX(ID_LINK_SHOW_ALL, BN_CLICKED, OnLnk_ShowAll)
	END_MSG_MAP()

public:
	BOOL StartTrackMouseLeave();
	int FindMsgSender(QQ_MSG_TYPE nType, UINT nSenderId);
	void AddMsgSender(QQ_MSG_TYPE nType, UINT nSenderId);
	void DelMsgSender(QQ_MSG_TYPE nType, UINT nSenderId);

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnPaint(CDCHandle dc);
	HBRUSH OnCtlColorStatic(CDCHandle dc, CStatic wndStatic);
	void OnTimer(UINT_PTR nIDEvent);
	void OnSize(UINT nType, CSize size);
	void OnClose();
	void OnDestroy();
	LRESULT OnList_Click(LPNMHDR pnmh);
	void OnLnk_CancelFlash(UINT uNotifyCode, int nID, CWindow wndCtl);// “取消闪烁”超链接控件
	void OnLnk_ShowAll(UINT uNotifyCode, int nID, CWindow wndCtl);// “显示全部”超链接控件

	BOOL InitCtrls();			// 初始化控件
	BOOL UnInitCtrls();			// 反初始化控件
	void SetDlgAutoSize();		// 自动调整对话框大小
	void SetCtrlsAutoSize();	// 自动调整控件大小
	void _AddMsgSender(int nIndex, CMessageSender * lpMsgSender);

public:
	CQQClient * m_lpQQClient;
	HWND m_hMainDlg;
	CRect m_rcTrayIcon;

private:
	CListViewCtrl m_ListCtrl;
	CHyperLink m_lnkCancelFlash, m_lnkShowAll;
	CImageEx m_imgBg;
	DWORD m_dwTimerId;
	int m_nListItemHeight;
	CHeadImageList m_headImageList;
};
