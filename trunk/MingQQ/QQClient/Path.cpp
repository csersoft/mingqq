#include "StdAfx.h"
#include "Path.h"

namespace ZYM
{

CPath::CPath(void)
{
}

CPath::~CPath(void)
{
}

// 获取应用程序执行路径
CString CPath::GetAppPath()
{
	static TCHAR szPath[MAX_PATH] = {0};

	if (szPath[0] == _T('\0'))
	{
		::GetModuleFileName(NULL, szPath, MAX_PATH);

		CString strPath = GetDirectoryName(szPath);
		_tcsncpy(szPath, strPath, MAX_PATH);
	}

	return szPath;
}

// 获取应用程序当前目录
CString CPath::GetCurDir()
{
	TCHAR szCurDir[MAX_PATH] = {0};
	::GetCurrentDirectory(MAX_PATH, szCurDir);

	if (szCurDir[_tcslen(szCurDir) - 1] != _T('\\'))
		_tcscat(szCurDir, _T("\\"));

	return szCurDir;
}

// 获取当前系统的临时文件夹的路径
CString CPath::GetTempPath()
{
	TCHAR szTempPath[MAX_PATH] = {0};

	::GetTempPath(MAX_PATH, szTempPath);

	if (szTempPath[_tcslen(szTempPath) - 1] != _T('\\'))
		_tcscat(szTempPath, _T("\\"));

	return szTempPath;
}

// 获取当前系统的临时文件夹的路径下的唯一命名的临时文件名(全路径)
CString CPath::GetTempFileName(LPCTSTR lpszFileName)
{
	return GetRandomFileName(GetTempPath(), lpszFileName);
}

// 获取随机文件名(全路径)
CString CPath::GetRandomFileName(LPCTSTR lpszPath, LPCTSTR lpszFileName)
{
	CString strPath, strFileName, strExtFileName, strFullPath;

	if (!IsDirectoryExist(lpszPath))
		strPath = GetCurDir();
	else
		strPath = lpszPath;

	strFileName = GetFileNameWithoutExtension(lpszFileName);
	strExtFileName = GetExtension(lpszFileName);

	for (int i = 2; i < 10000; i++)
	{
		if (strExtFileName.IsEmpty())
			strFullPath.Format(_T("%s%s%d"), strPath, strFileName, i);
		else
			strFullPath.Format(_T("%s%s%d.%s"), strPath, strFileName, i, strExtFileName);
		
		if (!IsFileExist(strFullPath))
		{
			return strFullPath;
		}
	}

	return _T("");
}

// 检测指定路径是否目录
BOOL CPath::IsDirectory(LPCTSTR lpszPath)
{
	if (NULL == lpszPath || _tcslen(lpszPath) <= 0)
		return FALSE;

	DWORD dwAttr = ::GetFileAttributes(lpszPath);

	if((dwAttr != 0xFFFFFFFF) && (dwAttr & FILE_ATTRIBUTE_DIRECTORY))
		return TRUE;
	else
		return FALSE;
}

// 检测指定文件是否存在
BOOL CPath::IsFileExist(LPCTSTR lpszFileName)
{
	if (NULL == lpszFileName || _tcslen(lpszFileName) <= 0)
		return FALSE;

	if(::GetFileAttributes(lpszFileName) != 0xFFFFFFFF)
		return TRUE;
	else
		return FALSE;
}

// 检测指定目录是否存在
BOOL CPath::IsDirectoryExist(LPCTSTR lpszPath)
{
	if (NULL == lpszPath || _tcslen(lpszPath) <= 0)
		return FALSE;

	DWORD dwAttr = ::GetFileAttributes(lpszPath);

	if((dwAttr != 0xFFFFFFFF) && (dwAttr & FILE_ATTRIBUTE_DIRECTORY))
		return TRUE;
	else
		return FALSE;
}

BOOL CPath::CreateDirectory(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	TCHAR cPath[MAX_PATH] = {0};
	TCHAR cTmpPath[MAX_PATH] = {0};
	TCHAR * lpPos = NULL;
	TCHAR cTmp = _T('\0');

	if (NULL == lpPathName || NULL == *lpPathName)
		return FALSE;

	_tcsncpy(cPath, lpPathName, MAX_PATH);

	lpPos = _tcschr(cPath, _T('\\'));
	while (lpPos != NULL)
	{
		if (lpPos == cPath)
		{
			lpPos++;
		}
		else
		{
			cTmp = *lpPos;
			*lpPos = _T('\0');
			_tcsncpy(cTmpPath, cPath, MAX_PATH);
			::CreateDirectory(cTmpPath, lpSecurityAttributes);
			*lpPos = cTmp;
			lpPos++;
		}
		lpPos = _tcschr(lpPos, _T('\\'));
	}

	return TRUE;
}

// 获取指定路径的根目录信息
CString CPath::GetPathRoot(LPCTSTR lpszPath)
{
	CString strPath(lpszPath);

	int nPos = strPath.Find(_T('\\'));
	if (nPos != -1)
	{
		strPath = strPath.Left(nPos + 1);
	}

	return strPath;
}

// 返回指定路径字符串的目录信息
CString CPath::GetDirectoryName(LPCTSTR lpszPath)
{
	CString strPath(lpszPath);

	int nPos = strPath.ReverseFind(_T('\\'));
	if (nPos != -1)
	{
		strPath = strPath.Left(nPos + 1);
	}

	return strPath;
}

// 返回指定路径字符串的文件名和扩展名
CString CPath::GetFileName(LPCTSTR lpszPath)
{
	CString strPath(lpszPath);

	int nPos = strPath.ReverseFind(_T('\\'));
	if (nPos != -1)
	{
		strPath = strPath.Right(strPath.GetLength() - nPos - 1);
	}

	return strPath;
}

// 返回不具有扩展名的路径字符串的文件名
CString CPath::GetFileNameWithoutExtension(LPCTSTR lpszPath)
{
	CString strPath(lpszPath);

	int nPos = strPath.ReverseFind(_T('\\'));
	if (nPos != -1)
		strPath = strPath.Right(strPath.GetLength() - nPos - 1);

	nPos = strPath.ReverseFind(_T('.'));
	if (nPos != -1)
		strPath = strPath.Left(nPos);

	return strPath;
}

// 返回指定的路径字符串的扩展名
CString CPath::GetExtension(LPCTSTR lpszPath)
{
	CString strPath(lpszPath);

	int nPos = strPath.ReverseFind(_T('.'));
	if (nPos != -1)
	{
		strPath = strPath.Right(strPath.GetLength() - nPos - 1);
	}

	return strPath;
}

// 根据指定的相对路径获取绝对路径
CString CPath::GetFullPath(LPCTSTR lpszPath)
{
	CString strPath(lpszPath);

	TCHAR cFirstChar = strPath.GetAt(0);
	TCHAR cSecondChar = strPath.GetAt(1);

	if (cFirstChar == _T('\\'))
	{
		CString strCurDir = GetCurDir();
		CString strRootPath = GetPathRoot(strCurDir);
		return strRootPath + strPath;
	}
	else if (::IsCharAlpha(cFirstChar) && cSecondChar == _T(':'))
	{
		return strPath;
	}
	else
	{
		CString strCurDir = GetAppPath();
		return strCurDir + strPath;
	}
}

}