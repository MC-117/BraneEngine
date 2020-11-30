#include "LoadingUI.h"
#include <afxpriv.h>

CDlgTemplate::CDlgTemplate(int nBufferLength)
{
	m_pBuffer = new WORD[nBufferLength];
	if ((!m_pBuffer) || (DLG_TEMPLATE_BUFFER_MAX_LENGT > nBufferLength))
	{
		throw 1;
	}
}

CDlgTemplate::~CDlgTemplate()
{
	if (m_pBuffer)
	{
		delete m_pBuffer;
		m_pBuffer = NULL;
	}
}

DLGTEMPLATE* CDlgTemplate::CreateTemplate(DWORD dwStyle, CRect& rect, CStringW strCaption, DWORD dwStyleEx)
{
	WORD* pTemp = m_pBuffer;
	DLGTEMPLATE* pDlgTemp = (DLGTEMPLATE*)pTemp;
	// 对话框模版
	pDlgTemp->style = dwStyle;
	pDlgTemp->dwExtendedStyle = dwStyleEx;
	pDlgTemp->cdit = 0;
	pDlgTemp->cx = rect.Width();
	pDlgTemp->cy = rect.Height();
	pDlgTemp->x = (short)rect.left;
	pDlgTemp->y = (short)rect.top;
	pTemp = (WORD*)(pDlgTemp + 1);
	// 菜单
	*pTemp++ = 0;
	// class
	*pTemp++ = 0;
	// caption
	wcscpy((WCHAR*)pTemp, strCaption);
	pTemp += strCaption.GetLength() + 1;
	// font
	return pDlgTemp;
}

void GetError(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code  

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process  

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
#ifdef UNICODE
	swprintf_s((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
#else
	sprintf_s((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
#endif // UNICODE

	
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

IMPLEMENT_DYNAMIC(LoadingUI, CCommonDialog)

BEGIN_MESSAGE_MAP(LoadingUI, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

LoadingUI::LoadingUI(CString path)
{
	m_hIcon = AfxGetApp()->LoadStandardIcon(IDI_APPLICATION);
	img.Load(path);
	if (img.IsNull())
		throw "Fail to load Img";
}

BOOL LoadingUI::Create(CStringW strCaption, BOOL bMode, CWnd * pParent)
{
	BOOL bResult = FALSE;
	DLGTEMPLATE* pTemp = NULL;
	CWnd* dk = GetDesktopWindow();
	CRect rect;
	dk->GetWindowRect(&rect);
	CPoint p = rect.CenterPoint();
	int w = img.GetWidth(), h = img.GetHeight();
	CRect r((p.x - w) / 4, (p.y - h) / 4, (p.x + w) / 4, (p.y + h) / 4);
	pTemp = m_dlgTemp.CreateTemplate(WS_POPUPWINDOW, r, strCaption);
	if (bMode)
	{
		bResult = InitModalIndirect(pTemp, pParent);
	}
	else
	{
		bResult = CreateDlgIndirect(pTemp, pParent, AfxGetInstanceHandle());
	}
	return bResult;
}

INT_PTR LoadingUI::DoModal()
{
	HWND hwnd = PreModal();
	DLGTEMPLATE* pTemp = NULL;
	CWnd* dk = GetDesktopWindow();
	CRect rect;
	dk->GetWindowRect(&rect);
	CPoint p = rect.CenterPoint();
	int w = img.GetWidth(), h = img.GetHeight();
	CRect r((p.x - w) / 4, (p.y - h) / 4, (p.x + w) / 4, (p.y + h) / 4);
	pTemp = m_dlgTemp.CreateTemplate(WS_POPUPWINDOW, r, L"Loading");
	if (closing)
		return IDOK;
	if (CreateDlgIndirect(pTemp, NULL, AfxGetInstanceHandle())) {
		DWORD dwFlags = MLF_SHOWONIDLE;
		if (GetStyle() & DS_NOIDLEMSG)
			dwFlags |= MLF_NOIDLEMSG;
		VERIFY(RunModalLoop(dwFlags) == m_nModalResult);
	}
	DestroyWindow();
	PostModal();
	return IDOK;
}

void LoadingUI::SetText(CString text)
{
	if (this->text == text)
		return;
	this->text = text;
	CRect cr;
	GetClientRect(cr);
	CRect rectText(0, cr.Height() - 25, cr.Width(), cr.Height());
	InvalidateRect(&rectText);
	updateText = true;
}

void LoadingUI::Close()
{
	closing = true;
	EndDialog(IDOK);
}

void LoadingUI::threadFunc(LoadingUI * ui)
{
	ui->DoModal();
}

BOOL LoadingUI::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (m_hIcon)
	{
		SetIcon(m_hIcon, TRUE);   // 设置大图标
		SetIcon(m_hIcon, FALSE);  // 设置小图标
	}
	CDC* pdc = GetDC();
	if (pdc != NULL) {
		CDC& dc = *pdc;
		VERIFY(new_font.CreatePointFont(100, _T("Aerial"), &dc));
	}
	AddFontResourceExA("Engine/Fonts/ChakraPetch-Light.ttf", FR_PRIVATE, 0);
	return TRUE;
}

BOOL LoadingUI::OnEraseBkgnd(CDC * pDC)
{
	if (pDC == NULL)
		return 1;
	CDC &dc = *pDC;
	CRect rectImg;
	GetClientRect(rectImg);
	img.Draw(dc.m_hDC, rectImg, Gdiplus::InterpolationMode::InterpolationModeHighQuality);
	return 1;
}

void LoadingUI::OnPaint()
{
	if (IsIconic()) {

	}
	else {
		CDC* pdc = GetDC();
		if (pdc == NULL)
			return;
		CDC& dc = *pdc;
		CRect cr;
		GetClientRect(cr);
		/*CFont new_font;
		VERIFY(new_font.CreatePointFont(100, _T("Aerial"), &dc));*/
		CFont* default_font = dc.SelectObject(&new_font);
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(RGB(250, 250, 250));

		CRect rectText(0, cr.Height() - 25, cr.Width(), cr.Height());
		dc.DrawText(text, rectText, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
		dc.SelectObject(default_font);
		//new_font.DeleteObject();
		updateText = false;
	}
}

void LoadingUI::OnShowWindow(BOOL bShow, UINT nStatus)
{
	/*if (bShow) {
		SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}*/
}
