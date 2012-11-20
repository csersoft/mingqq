#include "StdAfx.h"
#include <time.h>
#include "QQUtils.h"

WCHAR * AnsiToUnicode(const CHAR * lpszStr)
{
	WCHAR * lpUnicode;
	int nLen;

	if (NULL == lpszStr)
		return NULL;

	nLen = ::MultiByteToWideChar(CP_ACP, 0, lpszStr, -1, NULL, 0);
	if (0 == nLen)
		return NULL;

	lpUnicode = new WCHAR[nLen + 1];
	if (NULL == lpUnicode)
		return NULL;

	memset(lpUnicode, 0, sizeof(WCHAR) * (nLen + 1));
	nLen = ::MultiByteToWideChar(CP_ACP, 0, lpszStr, -1, lpUnicode, nLen);
	if (0 == nLen)
	{
		delete []lpUnicode;
		return NULL;
	}

	return lpUnicode;
}

CHAR * UnicodeToAnsi(const WCHAR * lpszStr)
{
	CHAR * lpAnsi;
	int nLen;

	if (NULL == lpszStr)
		return NULL;

	nLen = ::WideCharToMultiByte(CP_ACP, 0, lpszStr, -1, NULL, 0, NULL, NULL);
	if (0 == nLen)
		return NULL;

	lpAnsi = new CHAR[nLen + 1];
	if (NULL == lpAnsi)
		return NULL;

	memset(lpAnsi, 0, nLen + 1);
	nLen = ::WideCharToMultiByte(CP_ACP, 0, lpszStr, -1, lpAnsi, nLen, NULL, NULL);
	if (0 == nLen)
	{
		delete []lpAnsi;
		return NULL;
	}

	return lpAnsi;
}

CHAR * AnsiToUtf8(const CHAR * lpszStr)
{
	WCHAR * lpUnicode;
	CHAR * lpUtf8;
	int nLen;

	if (NULL == lpszStr)
		return NULL;

	nLen = ::MultiByteToWideChar(CP_ACP, 0, lpszStr, -1, NULL, NULL);
	if (0 == nLen)
		return NULL;

	lpUnicode = new WCHAR[nLen + 1];
	if (NULL == lpUnicode)
		return NULL;

	memset(lpUnicode, 0, sizeof(WCHAR) * (nLen + 1));
	nLen = ::MultiByteToWideChar(CP_ACP, 0, lpszStr, -1, lpUnicode, nLen);
	if (0 == nLen)
	{
		delete []lpUnicode;
		return NULL;
	}

	nLen = ::WideCharToMultiByte(CP_UTF8, 0, lpUnicode, -1, NULL, 0, NULL, NULL);
	if (0 == nLen)
	{
		delete []lpUnicode;
		return NULL;
	}

	lpUtf8 = new CHAR[nLen + 1];
	if (NULL == lpUtf8)
	{
		delete []lpUnicode;
		return NULL;
	}

	memset(lpUtf8, 0, nLen + 1);
	nLen = ::WideCharToMultiByte(CP_UTF8, 0, lpUnicode, -1, lpUtf8, nLen, NULL, NULL);
	if (0 == nLen)
	{
		delete []lpUnicode;
		delete []lpUtf8;
		return NULL;
	}
	
	delete []lpUnicode;

	return lpUtf8;
}

CHAR * Utf8ToAnsi(const CHAR * lpszStr)
{
	WCHAR * lpUnicode;
	CHAR * lpAnsi;
	int nLen;

	if (NULL == lpszStr)
		return NULL;

	nLen = ::MultiByteToWideChar(CP_UTF8, 0, lpszStr, -1, NULL, NULL);
	if (0 == nLen)
		return NULL;

	lpUnicode = new WCHAR[nLen + 1];
	if (NULL == lpUnicode)
		return NULL;
	
	memset(lpUnicode, 0, sizeof(WCHAR) * (nLen + 1));
	nLen = ::MultiByteToWideChar(CP_UTF8, 0, lpszStr, -1, lpUnicode, nLen);
	if (0 == nLen)
	{
		delete []lpUnicode;
		return NULL;
	}

	nLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicode, -1, NULL, 0, NULL, NULL);
	if (0 == nLen)
	{
		delete []lpUnicode;
		return NULL;
	}

	lpAnsi = new CHAR[nLen + 1];
	if (NULL == lpAnsi)
	{
		delete []lpUnicode;
		return NULL;
	}

	memset(lpAnsi, 0, nLen + 1);
	nLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicode, -1, lpAnsi, nLen, NULL, NULL);
	if (0 == nLen)
	{
		delete []lpUnicode;
		delete []lpAnsi;
		return NULL;
	}

	delete []lpUnicode;

	return lpAnsi;
}

