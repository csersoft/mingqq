#pragma once

#include <richole.h>
#include "GifImage.h"

class CImageOle : public IOleObject, public IViewObject2
{
public:
	CImageOle(void);
	~CImageOle(void);

public:
	// IUnknown接口
	virtual HRESULT WINAPI QueryInterface(REFIID iid, void ** ppvObject);
	virtual ULONG   WINAPI AddRef(void);
	virtual ULONG   WINAPI Release(void);

	// IOleObject接口
	virtual HRESULT WINAPI SetClientSite(IOleClientSite *pClientSite);
	virtual HRESULT WINAPI GetClientSite(IOleClientSite **ppClientSite);
	virtual HRESULT WINAPI SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj);
	virtual HRESULT WINAPI Close(DWORD dwSaveOption);
	virtual HRESULT WINAPI SetMoniker(DWORD dwWhichMoniker, IMoniker *pmk);
	virtual HRESULT WINAPI GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk);
	virtual HRESULT WINAPI InitFromData(IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved);
	virtual HRESULT WINAPI GetClipboardData(DWORD dwReserved, IDataObject **ppDataObject);
	virtual HRESULT WINAPI DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect);
	virtual HRESULT WINAPI EnumVerbs(IEnumOLEVERB **ppEnumOleVerb);
	virtual HRESULT WINAPI Update(void);
	virtual HRESULT WINAPI IsUpToDate(void);
	virtual HRESULT WINAPI GetUserClassID(CLSID *pClsid);
	virtual HRESULT WINAPI GetUserType(DWORD dwFormOfType, LPOLESTR *pszUserType);
	virtual HRESULT WINAPI SetExtent(DWORD dwDrawAspect, SIZEL *psizel);
	virtual HRESULT WINAPI GetExtent(DWORD dwDrawAspect, SIZEL *psizel);
	virtual HRESULT WINAPI Advise(IAdviseSink *pAdvSink, DWORD *pdwConnection);
	virtual HRESULT WINAPI Unadvise(DWORD dwConnection);
	virtual HRESULT WINAPI EnumAdvise(IEnumSTATDATA **ppenumAdvise);
	virtual HRESULT WINAPI GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus);
	virtual HRESULT WINAPI SetColorScheme(LOGPALETTE *pLogpal);

	// IViewObject接口
	virtual HRESULT WINAPI Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DVTARGETDEVICE *ptd,
		HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
		BOOL ( WINAPI *pfnContinue )(ULONG_PTR dwContinue), ULONG_PTR dwContinue);
	virtual HRESULT WINAPI GetColorSet(DWORD dwDrawAspect, LONG lindex, void *pvAspect, 
		DVTARGETDEVICE *ptd, HDC hicTargetDev, LOGPALETTE **ppColorSet);
	virtual HRESULT WINAPI Freeze(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DWORD *pdwFreeze);
	virtual HRESULT WINAPI Unfreeze(DWORD dwFreeze);
	virtual HRESULT WINAPI SetAdvise(DWORD aspects, DWORD advf, IAdviseSink *pAdvSink);
	virtual HRESULT WINAPI GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink);

	// IViewObject2接口
	virtual HRESULT WINAPI GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel);

	void SetOwnerHwnd(HWND hWnd);
	HWND GetOwnerHwnd();

	void SetFaceId(int nFaceId);
	int GetFaceId();

	BOOL LoadFromFile(LPCTSTR lpszFileName);
	void Destroy();

	void OnTimer(UINT_PTR idEvent);

	static VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

protected:
	ULONG m_ulRef;
	IOleClientSite *m_pOleClientSite;
	IAdviseSink *m_pAdvSink;
	HWND m_hOwnerWnd;
	int m_nFaceId;
	RECT m_rcItem;
	CGifImage *m_pImage;
 	Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
 	ULONG_PTR m_gdiplusToken;
};
