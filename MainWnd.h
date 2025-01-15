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
#include "ImgWnd.h"

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
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnResize)
		COMMAND_HANDLER(IDM_OPEN, BN_CLICKED, OnFileExplorerButtonClicked)
		COMMAND_HANDLER(IDM_CLOSE, BN_CLICKED, OnExitButtonClicked)
	END_MSG_MAP()

private:
	LRESULT		OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT		OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT		OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT		OnResize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) noexcept;
	LRESULT		OnFileExplorerButtonClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT		OnExitButtonClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	CImgWnd m_ImgWnd;
};

