// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"

CMainDlg::CMainDlg(void)
{
	m_nLastMsgType = QQ_MSG_TYPE_BUDDY;
	m_nLastSenderId = 0;

	m_hAppIcon = NULL;
	memset(m_hLoginIcon, 0, sizeof(m_hLoginIcon));
	m_nCurLoginIcon = 0;
	m_hMsgIcon = NULL;
	m_dwLoginTimerId = NULL;
	m_dwMsgTimerId = NULL;

	m_hDlgIcon = m_hDlgSmallIcon = NULL;
}

CMainDlg::~CMainDlg(void)
{
}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == m_edtSign.m_hWnd)   
	{
		if (pMsg->message == WM_CHAR && pMsg->wParam == VK_RETURN) 
		{
			SetFocus();
		} 
	}

	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

BOOL CMainDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	SetWindowPos(HWND_TOPMOST, 989, 64, 260, 540, NULL);

	m_hDlgIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(m_hDlgIcon, TRUE);
	m_hDlgSmallIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(m_hDlgSmallIcon, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);

	::RegisterHotKey(m_hWnd, 1001, MOD_CONTROL|MOD_ALT, _T('X'));		// 注册全局热键

	m_QQClient.Init();

	LoadAppIcon(m_QQClient.GetStatus());
	m_TrayIcon.AddIcon(m_hWnd, WM_TRAYICON_NOTIFY, 1, m_hAppIcon, _T("QQ2011"));

	CString strPath = ZYM::CPath::GetCurDir() + _T("Face\\");
	m_FaceList.LoadConfigFile(strPath + _T("FaceConfig.xml"));

	m_picHead = GetDlgItem(ID_PIC_HEAD);
	m_staNickName = GetDlgItem(ID_STATIC_NICKNAME);
	m_staSign = GetDlgItem(ID_STATIC_SIGN);
	m_edtSign = GetDlgItem(ID_EDIT_SIGN);
	m_cboStatus = GetDlgItem(ID_COMBO_STATUS);
	m_staQQNum = GetDlgItem(ID_STATIC_NUMBER);
	m_staLogining = GetDlgItem(ID_STATIC_LOGINING);
	m_btnCancel = GetDlgItem(ID_BTN_CANCEL);
	m_MainTabCtrl.SubclassWindow(GetDlgItem(IDC_TAB1));

	m_cboStatus.AddString(_T("我在线上"));
	m_cboStatus.AddString(_T("Q我吧"));
	m_cboStatus.AddString(_T("离开"));
	m_cboStatus.AddString(_T("忙碌"));
	m_cboStatus.AddString(_T("请匆打扰"));
	m_cboStatus.AddString(_T("隐身"));

	StartLogin();

	UINT nQQUin = _tcstol(m_LoginDlg.m_strUser, NULL, 10);
	CString strFileName = m_QQClient.GetHeadPicFullName(0, nQQUin).c_str();
	HRESULT hr = m_imgHead.Load(strFileName);
	if (hr != S_OK)
		m_imgHead.Load(ZYM::CPath::GetCurDir() + _T("Image\\DefBuddyHeadPic.png"));
	m_picHead.SetBitmap((HBITMAP)m_imgHead);

	m_headImageList.Init(24, 24, &m_QQClient);

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcTabCtrl(rcClient);
	rcTabCtrl.left += 2;
	rcTabCtrl.top += 60;
	m_MainTabCtrl.InitTabCtrl(rcTabCtrl, m_headImageList.GetImageList());

	return TRUE;
}

void CMainDlg::OnSysCommand(UINT nID, CPoint pt)
{
	if (nID == SC_MINIMIZE)
	{
		ShowWindow(SW_HIDE);
		return;
	}

	SetMsgHandled(FALSE);
}

void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	m_TrayIcon.OnTimer(nIDEvent);

	if (nIDEvent == m_dwMsgTimerId)		// 来消息闪烁动画
	{
		static BOOL bSwitch = FALSE;

		if (m_dwLoginTimerId != NULL)	// 未登录成功
			return;

		bSwitch = !bSwitch;
		if (bSwitch)
			m_TrayIcon.ModifyIcon(NULL, _T(""));
		else
			m_TrayIcon.ModifyIcon(m_hMsgIcon, _T(""));
	}
	else if (nIDEvent == m_dwLoginTimerId)	// 登录动画
	{
		m_TrayIcon.ModifyIcon(m_hLoginIcon[m_nCurLoginIcon], _T(""));
		m_nCurLoginIcon++;
		if (m_nCurLoginIcon >= 6)
			m_nCurLoginIcon = 0;
	}
}

void CMainDlg::OnHotKey(int nHotKeyID, UINT uModifiers, UINT uVirtKey)
{
	switch (nHotKeyID)
	{
	case 1001:
		{
			OnTrayIconNotify(WM_TRAYICON_NOTIFY, NULL, WM_LBUTTONUP);
		}
		break;
	}
}

void CMainDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);
	SetFocus();
}

void CMainDlg::OnClose()
{
	CloseDialog(0);
}

void CMainDlg::OnDestroy()
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	DestroyAppIcon();
	DestroyLoginIcon();

	if (m_hMsgIcon != NULL)
	{
		::DestroyIcon(m_hMsgIcon);
		m_hMsgIcon = NULL;
	}

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

	if (!m_imgHead.IsNull())
		m_imgHead.Destroy();

	m_headImageList.UnInit();

	m_FaceList.Reset();

	if (m_staQQNum.IsWindow())
		m_staQQNum.DestroyWindow();
	if (m_staLogining.IsWindow())
		m_staLogining.DestroyWindow();
	if (m_btnCancel.IsWindow())
		m_btnCancel.DestroyWindow();
	if (m_picHead.IsWindow())
		m_picHead.DestroyWindow();
	if (m_staNickName.IsWindow())
		m_staNickName.DestroyWindow();
	if (m_staSign.IsWindow())
		m_staSign.DestroyWindow();
	if (m_edtSign.IsWindow())
		m_edtSign.DestroyWindow();
	if (m_cboStatus.IsWindow())
		m_cboStatus.DestroyWindow();
	if (m_MainTabCtrl.IsWindow())
		m_MainTabCtrl.DestroyWindow();

	m_QQClient.UnInit();

	::UnregisterHotKey(m_hWnd, 1001);	// 反注册全局热键
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

void CMainDlg::OnPic_Clicked_Head(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CBuddyInfo * lpBuddyInfo = m_QQClient.GetUserInfo();
	if (lpBuddyInfo != NULL)
		ShowBuddyInfoDlg(lpBuddyInfo->m_nQQUin, TRUE);
}

void CMainDlg::OnSta_Clicked_Sign(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strText;
	m_staSign.GetWindowText(strText);
	m_edtSign.SetWindowText(strText);

	m_staSign.ShowWindow(SW_HIDE);
	m_edtSign.ShowWindow(SW_SHOW);

	m_edtSign.SetSel(0, -1);
	m_edtSign.SetFocus();
}

void CMainDlg::OnEdit_Sign_KillFocus(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strOldText, strNewText;
	m_staSign.GetWindowText(strOldText);
	m_edtSign.GetWindowText(strNewText);
	if (strOldText != strNewText)
	{
		m_QQClient.ModifyQQSign(strNewText);
		m_staSign.SetWindowText(strNewText);
	}
	m_edtSign.ShowWindow(SW_HIDE);
	m_staSign.ShowWindow(SW_SHOW);
}

void CMainDlg::OnCbo_SelChange_Status(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strStatus;
	m_cboStatus.GetWindowText(strStatus);

	QQ_STATUS nNewStatus;
	if (strStatus == _T("我在线上"))
		nNewStatus = QQ_STATUS_ONLINE;
	else if (strStatus == _T("Q我吧"))
		nNewStatus = QQ_STATUS_CALLME;
	else if (strStatus == _T("离开"))
		nNewStatus = QQ_STATUS_AWAY;
	else if (strStatus == _T("忙碌"))
		nNewStatus = QQ_STATUS_BUSY;
	else if (strStatus == _T("请匆打扰"))
		nNewStatus = QQ_STATUS_SILENT;
	else if (strStatus == _T("隐身"))
		nNewStatus = QQ_STATUS_HIDDEN;

	if (nNewStatus != m_QQClient.GetStatus())
	{
		LoadAppIcon(nNewStatus);
		if (NULL == m_dwMsgTimerId)
			m_TrayIcon.ModifyIcon(m_hAppIcon, _T("QQ2011"));
		m_QQClient.ChangeStatus(nNewStatus);
	}
}

void CMainDlg::OnBtn_Clicked_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_QQClient.CancelLogin();
}