CHAR * UnicodeToUtf8(const WCHAR * lpszStr)
{
	CHAR * lpUtf8;
	int nLen;

	if (NULL == lpszStr)
		return NULL;

	nLen = ::WideCharToMultiByte(CP_UTF8, 0, lpszStr, -1, NULL, 0, NULL, NULL);
	if (0 == nLen)
		return NULL;

	lpUtf8 = new CHAR[nLen + 1];
	if (NULL == lpUtf8)
		return NULL;

	memset(lpUtf8, 0, nLen + 1);
	nLen = ::WideCharToMultiByte(CP_UTF8, 0, lpszStr, -1, lpUtf8, nLen, NULL, NULL);
	if (0 == nLen)
	{
		delete []lpUtf8;
		return NULL;
	}
	
	return lpUtf8;
}

WCHAR * Utf8ToUnicode(const CHAR * lpszStr)
{
	WCHAR * lpUnicode;
	int nLen;

	if (NULL == lpszStr)
		return NULL;

	nLen = ::MultiByteToWideChar(CP_UTF8, 0, lpszStr, -1, NULL, 0);
	if (0 == nLen)
		return NULL;

	lpUnicode = new WCHAR[nLen + 1];
	if (NULL == lpUnicode)
		return NULL;

	memset(lpUnicode, 0, sizeof(WCHAR) * (nLen + 1));
	nLen = ::MultiByteToWideChar(CP_UTF8, 0, lpszStr, -1, lpUnicode, nLen);
	if (0 == nLen)
	{
		delete []lpUnicode;
		return NULL;
	}
	
	return lpUnicode;
}

BOOL AnsiToUnicode(const CHAR * lpszAnsi, WCHAR * lpszUnicode, int nLen)
{
	int nRet = ::MultiByteToWideChar(CP_ACP, 0, lpszAnsi, -1, lpszUnicode, nLen);
	return (0 == nRet) ? FALSE : TRUE;
}

BOOL UnicodeToAnsi(const WCHAR * lpszUnicode, CHAR * lpszAnsi, int nLen)
{
	int nRet = ::WideCharToMultiByte(CP_ACP, 0, lpszUnicode, -1, lpszAnsi, nLen, NULL, NULL);
	return (0 == nRet) ? FALSE : TRUE;
}

BOOL AnsiToUtf8(const CHAR * lpszAnsi, CHAR * lpszUtf8, int nLen)
{
	WCHAR * lpszUnicode = AnsiToUnicode(lpszAnsi);
	if (NULL == lpszUnicode)
		return FALSE;

	int nRet = UnicodeToUtf8(lpszUnicode, lpszUtf8, nLen);

	delete []lpszUnicode;

	return (0 == nRet) ? FALSE : TRUE;
}

BOOL Utf8ToAnsi(const CHAR * lpszUtf8, CHAR * lpszAnsi, int nLen)
{
	WCHAR * lpszUnicode = Utf8ToUnicode(lpszUtf8);
	if (NULL == lpszUnicode)
		return FALSE;

	int nRet = UnicodeToAnsi(lpszUnicode, lpszAnsi, nLen);

	delete []lpszUnicode;

	return (0 == nRet) ? FALSE : TRUE;
}

