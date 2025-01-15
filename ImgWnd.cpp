#include "ImgWnd.h"
#include "libjpeg/jpeglib.h"
#include <new>

LRESULT CImgWnd::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) noexcept
	//auto hImg = LoadBitmap(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDB_BITMAP1));
{
	ZeroMemory(currentPath, sizeof(currentPath));
	ShowWindow(SW_SHOW);
	return 0;
}

LRESULT CImgWnd::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) noexcept
{
	PostQuitMessage(0);
	return 0;
}

LRESULT CImgWnd::DrawImage(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/) noexcept
{
	//MessageBox(_T("호출호출"), _T("Notification"), MB_OK);
	PAINTSTRUCT ps;
	auto hdc = GetParent().BeginPaint(&ps);
	auto hMemDC = CreateCompatibleDC(hdc);
	SelectObject(hMemDC, hBitmap);

	BITMAP bit;

	GetObject(hBitmap, sizeof(BITMAP), &bit);

	BitBlt(hdc, 0, 0, bitmapInfo.bmiHeader.biWidth, bitmapInfo.bmiHeader.biHeight, hMemDC, 0, 0, SRCCOPY);

	DeleteDC(hMemDC);
	EndPaint(&ps);

	return 0;
}

LRESULT CImgWnd::ReadImage(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/) noexcept
{
	//지금 코드전체에서 TCHAR를 쓰니 맞추기
	//유니코드 빼보니, 빼게되면 수정할부분이 조금 있긴 함
#ifdef UNICODE
	auto fp = _wfopen(currentPath, _T("rb"));
#else
	auto fp = fopen(currentPath, _T("rb"));
#endif
	if (fp == nullptr) {
		MessageBox(_T("존재하지 않는 파일"), _T("Notification"), MB_OK);
		return -1;
	}
	
	_ReadImage(fp);
	fclose(fp);
	return 0;
}

LRESULT CImgWnd::_ReadImage(FILE* fp) noexcept
{
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, fp);

	if (!jpeg_read_header(&cinfo, true)) { //return 0 == SUSPENDED
		MessageBox(_T("헤더가 손상된 파일"), _T("Notification"), MB_OK);
		return -1;
	}

	if (!jpeg_start_decompress(&cinfo)) {
		MessageBox(_T("디코딩 실패"), _T("Notification"), MB_OK);
		jpeg_destroy_decompress(&cinfo);
		return -1;
	}
	
	auto imgData = (JSAMPLE *)malloc(cinfo.image_width * cinfo.image_height * 3);// cinfo.jpeg_color_space);
	if (imgData == nullptr) {
		MessageBox(_T("메모리 부족"), _T("Notification"), MB_OK);
		jpeg_destroy_decompress(&cinfo);
		return -1;
	}
	

	//cinfo -> BITMAPINFO로 값 매칭
	ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo);
	bitmapInfo.bmiHeader.biWidth = cinfo.image_width;
	bitmapInfo.bmiHeader.biHeight = cinfo.image_height;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = cinfo.output_components * cinfo.data_precision;	//컴포넌트 개수 * 컴포넌트당 비트수. 예를들어 RGB * 8비트면 24비트이미지
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	//bitmapInfo.bmiHeader.biSizeImage = 5760*1080;
	bitmapInfo.bmiHeader.biYPelsPerMeter = 1;
	bitmapInfo.bmiHeader.biXPelsPerMeter = 1;
	
	
	//컬러스페이스 종류가 꽤 많음. 일단 평범한 예시에서만 확인하고 다른타입은 따로 추가
	//종류별로 파일부터 다 구해야 테스트가 가능
	//switch (cinfo.out_color_space) {
	//case JCS_RGB :
		
	//}

	//bitmapInfo.bmiColors

	void* ppvBits; //비트맵을 입력할 포인터
	auto hdc = GetDC();
	auto hMemDC = CreateCompatibleDC(hdc); //직접채우지않고 다른 DC에 적어놓고 교체
	hBitmap = CreateDIBSection(hdc, &bitmapInfo, DIB_RGB_COLORS, &ppvBits, NULL, 0);
	
	auto cursor = reinterpret_cast<BYTE*>(ppvBits);
	cursor += cinfo.output_height * cinfo.output_width * cinfo.output_components - 1; //역순입력
	auto s_imgData = cinfo.image_width * cinfo.output_components;
	
	while (cinfo.output_scanline < cinfo.output_height) {
		auto ret = jpeg_read_scanlines(&cinfo, &imgData, 1);
		//memcpy(cursor, imgData, s_imgData);
		for (auto i = 0; i < s_imgData; i++)
			cursor[-i] = imgData[i];
		cursor -= s_imgData;
	}
	
	ReleaseDC(hdc);
	DeleteDC(hMemDC);
	free(imgData);
	//	BITMAPINFO bitmapInfo;
	//jpeg_create_decompress
	//	bitmapInfo.bmiHeader
	//	CreateDIBSection(GetDC(), )
	//	//MessageBox(currentPath, _T("Notification"), MB_OK);
	//ReleaseDC(dc);
	return 0;
}