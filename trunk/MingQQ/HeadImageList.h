#pragma once

#include "QQClient/QQClient.h"
#include "ImageEx.h"

struct HEAD_PIC_INDEX
{
	UINT nGroupCode;
	UINT nQQUin;
	BOOL bGray;
	int nIndex;
};

class CHeadImageList
{
public:
	CHeadImageList(void);
	~CHeadImageList(void);

public:
	BOOL Init(int cx, int cy, CQQClient * lpQQClient);
	void UnInit();
	HIMAGELIST GetImageList();
	int GetHeadPicIndex(QQ_MSG_TYPE nType, UINT nGroupCode, 
		UINT nQQUin, BOOL bGray, BOOL bForceReload);
	HICON ExtractIcon(int nImage);

private:
	HBITMAP LoadHeadPic(LPCTSTR lpszFileName, BOOL bGray, int cx, int cy);
	HBITMAP LoadHeadPic(UINT nGroupCode, UINT nQQUin, BOOL bGray, int cx, int cy);
	void GetNumber(UINT nGroupCode, UINT nQQUin, UINT& nGroupNum, UINT& nQQNum);

private:
	std::vector<HEAD_PIC_INDEX> m_arrHeadPicIndex;
	CImageList m_ImageList;
	CQQClient * m_lpQQClient;
	CSize m_szHead;
};
