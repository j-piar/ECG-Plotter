#include "StdAfx.h"
#include "XGraphLine.h"
#include "XGRAPH.h"

UINT CXGraphLine::s_nDisplayNumber= 0;
//UINT CXGraphLine::s_nSliceStart= 0;
//UINT CXGraphLine::s_nSliceEnd= 0;


extern wchar_t* char2wchar(const char* str);
extern Color	GetGDIPlusColor(COLORREF color);

CXGraphLine::CXGraphLine(void)
{
	m_nLineType= LINE_NORMAL;

	m_pFont= new CFont();
	if(m_pFont->GetSafeHandle() == NULL)
		m_pFont->CreateFont(14, 5, 0, 0, FW_MEDIUM, FALSE, FALSE, 0, 
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));

	m_nCurrGroup= 0;
	//m_nCurrStart= 0;
	//m_nCurrEnd= 0;
	
}


CXGraphLine::~CXGraphLine(void)
{
	//s_nLineNumber= 0;
	//s_nSliceStart= 0;
	//s_nSliceEnd= 0;
	//delete m_pDataNotation;
}

// line 그리기 [9/30/2013 ChoiJunHyeok]
void	CXGraphLine::Draw(CDCEx* pDC)
{
	CPen samplePen;
	CString str="";

	if(m_nLineType==LINE_NORMAL)
	{
		samplePen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		//str.Format("%d", s_nLineNumber);
	}

	else if(m_nLineType==LINE_MAX)
	{
		samplePen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		str.Format("max");
	}
	else if(m_nLineType==LINE_MIN)
	{
		samplePen.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
		str.Format("min");
	}
	//else if(m_nLineType==LINE_EVAL)
	//{
	//	samplePen.CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	//	str.Format("%d", m_nCurrDisplay);
	//}
	//else if(m_nLineType==LINE_SLICE_START)
	//{
	//	samplePen.CreatePen(PS_SOLID, 1, RGB(255, 0, 255));
	//	str.Format("s%d", m_nCurrStart);
	//}
	//else if(m_nLineType==LINE_SLICE_END)
	//{
	//	samplePen.CreatePen(PS_SOLID, 1, RGB(255, 0, 255));
	//	str.Format("e%d", m_nCurrEnd);
	//}

	TDataPoint* pDataPoint= new TDataPoint();
	pDataPoint->fXVal= m_lfX;
	pDataPoint->fYVal= m_lfY;


	
	int nAxis= m_pGraph->GetCurve(m_nCurve).GetXAxis();
	int nXPos = m_pGraph->GetXAxis (nAxis).GetPointForValue(pDataPoint).x;

	// 그리는 좌표를 유동적으로 변경 [10/2/2013 ChoiJunHyeok]
	m_ptStartDC.x= nXPos;	
	m_ptEndDC.x= nXPos;
	
	
	pDC->SelectObject(samplePen);
	pDC->MoveTo(m_ptStartDC);
	pDC->LineTo(m_ptEndDC);

	//pDC->SelectObject(((CKaiWinApp*)AfxGetApp())->GetCustomFont(0));
	pDC->SelectObject(m_pFont);
	
	int nOldMode= pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(str, CRect(m_ptEndDC.x-65, m_ptEndDC.y-22, m_ptEndDC.x-8, m_ptEndDC.y), DT_RIGHT);
}

// line 그리기 [9/30/2013 ChoiJunHyeok]
void	CXGraphLine::Draw(Graphics& graphics)
{
	Pen* pPen= NULL;
	CString str="";
	wchar_t *pwstr = char2wchar(_T("Arial"));
	Gdiplus::Font font(pwstr,(REAL)9, FW_NORMAL);
	
	if(m_nLineType==LINE_NORMAL)
	{
		pPen= new Pen(Color(255,0,0,0), 1);
		//str.Format("%d", s_nLineNumber);
	}
	else if(m_nLineType==LINE_MAX)
	{
		pPen= new Pen(Color(255,255,0,0), 1);
		str.Format("max");
	}
	else if(m_nLineType==LINE_MIN)
	{
		pPen= new Pen(Color(255,0,0,255), 1);
		str.Format("min");
	}
	//else if(m_nLineType==LINE_EVAL)
	//{
	//	pPen= new Pen(Color(255,0,0,0), 1);
	//	str.Format("%d", m_nCurrDisplay);
	//}
	//else if(m_nLineType==LINE_SLICE_START)
	//{
	//	pPen= new Pen(Color(255,255,0,255), 1);
	//	str.Format("s%d", m_nCurrStart);
	//}
	//else if(m_nLineType==LINE_SLICE_END)
	//{
	//	pPen= new Pen(Color(255,255,0,255), 1);
	//	str.Format("e%d", m_nCurrEnd);
	//}

	TDataPoint* pDataPoint= new TDataPoint();
	pDataPoint->fXVal= m_lfX;
	pDataPoint->fYVal= m_lfY;

	int nAxis= m_pGraph->GetCurve(m_nCurve).GetXAxis();
	int nXPos = m_pGraph->GetXAxis (nAxis).GetPointForValue(pDataPoint).x;


	//if (m_pGraph->GetInnerRect().left > point.x || m_pGraph->GetInnerRect().right < point.x)
	//	return;

	// 좌표 DC꺼 엉뚱하게 수정하고 적용하던 버그 수정 [10/29/2013 ChoiJunHyeok]
	// 그리는 좌표를 유동적으로 변경 [10/2/2013 ChoiJunHyeok]
	m_ptStart.X= nXPos;	
	m_ptEnd.X= nXPos;

	graphics.DrawLine(pPen, m_ptStart, m_ptEnd);

	wchar_t* pwstr2= char2wchar(str.GetBuffer());
	StringFormat format;
	COLORREF crText = RGB(0x11,0x11,0x11);
	format.SetAlignment(StringAlignmentCenter);
	SolidBrush textBrush(GetGDIPlusColor(crText));
	//RectF rc(m_ptEndDC.x-65, m_ptEndDC.y-22, m_ptEndDC.x-8, m_ptEndDC.y);
	RectF rc(m_ptEnd.X-45, m_ptEnd.Y-20, 50, 20);
	graphics.DrawString(pwstr2, str.GetLength(), &font, rc, &format, &textBrush);
	//graphics.DrawString()
	//pDC->SelectObject(((CKaiWinApp*)AfxGetApp())->GetCustomFont(0));
	//pDC->SelectObject(m_pFont);

	//int nOldMode= pDC->SetBkMode(TRANSPARENT);
	//pDC->DrawText(str, CRect(m_ptEndDC.x-65, m_ptEndDC.y-22, m_ptEndDC.x-8, m_ptEndDC.y), DT_RIGHT);

	
}