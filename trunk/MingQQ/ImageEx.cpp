#include "StdAfx.h"
#include "ImageEx.h"

CImageEx::CImageEx(void)
{
	m_bIsNinePart = FALSE;
	::SetRectEmpty(&m_rcNinePart);
}

CImageEx::~CImageEx(void)
{
}

BOOL CImageEx::LoadFromFile(LPCTSTR pszFileName)
{
	HRESULT hr = CImage::Load(pszFileName);
	if (hr == S_OK)
	{
		AlphaPremultiplication();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CImageEx::LoadFromIStream(IStream* pStream)
{
	HRESULT hr = CImage::Load(pStream);
	if (hr == S_OK)
	{
		AlphaPremultiplication();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CImageEx::LoadFromBuffer(const BYTE* lpBuf, DWORD dwSize)
{
	HGLOBAL hGlobal;
	LPVOID lpBuffer;
	BOOL bRet = FALSE;
	
	hGlobal = ::GlobalAlloc(GHND, dwSize);
	if (NULL == hGlobal)
		return bRet;

	lpBuffer = ::GlobalLock(hGlobal);
	if (NULL == lpBuffer)
	{
		::GlobalFree(hGlobal);
		return bRet;
	}

	memcpy(lpBuffer, lpBuf, dwSize);
	::GlobalUnlock(hGlobal);

	LPSTREAM lpStream = NULL;
	HRESULT hr = ::CreateStreamOnHGlobal(hGlobal, TRUE, &lpStream);
	if (hr != S_OK)
	{
		::GlobalFree(hGlobal);
		return bRet;
	}
	
	bRet = LoadFromIStream(lpStream);
	lpStream->Release();

	return bRet;
}

BOOL CImageEx::LoadFromResource(HINSTANCE hInstance, LPCTSTR pszResourceName, LPCTSTR pszResType)
{
	HRSRC hRsrc = ::FindResource(hInstance, pszResourceName, pszResType);
	if (NULL == hRsrc)
		return FALSE;

	DWORD dwSize = ::SizeofResource(hInstance, hRsrc); 
	if (0 == dwSize)
		return FALSE;

	HGLOBAL hGlobal = ::LoadResource(hInstance, hRsrc); 
	if (NULL == hGlobal)
		return FALSE;

	LPVOID pBuffer = ::LockResource(hGlobal);
	if (NULL == pBuffer)
	{
		::FreeResource(hGlobal);
		return FALSE;
	}

	HGLOBAL hGlobal2 = ::GlobalAlloc(GHND, dwSize);
	if (NULL == hGlobal2)
	{
		::FreeResource(hGlobal);
		return FALSE;
	}

	LPVOID pBuffer2 = ::GlobalLock(hGlobal2);
	if (NULL == pBuffer2)
	{
		::GlobalFree(hGlobal2);
		::FreeResource(hGlobal);
		return FALSE;
	}

	memcpy(pBuffer2, pBuffer, dwSize);
	::GlobalUnlock(hGlobal2);

	LPSTREAM pStream = NULL;
	HRESULT hr = ::CreateStreamOnHGlobal(hGlobal2, TRUE, &pStream);
	if (hr != S_OK)
	{
		::GlobalFree(hGlobal2);
		::FreeResource(hGlobal);
		return FALSE;
	}

	BOOL bRet = LoadFromIStream(pStream);

	if (pStream)
		pStream->Release();

	::FreeResource(hGlobal);

	return bRet;
}

BOOL CImageEx::LoadFromResource(HINSTANCE hInstance, UINT nIDResource, LPCTSTR pszResType)
{
	return LoadFromResource(hInstance, MAKEINTRESOURCE(nIDResource), pszResType);
}

void CImageEx::SetNinePart(const RECT * lpNinePart)
{
	if ((NULL == lpNinePart) || (0 == lpNinePart->left && 0 == lpNinePart->top
		&& 0 == lpNinePart->right && 0 == lpNinePart->bottom))
	{
		m_bIsNinePart = FALSE;
		::SetRectEmpty(&m_rcNinePart);
	}
	else
	{
		m_bIsNinePart = TRUE;
		m_rcNinePart = *lpNinePart;
	}
}

BOOL CImageEx::Draw2(HDC hDestDC, const RECT& rectDest)
{
	if (m_bIsNinePart)
	{
		return DrawNinePartImage(hDestDC, rectDest.left, rectDest.top,
			rectDest.right - rectDest.left, rectDest.bottom - rectDest.top, 
			m_rcNinePart.left, m_rcNinePart.top, m_rcNinePart.right, m_rcNinePart.bottom);
	}
	else
	{
		return Draw(hDestDC, rectDest);
	}
}

// 图像灰度化
void CImageEx::GrayScale()
{
	int nWidth = GetWidth();
	int nHeight = GetHeight();

	BYTE* pArray = (BYTE*)GetBits();
	int nPitch = GetPitch();
	int nBitCount = GetBPP() / 8;

	for (int i = 0; i < nHeight; i++) 
	{
		for (int j = 0; j < nWidth; j++) 
		{
			int grayVal = (BYTE)(((*(pArray + nPitch * i + j * nBitCount) * 306)
				+ (*(pArray + nPitch * i + j * nBitCount + 1) * 601)
				+ (*(pArray + nPitch * i + j * nBitCount + 2) * 117) + 512 ) >> 10);	// 计算灰度值

			*(pArray + nPitch * i + j * nBitCount) = grayVal;							// 赋灰度值
			*(pArray + nPitch * i + j * nBitCount + 1) = grayVal;
			*(pArray + nPitch * i + j * nBitCount + 2) = grayVal;
		}
	}
}

// Alpha预乘
BOOL CImageEx::AlphaPremultiplication()
{
	LPVOID pBitsSrc = NULL;
	BYTE * psrc = NULL;
	BITMAP stBmpInfo;

	HBITMAP hBmp = (HBITMAP)*this;

	::GetObject(hBmp, sizeof(BITMAP), &stBmpInfo);

	// Only support 32bit DIB section
	if (32 != stBmpInfo.bmBitsPixel || NULL == stBmpInfo.bmBits)
		return FALSE;

	psrc = (BYTE *) stBmpInfo.bmBits;

	// Just mix it
	for (int nPosY = 0; nPosY < abs(stBmpInfo.bmHeight); nPosY++)
	{
		for (int nPosX = stBmpInfo.bmWidth; nPosX > 0; nPosX--)
		{
			BYTE alpha  = psrc[3];
			psrc[0] = (BYTE)((psrc[0] * alpha) / 255);
			psrc[1] = (BYTE)((psrc[1] * alpha) / 255);
			psrc[2] = (BYTE)((psrc[2] * alpha) / 255);
			psrc += 4;
		}
	}

	return TRUE;
}

BOOL CImageEx::DrawNinePartImage(int pleft, int ptop, int pright, int pbottom,
					   HDC hDC, int height, int width, int left, int top, int right, int bottom)
{
	// 左上
	{
		CRect rcDest(pleft, ptop, pleft+left, ptop+top);
		CRect rcSrc(0, 0, left, top);
		if (!rcDest.IsRectEmpty() && !rcSrc.IsRectEmpty())
			Draw(hDC, rcDest, rcSrc);
	}

	// 左边
	{
		CRect rcDest(pleft, top+ptop, pleft+left, top+(height-top-bottom-ptop-pbottom));
		CRect rcSrc(0, top, left, top+(GetHeight()-top-bottom));
		if (!rcDest.IsRectEmpty() && !rcSrc.IsRectEmpty())
			Draw(hDC, rcDest, rcSrc);
	}

	// 上边
	{
		CRect rcDest(left+pleft, ptop, (left+pleft)+(width-left-right-pleft-pright), ptop+top);
		CRect rcSrc(left, 0, left+(GetWidth()-left-right), top);
		if (!rcDest.IsRectEmpty() && !rcSrc.IsRectEmpty())
			Draw(hDC, rcDest, rcSrc);
	}

	// 右上
	{
		CRect rcDest(width- right-pright, ptop, (width- right-pright)+right, ptop+top);
		CRect rcSrc(GetWidth()-right, 0, (GetWidth()-right)+right, top);
		if (!rcDest.IsRectEmpty() && !rcSrc.IsRectEmpty())
			Draw(hDC, rcDest, rcSrc);
	}

	// 右边
	{
		CRect rcDest(width-right-pright, top+ptop, (width-right-pright)+right, (top+ptop)+(height-top-bottom-ptop-pbottom));
		CRect rcSrc(GetWidth()-right, top, (GetWidth()-right)+right, top+(GetHeight()-top-bottom));
		if (!rcDest.IsRectEmpty() && !rcSrc.IsRectEmpty())
			Draw(hDC, rcDest, rcSrc);
	}

	// 下边
	{
		CRect rcDest(left+pleft, height-bottom-pbottom, (left+pleft)+(width-left-right-pleft-pright), (height-bottom-pbottom)+bottom);
		CRect rcSrc(left, GetHeight()-bottom, left+(GetWidth()-left-right), (GetHeight()-bottom)+bottom);
		if (!rcDest.IsRectEmpty() && !rcSrc.IsRectEmpty())
			Draw(hDC, rcDest, rcSrc);
	}

	// 右下
	{
		CRect rcDest(width-right-pright, height-bottom-pbottom, (width-right-pright)+right, (height-bottom-pbottom)+bottom);
		CRect rcSrc(GetWidth()-right, GetHeight()-bottom, (GetWidth()-right)+right, (GetHeight()-bottom)+bottom);
		if (!rcDest.IsRectEmpty() && !rcSrc.IsRectEmpty())
			Draw(hDC, rcDest, rcSrc);
	}

	// 左下
	{
		CRect rcDest(pleft, height-bottom-pbottom, pleft+left, (height-bottom-pbottom)+bottom);
		CRect rcSrc(0, GetHeight()-bottom, left, (GetHeight()-bottom)+bottom);
		if (!rcDest.IsRectEmpty() && !rcSrc.IsRectEmpty())
			Draw(hDC, rcDest, rcSrc);
	}

	// 中间
	{
		CRect rcDest(left+pleft, top+ptop, (left+pleft)+(width-left-right-pleft-pright), (top+ptop)+(height-top-bottom-ptop-pbottom));
		CRect rcSrc(left, top, left+(GetWidth()-left-right), top+(GetHeight()-top-bottom));
		if (!rcDest.IsRectEmpty() && !rcSrc.IsRectEmpty())
			Draw(hDC, rcDest, rcSrc);
	}

	return TRUE;
}

BOOL CImageEx::DrawNinePartImage(HDC hDC, int x, int y, 
								 int cx, int cy, int nLeft, int nTop, int nRight, int nBottom)
{
	// 左上
	{
		CRect rcDest(x, y, x+nLeft, y+nTop);
		CRect rcSrc(0, 0, nLeft, nTop);
		if (!rcDest.IsRectEmpty() && !rcSrc.IsRectEmpty())
			Draw(hDC, rcDest, rcSrc);
	}

	// 左边
	{
		CRect rcDest(x, y+nTop, x+nLeft, (y+nTop)+(cy-nTop-nBottom));
		CRect rcSrc(0, nTop, nLeft, nTop+(GetHeight()-nTop-nBottom));
		if (!rcDest.IsRectEmpty() && !rcSrc.IsRectEmpty())
			Draw(hDC, rcDest, rcSrc);
	}

	// 上边
	{
		CRect rcDest(x+nLeft, y, (x+nLeft)+(cx-nLeft-nRight), y+nTop);
		CRect rcSrc(nLeft, 0, nLeft+(GetWidth()-nLeft-nRight), nTop);
		if (!rcDest.IsRectEmpty() && !rcSrc.IsRectEmpty())
			Draw(hDC, rcDest, rcSrc);
	}

	// 右上
	{
		CRect rcDest(x+(cx-nRight), y, (x+(cx-nRight))+nRight, y+nTop);
		CRect rcSrc(GetWidth()-nRight, 0, (GetWidth()-nRight)+nRight, nTop);
		if (!rcDest.IsRectEmpty() && !rcSrc.IsRectEmpty())
			Draw(hDC, rcDest, rcSrc);
	}

	// 右边
	{
		CRect rcDest(x+(cx-nRight), y+nTop, (x+(cx-nRight))+nRight, (y+nTop)+(cy-nTop-nBottom));
		CRect rcSrc(GetWidth()-nRight, nTop, (GetWidth()-nRight)+nRight, nTop+(GetHeight()-nTop-nBottom));
		if (!rcDest.IsRectEmpty() && !rcSrc.IsRectEmpty())
			Draw(hDC, rcDest, rcSrc);
	}

	// 下边
	{
		CRect rcDest(x+nLeft, y+(cy-nBottom), (x+nLeft)+(cx-nLeft-nRight), (y+(cy-nBottom))+nBottom);
		CRect rcSrc(nLeft, GetHeight()-nBottom, nLeft+(GetWidth()-nLeft-nRight), (GetHeight()-nBottom)+nBottom);
		if (!rcDest.IsRectEmpty() && !rcSrc.IsRectEmpty())
			Draw(hDC, rcDest, rcSrc);
	}

	// 右下
	{
		CRect rcDest(x+(cx-nRight), y+(cy-nBottom), (x+(cx-nRight))+nRight, (y+(cy-nBottom))+nBottom);
		CRect rcSrc(GetWidth()-nRight, GetHeight()-nBottom, (GetWidth()-nRight)+nRight, (GetHeight()-nBottom)+nBottom);
		if (!rcDest.IsRectEmpty() && !rcSrc.IsRectEmpty())
			Draw(hDC, rcDest, rcSrc);
	}

	// 左下
	{
		CRect rcDest(x, y+(cy-nBottom), x+nLeft, (y+(cy-nBottom))+nBottom);
		CRect rcSrc(0, GetHeight()-nBottom, nLeft, (GetHeight()-nBottom)+nBottom);
		if (!rcDest.IsRectEmpty() && !rcSrc.IsRectEmpty())
			Draw(hDC, rcDest, rcSrc);
	}

	// 中间
	{
		CRect rcDest(x+nLeft, y+nTop, (x+nLeft)+(cx-nLeft-nRight), (y+nTop)+(cy-nTop-nBottom));
		CRect rcSrc(nLeft, nTop, nLeft+(GetWidth()-nLeft-nRight), nTop+(GetHeight()-nTop-nBottom));
		if (!rcDest.IsRectEmpty() && !rcSrc.IsRectEmpty())
			Draw(hDC, rcDest, rcSrc);
	}

	return TRUE;
}