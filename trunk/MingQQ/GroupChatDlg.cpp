#include "StdAfx.h"
#include "GroupChatDlg.h"

CFontInfo g_groupFontInfo;

CGroupChatDlg::CGroupChatDlg(void)
{
	m_lpQQClient = NULL;
	m_lpFaceList = NULL;
	m_hMainDlg = NULL;
	m_nGroupCode = 0;

	m_hDlgIcon = m_hDlgSmallIcon = NULL;

	m_bIsHasGroupInfo = FALSE;
	m_bIsHasGroupNumber = FALSE;
	m_bIsHasGMemberNumber = FALSE;
	m_nGMemberNumberReq = 0;

	m_nGroupId = m_nGroupNumber = 0;
	m_nUserNumber = 0;
	m_strGroupName = _T("群");
	m_strUserName = _T("");
	m_nMemberCnt = m_nOnlineMemberCnt = 0;
}

CGroupChatDlg::~CGroupChatDlg(void)
{
}

BOOL CGroupChatDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CGroupChatDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	CenterWindow();

	// set icons
	m_hDlgIcon = AtlLoadIconImage(IDI_GROUPCHATDLG_32, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(m_hDlgIcon, TRUE);
	m_hDlgSmallIcon = AtlLoadIconImage(IDI_GROUPCHATDLG_16, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(m_hDlgSmallIcon, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);

	if (!m_bIsHasGroupInfo)
	{
		if (m_lpQQClient != NULL)
			m_lpQQClient->UpdateGroupInfo(m_nGroupCode);
	}
	else
	{
		CGroupInfo * lpGroupInfo = GetGroupInfoPtr();
		if (lpGroupInfo != NULL)
		{
			if (!m_bIsHasGroupNumber)
				m_lpQQClient->UpdateGroupNum(m_nGroupCode);

			if (!m_bIsHasGMemberNumber && m_nGMemberNumberReq <= 0)
			{
				std::vector<UINT> arrQQUin;
				for (int i = 0; i < lpGroupInfo->GetMemberCount(); i++)
				{
					CBuddyInfo * lpBuddyInfo = lpGroupInfo->GetMember(i);
					if (lpBuddyInfo != NULL && 0 == lpBuddyInfo->m_nQQNum)
						arrQQUin.push_back(lpBuddyInfo->m_nQQUin);
				}
				m_nGMemberNumberReq = arrQQUin.size();
				if (m_nGMemberNumberReq > 0)
					m_lpQQClient->UpdateGroupMemberNum(m_nGroupCode, &arrQQUin);
				else
					m_bIsHasGMemberNumber = TRUE;
				arrQQUin.clear();
			}
		}
	}

	UpdateData();
	SetWindowText(m_strGroupName);
	InitCtrls();

	return TRUE;
}

void CGroupChatDlg::OnClose()
{
	DestroyWindow();
	//::PostMessage(m_hMainDlg, WM_CLOSE_GROUPCHATDLG, m_nGroupCode, 0);
}

void CGroupChatDlg::OnDestroy()
{
	SetMsgHandled(FALSE);

	UnInitCtrls();	// 反初始化控件

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

	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
}

// “群名称”超链接控件
void CGroupChatDlg::OnLnk_GroupName(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	::PostMessage(m_hMainDlg, WM_SHOW_GROUPINFODLG, m_nGroupCode, NULL);
}

// “表情”按钮
void CGroupChatDlg::OnBtn_Face(UINT uNotifyCode, int nID, CWindow wndCtl)
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
void CGroupChatDlg::OnCbo_SelChange_FontName(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strText;
	m_cboFontName.GetWindowText(strText);

	if (m_fontInfo.m_strName != strText.GetBuffer())
	{
		m_fontInfo.m_strName = strText.GetBuffer();
		RichEdit_SetDefFont(m_richSend, m_fontInfo.m_strName.c_str(),
			m_fontInfo.m_nSize, m_fontInfo.m_clrText, m_fontInfo.m_bBold,
			m_fontInfo.m_bItalic, m_fontInfo.m_bUnderLine);
		g_groupFontInfo.m_strName = m_fontInfo.m_strName;
	}
}

// “字体大小”组合框
void CGroupChatDlg::OnCbo_SelChange_FontSize(UINT uNotifyCode, int nID, CWindow wndCtl)
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
		g_groupFontInfo.m_nSize = m_fontInfo.m_nSize;
	}
}

