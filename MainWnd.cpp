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

	__super::Create(nullptr, &rWnd,  APP_NAME, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	CenterWindow();

	// 보이기
	ShowWindow(SW_SHOW);
	DragAcceptFiles(TRUE);

	return TRUE;
}

LRESULT CMainWnd::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) noexcept
{
	HWND hFileOpenButton = CreateWindow(
		_T("BUTTON"), _T("파일 열기"),
		WS_TABSTOP | WS_VISIBLE | WS_CHILD,
		10, 10, 100, 30,
		m_hWnd, (HMENU)IDD_DIALOG1, _Module.GetModuleInstance(), NULL);
	return 0;
}

LRESULT CMainWnd::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) noexcept
{
	PostQuitMessage(0);
	return 0;
}

LRESULT	CMainWnd::OnFileExplorerButtonClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled) noexcept
{
	//프로그램들이 파일탐색기부분이 어쩐지 다 똑같이 생겼다 했더니, 윈도우에서 제공되는걸 쓰는거였음
	auto ofn = OPENFILENAME();
	TCHAR filename[MAX_PATH + 1] = {'\0'};	//_T부터 시작해서, T Alias들은 다 define에 따라 wchar쓸지 char쓸지 바꿔주는 역할
	//TCHAR filepath[MAX_PATH + 1] = { '\0' };
	
	ZeroMemory(&ofn, sizeof(OPENFILENAME)); //memset으로 0대입하는것과 동치
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = sizeof(filename);
	ofn.lpstrFilter = _T("*.jpg\0");
	ofn.nFilterIndex = 1; //1개중에 첫번째..
	//ofn.lpstrFileTitle = NULL; //경로대신 파일명?
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE)
		MessageBox(ofn.lpstrFile, _T("Selected File"), MB_OK);

	return 0;
}

BOOL CMainWnd::PreTranslateMessage(MSG* /*pMsg*/) noexcept
{
	return FALSE;
}
