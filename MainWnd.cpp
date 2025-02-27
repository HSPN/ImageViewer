﻿#include "stdafx.h"
#include "MainWnd.h"
#include "resource.h"

#include <commdlg.h>

#define	APP_NAME				L"AtlWinApp"
#define APP_CLASS_NAME			L"AtlWinAppClass"
#define APP_TITLE				L"AtlWinApp"

CMainWnd::CMainWnd(void) noexcept
{
}

CMainWnd::~CMainWnd(void)
{
}

BOOL CMainWnd::CreateMainWindow()
{
	// atl 윈도우 생성
	CWndClassInfo &winInfo = GetWndClassInfo();
	winInfo.m_wc.lpszClassName = APP_CLASS_NAME;
	winInfo.m_wc.style = CS_DBLCLKS;
	winInfo.m_wc.hbrBackground = nullptr;
	winInfo.m_wc.hIcon = ::LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_MAIN));
	CRect rWnd(0, 0, 640, 480);

	__super::Create(nullptr, &rWnd, APP_NAME, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	CenterWindow();
	
	// 보이기
	ShowWindow(SW_SHOW);

	rWnd.right = rWnd.bottom = 0;
	m_ImgWnd.Create(m_hWnd, rWnd, _T("Image"), WS_CHILD);

	return TRUE;
}

LRESULT CMainWnd::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//첫인자는 현재 CMainWnd의 hInstance를 받음
	//두번째인자는 매크로 정수를 어거지로 문자열포인터로 바꾸는데, 값 찍어보니까 L"IDR_MENU1"로 바뀌는것도 아님. 저 포인터를 직접 참조하진 않고, 그냥 내부 동작에서 오프셋값같은걸로 쓰는듯
	auto hMenu = LoadMenu(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_MENU1));
	SetMenu(hMenu);

	//m_ImgWnd.Create(m_hWnd, CWindow::rcDefault, _T("Bitmap"));
	//m_ImgWnd.ShowWindow(SW_SHOWNORMAL);
	return 0;
}

LRESULT CMainWnd::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	PostQuitMessage(0);
	return 0;
}

LRESULT CMainWnd::OnResize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/) noexcept
{
	if (!m_ImgWnd.IsWindow()) return 0;

	auto bitmapInfo = m_ImgWnd.GetBitmapInfo();
	auto width = LOWORD(lParam);
	auto height = HIWORD(lParam);
	auto offsetWidth = max((width - bitmapInfo.bmWidth) / 2, 0);
	auto offsetHeight = max((height - bitmapInfo.bmHeight) / 2, 0);
	m_ImgWnd.MoveWindow(offsetWidth, offsetHeight, width, height);
	m_ImgWnd.RedrawWindow();
	return 0;
}

LRESULT CMainWnd::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(m_ImgWnd.IsWindow())
		m_ImgWnd.PostMessage(WM_PAINT, 0, 0);

	return 0;
}

LRESULT	CMainWnd::OnErase(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps;  
	auto hdc = BeginPaint(&ps);
	
	RECT rc; 
	GetClientRect(&rc);

	auto hMemDC = CreateCompatibleDC(hdc);
	auto hBitmap = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
	
	auto old = SelectObject(hMemDC, hBitmap); //널이 아닌값이 리턴되니 일단 Delete
	DeleteObject(old);
	
	FillRect(hMemDC, &rc, (HBRUSH)(COLOR_WINDOW + 1));
	BitBlt(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hMemDC, 0, 0, SRCCOPY);
	
	EndPaint(&ps);
	DeleteDC(hMemDC);
	DeleteObject(hBitmap);
	return 0;
}

BOOL CMainWnd::PreTranslateMessage(MSG* /*pMsg*/) noexcept
{
	return FALSE;
}

LRESULT	CMainWnd::OnFileExplorerButtonClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	//프로그램들이 파일탐색기부분이 어쩐지 다 똑같이 생겼다 했더니, 윈도우에서 제공되는걸 쓰는거였음
	auto ofn = OPENFILENAME();
	TCHAR filename[MAX_PATH] = { '\0' };	//_T부터 시작해서, T Alias들은 다 define에 따라 wchar쓸지 char쓸지 바꿔주는 역할
	//TCHAR filepath[MAX_PATH] = { '\0' };

	ZeroMemory(&ofn, sizeof(OPENFILENAME)); //memset으로 0대입하는것과 동치
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename);
	ofn.lpstrFilter = _T("jpg파일\0*.jpg\0"); //2쌍씩 작성해야함.. [설명\0패턴\0]
	ofn.nFilterIndex = 1; //1개중에 첫번째..

	//ofn.lpstrFileTitle = NULL; //FileTitle은 경로 제외한 파일명. 나중에 파일명 제외한 폴더경로 구할때 쓰면 편할듯
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE) {
		memcpy(m_ImgWnd.currentPath, ofn.lpstrFile, min(sizeof(m_ImgWnd.currentPath), sizeof(filename)));
		m_ImgWnd.PostMessage(WM_USER_READ_IMAGE, 0, 0);
	}
	//MessageBox(ofn.lpstrFile, _T("Selected File"), MB_OK);

	return 0;
}

LRESULT	CMainWnd::OnExitButtonClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	DestroyWindow();
	return 0;
}