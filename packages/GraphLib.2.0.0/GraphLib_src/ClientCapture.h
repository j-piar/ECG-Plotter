
#pragma once 

//#if _MSC_VER >= 1000
//#pragma once
//#endif // _MSC_VER >= 1000


class CClientCapture 
{
public:
	CClientCapture();
	~CClientCapture();

public:
	void		Release();
	BOOL		WriteDIB( CString csFile);
	void		OnDraw(HDC hDC, CRect rcRect, CRect rect);
	BOOL		Paint(HDC hDC, CPalette *pal, LPRECT lpDCRect, LPRECT lpDIBRect) const;
	void		Capture(CDC *dc, CRect rectDIB);

private:
	HANDLE hDIB;
	
	DWORD	 Height() const;
	BOOL		 WriteWindowToDIB( HDC hDC, CDC* dc, CRect rect);
	HANDLE	 DDBToDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* pPal, CDC* dc);

	BOOL		WriteWindowToDIB(CDC* dc, CRect rect);

	LPBITMAPINFO m_pBMI;
	LPBYTE 		   m_pBits;
	CPalette pal;
	

};