// “加粗”复选框
void CGroupChatDlg::OnChk_Bold(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCheck = m_btnBold.GetCheck();
	BOOL bBold = ((nCheck == BST_CHECKED) ? TRUE : FALSE);
	if (m_fontInfo.m_bBold != bBold)
	{
		m_fontInfo.m_bBold = bBold;
		RichEdit_SetDefFont(m_richSend, m_fontInfo.m_strName.c_str(),
			m_fontInfo.m_nSize, m_fontInfo.m_clrText, m_fontInfo.m_bBold,
			m_fontInfo.m_bItalic, m_fontInfo.m_bUnderLine);
		g_groupFontInfo.m_bBold = m_fontInfo.m_bBold;
	}
}

// “斜体”复选框
void CGroupChatDlg::OnChk_Italic(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCheck = m_btnItalic.GetCheck();
	BOOL bItalic = ((nCheck == BST_CHECKED) ? TRUE : FALSE);
	if (m_fontInfo.m_bItalic != bItalic)
	{
		m_fontInfo.m_bItalic = bItalic;
		RichEdit_SetDefFont(m_richSend, m_fontInfo.m_strName.c_str(),
			m_fontInfo.m_nSize, m_fontInfo.m_clrText, m_fontInfo.m_bBold,
			m_fontInfo.m_bItalic, m_fontInfo.m_bUnderLine);
		g_groupFontInfo.m_bItalic = m_fontInfo.m_bItalic;
	}
}

// “下划线”复选框
void CGroupChatDlg::OnChk_UnderLine(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCheck = m_btnUnderLine.GetCheck();
	BOOL bUnderLine = ((nCheck == BST_CHECKED) ? TRUE : FALSE);
	if (m_fontInfo.m_bUnderLine != bUnderLine)
	{
		m_fontInfo.m_bUnderLine = bUnderLine;
		RichEdit_SetDefFont(m_richSend, m_fontInfo.m_strName.c_str(),
			m_fontInfo.m_nSize, m_fontInfo.m_clrText, m_fontInfo.m_bBold,
			m_fontInfo.m_bItalic, m_fontInfo.m_bUnderLine);
		g_groupFontInfo.m_bUnderLine = m_fontInfo.m_bUnderLine;
	}
}

// “颜色”按钮
void CGroupChatDlg::OnBtn_Color(UINT uNotifyCode, int nID, CWindow wndCtl)
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
			g_groupFontInfo.m_clrText = m_fontInfo.m_clrText;
		}
	}
}

// “关闭”按钮
void CGroupChatDlg::OnBtn_Close(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	SendMessage(WM_CLOSE);
}

// “发送”按钮
void CGroupChatDlg::OnBtn_Send(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (NULL == m_lpQQClient)
		return;

	CString strText;
	RichEdit_GetText(m_richSend, strText);

	m_lpQQClient->SendGroupMsg(m_nGroupId, strText, _T("宋体"), 
		9, RGB(0, 0, 0), FALSE, FALSE, FALSE);

	m_richSend.SetWindowText(_T(""));

	AddMsg(strText);
}

//	“接收消息”富文本框链接点击消息
LRESULT CGroupChatDlg::OnRichEdit_Recv_Link(LPNMHDR pnmh)
{
	if (pnmh->code == EN_LINK)
	{
		ENLINK *pLink = (ENLINK *)pnmh;
		if (pLink->msg == WM_LBUTTONUP)
		{
			m_richRecv.SetSel(pLink->chrg);
			m_richRecv.GetSelText(m_strCurLink);

			if (m_strCurLink.Left(7).MakeLower() == _T("http://"))
				::ShellExecute(NULL, _T("open"), m_strCurLink, NULL, NULL, SW_SHOWNORMAL);
			else
			{
				DWORD dwPos = GetMessagePos();
				CPoint point(LOWORD(dwPos), HIWORD(dwPos));

				CMenu menu;
				menu.LoadMenu(IDR_MENU2);
				CMenuHandle menuPopup = menu.GetSubMenu(0);
				menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, m_hWnd);
				menu.DestroyMenu();
			}
		}
	}
	return 0;
}

