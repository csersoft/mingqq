#include "StdAfx.h"
#include "BuddyChatDlg.h"

CFontInfo g_buddyFontInfo;

CBuddyChatDlg::CBuddyChatDlg(void)
{
	m_lpQQClient = NULL;
	m_lpFaceList = NULL;
	m_hMainDlg = NULL;
	m_nQQUin = 0;
	m_nQQNumber = 0;
	m_strBuddyName = _T("");
	m_strBuddySign = _T("");
	m_strUserName = _T("");
	m_hDlgIcon = m_hDlgSmallIcon = NULL;
}

CBuddyChatDlg::~CBuddyChatDlg(void)
{
}

BOOL CBuddyChatDlg::PreTranslateMessage(MSG* pMsg)
{
	if (::GetForegroundWindow() == m_hWnd && !m_Accelerator.IsNull() && 
		m_Accelerator.TranslateAccelerator(m_hWnd, pMsg))
		return TRUE;

	if (pMsg->message == WM_RBUTTONDOWN)
	{
		if (pMsg->hwnd == m_richSend.m_hWnd)
		{
			CMenuHandle menuPopup = m_menuRichEdit.GetSubMenu(0);

			UINT nSel = ((m_richSend.GetSelectionType() != SEL_EMPTY) ? 0 : MF_GRAYED);
			menuPopup.EnableMenuItem(ID_MENU_CUT, MF_BYCOMMAND|nSel);
			menuPopup.EnableMenuItem(ID_MENU_COPY, MF_BYCOMMAND|nSel);

			UINT nPaste = (m_richSend.CanPaste() ? 0 : MF_GRAYED) ;
			menuPopup.EnableMenuItem(ID_MENU_PASTE, MF_BYCOMMAND|nPaste);
			
			menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				pMsg->pt.x, pMsg->pt.y, m_hWnd, NULL);
		}
		else if (pMsg->hwnd == m_richRecv.m_hWnd)
		{
			CMenuHandle menuPopup = m_menuRichEdit.GetSubMenu(1);

			UINT nSel = ((m_richRecv.GetSelectionType() != SEL_EMPTY) ? 0 : MF_GRAYED);
			menuPopup.EnableMenuItem(ID_MENU_COPY, MF_BYCOMMAND|nSel);
			//menuPopup.EnableMenuItem(ID_MENU_CLEAR, MF_BYCOMMAND|nSel);

			menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				pMsg->pt.x, pMsg->pt.y, m_hWnd, NULL);
		}
	}

	return CWindow::IsDialogMessage(pMsg);
}

