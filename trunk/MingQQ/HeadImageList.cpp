#include "StdAfx.h"
#include "HeadImageList.h"

CHeadImageList::CHeadImageList(void)
{
	m_lpQQClient = NULL;
	m_szHead = CSize(0, 0);
}

CHeadImageList::~CHeadImageList(void)
{
	UnInit();
}

BOOL CHeadImageList::Init(int cx, int cy, CQQClient * lpQQClient)
{
	if (NULL == lpQQClient)
		return FALSE;

	m_szHead = CSize(cx, cy);
	m_lpQQClient = lpQQClient;
	
	BOOL bRet = m_ImageList.Create(m_szHead.cx, m_szHead.cy, TRUE | ILC_COLOR32, 10, 1);
	if (!bRet)
		return FALSE;

	HBITMAP hBmp0 = LoadHeadPic(ZYM::CPath::GetCurDir() + _T("Image\\DefBuddyHeadPic.png"), 
		FALSE, m_szHead.cx, m_szHead.cy);
	HBITMAP hBmp1 = LoadHeadPic(ZYM::CPath::GetCurDir() + _T("Image\\DefBuddyHeadPic.png"),
		TRUE, m_szHead.cx, m_szHead.cy);
	HBITMAP hBmp2 = LoadHeadPic(ZYM::CPath::GetCurDir() + _T("Image\\DefGroupHeadPic.jpg"),
		FALSE, m_szHead.cx, m_szHead.cy);
	HBITMAP hBmp3 = LoadHeadPic(ZYM::CPath::GetCurDir() + _T("Image\\DefSysHeadPic.png"),
		FALSE, m_szHead.cx, m_szHead.cy);

	m_ImageList.SetBkColor(RGB(255,255,255));
	m_ImageList.Add(hBmp0);
	m_ImageList.Add(hBmp1);
	m_ImageList.Add(hBmp2);
	m_ImageList.Add(hBmp3);

	::DeleteObject(hBmp0);
	::DeleteObject(hBmp1);
	::DeleteObject(hBmp2);
	::DeleteObject(hBmp3);

	return TRUE;
}

void CHeadImageList::UnInit()
{
	m_ImageList.Destroy();
	m_arrHeadPicIndex.clear();
}

HIMAGELIST CHeadImageList::GetImageList()
{
	return m_ImageList.m_hImageList;
}

int CHeadImageList::GetHeadPicIndex(QQ_MSG_TYPE nType, UINT nGroupCode, 
									UINT nQQUin, BOOL bGray, BOOL bForceReload)
{
	HEAD_PIC_INDEX stHeadPicIndex;

	if (QQ_MSG_TYPE_SYSGROUP == nType)		// 系统消息
		return 3;

	BOOL bIsBuddy = FALSE;
	if (nGroupCode != 0 && nQQUin != 0)		// 群成员
		bIsBuddy = TRUE;
	else if (nQQUin != 0)					// 好友
		bIsBuddy = TRUE;

	for (int i = 0; i < (int)m_arrHeadPicIndex.size(); i++)
	{
		stHeadPicIndex = m_arrHeadPicIndex[i];
		if (stHeadPicIndex.nGroupCode == nGroupCode && stHeadPicIndex.nQQUin == nQQUin)
		{
			if (bIsBuddy)
			{
				if (stHeadPicIndex.bGray != bGray || bForceReload)
				{
					stHeadPicIndex.bGray = bGray;
					HBITMAP hBmp = LoadHeadPic(nGroupCode, nQQUin, bGray, m_szHead.cx, m_szHead.cy);
					if (NULL == hBmp)
						return bGray ? 1 : 0;
					m_ImageList.Replace(stHeadPicIndex.nIndex, hBmp, NULL);
					::DeleteObject(hBmp);
				}
			}
			return stHeadPicIndex.nIndex;
		}
	}

	HBITMAP hBmp = LoadHeadPic(nGroupCode, nQQUin, bGray, m_szHead.cx, m_szHead.cy);
	if (NULL == hBmp)
	{
		if (bIsBuddy)
			return bGray ? 1 : 0;
		else
			return 2;
	}

	stHeadPicIndex.nGroupCode = nGroupCode;
	stHeadPicIndex.nQQUin = nQQUin;
	stHeadPicIndex.bGray = bGray;
	stHeadPicIndex.nIndex = m_ImageList.Add(hBmp);
	m_arrHeadPicIndex.push_back(stHeadPicIndex);
	::DeleteObject(hBmp);
	return stHeadPicIndex.nIndex;
}