// “群成员”列表双击消息
LRESULT CGroupChatDlg::OnGMemberList_DblClick(LPNMHDR pnmh)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW *)pnmh;
	if (pNMListView->iItem != -1)
	{
		UINT nQQUin = (UINT)m_ListCtrl.GetItemData(pNMListView->iItem);
		::PostMessage(m_hMainDlg, WM_SHOW_SESSCHATDLG, m_nGroupCode, nQQUin);
	}
	return 0;
}

// “群成员”列表右键单击消息
LRESULT CGroupChatDlg::OnGMemberList_RClick(LPNMHDR pnmh)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW *)pnmh;
	if (pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();
		CPoint point(LOWORD(dwPos), HIWORD(dwPos));

		CMenu menu;
		menu.LoadMenu(IDR_MENU2);
		CMenuHandle menuPopup = menu.GetSubMenu(0);
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, m_hWnd);
		menu.DestroyMenu();
	}
	return 0;
}

// “剪切”菜单
void CGroupChatDlg::OnMenu_Cut(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_richSend.Cut();
}

// “复制”菜单
void CGroupChatDlg::OnMenu_Copy(UINT uNotifyCode, int nID, CWindow wndCtl)
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
void CGroupChatDlg::OnMenu_Paste(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_richSend.PasteSpecial(CF_TEXT);
}

// “全部选择”菜单
void CGroupChatDlg::OnMenu_SelAll(UINT uNotifyCode, int nID, CWindow wndCtl)
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
void CGroupChatDlg::OnMenu_Clear(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_richRecv.SetWindowText(_T(""));
}

// “查看资料”菜单
void CGroupChatDlg::OnMenu_ViewInfo(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));

	HWND hWnd = ::WindowFromPoint(point);
	if (hWnd == m_richRecv.m_hWnd)
	{
		UINT nQQNum = _tcstol(m_strCurLink, NULL, 10);
		CGroupInfo * lpGroupInfo = GetGroupInfoPtr();
		if (lpGroupInfo != NULL)
		{
			CBuddyInfo * lpBuddyInfo = lpGroupInfo->GetMemberByNum(nQQNum);
			if (lpBuddyInfo != NULL)
				::PostMessage(m_hMainDlg, WM_SHOW_GMEMBERINFODLG, m_nGroupCode, lpBuddyInfo->m_nQQUin);
		}
	}
	else if (hWnd == m_ListCtrl.m_hWnd)
	{
		int nSelItem = m_ListCtrl.GetNextItem(-1, LVIS_SELECTED);
		if (nSelItem > -1)
		{
			UINT nQQUin = (UINT)m_ListCtrl.GetItemData(nSelItem);
			::PostMessage(m_hMainDlg, WM_SHOW_GMEMBERINFODLG, m_nGroupCode, nQQUin);
		}
	}
}

// “发送消息”菜单
void CGroupChatDlg::OnMenu_SendMsg(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	DWORD dwPos = GetMessagePos();
	CPoint point(LOWORD(dwPos), HIWORD(dwPos));

	HWND hWnd = ::WindowFromPoint(point);
	if (hWnd == m_richRecv.m_hWnd)
	{
		UINT nQQNum = _tcstol(m_strCurLink, NULL, 10);
		CGroupInfo * lpGroupInfo = GetGroupInfoPtr();
		if (lpGroupInfo != NULL)
		{
			CBuddyInfo * lpBuddyInfo = lpGroupInfo->GetMemberByNum(nQQNum);
			if (lpBuddyInfo != NULL)
				::PostMessage(m_hMainDlg, WM_SHOW_SESSCHATDLG, m_nGroupCode, lpBuddyInfo->m_nQQUin);
		}
	}
	else if (hWnd == m_ListCtrl.m_hWnd)
	{
		int nSelItem = m_ListCtrl.GetNextItem(-1, LVIS_SELECTED);
		if (nSelItem > -1)
		{
			UINT nQQUin = (UINT)m_ListCtrl.GetItemData(nSelItem);
			::PostMessage(m_hMainDlg, WM_SHOW_SESSCHATDLG, m_nGroupCode, nQQUin);
		}
	}
}

