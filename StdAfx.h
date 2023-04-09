// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN	
#pragma once

#define IDM_TIMER1 0x01000
#define WM_TRAY_ICONE WM_USER + 101
#define MAX_LOADSTRING 100
#define BEFORENAVIGATE2 WM_USER
#define DOWNLOADBEGIN WM_USER+1
#define DOWNLOADCOMPLETE WM_USER+2
#define NAVIGATECOMPLETE2  WM_USER+3
#define DOCUMENTCOMPLETE WM_USER+4
#define COMMANDSTATECHANGE WM_USER+5
#define WC_SHELLEXPLORER "TShellExplorer"
#ifdef _WIN64
#define Titre "Applicateur d'images WIM edition 64bits"
#else
#define Titre "Applicateur d'images WIM edition 32bits"
#endif // WIN64


#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <windowsx.h>
#include <wingdi.h>
#include <commctrl.h>
#include <Winuser.h>
#include <Commdlg.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <stdio.h>
#include <conio.h>
#include <winioctl.h>
#include "resource.h"

#pragma comment (lib,"comctl32.lib") 
#pragma comment (lib,"kernel32.lib") 
#pragma comment (lib,"user32.lib") 
#pragma comment (lib,"gdi32.lib") 
#pragma comment (lib,"winspool.lib") 
#pragma comment (lib,"comdlg32.lib") 
#pragma comment (lib,"advapi32.lib") 
#pragma comment (lib,"shell32.lib") 
#pragma comment (lib,"ole32.lib") 
#pragma comment (lib,"oleaut32.lib") 
#pragma comment (lib,"uuid.lib") 
#pragma comment (lib,"odbc32.lib") 
#pragma comment (lib,"odbccp32.lib") 
#pragma comment (lib,"shlwapi.lib") 
#pragma warning(disable: 4996)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
