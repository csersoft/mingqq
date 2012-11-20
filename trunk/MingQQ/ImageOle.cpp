#include "StdAfx.h"
#include "ImageOle.h"

CImageOle::CImageOle(void)
{
	m_ulRef = 0;
	m_pOleClientSite = NULL;
	m_pAdvSink = NULL;
	m_hOwnerWnd = NULL;
	m_nFaceId = 0;
	::SetRectEmpty(&m_rcItem);
	m_pImage = NULL;
	
	GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
}

CImageOle::~CImageOle(void)
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

HRESULT WINAPI CImageOle::QueryInterface(REFIID iid, void ** ppvObject)
{
	if (iid == IID_IUnknown || iid == IID_IOleObject)
	{
		*ppvObject = (IOleObject *)this;
		((IOleObject *)(*ppvObject))->AddRef();
		return S_OK;
	}
	else if (iid == IID_IViewObject || iid == IID_IViewObject2)
	{
		*ppvObject = (IViewObject2 *)this;
		((IViewObject2 *)(*ppvObject))->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG WINAPI CImageOle::AddRef(void)
{
	m_ulRef++;
	return m_ulRef;
}

ULONG WINAPI CImageOle::Release(void)
{
	if (--m_ulRef == 0)
	{
		delete this;
		return 0;
	}

	return m_ulRef;
}

HRESULT WINAPI CImageOle::SetClientSite(IOleClientSite *pClientSite)
{
	m_pOleClientSite = pClientSite;
	return S_OK;
}

HRESULT WINAPI CImageOle::GetClientSite(IOleClientSite **ppClientSite)
{
	*ppClientSite = m_pOleClientSite;
	return S_OK;
}

HRESULT WINAPI CImageOle::SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj)
{
	return S_OK;
}

HRESULT WINAPI CImageOle::Close(DWORD dwSaveOption)
{
	::KillTimer(m_hOwnerWnd, (UINT_PTR)this);
	Destroy();
	m_pOleClientSite = NULL;
	m_hOwnerWnd = NULL;
	m_nFaceId = 0;
	::SetRectEmpty(&m_rcItem);
	if (m_pAdvSink != NULL)
	{
		m_pAdvSink->Release();
		m_pAdvSink = NULL;
	}
	return S_OK;
}

HRESULT WINAPI CImageOle::SetMoniker(DWORD dwWhichMoniker, IMoniker *pmk)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::InitFromData(IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::GetClipboardData(DWORD dwReserved, IDataObject **ppDataObject)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect)
{
	return S_OK;
}

HRESULT WINAPI CImageOle::EnumVerbs(IEnumOLEVERB **ppEnumOleVerb)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::Update(void)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::IsUpToDate(void)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::GetUserClassID(CLSID *pClsid)
{
	*pClsid = IID_NULL;
	return S_OK;
}

HRESULT WINAPI CImageOle::GetUserType(DWORD dwFormOfType, LPOLESTR *pszUserType)
{
	return E_NOTIMPL;
}

// 设置控件可使用的空间（控件在屏幕上使用的区域范围）
HRESULT WINAPI CImageOle::SetExtent(DWORD dwDrawAspect, SIZEL *psizel)
{
	return E_NOTIMPL;
}

// 获取控件可使用的空间
HRESULT WINAPI CImageOle::GetExtent(DWORD dwDrawAspect, SIZEL *psizel)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::Advise(IAdviseSink *pAdvSink, DWORD *pdwConnection)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::Unadvise(DWORD dwConnection)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::EnumAdvise(IEnumSTATDATA **ppenumAdvise)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::SetColorScheme(LOGPALETTE *pLogpal)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, 
							   DVTARGETDEVICE *ptd, HDC hdcTargetDev, HDC hdcDraw, 
							   LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
							   BOOL ( WINAPI *pfnContinue )(ULONG_PTR dwContinue), 
							   ULONG_PTR dwContinue)
{
	if (lindex != -1)
		return S_FALSE;

	m_rcItem.left   = lprcBounds->left;
	m_rcItem.top    = lprcBounds->top;
	m_rcItem.right  = lprcBounds->right;
	m_rcItem.bottom = lprcBounds->bottom;

	if (m_pImage != NULL)
		m_pImage->Draw(hdcDraw, m_rcItem);

	return S_OK;
}

HRESULT WINAPI CImageOle::GetColorSet(DWORD dwDrawAspect, LONG lindex, void *pvAspect, 
									  DVTARGETDEVICE *ptd, HDC hicTargetDev, LOGPALETTE **ppColorSet)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::Freeze(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DWORD *pdwFreeze)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::Unfreeze(DWORD dwFreeze)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::SetAdvise(DWORD aspects, DWORD advf, IAdviseSink *pAdvSink)
{
	if (pAdvSink != NULL)
	{
		m_pAdvSink = pAdvSink;
		m_pAdvSink->AddRef();
	}

	if (m_pImage != NULL && m_pImage->IsAnimatedGif())
	{
		::SetTimer(m_hOwnerWnd, (UINT_PTR)this, 
			m_pImage->GetFrameDelay(), CImageOle::TimerProc);
	}

	return S_OK;
}

HRESULT WINAPI CImageOle::GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel)
{
	if (m_pImage != NULL)
	{
		int nWidth = m_pImage->GetWidth();
		int nHeight = m_pImage->GetHeight();

		HDC hDC = ::GetDC(NULL);
		lpsizel->cx = ::MulDiv(nWidth, 2540, GetDeviceCaps(hDC, LOGPIXELSX));
		lpsizel->cy = ::MulDiv(nHeight, 2540, GetDeviceCaps(hDC, LOGPIXELSY));
		::ReleaseDC(NULL, hDC);
	}

	return S_OK;
}

void CImageOle::SetOwnerHwnd(HWND hWnd)
{
	m_hOwnerWnd = hWnd;
}

HWND CImageOle::GetOwnerHwnd()
{
	return m_hOwnerWnd;
}

void CImageOle::SetFaceId(int nFaceId)
{
	m_nFaceId = nFaceId;
}

int CImageOle::GetFaceId()
{
	return m_nFaceId;
}

BOOL CImageOle::LoadFromFile(LPCTSTR lpszFileName)
{
	if (NULL == lpszFileName || NULL == *lpszFileName)
		return FALSE;

	Destroy();

	m_pImage = new CGifImage;
	if (NULL == m_pImage)
		return FALSE;

	BOOL bRet = m_pImage->LoadFromFile(lpszFileName);
	if (!bRet)
	{
		delete m_pImage;
		m_pImage = NULL;
		return FALSE;
	}

	return TRUE;
}

void CImageOle::Destroy()
{
	if (m_pImage != NULL)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
}

void CImageOle::OnTimer(UINT_PTR idEvent)
{
	if (idEvent != (UINT_PTR)this)
		return;

	::KillTimer(m_hOwnerWnd, (UINT_PTR)this);

	if (m_pImage != NULL)
	{
		m_pImage->ActiveNextFrame();
		::SetTimer(m_hOwnerWnd, (UINT_PTR)this, 
			m_pImage->GetFrameDelay(), CImageOle::TimerProc);
	}

	if (m_pAdvSink != NULL)
		m_pAdvSink->OnViewChange(DVASPECT_CONTENT, -1);

	//::InvalidateRect(m_hOwnerWnd, &m_rcItem, FALSE);
}

VOID CALLBACK CImageOle::TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	CImageOle *pImageOle = (CImageOle *)idEvent;
	if (pImageOle != NULL)
		pImageOle->OnTimer(idEvent);
}