// “表情”控件选取消息
LRESULT CGroupChatDlg::OnFaceCtrlSel(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RichEdit_InsertFace(m_richSend, m_FaceSelDlg.GetSelFaceFileName(), m_FaceSelDlg.GetSelFaceId());
	m_richSend.SetFocus();
	return 0;
}

// 接收群消息
void CGroupChatDlg::OnRecvMsg(UINT nGroupCode, UINT nMsgId)
{
	if (NULL == m_lpQQClient || m_nGroupCode != nGroupCode || !m_bIsHasGroupInfo)
		return;

	if (::GetForegroundWindow() != m_hWnd)
		FlashWindowEx(m_hWnd, 3);

	if (nMsgId == 0)
	{
		CMessageList * lpMsgList = m_lpQQClient->GetMessageList();
		if (lpMsgList != NULL)
		{
			CMessageSender * lpMsgSender = lpMsgList->GetMsgSender(QQ_MSG_TYPE_GROUP, nGroupCode);
			if (lpMsgSender != NULL)
			{
				int nMsgCnt = lpMsgSender->GetMsgCount();
				for (int i = 0; i < nMsgCnt; i++)
				{
					CGroupMessage * lpGroupMsg = lpMsgSender->GetGroupMsg(i);
					if (lpGroupMsg != NULL)
					{
						AddMsg(lpGroupMsg);
					}
				}
				lpMsgList->DelMsgSender(QQ_MSG_TYPE_GROUP, nGroupCode);
				::PostMessage(m_hMainDlg, WM_DEL_MSG_SENDER, QQ_MSG_TYPE_GROUP, nGroupCode);
			}
		}
	}
	else
	{
		CMessageList * lpMsgList = m_lpQQClient->GetMessageList();
		if (lpMsgList != NULL)
		{
			CMessageSender * lpMsgSender = lpMsgList->GetMsgSender(QQ_MSG_TYPE_GROUP, nGroupCode);
			if (lpMsgSender != NULL)
			{
				CGroupMessage * lpGroupMsg = lpMsgSender->GetGroupMsgById(nMsgId);
				if (lpGroupMsg != NULL)
				{
					AddMsg(lpGroupMsg);
					lpMsgSender->DelMsgById(nMsgId);
				}

				if (lpMsgSender->GetMsgCount() <= 0)
					lpMsgList->DelMsgSender(QQ_MSG_TYPE_GROUP, nGroupCode);
			}
		}
	}
}

// 更新群信息
void CGroupChatDlg::OnUpdateGroupInfo()
{
	m_bIsHasGroupInfo = TRUE;

	UpdateData();						// 更新信息

	UpdateDlgTitle();					// 更新对话框标题
	UpdateGroupNameCtrl();				// 更新群名称控件
	UpdateGroupMemo();					// 更新群公告
	UpdateGroupMemberList();			// 更新群成员列表

	OnRecvMsg(m_nGroupCode, NULL);		// 显示消息

	CGroupInfo * lpGroupInfo = GetGroupInfoPtr();
	if (lpGroupInfo != NULL)
	{
		if (!m_bIsHasGroupNumber)
			m_lpQQClient->UpdateGroupNum(m_nGroupCode);

		if (!m_bIsHasGMemberNumber && m_nGMemberNumberReq <= 0)
		{
			std::vector<UINT> arrQQUin;
			for (int i = 0; i < lpGroupInfo->GetMemberCount(); i++)
			{
				CBuddyInfo * lpBuddyInfo = lpGroupInfo->GetMember(i);
				if (lpBuddyInfo != NULL && 0 == lpBuddyInfo->m_nQQNum)
					arrQQUin.push_back(lpBuddyInfo->m_nQQUin);
			}
			m_nGMemberNumberReq = arrQQUin.size();
			if (m_nGMemberNumberReq > 0)
				m_lpQQClient->UpdateGroupMemberNum(m_nGroupCode, &arrQQUin);
			else
				m_bIsHasGMemberNumber = TRUE;
			arrQQUin.clear();
		}
	}
}

// 更新群号码
void CGroupChatDlg::OnUpdateGroupNumber()
{
	m_bIsHasGroupNumber = TRUE;
	UpdateData();						// 更新信息
	UpdateGroupNameCtrl();				// 更新群名称控件
}