// 登录返回消息
LRESULT CMainDlg::OnLoginResult(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	QQ_LOGIN_RESULT_CODE nCode = (QQ_LOGIN_RESULT_CODE)lParam;

	KillTimer(m_dwLoginTimerId);
	m_dwLoginTimerId = NULL;

	QQ_STATUS nStatus = m_QQClient.GetStatus();

	LoadAppIcon(nStatus);
	m_TrayIcon.ModifyIcon(m_hAppIcon, _T("QQ2011"));

	switch (nCode)
	{
	case QLRC_SUCCESS:	// 登录成功
		{
			m_picHead.MoveWindow(2, 10, 40, 40, FALSE);
			m_staNickName.MoveWindow(48, 10, 130, 18, FALSE);
			m_staSign.MoveWindow(48, 28, 200, 18, FALSE);
			m_edtSign.MoveWindow(48, 28, 200, 18, FALSE);
			m_cboStatus.MoveWindow(178, 5, 70, 18, FALSE);

			m_staQQNum.ShowWindow(SW_HIDE);
			m_staLogining.ShowWindow(SW_HIDE);
			m_btnCancel.ShowWindow(SW_HIDE);

			m_picHead.ShowWindow(SW_SHOW);
			m_staNickName.ShowWindow(SW_SHOW);
			m_staSign.ShowWindow(SW_SHOW);
			m_edtSign.ShowWindow(SW_HIDE);
			m_cboStatus.ShowWindow(SW_SHOW);
			m_MainTabCtrl.ShowWindow(SW_SHOW);

			ShowWindow(SW_SHOW);

			int nIndex = 0;
			if (nStatus == QQ_STATUS_ONLINE)
				nIndex = 0;
			else if (nStatus == QQ_STATUS_CALLME)
				nIndex = 1;
			else if (nStatus == QQ_STATUS_AWAY)
				nIndex = 2;
			else if (nStatus == QQ_STATUS_BUSY)
				nIndex = 3;
			else if (nStatus == QQ_STATUS_SILENT)
				nIndex = 4;
			else if (nStatus == QQ_STATUS_HIDDEN)
				nIndex = 5;
			m_cboStatus.SetCurSel(nIndex);
			//MessageBox(_T("登录成功!"), _T("提示"), MB_OK);
		}
		break;

	case QLRC_FAILED:	// 登录失败
		{
			MessageBox(_T("登录失败!"), _T("提示"), MB_OK);
			StartLogin();
		}
		break;

	case QLRC_PASSWORD_ERROR:	// 密码错误
		{
			MessageBox(_T("密码错误!"), _T("提示"), MB_OK);
			StartLogin();
		}
		break;

	case QLRC_VERIFY_CODE_ERROR:	// 验证码错误
		{
			MessageBox(_T("验证码错误!"), _T("提示"), MB_OK);
		}
	case QLRC_NEED_VERIFY_CODE:	// 需要输入验证码
		{
			const BYTE * lpData = NULL;
			DWORD dwSize = 0;
			BOOL bRet = m_QQClient.GetVerifyCodePic(lpData, dwSize);
			if (bRet)
			{
				CVerifyCodeDlg VerifyCodeDlg;
				VerifyCodeDlg.m_Image.LoadFromBuffer(lpData, dwSize);
				if (VerifyCodeDlg.DoModal() == IDOK)
				{
					m_QQClient.SetVerifyCode(VerifyCodeDlg.m_strVerifyCode);
					m_QQClient.Login();
				}
			}
		}
		break;

	case QLRC_USER_CANCEL_LOGIN:	// 用户取消登录
		{
			StartLogin();
		}
		break;
	}

	return 0;
}

// 注销返回消息
LRESULT CMainDlg::OnLogoutResult(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CloseDialog(0);
	return 0;
}

// 更新用户信息
LRESULT CMainDlg::OnUpdateUserInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CBuddyInfoResult * lpBuddyInfoResult = (CBuddyInfoResult *)lParam;
	if (NULL == lpBuddyInfoResult)
		return 0;
	m_QQClient.OnUpdateUserInfo(lpBuddyInfoResult);
	delete lpBuddyInfoResult;

	return 0;
}

// 更新好友列表消息
LRESULT CMainDlg::OnUpdateBuddyList(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CBuddyListResult * lpBuddyListResult = (CBuddyListResult *)lParam;
	if (NULL == lpBuddyListResult)
		return 0;
	m_QQClient.OnUpdateBuddyList(lpBuddyListResult);
	delete lpBuddyListResult;

	UpdateBuddyTreeCtrl();

	return 0;
}

// 更新群列表消息
LRESULT CMainDlg::OnUpdateGroupList(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CGroupListResult * lpGroupListResult = (CGroupListResult *)lParam;
	if (NULL == lpGroupListResult)
		return 0;
	m_QQClient.OnUpdateGroupList(lpGroupListResult);
	delete lpGroupListResult;

	UpdateGroupTreeCtrl();

	return 0;
}

// 更新最近联系人列表消息
LRESULT CMainDlg::OnUpdateRecentList(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CRecentListResult * lpRecentListResult = (CRecentListResult *)lParam;
	if (NULL == lpRecentListResult)
		return 0;
	m_QQClient.OnUpdateRecentList(lpRecentListResult);
	delete lpRecentListResult;

	UpdateRecentTreeCtrl();

	return 0;
}

// 好友消息
LRESULT CMainDlg::OnBuddyMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CBuddyMessage * lpBuddyMsg = (CBuddyMessage *)lParam;
	if (NULL == lpBuddyMsg)
		return 0;
	m_QQClient.OnBuddyMsg(lpBuddyMsg);
	
	UINT nQQUin = lpBuddyMsg->m_nFromUin;
	UINT nMsgId = lpBuddyMsg->m_nMsgId;

	CString strFileName = ZYM::CPath::GetCurDir() + _T("Sound\\msg.wav");	// 播放来消息提示音
	::sndPlaySound(strFileName, SND_ASYNC);

	std::map<UINT, CBuddyChatDlg *>::iterator iter;
	iter = m_mapBuddyChatDlg.find(nQQUin);
	if (iter != m_mapBuddyChatDlg.end())
	{
		CBuddyChatDlg * lpBuddyDlg = iter->second;
		if (lpBuddyDlg != NULL && lpBuddyDlg->IsWindow())
		{
			lpBuddyDlg->OnRecvMsg(nQQUin, nMsgId);
			return 0;
		}
	}

	UpdateMsgIcon();

	if (m_MsgTipDlg.IsWindow())
		m_MsgTipDlg.AddMsgSender(QQ_MSG_TYPE_BUDDY, nQQUin);
	
	if (NULL == m_dwMsgTimerId)
		m_dwMsgTimerId = SetTimer(2, ::GetCaretBlinkTime(), NULL);

	return 0;
}

// 群消息
LRESULT CMainDlg::OnGroupMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CGroupMessage * lpGroupMsg = (CGroupMessage *)lParam;
	if (NULL == lpGroupMsg)
		return 0;
	m_QQClient.OnGroupMsg(lpGroupMsg);

	UINT nGroupCode = lpGroupMsg->m_nGroupCode;
	UINT nMsgId = lpGroupMsg->m_nMsgId;

	std::map<UINT, CGroupChatDlg *>::iterator iter;
	iter = m_mapGroupChatDlg.find(nGroupCode);
	if (iter != m_mapGroupChatDlg.end())
	{
		CGroupChatDlg * lpGroupDlg = iter->second;
		if (lpGroupDlg != NULL && lpGroupDlg->IsWindow())
		{
			lpGroupDlg->OnRecvMsg(nGroupCode, nMsgId);
			return 0;
		}
	}

	CMessageList * lpMsgList = m_QQClient.GetMessageList();
	if (lpMsgList != NULL)
	{
		CMessageSender * lpMsgSender = lpMsgList->GetMsgSender(QQ_MSG_TYPE_GROUP, nGroupCode);
		if (lpMsgSender != NULL)
		{
			if (lpMsgSender->GetMsgCount() == 1)
			{
				CString strFileName = ZYM::CPath::GetCurDir() + _T("Sound\\msg.wav");	// 播放来消息提示音
				::sndPlaySound(strFileName, SND_ASYNC);
			}
		}
	}

	UpdateMsgIcon();

	if (m_MsgTipDlg.IsWindow())
		m_MsgTipDlg.AddMsgSender(QQ_MSG_TYPE_GROUP, nGroupCode);

	if (NULL == m_dwMsgTimerId)
		m_dwMsgTimerId = SetTimer(2, ::GetCaretBlinkTime(), NULL);

	return 0;
}

// 临时会话消息
LRESULT CMainDlg::OnSessMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CSessMessage * lpSessMsg = (CSessMessage *)lParam;
	if (NULL == lpSessMsg)
		return 0;
	m_QQClient.OnSessMsg(lpSessMsg);

	UINT nQQUin = lpSessMsg->m_nFromUin;
	UINT nMsgId = lpSessMsg->m_nMsgId;
	UINT nGroupId = lpSessMsg->m_nGroupId;

	CString strFileName = ZYM::CPath::GetCurDir() + _T("Sound\\msg.wav");	// 播放来消息提示音
	::sndPlaySound(strFileName, SND_ASYNC);

	std::map<UINT, CSessChatDlg *>::iterator iter;
	iter = m_mapSessChatDlg.find(nQQUin);
	if (iter != m_mapSessChatDlg.end())
	{
		CSessChatDlg * lpSessChatDlg = iter->second;
		if (lpSessChatDlg != NULL && lpSessChatDlg->IsWindow())
		{
			lpSessChatDlg->OnRecvMsg(nQQUin, nMsgId);
			return 0;
		}
	}

	UpdateMsgIcon();

	if (m_MsgTipDlg.IsWindow())
		m_MsgTipDlg.AddMsgSender(QQ_MSG_TYPE_SESS, nQQUin);

	if (NULL == m_dwMsgTimerId)
		m_dwMsgTimerId = SetTimer(2, ::GetCaretBlinkTime(), NULL);

	return 0;
}

// 好友状态改变消息
LRESULT CMainDlg::OnStatusChangeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CStatusChangeMessage * lpStatusChangeMsg = (CStatusChangeMessage * )lParam;
	if (NULL == lpStatusChangeMsg)
		return 0;
	m_QQClient.OnStatusChangeMsg(lpStatusChangeMsg);
	QQ_STATUS nStatus = lpStatusChangeMsg->m_nStatus;
	delete lpStatusChangeMsg;

	UpdateBuddyTreeCtrl();

	if (nStatus != QQ_STATUS_OFFLINE)	// 播放好友上线声音
	{
		CString strFileName = ZYM::CPath::GetCurDir() + _T("Sound\\Global.wav");
		::sndPlaySound(strFileName, SND_ASYNC);
	}

	return 0;
}

// 被踢下线消息
LRESULT CMainDlg::OnKickMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CKickMessage * lpKickMsg = (CKickMessage *)lParam;
	if (NULL == lpKickMsg)
		return 0;

	if (lpKickMsg->m_bShowReason)
	{
		CString strReason = lpKickMsg->m_strReason.c_str();
		MessageBox(strReason, _T("提示"), MB_OK);
	}

	delete lpKickMsg;

	return 0;
}

// 群系统消息
LRESULT CMainDlg::OnSysGroupMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CSysGroupMessage * lpSysGroupMsg = (CSysGroupMessage * )lParam;
	if (NULL == lpSysGroupMsg)
		return 0;
	m_QQClient.OnSysGroupMsg(lpSysGroupMsg);
	UINT nGroupCode = lpSysGroupMsg->m_nGroupCode;

	CString strFileName = ZYM::CPath::GetCurDir() + _T("Sound\\system.wav");	// 播放来系统消息提示音
	::sndPlaySound(strFileName, SND_ASYNC);

	UpdateMsgIcon();

	if (m_MsgTipDlg.IsWindow())
		m_MsgTipDlg.AddMsgSender(QQ_MSG_TYPE_SYSGROUP, nGroupCode);

	if (NULL == m_dwMsgTimerId)
		m_dwMsgTimerId = SetTimer(2, ::GetCaretBlinkTime(), NULL);

	return 0;
}

