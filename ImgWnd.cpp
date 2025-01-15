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
	
	auto imgData = (JSAMPLE *)malloc(cinfo.image_width * cinfo.image_height * 3);// cinfo.jpeg_color_space);
	if (imgData == nullptr) {
		MessageBox(_T("�޸� ����"), _T("Notification"), MB_OK);
		jpeg_destroy_decompress(&cinfo);
		return -1;
	}
	

	//cinfo -> BITMAPINFO�� �� ��Ī
	ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo);
	bitmapInfo.bmiHeader.biWidth = cinfo.image_width;
	bitmapInfo.bmiHeader.biHeight = cinfo.image_height;
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
	cursor += cinfo.output_height * cinfo.output_width * cinfo.output_components - 1; //�����Է�
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