// 更新群成员号码
void CGroupChatDlg::OnUpdateGMemberNumber(WPARAM wParam, LPARAM lParam)
{
	m_nGMemberNumberReq--;

	CGetQQNumResult * lpGetQQNumResult = (CGetQQNumResult *)lParam;
	if (NULL == lpGetQQNumResult)
		return;
	UINT nQQUin = lpGetQQNumResult->m_nQQUin;
	
	UpdateData();						// 更新信息

	CString strNickName, strText;

	int nCount = m_ListCtrl.GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		UINT nQQUin2 = (UINT)m_ListCtrl.GetItemData(i);
		if (nQQUin2 == nQQUin)
		{
			CGroupInfo * lpGroupInfo = GetGroupInfoPtr();
			if (lpGroupInfo != NULL)
			{
				CBuddyInfo * lpBuddyInfo = lpGroupInfo->GetMemberByUin(nQQUin);
				if (lpBuddyInfo != NULL)
				{
					if (!lpBuddyInfo->m_strGroupCard.empty())
						strNickName = lpBuddyInfo->m_strGroupCard.c_str();
					else
						strNickName = lpBuddyInfo->m_strNickName.c_str();

					if (lpBuddyInfo->m_nQQNum != 0)
						strText.Format(_T("%s(%u)"), strNickName, lpBuddyInfo->m_nQQNum);
					else
						strText.Format(_T("%s"), strNickName);
					m_ListCtrl.SetItemText(i, 0, strText);
				}
			}
			break;
		}
	}
}

// 获取群信息指针
CGroupInfo * CGroupChatDlg::GetGroupInfoPtr()
{
	if (m_lpQQClient != NULL)
	{
		CGroupList * lpGroupList = m_lpQQClient->GetGroupList();
		if (lpGroupList != NULL)
			return lpGroupList->GetGroupByCode(m_nGroupCode);
	}
	return NULL;
}

// 获取用户信息指针
CBuddyInfo * CGroupChatDlg::GetUserInfoPtr()
{
	if (m_lpQQClient != NULL)
	{
		CBuddyInfo * lpUserInfo = m_lpQQClient->GetUserInfo();
		if (lpUserInfo != NULL)
		{
			CBuddyInfo * lpGMemberInfo = NULL;
			CGroupInfo * lpGroupInfo = GetGroupInfoPtr();
			if (lpGroupInfo != NULL)
				lpGMemberInfo = lpGroupInfo->GetMemberByUin(lpUserInfo->m_nQQUin);
			return (lpGMemberInfo != NULL) ? lpGMemberInfo : lpUserInfo;
		}
	}
	return NULL;
}

// 更新信息
void CGroupChatDlg::UpdateData()
{
	CGroupInfo * lpGroupInfo = GetGroupInfoPtr();
	if (lpGroupInfo != NULL)
	{
		m_nMemberCnt = lpGroupInfo->GetMemberCount();
		m_nOnlineMemberCnt = lpGroupInfo->GetOnlineMemberCount();
		m_nGroupId = lpGroupInfo->m_nGroupId;
		m_nGroupNumber = lpGroupInfo->m_nGroupNumber;
		m_strGroupName = lpGroupInfo->m_strName.c_str();
	}

	CBuddyInfo * lpUserInfo = GetUserInfoPtr();
	if (lpUserInfo != NULL)
	{
		if (!lpUserInfo->m_strGroupCard.empty())
			m_strUserName = lpUserInfo->m_strGroupCard.c_str();
		else
			m_strUserName = lpUserInfo->m_strNickName.c_str();
		m_nUserNumber = lpUserInfo->m_nQQNum;
	}
}

// 更新对话框标题栏
void CGroupChatDlg::UpdateDlgTitle()
{
	SetWindowText(m_strGroupName);
}

// 更新群名称超链接控件
BOOL CGroupChatDlg::UpdateGroupNameCtrl()
{
	CString strText;
	if (m_nGroupNumber != 0)
		strText.Format(_T("%s(%u)"), m_strGroupName, m_nGroupNumber);
	else
		strText.Format(_T("%s"), m_strGroupName);
	m_lnkGroupName.SetLabel(strText);
	return TRUE;
}

