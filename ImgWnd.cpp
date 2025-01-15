#include "ImgWnd.h"
#include <new>

LRESULT CImgWnd::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) noexcept
	//auto hImg = LoadBitmap(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDB_BITMAP1));
{
	ZeroMemory(currentPath, sizeof(currentPath));
	ShowWindow(SW_SHOW);
	return 0;
}

LRESULT CImgWnd::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DeleteObject(hBitmap);
	PostQuitMessage(0);
	return 0;
}
 
LRESULT CImgWnd::DrawImage(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//MessageBox(_T("ȣ��ȣ��"), _T("Notification"), MB_OK);
	PAINTSTRUCT ps;
	auto hdc = BeginPaint(&ps); //GetDC��� WM_PAINT�ȿ����� ���
	auto hMemDC = CreateCompatibleDC(hdc);
	auto old = SelectObject(hMemDC, hBitmap);
	DeleteObject(old);

	BITMAP bit;
	GetObject(hBitmap, sizeof(BITMAP), &bit);

	BitBlt(hdc, 0, 0,  bit.bmWidth, bit.bmHeight, hMemDC, 0, 0, SRCCOPY);

	DeleteDC(hMemDC);
	EndPaint(&ps);

	return 0;
}

LRESULT CImgWnd::ReadImage(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
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
	
	if (_ReadImage(fp) == 0) {
		//Parent�� Ŭ���̾�Ʈ����� �̹������� ������� ������ Ű��
		auto parent = GetParent();
		RECT rect;
		parent.GetClientRect(&rect);
		
		auto bitmapInfo = GetBitmapInfo();
		auto widthNew = max(bitmapInfo.bmWidth, rect.right);
		auto heightNew = max(bitmapInfo.bmHeight, rect.bottom);

		if (bitmapInfo.bmWidth > rect.right || bitmapInfo.bmHeight > rect.bottom)
			parent.ResizeClient(widthNew, heightNew);
		else //ũ�Ⱑ �Ⱥ��ϸ� WM_SIZE�޽����� ������ �ȵǼ� ���� ����
			parent.PostMessageW(WM_SIZE, 0, MAKELPARAM(widthNew, heightNew));
		
	}
	
	fclose(fp);
	return 0;
}

using jpeg_error_mgr_jmp = struct {
	jpeg_error_mgr pub;
	jmp_buf jmp_buffer;
};

LRESULT CImgWnd::_ReadImage(FILE* fp)
{
	jpeg_decompress_struct cinfo;
	jpeg_error_mgr_jmp jerr;
	

	
	cinfo.err = jpeg_std_error(&jerr.pub);

	//libjpeg�� read�� ������ ����, error_exit�� ȣ���ϰ� ���ܾ��� ���α׷��� �����Ŵ
	//jmp����� ó����� ���µ�
	jerr.pub.error_exit = [](j_common_ptr cinfo) {
		longjmp(reinterpret_cast<jpeg_error_mgr_jmp*>(cinfo->err)->jmp_buffer, 1);
	};
	if (setjmp(jerr.jmp_buffer)) {
		MessageBox(_T("�������� �ʴ� ����"), _T("Notification"), MB_OK);
		return -1;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, fp);

	if (!jpeg_read_header(&cinfo, true)) { //return 0 == SUSPENDED
		MessageBox(_T("����� �ջ�� ����"), _T("Notification"), MB_OK);
		return -1;
	}
	cinfo.out_color_space = JCS_RGB; //RGB ����

	if (!jpeg_start_decompress(&cinfo)) {
		MessageBox(_T("���ڵ� ����"), _T("Notification"), MB_OK);
		jpeg_destroy_decompress(&cinfo);
		return -1;
	}
	auto ret = _WriteBitmap(cinfo);
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	return ret;
}


LRESULT	CImgWnd::_WriteBitmap(jpeg_decompress_struct& cinfo)
{
	auto size_imgData = cinfo.output_width * cinfo.output_components;
	auto imgData = (JSAMPLE *)malloc(size_imgData);// cinfo.jpeg_color_space);
	if (imgData == nullptr) {
		MessageBox(_T("�޸� ����"), _T("Notification"), MB_OK);
		return -1;
	}
	
	//cinfo -> BITMAPINFO�� �� ��Ī
	BITMAPINFO bitmapInfo;
	ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo);
	bitmapInfo.bmiHeader.biWidth = cinfo.output_width;
	bitmapInfo.bmiHeader.biHeight = cinfo.output_height;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = cinfo.output_components * cinfo.data_precision;	//������Ʈ ���� * ������Ʈ�� ��Ʈ��. ������� RGB * 8��Ʈ�� 24��Ʈ�̹���
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	void* ppvBits; //��Ʈ���� �Է��� ������
	auto hdc = GetDC();
	hBitmap = CreateDIBSection(hdc, &bitmapInfo, DIB_RGB_COLORS, &ppvBits, NULL, 0);
	
	if(setjmp(reinterpret_cast<jpeg_error_mgr_jmp*>(cinfo.err)->jmp_buffer)) {
		if (imgData != nullptr) free(imgData);
		ReleaseDC(hdc);
		MessageBox(_T("�������� �ʴ� ����"), _T("Notification"), MB_OK);
		return -1;
	}

	auto cursor = reinterpret_cast<BYTE*>(ppvBits);
	cursor += cinfo.output_height * size_imgData - 1; //���� �����Է��� ����, ���� ������ ����
	
	while (cinfo.output_scanline < cinfo.output_height) {
		auto ret = jpeg_read_scanlines(&cinfo, &imgData, 1);
		if (ret < 1) {
			MessageBox(_T("�ջ�� ����"), _T("Notification"), MB_OK);
			ReleaseDC(hdc);
			free(imgData);
			return -1;
		}
		auto& size_color = cinfo.output_components;
		for (int pixel = 0; pixel < size_imgData; pixel+=size_color) //���δ� �����Է�, ���δ� ���������Է�, �ȼ� �� ������ �����Է��ؾ� �ȹٷγ���
			for (int color = 0; color < size_color; color++)
				cursor[pixel+(size_color - color) - (int)size_imgData] = imgData[pixel + color];
		cursor -= size_imgData;
	}
	ReleaseDC(hdc);
	free(imgData);
	
	return 0;
}

const BITMAP CImgWnd::GetBitmapInfo() const noexcept
{
	BITMAP bitmapInfo;
	if(!GetObject(hBitmap, sizeof(BITMAP), &bitmapInfo))
		ZeroMemory(&bitmapInfo, sizeof(bitmapInfo));
	return bitmapInfo;
}