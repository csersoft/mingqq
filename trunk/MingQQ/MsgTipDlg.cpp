#include "StdAfx.h"
#include "MsgTipDlg.h"

CMsgTipDlg::CMsgTipDlg(void)
{
	m_lpQQClient = NULL;
	m_hMainDlg = NULL;
	m_rcTrayIcon.SetRectEmpty();
	m_dwTimerId = NULL;
	m_nListItemHeight = 17;
}

CMsgTipDlg::~CMsgTipDlg(void)
{
}

BOOL CMsgTipDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE);

	InitCtrls();			// 初始化控件

	CString strFileName = ZYM::CPath::GetCurDir() + _T("Image\\MsgTipBg.png");	// 加载背景图片
	m_imgBg.LoadFromFile(strFileName);
	m_imgBg.SetNinePart(CRect(8,28,8,8));

	SetDlgAutoSize();		// 自动调整对话框大小

	return FALSE;
}

void CMsgTipDlg::OnPaint(CDCHandle dc)
{
	CPaintDC PaintDC(m_hWnd);

	CRect rcClient;
	GetClientRect(&rcClient);

	if (!m_imgBg.IsNull())
		m_imgBg.Draw2(PaintDC.m_hDC, rcClient);
}

HBRUSH CMsgTipDlg::OnCtlColorStatic(CDCHandle dc, CStatic wndStatic)
{
	return (HBRUSH)::GetStockObject(NULL_BRUSH);
}

void CMsgTipDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_dwTimerId)
	{
		RECT rcWindow;
		GetWindowRect(&rcWindow);

		POINT pt = {0};
		::GetCursorPos(&pt);

		if (!::PtInRect(&rcWindow, pt))
		{
			KillTimer(m_dwTimerId);
			m_dwTimerId = NULL;
			DestroyWindow();
		}
	}
}

void CMsgTipDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);

	SIZE szRoundCorner = {4,4};
	if (!IsIconic()) 
	{
		if (szRoundCorner.cx != 0 || szRoundCorner.cy != 0)
		{
			RECT rcWindow;
			::GetWindowRect(m_hWnd, &rcWindow);
			::OffsetRect(&rcWindow, -rcWindow.left, -rcWindow.top);
			rcWindow.right++;
			rcWindow.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn(rcWindow.left, 
				rcWindow.top, rcWindow.right, rcWindow.bottom, 
				szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn(m_hWnd, hRgn, TRUE);
			::DeleteObject(hRgn);
		}
	}

	SetCtrlsAutoSize();		// 自动调整控件大小
}

void CMsgTipDlg::OnClose()
{
	DestroyWindow();
}

void CMsgTipDlg::OnDestroy()
{
	SetMsgHandled(FALSE);

	UnInitCtrls();			// 反初始化控件

	if (m_dwTimerId != NULL)
	{
		KillTimer(m_dwTimerId);
		m_dwTimerId = NULL;
	}

	m_imgBg.Destroy();
}

LRESULT CMsgTipDlg::OnList_Click(LPNMHDR pnmh)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW *)pnmh;
	if (pNMListView->iItem != -1)
	{
		CString strType;
		m_ListCtrl.GetItemText(pNMListView->iItem, 3, strType);
		UINT nType = _tcstol(strType, NULL, 10);
		UINT nSenderId = m_ListCtrl.GetItemData(pNMListView->iItem);

		switch (nType)
		{
		case QQ_MSG_TYPE_BUDDY:
			::PostMessage(m_hMainDlg, WM_SHOW_BUDDYCHATDLG, 0, nSenderId);
			break;
		case QQ_MSG_TYPE_GROUP:
			::PostMessage(m_hMainDlg, WM_SHOW_GROUPCHATDLG, nSenderId, 0);
			break;
		case QQ_MSG_TYPE_SESS:
			{
				if (m_lpQQClient != NULL)
				{
					CMessageList * lpMsgList = m_lpQQClient->GetMessageList();
					if (lpMsgList != NULL)
					{
						CMessageSender * lpMsgSender = lpMsgList->GetMsgSender(QQ_MSG_TYPE_SESS, nSenderId);
						if (lpMsgSender != NULL)
							::PostMessage(m_hMainDlg, WM_SHOW_SESSCHATDLG, lpMsgSender->GetGroupCode(), nSenderId);
					}
				}	
			}
			break;
		case QQ_MSG_TYPE_SYSGROUP:
			::PostMessage(m_hMainDlg, WM_SHOW_SYSGROUPCHATDLG, nSenderId, 0);
			break;
		}
	}
	return 0;
}

