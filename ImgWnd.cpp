#include "ImgWnd.h"

LRESULT CImgWnd::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) noexcept
{
	auto hImg = LoadBitmap(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDB_BITMAP1));

	return 0;
}

LRESULT CImgWnd::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) noexcept
{
	PostQuitMessage(0);
	return 0;
}

LRESULT CImgWnd::OpenImage(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/) noexcept
{
	MessageBox(reinterpret_cast<LPCTSTR>(wParam), _T("Notification"), MB_OK);
	return 0;
}