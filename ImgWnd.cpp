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
	//���� �ڵ���ü���� TCHAR�� ���� ���߱�
#ifdef UNICODE
	auto fp = _wfopen(currentPath, _T("r"));
#else
	auto fp = fopen(currentPath, _T("r"));
#endif
	if (fp == nullptr) {
		MessageBox(_T("�������� �ʴ� ����"), _T("Notification"), MB_OK);
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
		MessageBox(_T("����� �ջ�� ����"), _T("Notification"), MB_OK);
		return -1;
	}

	if (!jpeg_start_decompress(&cinfo)) {
		MessageBox(_T("���ڵ� ����"), _T("Notification"), MB_OK);
		jpeg_destroy_decompress(&cinfo);
		return -1;
	}
	
	auto imgData = (JSAMPROW)malloc(cinfo.image_width * cinfo.jpeg_color_space);
	if (imgData == nullptr) {
		MessageBox(_T("�޸� ����"), _T("Notification"), MB_OK);
		jpeg_destroy_decompress(&cinfo);
		return -1;
	}

	//cinfo -> BITMAPINFO�� �� ��Ī
	BITMAPINFO bitmapInfo;
	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo);
	bitmapInfo.bmiHeader.biWidth = cinfo.image_width;
	bitmapInfo.bmiHeader.biHeight = cinfo.image_height;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = cinfo.output_components * cinfo.data_precision;	//������Ʈ ���� * ������Ʈ�� ��Ʈ��. ������� RGB * 8��Ʈ�� 24��Ʈ�̹���
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	bitmapInfo.bmiHeader.biSizeImage = 0;
	
	//�÷������̽� ������ �� ����. �ϴ� ����� ���ÿ����� Ȯ���ϰ� �ٸ�Ÿ���� ���� �߰�
	//�������� ���Ϻ��� �� ���ؾ� �׽�Ʈ�� ����
	//switch (cinfo.out_color_space) {
	//case JCS_RGB :
		
	//}

	//bitmapInfo.bmiColors

	char* ppvBits; //��Ʈ���� �Է��� ������
	auto hdc = GetParent().GetDC();
	auto hMemDC = CreateCompatibleDC(hdc); //����ä�����ʰ� �ٸ� DC�� ��� ������ ����
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