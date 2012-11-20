#include "StdAfx.h"
#include "Utils.h"

// 枚举系统字体回调函数
int CALLBACK EnumSysFontProc(const LOGFONT *lpelf, const TEXTMETRIC *lpntm, DWORD dwFontType, LPARAM lParam)
{
	if (dwFontType & TRUETYPE_FONTTYPE)
	{
		std::vector<tstring> * arrSysFont = (std::vector<tstring> *)lParam;
		if (arrSysFont != NULL)
		{
			for (int i = 0; i < (int)arrSysFont->size(); i++)
			{
				if ((*arrSysFont)[i] == lpelf->lfFaceName)
					return TRUE;
			}
			arrSysFont->push_back(lpelf->lfFaceName);
		}
	}

	return TRUE;
}

// 枚举系统字体
BOOL EnumSysFont(std::vector<tstring> * arrSysFont)
{
	if (NULL == arrSysFont)
		return FALSE;

	HDC hDC = ::GetDC(NULL);
	::EnumFontFamiliesEx(hDC, NULL, EnumSysFontProc, (LPARAM)arrSysFont, 0);
	::ReleaseDC(NULL, hDC);

	return TRUE;
}

// 闪烁窗口标题栏
BOOL FlashWindowEx(HWND hWnd, int nCount)
{
	FLASHWINFO stFlashInfo = {0};
	stFlashInfo.cbSize = sizeof(FLASHWINFO);
	stFlashInfo.hwnd = hWnd;
	stFlashInfo.dwFlags = FLASHW_TRAY | FLASHW_TIMERNOFG;
	stFlashInfo.uCount = nCount;
	stFlashInfo.dwTimeout = 0;
	return ::FlashWindowEx(&stFlashInfo);
}

// 获取系统任务栏区域
BOOL GetTrayWndRect(RECT * lpRect)
{
	if (NULL == lpRect)
		return FALSE;

	HWND hWnd = ::FindWindow(_T("Shell_TrayWnd"), NULL);
	if (hWnd != NULL)
		return ::GetWindowRect(hWnd, lpRect);
	else
		return FALSE;
}

// 设置RichEdit默认字体
void RichEdit_SetDefFont(CRichEditCtrl& richEdit, LPCTSTR lpFontName, int nFontSize,
						 COLORREF clrText, BOOL bBold, BOOL bItalic, BOOL bUnderLine)
{
	CHARFORMAT2 cf;

	memset(&cf, 0, sizeof(cf));

	richEdit.GetDefaultCharFormat(cf);

	cf.cbSize = sizeof(CHARFORMAT);

	if (lpFontName != NULL)					// 设置字体名称
	{
		cf.dwMask = cf.dwMask | CFM_FACE;
		_tcscpy(cf.szFaceName, lpFontName);
	}

	if (nFontSize > 0)						// 设置字体高度
	{
		cf.dwMask = cf.dwMask | CFM_SIZE;
		cf.yHeight = nFontSize * 20;
	}

	cf.dwMask |= CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;

	cf.crTextColor = clrText;				// 设置字体颜色
	cf.dwEffects &= ~CFE_AUTOCOLOR;

	if (bBold)								// 设置粗体
		cf.dwEffects |= CFE_BOLD;
	else
		cf.dwEffects &= ~CFE_BOLD;

	if (bItalic)							// 设置倾斜
		cf.dwEffects |= CFE_ITALIC;
	else
		cf.dwEffects &= ~CFE_ITALIC;

	if (bUnderLine)							// 设置下划线
		cf.dwEffects |= CFE_UNDERLINE;
	else
		cf.dwEffects &= ~CFE_UNDERLINE;

	richEdit.SetDefaultCharFormat(cf);
}

// 设置RichEdit默认字体名称和大小
void RichEdit_SetDefFont(CRichEditCtrl& richEdit, LPCTSTR lpFontName, int nFontSize)
{
	CHARFORMAT cf = {0};

	richEdit.GetDefaultCharFormat(cf);

	cf.cbSize = sizeof(CHARFORMAT);

	if (lpFontName != NULL)					// 设置字体名称
	{
		cf.dwMask = cf.dwMask | CFM_FACE;
		_tcscpy(cf.szFaceName, lpFontName);
	}

	if (nFontSize > 0)						// 设置字体高度
	{
		cf.dwMask = cf.dwMask | CFM_SIZE;
		cf.yHeight = nFontSize * 20;
	}

	richEdit.SetDefaultCharFormat(cf);
}

