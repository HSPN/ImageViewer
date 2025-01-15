#pragma once

#include "stdafx.h"
#include "resource.h"

#define WM_USER_READ_IMAGE (WM_USER + 1)

class CImgWnd : public CWindowImpl<CImgWnd, CWindow>
{
public:
	BEGIN_MSG_MAP(CImgWnd)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_USER_READ_IMAGE, ReadImage)
		MESSAGE_HANDLER(WM_PAINT, DrawImage)
	END_MSG_MAP()

	TCHAR currentPath[MAX_PATH + 1];
	const BITMAPINFO GetBitmapInfo() const noexcept;

private:
	LRESULT		OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) noexcept;
	LRESULT		OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT		ReadImage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT		_ReadImage(FILE *fp);

	LRESULT		DrawImage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	BITMAPINFO bitmapInfo;
	HBITMAP hBitmap;
	HDC hdc, hMemDC;
};