#include "StdAfx.h"
#include "MainTabCtrl.h"

CMainTabCtrl::CMainTabCtrl(void)
{
	
}

CMainTabCtrl::~CMainTabCtrl(void)
{
}

void CMainTabCtrl::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);
}

void CMainTabCtrl::OnDestroy()
{
	SetMsgHandled(FALSE);

	if (m_treeBuddy.IsWindow())
		m_treeBuddy.DestroyWindow();

	if (m_treeGroup.IsWindow())
		m_treeGroup.DestroyWindow();

	if (m_treeRecent.IsWindow())
		m_treeRecent.DestroyWindow();
}

LRESULT CMainTabCtrl::OnBuddyTreeDblClk(LPNMHDR pnmh)
{
	const _ATL_MSG* pMsg = GetCurrentMessage();
	if (pMsg != NULL)
	{
		HWND hParentWnd = GetParent();
		return ::SendMessage(hParentWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
	}
	return 0;
}

LRESULT CMainTabCtrl::OnGroupTreeDblClk(LPNMHDR pnmh)
{
	const _ATL_MSG* pMsg = GetCurrentMessage();
	if (pMsg != NULL)
	{
		HWND hParentWnd = GetParent();
		return ::SendMessage(hParentWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
	}
	return 0;
}

LRESULT CMainTabCtrl::OnRecentTreeDblClk(LPNMHDR pnmh)
{
	const _ATL_MSG* pMsg = GetCurrentMessage();
	if (pMsg != NULL)
	{
		HWND hParentWnd = GetParent();
		return ::SendMessage(hParentWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
	}
	return 0;
}

void CMainTabCtrl::InitTabCtrl(CRect& rcTabCtrl, HIMAGELIST hImageList)
{
	MoveWindow(rcTabCtrl);

	InsertItem(0, _T("QQ好友"));
	InsertItem(1, _T("QQ群"));
	InsertItem(2, _T("最近联系人"));

	CRect rcClient, rcItem, rcTree;

	GetClientRect(&rcClient);
	GetItemRect(0, &rcItem);

	rcTree.left = rcItem.left;
	rcTree.top = rcItem.bottom + 2;
	rcTree.right = rcTree.left + (rcClient.right- rcTree.left - 4);
	rcTree.bottom = rcTree.top + (rcClient.bottom - rcTree.top - 4);

	DWORD dwStyle = WS_CHILD|TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT;
	m_treeBuddy.Create(m_hWnd, rcTree, NULL, dwStyle|WS_VISIBLE, 0, ID_TREE_BUDDY);
	m_treeGroup.Create(m_hWnd, rcTree, NULL, dwStyle, 0, ID_TREE_GROUP);
	m_treeRecent.Create(m_hWnd, rcTree, NULL, dwStyle, 0, ID_TREE_RECENT);

	m_treeBuddy.SetImageList(hImageList, TVSIL_NORMAL);
	m_treeGroup.SetImageList(hImageList, TVSIL_NORMAL);
	m_treeRecent.SetImageList(hImageList, TVSIL_NORMAL);

	SetCurSel(0);
}

void CMainTabCtrl::OnSelChange(int nCurSel)
{
	switch (nCurSel)
	{
	case 0:
		{
			m_treeBuddy.ShowWindow(SW_SHOW);
			m_treeGroup.ShowWindow(SW_HIDE);
			m_treeRecent.ShowWindow(SW_HIDE);
		}
		break;
	case 1:
		{
			m_treeBuddy.ShowWindow(SW_HIDE);
			m_treeGroup.ShowWindow(SW_SHOW);
			m_treeRecent.ShowWindow(SW_HIDE);
		}
		break;
	case 2:
		{
			m_treeBuddy.ShowWindow(SW_HIDE);
			m_treeGroup.ShowWindow(SW_HIDE);
			m_treeRecent.ShowWindow(SW_SHOW);
		}
		break;
	}
}