#pragma once

#include <vector>
#include "XmlDocument.h"

class CFaceInfo
{
public:
	CFaceInfo(void);
	~CFaceInfo(void);

public:
	int m_nId;
	tstring m_strTip;
	tstring m_strFileName;
};

class CFaceList
{
public:
	CFaceList(void);
	~CFaceList(void);

public:
	void Reset();
	BOOL LoadConfigFile(LPCTSTR lpszFileName);
	CFaceInfo * GetFaceInfo(int nIndex);
	CFaceInfo * GetFaceInfoById(int nFaceId);

public:
	int m_nItemWidth, m_nItemHeight;
	int m_nZoomWidth, m_nZoomHeight;
	int m_nRow, m_nCol;
	std::vector<CFaceInfo *> m_arrFaceInfo;
};