// 更新好友号码
LRESULT CMainDlg::OnUpdateBuddyNumber(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CGetQQNumResult * lpGetQQNumResult = (CGetQQNumResult *)lParam;
	if (NULL == lpGetQQNumResult)
		return 0;
	m_QQClient.OnUpdateBuddyNumber(lpGetQQNumResult);
	UINT nQQUin = lpGetQQNumResult->m_nQQUin;
	UINT nQQNum = lpGetQQNumResult->m_nQQNum;
	delete lpGetQQNumResult;

	NotifyBuddyChatDlg(nQQUin, QQ_MSG_UPDATE_BUDDY_NUMBER);		// 通知好友聊天窗口更新
	NotifyBuddyInfoDlg(nQQUin, QQ_MSG_UPDATE_BUDDY_NUMBER);		// 通知好友信息窗口更新

	return 0;
}

// 更新群成员号码
LRESULT CMainDlg::OnUpdateGMemberNumber(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	CGetQQNumResult * lpGetQQNumResult = (CGetQQNumResult *)lParam;
	m_QQClient.OnUpdateGMemberNumber(nGroupCode, lpGetQQNumResult);
	NotifyGroupChatDlg(nGroupCode, QQ_MSG_UPDATE_GMEMBER_NUMBER, wParam, lParam);// 通知群聊天窗口更新
	if (NULL == lpGetQQNumResult)
		return 0;
	UINT nQQUin = lpGetQQNumResult->m_nQQUin;
	UINT nQQNum = lpGetQQNumResult->m_nQQNum;
	delete lpGetQQNumResult;

	NotifyGMemberInfoDlg(nGroupCode, nQQUin, QQ_MSG_UPDATE_GMEMBER_NUMBER);	// 通知群成员信息窗口更新

	return 0;
}

// 更新群号码
LRESULT CMainDlg::OnUpdateGroupNumber(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	CGetQQNumResult * lpGetQQNumResult = (CGetQQNumResult *)lParam;
	if (NULL == lpGetQQNumResult)
		return 0;
	m_QQClient.OnUpdateGroupNumber(nGroupCode, lpGetQQNumResult);
	UINT nGroupNum = lpGetQQNumResult->m_nQQNum;
	delete lpGetQQNumResult;

	NotifyGroupChatDlg(nGroupCode, QQ_MSG_UPDATE_GROUP_NUMBER, 0, 0);// 通知群聊天窗口更新
	NotifyGroupInfoDlg(nGroupCode, QQ_MSG_UPDATE_GROUP_NUMBER);		// 通知群信息窗口更新

	return 0;
}

// 更新好友个性签名
LRESULT CMainDlg::OnUpdateBuddySign(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CGetSignResult * lpGetSignResult = (CGetSignResult *)lParam;
	if (NULL == lpGetSignResult)
		return 0;
	m_QQClient.OnUpdateBuddySign(lpGetSignResult);
	UINT nQQUin = lpGetSignResult->m_nQQUin;
	delete lpGetSignResult;

	CBuddyInfo * lpBuddyInfo = m_QQClient.GetUserInfo();
	if (lpBuddyInfo != NULL && lpBuddyInfo->m_nQQUin == nQQUin)	// 更新用户个性签名
	{
		if (!lpBuddyInfo->m_strSign.empty())
			m_staSign.SetWindowText(lpBuddyInfo->m_strSign.c_str());
		else
			m_staSign.SetWindowText(_T("这家伙很懒,什么都没有留下。"));
	}

	NotifyBuddyChatDlg(nQQUin, QQ_MSG_UPDATE_BUDDY_SIGN);		// 通知好友聊天窗口更新
	NotifyBuddyInfoDlg(nQQUin, QQ_MSG_UPDATE_BUDDY_SIGN);		// 通知好友信息窗口更新

	return 0;
}

// 更新群成员个性签名
LRESULT CMainDlg::OnUpdateGMemberSign(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	CGetSignResult * lpGetSignResult = (CGetSignResult *)lParam;
	if (NULL == lpGetSignResult)
		return 0;
	m_QQClient.OnUpdateGMemberSign(nGroupCode, lpGetSignResult);
	UINT nQQUin = lpGetSignResult->m_nQQUin;
	delete lpGetSignResult;

	NotifyGMemberInfoDlg(nGroupCode, nQQUin, QQ_MSG_UPDATE_GMEMBER_SIGN);	// 通知群成员信息窗口更新

	return 0;
}

// 更新好友信息
LRESULT CMainDlg::OnUpdateBuddyInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CBuddyInfoResult * lpBuddyInfoResult = (CBuddyInfoResult *)lParam;
	if (NULL == lpBuddyInfoResult)
		return 0;
	m_QQClient.OnUpdateBuddyInfo(lpBuddyInfoResult);
	UINT nQQUin = lpBuddyInfoResult->m_nQQUin;
	delete lpBuddyInfoResult;

	CBuddyInfo * lpBuddyInfo = m_QQClient.GetUserInfo();
	if (lpBuddyInfo != NULL && lpBuddyInfo->m_nQQUin == nQQUin)	// 更新用户昵称
		m_staNickName.SetWindowText(lpBuddyInfo->m_strNickName.c_str());

	NotifyBuddyInfoDlg(nQQUin, QQ_MSG_UPDATE_BUDDY_INFO);		// 通知好友信息窗口更新

	return 0;
}

// 更新群成员信息
LRESULT CMainDlg::OnUpdateGMemberInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	CBuddyInfoResult * lpBuddyInfoResult = (CBuddyInfoResult *)lParam;
	if (NULL == lpBuddyInfoResult)
		return 0;
	m_QQClient.OnUpdateGMemberInfo(nGroupCode, lpBuddyInfoResult);
	UINT nQQUin = lpBuddyInfoResult->m_nQQUin;
	delete lpBuddyInfoResult;

	NotifyGMemberInfoDlg(nGroupCode, nQQUin, QQ_MSG_UPDATE_GMEMBER_INFO);	// 通知群成员信息窗口更新

	return 0;
}

// 更新群信息
LRESULT CMainDlg::OnUpdateGroupInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	CGroupInfoResult * lpGroupInfoResult = (CGroupInfoResult *)lParam;
	if (NULL == lpGroupInfoResult)
		return 0;
	m_QQClient.OnUpdateGroupInfo(nGroupCode, lpGroupInfoResult);
	delete lpGroupInfoResult;

	NotifyGroupChatDlg(nGroupCode, QQ_MSG_UPDATE_GROUP_INFO, 0, 0);	// 通知群聊天窗口更新
	NotifyGroupInfoDlg(nGroupCode, QQ_MSG_UPDATE_GROUP_INFO);		// 通知群信息窗口更新

	return 0;
}

// 更新临时会话信令
LRESULT CMainDlg::OnUpdateC2CMsgSig(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CGetC2CMsgSigResult * lpGetC2CMsgSigResult = (CGetC2CMsgSigResult *)lParam;
	if (NULL == lpGetC2CMsgSigResult)
		return 0;
	m_QQClient.OnUpdateC2CMsgSig(lpGetC2CMsgSigResult);
	UINT nQQUin = lpGetC2CMsgSigResult->m_nQQUin;
	delete lpGetC2CMsgSigResult;

	NotifySessChatDlg(nQQUin, QQ_MSG_UPDATE_C2CMSGSIG);	// 通知临时会话聊天窗口更新
	return 0;
}


HTREEITEM CMainDlg::FindTreeItemByQQUin(CTreeViewCtrl& treeCtrl, UINT nQQUin)
{
	HTREEITEM hItem = treeCtrl.GetRootItem();
	while (hItem != NULL)
	{
		UINT nQQUin2 = (UINT)treeCtrl.GetItemData(hItem);
		if (nQQUin2 == nQQUin)
			return hItem;

		HTREEITEM hFindItem = FindTreeItemByQQUin2(treeCtrl, hItem, nQQUin);
		if (hFindItem != NULL)
			return hFindItem;

		hItem = treeCtrl.GetNextItem(hItem, TVGN_NEXT);
	}
	return NULL;
}

HTREEITEM CMainDlg::FindTreeItemByQQUin2(CTreeViewCtrl& treeCtrl, HTREEITEM hItem, UINT nQQUin)
{
	if (treeCtrl.ItemHasChildren(hItem))
	{
		HTREEITEM hChildItem = treeCtrl.GetChildItem(hItem);

		while (hChildItem != NULL)
		{
			UINT nQQUin2 = (UINT)treeCtrl.GetItemData(hItem);
			if (nQQUin2 == nQQUin)
				return hItem;

			HTREEITEM hFindItem = FindTreeItemByQQUin2(treeCtrl, hChildItem, nQQUin);
			if (hFindItem != NULL)
				return hFindItem;

			hChildItem = treeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
		}
	}

	UINT nQQUin2 = (UINT)treeCtrl.GetItemData(hItem);
	return  (nQQUin2 == nQQUin) ? hItem : NULL;
}

// 更新好友头像图片
LRESULT CMainDlg::OnUpdateBuddyHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nQQUin = (UINT)lParam;

	CBuddyInfo * lpBuddyInfo = m_QQClient.GetUserInfo();
	if (lpBuddyInfo != NULL && lpBuddyInfo->m_nQQUin == nQQUin)	// 更新用户头像
	{
		if (!m_imgHead.IsNull())
			m_imgHead.Destroy();
		CString strFileName = m_QQClient.GetHeadPicFullName(0, nQQUin).c_str();
		m_imgHead.Load(strFileName);
		m_picHead.SetBitmap((HBITMAP)m_imgHead);
		return 0;
	}

	CTreeViewCtrl& treeBuddy = m_MainTabCtrl.m_treeBuddy;
	CTreeViewCtrl& treeRecent = m_MainTabCtrl.m_treeRecent;

	BOOL bGray = FALSE;
	CBuddyList * lpBuddyList = m_QQClient.GetBuddyList();
	if (lpBuddyList != NULL)
	{
		CBuddyInfo * lpBuddyInfo = lpBuddyList->GetBuddy(nQQUin);
		if (lpBuddyInfo != NULL)
			bGray = lpBuddyInfo->m_nStatus != QQ_STATUS_OFFLINE ? FALSE : TRUE;
	}

	int nIndex = m_headImageList.GetHeadPicIndex(QQ_MSG_TYPE_BUDDY, 0, nQQUin, bGray, TRUE);

	HTREEITEM hTreeItem = FindTreeItemByQQUin(treeBuddy, nQQUin);
	if (hTreeItem != NULL)
		treeBuddy.SetItemImage(hTreeItem, nIndex, nIndex);

	hTreeItem = FindTreeItemByQQUin(treeRecent, nQQUin);
	if (hTreeItem != NULL)
		treeRecent.SetItemImage(hTreeItem, nIndex, nIndex);

	return 0;
}