// 设置RichEdit默认字体颜色
void RichEdit_SetDefTextColor(CRichEditCtrl& richEdit, COLORREF clrText)
{
	CHARFORMAT cf = {0};

	richEdit.GetDefaultCharFormat(cf);

	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = cf.dwMask | CFM_COLOR;
	cf.crTextColor = clrText;
	cf.dwEffects &= ~CFE_AUTOCOLOR;

	richEdit.SetDefaultCharFormat(cf);
}

// 设置RichEdit字体
void RichEdit_SetFont(CRichEditCtrl& richEdit, LPCTSTR lpFontName, int nFontSize,
					  COLORREF clrText, BOOL bBold, BOOL bItalic, BOOL bUnderLine)
{
	CHARFORMAT2 cf;

	memset(&cf, 0, sizeof(cf));

	richEdit.SetSel(-1, -1);

	richEdit.GetSelectionCharFormat(cf);

	cf.cbSize = sizeof(CHARFORMAT);

	if (lpFontName != NULL)					// 设置字体名称
	{
		cf.dwMask = cf.dwMask | CFM_FACE;
		_tcscpy(cf.szFaceName, lpFontName);
	}

	if (nFontSize > 0)						// 设置字体高度
	{
		cf.dwMask = cf.dwMask | CFM_SIZE;
		cf.yHeight = nFontSize * 20;
	}

	cf.dwMask |= CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;

	cf.crTextColor = clrText;				// 设置字体颜色
	cf.dwEffects &= ~CFE_AUTOCOLOR;

	if (bBold)								// 设置粗体
		cf.dwEffects |= CFE_BOLD;
	else
		cf.dwEffects &= ~CFE_BOLD;

	if (bItalic)							// 设置倾斜
		cf.dwEffects |= CFE_ITALIC;
	else
		cf.dwEffects &= ~CFE_ITALIC;

	if (bUnderLine)							// 设置下划线
		cf.dwEffects |= CFE_UNDERLINE;
	else
		cf.dwEffects &= ~CFE_UNDERLINE;

	richEdit.SetSelectionCharFormat(cf);
}

// 设置RichEdit字体名称和大小
void RichEdit_SetFont(CRichEditCtrl& richEdit, LPCTSTR lpFontName, int nFontSize)
{
	CHARFORMAT cf = {0};

	richEdit.SetSel(-1, -1);

	richEdit.GetSelectionCharFormat(cf);

	cf.cbSize = sizeof(CHARFORMAT);

	if (lpFontName != NULL)					// 设置字体名称
	{
		cf.dwMask = cf.dwMask | CFM_FACE;
		_tcscpy(cf.szFaceName, lpFontName);
	}

	if (nFontSize > 0)						// 设置字体高度
	{
		cf.dwMask = cf.dwMask | CFM_SIZE;
		cf.yHeight = nFontSize * 20;
	}

	richEdit.SetSelectionCharFormat(cf);
}

// 设置RichEdit字体颜色
void RichEdit_SetTextColor(CRichEditCtrl& richEdit, COLORREF clrText)
{
	CHARFORMAT cf = {0};

	richEdit.SetSel(-1, -1);

	richEdit.GetSelectionCharFormat(cf);

	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = cf.dwMask | CFM_COLOR;
	cf.crTextColor = clrText;
	cf.dwEffects &= ~CFE_AUTOCOLOR;

	richEdit.SetSelectionCharFormat(cf);
}