// 更新群公告
BOOL CGroupChatDlg::UpdateGroupMemo()
{
	CGroupInfo * lpGroupInfo = GetGroupInfoPtr();
	if (lpGroupInfo != NULL)
	{
		if (!lpGroupInfo->m_strMemo.empty())
		{
			m_edtMemo.SetWindowText(lpGroupInfo->m_strMemo.c_str());
			return TRUE;
		}
	}
	m_edtMemo.SetWindowText(_T("暂无公告"));
	return TRUE;
}

// 更新群成员列表
BOOL CGroupChatDlg::UpdateGroupMemberList()
{
	if (NULL == m_lpQQClient)
		return FALSE;

	CGroupList * lpGroupList = m_lpQQClient->GetGroupList();
	if (NULL == lpGroupList)
		return FALSE;

	CGroupInfo * lpGroupInfo = lpGroupList->GetGroupByCode(m_nGroupCode);
	if (NULL == lpGroupInfo)
		return FALSE;

	CString strText, strNickName;

	strText.Format(_T("群成员(%d/%d)"), m_nOnlineMemberCnt, m_nMemberCnt);
	LVCOLUMN stColumn;
	stColumn.mask = LVCF_TEXT;
	stColumn.pszText = strText.GetBuffer(0);
	stColumn.cchTextMax = strText.GetLength();
	m_ListCtrl.SetColumn(0, &stColumn);

	m_ListCtrl.DeleteAllItems();
	for (int i = 0; i < lpGroupInfo->GetMemberCount(); i++)
	{
		CBuddyInfo * lpBuddyInfo = lpGroupInfo->GetMember(i);
		if (lpBuddyInfo != NULL)
		{
			if (!lpBuddyInfo->m_strGroupCard.empty())
				strNickName = lpBuddyInfo->m_strGroupCard.c_str();
			else
				strNickName = lpBuddyInfo->m_strNickName.c_str();

			if (lpBuddyInfo->m_nQQNum != 0)
				strText.Format(_T("%s(%u)"), strNickName, lpBuddyInfo->m_nQQNum);
			else
				strText.Format(_T("%s"), strNickName);

			BOOL bGray = lpBuddyInfo->m_nStatus != QQ_STATUS_OFFLINE ? FALSE : TRUE;
			int nIndex = m_headImageList.GetHeadPicIndex(QQ_MSG_TYPE_SESS, 
				m_nGroupCode, lpBuddyInfo->m_nQQUin, bGray, TRUE);
			m_ListCtrl.InsertItem(i, strText, nIndex);
			m_ListCtrl.SetItemData(i, lpBuddyInfo->m_nQQUin);
		}
	}
	return TRUE;
}