// 更新群成员头像图片
LRESULT CMainDlg::OnUpdateGMemberHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nQQUin = (UINT)lParam;
	return 0;
}

// 更新群头像图片
LRESULT CMainDlg::OnUpdateGroupHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;

	CTreeViewCtrl& treeGroup = m_MainTabCtrl.m_treeGroup;
	CTreeViewCtrl& treeRecent = m_MainTabCtrl.m_treeRecent;

	int nIndex = m_headImageList.GetHeadPicIndex(QQ_MSG_TYPE_GROUP, nGroupCode, 0, FALSE, TRUE);

	HTREEITEM hTreeItem = FindTreeItemByQQUin(treeGroup, nGroupCode);
	if (hTreeItem != NULL)
		treeGroup.SetItemImage(hTreeItem, nIndex, nIndex);

	hTreeItem = FindTreeItemByQQUin(treeRecent, nGroupCode);
	if (hTreeItem != NULL)
		treeRecent.SetItemImage(hTreeItem, nIndex, nIndex);

	return 0;
}

// 改变在线状态返回消息
LRESULT CMainDlg::OnChangeStatusResult(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bSuccess = (BOOL)wParam;
	QQ_STATUS nNewStatus = (QQ_STATUS)lParam;
	m_QQClient.OnChangeStatusResult(bSuccess, nNewStatus);
	if (!bSuccess)
		MessageBox(_T("改变在线状态失败！"));
	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	if (m_dwLoginTimerId != NULL)
	{
		KillTimer(m_dwLoginTimerId);
		m_dwLoginTimerId = NULL;
	}
	if (m_dwMsgTimerId != NULL)
	{
		KillTimer(m_dwMsgTimerId);
		m_dwMsgTimerId = NULL;
	}

	m_TrayIcon.RemoveIcon();
	CloseAllDlg();
	ShowWindow(SW_HIDE);

	if (m_QQClient.GetStatus() != QQ_STATUS_OFFLINE)
	{
		m_QQClient.Logout();
	}
	else
	{
		DestroyWindow();
		::PostQuitMessage(nVal);
	}
}

LRESULT CMainDlg::OnTrayIconNotify(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_TrayIcon.OnTrayIconNotify(wParam, lParam);

	UINT uID = (UINT)wParam;
	UINT uIconMsg = (UINT)lParam;
	
	switch (uIconMsg)
	{
	case WM_LBUTTONUP:
		OnTrayIcon_LButtunUp();
		break;
	case WM_RBUTTONUP:
		OnTrayIcon_RButtunUp();
		break;
	case WM_MOUSEHOVER:
		OnTrayIcon_MouseHover();
		break;
	case WM_MOUSELEAVE:
		OnTrayIcon_MouseLeave();
		break;
	}
	return 0;
}

LRESULT CMainDlg::OnShowOrCloseDlg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nQQUin = (UINT)lParam;

	switch (uMsg)
	{
	case WM_SHOW_BUDDYCHATDLG:
		ShowBuddyChatDlg(nQQUin, TRUE);
		break;
	case WM_SHOW_GROUPCHATDLG:
		ShowGroupChatDlg(nGroupCode, TRUE);
		break;
	case WM_SHOW_SESSCHATDLG:
		ShowSessChatDlg(nGroupCode, nQQUin, TRUE);
		break;
	case WM_SHOW_SYSGROUPCHATDLG:
		ShowSysGroupChatDlg(nGroupCode, TRUE);
		break;
	case WM_SHOW_BUDDYINFODLG:
		ShowBuddyInfoDlg(nQQUin, TRUE);
		break;
	case WM_SHOW_GMEMBERINFODLG:
		ShowGMemberInfoDlg(nGroupCode, nQQUin, TRUE);
		break;
	case WM_SHOW_GROUPINFODLG:
		ShowGroupInfoDlg(nGroupCode, TRUE);
		break;
	case WM_CLOSE_BUDDYCHATDLG:
		ShowBuddyChatDlg(nQQUin, FALSE);
		break;
	case WM_CLOSE_GROUPCHATDLG:
		ShowGroupChatDlg(nGroupCode, FALSE);
		break;
	case WM_CLOSE_SESSCHATDLG:
		ShowSessChatDlg(nGroupCode, nQQUin, FALSE);
		break;
	case WM_CLOSE_SYSGROUPCHATDLG:
		ShowSysGroupChatDlg(nGroupCode, FALSE);
		break;
	case WM_CLOSE_BUDDYINFODLG:
		ShowBuddyInfoDlg(nQQUin, FALSE);
		break;
	case WM_CLOSE_GMEMBERINFODLG:
		ShowGMemberInfoDlg(nGroupCode, nQQUin, FALSE);
		break;
	case WM_CLOSE_GROUPINFODLG:
		ShowGroupInfoDlg(nGroupCode, FALSE);
		break;
	}
	return 0;
}

LRESULT CMainDlg::OnDelMsgSender(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	QQ_MSG_TYPE nType = (QQ_MSG_TYPE)wParam;
	UINT nSenderId = (UINT)lParam;

	if (m_MsgTipDlg.IsWindow())
		m_MsgTipDlg.DelMsgSender(nType, nSenderId);

	UpdateMsgIcon();

	CMessageList * lpMsgList = m_QQClient.GetMessageList();
	if (lpMsgList != NULL && lpMsgList->GetMsgSenderCount() <= 0)
	{
		if (m_MsgTipDlg.IsWindow())
			m_MsgTipDlg.DestroyWindow();
		KillTimer(m_dwMsgTimerId);
		m_dwMsgTimerId = NULL;
		m_nLastMsgType = QQ_MSG_TYPE_BUDDY;
		m_nLastSenderId = 0;
		m_TrayIcon.ModifyIcon(m_hAppIcon, _T("QQ2011"));
	}

	return 0;
}

LRESULT CMainDlg::OnCancelFlash(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	KillTimer(m_dwMsgTimerId);
	m_dwMsgTimerId = NULL;
	m_TrayIcon.ModifyIcon(m_hAppIcon, _T("QQ2011"));
	return 0;
}

LRESULT CMainDlg::OnTabCtrlSelChange(LPNMHDR pnmh)
{
	int nCurSel = m_MainTabCtrl.GetCurSel();
	m_MainTabCtrl.OnSelChange(nCurSel);

	return 0;
}

LRESULT CMainDlg::OnBuddyTreeDblClk(LPNMHDR pnmh)
{
	POINT point;
	UINT uFlags;

	GetCursorPos(&point);
	m_MainTabCtrl.m_treeBuddy.ScreenToClient(&point);
	HTREEITEM hTreeItem = m_MainTabCtrl.m_treeBuddy.HitTest(point, &uFlags);
	if ((hTreeItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		UINT nQQUin = (UINT)m_MainTabCtrl.m_treeBuddy.GetItemData(hTreeItem);
		// 直接在树控件双击消息响应函数里面打开窗口会导致弹出的窗口无焦点(非激活状态)
		// ShowBuddyChatDlg(nQQUin, TRUE);
		PostMessage(WM_SHOW_BUDDYCHATDLG, 0, nQQUin);
	}
	return 0;
}

LRESULT CMainDlg::OnGroupTreeDblClk(LPNMHDR pnmh)
{
	POINT point;
	UINT uFlags;

	GetCursorPos(&point);
	m_MainTabCtrl.m_treeGroup.ScreenToClient(&point);
	HTREEITEM hTreeItem = m_MainTabCtrl.m_treeGroup.HitTest(point, &uFlags);
	if ((hTreeItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		UINT nGroupCode = (UINT)m_MainTabCtrl.m_treeGroup.GetItemData(hTreeItem);
		// 直接在树控件双击消息响应函数里面打开窗口会导致弹出的窗口无焦点(非激活状态)
		// ShowGroupChatDlg(nGroupCode, TRUE);
		PostMessage(WM_SHOW_GROUPCHATDLG, nGroupCode, 0);
	}
	return 0;
}

LRESULT CMainDlg::OnRecentTreeDblClk(LPNMHDR pnmh)
{
	POINT point;
	UINT uFlags;

	GetCursorPos(&point);
	m_MainTabCtrl.m_treeRecent.ScreenToClient(&point);
	HTREEITEM hTreeItem = m_MainTabCtrl.m_treeRecent.HitTest(point, &uFlags);
	if ((hTreeItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		int nIndex = (UINT)m_MainTabCtrl.m_treeRecent.GetItemData(hTreeItem);

		CRecentList * lpRecentList = m_QQClient.GetRecentList();
		if (lpRecentList != NULL)
		{
			CRecentInfo * lpRecentInfo = lpRecentList->GetRecent(nIndex);
			if (lpRecentInfo != NULL)
			{
				if (0 == lpRecentInfo->m_nType)			// 好友
				{
					// 直接在树控件双击消息响应函数里面打开窗口会导致弹出的窗口无焦点(非激活状态)
					// ShowBuddyChatDlg(lpRecentInfo->m_nQQUin, TRUE);
					PostMessage(WM_SHOW_BUDDYCHATDLG, 0, lpRecentInfo->m_nQQUin);
				}
				else if (1 == lpRecentInfo->m_nType)	// 群
				{
					CGroupList * lpGroupList = m_QQClient.GetGroupList();
					if (lpGroupList != NULL)
					{
						CGroupInfo * lpGroupInfo = lpGroupList->GetGroupById(lpRecentInfo->m_nQQUin);
						if (lpGroupInfo != NULL)
						{
							// 直接在树控件双击消息响应函数里面打开窗口会导致弹出的窗口无焦点(非激活状态)
							// ShowGroupChatDlg(lpGroupInfo->m_nGroupCode, TRUE);
							PostMessage(WM_SHOW_GROUPCHATDLG, lpGroupInfo->m_nGroupCode, 0);
						}
					}
				}
			}
		}
	}
	
	return 0;
}

// “打开主面板”菜单
void CMainDlg::OnMenu_OpenMainPanel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (m_LoginDlg.IsWindow())
	{
		m_LoginDlg.ShowWindow(SW_SHOW);
		::SetForegroundWindow(m_LoginDlg.m_hWnd);
	}
	else if (IsWindow())
	{
		ShowWindow(SW_SHOW);
		::SetForegroundWindow(m_hWnd);
	}
}

// “退出”菜单
void CMainDlg::OnMenu_Exit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	SendMessage(WM_CLOSE);
}

void CMainDlg::ShowBuddyChatDlg(UINT nQQUin, BOOL bShow)
{
	if (nQQUin == 0)
		return;

	if (bShow)
	{
		std::map<UINT, CBuddyChatDlg *>::iterator iter;
		iter = m_mapBuddyChatDlg.find(nQQUin);
		if (iter != m_mapBuddyChatDlg.end())
		{
			CBuddyChatDlg * lpBuddyChatDlg = iter->second;
			if (lpBuddyChatDlg != NULL)
			{
				if (!lpBuddyChatDlg->IsWindow())
					lpBuddyChatDlg->Create(NULL);
				lpBuddyChatDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyChatDlg->m_hWnd);
				lpBuddyChatDlg->OnRecvMsg(nQQUin, NULL);
			}
		}
		else
		{
			CBuddyChatDlg * lpBuddyChatDlg = new CBuddyChatDlg;
			if (lpBuddyChatDlg != NULL)
			{
				lpBuddyChatDlg->m_lpQQClient = &m_QQClient;
				lpBuddyChatDlg->m_lpFaceList = &m_FaceList;
				lpBuddyChatDlg->m_hMainDlg = m_hWnd;
				lpBuddyChatDlg->m_nQQUin = nQQUin;
				m_mapBuddyChatDlg[nQQUin] = lpBuddyChatDlg;
				lpBuddyChatDlg->Create(NULL);
				lpBuddyChatDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyChatDlg->m_hWnd);
				lpBuddyChatDlg->OnRecvMsg(nQQUin, NULL);
			}
		}
	}
	else
	{
		std::map<UINT, CBuddyChatDlg *>::iterator iter;
		iter = m_mapBuddyChatDlg.find(nQQUin);
		if (iter != m_mapBuddyChatDlg.end())
		{
			CBuddyChatDlg * lpBuddyChatDlg = iter->second;
			if (lpBuddyChatDlg != NULL)
			{
				if (lpBuddyChatDlg->IsWindow())
					lpBuddyChatDlg->DestroyWindow();
				delete lpBuddyChatDlg;
			}
			m_mapBuddyChatDlg.erase(iter);
		}
	}
}

