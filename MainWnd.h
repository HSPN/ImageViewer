////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// 메인 윈도우
/// 
/// @author   parkkh
/// @date     Friday, November 19, 2010  4:15:21 PM
/// 
/// Copyright(C) 2010 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once
#include "resource.h"

class CMainWnd  : public CWindowImpl<CMainWnd, CWindow>
{
public:
	CMainWnd() noexcept;
	~CMainWnd() final;
	BOOL		CreateMainWindow();
	BOOL		PreTranslateMessage(MSG* pMsg) noexcept;

public :
	BEGIN_MSG_MAP(CMainWnd)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_HANDLER(IDD_DIALOG1, BN_CLICKED, OnFileExplorerButtonClicked)
	END_MSG_MAP()

private:
	LRESULT		OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) noexcept;
	LRESULT		OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) noexcept;
	LRESULT		OnFileExplorerButtonClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) noexcept;
};