// 初始化控件
BOOL CGroupChatDlg::InitCtrls()
{
	m_picHead = GetDlgItem(ID_PIC_HEAD);
	m_lnkGroupName.SubclassWindow(GetDlgItem(ID_LINK_GROUPNAME));
	m_picAD = GetDlgItem(ID_PIC_AD);
	m_edtMemo = GetDlgItem(ID_EDIT_MEMO);
	m_ListCtrl = GetDlgItem(ID_LIST_MERBER);
	m_cboFontName = GetDlgItem(ID_COMBO_FONT_NAME);
	m_cboFontSize = GetDlgItem(ID_COMBO_FONT_SIZE);
	m_btnBold = GetDlgItem(ID_CHECK_BOLD);
	m_btnItalic = GetDlgItem(ID_CHECK_ITALIC);
	m_btnUnderLine = GetDlgItem(ID_CHECK_UNDERLINE);
	m_richRecv = GetDlgItem(ID_RICHEDIT_RECV);
	m_richSend = GetDlgItem(ID_RICHEDIT_SEND);
	
	// 群名称超链接控件
	DWORD dwStyle = m_lnkGroupName.GetHyperLinkExtendedStyle();
	dwStyle = dwStyle | HLINK_COMMANDBUTTON | HLINK_UNDERLINEHOVER;
	m_lnkGroupName.SetHyperLinkExtendedStyle(dwStyle);
	m_lnkGroupName.m_clrLink = RGB(0,0,0);

	m_fontInfo = g_groupFontInfo;

	// 发送消息富文本框控件
	RichEdit_SetDefFont(m_richSend, m_fontInfo.m_strName.c_str(),
		m_fontInfo.m_nSize, m_fontInfo.m_clrText, m_fontInfo.m_bBold,
		m_fontInfo.m_bItalic, m_fontInfo.m_bUnderLine);

	// 接收消息富文本框控件
	DWORD dwMask = m_richRecv.GetEventMask();
	dwMask = dwMask | ENM_LINK  | ENM_MOUSEEVENTS | ENM_SCROLLEVENTS | ENM_KEYEVENTS;
	m_richRecv.SetEventMask(dwMask);

	m_richRecv.SetReadOnly();

	InitMidToolBar();

	// 群成员列表控件
	dwStyle = m_ListCtrl.GetStyle();
	dwStyle |= LVS_REPORT | LVS_SINGLESEL;
	m_ListCtrl.SetWindowLong(GWL_STYLE, dwStyle);

	DWORD dwExStyle = m_ListCtrl.GetExtendedListViewStyle();
	dwExStyle = dwExStyle | LVS_EX_FULLROWSELECT;
	m_ListCtrl.SetExtendedListViewStyle(dwExStyle);

	m_headImageList.Init(16, 16, m_lpQQClient);

	CString strText;
	strText.Format(_T("群成员(%d/%d)"), m_nOnlineMemberCnt, m_nMemberCnt);
	m_ListCtrl.InsertColumn(0, strText, LVCFMT_LEFT | LVCFMT_IMAGE, 170);
	m_ListCtrl.SetImageList(m_headImageList.GetImageList(), LVSIL_SMALL);

	UpdateGroupNameCtrl();	// 更新群名称控件
	UpdateDlgTitle();		// 更新对话框标题
	UpdateGroupMemo();		// 更新群公告
	UpdateGroupMemberList();// 更新群成员列表

	HRESULT hr = S_FALSE;
	
	if (m_lpQQClient != NULL && m_nGroupNumber != 0)
	{
		CString strFullName = m_lpQQClient->GetHeadPicFullName(m_nGroupNumber, 0).c_str();
		hr = m_imgHead.Load(strFullName);
	}
	if (hr != S_OK)
		hr = m_imgHead.Load(ZYM::CPath::GetCurDir() + _T("Image\\DefGroupHeadPic.png"));
	if (S_OK == hr)
		m_picHead.SetBitmap((HBITMAP)m_imgHead);
	
	//m_picAD.MoveWindow(381, 18, 200, 42);
	hr = m_imgAD.Load(ZYM::CPath::GetCurDir() + _T("Image\\AD_4.png"));
	if (S_OK == hr)
		m_picAD.SetBitmap((HBITMAP)m_imgAD);
	
	m_Accelerator.LoadAccelerators(ID_ACCE_CHATDLG);
	m_menuRichEdit.LoadMenu(IDR_MENU1);

	return TRUE;
}

// 反初始化控件
BOOL CGroupChatDlg::UnInitCtrls()
{
	m_Accelerator.DestroyObject();
	m_menuRichEdit.DestroyMenu();

	m_headImageList.UnInit();

	if (!m_imgHead.IsNull())
		m_imgHead.Destroy();

	if (!m_imgAD.IsNull())
		m_imgAD.Destroy();

	if (m_picHead.IsWindow())
		m_picHead.DestroyWindow();

	if (m_lnkGroupName.IsWindow())
	{
		m_lnkGroupName.DestroyWindow();
		m_lnkGroupName.m_tip.m_hWnd = NULL;
		m_lnkGroupName.m_hFontNormal = NULL;
	}

	if (m_picAD.IsWindow())
		m_picAD.DestroyWindow();

	if (m_edtMemo.IsWindow())
		m_edtMemo.DestroyWindow();

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

	m_ListCtrl.DeleteAllItems();

	if (m_ListCtrl.IsWindow())
		m_ListCtrl.DestroyWindow();

	if (m_richRecv.IsWindow())
		m_richRecv.DestroyWindow();

	if (m_richSend.IsWindow())
		m_richSend.DestroyWindow();

	if (m_FaceSelDlg.IsWindow())
		m_FaceSelDlg.DestroyWindow();

	return TRUE;
}

