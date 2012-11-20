#pragma once

#include <string>

#if defined(UNICODE) || defined(_UNICODE)
	typedef std::wstring tstring;
#else
	typedef std::string tstring;
#endif

WCHAR * AnsiToUnicode(const CHAR * lpszStr);
CHAR * UnicodeToAnsi(const WCHAR * lpszStr);
CHAR * AnsiToUtf8(const CHAR * lpszStr);
CHAR * Utf8ToAnsi(const CHAR * lpszStr);
CHAR * UnicodeToUtf8(const WCHAR * lpszStr);
WCHAR * Utf8ToUnicode(const CHAR * lpszStr);

BOOL AnsiToUnicode(const CHAR * lpszAnsi, WCHAR * lpszUnicode, int nLen);
BOOL UnicodeToAnsi(const WCHAR * lpszUnicode, CHAR * lpszAnsi, int nLen);
BOOL AnsiToUtf8(const CHAR * lpszAnsi, CHAR * lpszUtf8, int nLen);
BOOL Utf8ToAnsi(const CHAR * lpszUtf8, CHAR * lpszAnsi, int nLen);
BOOL UnicodeToUtf8(const WCHAR * lpszUnicode, CHAR * lpszUtf8, int nLen);
BOOL Utf8ToUnicode(const CHAR * lpszUtf8, WCHAR * lpszUnicode, int nLen);

std::wstring AnsiToUnicode(const std::string& strAnsi);
std::string UnicodeToAnsi(const std::wstring& strUnicode);
std::string AnsiToUtf8(const std::string& strAnsi);
std::string Utf8ToAnsi(const std::string& strUtf8);
std::string UnicodeToUtf8(const std::wstring& strUnicode);
std::wstring Utf8ToUnicode(const std::string& strUtf8);

BOOL ToHexStr(const CHAR * lpStr, int nSrcLen, CHAR * lpHex, int nDestLen);
BOOL StrToHex(const CHAR * lpStr, CHAR * lpHex, int nLen);

u_short Swap16(u_short nValue);		// 16位整型数据网络字节序与正常字节序转换
u_long Swap32(u_long nValue);		// 32位整型数据网络字节序与正常字节序转换

COLORREF HexStrToRGB(LPCTSTR lpszStr);
LPCTSTR RGBToHexStr(COLORREF color);

BOOL IsToday(time_t lTime);
const TCHAR * FormatTime(time_t lTime, LPCTSTR lpFmt);

BOOL IsToday(SYSTEMTIME * lpTime);
BOOL GetFileLastWriteTime(LPCTSTR lpszFileName, SYSTEMTIME * lpSysTime);	// 获取文件最后修改时间