// “取消闪烁”超链接控件
void CMsgTipDlg::OnLnk_CancelFlash(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	::PostMessage(m_hMainDlg, WM_CANCEL_FLASH, 0, 0);
	SendMessage(WM_CLOSE);
}

// “显示全部”超链接控件
void CMsgTipDlg::OnLnk_ShowAll(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCount = m_ListCtrl.GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		CString strType;
		m_ListCtrl.GetItemText(i, 3, strType);
		UINT nType = _tcstol(strType, NULL, 10);
		UINT nSenderId = m_ListCtrl.GetItemData(i);

		switch (nType)
		{
		case QQ_MSG_TYPE_BUDDY:
			::PostMessage(m_hMainDlg, WM_SHOW_BUDDYCHATDLG, 0, nSenderId);
			break;
		case QQ_MSG_TYPE_GROUP:
			::PostMessage(m_hMainDlg, WM_SHOW_GROUPCHATDLG, nSenderId, 0);
			break;
		case QQ_MSG_TYPE_SESS:
			{
				if (m_lpQQClient != NULL)
				{
					CMessageList * lpMsgList = m_lpQQClient->GetMessageList();
					if (lpMsgList != NULL)
					{
						CMessageSender * lpMsgSender = lpMsgList->GetMsgSender(QQ_MSG_TYPE_SESS, nSenderId);
						if (lpMsgSender != NULL)
							::PostMessage(m_hMainDlg, WM_SHOW_SESSCHATDLG, lpMsgSender->GetGroupCode(), nSenderId);
					}
				}	
			}
			break;
		case QQ_MSG_TYPE_SYSGROUP:
			::PostMessage(m_hMainDlg, WM_SHOW_SYSGROUPCHATDLG, nSenderId, 0);
			break;
		}
	}
}

BOOL CMsgTipDlg::StartTrackMouseLeave()
{
	m_dwTimerId = SetTimer(990, 160, NULL);
	return m_dwTimerId != NULL ? TRUE : FALSE;
}

int CMsgTipDlg::FindMsgSender(QQ_MSG_TYPE nType, UINT nSenderId)
{
	CString strType;
	UINT nType2, nSenderId2;

	int nCount = m_ListCtrl.GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		m_ListCtrl.GetItemText(i, 3, strType);
		nType2 = _tcstol(strType, NULL, 10);
		nSenderId2 = m_ListCtrl.GetItemData(i);

		if (nType == nType2 && nSenderId == nSenderId2)
			return i;
	}
	return -1;
}

void CMsgTipDlg::AddMsgSender(QQ_MSG_TYPE nType, UINT nSenderId)
{
	CMessageSender * lpMsgSender = NULL;
	if (m_lpQQClient != NULL)
	{
		CMessageList * lpMsgList = m_lpQQClient->GetMessageList();
		if (lpMsgList != NULL)
			lpMsgSender = lpMsgList->GetMsgSender(nType, nSenderId);
	}
	if (NULL == lpMsgSender)
		return;

	int nItemIndex = FindMsgSender(nType, nSenderId);
	if (nItemIndex != -1)
	{
		int nMsgCnt = lpMsgSender->GetMsgCount();
		CString strMsgCnt;
		strMsgCnt.Format(_T("(%d)"), nMsgCnt);
		m_ListCtrl.SetItemText(nItemIndex, 2, strMsgCnt);
	}
	else
	{
		SetDlgAutoSize();	// 自动调整对话框大小
		_AddMsgSender(0, lpMsgSender);
	}
}