HICON CHeadImageList::ExtractIcon(int nImage)
{
	return m_ImageList.ExtractIcon(nImage);
}

HBITMAP CHeadImageList::LoadHeadPic(LPCTSTR lpszFileName, BOOL bGray, int cx, int cy)
{
	HBITMAP hBmp = NULL;

	if (NULL == lpszFileName || NULL == *lpszFileName)
		return NULL;

	Gdiplus::Bitmap imgHead(lpszFileName);
	if (imgHead.GetLastStatus() != Gdiplus::Ok)
		return NULL;

	if (imgHead.GetWidth() != cx || imgHead.GetHeight() != cy)
	{
		Gdiplus::Bitmap* pThumbnail = (Gdiplus::Bitmap*)imgHead.GetThumbnailImage(cx, cy, NULL, NULL);
		if (pThumbnail != NULL)
		{
			pThumbnail->GetHBITMAP(Gdiplus::Color(255,255,255), &hBmp);
			delete pThumbnail;
		}
	}
	else
	{
		imgHead.GetHBITMAP(Gdiplus::Color(255,255,255), &hBmp);
	}

	if (NULL == hBmp)
		return NULL;

	if (bGray)
	{
		CImageEx imgHead2;
		imgHead2.Attach(hBmp);
		imgHead2.GrayScale();
		imgHead2.Detach();
	}

	return hBmp;
}

HBITMAP CHeadImageList::LoadHeadPic(UINT nGroupCode, UINT nQQUin, BOOL bGray, int cx, int cy)
{
	UINT nGroupNum, nQQNum;
	GetNumber(nGroupCode, nQQUin, nGroupNum, nQQNum);

	tstring strFullName = m_lpQQClient->GetHeadPicFullName(nGroupNum, nQQNum);

	return LoadHeadPic(strFullName.c_str(), bGray, cx, cy);
}

void CHeadImageList::GetNumber(UINT nGroupCode, UINT nQQUin, UINT& nGroupNum, UINT& nQQNum)
{
	nGroupNum = nQQNum = 0;

	if (nGroupCode != 0 && nQQUin != 0)
	{
		CGroupList * lpGroupList = m_lpQQClient->GetGroupList();
		if (lpGroupList != NULL)
		{
			CBuddyInfo * lpBuddyInfo = lpGroupList->GetGroupMember(nGroupCode, nQQUin);
			if (lpBuddyInfo != NULL)
				nQQNum = lpBuddyInfo->m_nQQNum;
		}
	}
	else if (nGroupCode != 0)
	{
		CGroupList * lpGroupList = m_lpQQClient->GetGroupList();
		if (lpGroupList != NULL)
		{
			CGroupInfo * lpGroupInfo = lpGroupList->GetGroupByCode(nGroupCode);
			if (lpGroupInfo != NULL)
				nGroupNum = lpGroupInfo->m_nGroupNumber;
		}
	}
	else if (nQQUin != 0)
	{
		CBuddyList * lpBuddyList = m_lpQQClient->GetBuddyList();
		if (lpBuddyList != NULL)
		{
			CBuddyInfo * lpBuddyInfo = lpBuddyList->GetBuddy(nQQUin);
			if (lpBuddyInfo != NULL)
				nQQNum = lpBuddyInfo->m_nQQNum;
		}
	}
}