void CMainDlg::ShowGroupChatDlg(UINT nGroupCode, BOOL bShow)
{
	if (nGroupCode == 0)
		return;

	if (bShow)
	{
		std::map<UINT, CGroupChatDlg *>::iterator iter;
		iter = m_mapGroupChatDlg.find(nGroupCode);
		if (iter != m_mapGroupChatDlg.end())
		{
			CGroupChatDlg * lpGroupChatDlg = iter->second;
			if (lpGroupChatDlg != NULL)
			{
				if (!lpGroupChatDlg->IsWindow())
					lpGroupChatDlg->Create(NULL);
				lpGroupChatDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpGroupChatDlg->m_hWnd);
				lpGroupChatDlg->OnRecvMsg(nGroupCode, NULL);
			}
		}
		else
		{		
			CGroupChatDlg * lpGroupChatDlg = new CGroupChatDlg;
			if (lpGroupChatDlg != NULL)
			{
				lpGroupChatDlg->m_lpQQClient = &m_QQClient;
				lpGroupChatDlg->m_lpFaceList = &m_FaceList;
				lpGroupChatDlg->m_hMainDlg = m_hWnd;
				lpGroupChatDlg->m_nGroupCode = nGroupCode;
				m_mapGroupChatDlg[nGroupCode] = lpGroupChatDlg;
				lpGroupChatDlg->Create(NULL);
				lpGroupChatDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpGroupChatDlg->m_hWnd);
				lpGroupChatDlg->OnRecvMsg(nGroupCode, NULL);
			}
		}
	}
	else
	{
		std::map<UINT, CGroupChatDlg *>::iterator iter;
		iter = m_mapGroupChatDlg.find(nGroupCode);
		if (iter != m_mapGroupChatDlg.end())
		{
			CGroupChatDlg * lpGroupChatDlg = iter->second;
			if (lpGroupChatDlg != NULL)
			{
				if (lpGroupChatDlg->IsWindow())
					lpGroupChatDlg->DestroyWindow();
				delete lpGroupChatDlg;
			}
			m_mapGroupChatDlg.erase(iter);
		}
	}
}

void CMainDlg::ShowSessChatDlg(UINT nGroupCode, UINT nQQUin, BOOL bShow)
{
	if (nQQUin == 0)
		return;

	if (bShow)
	{
		std::map<UINT, CSessChatDlg *>::iterator iter;
		iter = m_mapSessChatDlg.find(nQQUin);
		if (iter != m_mapSessChatDlg.end())
		{
			CSessChatDlg * lpSessChatDlg = iter->second;
			if (lpSessChatDlg != NULL)
			{
				if (!lpSessChatDlg->IsWindow())
					lpSessChatDlg->Create(NULL);
				lpSessChatDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpSessChatDlg->m_hWnd);
				lpSessChatDlg->OnRecvMsg(nQQUin, NULL);
			}
		}
		else
		{
			CSessChatDlg * lpSessChatDlg = new CSessChatDlg;
			if (lpSessChatDlg != NULL)
			{
				lpSessChatDlg->m_lpQQClient = &m_QQClient;
				lpSessChatDlg->m_lpFaceList = &m_FaceList;
				lpSessChatDlg->m_hMainDlg = m_hWnd;
				lpSessChatDlg->m_nGroupCode = nGroupCode;
				lpSessChatDlg->m_nQQUin = nQQUin;
				m_mapSessChatDlg[nQQUin] = lpSessChatDlg;
				lpSessChatDlg->Create(NULL);
				lpSessChatDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpSessChatDlg->m_hWnd);
				lpSessChatDlg->OnRecvMsg(nQQUin, NULL);
			}
		}
	}
	else
	{
		std::map<UINT, CSessChatDlg *>::iterator iter;
		iter = m_mapSessChatDlg.find(nQQUin);
		if (iter != m_mapSessChatDlg.end())
		{
			CSessChatDlg * lpSessChatDlg = iter->second;
			if (lpSessChatDlg != NULL)
			{
				if (lpSessChatDlg->IsWindow())
					lpSessChatDlg->DestroyWindow();
				delete lpSessChatDlg;
			}
			m_mapSessChatDlg.erase(iter);
		}
	}
}

void CMainDlg::ShowSysGroupChatDlg(UINT nGroupCode, BOOL bShow)
{
	CString strText, strGroupName, strAdminName, strAdminUin, strMsg;
	UINT nGroupNumber;

	CGroupList * lpGroupList = m_QQClient.GetGroupList();
	CMessageList * lpMsgList = m_QQClient.GetMessageList();
	if (NULL == lpGroupList || NULL == lpMsgList)
		return;

	CGroupInfo * lpGroupInfo = lpGroupList->GetGroupByCode(nGroupCode);
	CMessageSender * lpMsgSender = lpMsgList->GetMsgSender(QQ_MSG_TYPE_SYSGROUP, nGroupCode);
	if (NULL == lpGroupInfo || NULL == lpMsgSender)
		return;

	strGroupName = lpGroupInfo->m_strName.c_str();

	int nMsgCnt = lpMsgSender->GetMsgCount();
	for (int i = 0; i < nMsgCnt; i++)
	{
		CSysGroupMessage * lpSysGroupMsg = lpMsgSender->GetSysGroupMsg(i);
		if (lpSysGroupMsg != NULL)
		{
			strAdminName = lpSysGroupMsg->m_strAdminNickName.c_str();
			strAdminUin = lpSysGroupMsg->m_strAdminUin.c_str();
			strMsg = lpSysGroupMsg->m_strMsg.c_str();
			nGroupNumber = lpSysGroupMsg->m_nGroupNumber;

			if (lpSysGroupMsg->m_strSubType == _T("group_request_join_agree"))
			{
				LPCTSTR lpFmt = _T("管理员已同意您加入群%s(%u)");
				strText.Format(lpFmt, strGroupName, nGroupNumber);
				MessageBox(strText, _T("提示"), MB_OK);
			}
			else if (lpSysGroupMsg->m_strSubType == _T("group_request_join_deny"))
			{
				LPCTSTR lpFmt = _T("管理员已拒绝您加入群%s(%u)。拒绝理由：%s");
				strText.Format(lpFmt, strGroupName, nGroupNumber, strMsg);
				MessageBox(strText, _T("提示"), MB_OK);
			}
			else if (lpSysGroupMsg->m_strSubType == _T("group_leave"))
			{
				LPCTSTR lpFmt = _T("%s(%u)管理员%s(%s)已将您移除出该群。");
				strText.Format(lpFmt, strGroupName, nGroupNumber, strAdminName, strAdminUin);
				MessageBox(strText, _T("提示"), MB_OK);
			}
		}
	}
	lpMsgList->DelMsgSender(QQ_MSG_TYPE_SYSGROUP, nGroupCode);
	OnDelMsgSender(NULL, QQ_MSG_TYPE_SYSGROUP, nGroupCode);
}

