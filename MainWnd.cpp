#include "stdafx.h"
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
	DragAcceptFiles(TRUE);

	CWndClassInfo& imgWinInfo = m_ImgWnd.GetWndClassInfo();
	imgWinInfo.m_wc.lpszClassName = APP_CLASS_NAME;
	imgWinInfo.m_wc.style = CS_DBLCLKS;
	imgWinInfo.m_wc.hbrBackground = nullptr;
	imgWinInfo.m_wc.hIcon = ::LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_MAIN));
	m_ImgWnd.Create(m_hWnd, &rWnd, _T("Image"), WS_CHILD);

	return TRUE;
}

LRESULT CMainWnd::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) noexcept
{
	//첫인자는 현재 CMainWnd의 hInstance를 받음
	//두번째인자는 매크로 정수를 어거지로 문자열포인터로 바꾸는데, 값 찍어보니까 L"IDR_MENU1"로 바뀌는것도 아님. 저 포인터를 직접 참조하진 않고, 그냥 내부 동작에서 오프셋값같은걸로 쓰는듯
	auto hMenu = LoadMenu(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_MENU1));
	SetMenu(hMenu);

	//m_ImgWnd.Create(m_hWnd, CWindow::rcDefault, _T("Bitmap"));
	//m_ImgWnd.ShowWindow(SW_SHOWNORMAL);
	return 0;
}

LRESULT CMainWnd::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) noexcept
{
	if(m_ImgWnd.IsWindow())
		m_ImgWnd.DestroyWindow();
	PostQuitMessage(0);
	return 0;
}

LRESULT CMainWnd::OnResize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/) noexcept
{
	if (!m_ImgWnd.IsWindow()) return 0;

	auto bitmapInfo = m_ImgWnd.GetBitmapInfo();
	auto width = LOWORD(lParam);
	auto height = HIWORD(lParam);
	auto offsetWidth = max((width - bitmapInfo.bmiHeader.biWidth) / 2, 0);
	auto offsetHeight = max((height - bitmapInfo.bmiHeader.biHeight) / 2, 0);
	m_ImgWnd.MoveWindow(offsetWidth, offsetHeight, width, height);
	m_ImgWnd.RedrawWindow();
	return 0;
}

LRESULT CMainWnd::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/) noexcept
{
	PAINTSTRUCT ps;  
	auto hdc = BeginPaint(&ps); //GetDC대신 WM_PAINT안에서만 사용
	auto hMemDC = CreateCompatibleDC(hdc);
	auto hBitmap = CreateCompatibleBitmap(hdc, 0, 0);
	auto old = SelectObject(hMemDC, hBitmap);

	RECT rect;
	GetWindowRect(&rect);
	rect.right -= rect.left;
	rect.left = 0;
	rect.bottom -= rect.top;
	rect.top = 0;

	//Rectangle(hMemDC, 0, 0, rect.right, rect.bottom);
	FillRect(hMemDC, &rect, (HBRUSH)(COLOR_WINDOW+1));
	BitBlt(hdc, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY);

	DeleteDC(hMemDC);
	SelectObject(hMemDC, old);
	EndPaint(&ps);
	DeleteObject(hBitmap);
	//return 0;
	if(m_ImgWnd.IsWindow())
		m_ImgWnd.PostMessage(WM_PAINT, 0, 0);
	return 0;
}

BOOL CMainWnd::PreTranslateMessage(MSG* /*pMsg*/) noexcept
{
	return FALSE;
}

LRESULT	CMainWnd::OnFileExplorerButtonClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) noexcept
{
	//프로그램들이 파일탐색기부분이 어쩐지 다 똑같이 생겼다 했더니, 윈도우에서 제공되는걸 쓰는거였음
	auto ofn = OPENFILENAME();
	TCHAR filename[MAX_PATH + 1] = { '\0' };	//_T부터 시작해서, T Alias들은 다 define에 따라 wchar쓸지 char쓸지 바꿔주는 역할
	//TCHAR filepath[MAX_PATH + 1] = { '\0' };

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

LRESULT	CMainWnd::OnExitButtonClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) noexcept
{
	DestroyWindow();
	return 0;
}