BOOL CBuddyChatDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	CenterWindow();

	// set icons
	m_hDlgIcon = AtlLoadIconImage(IDI_BUDDYCHATDLG_32, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(m_hDlgIcon, TRUE);
	m_hDlgSmallIcon = AtlLoadIconImage(IDI_BUDDYCHATDLG_16, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(m_hDlgSmallIcon, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);

	CBuddyInfo * lpBuddyInfo = GetBuddyInfoPtr();
	if (lpBuddyInfo != NULL)
	{
		if (!lpBuddyInfo->IsHasQQNum())
			m_lpQQClient->UpdateBuddyNum(m_nQQUin);
		else

		if (!lpBuddyInfo->IsHasQQSign())
			m_lpQQClient->UpdateBuddySign(m_nQQUin);
	}

	InitCtrls();		// 初始化控件

	return TRUE;
}

void CBuddyChatDlg::OnClose()
{
	::PostMessage(m_hMainDlg, WM_CLOSE_BUDDYCHATDLG, NULL, m_nQQUin);
}

void CBuddyChatDlg::OnDestroy()
{
	SetMsgHandled(FALSE);

	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);

	UnInitCtrls();		// 反初始化控件

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

// “好友名称”超链接控件
void CBuddyChatDlg::OnLnk_BuddyName(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	::PostMessage(m_hMainDlg, WM_SHOW_BUDDYINFODLG, NULL, m_nQQUin);
}

// “表情”按钮
void CBuddyChatDlg::OnBtn_Face(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_FaceSelDlg.SetFaceList(m_lpFaceList);
	if (!m_FaceSelDlg.IsWindow())
	{
		m_FaceSelDlg.Create(m_hWnd);

		HWND hWnd = GetDlgItem(ID_BTN_FACE);

		CRect rcBtn;
		::GetWindowRect(hWnd, &rcBtn);
		
		int cx = 432;
		int cy = 236;
		int x = rcBtn.left - cx / 2;
		int y = rcBtn.top - cy;

		m_FaceSelDlg.SetWindowPos(NULL, x, y, cx, cy, NULL);
		m_FaceSelDlg.ShowWindow(SW_SHOW);
	}
}

// “字体名称”组合框
void CBuddyChatDlg::OnCbo_SelChange_FontName(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strText;
	m_cboFontName.GetWindowText(strText);

	if (m_fontInfo.m_strName != strText.GetBuffer())
	{
		m_fontInfo.m_strName = strText.GetBuffer();
		RichEdit_SetDefFont(m_richSend, m_fontInfo.m_strName.c_str(),
			m_fontInfo.m_nSize, m_fontInfo.m_clrText, m_fontInfo.m_bBold,
			m_fontInfo.m_bItalic, m_fontInfo.m_bUnderLine);
		g_buddyFontInfo.m_strName = m_fontInfo.m_strName;
	}
}

// “字体大小”组合框
void CBuddyChatDlg::OnCbo_SelChange_FontSize(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strText;
	m_cboFontSize.GetWindowText(strText);

	int nSize = _tcstol(strText.GetBuffer(), NULL, 10);
	if (m_fontInfo.m_nSize != nSize)
	{
		m_fontInfo.m_nSize = nSize;
		RichEdit_SetDefFont(m_richSend, m_fontInfo.m_strName.c_str(),
			m_fontInfo.m_nSize, m_fontInfo.m_clrText, m_fontInfo.m_bBold,
			m_fontInfo.m_bItalic, m_fontInfo.m_bUnderLine);
		g_buddyFontInfo.m_nSize = m_fontInfo.m_nSize;
	}
}

// “加粗”复选框
void CBuddyChatDlg::OnChk_Bold(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCheck = m_btnBold.GetCheck();
	BOOL bBold = ((nCheck == BST_CHECKED) ? TRUE : FALSE);
	if (m_fontInfo.m_bBold != bBold)
	{
		m_fontInfo.m_bBold = bBold;
		RichEdit_SetDefFont(m_richSend, m_fontInfo.m_strName.c_str(),
			m_fontInfo.m_nSize, m_fontInfo.m_clrText, m_fontInfo.m_bBold,
			m_fontInfo.m_bItalic, m_fontInfo.m_bUnderLine);
		g_buddyFontInfo.m_bBold = m_fontInfo.m_bBold;
	}
}

// “斜体”复选框
void CBuddyChatDlg::OnChk_Italic(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCheck = m_btnItalic.GetCheck();
	BOOL bItalic = ((nCheck == BST_CHECKED) ? TRUE : FALSE);
	if (m_fontInfo.m_bItalic != bItalic)
	{
		m_fontInfo.m_bItalic = bItalic;
		RichEdit_SetDefFont(m_richSend, m_fontInfo.m_strName.c_str(),
			m_fontInfo.m_nSize, m_fontInfo.m_clrText, m_fontInfo.m_bBold,
			m_fontInfo.m_bItalic, m_fontInfo.m_bUnderLine);
		g_buddyFontInfo.m_bItalic = m_fontInfo.m_bItalic;
	}
}

// “下划线”复选框
void CBuddyChatDlg::OnChk_UnderLine(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCheck = m_btnUnderLine.GetCheck();
	BOOL bUnderLine = ((nCheck == BST_CHECKED) ? TRUE : FALSE);
	if (m_fontInfo.m_bUnderLine != bUnderLine)
	{
		m_fontInfo.m_bUnderLine = bUnderLine;
		RichEdit_SetDefFont(m_richSend, m_fontInfo.m_strName.c_str(),
			m_fontInfo.m_nSize, m_fontInfo.m_clrText, m_fontInfo.m_bBold,
			m_fontInfo.m_bItalic, m_fontInfo.m_bUnderLine);
		g_buddyFontInfo.m_bUnderLine = m_fontInfo.m_bUnderLine;
	}
}

// “颜色”按钮
void CBuddyChatDlg::OnBtn_Color(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CColorDialog colorDlg;
	if (colorDlg.DoModal() == IDOK)
	{
		COLORREF clrText = colorDlg.GetColor();
		if (m_fontInfo.m_clrText != clrText)
		{
			m_fontInfo.m_clrText = colorDlg.GetColor();
			RichEdit_SetDefFont(m_richSend, m_fontInfo.m_strName.c_str(),
				m_fontInfo.m_nSize, m_fontInfo.m_clrText, m_fontInfo.m_bBold,
				m_fontInfo.m_bItalic, m_fontInfo.m_bUnderLine);
			g_buddyFontInfo.m_clrText = m_fontInfo.m_clrText;
		}
	}
}

// “关闭”按钮
void CBuddyChatDlg::OnBtn_Close(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	SendMessage(WM_CLOSE);
}

// “发送”按钮
void CBuddyChatDlg::OnBtn_Send(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (NULL == m_lpQQClient)
		return;

	CString strText;
	RichEdit_GetText(m_richSend, strText);

	m_lpQQClient->SendBuddyMsg(m_nQQUin, 0, strText, m_fontInfo.m_strName.c_str(), 
		m_fontInfo.m_nSize, m_fontInfo.m_clrText, m_fontInfo.m_bBold, 
		m_fontInfo.m_bItalic, m_fontInfo.m_bUnderLine);

	m_richSend.SetWindowText(_T(""));

	AddMsg(strText);
}

//	“接收消息”富文本框链接点击消息
LRESULT CBuddyChatDlg::OnRichEdit_Recv_Link(LPNMHDR pnmh)
{
	CString strUrl;

	if (pnmh->code == EN_LINK)
	{
		ENLINK *pLink = (ENLINK *)pnmh;
		if (pLink->msg == WM_LBUTTONUP)
		{
			m_richRecv.SetSel(pLink->chrg);
			m_richRecv.GetSelText(strUrl);

			if (strUrl.Left(7).MakeLower() == _T("http://"))
			{
				::ShellExecute(NULL, _T("open"), strUrl, NULL, NULL, SW_SHOWNORMAL);
			}
		}
	}
	return 0;
}

// “剪切”菜单
void CBuddyChatDlg::OnMenu_Cut(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_richSend.Cut();
}

// “复制”菜单
void CBuddyChatDlg::OnMenu_Copy(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	HWND hWnd = GetFocus();
	if (hWnd == m_richSend.m_hWnd)
	{
		m_richSend.Copy();
	}
	else if (hWnd == m_richRecv.m_hWnd)
	{
		m_richRecv.Copy();
	}
}

// “粘贴”菜单
void CBuddyChatDlg::OnMenu_Paste(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_richSend.PasteSpecial(CF_TEXT);
}

// “全部选择”菜单
void CBuddyChatDlg::OnMenu_SelAll(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	HWND hWnd = GetFocus();
	if (hWnd == m_richSend.m_hWnd)
	{
		m_richSend.SetSel(0, -1);
	}
	else if (hWnd == m_richRecv.m_hWnd)
	{
		m_richRecv.SetSel(0, -1);
	}
}

// “清屏”菜单
void CBuddyChatDlg::OnMenu_Clear(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_richRecv.SetWindowText(_T(""));
}

// “表情”控件选取消息
LRESULT CBuddyChatDlg::OnFaceCtrlSel(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RichEdit_InsertFace(m_richSend, m_FaceSelDlg.GetSelFaceFileName(), m_FaceSelDlg.GetSelFaceId());
	m_richSend.SetFocus();
	return 0;
}

void CBuddyChatDlg::OnRecvMsg(UINT nQQUin, UINT nMsgId)
{
	if (NULL == m_lpQQClient || m_nQQUin != nQQUin)
		return;

	if (::GetForegroundWindow() != m_hWnd)
		FlashWindowEx(m_hWnd, 3);

	if (nMsgId == 0)
	{
		CMessageList * lpMsgList = m_lpQQClient->GetMessageList();
		if (lpMsgList != NULL)
		{
			CMessageSender * lpMsgSender = lpMsgList->GetMsgSender(QQ_MSG_TYPE_BUDDY, nQQUin);
			if (lpMsgSender != NULL)
			{
				int nMsgCnt = lpMsgSender->GetMsgCount();
				for (int i = 0; i < nMsgCnt; i++)
				{
					CBuddyMessage * lpBuddyMsg = lpMsgSender->GetBuddyMsg(i);
					if (lpBuddyMsg != NULL)
					{
						AddMsg(lpBuddyMsg);
					}
				}
				lpMsgList->DelMsgSender(QQ_MSG_TYPE_BUDDY, nQQUin);
				::PostMessage(m_hMainDlg, WM_DEL_MSG_SENDER, QQ_MSG_TYPE_BUDDY, nQQUin);
			}
		}
	}
	else
	{
		CMessageList * lpMsgList = m_lpQQClient->GetMessageList();
		if (lpMsgList != NULL)
		{
			CMessageSender * lpMsgSender = lpMsgList->GetMsgSender(QQ_MSG_TYPE_BUDDY, nQQUin);
			if (lpMsgSender != NULL)
			{
				CBuddyMessage * lpBuddyMsg = lpMsgSender->GetBuddyMsgById(nMsgId);
				if (lpBuddyMsg != NULL)
				{
					AddMsg(lpBuddyMsg);
					lpMsgSender->DelMsgById(nMsgId);
				}

				if (lpMsgSender->GetMsgCount() <= 0)
					lpMsgList->DelMsgSender(QQ_MSG_TYPE_BUDDY, nQQUin);
			}
		}
	}
}

// 更新好友号码通知
void CBuddyChatDlg::OnUpdateBuddyNumber()
{
	UpdateData();
	UpdateBuddyNameCtrl();
}

// 更新好友签名通知
void CBuddyChatDlg::OnUpdateBuddySign()
{
	UpdateData();
	UpdateBuddySignCtrl();
}

// 获取好友信息指针
CBuddyInfo * CBuddyChatDlg::GetBuddyInfoPtr()
{
	if (m_lpQQClient != NULL)
	{
		CBuddyList * lpBuddyList = m_lpQQClient->GetBuddyList();
		if (lpBuddyList != NULL)
			return lpBuddyList->GetBuddy(m_nQQUin);
	}
	return NULL;
}

// 获取用户信息指针
CBuddyInfo * CBuddyChatDlg::GetUserInfoPtr()
{
	if (m_lpQQClient != NULL)
		return m_lpQQClient->GetUserInfo();
	else
		return NULL;
}

// 更新数据
void CBuddyChatDlg::UpdateData()
{
	CBuddyInfo * lpBuddyInfo = GetBuddyInfoPtr();
	if (lpBuddyInfo != NULL)
	{
		m_nQQNumber = lpBuddyInfo->m_nQQNum;
		if (!lpBuddyInfo->m_strMarkName.empty())
			m_strBuddyName = lpBuddyInfo->m_strMarkName.c_str();
		else
			m_strBuddyName = lpBuddyInfo->m_strNickName.c_str();
		if (!lpBuddyInfo->m_strSign.empty())
			m_strBuddySign = lpBuddyInfo->m_strSign.c_str();
		else
			m_strBuddySign = _T("这家伙很懒,什么都没有留下。");
	}

	CBuddyInfo * lpUserInfo = GetUserInfoPtr();
	if (lpUserInfo != NULL)
	{
		m_strUserName = lpUserInfo->m_strNickName.c_str();
	}
}

// 更新对话框标题栏
void CBuddyChatDlg::UpdateDlgTitle()
{
	SetWindowText(m_strBuddyName);
}

// 更新好友名称控件
void CBuddyChatDlg::UpdateBuddyNameCtrl()
{
	CString strText;
	if (m_nQQNumber != 0)
		strText.Format(_T("%s(%u)"), m_strBuddyName, m_nQQNumber);
	else
		strText.Format(_T("%s"), m_strBuddyName);
	m_lnkBuddyName.SetLabel(strText);
}

// 更新好友签名控件
void CBuddyChatDlg::UpdateBuddySignCtrl()
{
	m_edtBuddySign.SetWindowText(m_strBuddySign);
}

// 初始化控件
BOOL CBuddyChatDlg::InitCtrls()
{
	m_picHead = GetDlgItem(ID_PIC_HEAD);
	m_picAD1 = GetDlgItem(ID_PIC_AD1);
	m_picAD2 = GetDlgItem(ID_PIC_AD2);
	m_picAD3 = GetDlgItem(ID_PIC_AD3);
	m_lnkBuddyName.SubclassWindow(GetDlgItem(ID_LINK_BUDDYNAME));
	m_edtBuddySign = GetDlgItem(ID_EDIT_SIGN);
	m_richRecv = GetDlgItem(ID_RICHEDIT_RECV);
	m_richSend = GetDlgItem(ID_RICHEDIT_SEND);
	m_cboFontName = GetDlgItem(ID_COMBO_FONT_NAME);
	m_cboFontSize = GetDlgItem(ID_COMBO_FONT_SIZE);
	m_btnBold = GetDlgItem(ID_CHECK_BOLD);
	m_btnItalic = GetDlgItem(ID_CHECK_ITALIC);
	m_btnUnderLine = GetDlgItem(ID_CHECK_UNDERLINE);

	// 好友名称超链接控件
	DWORD dwStyle = m_lnkBuddyName.GetHyperLinkExtendedStyle();
	dwStyle = dwStyle | HLINK_COMMANDBUTTON | HLINK_UNDERLINEHOVER;
	m_lnkBuddyName.SetHyperLinkExtendedStyle(dwStyle);
	m_lnkBuddyName.m_clrLink = RGB(0,0,0);

	m_fontInfo = g_buddyFontInfo;

	// 发送消息富文本框控件
	RichEdit_SetDefFont(m_richSend, m_fontInfo.m_strName.c_str(),
		m_fontInfo.m_nSize, m_fontInfo.m_clrText, m_fontInfo.m_bBold,
		m_fontInfo.m_bItalic, m_fontInfo.m_bUnderLine);

	// 接收消息富文本框控件
	DWORD dwMask = m_richRecv.GetEventMask();
	dwMask = dwMask | ENM_LINK  | ENM_MOUSEEVENTS | ENM_SCROLLEVENTS | ENM_KEYEVENTS;
	m_richRecv.SetEventMask(dwMask);
	::SendMessage(m_richRecv.m_hWnd, EM_AUTOURLDETECT, true, 0);

	m_richRecv.SetReadOnly();

	InitMidToolBar();

	UpdateData();
	UpdateDlgTitle();
	UpdateBuddyNameCtrl();
	UpdateBuddySignCtrl();

	HRESULT hr = S_FALSE;
	if (m_lpQQClient != NULL && m_nQQNumber != 0)
	{
		CString strFullName = m_lpQQClient->GetHeadPicFullName(0, m_nQQNumber).c_str();
		hr = m_imgHead.Load(strFullName);
	}
	if (hr != S_OK)
		hr = m_imgHead.Load(ZYM::CPath::GetCurDir() + _T("Image\\DefBuddyHeadPic.png"));
	if (S_OK == hr)
		m_picHead.SetBitmap((HBITMAP)m_imgHead);

	hr = m_imgAD1.Load(ZYM::CPath::GetCurDir() + _T("Image\\AD_1.png"));
	if (S_OK == hr)
		m_picAD1.SetBitmap((HBITMAP)m_imgAD1);

	hr = m_imgAD2.Load(ZYM::CPath::GetCurDir() + _T("Image\\AD_2.png"));
	if (S_OK == hr)
		m_picAD2.SetBitmap((HBITMAP)m_imgAD2);

	hr = m_imgAD3.Load(ZYM::CPath::GetCurDir() + _T("Image\\AD_3.png"));
	if (S_OK == hr)
		m_picAD3.SetBitmap((HBITMAP)m_imgAD3);
	
	m_menuRichEdit.LoadMenu(IDR_MENU1);
	m_Accelerator.LoadAccelerators(ID_ACCE_CHATDLG);

	return TRUE;
}

// 反初始化控件
BOOL CBuddyChatDlg::UnInitCtrls()
{	
	m_Accelerator.DestroyObject();
	m_menuRichEdit.DestroyMenu();

	if (!m_imgHead.IsNull())
		m_imgHead.Destroy();

	if (!m_imgAD1.IsNull())
		m_imgAD1.Destroy();

	if (!m_imgAD2.IsNull())
		m_imgAD2.Destroy();

	if (!m_imgAD3.IsNull())
		m_imgAD3.Destroy();

	if (m_picHead.IsWindow())
		m_picHead.DestroyWindow();

	if (m_picAD1.IsWindow())
		m_picAD1.DestroyWindow();

	if (m_picAD2.IsWindow())
		m_picAD2.DestroyWindow();

	if (m_picAD3.IsWindow())
		m_picAD3.DestroyWindow();

	if (m_lnkBuddyName.IsWindow())
	{
		m_lnkBuddyName.DestroyWindow();
		m_lnkBuddyName.m_tip.m_hWnd = NULL;
		m_lnkBuddyName.m_hFontNormal = NULL;
	}

	if (m_edtBuddySign.IsWindow())
		m_edtBuddySign.DestroyWindow();

	if (m_richRecv.IsWindow())
		m_richRecv.DestroyWindow();

	if (m_richSend.IsWindow())
		m_richSend.DestroyWindow();

	if (m_cboFontName.IsWindow())
		m_cboFontName.DestroyWindow();

	if (m_cboFontSize.IsWindow())
		m_cboFontSize.DestroyWindow();

	if (m_btnBold.IsWindow())
		m_btnBold.DestroyWindow();

	if (m_btnItalic.IsWindow())
		m_btnItalic.DestroyWindow();

	if (m_btnUnderLine.IsWindow())
		m_btnUnderLine.DestroyWindow();

	if (m_FaceSelDlg.IsWindow())
		m_FaceSelDlg.DestroyWindow();

	return TRUE;
}

// 初始化中间工具栏
BOOL CBuddyChatDlg::InitMidToolBar()
{
	std::vector<tstring> arrSysFont;
	EnumSysFont(&arrSysFont);

	for (int i = 0; i < (int)arrSysFont.size(); i++)
	{
		m_cboFontName.AddString(arrSysFont[i].c_str());
	}
	arrSysFont.clear();
	int nIndex = m_cboFontName.FindString(0, m_fontInfo.m_strName.c_str());
	m_cboFontName.SetCurSel(nIndex);

	CString strFontSize;
	for (int i = 8; i <= 22; i++)
	{
		strFontSize.Format(_T("%d"), i);
		m_cboFontSize.AddString(strFontSize);
	}
	TCHAR cFontSize[16] = {0};
	wsprintf(cFontSize, _T("%d"), m_fontInfo.m_nSize);
	nIndex = m_cboFontSize.FindString(0, cFontSize);
	m_cboFontSize.SetCurSel(nIndex);

	if (m_fontInfo.m_bBold)
		m_btnBold.SetCheck(BST_CHECKED);
	else
		m_btnBold.SetCheck(BST_UNCHECKED);

	if (m_fontInfo.m_bItalic)
		m_btnItalic.SetCheck(BST_CHECKED);
	else
		m_btnItalic.SetCheck(BST_UNCHECKED);

	if (m_fontInfo.m_bUnderLine)
		m_btnUnderLine.SetCheck(BST_CHECKED);
	else
		m_btnUnderLine.SetCheck(BST_UNCHECKED);

	return TRUE;
}

void CBuddyChatDlg::AddMsg(CBuddyMessage * lpBuddyMsg)
{
	if (NULL == lpBuddyMsg || NULL == m_lpQQClient)
		return;

	CString strText, strSender, strTime;
	CFontInfo fontInfo;

	if (IsToday(lpBuddyMsg->m_nTime))
		strTime = FormatTime(lpBuddyMsg->m_nTime, _T("%H:%M:%S"));
	else
		strTime = FormatTime(lpBuddyMsg->m_nTime, _T("%Y-%m-%d %H:%M:%S"));

	strText.Format(_T("%s  %s\r\n"), m_strBuddyName, strTime);
	RichEdit_SetFont(m_richRecv, _T("宋体"), 9, RGB(0, 0, 255), FALSE, FALSE, FALSE);
	RichEdit_SetStartIndent(m_richRecv, 0);
	RichEdit_AppendText(m_richRecv, strText);

	RichEdit_SetStartIndent(m_richRecv, 300);
	for (int i = 0; i < (int)lpBuddyMsg->m_arrContent.size(); i++)
	{
		CContent * lpContent = lpBuddyMsg->m_arrContent[i];
		if (lpContent != NULL)
		{
			switch (lpContent->m_nType)
			{
			case CONTENT_TYPE_FONT_INFO:
				{
					fontInfo.m_strName = lpContent->m_FontInfo.m_strName;
					fontInfo.m_nSize = lpContent->m_FontInfo.m_nSize;
					fontInfo.m_clrText = lpContent->m_FontInfo.m_clrText;
					fontInfo.m_bBold = lpContent->m_FontInfo.m_bBold;
					fontInfo.m_bItalic = lpContent->m_FontInfo.m_bItalic;
					fontInfo.m_bUnderLine = lpContent->m_FontInfo.m_bUnderLine;
				}
				break;

			case CONTENT_TYPE_TEXT:
				{
					RichEdit_SetFont(m_richRecv, fontInfo.m_strName.c_str(),
						fontInfo.m_nSize, fontInfo.m_clrText, fontInfo.m_bBold, 
						fontInfo.m_bItalic, fontInfo.m_bUnderLine);
					strText.Format(_T("%s"), lpContent->m_strText.c_str());
					RichEdit_AppendText(m_richRecv, strText);
				}
				break;

			case CONTENT_TYPE_FACE:
				{
					if (m_lpFaceList != NULL)
					{
						CFaceInfo * lpFaceInfo = m_lpFaceList->GetFaceInfoById(lpContent->m_nFaceId);
						if (lpFaceInfo != NULL)
						{
							RichEdit_InsertFace(m_richRecv, lpFaceInfo->m_strFileName.c_str(), lpFaceInfo->m_nId);
						}
					}
				}
				break;

			case CONTENT_TYPE_CUSTOM_FACE:
				{
					CString strFullName = m_lpQQClient->GetChatPicFullName(lpContent->m_strCFaceName.c_str()).c_str();
					if (!ZYM::CPath::IsFileExist(strFullName))
						strFullName = ZYM::CPath::GetCurDir() + _T("Image\\DownloadFailed.gif");
					RichEdit_InsertFace(m_richRecv, strFullName, 0);
				}
				break;
			}
		}
	}

	RichEdit_AppendText(m_richRecv, _T("\r\n"));
}

// "/f[表情id]/c[自定义表情文件名]"
void CBuddyChatDlg::AddMsg(LPCTSTR lpText)
{
	CString strTemp;
	int nLen;

	if (NULL == lpText || NULL == *lpText)
		return;

	CString strTitle, strTime;
	strTime = FormatTime(time(NULL), _T("%H:%M:%S"));
	strTitle.Format(_T("%s  %s\r\n"), m_strUserName, strTime);
	RichEdit_SetFont(m_richRecv, _T("宋体"), 9, RGB(0,128,64), FALSE, FALSE, FALSE);
	RichEdit_SetStartIndent(m_richRecv, 0);
	RichEdit_AppendText(m_richRecv, strTitle);

	RichEdit_SetStartIndent(m_richRecv, 300);

	nLen = (int)_tcslen(lpText);
	for (int i = 0; i < nLen; i++)
	{
		if (lpText[i] == _T('/'))
		{
			if (lpText[i+1] == _T('/'))
			{
				strTemp += lpText[i];
				i++;
				continue;
			}
			else if (i < (nLen - 4))
			{
				if (lpText[i+1] == _T('f') && lpText[i+2] == _T('[') && lpText[i+3] != _T(']'))
				{
					const TCHAR * lpStart = &lpText[i+2];
					const TCHAR * lpEnd = _tcschr(&lpText[i+3], _T(']'));
					if (NULL == lpEnd)
					{
						strTemp += lpText[i];
						continue;
					}

					int nBufLen = lpEnd - lpStart + 1;
					if (nBufLen >= 1024)
					{
						strTemp += lpText[i];
						continue;
					}

					if (!strTemp.IsEmpty())
					{
						RichEdit_SetFont(m_richRecv, m_fontInfo.m_strName.c_str(),
							m_fontInfo.m_nSize, m_fontInfo.m_clrText, m_fontInfo.m_bBold,
							m_fontInfo.m_bItalic, m_fontInfo.m_bUnderLine);
						RichEdit_AppendText(m_richRecv, strTemp);
						strTemp = _T("");
					}

					TCHAR cBuf[1024] = {0};
					_tcsncpy(cBuf, lpStart, nBufLen);

					LPCTSTR lpFaceFmt = _T("[%d]");
					int nFaceId = 0;

					_stscanf(cBuf, lpFaceFmt, &nFaceId);

					CFaceInfo * lpFaceInfo = m_lpFaceList->GetFaceInfoById(nFaceId);
					if (lpFaceInfo != NULL)
					{
						RichEdit_InsertFace(m_richRecv, 
							lpFaceInfo->m_strFileName.c_str(), lpFaceInfo->m_nId);
					}

					i += (nBufLen + 1);
					continue;
				}
			}
		}

		strTemp += lpText[i];
	}

	if (!strTemp.IsEmpty())
	{
		RichEdit_SetFont(m_richRecv, m_fontInfo.m_strName.c_str(),
			m_fontInfo.m_nSize, m_fontInfo.m_clrText, m_fontInfo.m_bBold,
			m_fontInfo.m_bItalic, m_fontInfo.m_bUnderLine);
		RichEdit_AppendText(m_richRecv, strTemp);
		strTemp = _T("");
	}

	RichEdit_AppendText(m_richRecv, _T("\r\n"));
}