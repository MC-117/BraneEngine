#pragma once
#ifndef _LOADINGUI_H_
#define _LOADINGUI_H_

#define _AFXDLL
#include <afxdlgs.h>
#include <atlimage.h>
#include <thread>

#define DLG_TEMPLATE_BUFFER_MAX_LENGT 1024
class CDlgTemplate
{
public:
	CDlgTemplate(int nBufferLength = DLG_TEMPLATE_BUFFER_MAX_LENGT);
	~CDlgTemplate();
private:
	WORD* m_pBuffer;
public:
	DLGTEMPLATE* CreateTemplate(DWORD dwStyle, CRect& rect, CStringW strCaption, DWORD dwStyleEx = 0);
};

class LoadingUI : public CDialog
{
	DECLARE_DYNAMIC(LoadingUI)
public:
	CDlgTemplate m_dlgTemp;
	CImage img;

	CString text = "";

	LoadingUI(CString path);
	BOOL Create(CStringW strCaption, BOOL bMode, CWnd* pParent);

	INT_PTR DoModal();

	void SetText(CStringW text);
	void Close();

	static void threadFunc(LoadingUI* ui);
protected:

	HICON  m_hIcon;
	enum { IDICON = 1 };
	bool updateText = true;
	bool closing = false;
	CFont new_font;

	virtual BOOL OnInitDialog();
	virtual BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnPaint();
	virtual void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_MESSAGE_MAP()
};

#endif // !_LOADINGUI_H_