BOOL UnicodeToUtf8(const WCHAR * lpszUnicode, CHAR * lpszUtf8, int nLen)
{
	int nRet = ::WideCharToMultiByte(CP_UTF8, 0, lpszUnicode, -1, lpszUtf8, nLen, NULL, NULL);
	return (0 == nRet) ? FALSE : TRUE;
}

BOOL Utf8ToUnicode(const CHAR * lpszUtf8, WCHAR * lpszUnicode, int nLen)
{
	int nRet = ::MultiByteToWideChar(CP_UTF8, 0, lpszUtf8, -1, lpszUnicode, nLen);
	return (0 == nRet) ? FALSE : TRUE;
}

std::wstring AnsiToUnicode(const std::string& strAnsi)
{
	std::wstring strUnicode;

	WCHAR * lpszUnicode = AnsiToUnicode(strAnsi.c_str());
	if (lpszUnicode != NULL)
	{
		strUnicode = lpszUnicode;
		delete []lpszUnicode;
	}

	return strUnicode;
}
std::string UnicodeToAnsi(const std::wstring& strUnicode)
{
	std::string strAnsi;

	CHAR * lpszAnsi = UnicodeToAnsi(strUnicode.c_str());
	if (lpszAnsi != NULL)
	{
		strAnsi = lpszAnsi;
		delete []lpszAnsi;
	}

	return strAnsi;
}

std::string AnsiToUtf8(const std::string& strAnsi)
{
	std::string strUtf8;

	CHAR * lpszUtf8 = AnsiToUtf8(strAnsi.c_str());
	if (lpszUtf8 != NULL)
	{
		strUtf8 = lpszUtf8;
		delete []lpszUtf8;
	}

	return strUtf8;
}

std::string Utf8ToAnsi(const std::string& strUtf8)
{
	std::string strAnsi;

	CHAR * lpszAnsi = Utf8ToAnsi(strUtf8.c_str());
	if (lpszAnsi != NULL)
	{
		strAnsi = lpszAnsi;
		delete []lpszAnsi;
	}

	return strAnsi;
}

std::string UnicodeToUtf8(const std::wstring& strUnicode)
{
	std::string strUtf8;

	CHAR * lpszUtf8 = UnicodeToUtf8(strUnicode.c_str());
	if (lpszUtf8 != NULL)
	{
		strUtf8 = lpszUtf8;
		delete []lpszUtf8;
	}

	return strUtf8;
}

std::wstring Utf8ToUnicode(const std::string& strUtf8)
{
	std::wstring strUnicode;

	WCHAR * lpszUnicode = Utf8ToUnicode(strUtf8.c_str());
	if (lpszUnicode != NULL)
	{
		strUnicode = lpszUnicode;
		delete []lpszUnicode;
	}

	return strUnicode;
}

BOOL ToHexStr(const CHAR * lpStr, int nSrcLen, CHAR * lpHex, int nDestLen)
{
	const CHAR cHexTable[] = "0123456789ABCDEF";

	if (lpStr == NULL || nSrcLen <= 0
		|| lpHex == NULL || nDestLen <= 0)
		return FALSE;

	if (nDestLen <= nSrcLen * 2)
		return FALSE;

	int i = 0;
	for (int j = 0; j < (int)strlen(lpStr); j++)
	{
		unsigned int a = (unsigned int)lpStr[j];
		lpHex[i++] = cHexTable[(a & 0xf0) >> 4];
		lpHex[i++] = cHexTable[(a & 0x0f)];
	}
	lpHex[i] = '\0';

	return TRUE;
}

BOOL StrToHex(const CHAR * lpStr, CHAR * lpHex, int nLen)
{
	const CHAR cHexTable[] = "0123456789ABCDEF";

	if (lpStr == NULL || lpHex == NULL || nLen <= 0)
		return FALSE;

	int nSrcLen = strlen(lpStr);
	if (nLen <= nSrcLen * 2)
		return FALSE;

	int i = 0;
	for (int j = 0; j < (int)strlen(lpStr); j++)
	{
		unsigned int a = (unsigned int)lpStr[j];
		lpHex[i++] = cHexTable[(a & 0xf0) >> 4];
		lpHex[i++] = cHexTable[(a & 0x0f)];
	}
	lpHex[i] = '\0';

	return TRUE;
}

