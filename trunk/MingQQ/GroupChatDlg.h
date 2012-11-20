#pragma once

#include "resource.h"
#include "QQClient/QQClient.h"
#include "Utils.h"
#include "FaceList.h"
#include "FaceSelDlg.h"
#include "ImageEx.h"
#include "HeadImageList.h"

class CGroupChatDlg : public CDialogImpl<CGroupChatDlg>, public CMessageFilter
{
public:
	CGroupChatDlg(void);
	~CGroupChatDlg(void);

	enum { IDD = IDD_GROUPCHATDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP_EX(CGroupChatDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_HANDLER_EX(ID_LINK_GROUPNAME, BN_CLICKED, OnLnk_GroupName)
		COMMAND_HANDLER_EX(ID_BTN_FACE, BN_CLICKED, OnBtn_Face)
		COMMAND_HANDLER_EX(ID_COMBO_FONT_NAME, CBN_SELCHANGE, OnCbo_SelChange_FontName)
		COMMAND_HANDLER_EX(ID_COMBO_FONT_SIZE, CBN_SELCHANGE, OnCbo_SelChange_FontSize)
		COMMAND_HANDLER_EX(ID_CHECK_BOLD, BN_CLICKED, OnChk_Bold)
		COMMAND_HANDLER_EX(ID_CHECK_ITALIC, BN_CLICKED, OnChk_Italic)
		COMMAND_HANDLER_EX(ID_CHECK_UNDERLINE, BN_CLICKED, OnChk_UnderLine)
		COMMAND_HANDLER_EX(ID_BTN_COLOR, BN_CLICKED, OnBtn_Color)
		COMMAND_HANDLER_EX(ID_BTN_CLOSE, BN_CLICKED, OnBtn_Close)
		COMMAND_ID_HANDLER_EX(ID_BTN_SEND, OnBtn_Send)
		NOTIFY_HANDLER_EX(ID_RICHEDIT_RECV, EN_LINK, OnRichEdit_Recv_Link)
		NOTIFY_HANDLER_EX(ID_LIST_MERBER, NM_DBLCLK, OnGMemberList_DblClick)
		NOTIFY_HANDLER_EX(ID_LIST_MERBER, NM_RCLICK, OnGMemberList_RClick)
		COMMAND_ID_HANDLER_EX(ID_MENU_CUT, OnMenu_Cut)
		COMMAND_ID_HANDLER_EX(ID_MENU_COPY, OnMenu_Copy)
		COMMAND_ID_HANDLER_EX(ID_MENU_PASTE, OnMenu_Paste)
		COMMAND_ID_HANDLER_EX(ID_MENU_SELALL, OnMenu_SelAll)
		COMMAND_ID_HANDLER_EX(ID_MENU_CLEAR, OnMenu_Clear)
		COMMAND_ID_HANDLER_EX(ID_MENU_VIEW_INFO, OnMenu_ViewInfo)
		COMMAND_ID_HANDLER_EX(ID_MENU_SEND_MSG, OnMenu_SendMsg)
		MESSAGE_HANDLER_EX(FACE_CTRL_SEL, OnFaceCtrlSel)
	END_MSG_MAP()

public:
	void OnRecvMsg(UINT nGroupCode, UINT nMsgId);				// 接收群消息
	void OnUpdateGroupInfo();									// 更新群信息
	void OnUpdateGroupNumber();									// 更新群号码
	void OnUpdateGMemberNumber(WPARAM wParam, LPARAM lParam);	// 更新群成员号码

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();
	void OnLnk_GroupName(UINT uNotifyCode, int nID, CWindow wndCtl);// “群名称”超链接控件
	void OnBtn_Face(UINT uNotifyCode, int nID, CWindow wndCtl);	// “表情”按钮
	void OnCbo_SelChange_FontName(UINT uNotifyCode, int nID, CWindow wndCtl);	// “字体名称”组合框
	void OnCbo_SelChange_FontSize(UINT uNotifyCode, int nID, CWindow wndCtl);	// “字体大小”组合框
	void OnChk_Bold(UINT uNotifyCode, int nID, CWindow wndCtl);	// “加粗”复选框
	void OnChk_Italic(UINT uNotifyCode, int nID, CWindow wndCtl);// “斜体”复选框
	void OnChk_UnderLine(UINT uNotifyCode, int nID, CWindow wndCtl);// “下划线”复选框
	void OnBtn_Color(UINT uNotifyCode, int nID, CWindow wndCtl);// “颜色”按钮
	void OnBtn_Close(UINT uNotifyCode, int nID, CWindow wndCtl);// “关闭”按钮
	void OnBtn_Send(UINT uNotifyCode, int nID, CWindow wndCtl);	// “发送”按钮
	LRESULT OnRichEdit_Recv_Link(LPNMHDR pnmh);						//	“接收消息”富文本框链接点击消息
	LRESULT OnGMemberList_DblClick(LPNMHDR pnmh);					// “群成员”列表双击消息
	LRESULT OnGMemberList_RClick(LPNMHDR pnmh);						// “群成员”列表右键单击消息
	void OnMenu_Cut(UINT uNotifyCode, int nID, CWindow wndCtl);		// “剪切”菜单
	void OnMenu_Copy(UINT uNotifyCode, int nID, CWindow wndCtl);	// “复制”菜单
	void OnMenu_Paste(UINT uNotifyCode, int nID, CWindow wndCtl);	// “粘贴”菜单
	void OnMenu_SelAll(UINT uNotifyCode, int nID, CWindow wndCtl);	// “全部选择”菜单
	void OnMenu_Clear(UINT uNotifyCode, int nID, CWindow wndCtl);	// “清屏”菜单
	void OnMenu_ViewInfo(UINT uNotifyCode, int nID, CWindow wndCtl);// “查看资料”菜单
	void OnMenu_SendMsg(UINT uNotifyCode, int nID, CWindow wndCtl);	// “发送消息”菜单
	LRESULT OnFaceCtrlSel(UINT uMsg, WPARAM wParam, LPARAM lParam);	// “表情”控件选取消息

	CGroupInfo * GetGroupInfoPtr();		// 获取群信息指针
	CBuddyInfo * GetUserInfoPtr();		// 获取用户信息指针
	void UpdateData();					// 更新信息
	void UpdateDlgTitle();				// 更新对话框标题栏
	BOOL UpdateGroupNameCtrl();			// 更新群名称控件
	BOOL UpdateGroupMemo();				// 更新群公告
	BOOL UpdateGroupMemberList();		// 更新群成员列表
	BOOL InitCtrls();					// 初始化控件
	BOOL UnInitCtrls();					// 反初始化控件
	BOOL InitMidToolBar();				// 初始化中间工具栏
	void AddMsg(CGroupMessage * lpGroupMsg);
	void AddMsg(LPCTSTR lpText);
	void GetSenderInfo(UINT nQQUin, CString& strName, UINT& nQQNum);

public:
	CQQClient * m_lpQQClient;
	CFaceList * m_lpFaceList;
	HWND m_hMainDlg;
	UINT m_nGroupCode;

private:
	CStatic m_picHead;
	CHyperLink m_lnkGroupName;
	CStatic m_picAD;
	CEdit m_edtMemo;
	CComboBox m_cboFontName, m_cboFontSize;
	CButton m_btnBold, m_btnItalic, m_btnUnderLine;
	CListViewCtrl m_ListCtrl;
	CRichEditCtrl m_richRecv, m_richSend;
	CFaceSelDlg m_FaceSelDlg;
	CAccelerator m_Accelerator;
	CMenu m_menuRichEdit;
	CHeadImageList m_headImageList;
	CImageEx m_imgHead, m_imgAD;
	HICON m_hDlgIcon, m_hDlgSmallIcon;

	CFontInfo m_fontInfo;
	BOOL m_bIsHasGroupInfo;
	BOOL m_bIsHasGroupNumber;
	BOOL m_bIsHasGMemberNumber;
	int m_nGMemberNumberReq;
	CString m_strCurLink;

	UINT m_nGroupId, m_nGroupNumber;
	UINT m_nUserNumber;
	CString m_strGroupName;
	CString m_strUserName;
	int m_nMemberCnt, m_nOnlineMemberCnt;
};
