#include "ImgWnd.h"

LRESULT CImgWnd::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) noexcept
{
	auto hImg = LoadBitmap(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDB_BITMAP1));
	ZeroMemory(currentPath, sizeof(currentPath));
	return 0;
}

LRESULT CImgWnd::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) noexcept
{
	PostQuitMessage(0);
	return 0;
}

LRESULT CImgWnd::DrawImage(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/) noexcept
{
	MessageBox(currentPath, _T("Notification"), MB_OK);
	return 0;
}