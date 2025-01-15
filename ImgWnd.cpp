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
	DeleteObject(hBitmap);
	PostQuitMessage(0);
	return 0;
}

LRESULT CImgWnd::DrawImage(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/) noexcept
{
	//MessageBox(_T("ȣ��ȣ��"), _T("Notification"), MB_OK);
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
	//���� �ڵ���ü���� TCHAR�� ���� ���߱�
	//�����ڵ� ������, ���ԵǸ� �����Һκ��� ���� �ֱ� ��
#ifdef UNICODE
	auto fp = _wfopen(currentPath, _T("rb"));
#else
	auto fp = fopen(currentPath, _T("rb"));
#endif
	if (fp == nullptr) {
		MessageBox(_T("�������� �ʴ� ����"), _T("Notification"), MB_OK);
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
		MessageBox(_T("����� �ջ�� ����"), _T("Notification"), MB_OK);
		return -1;
	}

	if (!jpeg_start_decompress(&cinfo)) {
		MessageBox(_T("���ڵ� ����"), _T("Notification"), MB_OK);
		jpeg_destroy_decompress(&cinfo);
		return -1;
	}

	auto size_imgData = cinfo.output_width * cinfo.output_components;
	auto imgData = (JSAMPLE *)malloc(size_imgData);// cinfo.jpeg_color_space);
	if (imgData == nullptr) {
		MessageBox(_T("�޸� ����"), _T("Notification"), MB_OK);
		jpeg_destroy_decompress(&cinfo);
		return -1;
	}
	

	//cinfo -> BITMAPINFO�� �� ��Ī
	ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo);
	bitmapInfo.bmiHeader.biWidth = cinfo.output_width;
	bitmapInfo.bmiHeader.biHeight = cinfo.output_height;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = cinfo.output_components * cinfo.data_precision;	//������Ʈ ���� * ������Ʈ�� ��Ʈ��. ������� RGB * 8��Ʈ�� 24��Ʈ�̹���
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	//bitmapInfo.bmiHeader.biSizeImage = 5760*1080;
	bitmapInfo.bmiHeader.biYPelsPerMeter = 1;
	bitmapInfo.bmiHeader.biXPelsPerMeter = 1;
	
	
	//�÷������̽� ������ �� ����. �ϴ� ����� ���ÿ����� Ȯ���ϰ� �ٸ�Ÿ���� ���� �߰�
	//�������� ���Ϻ��� �� ���ؾ� �׽�Ʈ�� ����
	//switch (cinfo.out_color_space) {
	//case JCS_RGB :
		
	//}

	//bitmapInfo.bmiColors

	void* ppvBits; //��Ʈ���� �Է��� ������
	auto hdc = GetDC();
	auto hMemDC = CreateCompatibleDC(hdc); //����ä�����ʰ� �ٸ� DC�� ������� ��ü
	hBitmap = CreateDIBSection(hdc, &bitmapInfo, DIB_RGB_COLORS, &ppvBits, NULL, 0);

	auto cursor = reinterpret_cast<BYTE*>(ppvBits);
	cursor += cinfo.output_height * size_imgData - 1; //���� �����Է��� ����, ���� ������ ����
	
	while (cinfo.output_scanline < cinfo.output_height) {
		auto ret = jpeg_read_scanlines(&cinfo, &imgData, 1);
		auto& size_color = cinfo.output_components;
		for (int pixel = 0; pixel < size_imgData; pixel+=size_color) //���δ� �����Է�, ���δ� ���������Է�, �ȼ� �� ������ �����Է��ؾ� �ȹٷγ���
			for (int color = 0; color < size_color; color++)
				cursor[pixel+(size_color - color) - (int)size_imgData] = imgData[pixel + color];
		cursor -= size_imgData;
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