// 16位整型数据网络字节序与正常字节序转换
u_short Swap16(u_short nValue)
{
	u_short nRetValue = (u_short)((nValue & 0xff00) >> 0x08);
	nRetValue |= (u_short)(nValue << 0x08);
	return nRetValue;

}

// 32位整型数据网络字节序与正常字节序转换
u_long Swap32(u_long nValue)
{
	u_long nRetValue = ((nValue & 0xff000000) >> 0x18);
	nRetValue |= ((nValue & 0x00ff0000) >> 0x08);
	nRetValue |= ((nValue & 0x0000ff00) << 0x08);
	nRetValue |= ((nValue & 0x000000ff) << 0x18);

	return nRetValue;
}

COLORREF HexStrToRGB(LPCTSTR lpszStr)
{
	long lValue = _tcstol(lpszStr, NULL, 16);
	return RGB((lValue & 0xFF0000) >> 16,  (lValue & 0xFF00 ) >> 8, lValue & 0xFF);
}

LPCTSTR RGBToHexStr(COLORREF color)
{
	static TCHAR szBuf[16] = {0};
	wsprintf(szBuf, _T("%02x%02x%02x"), GetRValue(color), GetGValue(color), GetBValue(color));
	return szBuf;
}

BOOL IsToday(time_t lTime)
{
	time_t lCurTime;
	struct tm * lpCurTimeInfo, * lpTimeInfo;

	lCurTime = time(NULL);
	lpCurTimeInfo = localtime(&lCurTime);

	lpTimeInfo = localtime(&lTime);

	if (lpCurTimeInfo != NULL && lpTimeInfo != NULL
		&& lpCurTimeInfo->tm_year == lpTimeInfo->tm_year
		&& lpCurTimeInfo->tm_mon == lpTimeInfo->tm_mon
		&& lpCurTimeInfo->tm_mday == lpTimeInfo->tm_mday)
		return TRUE;
	else
		return FALSE;
}

// _T("%Y-%m-%d %H:%M:%S")
const TCHAR * FormatTime(time_t lTime, LPCTSTR lpFmt)
{
	struct tm * lpTimeInfo;
	static TCHAR cTime[32];

	memset(cTime, 0, sizeof(cTime));

	lpTimeInfo = localtime(&lTime);
	if (lpTimeInfo != NULL)
		_tcsftime(cTime, sizeof(cTime) / sizeof(TCHAR), lpFmt, lpTimeInfo);
	return cTime;
}

BOOL IsToday(SYSTEMTIME * lpTime)
{
	if (NULL == lpTime)
		return FALSE;

	SYSTEMTIME stSysTime = {0};
	::GetLocalTime(&stSysTime);

	if (stSysTime.wYear == lpTime->wYear
		&& stSysTime.wMonth == lpTime->wMonth
		&& stSysTime.wDay == lpTime->wDay)
		return TRUE;
	else
		return FALSE;
}

// 获取文件最后修改时间
BOOL GetFileLastWriteTime(LPCTSTR lpszFileName, SYSTEMTIME * lpSysTime)
{
	BOOL bRet = FALSE;

	if (NULL == lpszFileName || NULL == *lpszFileName
		|| NULL == lpSysTime)
		return FALSE;

	HANDLE hFile = ::CreateFile(lpszFileName, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL); 
	if (hFile != INVALID_HANDLE_VALUE)
	{
		FILETIME stLastWriteTime = {0};
		bRet = ::GetFileTime(hFile, NULL, NULL, &stLastWriteTime);
		if (bRet)
		{
			FILETIME stLocalFileTime = {0};
			bRet = ::FileTimeToLocalFileTime(&stLastWriteTime, &stLocalFileTime);
			if (bRet)
				bRet = ::FileTimeToSystemTime(&stLocalFileTime, lpSysTime);
		}
		::CloseHandle(hFile);
	}
	return bRet;
}