// 初始化中间工具栏
BOOL CGroupChatDlg::InitMidToolBar()
{
	std::vector<tstring> arrSysFont;
	EnumSysFont(&arrSysFont);

	for (int i = 0; i < (int)arrSysFont.size(); i++)
	{
		m_cboFontName.AddString(arrSysFont[i].c_str());
	}
	arrSysFont.clear();
	int nIndex = m_cboFontName.FindString(0, _T("宋体"));
	m_cboFontName.SetCurSel(nIndex);

	CString strFontSize;
	for (int i = 8; i <= 22; i++)
	{
		strFontSize.Format(_T("%d"), i);
		m_cboFontSize.AddString(strFontSize);
	}
	nIndex = m_cboFontSize.FindString(0, _T("9"));
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

void CGroupChatDlg::AddMsg(CGroupMessage * lpGroupMsg)
{
	if (NULL == lpGroupMsg || NULL == m_lpQQClient)
		return;

	CString strText, strTime;
	CFontInfo fontInfo;

	if (IsToday(lpGroupMsg->m_nTime))
		strTime = FormatTime(lpGroupMsg->m_nTime, _T("%H:%M:%S"));
	else
		strTime = FormatTime(lpGroupMsg->m_nTime, _T("%Y-%m-%d %H:%M:%S"));

	CString strSenderName;
	UINT nSenderNum;
	GetSenderInfo(lpGroupMsg->m_nSendUin, strSenderName, nSenderNum);

	m_richRecv.SetAutoURLDetect(FALSE);

	if (nSenderNum != 0)
	{
		strText.Format(_T("%s("), strSenderName);
		RichEdit_SetFont(m_richRecv, _T("宋体"), 9, RGB(0, 0, 255), FALSE, FALSE, FALSE);
		RichEdit_SetStartIndent(m_richRecv, 0);
		RichEdit_AppendText(m_richRecv, strText);

		strText.Format(_T("%u"), nSenderNum);
		CHARFORMAT2 cf;
		memset(&cf, 0, sizeof(CHARFORMAT2));
		cf.cbSize = sizeof(CHARFORMAT2); 
		cf.dwMask = CFM_LINK; 
		cf.dwEffects |= CFE_LINK;
		m_richRecv.SetCharFormat(cf, SCF_SELECTION);
		m_richRecv.AppendText(strText, FALSE);

		strText.Format(_T(")  %s\r\n"), strTime);
		RichEdit_SetFont(m_richRecv, _T("宋体"), 9, RGB(0, 0, 255), FALSE, FALSE, FALSE);
		RichEdit_AppendText(m_richRecv, strText);
	}
	else
	{
		strText.Format(_T("%s  %s\r\n"), strSenderName, strTime);
		RichEdit_SetFont(m_richRecv, _T("宋体"), 9, RGB(0, 0, 255), FALSE, FALSE, FALSE);
		RichEdit_SetStartIndent(m_richRecv, 0);
		RichEdit_AppendText(m_richRecv, strText);
	}

	m_richRecv.SetAutoURLDetect(TRUE);

	RichEdit_SetStartIndent(m_richRecv, 300);
	for (int i = 0; i < (int)lpGroupMsg->m_arrContent.size(); i++)
	{
		CContent * lpContent = lpGroupMsg->m_arrContent[i];
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
void CGroupChatDlg::AddMsg(LPCTSTR lpText)
{
	CString strTemp;
	int nLen;

	if (NULL == lpText || NULL == *lpText)
		return;

	CString strTitle, strTime;
	strTime = FormatTime(time(NULL), _T("%H:%M:%S"));
	strTitle.Format(_T("%s(%u)  %s\r\n"), m_strUserName, m_nUserNumber, strTime);
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

void CGroupChatDlg::GetSenderInfo(UINT nQQUin, CString& strName, UINT& nQQNum)
{
	strName = _T("");
	nQQNum = 0;

	CGroupInfo * lpGroupInfo = GetGroupInfoPtr();
	if (lpGroupInfo != NULL)
	{
		CBuddyInfo * lpBuddyInfo = lpGroupInfo->GetMemberByUin(nQQUin);
		if (lpBuddyInfo != NULL)
		{
			if (!lpBuddyInfo->m_strGroupCard.empty())
				strName = lpBuddyInfo->m_strGroupCard.c_str();
			else
				strName = lpBuddyInfo->m_strNickName.c_str();
			nQQNum = lpBuddyInfo->m_nQQNum;
		}
	}
}