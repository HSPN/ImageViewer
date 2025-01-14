#pragma once

#include "stdafx.h"
#include "resource.h"

#define WM_USER_DRAW_IMAGE (WM_USER + 1)

class CImgWnd : public CWindowImpl<CImgWnd, CWindow>
{
public:
	BEGIN_MSG_MAP(CImgWnd)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_USER_DRAW_IMAGE, DrawImage)
	END_MSG_MAP()

	TCHAR currentPath[MAX_PATH + 1];

private:
	LRESULT		OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) noexcept;
	LRESULT		OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) noexcept;
	LRESULT		DrawImage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) noexcept;
};