void CMainDlg::ShowBuddyInfoDlg(UINT nQQUin, BOOL bShow)
{
	if (nQQUin == 0)
		return;

	if (bShow)
	{
		std::map<UINT, CBuddyInfoDlg *>::iterator iter;
		iter = m_mapBuddyInfoDlg.find(nQQUin);
		if (iter != m_mapBuddyInfoDlg.end())
		{
			CBuddyInfoDlg * lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (!lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->Create(NULL);
				lpBuddyInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyInfoDlg->m_hWnd);
			}
		}
		else
		{
			CBuddyInfoDlg * lpBuddyInfoDlg = new CBuddyInfoDlg;
			if (lpBuddyInfoDlg != NULL)
			{
				m_mapBuddyInfoDlg[nQQUin] = lpBuddyInfoDlg;
				lpBuddyInfoDlg->m_lpQQClient = &m_QQClient;
				lpBuddyInfoDlg->m_hMainDlg = m_hWnd;
				lpBuddyInfoDlg->m_nQQUin = nQQUin;
				lpBuddyInfoDlg->m_bIsGMember = FALSE;
				lpBuddyInfoDlg->m_nGroupCode = 0;
				lpBuddyInfoDlg->Create(NULL);
				lpBuddyInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyInfoDlg->m_hWnd);
			}
		}
	}
	else
	{
		std::map<UINT, CBuddyInfoDlg *>::iterator iter;
		iter = m_mapBuddyInfoDlg.find(nQQUin);
		if (iter != m_mapBuddyInfoDlg.end())
		{
			CBuddyInfoDlg * lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->DestroyWindow();
				delete lpBuddyInfoDlg;
			}
			m_mapBuddyInfoDlg.erase(iter);
		}
	}
}

void CMainDlg::ShowGMemberInfoDlg(UINT nGroupCode, UINT nQQUin, BOOL bShow)
{
	if (0 == nGroupCode || 0 == nQQUin)
		return;

	if (bShow)
	{
		CGMemberInfoMapKey key;
		key.m_nGroupCode = nGroupCode;
		key.m_nQQUin = nQQUin;
		std::map<CGMemberInfoMapKey, CBuddyInfoDlg *>::iterator iter;
		iter = m_mapGMemberInfoDlg.find(key);
		if (iter != m_mapGMemberInfoDlg.end())
		{
			CBuddyInfoDlg * lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (!lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->Create(NULL);
				lpBuddyInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyInfoDlg->m_hWnd);
			}
		}
		else
		{
			CBuddyInfoDlg * lpBuddyInfoDlg = new CBuddyInfoDlg;
			if (lpBuddyInfoDlg != NULL)
			{
				m_mapGMemberInfoDlg[key] = lpBuddyInfoDlg;
				lpBuddyInfoDlg->m_lpQQClient = &m_QQClient;
				lpBuddyInfoDlg->m_hMainDlg = m_hWnd;
				lpBuddyInfoDlg->m_nQQUin = nQQUin;
				lpBuddyInfoDlg->m_bIsGMember = TRUE;
				lpBuddyInfoDlg->m_nGroupCode = nGroupCode;
				lpBuddyInfoDlg->Create(NULL);
				lpBuddyInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyInfoDlg->m_hWnd);
			}
		}
	}
	else
	{
		CGMemberInfoMapKey key;
		key.m_nGroupCode = nGroupCode;
		key.m_nQQUin = nQQUin;
		std::map<CGMemberInfoMapKey, CBuddyInfoDlg *>::iterator iter;
		iter = m_mapGMemberInfoDlg.find(key);
		if (iter != m_mapGMemberInfoDlg.end())
		{
			CBuddyInfoDlg * lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->DestroyWindow();
				delete lpBuddyInfoDlg;
			}
			m_mapGMemberInfoDlg.erase(iter);
		}
	}
}

void CMainDlg::ShowGroupInfoDlg(UINT nGroupCode, BOOL bShow)
{
	if (0 == nGroupCode)
		return;

	if (bShow)
	{
		std::map<UINT, CGroupInfoDlg *>::iterator iter;
		iter = m_mapGroupInfoDlg.find(nGroupCode);
		if (iter != m_mapGroupInfoDlg.end())
		{
			CGroupInfoDlg * lpGroupInfoDlg = iter->second;
			if (lpGroupInfoDlg != NULL)
			{
				if (!lpGroupInfoDlg->IsWindow())
					lpGroupInfoDlg->Create(NULL);
				lpGroupInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpGroupInfoDlg->m_hWnd);
			}
		}
		else
		{
			CGroupInfoDlg * lpGroupInfoDlg = new CGroupInfoDlg;
			if (lpGroupInfoDlg != NULL)
			{
				m_mapGroupInfoDlg[nGroupCode] = lpGroupInfoDlg;
				lpGroupInfoDlg->m_lpQQClient = &m_QQClient;
				lpGroupInfoDlg->m_hMainDlg = m_hWnd;
				lpGroupInfoDlg->m_nGroupCode = nGroupCode;
				lpGroupInfoDlg->Create(NULL);
				lpGroupInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpGroupInfoDlg->m_hWnd);
			}
		}
	}
	else
	{
		std::map<UINT, CGroupInfoDlg *>::iterator iter;
		iter = m_mapGroupInfoDlg.find(nGroupCode);
		if (iter != m_mapGroupInfoDlg.end())
		{
			CGroupInfoDlg * lpGroupInfoDlg = iter->second;
			if (lpGroupInfoDlg != NULL)
			{
				if (lpGroupInfoDlg->IsWindow())
					lpGroupInfoDlg->DestroyWindow();
				delete lpGroupInfoDlg;
			}
			m_mapGroupInfoDlg.erase(iter);
		}
	}
}

// 通知好友聊天窗口更新
void CMainDlg::NotifyBuddyChatDlg(UINT nQQUin, UINT uMsg)
{
	std::map<UINT, CBuddyChatDlg *>::iterator iter;
	iter = m_mapBuddyChatDlg.find(nQQUin);
	if (iter != m_mapBuddyChatDlg.end())
	{
		CBuddyChatDlg * lpBuddyChatDlg = iter->second;
		if (lpBuddyChatDlg != NULL && lpBuddyChatDlg->IsWindow())
		{
			switch (uMsg)
			{
			case QQ_MSG_UPDATE_BUDDY_NUMBER:
				lpBuddyChatDlg->OnUpdateBuddyNumber();
				break;

			case QQ_MSG_UPDATE_BUDDY_SIGN:
				lpBuddyChatDlg->OnUpdateBuddySign();
				break;
			}
		}
	}
}

// 通知群聊天窗口更新
void CMainDlg::NotifyGroupChatDlg(UINT nGroupCode, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	std::map<UINT, CGroupChatDlg *>::iterator iter;
	iter = m_mapGroupChatDlg.find(nGroupCode);
	if (iter != m_mapGroupChatDlg.end())
	{
		CGroupChatDlg * lpGroupChatDlg = iter->second;
		if (lpGroupChatDlg != NULL && lpGroupChatDlg->IsWindow())
		{
			switch (uMsg)
			{
			case QQ_MSG_UPDATE_GROUP_INFO:
				lpGroupChatDlg->OnUpdateGroupInfo();
				break;

			case QQ_MSG_UPDATE_GROUP_NUMBER:
				lpGroupChatDlg->OnUpdateGroupNumber();
				break;

			case QQ_MSG_UPDATE_GMEMBER_NUMBER:
				lpGroupChatDlg->OnUpdateGMemberNumber(wParam, lParam);
				break;
			}
		}
	}
}

// 通知临时会话聊天窗口更新
void CMainDlg::NotifySessChatDlg(UINT nQQUin, UINT uMsg)
{
	std::map<UINT, CSessChatDlg *>::iterator iter;
	iter = m_mapSessChatDlg.find(nQQUin);
	if (iter != m_mapSessChatDlg.end())
	{
		CSessChatDlg * lpSessChatDlg = iter->second;
		if (lpSessChatDlg != NULL && lpSessChatDlg->IsWindow())
		{
			switch (uMsg)
			{
			case QQ_MSG_UPDATE_C2CMSGSIG:
				lpSessChatDlg->OnUpdateC2CMsgSig();
				break;
			}
		}
	}
}

// 通知好友信息窗口更新
void CMainDlg::NotifyBuddyInfoDlg(UINT nQQUin, UINT uMsg)
{
	std::map<UINT, CBuddyInfoDlg *>::iterator iter;
	iter = m_mapBuddyInfoDlg.find(nQQUin);
	if (iter != m_mapBuddyInfoDlg.end())
	{
		CBuddyInfoDlg * lpBuddyInfoDlg = iter->second;
		if (lpBuddyInfoDlg != NULL && lpBuddyInfoDlg->IsWindow())
		{
			switch (uMsg)
			{
			case QQ_MSG_UPDATE_BUDDY_INFO:
				lpBuddyInfoDlg->OnUpdateBuddyInfo();
				break;

			case QQ_MSG_UPDATE_BUDDY_NUMBER:
				lpBuddyInfoDlg->OnUpdateBuddyNumber();
				break;

			case QQ_MSG_UPDATE_BUDDY_SIGN:
				lpBuddyInfoDlg->OnUpdateBuddySign();
				break;
			}
		}
	}
}

// 通知群成员信息窗口更新
void CMainDlg::NotifyGMemberInfoDlg(UINT nGroupCode, UINT nQQUin, UINT uMsg)
{
	CGMemberInfoMapKey key;
	key.m_nGroupCode = nGroupCode;
	key.m_nQQUin = nQQUin;
	std::map<CGMemberInfoMapKey, CBuddyInfoDlg *>::iterator iter;
	iter = m_mapGMemberInfoDlg.find(key);
	if (iter != m_mapGMemberInfoDlg.end())
	{
		CBuddyInfoDlg * lpBuddyInfoDlg = iter->second;
		if (lpBuddyInfoDlg != NULL && lpBuddyInfoDlg->IsWindow())
		{
			switch (uMsg)
			{
			case QQ_MSG_UPDATE_GMEMBER_INFO:
				lpBuddyInfoDlg->OnUpdateBuddyInfo();
				break;

			case QQ_MSG_UPDATE_GMEMBER_NUMBER:
				lpBuddyInfoDlg->OnUpdateBuddyNumber();
				break;

			case QQ_MSG_UPDATE_GMEMBER_SIGN:
				lpBuddyInfoDlg->OnUpdateBuddySign();
				break;
			}
		}
	}

}

// 通知群信息窗口更新
void CMainDlg::NotifyGroupInfoDlg(UINT nGroupCode, UINT uMsg)
{
	std::map<UINT, CGroupInfoDlg *>::iterator iter;
	iter = m_mapGroupInfoDlg.find(nGroupCode);
	if (iter != m_mapGroupInfoDlg.end())
	{
		CGroupInfoDlg * lpGroupInfoDlg = iter->second;
		if (lpGroupInfoDlg != NULL && lpGroupInfoDlg->IsWindow())
		{
			switch (uMsg)
			{
			case QQ_MSG_UPDATE_GROUP_INFO:
				lpGroupInfoDlg->OnUpdateGroupInfo();
				break;

			case QQ_MSG_UPDATE_GROUP_NUMBER:
				lpGroupInfoDlg->OnUpdateGroupNumber();
				break;
			}
		}
	}
}

