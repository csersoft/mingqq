// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <map>
#include "LoginDlg.h"
#include "VerifyCodeDlg.h"
#include "BuddyChatDlg.h"
#include "GroupChatDlg.h"
#include "SessChatDlg.h"
#include "QQClient/QQClient.h"
#include "TrayIcon.h"
#include "FaceList.h"
#include "BuddyInfoDlg.h"
#include "GroupInfoDlg.h"
#include "Utils.h"
#include "MsgTipDlg.h"
#include "MainTabCtrl.h"
#include "HeadImageList.h"

#pragma comment(lib, "winmm.lib")

class CGMemberInfoMapKey
{
public:
	CGMemberInfoMapKey(void):m_nGroupCode(0), m_nQQUin(0){}
	~CGMemberInfoMapKey(void){}

public:
	bool operator < (const CGMemberInfoMapKey& rhs) const
	{
		if(m_nGroupCode < rhs.m_nGroupCode) return true;
		if(m_nGroupCode > rhs.m_nGroupCode) return false;
		return  m_nQQUin < rhs.m_nQQUin;
	}

public:
	UINT m_nGroupCode;
	UINT m_nQQUin;
};

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler
{
public:
	CMainDlg(void);
	~CMainDlg(void);

	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SYSCOMMAND(OnSysCommand)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_HOTKEY(OnHotKey)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_HANDLER_EX(ID_PIC_HEAD, BN_CLICKED, OnPic_Clicked_Head)
		COMMAND_HANDLER_EX(ID_STATIC_SIGN, BN_CLICKED, OnSta_Clicked_Sign)
		COMMAND_HANDLER_EX(ID_EDIT_SIGN, EN_KILLFOCUS, OnEdit_Sign_KillFocus)
		COMMAND_HANDLER_EX(ID_COMBO_STATUS, CBN_SELCHANGE, OnCbo_SelChange_Status)
		COMMAND_HANDLER_EX(ID_BTN_CANCEL, BN_CLICKED, OnBtn_Clicked_Cancel)
		NOTIFY_HANDLER_EX(IDC_TAB1, TCN_SELCHANGE, OnTabCtrlSelChange)
		NOTIFY_HANDLER_EX(ID_TREE_BUDDY, NM_DBLCLK, OnBuddyTreeDblClk)
		NOTIFY_HANDLER_EX(ID_TREE_GROUP, NM_DBLCLK, OnGroupTreeDblClk)
		NOTIFY_HANDLER_EX(ID_TREE_RECENT, NM_DBLCLK, OnRecentTreeDblClk)
		COMMAND_ID_HANDLER_EX(ID_MENU_OPEN_MAIN_PANEL, OnMenu_OpenMainPanel)
		COMMAND_ID_HANDLER_EX(ID_MENU_EXIT, OnMenu_Exit)

		MESSAGE_HANDLER_EX(QQ_MSG_LOGIN_RESULT, OnLoginResult)
		MESSAGE_HANDLER_EX(QQ_MSG_LOGOUT_RESULT, OnLogoutResult)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_USER_INFO, OnUpdateUserInfo)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_BUDDY_LIST, OnUpdateBuddyList)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_GROUP_LIST, OnUpdateGroupList)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_RECENT_LIST, OnUpdateRecentList)
		MESSAGE_HANDLER_EX(QQ_MSG_BUDDY_MSG, OnBuddyMsg)
		MESSAGE_HANDLER_EX(QQ_MSG_GROUP_MSG, OnGroupMsg)
		MESSAGE_HANDLER_EX(QQ_MSG_SESS_MSG, OnSessMsg)
		MESSAGE_HANDLER_EX(QQ_MSG_STATUS_CHANGE_MSG, OnStatusChangeMsg)
		MESSAGE_HANDLER_EX(QQ_MSG_KICK_MSG, OnKickMsg)
		MESSAGE_HANDLER_EX(QQ_MSG_SYS_GROUP_MSG, OnSysGroupMsg)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_BUDDY_NUMBER, OnUpdateBuddyNumber)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_GMEMBER_NUMBER, OnUpdateGMemberNumber)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_GROUP_NUMBER, OnUpdateGroupNumber)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_BUDDY_SIGN, OnUpdateBuddySign)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_GMEMBER_SIGN, OnUpdateGMemberSign)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_BUDDY_INFO, OnUpdateBuddyInfo)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_GMEMBER_INFO, OnUpdateGMemberInfo)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_GROUP_INFO, OnUpdateGroupInfo)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_C2CMSGSIG, OnUpdateC2CMsgSig)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_BUDDY_HEADPIC, OnUpdateBuddyHeadPic)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_GMEMBER_HEADPIC, OnUpdateGMemberHeadPic)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_GROUP_HEADPIC, OnUpdateGroupHeadPic)
		MESSAGE_HANDLER_EX(QQ_MSG_CHANGE_STATUS_RESULT, OnChangeStatusResult)

		MESSAGE_HANDLER_EX(WM_TRAYICON_NOTIFY, OnTrayIconNotify)
		MESSAGE_HANDLER_EX(WM_SHOW_BUDDYCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_GROUPCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_SESSCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_SYSGROUPCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_BUDDYINFODLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_GMEMBERINFODLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_GROUPINFODLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_BUDDYCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_GROUPCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_SESSCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_SYSGROUPCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_BUDDYINFODLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_GMEMBERINFODLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_GROUPINFODLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_DEL_MSG_SENDER, OnDelMsgSender)
		MESSAGE_HANDLER_EX(WM_CANCEL_FLASH, OnCancelFlash)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnSysCommand(UINT nID, CPoint pt);
	void OnTimer(UINT_PTR nIDEvent);
	void OnHotKey(int nHotKeyID, UINT uModifiers, UINT uVirtKey);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnClose();
	void OnDestroy();
	void OnPic_Clicked_Head(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnSta_Clicked_Sign(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnEdit_Sign_KillFocus(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnCbo_SelChange_Status(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtn_Clicked_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnTabCtrlSelChange(LPNMHDR pnmh);
	LRESULT OnBuddyTreeDblClk(LPNMHDR pnmh);
	LRESULT OnGroupTreeDblClk(LPNMHDR pnmh);
	LRESULT OnRecentTreeDblClk(LPNMHDR pnmh);
	void OnMenu_OpenMainPanel(UINT uNotifyCode, int nID, CWindow wndCtl);// “打开主面板”菜单
	void OnMenu_Exit(UINT uNotifyCode, int nID, CWindow wndCtl);	// “退出”菜单

	void CloseDialog(int nVal);

	LRESULT OnLoginResult(UINT uMsg, WPARAM wParam, LPARAM lParam);			// 登录返回消息
	LRESULT OnLogoutResult(UINT uMsg, WPARAM wParam, LPARAM lParam);		// 注销返回消息
	LRESULT OnUpdateUserInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);		// 更新用户信息
	LRESULT OnUpdateBuddyList(UINT uMsg, WPARAM wParam, LPARAM lParam);		// 更新好友列表消息
	LRESULT OnUpdateGroupList(UINT uMsg, WPARAM wParam, LPARAM lParam);		// 更新群列表消息
	LRESULT OnUpdateRecentList(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 更新最近联系人列表消息
	LRESULT OnBuddyMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);			// 好友消息
	LRESULT OnGroupMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);			// 群消息
	LRESULT OnSessMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);				// 临时会话消息
	LRESULT OnStatusChangeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);		// 好友状态改变消息
	LRESULT OnKickMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);				// 被踢下线消息
	LRESULT OnSysGroupMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);			// 群系统消息
	LRESULT OnUpdateBuddyNumber(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 更新好友号码
	LRESULT OnUpdateGMemberNumber(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 更新群成员号码_
	LRESULT OnUpdateGroupNumber(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 更新群号码
	LRESULT OnUpdateBuddySign(UINT uMsg, WPARAM wParam, LPARAM lParam);		// 更新好友个性签名
	LRESULT OnUpdateGMemberSign(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 更新群成员个性签名
	LRESULT OnUpdateBuddyInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);		// 更新好友信息
	LRESULT OnUpdateGMemberInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 更新群成员信息
	LRESULT OnUpdateGroupInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);		// 更新群信息
	LRESULT OnUpdateC2CMsgSig(UINT uMsg, WPARAM wParam, LPARAM lParam);		// 更新临时会话信令
	LRESULT OnUpdateBuddyHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 更新好友头像图片
	LRESULT OnUpdateGMemberHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam);// 更新群成员头像图片
	LRESULT OnUpdateGroupHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 更新群头像图片
	LRESULT OnChangeStatusResult(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 改变在线状态返回消息

	LRESULT OnTrayIconNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnShowOrCloseDlg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDelMsgSender(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCancelFlash(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void ShowBuddyChatDlg(UINT nQQUin, BOOL bShow);
	void ShowGroupChatDlg(UINT nGroupCode, BOOL bShow);
	void ShowSessChatDlg(UINT nGroupCode, UINT nQQUin, BOOL bShow);
	void ShowSysGroupChatDlg(UINT nGroupCode, BOOL bShow);
	void ShowBuddyInfoDlg(UINT nQQUin, BOOL bShow);
	void ShowGMemberInfoDlg(UINT nGroupCode, UINT nQQUin, BOOL bShow);
	void ShowGroupInfoDlg(UINT nGroupCode, BOOL bShow);

	void NotifyBuddyChatDlg(UINT nQQUin, UINT uMsg);		// 通知好友聊天窗口更新
	void NotifyGroupChatDlg(UINT nGroupCode,				// 通知群聊天窗口更新
		UINT uMsg, WPARAM wParam, LPARAM lParam);
	void NotifySessChatDlg(UINT nQQUin, UINT uMsg);			// 通知临时会话聊天窗口更新
	void NotifyBuddyInfoDlg(UINT nQQUin, UINT uMsg);		// 通知好友信息窗口更新
	void NotifyGMemberInfoDlg(UINT nGroupCode, UINT nQQUin, UINT uMsg);	// 通知群成员信息窗口更新
	void NotifyGroupInfoDlg(UINT nGroupCode, UINT uMsg);	// 通知群信息窗口更新

	void UpdateBuddyTreeCtrl();
	void UpdateGroupTreeCtrl();
	void UpdateRecentTreeCtrl();

	void OnTrayIcon_LButtunUp();
	void OnTrayIcon_RButtunUp();
	void OnTrayIcon_MouseHover();
	void OnTrayIcon_MouseLeave();

	HTREEITEM FindTreeItemByQQUin(CTreeViewCtrl& treeCtrl, UINT nQQUin);
	HTREEITEM FindTreeItemByQQUin2(CTreeViewCtrl& treeCtrl, HTREEITEM hItem, UINT nQQUin);

	BOOL LoadAppIcon(QQ_STATUS nStatus);
	void DestroyAppIcon();
	BOOL LoadLoginIcon();
	void DestroyLoginIcon();
	void UpdateMsgIcon();

	void StartLogin();

	void CloseAllDlg();

private:
	CLoginDlg m_LoginDlg;
	CMsgTipDlg m_MsgTipDlg;
	CStatic m_staQQNum;
	CStatic m_staLogining;
	CButton m_btnCancel;
	CStatic m_picHead;
	CStatic m_staNickName;
	CStatic m_staSign;
	CEdit m_edtSign;
	CComboBox m_cboStatus;
	CMainTabCtrl m_MainTabCtrl;

	std::map<UINT, CBuddyChatDlg *> m_mapBuddyChatDlg;
	std::map<UINT, CGroupChatDlg *> m_mapGroupChatDlg;
	std::map<UINT, CSessChatDlg *> m_mapSessChatDlg;
	std::map<UINT, CBuddyInfoDlg *> m_mapBuddyInfoDlg;
	std::map<CGMemberInfoMapKey, CBuddyInfoDlg *> m_mapGMemberInfoDlg;
	std::map<UINT, CGroupInfoDlg *> m_mapGroupInfoDlg;

	Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR m_gdiplusToken;

	CQQClient m_QQClient;
	CFaceList m_FaceList;
	CTrayIcon m_TrayIcon;
	CHeadImageList m_headImageList;
	CImageEx m_imgHead;
	
	QQ_MSG_TYPE m_nLastMsgType;
	UINT m_nLastSenderId;

	HICON m_hAppIcon;
	HICON m_hLoginIcon[6];
	int m_nCurLoginIcon;
	HICON m_hMsgIcon;
	DWORD m_dwLoginTimerId;
	DWORD m_dwMsgTimerId;

	HICON m_hDlgIcon, m_hDlgSmallIcon;
};
