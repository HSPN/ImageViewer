#include "ImgWnd.h"
#include "libjpeg/jpeglib.h"
#include <new>

LRESULT CImgWnd::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) noexcept
{
	//auto hImg = LoadBitmap(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDB_BITMAP1));
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
	//지금 코드전체에서 TCHAR를 쓰니 맞추기
#ifdef UNICODE
	auto fp = _wfopen(currentPath, _T("r"));
#else
	auto fp = fopen(currentPath, _T("r"));
#endif
	if (fp == nullptr) {
		MessageBox(_T("존재하지 않는 파일"), _T("Notification"), MB_OK);
		return -1;
	}

	_DrawImage(fp);
	fclose(fp);
	return 0;
}

LRESULT CImgWnd::_DrawImage(FILE* fp) noexcept
{
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, fp);

	if (!jpeg_read_header(&cinfo, true)) {
		MessageBox(_T("헤더가 손상된 파일"), _T("Notification"), MB_OK);
		return -1;
	}

	if (!jpeg_start_decompress(&cinfo)) {
		MessageBox(_T("디코딩 실패"), _T("Notification"), MB_OK);
		jpeg_destroy_decompress(&cinfo);
		return -1;
	}
	
	auto imgData = (JSAMPROW)malloc(cinfo.image_width * cinfo.jpeg_color_space);
	if (imgData == nullptr) {
		MessageBox(_T("메모리 부족"), _T("Notification"), MB_OK);
		jpeg_destroy_decompress(&cinfo);
		return -1;
	}

	//cinfo -> BITMAPINFO로 값 매칭
	BITMAPINFO bitmapInfo;
	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo);
	bitmapInfo.bmiHeader.biWidth = cinfo.image_width;
	bitmapInfo.bmiHeader.biHeight = cinfo.image_height;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = cinfo.output_components * cinfo.data_precision;	//컴포넌트 개수 * 컴포넌트당 비트수. 예를들어 RGB * 8비트면 24비트이미지
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	bitmapInfo.bmiHeader.biSizeImage = 0;
	
	//컬러스페이스 종류가 꽤 많음. 일단 평범한 예시에서만 확인하고 다른타입은 따로 추가
	//종류별로 파일부터 다 구해야 테스트가 가능
	//switch (cinfo.out_color_space) {
	//case JCS_RGB :
		
	//}

	//bitmapInfo.bmiColors

	char* ppvBits; //비트맵을 입력할 포인터
	auto hdc = GetParent().GetDC();
	auto hMemDC = CreateCompatibleDC(hdc); //직접채우지않고 다른 DC에 적어서 깜빡임 방지
	auto hBitmap = CreateDIBSection(hdc, &bitmapInfo, DIB_RGB_COLORS, (void **) &ppvBits, NULL, 0);
	
	//for(auto ppvBits
	////bitmapInfo.bmiHeader
	////bitmapInfo

	//
	auto cursor = ppvBits;
	while (cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, &imgData, 1);
		memcpy(imgData, cursor, sizeof(imgData));
		cursor += sizeof(imgData);
	}
	ReleaseDC(hdc);
	//DeleteDC(hMemDC);
	//	BITMAPINFO bitmapInfo;
	//jpeg_create_decompress
	//	bitmapInfo.bmiHeader
	//	CreateDIBSection(GetDC(), )
	//	//MessageBox(currentPath, _T("Notification"), MB_OK);
	//ReleaseDC(dc);
	
	PAINTSTRUCT ps;
	hdc = BeginPaint(&ps);
	//hMemDC = CreateCompatibleDC(hdc);
	SelectObject(hdc, hBitmap);
	BitBlt(hdc, 0, 0, cinfo.image_width, cinfo.image_height, hdc, 0, 0, SRCCOPY);

	//DeleteDC(hMemDC);
	EndPaint(&ps);
	return 0;
}