void CMsgTipDlg::DelMsgSender(QQ_MSG_TYPE nType, UINT nSenderId)
{
	SetDlgAutoSize();	// 自动调整对话框大小
	int nItemIndex = FindMsgSender(nType, nSenderId);
	if (nItemIndex != -1)
		m_ListCtrl.DeleteItem(nItemIndex);
}

// 初始化控件
BOOL CMsgTipDlg::InitCtrls()
{
	DWORD dwStyle = m_lnkCancelFlash.GetHyperLinkExtendedStyle();
	dwStyle = dwStyle | HLINK_COMMANDBUTTON | HLINK_NOTUNDERLINED;
	m_lnkCancelFlash.SetHyperLinkExtendedStyle(dwStyle);

	dwStyle = m_lnkShowAll.GetHyperLinkExtendedStyle();
	dwStyle = dwStyle | HLINK_COMMANDBUTTON | HLINK_NOTUNDERLINED;
	m_lnkShowAll.SetHyperLinkExtendedStyle(dwStyle);

	m_ListCtrl = GetDlgItem(IDC_LIST1);
	m_lnkCancelFlash.SubclassWindow(GetDlgItem(ID_LINK_CANCEL_FLASH));
	m_lnkShowAll.SubclassWindow(GetDlgItem(ID_LINK_SHOW_ALL));

	m_lnkCancelFlash.m_clrLink = RGB(0,114,193);
	m_lnkShowAll.m_clrLink = RGB(0,114,193);

	dwStyle = m_ListCtrl.GetStyle();
	dwStyle |= LVS_REPORT | LVS_SINGLESEL;
	m_ListCtrl.SetWindowLong(GWL_STYLE, dwStyle);

	DWORD dwExStyle = m_ListCtrl.GetExtendedListViewStyle();
	dwExStyle = dwExStyle | LVS_EX_FULLROWSELECT;
	m_ListCtrl.SetExtendedListViewStyle(dwExStyle);

	m_ListCtrl.InsertColumn(0, _T("图标"), LVCFMT_LEFT | LVCFMT_IMAGE, 20);
	m_ListCtrl.InsertColumn(1, _T("发送者"), LVCFMT_LEFT, 146);
	m_ListCtrl.InsertColumn(2, _T("消息数"), LVCFMT_RIGHT, 38);
	m_ListCtrl.InsertColumn(3, _T("Type"), LVCFMT_RIGHT, 0);

	m_headImageList.Init(16, 16, m_lpQQClient);
	m_ListCtrl.SetImageList(m_headImageList.GetImageList(), LVSIL_SMALL);

	m_ListCtrl.DeleteAllItems();
	if (m_lpQQClient != NULL)
	{
		CMessageList * lpMsgList = m_lpQQClient->GetMessageList();
		if (lpMsgList != NULL)
		{
			int nMsgSenderCnt = lpMsgList->GetMsgSenderCount();
			for (int i = 0; i < nMsgSenderCnt; i++)
			{
				CMessageSender * lpMsgSender = lpMsgList->GetMsgSender(i);
				if (lpMsgSender != NULL)
				{
					_AddMsgSender(0, lpMsgSender);
				}
			}
		}
	}

	return TRUE;
}

// 反初始化控件
BOOL CMsgTipDlg::UnInitCtrls()
{
	m_headImageList.UnInit();

	m_ListCtrl.DeleteAllItems();
	if (m_ListCtrl.IsWindow())
		m_ListCtrl.DestroyWindow();

	if (m_lnkCancelFlash.IsWindow())
	{
		m_lnkCancelFlash.DestroyWindow();
		m_lnkCancelFlash.m_tip.m_hWnd = NULL;
		m_lnkCancelFlash.m_hFontNormal = NULL;
	}

	if (m_lnkShowAll.IsWindow())
	{
		m_lnkShowAll.DestroyWindow();
		m_lnkShowAll.m_tip.m_hWnd = NULL;
		m_lnkShowAll.m_hFontNormal = NULL;
	}

	return TRUE;
}