void CMainDlg::UpdateBuddyTreeCtrl()
{
	CBuddyList * lpBuddyList = m_QQClient.GetBuddyList();
	if (NULL == lpBuddyList)
		return;

	CString strText;
	HTREEITEM hTreeItem;
	CTreeViewCtrl& treeBuddy = m_MainTabCtrl.m_treeBuddy;

	//treeBuddy.SetRedraw(FALSE);

	treeBuddy.DeleteAllItems();

	int nBuddyTeamCount = lpBuddyList->GetBuddyTeamCount();
	for (int i = 0; i < nBuddyTeamCount; i++)
	{
		CBuddyTeamInfo * lpBuddyTeamInfo = lpBuddyList->GetBuddyTeam(i);
		int nBuddyCount = lpBuddyList->GetBuddyCount(i);
		int nOnlineBuddyCount = lpBuddyList->GetOnlineBuddyCount(i);
		if (lpBuddyTeamInfo != NULL)
		{
			strText.Format(_T("%s [%d/%d]"), lpBuddyTeamInfo->m_strName.c_str(), nOnlineBuddyCount, nBuddyCount);
			hTreeItem = treeBuddy.InsertItem(strText, 0, 0, TVI_ROOT,TVI_LAST);
			treeBuddy.SetItemData(hTreeItem, 0);
		}

		for (int j = 0; j < nBuddyCount; j++)
		{
			CBuddyInfo * lpBuddyInfo = lpBuddyList->GetBuddy(i, j);
			if (lpBuddyInfo != NULL)
			{
				if (lpBuddyInfo->m_strMarkName != _T(""))
					strText.Format(_T("%s(%s)"), lpBuddyInfo->m_strMarkName.c_str(), lpBuddyInfo->m_strNickName.c_str());
				else
					strText = lpBuddyInfo->m_strNickName.c_str();

				BOOL bGray = lpBuddyInfo->m_nStatus != QQ_STATUS_OFFLINE ? FALSE : TRUE;
				int nIndex = m_headImageList.GetHeadPicIndex(QQ_MSG_TYPE_BUDDY, 0, lpBuddyInfo->m_nQQUin, bGray, TRUE);

				HTREEITEM hTreeItem2 = treeBuddy.InsertItem(strText, nIndex, nIndex, hTreeItem, TVI_LAST);
				treeBuddy.SetItemData(hTreeItem2, lpBuddyInfo->m_nQQUin);
			}
		}
	}

	//treeBuddy.SetRedraw(TRUE);
}

void CMainDlg::UpdateGroupTreeCtrl()
{
	CGroupList * lpGroupList = m_QQClient.GetGroupList();
	if (NULL == lpGroupList)
		return;

	CTreeViewCtrl& treeGroup = m_MainTabCtrl.m_treeGroup;

	treeGroup.DeleteAllItems();

	int nGroupCount = lpGroupList->GetGroupCount();

	CString strText;
	strText.Format(_T("%s [%d]"), _T("我的QQ群"), nGroupCount);
	HTREEITEM hTreeItem = treeGroup.InsertItem(strText, 0, 0, TVI_ROOT,TVI_LAST);
	treeGroup.SetItemData(hTreeItem, 0);

	for (int i = 0; i < nGroupCount; i++)
	{
		CGroupInfo * lpGroupInfo = lpGroupList->GetGroup(i);
		if (lpGroupInfo != NULL)
		{
			strText = lpGroupInfo->m_strName.c_str();

			int nIndex = m_headImageList.GetHeadPicIndex(QQ_MSG_TYPE_GROUP, 
				lpGroupInfo->m_nGroupCode, 0, FALSE, TRUE);

			HTREEITEM hTreeItem2 = treeGroup.InsertItem(strText, nIndex, nIndex, hTreeItem, TVI_LAST);
			treeGroup.SetItemData(hTreeItem2, lpGroupInfo->m_nGroupCode);
		}
	}
}

void CMainDlg::UpdateRecentTreeCtrl()
{
	CRecentList * lpRecentList = m_QQClient.GetRecentList();
	if (NULL == lpRecentList)
		return;

	CTreeViewCtrl& treeRecent = m_MainTabCtrl.m_treeRecent;

	treeRecent.DeleteAllItems();

	int nRecentCount = lpRecentList->GetRecentCount();

	CString strText;
	strText.Format(_T("%s [%d]"), _T("最近联系人"), nRecentCount);
	HTREEITEM hTreeItem = treeRecent.InsertItem(strText, 0, 0, TVI_ROOT,TVI_LAST);
	treeRecent.SetItemData(hTreeItem, -1);

	int nCount = 0;
	for (int i = 0; i < nRecentCount; i++)
	{
		CRecentInfo * lpRecentInfo = lpRecentList->GetRecent(i);
		if (lpRecentInfo != NULL)
		{
			if (0 == lpRecentInfo->m_nType)			// 好友
			{
				CBuddyList * lpBuddyList = m_QQClient.GetBuddyList();
				if (lpBuddyList != NULL)
				{
					CBuddyInfo * lpBuddyInfo = lpBuddyList->GetBuddy(lpRecentInfo->m_nQQUin);
					if (lpBuddyInfo != NULL)
					{
						if (lpBuddyInfo->m_strMarkName != _T(""))
							strText.Format(_T("%s(%s)"), lpBuddyInfo->m_strMarkName.c_str(), lpBuddyInfo->m_strNickName.c_str());
						else
							strText = lpBuddyInfo->m_strNickName.c_str();

						BOOL bGray = lpBuddyInfo->m_nStatus != QQ_STATUS_OFFLINE ? FALSE : TRUE;
						int nIndex = m_headImageList.GetHeadPicIndex(QQ_MSG_TYPE_BUDDY,
							0, lpBuddyInfo->m_nQQUin, bGray, FALSE);

						HTREEITEM hTreeItem2 = treeRecent.InsertItem(strText, nIndex, nIndex, hTreeItem, TVI_LAST);
						treeRecent.SetItemData(hTreeItem2, i);
						nCount++;
					}
				}
			}
			else if (1 == lpRecentInfo->m_nType)	// 群
			{
				CGroupList * lpGroupList = m_QQClient.GetGroupList();
				if (lpGroupList != NULL)
				{
					CGroupInfo * lpGroupInfo = lpGroupList->GetGroupById(lpRecentInfo->m_nQQUin);
					if (lpGroupInfo != NULL)
					{
						strText = lpGroupInfo->m_strName.c_str();

						int nIndex = m_headImageList.GetHeadPicIndex(QQ_MSG_TYPE_GROUP,
							lpGroupInfo->m_nGroupCode, 0, FALSE, FALSE);

						HTREEITEM hTreeItem2 = treeRecent.InsertItem(strText, nIndex, nIndex, hTreeItem, TVI_LAST);
						treeRecent.SetItemData(hTreeItem2, i);
						nCount++;
					}
				}
			}
			else if (2 == lpRecentInfo->m_nType)	// 好像是讨论组
			{
			}
		}
	}

	strText.Format(_T("%s [%d]"), _T("最近联系人"), nCount);
	hTreeItem = treeRecent.GetRootItem();
	treeRecent.SetItemText(hTreeItem, strText);
}

void CMainDlg::OnTrayIcon_LButtunUp()
{
	if (m_dwMsgTimerId != NULL)
	{
		CMessageList * lpMsgList = m_QQClient.GetMessageList();
		if (lpMsgList != NULL && lpMsgList->GetMsgSenderCount() > 0)
		{
			CMessageSender * lpMsgSender = lpMsgList->GetLastMsgSender();
			if (lpMsgSender != NULL)
			{
				QQ_MSG_TYPE nType = lpMsgSender->GetMsgType();
				UINT nSenderId = lpMsgSender->GetSenderId();
				UINT nGroupCode = lpMsgSender->GetGroupCode();

				switch (nType)
				{
				case QQ_MSG_TYPE_BUDDY:
					ShowBuddyChatDlg(nSenderId, TRUE);
					break;
				case QQ_MSG_TYPE_GROUP:
					ShowGroupChatDlg(nSenderId, TRUE);
					break;
				case QQ_MSG_TYPE_SESS:
					ShowSessChatDlg(nGroupCode, nSenderId, TRUE);
					break;
				}
			}
		}
	}
	else
	{
		if (m_LoginDlg.IsWindow())
		{
			m_LoginDlg.ShowWindow(SW_SHOW);
			::SetForegroundWindow(m_LoginDlg.m_hWnd);
		}
		else if (IsWindow())
		{
			ShowWindow(SW_SHOW);
			::SetForegroundWindow(m_hWnd);
		}
	}
}

void CMainDlg::OnTrayIcon_RButtunUp()
{
	if (m_MsgTipDlg.IsWindow())
		m_MsgTipDlg.DestroyWindow();

	CPoint pt;
	GetCursorPos(&pt);

	::SetForegroundWindow(m_hWnd);

	CMenu menu;
	menu.LoadMenu(IDR_MENU3);

	CMenuHandle PopupMenu = menu.GetSubMenu(0);
	PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);

	menu.DestroyMenu();

	// BUGFIX: See "PRB: Menus for Notification Icons Don't Work Correctly"
	::PostMessage(m_hWnd, WM_NULL, 0, 0);
}

void CMainDlg::OnTrayIcon_MouseHover()
{
	if (m_dwMsgTimerId != NULL)
	{
		CMessageList * lpMsgList = m_QQClient.GetMessageList();
		if (lpMsgList != NULL && lpMsgList->GetMsgSenderCount() > 0)
		{
			CRect rcTrayIcon;
			m_TrayIcon.GetTrayIconRect(&rcTrayIcon);

			m_MsgTipDlg.m_lpQQClient = &m_QQClient;
			m_MsgTipDlg.m_hMainDlg = m_hWnd;
			m_MsgTipDlg.m_rcTrayIcon = rcTrayIcon;
			if (!m_MsgTipDlg.IsWindow())
				m_MsgTipDlg.Create(m_hWnd);
			m_MsgTipDlg.ShowWindow(SW_SHOWNOACTIVATE);
		}
	}
	//::OutputDebugString(_T("WM_MOUSEHOVER\n"));
}

