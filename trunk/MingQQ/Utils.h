#pragma once

#include <vector>
#include <string>
#include "ImageOle.h"

#if defined(UNICODE) || defined(_UNICODE)
	typedef std::wstring tstring;
#else
	typedef std::string tstring;
#endif

BOOL EnumSysFont(std::vector<tstring> * arrSysFont);	// 枚举系统字体
BOOL FlashWindowEx(HWND hWnd, int nCount);				// 闪烁窗口标题栏
BOOL GetTrayWndRect(RECT * lpRect);						// 获取系统任务栏区域

void RichEdit_SetDefFont(CRichEditCtrl& richEdit, LPCTSTR lpFontName, int nFontSize,	// 设置RichEdit默认字体
					  COLORREF clrText, BOOL bBold, BOOL bItalic, BOOL bUnderLine);	
void RichEdit_SetDefFont(CRichEditCtrl& richEdit, LPCTSTR lpFontName, int nFontSize);	// 设置RichEdit默认字体名称和大小
void RichEdit_SetDefTextColor(CRichEditCtrl& richEdit, COLORREF clrText);	// 设置RichEdit默认字体颜色
void RichEdit_SetFont(CRichEditCtrl& richEdit, LPCTSTR lpFontName, int nFontSize,	// 设置RichEdit字体
					  COLORREF clrText, BOOL bBold, BOOL bItalic, BOOL bUnderLine);	
void RichEdit_SetFont(CRichEditCtrl& richEdit, LPCTSTR lpFontName, int nFontSize);	// 设置RichEdit字体名称和大小
void RichEdit_SetTextColor(CRichEditCtrl& richEdit, COLORREF clrText);	// 设置RichEdit字体颜色
void RichEdit_AppendText(CRichEditCtrl& richEdit, LPCTSTR lpText);		// RichEdit添加文本
void RichEdit_SetStartIndent(CRichEditCtrl& richEdit, int nSize);	// RichEdit设置左缩进(单位:缇)
BOOL RichEdit_InsertFace(CRichEditCtrl& richEdit, LPCTSTR lpszFileName, int nFaceId);	// 插入表情图片
void RichEdit_GetText(CRichEditCtrl& richEdit, CString& strText);	// 获取文本

/********************************* CMainDlg自定义消息 *********************************/
#define			WM_TRAYICON_NOTIFY		WM_USER + 1		// 系统托盘图标通知消息
#define			WM_SHOW_BUDDYCHATDLG	WM_USER + 2		// 显示好友会话聊天对话框消息
#define			WM_SHOW_GROUPCHATDLG	WM_USER + 3		// 显示群会话聊天对话框消息
#define			WM_SHOW_SESSCHATDLG		WM_USER + 4		// 显示临时会话聊天对话框消息
#define			WM_SHOW_SYSGROUPCHATDLG	WM_USER + 5		// 显示群系统对话框消息
#define			WM_SHOW_BUDDYINFODLG	WM_USER + 6		// 显示好友信息对话框消息
#define			WM_SHOW_GMEMBERINFODLG	WM_USER + 8		// 显示群成员信息对话框消息
#define			WM_SHOW_GROUPINFODLG	WM_USER + 9		// 显示群信息对话框消息
#define			WM_CLOSE_BUDDYCHATDLG	WM_USER + 10	// 关闭好友会话聊天对话框消息
#define			WM_CLOSE_GROUPCHATDLG	WM_USER + 11	// 关闭群会话聊天对话框消息
#define			WM_CLOSE_SESSCHATDLG	WM_USER + 12	// 关闭临时会话聊天对话框消息
#define			WM_CLOSE_SYSGROUPCHATDLG WM_USER + 13	// 关闭群系统对话框消息
#define			WM_CLOSE_BUDDYINFODLG	WM_USER + 14	// 关闭好友信息对话框消息
#define			WM_CLOSE_GROUPINFODLG	WM_USER + 15	// 关闭群成员信息对话框消息
#define			WM_CLOSE_GMEMBERINFODLG	WM_USER + 16	// 关闭群信息对话框消息
#define			WM_DEL_MSG_SENDER		WM_USER + 17	// 删除发送者消息
#define			WM_CANCEL_FLASH			WM_USER + 18	// 取消托盘图标闪烁消息
/********************************* CMainDlg自定义消息 *********************************/