// 自动调整对话框大小
void CMsgTipDlg::SetDlgAutoSize()
{
	int nMsgSenderCnt = 0;
	if (m_lpQQClient != NULL)
	{
		CMessageList * lpMsgList = m_lpQQClient->GetMessageList();
		if (lpMsgList != NULL)
			nMsgSenderCnt = lpMsgList->GetMsgSenderCount();
	}

	int cyListCtrl = nMsgSenderCnt * m_nListItemHeight + 5;

	int cxDlg = 212;
	int cyDlg = cyListCtrl + 36 + 22;

	int cxScreen = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int cyScreen = ::GetSystemMetrics(SM_CYFULLSCREEN);

	CRect rcTrayWnd;
	GetTrayWndRect(&rcTrayWnd);

	int nLeft, nTop;

	if (rcTrayWnd.top == rcTrayWnd.left && rcTrayWnd.bottom > rcTrayWnd.right)	// 左边
	{
		nLeft = rcTrayWnd.right + 2;
		nTop = m_rcTrayIcon.top - (cyDlg - m_rcTrayIcon.Width()) / 2;
		if (nTop + cyDlg > cyScreen)
			nTop = cyScreen - cyDlg;
	}
	else if (rcTrayWnd.top == rcTrayWnd.left && rcTrayWnd.bottom < rcTrayWnd.right)	// 上边
	{
		nLeft = m_rcTrayIcon.left - (cxDlg - m_rcTrayIcon.Width()) / 2;
		nTop = rcTrayWnd.bottom + 2;
		if (nLeft + cxDlg > cxScreen)
			nLeft = cxScreen - cxDlg;
	}
	else if (rcTrayWnd.top > rcTrayWnd.left)	// 下边
	{
		nLeft = m_rcTrayIcon.left - (cxDlg - m_rcTrayIcon.Width()) / 2;
		nTop = rcTrayWnd.top - cyDlg - 2;
		if (nLeft + cxDlg > cxScreen)
			nLeft = cxScreen - cxDlg;
	}
	else	// 右边
	{
		nLeft = rcTrayWnd.left - cxDlg - 2;
		nTop = m_rcTrayIcon.top - (cyDlg - m_rcTrayIcon.Width()) / 2;
		if (nTop + cyDlg > cyScreen)
			nTop = cyScreen - cyDlg;
	}

	SetWindowPos(HWND_TOPMOST, nLeft, nTop, cxDlg, cyDlg, SWP_NOACTIVATE);
}

// 自动调整控件大小
void CMsgTipDlg::SetCtrlsAutoSize()
{
	CRect rcClient;
	GetClientRect(&rcClient);

	int nMsgSenderCnt = 0;
	if (m_lpQQClient != NULL)
	{
		CMessageList * lpMsgList = m_lpQQClient->GetMessageList();
		if (lpMsgList != NULL)
			nMsgSenderCnt = lpMsgList->GetMsgSenderCount();
	}

	int cxListCtrl = 204;
	int cyListCtrl = nMsgSenderCnt * m_nListItemHeight + 5;

	m_ListCtrl.MoveWindow(4, 36, cxListCtrl, cyListCtrl);
	m_lnkCancelFlash.MoveWindow(12, rcClient.bottom - 22, 48, 12);
	m_lnkShowAll.MoveWindow(156, rcClient.bottom - 22, 48, 12);
}