// RichEdit添加文本
void RichEdit_AppendText(CRichEditCtrl& richEdit, LPCTSTR lpText)
{
	richEdit.SetSel(-1, -1);
	richEdit.ReplaceSel(lpText);
	richEdit.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

// RichEdit设置左缩进(单位:缇)
void RichEdit_SetStartIndent(CRichEditCtrl& richEdit, int nSize)
{
	PARAFORMAT2 pf2;
	memset(&pf2, 0, sizeof(PARAFORMAT2));
	pf2.cbSize = sizeof(PARAFORMAT2);
	pf2.dwMask = PFM_STARTINDENT;
	pf2.dxStartIndent = nSize;
	richEdit.SetParaFormat(pf2);
}

// 插入表情图片
BOOL RichEdit_InsertFace(CRichEditCtrl& richEdit, LPCTSTR lpszFileName, int nFaceId)
{
	IRichEditOle * pRichEditOle = richEdit.GetOleInterface();
	if (NULL == pRichEditOle)
		return FALSE;

	IOleClientSite *pOleClientSite = NULL;
	pRichEditOle->GetClientSite(&pOleClientSite);
	if (NULL == pOleClientSite)
	{
		pRichEditOle->Release();
		return FALSE;
	}

	SCODE sc;
	LPLOCKBYTES lpLockBytes = NULL;
	sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
	if (sc != S_OK)
	{
		pOleClientSite->Release();
		pRichEditOle->Release();
		return FALSE;
	}

	IStorage *pStorage = NULL;
	sc = ::StgCreateDocfileOnILockBytes(lpLockBytes,
		STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, &pStorage);
	if (sc != S_OK)
	{
		pOleClientSite->Release();
		lpLockBytes->Release();
		pRichEditOle->Release();
		return FALSE;
	}

	CImageOle *pImageOle = new CImageOle;
	if (NULL == pImageOle)
	{
		pOleClientSite->Release();
		lpLockBytes->Release();
		pStorage->Release();
		pRichEditOle->Release();
		return FALSE;
	}

	pImageOle->SetOwnerHwnd(richEdit.m_hWnd);
	pImageOle->LoadFromFile(lpszFileName);
	pImageOle->SetFaceId(nFaceId);

	IOleObject *pOleObject = NULL;
	pImageOle->QueryInterface(IID_IOleObject, (void **)&pOleObject);
	if (NULL == pOleObject)
	{
		pOleClientSite->Release();
		lpLockBytes->Release();
		pStorage->Release();
		pRichEditOle->Release();
		delete pImageOle;
		return FALSE;
	}

	HRESULT hr = ::OleSetContainedObject(pOleObject, TRUE);

	REOBJECT reobject = {0};
	reobject.cbStruct = sizeof(REOBJECT);
	reobject.clsid    = CLSID_NULL;
	reobject.cp       = REO_CP_SELECTION;
	reobject.dvaspect = DVASPECT_CONTENT;
	reobject.poleobj  = pOleObject;
	reobject.polesite = pOleClientSite;
	reobject.pstg     = pStorage;
	reobject.dwUser   = 0;

	pRichEditOle->InsertObject(&reobject);

	pOleObject->Release();
	pOleClientSite->Release();
	pStorage->Release();
	pRichEditOle->Release();

	richEdit.Invalidate();

	return TRUE;
}

// 获取文本
void RichEdit_GetText(CRichEditCtrl& richEdit, CString& strText)
{
	std::vector<CString> arrItem;
	CImageOle * pImageOle;
	int nFaceId, nPos = 0;
	CString strTemp;

	IRichEditOle * pRichEditOle = richEdit.GetOleInterface();
	if (NULL == pRichEditOle)
		return;

	richEdit.GetWindowText(strText);

	int nCount = pRichEditOle->GetObjectCount();
	for (int i = 0; i < nCount; i++)
	{
		REOBJECT reobject;
		memset(&reobject, 0, sizeof(REOBJECT));
		reobject.cbStruct = sizeof(REOBJECT);
		HRESULT hr = pRichEditOle->GetObject(i, &reobject, REO_GETOBJ_ALL_INTERFACES);
		if (hr == S_OK)
		{
			if (reobject.cp > 0 && reobject.cp > nPos)
			{
				strTemp = strText.Mid(nPos, reobject.cp-nPos);
				strTemp.Replace(_T("/"), _T("//"));
				arrItem.push_back(strTemp);
			}
			nPos = reobject.cp+1;
			pImageOle = (CImageOle *)reobject.poleobj;
			if (pImageOle != NULL)
			{
				nFaceId = pImageOle->GetFaceId();
				strTemp.Format(_T("/f[%03d]"), nFaceId);
				arrItem.push_back(strTemp);
				pImageOle->Release();
			}

			if (reobject.polesite)
				reobject.polesite->Release();

			if (reobject.pstg)
				reobject.pstg->Release();
		}
	}

	if (nPos < strText.GetLength())
	{
		strTemp = strText.Mid(nPos);
		strTemp.Replace(_T("/"), _T("//"));
		arrItem.push_back(strTemp);
	}

	strText = _T("");
	for (int i = 0; i < (int)arrItem.size(); i++)
	{
		strText += arrItem[i];
	}
	arrItem.clear();

	pRichEditOle->Release();
}
