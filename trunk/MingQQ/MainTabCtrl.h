#pragma once

#include "resource.h"

class CMainTabCtrl : public CWindowImpl<CMainTabCtrl, CTabCtrl>
{
public:
	CMainTabCtrl(void);
	~CMainTabCtrl(void);

	BEGIN_MSG_MAP_EX(CMainTabCtrl)
		MSG_WM_SIZE(OnSize)
		MSG_WM_DESTROY(OnDestroy)
		NOTIFY_HANDLER_EX(ID_TREE_BUDDY, NM_DBLCLK, OnBuddyTreeDblClk)
		NOTIFY_HANDLER_EX(ID_TREE_GROUP, NM_DBLCLK, OnGroupTreeDblClk)
		NOTIFY_HANDLER_EX(ID_TREE_RECENT, NM_DBLCLK, OnRecentTreeDblClk)
	END_MSG_MAP()

public:
	void InitTabCtrl(CRect& rcTabCtrl, HIMAGELIST hImageList);
	void OnSelChange(int nCurSel);

private:
	void OnSize(UINT nType, CSize size);
	void OnDestroy();
	LRESULT OnBuddyTreeDblClk(LPNMHDR pnmh);
	LRESULT OnGroupTreeDblClk(LPNMHDR pnmh);
	LRESULT OnRecentTreeDblClk(LPNMHDR pnmh);

public:
	CTreeViewCtrl m_treeBuddy;
	CTreeViewCtrl m_treeGroup;
	CTreeViewCtrl m_treeRecent;
};