void CMainDlg::OnTrayIcon_MouseLeave()
{
	if (m_MsgTipDlg.IsWindow())
	{
		CRect rcWindow;
		m_MsgTipDlg.GetWindowRect(&rcWindow);

		POINT pt = {0};
		::GetCursorPos(&pt);

		if (!::PtInRect(&rcWindow, pt))
			m_MsgTipDlg.DestroyWindow();
		else
			m_MsgTipDlg.StartTrackMouseLeave();
	}
	//::OutputDebugString(_T("WM_MOUSELEAVE\n"));
}

BOOL CMainDlg::LoadAppIcon(QQ_STATUS nStatus)
{
	DestroyAppIcon();

	CString strFileName;
	switch (nStatus)
	{
	case QQ_STATUS_ONLINE:
		strFileName = ZYM::CPath::GetCurDir() + _T("Image\\imonline.ico");
		break;
	case QQ_STATUS_CALLME:
		strFileName = ZYM::CPath::GetCurDir() + _T("Image\\Qme.ico");
		break;
	case QQ_STATUS_AWAY:
		strFileName = ZYM::CPath::GetCurDir() + _T("Image\\away.ico");
		break;
	case QQ_STATUS_BUSY:
		strFileName = ZYM::CPath::GetCurDir() + _T("Image\\busy.ico");
		break;
	case QQ_STATUS_SILENT:
		strFileName = ZYM::CPath::GetCurDir() + _T("Image\\mute.ico");
		break;
	case QQ_STATUS_HIDDEN:
		strFileName = ZYM::CPath::GetCurDir() + _T("Image\\invisible.ico");
		break;
	case QQ_STATUS_OFFLINE:
		strFileName = ZYM::CPath::GetCurDir() + _T("Image\\offline.ico");
		break;
	}
	m_hAppIcon = AtlLoadIconImage((LPCTSTR)strFileName, LR_DEFAULTCOLOR|LR_LOADFROMFILE, 
		::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	return m_hAppIcon != NULL ? TRUE : FALSE;
}

void CMainDlg::DestroyAppIcon()
{
	if (m_hAppIcon != NULL)
	{
		::DestroyIcon(m_hAppIcon);
		m_hAppIcon = NULL;
	}
}

BOOL CMainDlg::LoadLoginIcon()
{
	DestroyLoginIcon();

	CString strFileName;
	for (int i = 0; i < 6; i++)
	{
		strFileName.Format(_T("%sImage\\Loading_%d.ico"), ZYM::CPath::GetCurDir(), i+1);
		m_hLoginIcon[i] = AtlLoadIconImage((LPCTSTR)strFileName, LR_DEFAULTCOLOR|LR_LOADFROMFILE, 
			::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	}
	
	return TRUE;
}

void CMainDlg::DestroyLoginIcon()
{
	for (int i = 0; i < 6; i++)
	{
		if (m_hLoginIcon[i] != NULL)
		{
			::DestroyIcon(m_hLoginIcon[i]);
			m_hLoginIcon[i] = NULL;
		}
	}
}

void CMainDlg::UpdateMsgIcon()
{
	CMessageList * lpMsgList = m_QQClient.GetMessageList();
	if (NULL == lpMsgList)
		return;

	CMessageSender * lpMsgSender = lpMsgList->GetLastMsgSender();
	if (NULL == lpMsgSender)
		return;

	QQ_MSG_TYPE nMsgType = lpMsgSender->GetMsgType();
	UINT nSenderId = lpMsgSender->GetSenderId();
	UINT nGroupCode = lpMsgSender->GetGroupCode();

	if (m_nLastMsgType != nMsgType || m_nLastSenderId != nSenderId)
	{
		m_nLastMsgType = nMsgType;
		m_nLastSenderId = nSenderId;

		if (m_hMsgIcon != NULL)
		{
			::DestroyIcon(m_hMsgIcon);
			m_hMsgIcon = NULL;
		}

		int nIndex = 0;
		if (QQ_MSG_TYPE_BUDDY == nMsgType)
			nIndex = m_headImageList.GetHeadPicIndex(nMsgType, 0, nSenderId, FALSE, FALSE);
		else if (QQ_MSG_TYPE_GROUP == nMsgType)
			nIndex = m_headImageList.GetHeadPicIndex(nMsgType, nSenderId, 0, FALSE, FALSE);
		else if (QQ_MSG_TYPE_SESS == nMsgType)
			nIndex = m_headImageList.GetHeadPicIndex(nMsgType, nGroupCode, nSenderId, FALSE, FALSE);
		else if (QQ_MSG_TYPE_SYSGROUP == nMsgType)
			nIndex = m_headImageList.GetHeadPicIndex(nMsgType, 0, 0, FALSE, FALSE);

		if (0 == nIndex)
			m_hMsgIcon = AtlLoadIconImage(IDI_BUDDY, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		else if (2 == nIndex)
			m_hMsgIcon = AtlLoadIconImage(IDI_GROUPCHATDLG_16, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		else if (3 == nIndex)
			m_hMsgIcon = AtlLoadIconImage(IDI_SYS, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		else
			m_hMsgIcon = m_headImageList.ExtractIcon(nIndex);
	}
}

void CMainDlg::StartLogin()
{
	LoadAppIcon(m_QQClient.GetStatus());
	m_TrayIcon.AddIcon(m_hWnd, WM_TRAYICON_NOTIFY, 1, m_hAppIcon, _T("QQ2011"));	

	ShowWindow(SW_HIDE);
	if (m_LoginDlg.DoModal(m_hWnd) != IDOK)	// 显示登录对话框
	{
		CloseDialog(IDOK);
		return;
	}
	ShowWindow(SW_SHOW);

	m_picHead.ShowWindow(SW_HIDE);
	m_staNickName.ShowWindow(SW_HIDE);
	m_staSign.ShowWindow(SW_HIDE);
	m_edtSign.ShowWindow(SW_HIDE);
	m_cboStatus.ShowWindow(SW_HIDE);
	m_MainTabCtrl.ShowWindow(SW_HIDE);

	m_staQQNum.SetWindowText(m_LoginDlg.m_strUser);
	m_staLogining.SetWindowText(_T("正在登录"));

	CRect rcClient;
	GetClientRect(&rcClient);

	m_staQQNum.MoveWindow(rcClient.left, 180, rcClient.Width(), 16, FALSE);
	m_staLogining.MoveWindow(rcClient.left, 196, rcClient.Width(), 16, FALSE);
	m_btnCancel.MoveWindow(87, 260, 86, 30, FALSE);

	m_staQQNum.ShowWindow(SW_SHOW);
	m_staLogining.ShowWindow(SW_SHOW);
	m_btnCancel.ShowWindow(SW_SHOW);

	LoadLoginIcon();
	m_dwLoginTimerId = SetTimer(1, 200, NULL);

	TCHAR cPwdHash[1024] = {0};

	//m_QQClient.CalcPwdHash(_T("testtest"), _T("!D9Y"), cPwdHash, 1024);

	m_QQClient.SetUser(m_LoginDlg.m_strUser, m_LoginDlg.m_strPwd);
	m_QQClient.SetLoginStatus(m_LoginDlg.m_nStatus);
	m_QQClient.SetCallBackWnd(m_hWnd);
	m_QQClient.Login();
}

void CMainDlg::CloseAllDlg()
{
	{
		std::map<UINT, CBuddyChatDlg *>::iterator iter;
		for (iter = m_mapBuddyChatDlg.begin(); iter != m_mapBuddyChatDlg.end(); iter++)
		{
			CBuddyChatDlg * lpBuddyChatDlg = iter->second;
			if (lpBuddyChatDlg != NULL)
			{
				if (lpBuddyChatDlg->IsWindow())
					lpBuddyChatDlg->DestroyWindow();
				delete lpBuddyChatDlg;
			}
		}
		m_mapBuddyChatDlg.clear();
	}

	{
		std::map<UINT, CGroupChatDlg *>::iterator iter;
		for (iter = m_mapGroupChatDlg.begin(); iter != m_mapGroupChatDlg.end(); iter++)
		{
			CGroupChatDlg * lpGroupChatDlg = iter->second;
			if (lpGroupChatDlg != NULL)
			{
				if (lpGroupChatDlg->IsWindow())
					lpGroupChatDlg->DestroyWindow();
				delete lpGroupChatDlg;
			}
		}
		m_mapGroupChatDlg.clear();
	}

	{
		std::map<UINT, CSessChatDlg *>::iterator iter;
		for (iter = m_mapSessChatDlg.begin(); iter != m_mapSessChatDlg.end(); iter++)
		{
			CSessChatDlg * lpSessChatDlg = iter->second;
			if (lpSessChatDlg != NULL)
			{
				if (lpSessChatDlg->IsWindow())
					lpSessChatDlg->DestroyWindow();
				delete lpSessChatDlg;
			}
		}
		m_mapSessChatDlg.clear();
	}

	{
		std::map<UINT, CBuddyInfoDlg *>::iterator iter;
		for (iter = m_mapBuddyInfoDlg.begin(); iter != m_mapBuddyInfoDlg.end(); iter++)
		{
			CBuddyInfoDlg * lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->DestroyWindow();
				delete lpBuddyInfoDlg;
			}
		}
		m_mapBuddyInfoDlg.clear();
	}

	{
		std::map<CGMemberInfoMapKey, CBuddyInfoDlg *>::iterator iter;
		for (iter = m_mapGMemberInfoDlg.begin(); iter != m_mapGMemberInfoDlg.end(); iter++)
		{
			CBuddyInfoDlg * lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->DestroyWindow();
				delete lpBuddyInfoDlg;
			}
		}
		m_mapGMemberInfoDlg.clear();
	}

	{
		std::map<UINT, CGroupInfoDlg *>::iterator iter;
		for (iter = m_mapGroupInfoDlg.begin(); iter != m_mapGroupInfoDlg.end(); iter++)
		{
			CGroupInfoDlg * lpGroupInfoDlg = iter->second;
			if (lpGroupInfoDlg != NULL)
			{
				if (lpGroupInfoDlg->IsWindow())
					lpGroupInfoDlg->DestroyWindow();
				delete lpGroupInfoDlg;
			}
		}
		m_mapGroupInfoDlg.clear();
	}
}