void CMsgTipDlg::_AddMsgSender(int nIndex, CMessageSender * lpMsgSender)
{
	if (NULL == lpMsgSender || NULL == m_lpQQClient)
		return;

	CString strSenderName, strMsgCnt, strMsgType;

	QQ_MSG_TYPE nMsgType = lpMsgSender->GetMsgType();
	UINT nMsgCnt = lpMsgSender->GetMsgCount();
	UINT nSenderId = lpMsgSender->GetSenderId();
	UINT nGroupCode = lpMsgSender->GetGroupCode();

	int nImage = 0;
	if (QQ_MSG_TYPE_BUDDY == nMsgType)
		nImage = m_headImageList.GetHeadPicIndex(nMsgType, 0, nSenderId, FALSE, FALSE);
	else if (QQ_MSG_TYPE_GROUP == nMsgType)
		nImage = m_headImageList.GetHeadPicIndex(nMsgType, nSenderId, 0, FALSE, FALSE);
	else if (QQ_MSG_TYPE_SESS == nMsgType)
		nImage = m_headImageList.GetHeadPicIndex(nMsgType, nGroupCode, nSenderId, FALSE, FALSE);
	else if (QQ_MSG_TYPE_SYSGROUP == nMsgType)
		nImage = m_headImageList.GetHeadPicIndex(nMsgType, 0, 0, FALSE, FALSE);

	switch (nMsgType)
	{
	case QQ_MSG_TYPE_BUDDY:
		{
			CBuddyList * lpBuddyList = m_lpQQClient->GetBuddyList();
			if (lpBuddyList != NULL)
			{
				CBuddyInfo * lpBuddyInfo = lpBuddyList->GetBuddy(nSenderId);
				if (lpBuddyInfo != NULL)
				{
					CString strBuddyName;

					if (!lpBuddyInfo->m_strMarkName.empty())
						strBuddyName = lpBuddyInfo->m_strMarkName.c_str();
					else
						strBuddyName = lpBuddyInfo->m_strNickName.c_str();

					if (lpBuddyInfo->m_nQQNum != 0)
						strSenderName.Format(_T("%s(%u)"), strBuddyName, lpBuddyInfo->m_nQQNum);
					else
						strSenderName.Format(_T("%s"), strBuddyName);
				}
			}
		}
		break;
	case QQ_MSG_TYPE_GROUP:
		{
			CGroupList * lpGroupList = m_lpQQClient->GetGroupList();
			if (lpGroupList != NULL)
			{
				CGroupInfo * lpGroupInfo = lpGroupList->GetGroupByCode(nSenderId);
				if (lpGroupInfo != NULL)
				{
					if (lpGroupInfo->m_nGroupNumber != 0)
						strSenderName.Format(_T("%s(%u)"), lpGroupInfo->m_strName.c_str(), lpGroupInfo->m_nGroupNumber);
					else
						strSenderName.Format(_T("%s"), lpGroupInfo->m_strName.c_str());
				}
			}
		}
		break;
	case QQ_MSG_TYPE_SESS:
		{
			CGroupList * lpGroupList = m_lpQQClient->GetGroupList();
			if (lpGroupList != NULL)
			{
				CBuddyInfo * lpBuddyInfo = lpGroupList->GetGroupMember(nGroupCode, nSenderId);
				if (lpBuddyInfo != NULL)
				{
					CString strBuddyName;

					if (!lpBuddyInfo->m_strGroupCard.empty())
						strBuddyName = lpBuddyInfo->m_strGroupCard.c_str();
					else
						strBuddyName = lpBuddyInfo->m_strNickName.c_str();

					if (lpBuddyInfo->m_nQQNum != 0)
						strSenderName.Format(_T("%s(%u)"), strBuddyName, lpBuddyInfo->m_nQQNum);
					else
						strSenderName.Format(_T("%s"), strBuddyName);
				}
			}
		}
		break;
	case QQ_MSG_TYPE_SYSGROUP:
		{
			strSenderName = _T("群系统消息");
		}
		break;
	}
	strMsgCnt.Format(_T("(%d)"), nMsgCnt);
	strMsgType.Format(_T("%d"), nMsgType);

	m_ListCtrl.InsertItem(nIndex, _T(""), nImage);
	m_ListCtrl.SetItemText(nIndex, 1, strSenderName);
	m_ListCtrl.SetItemText(nIndex, 2, strMsgCnt);
	m_ListCtrl.SetItemText(nIndex, 3, strMsgType);
	m_ListCtrl.SetItemData(nIndex, nSenderId);
}