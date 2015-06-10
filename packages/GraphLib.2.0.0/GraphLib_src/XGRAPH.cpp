// XGRAPH.cpp : Definiert den Einsprungpunkt f? die DLL-Anwendung.
//

#include "stdafx.h"
#include "XGRAPH.h"
#include "XGraphLabel.h"
#ifndef _WIN32_WCE
#include "AxisDlg.h"
#include "ChartPage.h"
#include "CurveDlg.h"
#include "ChartDlg.h"
#endif
#include <afxpriv.h>

#include "float.h"
#include "GfxUtils.h"
#include "math.h"
#include "MemDC.h"

#include "WinGDI.h"
#include "XGraphData.h"
#include "..\KaiWin\MainFrm.h"
#include "..\KaiWinSubStandard\SubStandardView.h"
#include "..\KaiWinSubStandard\StandardRightView.h"
#include "XGraphShowOptionDlg.h"
#include "..\KaiWinCommon\FlightData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// XGraph.cpp: Implementation


wchar_t* char2wchar(const char* str)
{
    ASSERT(str);
    size_t nlen = strlen(str)+1;
    wchar_t* pdstr =(LPWSTR)malloc(sizeof(wchar_t)* nlen);    
    
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,  str, -1, pdstr, nlen);
	//mbstowcs(pdstr, str, nlen);

    return pdstr;
};
//
Color	GetGDIPlusColor(COLORREF color)
{
	Color result(GetRValue(color),GetGValue(color),GetBValue(color));
	return result;
}



/////////////////////////////////////////////////////////////////////////////
// CXGraph

//IMPLEMENT_SERIAL( CXGraph, CWnd, 1 )
  
#pragma warning (disable : 4244)
#pragma warning (disable : 4800)

UINT CXGraph::s_ID= 15000;

CXGraph::NM_POINTMOVING CXGraph::m_nmCurrentPointMoving;
	
CXGraph::CXGraph()
{
	WNDCLASS wndcls;

	m_nSelectedMarker= -1;

	m_nSelMode= GRAPH_MODE_SELECT;

	m_bSliceStartTurn= TRUE;	// 슬라이스 순서 [10/7/2013 ChoiJunHyeok]
	m_nSliceNum= 0;

	m_nSnappedCurve= -1;	// snap curve 초기화 [11/7/2013 ChoiJunHyeok]

	m_pFrame= (CMainFrame*)AfxGetMainWnd();

	// setup drawing function pointers
	m_pDrawFn[0] = DrawRect;
	m_pDrawFn[1] = DrawCircle;
	m_pDrawFn[2] = DrawCross;
	m_pDrawFn[3] = DrawRhombus;
	m_pDrawFn[4] = DrawLeftTriangle;
	m_pDrawFn[5] = DrawUpTriangle;
	m_pDrawFn[6] = DrawRightTriangle;
	m_pDrawFn[7] = DrawDownTriangle;

	m_pDrawGDIFn[0] = DrawRectGDI;
	m_pDrawGDIFn[1] = DrawCircleGDI;
	m_pDrawGDIFn[2] = DrawCrossGDI;
	m_pDrawGDIFn[3] = DrawRhombusGDI;
	m_pDrawGDIFn[4] = DrawLeftTriangleGDI;
	m_pDrawGDIFn[5] = DrawUpTriangleGDI;
	m_pDrawGDIFn[6] = DrawRightTriangleGDI;
	m_pDrawGDIFn[7] = DrawDownTriangleGDI;
	
    HINSTANCE hInst   = AfxGetInstanceHandle();
	m_crBackColor     = RGB(255,255,255);
	m_crGraphColor    = RGB(255,255,255);
	m_crInnerColor    = RGB(240,240,240);
	m_nLeftMargin     = 
	m_nTopMargin      = 
	m_nRightMargin    = 5;
	m_nBottomMargin   = 20;
	m_LegendAlignment = right;
	m_bLButtonDown    = false;
	m_bRButtonDown    = false;
	m_bObjectSelected = false;
	m_bTmpDC		  = false;
	m_bDoubleBuffer   = true;
	m_bShowLegend     = true;
	m_bInteraction    = true;
	m_bDataPointMoving= false;
	m_nSelectedSerie  = -1;
	m_pTracker        = NULL;
	m_pCurrentObject  = NULL;
	m_pPrintDC        = NULL;
	m_pDrawDC         = NULL;
	m_nAxisSpace      = 5;
	m_nCursorFlags    = XGC_LEGEND | XGC_VERT | XGC_HORIZ;
	m_OldPoint        = CPoint(0,0);
	m_opOperation	  = opNone;
	m_bSnapCursor     = true;
	m_nSnapRange      = 10;
	m_cPrintHeader    = _T("");
	m_cPrintFooter    = _T("");
	m_pCurrentObject  = NULL;
	m_nForcedSnapCurve = -1;
	m_nSnappedCurve1   = 0;
	m_fCurrentEditValue = 0.0;
	
	m_OldCursorRect.SetRectEmpty();

	// Create one legend for the cursor
	m_CursorLabel.m_bVisible = false;
	m_CursorLabel.m_bCanEdit = false;
	m_CursorLabel.m_bBorder  = true;
	m_CursorLabel.m_clRect.SetRect(100,100,200,175);

	m_Objects.AddHead (&m_CursorLabel);

    // Register window class
    if (!(::GetClassInfo(hInst, _T("XGraph"), &wndcls)))
    {
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = 0;
		wndcls.cbWndExtra		= 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = _T("XGraph");

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return ;
        }
    }
}

CXGraph::~CXGraph()
{
	ResetAll();
}



BEGIN_MESSAGE_MAP(CXGraph, CWnd)
	//{{AFX_MSG_MAP(CXGraph)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
#ifndef _WIN32_WCE
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
#endif
	ON_WM_KEYUP()
	ON_COMMAND(IDM_ZOOM, Zoom )
	ON_COMMAND(IDM_PAN, Pan )
	ON_COMMAND(IDM_CURSOR, Cursor )
	ON_COMMAND(IDM_SELECT, NoOp )
	ON_COMMAND(IDM_RESET, ResetZoom )
	ON_COMMAND(IDM_INSERTLABEL, InsertEmptyLabel)
	ON_COMMAND(IDM_PROPERTIES, OnProperties)
#ifndef _WIN32_WCE
	ON_COMMAND(IDM_PRINT, OnPrint)
#endif
	ON_COMMAND(IDM_LINEARTREND, LinearTrend)
	ON_COMMAND(IDM_CUBICTREND, CubicTrend)
	ON_COMMAND(IDM_PARENTCALLBACK, ParentCallback)
	ON_COMMAND(IDM_ZOOM_BACK, RestoreLastZoom)
	ON_COMMAND(IDM_EDITCURVE, Edit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


bool CXGraph::DeleteXAxis(int nAxis)
{
	if (nAxis < 0 || nAxis >= static_cast<int>(m_XAxis.size()))
		return false;

	m_XAxis.erase (m_XAxis.begin()+nAxis);
	
	return true;
}

bool CXGraph::DeleteYAxis(int nAxis)
{
	if (nAxis < 0 || nAxis >= static_cast<int>(m_YAxis.size()))
		return false;

	m_YAxis.erase (m_YAxis.begin()+nAxis);
	
	return true;
}

bool CXGraph::DeleteCurve(int nCurve)
{
	if (nCurve < 0 || nCurve >= static_cast<int>(m_vSeries.size()))
		return false;
	
	m_vSeries.erase (m_vSeries.begin()+nCurve);

	return true;
}

CXGraphDataSerie& CXGraph::SetCurveData(TDataPoint* pValues, long nCount, int nCurve, int nXAxis, int nYAxis, bool bAutoDelete, bool bAutoScale)
{
	ASSERT (nCurve <= static_cast<int>(m_vSeries.size()));
	ASSERT (nXAxis <= static_cast<int>(m_XAxis.size()));
	ASSERT (nYAxis <= static_cast<int>(m_YAxis.size()));

			
	if (pValues == NULL || nCount == 0)
		return (CXGraphDataSerie&)*((CXGraphDataSerie*)NULL);


	if (nCurve == m_vSeries.size ())
	{
		// New data serie

		CXGraphDataSerie serie;
		
		serie.m_bAutoDelete = bAutoDelete;
		serie.m_nXAxis		= nXAxis;
		serie.m_nYAxis		= nYAxis;
		serie.m_nCount		= nCount;
		serie.m_nIndex      = nCurve;
		
		m_strPlotX= m_strPlotX.Trim();
		// time, index 아니면 scatter로 띄운다. [10/29/2013 ChoiJunHyeok]
		if(		(m_strPlotX.CompareNoCase("IRIG TIME")==0)
			||  (m_strPlotX.CompareNoCase("IRIG_TIME")==0)
			||  (m_strPlotX.CompareNoCase("IRIG-TIME")==0)
			||  (m_strPlotX.CompareNoCase("Time [sec]")==0)
			||  (m_strPlotX.CompareNoCase("Index")==0)
			)
		{
			serie.SetType(CXGraphDataSerie::EGraphType::gtLine);
		}
		else
		{
			serie.SetType(CXGraphDataSerie::EGraphType::gtScatter);
		}

		if (bAutoDelete)
		{
			serie.m_vPointData = (TDataPoint*) new TDataPoint[nCount];
			memcpy(serie.m_vPointData, pValues, sizeof(TDataPoint) * nCount);
		}
		else
			serie.m_vPointData = pValues;
		
		serie.m_crColor = BASECOLORTABLE[m_vSeries.size () % 12];

		m_vSeries.push_back (serie);
	}
	else
	{
		// existing data serie, just update

		m_vSeries[nCurve].m_nXAxis = nXAxis;
		m_vSeries[nCurve].m_nYAxis = nYAxis;
		m_vSeries[nCurve].m_vPointData  = pValues;
		m_vSeries[nCurve].m_nCount = nCount;
		m_vSeries[nCurve].m_bAutoDelete = bAutoDelete;
	}

	// Need additional X-Axis
	if (nXAxis == m_XAxis.size())
	{
		CXGraphAxis axis;
		axis.m_pGraph   = this;
		axis.m_AxisKind = CXGraphAxis::xAxis;
		m_XAxis.push_back (axis);
	}
	
	// Need additional Y-Axis
	if (nYAxis == m_YAxis.size())
	{
		CXGraphAxis axis;
		axis.m_pGraph   = this;
		axis.m_AxisKind = CXGraphAxis::yAxis;
		m_YAxis.push_back (axis);
	}

	m_vSeries[nCurve].m_pGraph = this;
	
	if(bAutoScale)	// 스케일 재조정 [11/25/2013 ChoiJunHyeok]
		Autoscale(nXAxis, nYAxis, nCurve);
		
	Invalidate(TRUE);
		
	return m_vSeries[nCurve];
}


void CXGraph::Autoscale(int nXAxis, int nYAxis, int nCurve)
{
	// Get min/max for data serie
	double fxMinRet = 1e30;
	double fxMaxRet = -1e30;
	double fyMinRet = 1e30;
	double fyMaxRet = -1e30;


	for(int i = 0; i < GetCurveCount(); i++)
	{
		double fxMin = 1e30;
		double fxMax = -1e30;
		double fyMin = 1e30;
		double fyMax = -1e30;

		GetMinMaxForData(m_vSeries[i], fxMin, fxMax, fyMin, fyMax);

		if(m_vSeries[i].m_nXAxis == nXAxis)
		{
			fxMinRet = min(fxMin, fxMinRet);
			fxMaxRet = max(fxMax, fxMaxRet);
		}

		if(m_vSeries[i].m_nYAxis == nYAxis)
		{
			fyMinRet = min(fyMin, fyMinRet);
			fyMaxRet = max(fyMax, fyMaxRet);
		}
	}

	if(GetCurveCount() == 0)
	{
		fxMinRet = 0; fyMinRet = 0;
		fxMaxRet = 1; fyMaxRet = 1;
	}

	double fMargin = (fyMaxRet - fyMinRet) * 0.05;
	fyMaxRet += fMargin;
	fyMinRet -= fMargin;

	VERIFY(m_XAxis[nXAxis].SetRange(fxMinRet, fxMaxRet));
	VERIFY(m_YAxis[nYAxis].SetRange(fyMinRet, fyMaxRet));

	/*GetMinMaxForData(m_vSeries[nCurve], fxMin, 
									 fxMax,
									 fyMin, 
									 fyMax);

	// Set axis ranges
	VERIFY(m_XAxis[nXAxis].SetRange(fxMin, fxMax));
	VERIFY(m_YAxis[nYAxis].SetRange(fyMin, fyMax));
	*/
}


CXGraphAxis& CXGraph::GetXAxis(int nAxis)
{
	ASSERT(nAxis >= 0 && nAxis < static_cast<int>(m_XAxis.size()));
	return m_XAxis[nAxis]; 
}

CXGraphAxis& CXGraph::GetYAxis(int nAxis)
{
	ASSERT(nAxis >= 0 && nAxis < static_cast<int>(m_YAxis.size()));
	return m_YAxis[nAxis]; 
}

CXGraphDataSerie& CXGraph::GetCurve(int nCurve)
{
	ASSERT(nCurve >= 0 && nCurve < static_cast<int>(m_vSeries.size()));
	return m_vSeries[nCurve];
}

TDataPoint CXGraph::GetCursorAbsolute(int nCurve)
{
	TDataPoint vPoint = { 0.0, 0.0};
	long     nIndex;

	vPoint.fXVal = m_XAxis[m_vSeries[nCurve].m_nXAxis].GetValueForPos (m_CurrentPoint.x);

	double fSnappedXVal = m_XAxis[m_vSeries[nCurve].m_nXAxis].GetValueForPos (m_CurrentPoint.x);
	// Find index for this value
	m_XAxis[m_vSeries[nCurve].m_nXAxis].GetIndexByXVal(nIndex, fSnappedXVal, nCurve);
	// get yval for index
	vPoint.fYVal = m_vSeries[nCurve].m_vPointData[nIndex].fYVal;

	return vPoint;
}

void CXGraph::SetCursorAbsolute(int nCurve, TDataPoint vPoint, bool bForceVisible )
{
	long nIndex;

	m_CurrentPoint = m_XAxis[m_vSeries[nCurve].m_nXAxis].GetPointForValue (&vPoint);

	m_XAxis[m_vSeries[nCurve].m_nXAxis].GetIndexByXVal(nIndex, vPoint.fXVal, nCurve);
	vPoint.fYVal = m_vSeries[nCurve].m_vPointData[nIndex].fYVal;


	if (m_nCursorFlags & XGC_ADJUSTSMOOTH && bForceVisible)
	{
		double fRange = m_XAxis[m_vSeries[nCurve].m_nXAxis].m_fCurMax - m_XAxis[m_vSeries[nCurve].m_nXAxis].m_fCurMin;
		m_XAxis[m_vSeries[nCurve].m_nXAxis].SetCurrentRange(vPoint.fXVal -  fRange / 2, vPoint.fXVal + fRange / 2);
	}
	else
	if ( (vPoint.fXVal < m_XAxis[m_vSeries[nCurve].m_nXAxis].m_fCurMin || 
		  vPoint.fXVal > m_XAxis[m_vSeries[nCurve].m_nXAxis].m_fCurMax ) &&
		  bForceVisible)
	{
		double fRange = m_XAxis[m_vSeries[nCurve].m_nXAxis].m_fCurMax - m_XAxis[m_vSeries[nCurve].m_nXAxis].m_fCurMin;
		m_XAxis[m_vSeries[nCurve].m_nXAxis].SetCurrentRange(vPoint.fXVal - (fRange / 2.0), vPoint.fXVal + (fRange / 2.0));
	}

	if (m_nCursorFlags & XGC_ADJUSTSMOOTH && bForceVisible)
	{
		double fRange = m_YAxis[m_vSeries[nCurve].m_nYAxis].m_fCurMax - m_YAxis[m_vSeries[nCurve].m_nYAxis].m_fCurMin;
		m_YAxis[m_vSeries[nCurve].m_nYAxis].SetCurrentRange(vPoint.fYVal - (fRange / 2.0), vPoint.fYVal + (fRange / 2.0));
	}
	else
	if ( (vPoint.fYVal < m_YAxis[m_vSeries[nCurve].m_nYAxis].m_fCurMin || 
		  vPoint.fYVal > m_YAxis[m_vSeries[nCurve].m_nYAxis].m_fCurMax ) &&
		  bForceVisible)
	{
		double fRange = m_YAxis[m_vSeries[nCurve].m_nYAxis].m_fCurMax - m_YAxis[m_vSeries[nCurve].m_nYAxis].m_fCurMin;
		m_YAxis[m_vSeries[nCurve].m_nYAxis].SetCurrentRange(vPoint.fYVal - (fRange / 2.0), vPoint.fYVal + (fRange / 2.0));
	}

	Invalidate();

}

void CXGraph::GetMinMaxForData (CXGraphDataSerie& serie, double& fXMin, double& fXMax, double& fYMin, double& fYMax)
{	
	// check mins and maxs
	for (long i = 0; i < serie.m_nCount; i++)
	{
		if (serie.m_vPointData[i].fXVal > fXMax)
			fXMax = serie.m_vPointData[i].fXVal;

		if (serie.m_vPointData[i].fXVal < fXMin)
			fXMin = serie.m_vPointData[i].fXVal;

		if (serie.m_vPointData[i].fYVal > fYMax)
			fYMax = serie.m_vPointData[i].fYVal;

		if (serie.m_vPointData[i].fYVal < fYMin)
			fYMin = serie.m_vPointData[i].fYVal;
	}
}

// Returns the index for the given axis
int CXGraph::GetAxisIndex(CXGraphAxis* pAxis)
{
	for (int x = 0; x < static_cast<int>(m_XAxis.size()); x++)
		if (pAxis == &m_XAxis[x])
			return x;
	
	for (int y = 0; y < static_cast<int>(m_YAxis.size()); y++)
		if (pAxis == &m_YAxis[y])
			return y;

	return -1;
}

// Returns the index for the given curve
int CXGraph::GetCurveIndex(CXGraphDataSerie* pSerie)
{
	for (int i = 0; i < static_cast<int>(m_vSeries.size()); i++)
		if (pSerie == &m_vSeries[i])
			return i;

	return -1;
}

void CXGraph::SetGraphMargins(int nLeft, int nTop, int nRight, int nBottom)
{
	m_nLeftMargin   = nLeft;
	m_nTopMargin    = nTop;
	m_nRightMargin  = nRight;
	m_nBottomMargin = nBottom;
}


void CXGraph::DrawZoom(Graphics &graphics)
{
	CRect zoomRect(m_MouseDownPoint.x, m_MouseDownPoint.y, m_CurrentPoint.x, m_CurrentPoint.y);

	zoomRect.NormalizeRect ();

	zoomRect.left = max(zoomRect.left, m_clInnerRect.left);
	zoomRect.top = max(zoomRect.top, m_clInnerRect.top);
	zoomRect.right = min(zoomRect.right, m_clInnerRect.right);
	zoomRect.bottom = min(zoomRect.bottom, m_clInnerRect.bottom);
	// If not in doublebuffer mode and zoomrect is not empty delete old zoomrect
	Pen pen(Color(0,0,0),0.5);
	pen.SetDashStyle(DashStyleDot);

	if (!m_clCurrentZoom.IsRectEmpty ())
		graphics.DrawRectangle(&pen,RectF(m_clCurrentZoom.left,m_clCurrentZoom.top,m_clCurrentZoom.Width(),m_clCurrentZoom.Height()));

	m_clCurrentZoom = zoomRect;
	//	pDC->DrawFocusRect(m_clCurrentZoom);
	graphics.DrawRectangle(&pen,RectF(m_clCurrentZoom.left,m_clCurrentZoom.top,
		m_clCurrentZoom.Width(),m_clCurrentZoom.Height()));
}

void CXGraph::DrawZoom (CDCEx* pDC)
{
	CRect zoomRect(m_MouseDownPoint.x, m_MouseDownPoint.y, m_CurrentPoint.x, m_CurrentPoint.y);

	zoomRect.NormalizeRect ();

	zoomRect.left = max(zoomRect.left, m_clInnerRect.left);
	zoomRect.top = max(zoomRect.top, m_clInnerRect.top);
	zoomRect.right = min(zoomRect.right, m_clInnerRect.right);
	zoomRect.bottom = min(zoomRect.bottom, m_clInnerRect.bottom);


	// If not in doublebuffer mode and zoomrect is not empty delete old zoomrect
	if (!m_bDoubleBuffer && !m_clCurrentZoom.IsRectEmpty ())
		pDC->DrawFocusRect (m_clCurrentZoom);

	m_clCurrentZoom = zoomRect;

	if (m_bDoubleBuffer)
	{
		// In db-mode we use a semi-transparent zoomrect ...
		CBrushSelector bs(RGB(150,150,255), pDC);
		COLORREF crOldBkColor = pDC->SetBkColor (m_crInnerColor); 
		int	nOldROP2 = pDC->SetROP2 (R2_NOTXORPEN);

		pDC->Rectangle(zoomRect);
		pDC->SetBkColor (crOldBkColor);
		pDC->SetROP2 (nOldROP2);
		
	}
	
	pDC->DrawFocusRect (m_clCurrentZoom);
	
}

void CXGraph::AddSliceStart()
{
	for (size_t i = 0; i < m_XAxis.size(); i++)
	{
		double fXValue = m_XAxis[i].GetValueForPos (m_CurrentPoint.x);
		double fYValue = m_YAxis[i].GetValueForPos (m_CurrentPoint.y);

		m_XAxis[i].SetAxisMarker(m_nSnappedCurve, m_XAxis[i].m_vAxisMarkers.size(), fXValue, fYValue, RGB(255,0,255), LINE_SLICE_START);
	}

	Invalidate(TRUE);
}

void CXGraph::AddSliceStart(CPoint _CurrPoint)
{
	for (size_t i = 0; i < m_XAxis.size(); i++)
	{
		double fXValue = m_XAxis[i].GetValueForPos (_CurrPoint.x);
		double fYValue = m_YAxis[i].GetValueForPos (_CurrPoint.y);

		m_XAxis[i].SetAxisMarker(m_nSnappedCurve, m_XAxis[i].m_vAxisMarkers.size(), fXValue, fYValue, RGB(255,0,255), LINE_SLICE_START);
	}

	Invalidate(TRUE);
}

void CXGraph::AddSliceEnd()
{
	for (size_t i = 0; i < m_XAxis.size(); i++)
	{
		double fXValue = m_XAxis[i].GetValueForPos (m_CurrentPoint.x);
		double fYValue = m_YAxis[i].GetValueForPos (m_CurrentPoint.y);

		double lfpair;
		if(m_XAxis[i].SearchPairValue(m_nSliceNum, lfpair)==FALSE)
			return;

		if(lfpair > fXValue)	// end 찍을 때 start 보다 앞에 있으면 거절 [10/23/2013 ChoiJunHyeok]
		{
			m_bSliceStartTurn= FALSE;	// 다시 end를 안찍은 것으로 돌린다. [10/23/2013 ChoiJunHyeok]
			m_pFrame->OnSendMessage("The end line must be front of start line.", TRUE);
			return;
		}

		m_XAxis[i].SetAxisMarker(m_nSnappedCurve, m_XAxis[i].m_vAxisMarkers.size(), fXValue, fYValue, RGB(255,0,255), LINE_SLICE_END);
	}


	Invalidate(TRUE);
}


void CXGraph::AddSliceEnd(CPoint _CurrPoint)
{
	for (size_t i = 0; i < m_XAxis.size(); i++)
	{
		double fXValue = m_XAxis[i].GetValueForPos (_CurrPoint.x);
		double fYValue = m_YAxis[i].GetValueForPos (_CurrPoint.y);

		double lfpair;
		if(m_XAxis[i].SearchPairValue(m_nSliceNum, lfpair)==FALSE)
			return;

		if(lfpair > fXValue)	// end 찍을 때 start 보다 앞에 있으면 거절 [10/23/2013 ChoiJunHyeok]
		{
			m_bSliceStartTurn= FALSE;	// 다시 end를 안찍은 것으로 돌린다. [10/23/2013 ChoiJunHyeok]
			m_pFrame->OnSendMessage("The end line must be front of start line.", TRUE);
			return;
		}

		m_XAxis[i].SetAxisMarker(m_nSnappedCurve, m_XAxis[i].m_vAxisMarkers.size(), fXValue, fYValue, RGB(255,0,255), LINE_SLICE_END);
	}


	Invalidate(TRUE);
}

// maximum context menu [10/24/2013 ChoiJunHyeok]
void CXGraph::MaximumMessage()
{
	if(m_vSeries.empty())
		return;

	if(m_nSliceNum == 0)	// 할당된 구역이 없으면 [10/23/2013 ChoiJunHyeok]
	{
		AddMaximumLine(TRUE, 0, 0, 0);
	}
	else	// 구역이 있으면 [10/24/2013 ChoiJunHyeok]
	{
		if(m_bSliceStartTurn==FALSE)
		{
			m_pFrame->OnSendMessage("Please make pair of groups.", TRUE);
		}
		else
		{
			CXGraphShowOptionDlg dlg;
			CString strTitle; strTitle.Format("( %s : %s ) Option", m_strPlotX, m_strPlotY);
			dlg.m_strDialogTitle= strTitle;
			dlg.m_nGroupCount= m_nSliceNum;
			dlg.m_eShowParam= 1;	// 처음 옵션을 max check
			if( dlg.DoModal() == IDOK )
			{
				double lfStart, lfEnd;
				if( m_XAxis[0].SearchGroup(dlg.m_nSelectGroupNum, lfStart, lfEnd) )
				{
					if(dlg.m_bShowMin)
						AddMinimumLine(FALSE, lfStart, lfEnd, dlg.m_nSelectGroupNum);
					if(dlg.m_bShowMax)
						AddMaximumLine(FALSE, lfStart, lfEnd, dlg.m_nSelectGroupNum);
					if(dlg.m_bAverage)
						AddAverageLabel(FALSE, lfStart, lfEnd, dlg.m_nSelectGroupNum);
				}
				else
				{
					m_pFrame->OnSendMessage("Cannot find current group.",TRUE);
				}
			}
		}		
	}
}
void CXGraph::MinimumMessage()
{
	if(m_vSeries.empty())
		return;

	if(m_nSliceNum == 0)	// 할당된 구역이 없으면 [10/23/2013 ChoiJunHyeok]
	{
		AddMinimumLine(TRUE, 0, 0, 0);
	}
	else	// 구역이 있으면 [10/24/2013 ChoiJunHyeok]
	{
		if(m_bSliceStartTurn==FALSE)
		{
			m_pFrame->OnSendMessage("Please make pair of groups.", TRUE);
		}
		else
		{
			CXGraphShowOptionDlg dlg;
			CString strTitle; strTitle.Format("( %s : %s ) Option", m_strPlotX, m_strPlotY);
			dlg.m_strDialogTitle= strTitle;
			dlg.m_nGroupCount= m_nSliceNum;
			dlg.m_eShowParam= 0;	// 처음 옵션을 min check
			if( dlg.DoModal() == IDOK )
			{			
				double lfStart, lfEnd;
				if( m_XAxis[0].SearchGroup(dlg.m_nSelectGroupNum, lfStart, lfEnd) )
				{
					if(dlg.m_bShowMin)
						AddMinimumLine(FALSE, lfStart, lfEnd, dlg.m_nSelectGroupNum);
					if(dlg.m_bShowMax)
						AddMaximumLine(FALSE, lfStart, lfEnd, dlg.m_nSelectGroupNum);
					if(dlg.m_bAverage)
						AddAverageLabel(FALSE, lfStart, lfEnd, dlg.m_nSelectGroupNum);
				}
				else
				{
					m_pFrame->OnSendMessage("Cannot find current group.",TRUE);
				}
				
			}
		}		
	}

}


void CXGraph::AverageMessage()
{
	CXGraphShowOptionDlg dlg;
	CString strTitle; strTitle.Format("( %s : %s ) Option", m_strPlotX, m_strPlotY);
	dlg.m_strDialogTitle= strTitle;
	dlg.m_nGroupCount= m_nSliceNum;
	dlg.m_eShowParam= 2;	// 처음 옵션을 average check

	if(m_nSliceNum == 0)	// 할당된 구역이 없으면 [10/23/2013 ChoiJunHyeok]
	{
		AddAverageLabel(TRUE, 0, 0, 0);
	}
	else	// 구역이 있으면 [10/24/2013 ChoiJunHyeok]
	{
		if( dlg.DoModal() == IDOK )
		{			
			double lfStart, lfEnd;
			if( m_XAxis[0].SearchGroup(dlg.m_nSelectGroupNum, lfStart, lfEnd) )
			{
				if(dlg.m_bShowMin)
					AddMinimumLine(FALSE, lfStart, lfEnd, dlg.m_nSelectGroupNum);
				if(dlg.m_bShowMax)
					AddMaximumLine(FALSE, lfStart, lfEnd, dlg.m_nSelectGroupNum);
				if(dlg.m_bAverage)
					AddAverageLabel(FALSE, lfStart, lfEnd, dlg.m_nSelectGroupNum);
			}
			else
			{
				m_pFrame->OnSendMessage("Cannot find current group.",TRUE);
			}

		}
	}

	

	
	//return *pLabel;
}


CXGraphLabel& CXGraph::InsertLabel(CRect rect, CString cText)
{
	CXGraphLabel *pLabel = new CXGraphLabel;

	pLabel->m_clRect = rect;
	pLabel->m_cText  = cText;
	pLabel->m_pGraph = this;
	
	m_Objects.AddTail (pLabel);

	return *pLabel;
}

CXGraphLabel& CXGraph::InsertLabel(CString cText)
{
	CXGraphLabel *pLabel = new CXGraphLabel;

	CRect rect(m_clInnerRect.CenterPoint().x - 50,m_clInnerRect.CenterPoint().y - 50,
		       m_clInnerRect.CenterPoint().x + 50,m_clInnerRect.CenterPoint().y + 50);
	
	pLabel->m_clRect = rect;
	pLabel->m_cText  = cText;
	pLabel->m_pGraph = this;
	
	m_Objects.AddTail (pLabel);

	return *pLabel;
}

void CXGraph::Cursor()
{
	Pan();
	m_nSnappedCurve = m_nForcedSnapCurve;
	m_oldCursorPoint = CPoint(-1, -1);
	m_opOperation = opCursor;
	
	if (m_nCursorFlags & XGC_LEGEND)
		m_CursorLabel.m_clRect.SetRect(m_clInnerRect.left + 1, m_clInnerRect.top , m_clInnerRect.left + 150, m_clInnerRect.top + 50);
	

}


void CXGraph::Zoom()
{
	m_CursorLabel.m_bVisible = false;
	m_clCurrentZoom.SetRectEmpty ();
	m_opOperation = opZoom;
	Invalidate();
}

void CXGraph::Peak()
{
	m_opOperation = opPeak;
	Invalidate();
}

void CXGraph::ExportCSV(CString strPath, CFlightDataManager* pData)
{
	CFlightDataManager* pNewData= new CFlightDataManager();
	pData->PeakFlightData(m_vnPeakRemoveKey, pNewData);

	if ( !pNewData->ExportFlightData(strPath.GetBuffer()) )
	{
		CString szMessage;
		szMessage.Format("%s",_T("[EXPORT] Export Failed"));
		m_pFrame->OnSendMessage(szMessage, TRUE);
	}
}

// 범위 해당 되는 부분을 지움 [10/xx/2013 ChoiJunHyeok]
void CXGraph::PeakRemove()
{
	CXSeriesData* pOldData= Data()->GetSeriesData()[0];
	//CSeriesData backupData= Data()->GetSeriesData()[nMarkSeriesIdx];
	CXSeriesData* pNewData= new CXSeriesData();	

	size_t nSize= pOldData->m_vXData.size();
	for (size_t i=0; i<nSize; ++i)
	{
		double lfX= fabs( pOldData->m_vXData[i] );
		double lfY= fabs( pOldData->m_vYData[i] );

		double lfXvalue1= fabs( m_XAxis[0].GetValueForPos(m_MouseDownPoint.x) );
		double lfXvalue2= abs( m_XAxis[0].GetValueForPos(m_CurrentPoint.x) );
		double lfYvalue1= abs( m_YAxis[0].GetValueForPos(m_MouseDownPoint.y) );
		double lfYvalue2= abs( m_YAxis[0].GetValueForPos(m_CurrentPoint.y) );

		// X, Y 각각 1,2번 반대로 되어있으면 순서대로 바꿈
		if(lfXvalue2 < lfXvalue1)
		{
			double lfTemp= lfXvalue2;
			lfXvalue2= lfXvalue1;
			lfXvalue1= lfTemp;
		}
		if(lfYvalue2 < lfYvalue1)
		{
			double lfTemp= lfYvalue2;
			lfYvalue2= lfYvalue1;
			lfYvalue1= lfTemp;
		}

			

		if( (lfXvalue1 <= lfX) && (lfXvalue2 >= lfX) )
		{
			if( (lfYvalue1 <= lfY) && (lfYvalue2 >= lfY) )
			{
				printf("\n%d번째 포인트 제외\nX: %.3lf\nY: %.3lf", i, pOldData->m_vXData[i], pOldData->m_vYData[i]);
				m_vnPeakRemoveKey.push_back(pOldData->m_vKey[i]);	// 지운 포인트 저장 [10/25/2013 ChoiJunHyeok]
				continue;
			}
		}
		pNewData->InsertXYK(pOldData->m_vXData[i], pOldData->m_vYData[i], pOldData->m_vKey[i]);
	}
	vector<CXSeriesData*> pSeriesData= Data()->GetSeriesData();
	pSeriesData.clear();
	pSeriesData.push_back(pNewData);
	Data()->SetSeriesData(pSeriesData);	// 새로운 데이터를 다시 세팅한다. [10/22/2013 ChoiJunHyeok]

	this->InputCurveData(0, FALSE);	// 세팅한 데이터를 그래프에 다시 넣는다. [11/25/2013 ChoiJunHyeok]
	Invalidate();
	//SetDrawAverage(FALSE);
	//this->RePlot(FALSE);
}

// 플로팅 위치 설정 [10/23/2013 ChoiJunHyeok]
void CXGraph::PlotRectSet(UINT nRectPos, UINT nRow, UINT nCol)
{
	int nCurrIdx;
	for(int i=0; i<nCol; ++i)
	{
		for(int j=0; j<nRow; ++j)
		{
			nCurrIdx= i* nRow +j;//i*j;

			if(nCurrIdx == nRectPos)	// 현재 위치가 그 위치면 [10/23/2013 ChoiJunHyeok]
			{
				m_nPlottingPosRow= j;
				m_nPlottingPosCol= i;			
			}
		}
	}

	if(m_nPlottingPosRow == nRow-1)	// 바닥이면 [10/23/2013 ChoiJunHyeok]
	{
		m_bPlottingBottom= TRUE;
	}
	else
	{
		m_bPlottingBottom= FALSE;
	}
}

void CXGraph::PlotDataSet(UINT nSeriesNumber, CString strX, CString strY, vector<CString> vSerieNames)
{
	m_nSerieCount= nSeriesNumber;
	//m_vpDataMgr= vpDataMgr;
	m_strPlotX= strX;
	m_strPlotY= strY;
	m_vSerieNames= vSerieNames;
}

void CXGraph::RePlot(BOOL bFristPlot)
{
	if(m_nSerieCount==0)
	{
		AfxMessageBox("CXGraph::RePlot()\nPlease use PlotDataSet method frist.");
	}
	// 먼저 있는 시리즈 전부 초기화하고 [8/28/2013 ChoiJunHyeok]
	//CXTPChartSeriesCollection* pCollection = m_vChartControl[x]->GetContent()->GetSeries();
	//pCollection->RemoveAll();

	//m_vChartControl[x]->SetLogScale(FALSE);
	ResetAll();

	ForceSnap(0);

	if(bFristPlot)
	{
		SetDoubleBuffer(TRUE);
		if(m_nSerieCount >= 2)
			SetShowLegend(TRUE);
		else
			SetShowLegend(FALSE);
		
	}


	SetInteraction(TRUE);
	EnableWindow(TRUE);

	if(bFristPlot)
	{
		NoOp();
		//if(m_nSerieCount >= 2)
		//	NoOp();
		//else
		//	NoOp();//Cursor();
	}


	for (size_t s=0; s<m_nSerieCount; ++s)	// FlightData 개수별로
	{
		InputCurveData(s, TRUE);

		GetCurve(s).SetLabel(m_vSerieNames[s]);
		GetCurve(s).SetLineSize(2);

		if(bFristPlot)
		{
			GetCurve(s).SetShowMarker(FALSE);
			GetCurve(s).SetMarker(s%8);
			GetCurve(s).SetMarkerType(1);			
		}
		
	}

	int XCount = GetXAxisCount();
	int YCount = GetYAxisCount();

	double	fMaxValue=0, fMinValue = 0, fSubValue = 0;

	for(size_t i = 0; i < XCount; i++)
	{
		CXGraphAxis& Axis = GetXAxis(i);
		if(m_bPlottingBottom)	// 아래에 있으면 레이블 안보임 [10/23/2013 ChoiJunHyeok]
		{
			Axis.SetLabel(m_strPlotX);
		}
		else
		{
			Axis.SetLabel("");
		}
		
		Axis.SetShowMarker(FALSE);
		Axis.SetShowGrid(true);

		if((m_strPlotX.CompareNoCase("IRIG TIME")==0)
			|| (m_strPlotX.CompareNoCase("IRIG_TIME")==0)
			|| (m_strPlotX.CompareNoCase("IRIG-TIME")==0)
			|| (m_strPlotX.CompareNoCase("Time")==0)
			)
		{
			//Axis.SetDateTime(TRUE);
			Axis.SetIRIGTime(TRUE);	//  [10/8/2013 ChoiJunHyeok]
		}
		else
		{
			Axis.SetIRIGTime(FALSE);
		}

		//Axis.SetAutoScale(false);
		//fMaxValue = Axis.GetMaxValue();

		fMaxValue = Axis.GetMaxValue();
		fMinValue = Axis.GetMinValue();

		//fSubValue = (fMaxValue-fMinValue)*0.05;
		Axis.SetRange(fMinValue,fMaxValue, TRUE);
	}

	for(size_t i = 0; i < YCount; i++)
	{
		CXGraphAxis& YAxis= GetYAxis(i);
		YAxis.SetLabel(m_strPlotY);
		YAxis.SetShowMarker(FALSE);
		YAxis.SetShowGrid(true);
		YAxis.SetMarkerSize(3);
		YAxis.SetPlacement(CXGraphAxis::apAutomatic);

		fMaxValue = YAxis.GetMaxValue();
		fMinValue = YAxis.GetMinValue();

		if(fMaxValue == fMinValue) 
		{
			fMaxValue += 1.0;
			fMinValue -= 1.0;
		}

		//fSubValue = (fMaxValue-fMinValue)*0.05;
		YAxis.SetRange(fMinValue,fMaxValue, TRUE);

	}	

	//if(bFristPlot)		ResetZoom();
}




void CXGraph::Range()
{
	m_opOperation = opRange;
	Invalidate();
}


void CXGraph::Edit()
{
	m_opOperation = opEditCurve;
	Invalidate();
}

void CXGraph::ResetZoom2()
{
	int XCount = GetXAxisCount();
	int YCount = GetYAxisCount();

	double	fMaxValue=0, fMinValue = 0;

	for(size_t i = 0; i < XCount; i++)
	{
		CXGraphAxis& Axis = GetXAxis(i);
		fMaxValue = Axis.GetMaxValue();
		fMinValue = Axis.GetMinValue();

		Axis.SetRange(fMinValue,fMaxValue, TRUE);
		Axis.SetBestStep();
	}

	for(size_t i = 0; i < YCount; i++)
	{
		CXGraphAxis& YAxis= GetYAxis(i);

		fMaxValue = YAxis.GetMaxValue();
		fMinValue = YAxis.GetMinValue();

		/*if(fMaxValue == fMinValue) 
		{
			fMaxValue += 1.0;
			fMinValue -= 1.0;
		}*/

		YAxis.SetRange(fMinValue,fMaxValue, TRUE);
		YAxis.SetBestStep();

	}	


	Invalidate();
}

void CXGraph::ResetZoom()
{
	m_CursorLabel.m_bVisible = false;
	for (int y = 0; y < static_cast<int>(m_YAxis.size()); y++)
		m_YAxis[y].Reset();
	for (int x = 0; x < static_cast<int>(m_XAxis.size()); x++)
		m_XAxis[x].Reset();
	Invalidate();
}

void CXGraph::NoOp()
{
	m_CursorLabel.m_bVisible = false;
	m_opOperation = opNone;
	Invalidate();
}

void CXGraph::SliceMode()
{
	m_opOperation= opSlice;

	Invalidate();
}

void CXGraph::Pan()
{
	m_CursorLabel.m_bVisible = false;
	m_opOperation = opPan;

	for (int y = 0; y < static_cast<int>(m_YAxis.size()); y++)
		m_YAxis[y].m_bAutoScale = false;
	
	for (int x = 0; x < static_cast<int>(m_XAxis.size()); x++)
		m_XAxis[x].m_bAutoScale = false;

	Invalidate();
}

void CXGraph::DoPan(CPoint point)
{
	for (int y = 0; y < static_cast<int>(m_YAxis.size()); y++)
	{
		double fY1 = m_YAxis[y].GetValueForPos (point.y);
		double fY2 = m_YAxis[y].GetValueForPos (m_OldPoint.y);
		double fOffset = fY1 - fY2;
		VERIFY(m_YAxis[y].SetCurrentRange(m_YAxis[y].m_fCurMin - fOffset,m_YAxis[y].m_fCurMax - fOffset));
	}

	for (int x = 0; x < static_cast<int>(m_XAxis.size()); x++)
	{
		double fX1 = m_XAxis[x].GetValueForPos (point.x);
		double fX2 = m_XAxis[x].GetValueForPos (m_OldPoint.x);
		double fOffset = fX1 - fX2;
		VERIFY(m_XAxis[x].SetCurrentRange(m_XAxis[x].m_fCurMin - fOffset,m_XAxis[x].m_fCurMax - fOffset));
	}

	m_OldPoint = point;

	Invalidate();
}

void CXGraph::DoZoom(BOOL bSyncMessage, BOOL bYautoScale)
{		
	if (m_clCurrentZoom.Width () < MIN_ZOOM_PIXELS ||
		m_clCurrentZoom.Height () < MIN_ZOOM_PIXELS)
		return;

	ZOOM zoom;
			
	for (UINT y = 0; y < m_YAxis.size(); y++)
	{
		double fMin, fMax;

		if(bYautoScale)	// Y를 선택한 범위를 무시하고 현재 min, max로 오토스케일링 한다. [11/13/2013 ChoiJunHyeok]
		{
			fMin = m_YAxis[y].GetValueForPos(m_clInnerRect.bottom);
			fMax = m_YAxis[y].GetValueForPos(m_clInnerRect.top);

			zoom.fMin = m_YAxis[y].m_fCurMin;
			zoom.fMax = m_YAxis[y].m_fCurMax;

			m_YAxis[y].m_ZoomHistory.push_back (zoom);

			m_YAxis[y].m_fCurMin = fMin;
			m_YAxis[y].m_fCurMax = fMax;
			m_YAxis[y].SetBestStep ();
			
		}
		else
		{
			fMin = m_YAxis[y].GetValueForPos(min(m_clCurrentZoom.bottom, m_clInnerRect.bottom));
			fMax = m_YAxis[y].GetValueForPos(max(m_clCurrentZoom.top, m_clInnerRect.top));

			if ((fMax - fMin) != 0.0)
			{			
				zoom.fMin = m_YAxis[y].m_fCurMin;
				zoom.fMax = m_YAxis[y].m_fCurMax;

				m_YAxis[y].m_ZoomHistory.push_back (zoom);

				m_YAxis[y].m_fCurMin = fMin;
				m_YAxis[y].m_fCurMax = fMax;
				m_YAxis[y].SetBestStep ();

			}
		}
		
	}
	

	for (UINT x = 0; x < m_XAxis.size(); x++)
	{
		double fMin, fMax;

		fMin = m_XAxis[x].GetValueForPos(max(m_clCurrentZoom.left, m_clInnerRect.left));
		fMax = m_XAxis[x].GetValueForPos(min(m_clCurrentZoom.right, m_clInnerRect.right));
		
		if ((fMax - fMin) != 0.0)
		{
			zoom.fMin = m_XAxis[x].m_fCurMin;
			zoom.fMax = m_XAxis[x].m_fCurMax;
	
			m_XAxis[x].m_ZoomHistory.push_back (zoom);
	
			m_XAxis[x].m_fCurMin = fMin;
			m_XAxis[x].m_fCurMax = fMax;
			m_XAxis[x].SetBestStep();
			
		}
	}

	if(bSyncMessage)
	{
		//CRect* pPostRect= new CRect(m_clCurrentZoom.left, m_clInnerRect.top, m_clCurrentZoom.right, m_clInnerRect.bottom);
		CRect* pPostRect= new CRect(m_clCurrentZoom.left, m_clCurrentZoom.top, m_clCurrentZoom.right, m_clCurrentZoom.bottom);
		::PostMessage(GetParent()->m_hWnd, XG_ZOOMCHANGE, (WPARAM)pPostRect, (long) this);
	}
	
	
	m_clCurrentZoom.SetRectEmpty ();
}

int CXGraph::GetZoomDepth()
{
	if (m_XAxis.size() > 0)
		return m_XAxis[0].m_ZoomHistory.size();

	return 0;

}

void CXGraph::ApplyZoom(ZOOM zoom)
{
	for (UINT y = 0; y < m_YAxis.size(); y++)
	{
		m_YAxis[y].m_fCurMin = zoom.fMin;
		m_YAxis[y].m_fCurMax = zoom.fMax;
		m_YAxis[y].SetBestStep ();
	}

	for (UINT x = 0; x < m_XAxis.size(); x++)
	{
		m_XAxis[x].m_fCurMin = zoom.fMin;
		m_XAxis[x].m_fCurMax = zoom.fMax;
		m_XAxis[x].SetBestStep ();
	}
}


void CXGraph::RestoreLastZoom()
{		
	for (UINT y = 0; y < m_YAxis.size(); y++)
	{
		if (m_YAxis[y].m_ZoomHistory.size() > 0)
		{
			m_YAxis[y].m_fCurMin = m_YAxis[y].m_ZoomHistory[m_YAxis[y].m_ZoomHistory.size() - 1].fMin;
			m_YAxis[y].m_fCurMax = m_YAxis[y].m_ZoomHistory[m_YAxis[y].m_ZoomHistory.size() - 1].fMax;
			m_YAxis[y].m_ZoomHistory.pop_back ();
			m_YAxis[y].SetBestStep ();

		}
	}

	for (UINT x = 0; x < m_XAxis.size(); x++)
	{
		if (m_XAxis[x].m_ZoomHistory.size() > 0)
		{
			m_XAxis[x].m_fCurMin = m_XAxis[x].m_ZoomHistory[m_XAxis[x].m_ZoomHistory.size() - 1].fMin;
			m_XAxis[x].m_fCurMax = m_XAxis[x].m_ZoomHistory[m_XAxis[x].m_ZoomHistory.size() - 1].fMax;
			m_XAxis[x].m_ZoomHistory.pop_back ();
			m_XAxis[x].SetBestStep ();

		}
	}

	
	
	//::PostMessage(GetParent()->m_hWnd, XG_ZOOMCHANGE, (WPARAM)&m_clCurrentZoom, (long) this);
	
	m_clCurrentZoom.SetRectEmpty ();

	Invalidate();

}



void CXGraph::DrawLegend(Graphics& graphics, CRect& ChartRect)
{
	int nLegendWidth = 0;
	int nLegendHeight = 0;
	int nItemHeight = 0;

	RectF labelTextRect;
	SolidBrush brBkgndOuter(Color::Black);
	wchar_t* pwstr = char2wchar(_T("Arial"));
	Gdiplus::Font font(pwstr,(REAL)10,FW_NORMAL,UnitPixel);
	free(pwstr);

	CString cLegend;
	//	CQuickFont font(_T("Arial"), 14, FW_NORMAL);
	{
		//		CFontSelector fs(&font, pDC, false);
		UINT DrawCount = 0;
		UINT i = 0, Count = (UINT)m_vSeries.size();
		for(i = 0;i < Count; i++)
		{
			// JaeJin corrects codes
			//if (!m_Data[i].m_bVisible)
			//	continue;
			/////////////////////////////////////////////////
			if(m_vSeries[i].GetVisible() == true)
			{
				cLegend.Format(_T("%s"), m_vSeries[i].m_cLabel);

				wchar_t* pwstr = char2wchar(cLegend.GetBuffer());
				graphics.MeasureString(pwstr,
					cLegend.GetLength(),
					&font, 
					RectF((REAL)0,0, 0, 0), 
					&labelTextRect); 

				free(pwstr);

				nLegendWidth = __max(nLegendWidth, (int)labelTextRect.Width);
				nLegendHeight += (nItemHeight = (int)labelTextRect.Height);
				DrawCount++;
			}
		}
		// JaeJin corrects codes
		// Select Voltage 항목 추가
		nLegendWidth += 20;
		if(DrawCount == 0) return;
		////////////////////////////////////////////////////////////////////////////////////////////////
	}
	//	CFontSelector fs(&font, pDC);

	nLegendWidth += 16;
	CRect legendRect;
	// JaeJin add codes
	// select voltage를 위한 Box 영역 추가를 위한 변수
	CRect SelLegRect;
	////////////////////////////////////////////////////////////////////////////////////////////////

	//if (m_LegendAlignment == left)
	//{
	//	legendRect.SetRect (ChartRect.left + 10, ChartRect.top + 10, ChartRect.left + 10 + nLegendWidth, ChartRect.top + 12 + nLegendHeight);
	//	ChartRect.left += (nLegendWidth + 20);
	//}

	//if (m_LegendAlignment == right)
	//{
	//	legendRect.SetRect (ChartRect.right - 10 - nLegendWidth, ChartRect.top + 10, ChartRect.right - 10, ChartRect.top + 12 + nLegendHeight);
	//	ChartRect.right -= (nLegendWidth + 20);
	//}
	if (m_LegendAlignment == left)
	{

		legendRect.SetRect (ChartRect.left + 80, ChartRect.top + 10, ChartRect.left + 80 + nLegendWidth, ChartRect.top + 12 + nLegendHeight);

		if(GetYAxisCount() > 1)
			if(m_YAxis[1].m_Placement == CXGraphAxis::apLeft)
				legendRect.SetRect (ChartRect.left + 150, ChartRect.top + 10, ChartRect.left + 150 + nLegendWidth, ChartRect.top + 12 + nLegendHeight);
		//ChartRect.left += (nLegendWidth + 70);
	}

	if (m_LegendAlignment == right)
	{
		legendRect.SetRect (ChartRect.right - 20 - nLegendWidth, ChartRect.top + 10, ChartRect.right - 20, ChartRect.top + 12 + nLegendHeight);

		if(GetYAxisCount() > 1 )
			if(m_YAxis[1].m_Placement == CXGraphAxis::apRight)
				legendRect.SetRect (ChartRect.right - 75 - nLegendWidth, ChartRect.top + 10, ChartRect.right - 75, ChartRect.top + 12 + nLegendHeight);
			
		//ChartRect.right -= (nLegendWidth + 20);
	}
	else
	{

	}

	//	CPenSelector ps(0, 1, pDC);
	COLORREF brushColor = ::GetSysColor(COLOR_WINDOW);
	//	CBrushSelector bs(brushColor, pDC);



	if(legendRect.Height() > 0)
	{
		//		pDC->Rectangle(legendRect);
		RectF rect(legendRect.left,legendRect.top,legendRect.Width(),legendRect.Height());
		Pen pen(Color(0,0,0),1);
		graphics.DrawRectangle(&pen,rect);
	}
	// JaeJin corrects codes
	// Select Voltage 항목 추가
	legendRect.left += 20;
	////////////////////////////////////////////////////////////////////////////////////////////////
	legendRect.DeflateRect (1,1,1,1);

	int nItem = 0;
	PointF orgpt;

	UINT i = 0, Count = (UINT)m_vSeries.size();
	for(i = 0; i < Count; i++)
	{
		// JaeJin corrects codes
		if (m_vSeries[i].GetVisible() == false)
			continue;
		/////////////////////////////////////////////////////

		cLegend.Format(_T("%s"), m_vSeries[i].m_cLabel);
		CRect itemRect(legendRect.left+10, legendRect.top + nItemHeight * nItem, legendRect.right, legendRect.top + nItemHeight * nItem + nItemHeight);
		//////////////////////////////////////////////////////////////////////


		//	pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
		//	pDC->DrawText(cLegend, itemRect, DT_LEFT);
		orgpt.X = legendRect.left+10;
		orgpt.Y = legendRect.top + nItemHeight * nItem;

		wchar_t* pwstr = char2wchar(cLegend.GetBuffer());
		graphics.DrawString(pwstr,cLegend.GetLength(),&font,orgpt,&brBkgndOuter);
		free(pwstr);
		itemRect.left = legendRect.left - 15;
		itemRect.right = legendRect.left + 5;
		itemRect.DeflateRect(0,1,0,2);

		m_vSeries[i].DrawLengend(graphics,itemRect);

		//if (m_Data[i]->m_nMarkerType == 0)
		//	pDC->FillSolidRect(itemRect,/* pDC->m_bMono ? 0L :*/ m_Data[i]->m_crColor);
		//else
		//	m_YAxis[0]->DrawMarker(pDC, CPoint(itemRect.left - 1, itemRect.top + 1), m_Data[i]->m_nMarker, 8, /*pDC->m_bMono ? 0L :*/ m_Data[i]->m_crColor, true);
		nItem++;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen f? Nachrichten CXGraph 

void CXGraph::DrawLegend(CDCEx *pDC, CRect& ChartRect)
{
	int nLegendWidth = 0;
	int nLegendHeight = 0;
	int nItemHeight = 0;

	CQuickFont font("Arial", 13, FW_NORMAL);
	{
		CFontSelector fs(&font, pDC, false);

		for (UINT i = 0; i < m_vSeries.size(); i++)
		{
			if (!m_vSeries[i].m_bVisible)
				continue;

			CString cLegend;
			cLegend.Format(_T("%02d %s"), i + 1, m_vSeries[i].m_cLabel);
			nLegendWidth = __max(nLegendWidth, pDC->GetTextExtent (cLegend).cx);
			nLegendHeight += (nItemHeight = pDC->GetTextExtent (cLegend).cy);
		}
	}

	CFontSelector fs(&font, pDC);

	nLegendWidth += 16;

	CRect legendRect;

	// 임시 [9/30/2013 ChoiJunHyeok]
	m_LegendAlignment= right;
	
	if (m_LegendAlignment == left)
	{
		legendRect.SetRect (ChartRect.left + 20, ChartRect.top + 10, ChartRect.left + 20 + nLegendWidth, ChartRect.top + 12 + nLegendHeight);
		//ChartRect.left += (nLegendWidth + 70);
	}
	
	if (m_LegendAlignment == right)
	{
		legendRect.SetRect (ChartRect.right - 20 - nLegendWidth, ChartRect.top + 10, ChartRect.right - 20, ChartRect.top + 12 + nLegendHeight);
		//ChartRect.right -= (nLegendWidth + 20);
	}
	else
	{
		
	}

	CPenSelector ps(0, 1, pDC);

	COLORREF brushColor = ::GetSysColor(COLOR_WINDOW);

	CBrushSelector bs(brushColor, pDC);
	
	pDC->Rectangle(legendRect);

	legendRect.DeflateRect (1,1,1,1);
	
	int nItem = 0;

	m_SelectByMarker.clear();

	for (UINT i = 0; i < m_vSeries.size(); i++)
	{
		SelectByMarker sbm;

		if (!m_vSeries[i].m_bVisible)
			continue;

		CString cLegend;
		cLegend.Format(_T("%02d %s"), i + 1, m_vSeries[i].m_cLabel);
		CRect itemRect(legendRect.left + 12, legendRect.top + nItemHeight * nItem, legendRect.right, legendRect.top + nItemHeight * nItem + nItemHeight);

		sbm.markerRect = itemRect;
		sbm.markerRect.left -= 12;
		sbm.pObject = &m_vSeries[i];

		m_SelectByMarker.push_back(sbm);
				
		pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));

		pDC->DrawText(cLegend, itemRect, DT_LEFT);

		if (m_vSeries[i].m_bSelected)
			pDC->DrawFocusRect (sbm.markerRect);

		itemRect.left = legendRect.left + 2;
		itemRect.right = legendRect.left + 10;
		itemRect.DeflateRect (0,1,0,2);

		if (m_vSeries[i].m_nMarkerType == 0)
			pDC->FillSolidRect (itemRect, m_vSeries[i].m_crColor);
		else
			m_YAxis[0].DrawMarker (pDC, CPoint(itemRect.left - 1, itemRect.top + 1), m_vSeries[i].m_nMarker, 8, m_vSeries[i].m_crColor, true);
	
		nItem++;
	}
}


void CXGraph::OnDraw(CDC *pDC)
{
	CDCEx dc;
	
	dc.Attach (pDC->m_hDC);
	dc.Prepare (pDC->m_hAttribDC);
	
	m_pDrawDC = &dc;
	//m_pDrawDC->m_bMono = false;
	m_pDrawDC->m_bPrinting = false;
	m_pDrawDC->m_bPrintPreview = false;
	
	m_clPrintRect.SetRect(0, 0, pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));

	OnPaint();

	m_pDrawDC = NULL;

	dc.Detach();
}


#ifdef GDI_DRAW

// GDI+ paint [9/30/2013 ChoiJunHyeok]
void CXGraph::OnPaint()
{
	puts("CXGraph::OnPaint()");

	int		nSaveDC	= 0;
	CDCEx*	pDC	= new CDCEx;
	CXMemDC*	pmdc	= NULL;
	CPaintDC	dc(this);

	CRect		clRect, clChartRect;
	m_oldCursorPoint = CPoint(-1, -1);
	if(m_pDrawDC != NULL)
	{
		delete pDC;
		pDC = (CDCEx*)m_pDrawDC;
	}
	else if(m_pPrintDC != NULL)
	{
		delete pDC;
		pDC = (CDCEx*)m_pPrintDC;
	}
	else
	{
		CRect rect;
		GetClientRect(&rect);
		pmdc = new CXMemDC(&dc,rect);
		pDC->Attach(pmdc->m_hDC);
		nSaveDC = pDC->SaveDC();
		pDC->m_bPrinting = false;
		pDC->m_bPrintPreview = false;
	}
	m_oldCursorPoint = CPoint(-1, -1);
	// Background
	Graphics graphics(pDC->m_hDC);
	if (pDC->m_bPrinting)
		clRect = m_clPrintRect;
	else
		GetClientRect(clRect);

	Pen pen(GetGDIPlusColor(m_crBackColor));
	RectF rect(clRect.left,clRect.top,clRect.Width(),clRect.Height());
	graphics.DrawRectangle(&pen,rect);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
//	pDC->FillSolidRect(clRect, m_crBackColor);
	{
		SolidBrush brush(GetGDIPlusColor(m_crBackColor));
		graphics.FillRectangle(&brush,RectF(clRect.left,clRect.top,
											clRect.Width(),clRect.Height()));
	}

	// Chart
	clChartRect.left   = clRect.left + m_nLeftMargin;
	clChartRect.top    = clRect.top + m_nTopMargin;
	clChartRect.right  = clRect.right - m_nRightMargin;
	clChartRect.bottom = clRect.bottom - m_nBottomMargin;

//	pDC->FillSolidRect(clChartRect, m_crGraphColor);
	{
		SolidBrush brush(GetGDIPlusColor(m_crGraphColor));
		graphics.FillRectangle(&brush,RectF(clChartRect.left,clChartRect.top,
											clChartRect.Width(),clChartRect.Height()));
	}

	int nLeft   = clChartRect.left;
	int nRight  = clChartRect.right;
	int nBottom = clChartRect.bottom;
	UINT	nYAxis = 0;
	UINT	nXAxis = 0;

	CXGraphAxis::EAxisPlacement lastPlacement = CXGraphAxis::apAutomatic;
	
	// Calculate layout, prepare automatic axis
	UINT Count = (UINT)m_YAxis.size();
	for (nYAxis = 0; nYAxis < Count; nYAxis++)
	{
		if (m_YAxis[nYAxis].m_Placement == CXGraphAxis::apAutomatic)
		{
			if(lastPlacement == CXGraphAxis::apLeft)
			{
				lastPlacement = CXGraphAxis::apRight;
				m_YAxis[nYAxis].m_Placement = CXGraphAxis::apRight;
			}
			else
			{
				lastPlacement = CXGraphAxis::apLeft;
				m_YAxis[nYAxis].m_Placement = CXGraphAxis::apLeft;
			}
		}
	}
	// Y-Axis, left side
	for (nYAxis = 0; nYAxis < Count; nYAxis++)
	{
		if (m_YAxis[nYAxis].m_Placement != CXGraphAxis::apLeft)
			continue;

		int nTickWidth  = m_YAxis[nYAxis].GetMaxLabelWidth (pDC);
		int nLabelWidth = (50 + m_YAxis[nYAxis].m_nArcSize);

		if (m_YAxis[nYAxis].m_bVisible)
			nLeft += (max (nTickWidth, nLabelWidth) + m_nAxisSpace );

		m_YAxis[nYAxis].m_nLeft   = nLeft;
		m_YAxis[nYAxis].m_clChart = clChartRect;
	}

	// Y-Axis, right side
	for (nYAxis = 0; nYAxis < Count; nYAxis++)
	{
		if (m_YAxis[nYAxis].m_Placement != CXGraphAxis::apRight)
			continue;

		int nTickWidth  = m_YAxis[nYAxis].GetMaxLabelWidth(pDC);
		int nLabelWidth = 50;

		if (m_YAxis[nYAxis].m_bVisible)
			nRight -= (max (nTickWidth, nLabelWidth) + m_nAxisSpace );

		m_YAxis[nYAxis].m_nLeft   = nRight;
		m_YAxis[nYAxis].m_clChart = clChartRect;
	}
	// X-Axis
	Count = (UINT)m_XAxis.size();
	for (nXAxis = 0; nXAxis < Count; nXAxis++)
	{
		if (m_XAxis[nXAxis].m_bVisible)
			nBottom -= (m_XAxis[nXAxis].GetMaxLabelHeight(pDC) + m_nAxisSpace);

		m_XAxis[nXAxis].m_nTop    = nBottom;
		m_XAxis[nXAxis].m_nLeft   = nLeft;
		m_XAxis[nXAxis].m_clChart = clChartRect;
	}

	// Draw Inner Rect
	//
	
	m_clInnerRect.SetRect(nLeft + 1, clChartRect.top, nRight, nBottom);
	{
		SolidBrush brush(GetGDIPlusColor(m_crInnerColor));
		graphics.FillRectangle(&brush,RectF(m_clInnerRect.left,m_clInnerRect.top,
											m_clInnerRect.Width(),m_clInnerRect.Height()));
	}

	// Draw axis
	// Y-Axis
	Count = (UINT)m_YAxis.size();
	for (nYAxis = 0; nYAxis < Count; nYAxis++)
	{
		m_YAxis[nYAxis].m_nTop = nBottom;
		m_YAxis[nYAxis].Draw(graphics);
	}

	// X-Axis
	Count = (UINT)m_XAxis.size();
	for (nXAxis = 0; nXAxis < Count; nXAxis++)
		m_XAxis[nXAxis].Draw(graphics);

	RectF rectClip2(m_clInnerRect.left, m_clInnerRect.top, 
					m_clInnerRect.Width(), m_clInnerRect.Height());
	graphics.SetClip(rectClip2);

	// Draw curves
	UINT nCurve = 0;
	Count = (UINT)m_vSeries.size();
	for(nCurve = 0; nCurve < Count; nCurve++)
		if(m_vSeries[nCurve].m_bVisible)
			m_vSeries[nCurve].Draw(graphics);

	// Draw curve markers
	for(nCurve = 0; nCurve < Count; nCurve++)
		if(m_vSeries[nCurve].m_bVisible && m_vSeries[nCurve].m_bShowMarker)
			m_vSeries[nCurve].DrawMarker(graphics);

	// Setup clipping
	//CRgn  clipRegion;
	CRect rectClip(m_clInnerRect.left, m_clInnerRect.top, m_clInnerRect.right, m_clInnerRect.bottom);

	// Should move inside CDCEx in future
	if (pDC->m_bPrinting && !pDC->m_bPrintPreview)
		pDC->AdjustRatio(rectClip);

	switch(m_opOperation)
	{
		case opZoom:
			//if(m_bLButtonDown == false) DrawZoom(pDC);			
			if (m_bLButtonDown )
				DrawZoom(graphics);
			break;
		/*case opCapture:
			if(m_bLButtonDown == false) DrawCapture(pDC);
			break;*/
		case opCursor:
			DrawCursor(graphics);
			break;
	}

	for (size_t i=0; i<m_vGraphLines.size(); ++i)	// line들을 그린다. [9/30/2013 ChoiJunHyeok]
		m_vGraphLines[i].Draw(graphics);

	this->DrawRange(graphics);	// range, peak 모드 [10/8/2013 ChoiJunHyeok]

	// DrawObjects
	for(POSITION pos = m_Objects.GetHeadPosition (); pos != NULL; )
	{
		CXGraphObject *pObject = (CXGraphObject*)m_Objects.GetNext(pos);
		pObject->Draw(pDC);
	} 


	if (m_bShowLegend)
		DrawLegend(graphics, clChartRect);





	if(m_pDrawDC == NULL && m_pPrintDC == NULL)
	{
		pDC->RestoreDC(nSaveDC);
		pDC->Detach();
		delete pDC;
	}
	if(pmdc)		delete pmdc;

	return;
	/*if(m_bDataPointMoving)
	{
		CQuickFont font("Arial", 13, FW_NORMAL);
		CFontSelector fs(&font, pDC, false);
		pDC->SetBkMode(TRANSPARENT);
		CString cPoint;
		cPoint.Format ("%.2f", m_fCurrentEditValue);
		pDC->TextOut(m_CurrentPoint.x + 10, m_CurrentPoint.y, cPoint);
	}*/

	if(m_pDrawDC == NULL && m_pPrintDC == NULL)
	{
		pDC->RestoreDC(nSaveDC);
		pDC->Detach();
		delete pDC;
	}
	if(pmdc)		delete pmdc;
}
#else

// original paint [9/30/2013 ChoiJunHyeok]
void CXGraph::OnPaint() 
{
	CDCEx*    pDC   = new CDCEx;
	CXMemDC*   pmdc  = NULL;
	CPaintDC* pdc   = new CPaintDC(this);
	CRect     clRect, clChartRect;
	int		  nSaveDC;
	

	m_oldCursorPoint = CPoint(-1, -1);

	if(m_pDrawDC != NULL)
	{
		pDC = (CDCEx*) m_pDrawDC;
	}
	else
	{
		if (m_bDoubleBuffer)
		{
			pmdc = new CXMemDC(pdc);
			pDC->Attach(pmdc->m_hDC);
			nSaveDC = pDC->SaveDC();
			//pDC->m_bMono = false;
			pDC->m_bPrinting = false;
			pDC->m_bPrintPreview = false;
		}
		else
		{
			pDC->Attach(pdc->m_hDC);
			nSaveDC = pDC->SaveDC();
			//pDC->m_bMono = false;
			pDC->m_bPrinting = false;
			pDC->m_bPrintPreview = false;
		}
	}

	Graphics graphics(pDC->m_hDC);
	
	m_oldCursorPoint = CPoint(-1, -1);

	// Background
	if (pDC->m_bPrinting)
		clRect = m_clPrintRect;
	else
		GetClientRect(clRect);

		pDC->FillSolidRect(clRect, m_crBackColor);
	
	// Chart
	clChartRect.left   = clRect.left + m_nLeftMargin;
	clChartRect.top    = clRect.top + m_nTopMargin;
	clChartRect.right  = clRect.right - m_nRightMargin;
	clChartRect.bottom = clRect.bottom - m_nBottomMargin;



		pDC->FillSolidRect(clChartRect, m_crGraphColor);

	int nLeft   = clChartRect.left;
	int nRight  = clChartRect.right;
	int nBottom = clChartRect.bottom;

	CXGraphAxis::EAxisPlacement lastPlacement = CXGraphAxis::apAutomatic;

	// Calculate layout, prepare automatic axis
	for (size_t nYAxis = 0; nYAxis < m_YAxis.size (); nYAxis++)
	{
		if (m_YAxis[nYAxis].m_Placement == CXGraphAxis::apAutomatic)
		{
			if (lastPlacement == CXGraphAxis::apLeft)
			{
				lastPlacement = CXGraphAxis::apRight;
				m_YAxis[nYAxis].m_Placement = CXGraphAxis::apRight;
			}
			else
			{
				lastPlacement = CXGraphAxis::apLeft;
				m_YAxis[nYAxis].m_Placement = CXGraphAxis::apLeft;
			}
		}
	}

	// Y-Axis, left side
	for (size_t nYAxis = 0; nYAxis < m_YAxis.size (); nYAxis++)
	{
		if (m_YAxis[nYAxis].m_Placement != CXGraphAxis::apLeft)
			continue;
	
		int nTickWidth  = m_YAxis[nYAxis].GetMaxLabelWidth (pDC); 
		int nLabelWidth = (m_YAxis[nYAxis].GetTitleSize (pDC).cx + m_YAxis[nYAxis].m_nArcSize);

		if (m_YAxis[nYAxis].m_bVisible)
			nLeft += (max (nTickWidth, nLabelWidth) + m_nAxisSpace );

		m_YAxis[nYAxis].m_nLeft   = nLeft;
		m_YAxis[nYAxis].m_clChart = clChartRect;
	}
	
	// Y-Axis, right side
	for (size_t nYAxis = 0; nYAxis < m_YAxis.size (); nYAxis++)
	{
		if (m_YAxis[nYAxis].m_Placement != CXGraphAxis::apRight)
			continue;

		int nTickWidth  = m_YAxis[nYAxis].GetMaxLabelWidth (pDC); 
		int nLabelWidth = m_YAxis[nYAxis].GetTitleSize (pDC).cx;
	
		if (m_YAxis[nYAxis].m_bVisible)
			nRight -= (max (nTickWidth, nLabelWidth) + m_nAxisSpace );
		
		m_YAxis[nYAxis].m_nLeft   = nRight;
		m_YAxis[nYAxis].m_clChart = clChartRect;
	}

	// X-Axis
	for (UINT nXAxis = 0; nXAxis < m_XAxis.size (); nXAxis++)
	{
		if (m_XAxis[nXAxis].m_bVisible)
			nBottom -= (m_XAxis[nXAxis].GetMaxLabelHeight (pDC) + m_nAxisSpace);

		m_XAxis[nXAxis].m_nTop    = nBottom;
		m_XAxis[nXAxis].m_nLeft   = nLeft;
		m_XAxis[nXAxis].m_clChart = clChartRect;
	}

	// Draw Inner Rect
	// 

	m_clInnerRect.SetRect (nLeft + 1, clChartRect.top, nRight, nBottom);
	pDC->FillSolidRect(m_clInnerRect, m_crInnerColor);
	
	// Draw axis
	// Y-Axis
	for (size_t nYAxis = 0; nYAxis < m_YAxis.size (); nYAxis++)
	{
		m_YAxis[nYAxis].m_nTop = nBottom;
		m_YAxis[nYAxis].Draw(pDC);
	}
	
	// X-Axis
	for (size_t nXAxis = 0; nXAxis < m_XAxis.size (); nXAxis++)
		m_XAxis[nXAxis].Draw(pDC);
	
	// Setup clipping
	CRgn  clipRegion;
	CRect rectClip(m_clInnerRect.left, m_clInnerRect.top, m_clInnerRect.right, m_clInnerRect.bottom);
	
	// Should move inside CDCEx in future
	if (pDC->m_bPrinting && !pDC->m_bPrintPreview)
		pDC->AdjustRatio (rectClip);

	
	clipRegion.CreateRectRgn (rectClip.left, rectClip.top, rectClip.right, rectClip.bottom);


	// PRB: Clipping doesn't work in print preview, no idea why ...
	pDC->SelectClipRgn (&clipRegion);
		

	// Draw curves
	for (UINT nCurve = 0; nCurve < m_vSeries.size (); nCurve++)
		if (m_vSeries[nCurve].m_bVisible)
			m_vSeries[nCurve].Draw(pDC);

	// Draw curve markers
	for (size_t nCurve = 0; nCurve < m_vSeries.size (); nCurve++)
		if (m_vSeries[nCurve].m_bVisible && m_vSeries[nCurve].m_bShowMarker)
			m_vSeries[nCurve].DrawMarker(pDC);

	// Draw zoom if active
	if (m_opOperation == opZoom && m_bLButtonDown)
		DrawZoom(pDC);
	

	// Draw cursor if active
	if (m_opOperation == opCursor)
		DrawCursor(pDC);

	// DrawObjects
	for (POSITION pos = m_Objects.GetHeadPosition (); pos != NULL; )
	{
		CXGraphObject *pObject = (CXGraphObject*) m_Objects.GetNext (pos);
		pObject->Draw (pDC);
	}

	if (m_bDataPointMoving)
	{
		CQuickFont font("Arial", 13, FW_NORMAL);
		CFontSelector fs(&font, pDC, false);
		pDC->SetBkMode (TRANSPARENT);
		CString cPoint;
		cPoint.Format ("%.2f", m_fCurrentEditValue);
		pDC->TextOut(m_CurrentPoint.x + 10, m_CurrentPoint.y, cPoint);
	}

	for (size_t i=0; i<m_vGraphLines.size(); ++i)	// line들을 그린다. [9/30/2013 ChoiJunHyeok]
		m_vGraphLines[i].Draw(pDC);

	if (m_bShowLegend)	// legend 그린다 [9/30/2013 ChoiJunHyeok]
		DrawLegend(pDC, clChartRect);

	pDC->RestoreDC(nSaveDC);
	pDC->Detach();

	delete pDC;

	if (pmdc)
		delete pmdc;
	
	delete pdc;

	
}


#endif

void CXGraph::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	Invalidate(TRUE);
}

#if _MFC_VER >= 0x0700
BOOL CXGraph::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
#else
void CXGraph::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
#endif
{
	if (nFlags & MK_CONTROL)
	{
		if(zDelta > 0)
		{
			this->CameraZoomIn();
		}
		else
		{
			this->CameraZoomOut();
		}
	}
	else if (nFlags & MK_SHIFT)
	{
		for (int y = 0; y < GetYAxisCount(); y++)
		{
			m_YAxis[y].SetAutoScale(false);
			double fStep = (m_YAxis[y].m_fCurMax - m_YAxis[y].m_fCurMin) / 10.0;
			if (zDelta < 0)
				m_YAxis[y].SetCurrentRange(m_YAxis[y].m_fCurMin - fStep,m_YAxis[y].m_fCurMax - fStep);
			else
				m_YAxis[y].SetCurrentRange(m_YAxis[y].m_fCurMin + fStep,m_YAxis[y].m_fCurMax + fStep);
		}
	}
	else
	{
		for (int x = 0; x < GetXAxisCount(); x++)
		{
			m_XAxis[x].SetAutoScale(false);
			double fStep = (m_XAxis[x].m_fCurMax - m_XAxis[x].m_fCurMin) / 10.0;
			if (zDelta < 0)
				m_XAxis[x].SetCurrentRange(m_XAxis[x].m_fCurMin - fStep,m_XAxis[x].m_fCurMax - fStep);
			else
				m_XAxis[x].SetCurrentRange(m_XAxis[x].m_fCurMin + fStep,m_XAxis[x].m_fCurMax + fStep);
		}
	}

	Invalidate(TRUE);

#if _MFC_VER >= 0x0700
	return TRUE;
#endif
}


void CXGraph::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_bLButtonDown = (nFlags & MK_LBUTTON);


	m_CurrentPoint = point;
		
	if (m_opOperation == opCursor)
	{
		if(m_clInnerRect.PtInRect (point) == FALSE) return;

		m_bTmpDC = true;
		CDCEx dc;
		dc.Attach(GetDC()->m_hDC);
		DrawCursor(&dc);
		ReleaseDC(&dc);
		dc.Detach ();
		m_bTmpDC = false;
		::PostMessage(GetParent()->m_hWnd, XG_CURSORMOVED, 0, (long) this);

		Invalidate();
	}	

	if(m_opOperation == opPeak)	// peak remove 사각형을 계속 그리도록 한다. [10/22/2013 ChoiJunHyeok]
	{
		Invalidate();
	}
	


	m_bObjectSelected = CheckObjectSelection(false, m_opOperation != opEditCurve);

	if (m_bLButtonDown && m_opOperation == opPan && m_clInnerRect.PtInRect (point))
	{
		if (m_OldPoint == CPoint(0,0))
			m_OldPoint = point;
		else
			DoPan(point);
	}

	// 우클릭 pan 막음 [9/27/2013 ChoiJunHyeok]
	//if (m_bRButtonDown && m_opOperation == opCursor && m_clInnerRect.PtInRect (point))
	//{
	//	/*if (m_OldPoint == CPoint(0,0))
	//		m_OldPoint = point;
	//	else*/
	//		DoPan(point);
	//}
	Graphics graphics(GetDC()->m_hDC);		//  [10/4/2013 WHLEE]
	if (m_bLButtonDown && m_opOperation == opZoom /*&& m_clInnerRect.PtInRect (point)*/)
	{
		if (m_bDoubleBuffer)
			Invalidate();
		else
		{
			CDCEx dc;
			dc.Attach(GetDC()->m_hDC);

			#ifdef GDI_DRAW
				DrawZoom(graphics);			
			#else
				DrawZoom(&dc);		
			#endif

			ReleaseDC(&dc);
			dc.Detach ();
		}
	}
	
	CWnd::OnMouseMove(nFlags, point);
}


void CXGraph::AdjustPointToData(CPoint& point)
{
	int  nYDistance = 10000000;
	int  nY = point.y;
	long nIndex;

	for (int i = 0; i < m_vSeries.size(); i++)
	{
		// Get xval for current position
		double fSnappedXVal = m_XAxis[m_vSeries[i].m_nXAxis].GetValueForPos (point.x);
		// Find index for this value
		m_XAxis[m_vSeries[i].m_nXAxis].GetIndexByXVal(nIndex, fSnappedXVal, i);
		
		int y = m_YAxis[m_vSeries[i].m_nYAxis].GetPointForValue(&m_vSeries[i].m_vPointData[nIndex]).y;
				
		if (abs(point.y - y) < nYDistance)
		{
			nYDistance = abs(point.y - y);
			nY = y;
			if (point == m_MouseDownPoint)
			{
				m_nSnappedCurve = i;
				m_MeasureDef.nIndex1 = nIndex;
				m_MeasureDef.nCurve1 = i;
			}
			else
			{
				m_nSnappedCurve1 = i;
				m_MeasureDef.nIndex2 = nIndex;
				m_MeasureDef.nCurve2 = i;
			}

		}
	}

	point.y = nY;
	
}



bool CXGraph::CheckObjectSelection(bool bEditAction, bool bCheckFocusOnly)
{
	UINT i;
	static int nXMarkerMoving = -1;
//	static int nYMarkerMoving = -1;

	AFX_MANAGE_STATE(AfxGetModuleState( ));

	bool bHasNotified = false;

	if (!m_bInteraction)
		return false;

			 
	// Check x axis selection
	for (i = 0; i < m_XAxis.size(); i++)
	{
		// Check for Marker selections
		if (m_opOperation == opNone)	// 선택모드일 때 데이터 선택 체크 [10/8/2013 ChoiJunHyeok]
		{
			//m_XAxis[i].m_vAxisMarkers[nXMarkerMoving].BackupXvalue();

			for (int nMarker = 0; nMarker < m_XAxis[i].m_vAxisMarkers.size(); nMarker++)
			{
				CRect	   hitRect;
				TDataPoint dPoint;

				if (nXMarkerMoving == -1)
				{
					dPoint.fXVal = m_XAxis[i].m_vAxisMarkers[nMarker].m_lfXValue;
					dPoint.fYVal = m_XAxis[i].m_vAxisMarkers[nMarker].m_lfYValue;
					CPoint point = m_XAxis[i].GetPointForValue (&dPoint);

					hitRect.SetRect(point.x - 10, m_clInnerRect.top, point.x + 10, m_clInnerRect.bottom);

					if (bCheckFocusOnly && nXMarkerMoving == -1 && !m_bLButtonDown)
						if (hitRect.PtInRect (m_CurrentPoint))
							return true;
						else
							continue;
				}

				if(nXMarkerMoving != -1)
				{
					m_XAxis[i].m_vAxisMarkers[nXMarkerMoving].MoveNotation();
				}


					
				if (m_bLButtonDown && nXMarkerMoving == -1 && hitRect.PtInRect (m_CurrentPoint))	// 마우스 클릭시 히트된 마커 찾는다 [10/16/2013 ChoiJunHyeok]
				{
					printf("find marker : %d\n", nMarker);
					nXMarkerMoving = nMarker;
					m_nSelectedMarker= nMarker;	// 선택된 마커는 최근 선택한 마커이다. [10/21/2013 ChoiJunHyeok]
					UINT nCurrMovingMarkerGroup= m_XAxis[i].m_vAxisMarkers[nXMarkerMoving].m_nSliceNumber;

					m_XAxis[i].m_vAxisMarkers[nXMarkerMoving].BackupXvalue();

					RemoveInvolvedObjects(nCurrMovingMarkerGroup);					
				}


				if (!m_bLButtonDown && nXMarkerMoving != -1)	// 마커 움직이다가 마우스떼면 [10/16/2013 ChoiJunHyeok]
				{
					printf("marker up\n");
					double lfCurrXValue= m_XAxis[i].GetValueForPos (m_CurrentPoint.x);	// 현재 x 위치 가져옴

					double lfValue;
					UINT nGroupNumber= m_XAxis[i].m_vAxisMarkers[nXMarkerMoving].m_nSliceNumber;
					UINT eLineType= m_XAxis[i].m_vAxisMarkers[nXMarkerMoving].m_eLineType;
					BOOL bCurrLineIsStartLine;
					BOOL bPass= FALSE;

					if(eLineType==LINE_SLICE_START)
						bCurrLineIsStartLine= TRUE;
					else if(eLineType==LINE_SLICE_END)
						bCurrLineIsStartLine= FALSE;




					BOOL bSliceMoveErr= FALSE;
					if(eLineType==LINE_EVAL) // eval이면 검사 안 함 [10/24/2013 ChoiJunHyeok]
					{
						m_XAxis[i].MoveXAxisMarker(nXMarkerMoving, lfCurrXValue);
					}
					else
					{
						BOOL bFindPair= m_XAxis[i].SearchPairValue(nGroupNumber, bCurrLineIsStartLine, lfValue);	// 현재 움직이는 라인 반대쪽 값 가져옴
						
						if(bCurrLineIsStartLine)	// 현재 움직인 라인이 시작이면
						{
							if(lfValue > lfCurrXValue)	// 시작값보다 끝값이 작을 때만 패스
								bPass= TRUE;
						}
						else	// 끝이면
						{
							if(lfValue < lfCurrXValue)	// 시작값보다 끝값이 클 때만 패스
								bPass= TRUE;
						}

						if(bPass)
						{
							m_XAxis[i].MoveXAxisMarker(nXMarkerMoving, lfCurrXValue); 	// 없는 데이터를 표시하지 않도록 조절함 [10/17/2013 ChoiJunHyeok]
						}
						else
						{
							if(!bFindPair)	// 짝이없으면 error검사 안 함.
							{
								m_XAxis[i].MoveXAxisMarker(nXMarkerMoving, lfCurrXValue); 	// 없는 데이터를 표시하지 않도록 조절함 [10/17/2013 ChoiJunHyeok]
							}
							else
							{
								bSliceMoveErr= TRUE;
								double lfXoldValue= m_XAxis[i].m_vAxisMarkers[nXMarkerMoving].RestoreXvalue();	// 마커를 옮기기 전으로 다시 이동
								m_pFrame->OnSendMessage("Marker moving error.", TRUE);
								m_XAxis[i].MoveXAxisMarker(nXMarkerMoving, lfXoldValue); 	// 없는 데이터를 표시하지 않도록 조절함 [10/17/2013 ChoiJunHyeok]
							}
						}
					}

					if(!bSliceMoveErr)
					{	// 슬라이스 옮긴거 체크 [11/12/2013 ChoiJunHyeok]

						CMarkerInfo* markerInfo= new CMarkerInfo();						
						markerInfo->lfOldxValue= m_XAxis[i].m_vAxisMarkers[nXMarkerMoving].GetBackupXValue();
						markerInfo->lfCurrXValue= m_XAxis[i].m_vAxisMarkers[nXMarkerMoving].m_lfXValue;
						markerInfo->nMarkerType= m_XAxis[i].m_vAxisMarkers[nXMarkerMoving].m_eLineType;
						::PostMessage(GetParent()->m_hWnd, XG_MOVEMARKER, (WPARAM)markerInfo, (long) this);
					}

					nXMarkerMoving = -1;	// 선택 라인 초기화					
				}

				if (nXMarkerMoving != -1 && m_bLButtonDown)	// 마커 옮길 때 값 바꿈 [10/8/2013 ChoiJunHyeok]
				{
					printf("marker moving\n");
					m_XAxis[i].m_vAxisMarkers[nXMarkerMoving].m_lfXValue = m_XAxis[i].GetValueForPos (m_CurrentPoint.x);
					
					this->SendStandardRightViewToTime(i);	// standardView일때 evaluate maker들 시간 전송함. [10/16/2013 ChoiJunHyeok]

					//m_XAxis[i].m_vAxisMarkers[nXMarkerMoving].m_lfYValue = m_YAxis[i].GetValueForPos (nYPos);
					Invalidate();
				}
				

			}
		}
		
	
		if (bCheckFocusOnly)
			if (m_XAxis[i].m_clRect.PtInRect (m_CurrentPoint))
				return true;
			else
				continue;
		
		
		m_XAxis[i].m_bSelected = m_XAxis[i].m_clRect.PtInRect (m_CurrentPoint);

		if (m_XAxis[i].m_bSelected && !bHasNotified)
		{
			bHasNotified = true;
			::PostMessage(GetParent()->m_hWnd, bEditAction ? XG_XAXISDBLCLICK : XG_XAXISCLICK, i, (long) this);

			if (bEditAction && m_bInteraction)
			{
				CChartPage dlg;
				dlg.m_pGraph = this;
				dlg.m_pGraphAxis = &m_XAxis[i];
				dlg.DoModal();		
				Invalidate();
			}
		}
	}

	bool bCheckCurves = true;

	// Check object selection
	for (POSITION pos = m_Objects.GetHeadPosition (); pos != NULL; )
	{
		CXGraphObject *pObject = (CXGraphObject*) m_Objects.GetNext (pos);

		if (bCheckFocusOnly)
			if (pObject->m_clRect.PtInRect (m_CurrentPoint) && pObject->m_bVisible)
				return true;
			else
				continue;
		
		pObject->m_bSelected = (pObject->m_clRect.PtInRect (m_CurrentPoint) && pObject->m_bVisible);
		
		if (bEditAction && pObject->m_bSelected )
		{
			bHasNotified = true;
			bCheckCurves = false;
			pObject->Edit ();
		}
		
		if (pObject->m_bSelected && !pObject->m_bEditing && m_pTracker == NULL)
		{
			bHasNotified = true;
			pObject->BeginSize ();
			m_pTracker = &pObject->m_Tracker; 
			bCheckCurves = false;
		}

		if (!pObject->m_bSelected && pObject->m_bEditing)
			pObject->EndEdit ();

		if (!pObject->m_bSelected && pObject->m_bSizing)
		{
			m_pTracker = NULL;
			pObject->EndSize ();
		}
		
	}

	if (!bCheckFocusOnly)
		m_nSelectedSerie = -1;

	
	// Special check for moving datapoint in edit mode
	static int  nEditSerie = -1;
	static long nXIndex = -1;

	// Reset previous stored informations about last selected curve and index
	if (!m_bLButtonDown)
	{
		nXIndex = -1;
		nEditSerie = -1;
	}

	
	// check curve selection
	for (i = 0; i < m_vSeries.size(); i++)
	{
		CXGraphDataSerie& serie = m_vSeries[i];
		
		if (!serie.m_bVisible)
			continue;

		if (!bCheckFocusOnly)
			serie.m_bSelected = false;

		if (!bCheckCurves)
			continue;
		

		for (int j = 0; j < serie.m_CurveRegions.size(); j++)
		{
			CRect hitRect(serie.m_CurveRegions[j].p1.x, serie.m_CurveRegions[j].p1.y,serie.m_CurveRegions[j].p2.x, serie.m_CurveRegions[j].p2.y);
			hitRect.NormalizeRect ();
			hitRect.InflateRect (2,2,2,2);
			
			if (bCheckFocusOnly)
				if (hitRect.PtInRect (m_CurrentPoint) && serie.HitTestLine(serie.m_CurveRegions[j].p1, serie.m_CurveRegions[j].p2, m_CurrentPoint, 4))
					return true;
				else
					continue;
			
			if (hitRect.PtInRect (m_CurrentPoint) && serie.HitTestLine(serie.m_CurveRegions[j].p1, serie.m_CurveRegions[j].p2,m_CurrentPoint, 4) && !bHasNotified)
			{
				::PostMessage(GetParent()->m_hWnd, bEditAction ? XG_CURVEDBLCLICK : XG_CURVECLICK, i, (long) this);
				m_nSelectedSerie = i;

				// User is editing a datapoint
				if (nXIndex == -1)
				{
					nEditSerie = i;
					double fX = m_XAxis[serie.GetXAxis ()].GetValueForPos (m_CurrentPoint.x - serie.m_nMarkerSize / 2);
					// Remember index of moving point
					GetXAxis(serie.GetXAxis ()).GetIndexByXVal (nXIndex, fX, i);
				}
				
				serie.m_bSelected = true;
				bHasNotified = true;
#ifndef _WIN32_WCE
				if (bEditAction && m_bInteraction)
				{
					CXGraphDataSerie& serie = m_vSeries[i];
					m_nSelectedSerie = i;

					CChartPage dlg;
					dlg.m_pGraph = this;
					dlg.m_pGraphDataSerie = &serie;
					dlg.DoModal();			
					Invalidate();

					//CChartPage dlg;
					//dlg.m_pGraph = this;
					//dlg.m_pGraphAxis = &m_YAxis[i];
					//dlg.DoModal();		
					//Invalidate();
				}
#endif
				break;
			}
		}
	}

	m_bDataPointMoving = false;

	if (m_bLButtonDown && nXIndex != -1 && m_opOperation == opEditCurve)
	{
		m_nmCurrentPointMoving.nCurve = nEditSerie;
		m_nmCurrentPointMoving.nIndex = nXIndex;
		m_nmCurrentPointMoving.dOldVal = m_vSeries[nEditSerie].m_vPointData[nXIndex].fYVal;
		m_nmCurrentPointMoving.dNewVal = GetYAxis(m_vSeries[nEditSerie].GetYAxis()).GetValueForPos (m_CurrentPoint.y);
		
		// Inform parent
		::PostMessage(GetParent()->m_hWnd, XG_POINTCHANGED, (long)&m_nmCurrentPointMoving, (long) this);

		m_vSeries[nEditSerie].m_vPointData[nXIndex].fYVal = GetYAxis(m_vSeries[nEditSerie].GetYAxis()).GetValueForPos (m_CurrentPoint.y);
		m_fCurrentEditValue = m_vSeries[nEditSerie].m_vPointData[nXIndex].fYVal;
		m_bDataPointMoving = true;
		
		Invalidate();
	}

	if (!bHasNotified)
	for (i = 0; i < m_SelectByMarker.size(); i++)
	{
		bool bSelected = m_SelectByMarker[i].markerRect.PtInRect (m_CurrentPoint);
		
		if (bCheckFocusOnly && bSelected)
			return true;
		
		m_SelectByMarker[i].pObject->m_bSelected = bSelected;

		if (bSelected)
		{
			bHasNotified = true;
#ifndef _WIN32_WCE
			if (bEditAction && m_bInteraction)
			{
				CXGraphDataSerie& serie = m_vSeries[i];
				m_nSelectedSerie = i;

				CChartPage dlg;
				dlg.m_pGraph = this;
				dlg.m_pGraphDataSerie = &serie;
				dlg.DoModal();			
				Invalidate();
			}
#endif
			break;
		}
	}

	if (!bHasNotified && bEditAction && m_bInteraction)
	{
		::PostMessage(GetParent()->m_hWnd, XG_GRAPHDBLCLICK, 0, (long) this);
		
		CChartPage dlg;
		dlg.m_pGraph = this;
		dlg.DoModal();
		Invalidate();
	}

		
	return false;
}

	// curr->move + 타입확인 마커를 옮긴다. [11/12/2013 ChoiJunHyeok]
void CXGraph::MoveXMarker(double lfCurrValue, double lfMoveValue, UINT nMarkerType)
{
	for (size_t i=0; i<m_XAxis.size(); ++i)
	{
		for (size_t j=0; j<m_XAxis[i].m_vAxisMarkers.size(); ++j)
		{
			if(m_XAxis[i].m_vAxisMarkers[j].m_eLineType != nMarkerType)	// 마커 타입이 다르면 거부 [11/12/2013 ChoiJunHyeok]
				continue;

			if(m_XAxis[i].m_vAxisMarkers[j].m_lfXValue == lfCurrValue)	// x값이 같은 marker이면 [11/12/2013 ChoiJunHyeok]
			{
				m_XAxis[i].m_vAxisMarkers[j].m_lfXValue= lfMoveValue;	// x 옮김. [11/12/2013 ChoiJunHyeok]
				m_XAxis[i].MoveXAxisMarker( j, lfMoveValue);
				this->Invalidate();
			}
		}
	}
}
void CXGraph::ShowChartProperty()
{
	CChartPage dlg;
	dlg.m_pGraph = this;
	dlg.DoModal();
	Invalidate();
}

//CXGraphDataNotation* CXGraph::InsertDataNotation(int nCurve, double lfX, double lfY)
//{
//	CString cText, cXFmt, cYFmt, cFmt;
//
//	cFmt  = "X : %s, Y : %s";
//	cXFmt.Format("%lf",);
//	cYFmt.Format("%lf",);
//	cText.Format(cFmt, cXFmt, cYFmt);
//
//	CXGraphDataNotation *pNotation = (CXGraphDataNotation*) new CXGraphDataNotation;
//
//	pNotation->m_fXVal  = lfX
//	pNotation->m_fYVal  = lfY
//	pNotation->m_cText  = cText;
//	pNotation->m_nCurve = nCurve;
//	pNotation->m_nIndex = nIndex;
//	pNotation->m_pGraph = this;
//
//	m_Objects.AddTail (pNotation);
//
//	Invalidate();
//
//	return pNotation;	// 만든 notation pointer [10/24/2013 ChoiJunHyeok]
//}

CXGraphDataNotation* CXGraph::InsertDataNotation(int nCurve, int nIndex)
{
	CString cText, cXFmt, cYFmt, cFmt;

	CXGraphAxis _currXaxis= m_XAxis[m_vSeries[nCurve].m_nXAxis];
	CXGraphAxis _currYaxis= m_YAxis[m_vSeries[nCurve].m_nYAxis];

	bool bXDT = _currXaxis.m_bDateTime;
	bool bYDT = _currYaxis.m_bDateTime;

	cXFmt = "%s";
	cYFmt = "%s";
	//cFmt  = "[%d] : " + cXFmt + "%s," + cYFmt + "%s";
	cFmt  = "X : %s, Y : %s";

	/*if (bXDT)
		cXFmt = COleDateTime(m_vSeries[nCurve].m_vPointData[nIndex].fXVal).Format(m_XAxis[m_vSeries[nCurve].m_nXAxis].m_cDisplayFmt);
	else*/

	if(_currXaxis.GetIRIGTime())
	{
		int nDay, nHour, nMin, nSec, nMSec;

		double lfDays= m_vSeries[nCurve].m_vPointData[nIndex].fXVal / (60*60*24);
		nDay= (int)lfDays;

		double lfHours= (lfDays-(double)nDay) *24;
		nHour= (int)lfHours;

		double lfMins= (lfHours-(double)nHour) *60;
		nMin= (int)lfMins;

		double lfSecs= (lfMins-(double)nMin) *60;
		nSec= (int)lfSecs;

		double lfM100Sec = floor(((lfSecs-(double)nSec)*1000) + 0.5);		
		nMSec= (int)lfM100Sec;

		//szValue.Format("%03d:%02d:%02d:%02d.%03d", nDay, nHour, nMin, nSec, nSec);
		cXFmt.Format("%02d:%02d:%02d.%d",nHour, nMin, nSec, nSec);
	}
	else
	{
		cXFmt.Format(_currXaxis.m_cDisplayFmt, m_vSeries[nCurve].m_vPointData[nIndex].fXVal);
	}
		

	/*if (bYDT)
		cYFmt = COleDateTime(m_vSeries[nCurve].m_vPointData[nIndex].fYVal).Format(m_YAxis[m_vSeries[nCurve].m_nYAxis].m_cDisplayFmt);
	else*/
		cYFmt.Format(_currYaxis.m_cDisplayFmt, m_vSeries[nCurve].m_vPointData[nIndex].fYVal);

	/*cText.Format(cFmt, nIndex + 1,
				 cXFmt, m_XAxis[m_Data[nCurve].m_nXAxis].m_cLabel,
				 cYFmt, m_YAxis[m_Data[nCurve].m_nYAxis].m_cLabel);*/
	cText.Format(cFmt, cXFmt, cYFmt);
	
	CXGraphDataNotation *pNotation = (CXGraphDataNotation*) new CXGraphDataNotation;
	
	pNotation->m_fXVal  = m_vSeries[nCurve].m_vPointData[nIndex].fXVal;
	pNotation->m_fYVal  = m_vSeries[nCurve].m_vPointData[nIndex].fYVal;
	pNotation->m_cText  = cText;
	pNotation->m_nCurve = nCurve;
	pNotation->m_nIndex = nIndex;
	pNotation->m_pGraph = this;
	
	m_Objects.AddTail (pNotation);

	Invalidate();

	return pNotation;	// 만든 notation pointer [10/24/2013 ChoiJunHyeok]
}

void CXGraph::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_bLButtonDown)
		return;

	ReleaseCapture();

	m_OldPoint     = CPoint(0,0);
	m_MouseUpPoint = point;
	m_bLButtonDown = false;


	if (m_opOperation == opCursor)
	{
		AddEvaluateMarker();

		
	}

	if (m_opOperation == opSlice)
	{
		::PostMessage(GetParent()->m_hWnd, XG_ADDSLICE, 0, (long) this);	// slice message 보낸다. [11/12/2013 ChoiJunHyeok]
	}

	if(m_opOperation==opPeak)	// 마우스 떼면 선택된 부분을 제거한다 [10/22/2013 ChoiJunHyeok]
	{

		PeakRemove();
	}
	
	if (m_opOperation == opZoom)
		DoZoom(TRUE, TRUE);
	
	if (m_opOperation == opNone || m_opOperation == opEditCurve)
		CheckObjectSelection();

	Invalidate();

	CWnd::OnLButtonUp(nFlags, point);
}

void CXGraph::OnLButtonDown(UINT nFlags, CPoint point) 
{

	SetFocus();
	SetCapture();

	m_MouseDownPoint = point;
	m_MouseDownPoint = point;
		
	m_bLButtonDown   = true;
	m_pCurrentObject = NULL;

	if(m_opOperation==opRange)	// 라인 클리어 [10/8/2013 ChoiJunHyeok]
	{
		m_vGraphLines.clear();
		m_Objects.RemoveAll();
	}
			
	// Check objects
	for (POSITION pos = m_Objects.GetHeadPosition (); pos != NULL; )
	{
		CXGraphObject *pObject = (CXGraphObject*) m_Objects.GetNext (pos);

		if (pObject->m_clRect.PtInRect(point))
			m_pCurrentObject = pObject;
				
		if (pObject->m_bSizing && pObject->m_Tracker.HitTest(point) != CRectTracker::hitNothing)
		{
			ReleaseCapture();
			
			if (pObject->m_Tracker.Track(this, point) && pObject->m_bCanMove && pObject->m_bCanResize)
			{
				pObject->m_clRect = pObject->m_Tracker.m_rect;
				Invalidate(FALSE);
			}
		}
	}

	//for (size_t i=0; i<m_XAxis.size(); ++i)
	//{
	//	for (int nMarker = 0; nMarker < m_XAxis[i].m_vAxisMarkers.size(); nMarker++)
	//	{
	//		CRect	   hitRect;
	//		TDataPoint dPoint;

	//		hitRect.SetRect(point.x - 10, m_clInnerRect.top, point.x + 10, m_clInnerRect.bottom);

	//		if (m_bLButtonDown && hitRect.PtInRect (m_CurrentPoint))	// 마우스 클릭시 히트된 마커 찾는다 [10/16/2013 ChoiJunHyeok]
	//		{
	//			printf("L-button down :: find marker : %d\n", nMarker);
	//			m_nSelectedMarker= nMarker;	// 선택된 마커는 최근 선택한 마커이다. [10/21/2013 ChoiJunHyeok]
	//		}
	//	}
	//}

	

	CWnd::OnLButtonDown(nFlags, point);
}

BOOL CXGraph::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CXGraph::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (m_opOperation == opNone)
	{
		m_pTracker = NULL;
		CheckObjectSelection(true);
	}
	
	CWnd::OnLButtonDblClk(nFlags, point);
}

BOOL CXGraph::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{	
	if (m_pTracker && m_pTracker->SetCursor(pWnd,  nHitTest))
        return TRUE;

	if (m_opOperation == opPan)
	{
		HCURSOR hCur = AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
		if (hCur)
		{
			::SetCursor(hCur);			 
			return TRUE;
		}
	}
	else if (m_opOperation == opZoom || m_opOperation == opCursor)
	{
		HCURSOR hCur = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
		if (hCur)
		{
			::SetCursor(hCur);			 
			return TRUE;
		}
	}
	else if (m_bObjectSelected && m_opOperation == opNone)
	{
		HCURSOR hCur = AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(IDC_HAND));
		if (hCur)
		{
			::SetCursor(hCur);			 
			return TRUE;
		}
	}
	else if (m_opOperation == opEditCurve && m_nSelectedSerie != -1)
	{
		HCURSOR hCur = AfxGetApp()->LoadStandardCursor(IDC_SIZENS);
		if (hCur)
		{
			::SetCursor(hCur);			 
			return TRUE;
		}
	}

	
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CXGraph::PreTranslateMessage(MSG* pMsg) 
{
	if ((pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP) && pMsg->wParam == VK_DELETE)
	{
		if (m_pCurrentObject && m_pCurrentObject->m_bSizing)
		{
			if (m_Objects.GetHead() != m_pCurrentObject)
			{
				m_Objects.RemoveAt(m_Objects.Find(m_pCurrentObject));
				delete m_pCurrentObject;
				m_pCurrentObject = NULL;
				m_pTracker = NULL;
				Invalidate();
			}
		}
	}
		
	return CWnd::PreTranslateMessage(pMsg);
}

#ifndef _WIN32_WCE
void CXGraph::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CheckObjectSelection();

	m_bRButtonDown = false;

	if (m_opOperation == opCursor)
	{
		m_oldCursorPoint = CPoint(-1,-1);
		Invalidate();
	}

	if (m_pCurrentObject && m_pCurrentObject->m_clRect.PtInRect (point))
		m_pCurrentObject->InvokeProperties();
	else
		::PostMessage(GetParent()->m_hWnd, XG_RBUTTONUP, 0, (long) this);
		

	CWnd::OnRButtonUp(nFlags, point);
}
#endif

void CXGraph::InsertEmptyLabel()
{
	InsertLabel();
	Invalidate();
}

void CXGraph::OnProperties()
{
	AFX_MANAGE_STATE(AfxGetModuleState( ));
	CheckObjectSelection(true);
}

// 데이터 range를 변경한다. [8/29/2013 ChoiJunHyeok]
void CXGraph::YAxisReset(double lfMin, double lfMax)
{
	for (size_t i=0; i<GetYAxisCount(); ++i)
	{
		GetYAxis(i).SetRange(lfMin, lfMax);
	}	
}

void CXGraph::GetYAxisRange(double& lfMin, double& lfMax)
{
	double lfStep= 0.0f;
	GetYAxis(0).GetRange(lfMin, lfMin, lfStep);
}


// 데이터 곱함
void CXGraph::DataMultiplication(double _lfFactor)
{
	Data()->GetSeriesData()[0]->MultiplicationY(_lfFactor);
	m_Objects.RemoveAll();
	//this->SetDrawAverage(FALSE);
}


// 데이터를 offset만큼 더한다. [8/29/2013 ChoiJunHyeok]
void CXGraph::DataOffset(double _lfOffsetValue)
{
	Data()->GetSeriesData()[0]->PlusY(_lfOffsetValue);
	m_Objects.RemoveAll();
	//this->SetDrawAverage(FALSE);
}


#ifndef _WIN32_WCE
void CXGraph::OnPrint()
{
	AFX_MANAGE_STATE(AfxGetModuleState());

	CPrintDialog dlg(FALSE);

	if (dlg.DoModal () == IDOK)
	{
		CDCEx dc;
		
		dc.Attach (dlg.GetPrinterDC ());

		dc.Prepare (dc.m_hAttribDC);

		int nWidth  = dc.GetDeviceCaps (HORZRES);
		int nHeight = dc.GetDeviceCaps (VERTRES);

		m_clPrintRect.SetRect(0, 0, nWidth / dc.m_fScaleX, nHeight / dc.m_fScaleY);

		m_pPrintDC = &dc;
		m_pPrintDC->m_bPrinting = true;
		m_pPrintDC->StartDoc ("Graph"); 
		m_pPrintDC->StartPage ();
		OnPaint();
		m_pPrintDC->EndPage ();
		m_pPrintDC->EndDoc ();
		m_pPrintDC = NULL;
		dc.Detach ();
	}
	
	//Invalidate();
}

void CXGraph::PrintGraph(CDC *pDC)
{ 
	CDCEx dc;
	
	dc.Attach (pDC->m_hDC);

	dc.Prepare (pDC->m_hAttribDC);
	
	int nWidth, nHeight;

	if (pDC->m_hDC == pDC->m_hAttribDC)
	{
		m_pPrintDC = &dc;
		nWidth  = pDC->GetDeviceCaps (HORZRES);
		nHeight = pDC->GetDeviceCaps (VERTRES);
		dc.m_bPrintPreview = false;
		dc.m_bPrinting = true;
	    DOCINFO docinfo;
	    memset(&docinfo, 0, sizeof(docinfo));
	    docinfo.cbSize = sizeof(docinfo);
        docinfo.lpszDocName = _T("Graph");
		int nRet;//= dc.StartDoc(&docinfo);
		nRet = GetLastError();
		nRet = dc.StartPage();
		m_clPrintRect.SetRect(0, 0, nWidth / dc.m_fScaleX, nHeight / dc.m_fScaleY);
		OnPaint();
		dc.EndPage ();
		dc.EndDoc ();
		m_pPrintDC = NULL;

	}
	else
	{
		nWidth  = ::GetDeviceCaps (pDC->m_hAttribDC ,HORZRES);
		nHeight = ::GetDeviceCaps (pDC->m_hAttribDC ,VERTRES);
		dc.m_bPrintPreview = true;
		m_clPrintRect.SetRect(0, 0, nWidth / dc.m_fScaleX, nHeight / dc.m_fScaleY);
		m_pPrintDC = &dc;
		m_pPrintDC->m_bPrinting = true;
		OnPaint();
		m_pPrintDC = NULL;
	}
}

void CXGraph::PrintGraph(CDC *pDC, CRect printRect)
{ 
	CDCEx dc;
	
	dc.Attach (pDC->m_hDC);

	dc.Prepare (pDC->m_hAttribDC);

	int nWidth, nHeight;

	nWidth  = pDC->GetDeviceCaps (HORZRES);
	nHeight = pDC->GetDeviceCaps (VERTRES);
		
	if (pDC->m_hDC == pDC->m_hAttribDC)
	{
		m_pPrintDC = &dc;
		dc.m_bPrintPreview = false;
		dc.m_bPrinting = true;
	    DOCINFO docinfo;
	    memset(&docinfo, 0, sizeof(docinfo));
	    docinfo.cbSize = sizeof(docinfo);
        docinfo.lpszDocName = _T("Graph");
		int nRet;//= dc.StartDoc(&docinfo);
		nRet = GetLastError();
		nRet = dc.StartPage();
		m_clPrintRect.SetRect(printRect.left / dc.m_fScaleX, printRect.top / dc.m_fScaleX, printRect.right / dc.m_fScaleX, printRect.bottom / dc.m_fScaleX);
		OnPaint();
		dc.EndPage ();
		dc.EndDoc ();
		m_pPrintDC = NULL;

	}
	else
	{
		dc.m_bPrintPreview = true;
		m_clPrintRect.SetRect(printRect.left / dc.m_fScaleX, printRect.top / dc.m_fScaleX, printRect.right / dc.m_fScaleX, printRect.bottom / dc.m_fScaleX);
		m_pPrintDC = &dc;
		m_pPrintDC->m_bPrinting = true;
		OnPaint();
		m_pPrintDC = NULL;
	}

}
#endif

void CXGraph::CubicTrend()
{
	AddCubicTrend(m_vSeries[m_nSelectedSerie]);
}

void CXGraph::LinearTrend()
{
	AddLinearTrend(m_vSeries[m_nSelectedSerie]);
}

void CXGraph::AddLinearTrend(CXGraphDataSerie& serie)
{
	TDataPoint* pTrend = serie.GetLinearTrend ();
	int			nCurveCount = GetCurveCount ();
	CString cLabel = serie.m_cLabel;
	CXGraphDataSerie &ret = SetCurveData (pTrend, serie.m_nCount, nCurveCount, serie.m_nXAxis, serie.m_nYAxis, true);
	ret.m_cLabel = cLabel + " linear trend";
	delete pTrend;
}

void CXGraph::AddCubicTrend(CXGraphDataSerie& serie)
{
	TDataPoint* pTrend = serie.GetCubicTrend ();
	int			nCurveCount = GetCurveCount ();
	CString cLabel = serie.m_cLabel;
	CXGraphDataSerie &ret = SetCurveData (pTrend, serie.m_nCount, nCurveCount, serie.m_nXAxis, serie.m_nYAxis, true);
	ret.m_cLabel = cLabel + " cubic trend";
	delete pTrend;
}


void CXGraph::DrawCursor(Graphics &graphics)
{
//	CPenSelector ps(0L, 1, pDC);
	long		 nIndex;
	TDataPoint   point;
//	int			 nOldROP2 = pDC->SetROP2 (R2_NOTXORPEN);

	m_nSnappedCurve = -1;

	if (m_nForcedSnapCurve != -1)
	{
		m_nSnappedCurve = m_nForcedSnapCurve;
		m_fSnappedXVal = m_XAxis[ m_vSeries[m_nForcedSnapCurve].m_nXAxis ].GetValueForPos(m_CurrentPoint.x);
		// Find index for this value
		m_XAxis[m_vSeries[m_nForcedSnapCurve].m_nXAxis].GetIndexByXVal(nIndex, m_fSnappedXVal, m_nForcedSnapCurve);
		// get yval for index
		m_fSnappedYVal = m_vSeries[m_nForcedSnapCurve].m_vPointData[nIndex].fYVal;
		point.fYVal = m_fSnappedYVal;

		//m_CurrentPoint.x = m_XAxis[ m_vSeries[m_nSnappedCurve].m_nXAxis ].GetPointForValue(&point).x;
		m_CurrentPoint.y = m_YAxis[ m_vSeries[m_nSnappedCurve].m_nYAxis ].GetPointForValue(&point).y;
	}
	else
	{
		if (m_bSnapCursor)
		{
			// Snap cursor to nearest curve
			for (int i = 0; i < m_vSeries.size(); i++)
			{
				// Get xval for current position
				m_fSnappedXVal = m_XAxis[m_vSeries[i].m_nXAxis].GetValueForPos (m_CurrentPoint.x);
				// Find index for this value
				m_XAxis[m_vSeries[i].m_nXAxis].GetIndexByXVal(nIndex, m_fSnappedXVal, i);
				// get yval for index
				m_fSnappedYVal = m_vSeries[i].m_vPointData[nIndex].fYVal;
				point.fYVal = m_fSnappedYVal;
				// Check if cursor is in snap-range
				int y = (int) m_YAxis[m_vSeries[i].m_nYAxis].GetPointForValue(&point).y;
				if (abs(m_CurrentPoint.y - y) < m_nSnapRange)
				{
					m_nSnappedCurve = i;
					m_CurrentPoint.y = y;
					break;
				}
			}
		}
	}

	Pen pen(GetGDIPlusColor(0),1);

	if (m_nCursorFlags & XGC_VERT)
	{
		graphics.DrawLine(&pen,m_CurrentPoint.x, m_clInnerRect.top,
							   m_CurrentPoint.x, m_clInnerRect.bottom);
	}

	if (m_nCursorFlags & XGC_HORIZ)
	{
		graphics.DrawLine(&pen,m_clInnerRect.left, m_CurrentPoint.y,
							   m_clInnerRect.right, m_CurrentPoint.y);
	}
	RectF rect(m_CurrentPoint.x - (m_nSnapRange / 2),
			   m_CurrentPoint.y - (m_nSnapRange / 2),
			   m_nSnapRange,m_nSnapRange);
	graphics.DrawRectangle(&pen,rect);
	m_oldCursorPoint = m_CurrentPoint;
	if (m_nCursorFlags & XGC_LEGEND)
		DrawCursorLegend(graphics);
}


void CXGraph::DrawRange(Graphics& graphics)
{
	if(m_opOperation == opPeak)
	{
		if(!m_bLButtonDown)
			return;

		//Color crLine(51,153,255);
		Color crLine(120,120,200);
		Color crFill(50,80,70,200);
		//Color crFill(100,168,202,236);

		//SolidBrush B1(crRect);
		SolidBrush B2(crFill);
		Pen P1(crLine, 0.8f);

		//m_CurrentPoint.x= 0; m_CurrentPoint.y= 0;

		graphics.DrawRectangle(&P1,
			m_MouseDownPoint.x,
			m_MouseDownPoint.y,
			m_CurrentPoint.x-m_MouseDownPoint.x,
			m_CurrentPoint.y-m_MouseDownPoint.y );

		graphics.FillRectangle(&B2,
			m_MouseDownPoint.x,
			m_MouseDownPoint.y,
			m_CurrentPoint.x-m_MouseDownPoint.x,
			m_CurrentPoint.y-m_MouseDownPoint.y);

		printf("\n\npeak rect\nleft : %d\ntop : %d\nW : %d\n%H : %d\n",
			m_MouseDownPoint.x, m_MouseDownPoint.y, m_CurrentPoint.x-m_MouseDownPoint.x, m_CurrentPoint.y-m_MouseDownPoint.y);
	}
	else if(m_opOperation ==  opRange)
	{
		Color crLine(120,120,200);
		Color crFill(80,80,120,200);

		//SolidBrush B1(crRect);
		SolidBrush B2(crFill);
		Pen P1(crLine, 0.8f);

		//m_CurrentPoint.x= 0; m_CurrentPoint.y= 0;

		graphics.FillRectangle(&B2,
			m_MouseDownPoint.x,
			m_clInnerRect.top,
			m_MouseUpPoint.x-m_MouseDownPoint.x,
			m_clInnerRect.bottom-m_clInnerRect.top);

		graphics.DrawLine(&P1, PointF(m_MouseDownPoint.x,m_clInnerRect.top), PointF(m_MouseDownPoint.x, m_clInnerRect.bottom));

		graphics.DrawLine(&P1, PointF(m_MouseUpPoint.x,m_clInnerRect.top), PointF(m_MouseUpPoint.x, m_clInnerRect.bottom));
	}
	
}

void CXGraph::DrawCursor(CDCEx* pDC)
{
	CPenSelector ps(0L, 1, pDC);
	long		 nIndex;
	TDataPoint   point;
	int			 nOldROP2 = pDC->SetROP2 (R2_NOTXORPEN);

	m_nSnappedCurve = -1;

	if ((m_nForcedSnapCurve != -1) && (!m_vSeries.empty()) )
	{
		m_nSnappedCurve = m_nForcedSnapCurve;
		m_fSnappedXVal = m_XAxis[m_vSeries[m_nForcedSnapCurve].m_nXAxis].GetValueForPos (m_CurrentPoint.x);
		// Find index for this value
		m_XAxis[m_vSeries[m_nForcedSnapCurve].m_nXAxis].GetIndexByXVal(nIndex, m_fSnappedXVal, m_nForcedSnapCurve);
		// get yval for index
		m_fSnappedYVal = m_vSeries[m_nForcedSnapCurve].m_vPointData[nIndex].fYVal;
		point.fYVal = m_fSnappedYVal;
		m_CurrentPoint.y = m_YAxis[m_vSeries[m_nSnappedCurve].m_nYAxis].GetPointForValue(&point).y;
	}
	else
	if (m_bSnapCursor)
	// Snap cursor to nearest curve
	for (int i = 0; i < m_vSeries.size(); i++)
	{
		// Get xval for current position
		m_fSnappedXVal = m_XAxis[m_vSeries[i].m_nXAxis].GetValueForPos (m_CurrentPoint.x);
		// Find index for this value
		m_XAxis[m_vSeries[i].m_nXAxis].GetIndexByXVal(nIndex, m_fSnappedXVal, i);
		// get yval for index
		m_fSnappedYVal = m_vSeries[i].m_vPointData[nIndex].fYVal;
		point.fYVal = m_fSnappedYVal;
		// Check if cursor is in snap-range
		int y = m_YAxis[m_vSeries[i].m_nYAxis].GetPointForValue(&point).y;
		if (abs(m_CurrentPoint.y - y) < m_nSnapRange)
		{
			m_nSnappedCurve = i;
			m_CurrentPoint.y = y;
			break;
		}
	}

	if (m_oldCursorPoint != CPoint(-1,-1))
	{
		if (m_nCursorFlags & XGC_VERT)
		{
			pDC->MoveTo (m_oldCursorPoint.x, m_clInnerRect.top);
			pDC->LineTo (m_oldCursorPoint.x, m_clInnerRect.bottom);
		}

		if (m_nCursorFlags & XGC_HORIZ)
		{
			pDC->MoveTo (m_clInnerRect.left, m_oldCursorPoint.y);
			pDC->LineTo (m_clInnerRect.right, m_oldCursorPoint.y);
		}

		// eval 커서 사각형
		pDC->Rectangle(m_oldCursorPoint.x - (m_nSnapRange / 2), m_oldCursorPoint.y - (m_nSnapRange / 2),
			m_oldCursorPoint.x + (m_nSnapRange / 2) + 1, m_oldCursorPoint.y + (m_nSnapRange / 2) + 1);
	}

	if (m_nCursorFlags & XGC_VERT)
	{
		pDC->MoveTo (m_CurrentPoint.x, m_clInnerRect.top);
		pDC->LineTo (m_CurrentPoint.x, m_clInnerRect.bottom);
	}

	if (m_nCursorFlags & XGC_HORIZ)
	{
		pDC->MoveTo (m_clInnerRect.left, m_CurrentPoint.y);
		pDC->LineTo (m_clInnerRect.right, m_CurrentPoint.y);
	}

	pDC->Rectangle(m_CurrentPoint.x - (m_nSnapRange / 2), m_CurrentPoint.y - (m_nSnapRange / 2),
		m_CurrentPoint.x + (m_nSnapRange / 2) + 1, m_CurrentPoint.y + (m_nSnapRange / 2) + 1);

	m_oldCursorPoint = m_CurrentPoint;	
	//pDC->SetROP2 (nOldROP2);


	if (m_nCursorFlags & XGC_LEGEND)
		DrawCursorLegend(pDC);
}



void CXGraph::DrawCursorLegend(Graphics &graphics)
{
	int    i;
	double fVal;

	CString cLabel = "", cFmt, cItem;
	m_CursorLabel.m_bVisible = true;
	for (i = 0; i < m_XAxis.size(); i++)
	{
		//CHANGED: bugfix, correct handling of X axis in time mode
		fVal = m_XAxis[i].GetValueForPos (m_CurrentPoint.x);
		if(m_XAxis[i].m_bDateTime)
		{
			cFmt = m_XAxis[i].m_cDisplayFmt;
			cFmt.Replace (_T("\r"),_T(" "));
			cFmt.Replace (_T("\n"),_T(" "));
			cFmt = _T("X[%02d] : ") + COleDateTime(fVal).Format(cFmt) + _T(" %s");
			cItem.Format(cFmt, i+1, m_XAxis[i].m_cLabel);
			cLabel += (_T(" ") + cItem + _T("\r\n"));
		}
		else
		{
			cFmt = _T("X[%02d] : ") + m_XAxis[i].m_cDisplayFmt + _T(" %s");
			cItem.Format(cFmt, i+1, fVal, m_XAxis[i].m_cLabel);
			cLabel += (_T(" ") + cItem + _T("\r\n"));
		}
		//ENDCHANGES
	}

	for (i = 0; i < m_YAxis.size(); i++)
	{
		fVal = m_YAxis[i].GetValueForPos(m_CurrentPoint.y);
		cFmt = _T("Y[%02d] : ") + m_YAxis[i].m_cDisplayFmt + _T(" %s");
		cItem.Format(cFmt, i+1, fVal, m_YAxis[i].m_cLabel);
		cLabel += (_T(" ") + cItem + _T("\r\n"));
	}

	if (m_nSnappedCurve != -1)
	{
		//CHANGED: bugfix, correct handling of X axis in time mode
		CString cXFmt = m_XAxis[m_vSeries[ m_nSnappedCurve ].m_nXAxis].m_cDisplayFmt;
		cXFmt.Replace (_T("\r"),_T(" "));
		cXFmt.Replace (_T("\n"),_T(" "));
		CString cYFmt = m_YAxis[m_vSeries[ m_nSnappedCurve ].m_nYAxis].m_cDisplayFmt;
		if(m_XAxis[ m_vSeries[ m_nSnappedCurve ].m_nXAxis ].m_bDateTime)
		{
			cFmt = _T("%s[%02d] : ") + COleDateTime(m_fSnappedXVal).Format(cXFmt) + _T(", ") + cYFmt;
			cItem.Format(cFmt, m_vSeries[m_nSnappedCurve].m_cLabel, m_nSnappedCurve + 1, m_fSnappedYVal);
			cLabel += (_T(" ") + cItem + _T("\r\n"));
		}
		else
		{
			cFmt = _T("%s[%02d] : ") + cXFmt + _T(", ") + cYFmt;
			cItem.Format(cFmt, m_vSeries[m_nSnappedCurve].m_cLabel, m_nSnappedCurve + 1, m_fSnappedXVal, m_fSnappedYVal);
			cLabel += (_T(" ") + cItem + _T("\r\n"));
		}
		//ENDCHANGES
	}

	m_CursorLabel.m_cText = cLabel;
	m_CursorLabel.Draw(graphics);
}

void CXGraph::DrawCursorLegend (CDCEx* pDC)
{
	int    i;
    double fVal;

    CString cLabel = "", cFmt, cItem;

    m_CursorLabel.m_bVisible = true;
	   
    for (i = 0; i < m_XAxis.size(); i++)
    {
        //CHANGED: bugfix, correct handling of X axis in time mode
        fVal = m_XAxis[i].GetValueForPos (m_CurrentPoint.x);
        if(m_XAxis[i].m_bDateTime)
        {
			cFmt = m_XAxis[i].m_cDisplayFmt;
			cFmt.Replace (_T("\r"),_T(" "));
			cFmt.Replace (_T("\n"),_T(" "));
#ifndef _WIN32_WCE
            cFmt = _T("X[%02d] : ") + COleDateTime(fVal).Format(cFmt) + _T(" %s");
#else
			cFmt = _T("X[%02d] : ") + COleDateTime(fVal).Format() + _T(" %s");
#endif
            cItem.Format(cFmt, i+1, m_XAxis[i].m_cLabel);
            cLabel += (_T(" ") + cItem + _T("\r\n"));
        }
        else
        {
            cFmt = _T("X[%02d] : ") + m_XAxis[i].m_cDisplayFmt + _T(" %s");
            cItem.Format(cFmt, i+1, fVal, m_XAxis[i].m_cLabel);
            cLabel += (_T(" ") + cItem + _T("\r\n"));
        }
        //ENDCHANGES
    }
    
    for (i = 0; i < m_YAxis.size(); i++)
    {
        fVal = m_YAxis[i].GetValueForPos (m_CurrentPoint.y);
        cFmt = _T("Y[%02d] : ") + m_YAxis[i].m_cDisplayFmt + _T(" %s");
        cItem.Format(cFmt, i+1, fVal, m_YAxis[i].m_cLabel);
        cLabel += (_T(" ") + cItem + _T("\r\n"));
    }

    if (m_nSnappedCurve != -1)
    {
        //CHANGED: bugfix, correct handling of X axis in time mode
        CString cXFmt = m_XAxis[m_vSeries[m_nSnappedCurve].m_nXAxis].m_cDisplayFmt;
		cXFmt.Replace (_T("\r"),_T(" "));
		cXFmt.Replace (_T("\n"),_T(" "));
        CString cYFmt = m_YAxis[m_vSeries[m_nSnappedCurve].m_nYAxis].m_cDisplayFmt;
        if(m_XAxis[m_vSeries[m_nSnappedCurve].m_nXAxis].m_bDateTime)
        {			
#ifndef _WIN32_WCE
            cFmt = _T("%s[%02d] : ") + COleDateTime(m_fSnappedXVal).Format(cXFmt) + _T(", ") + cYFmt;
#else
			cFmt = _T("%s[%02d] : ") + COleDateTime(m_fSnappedXVal).Format() + _T(", ") + cYFmt;
#endif
            cItem.Format(cFmt, m_vSeries[m_nSnappedCurve].m_cLabel, m_nSnappedCurve + 1, m_fSnappedYVal);
            cLabel += (_T(" ") + cItem + _T("\r\n"));
        }
        else
        {
            cFmt = _T("%s[%02d] : ") + cXFmt + _T(", ") + cYFmt;
            cItem.Format(cFmt, m_vSeries[m_nSnappedCurve].m_cLabel, m_nSnappedCurve + 1, m_fSnappedXVal, m_fSnappedYVal);
            cLabel += (_T(" ") + cItem + _T("\r\n"));
        }
        //ENDCHANGES
    }

    m_CursorLabel.m_cText = cLabel;

    m_CursorLabel.Draw (pDC);
}

LRESULT CXGraph::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	return CWnd::WindowProc(message, wParam, lParam);
}

bool CXGraph::SelectCurve(int nCurve)
{
	if (nCurve < 0 || nCurve >= m_vSeries.size())
		return false;

	for (int i = 0; i < m_vSeries.size(); i++)
		m_vSeries[i].m_bSelected = (i == nCurve);

	Invalidate();

	return true;
}

bool CXGraph::SelectXAxis(int nAxis)
{
	if (nAxis < 0 || nAxis >= m_XAxis.size())
		return false;

	for (int i = 0; i < m_XAxis.size(); i++)
		m_XAxis[i].m_bSelected = (i == nAxis);

	Invalidate();

	return true;
}

bool CXGraph::SelectYAxis(int nAxis)
{
	if (nAxis < 0 || nAxis >= m_YAxis.size())
		return false;

	for (int i = 0; i < m_YAxis.size(); i++)
		m_YAxis[i].m_bSelected = (i == nAxis);

	Invalidate();
	
	return true;
}

void CXGraph::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_HOME)
	{
		for (size_t i = 0; i < m_XAxis.size(); i++)
			m_XAxis[i].Reset ();
		
		for (size_t i = 0; i < m_YAxis.size(); i++)
			m_YAxis[i].Reset();
		
		Invalidate();
	}

	if (nChar == VK_ADD)
	{
		CameraZoomIn();

	}

	if (nChar == VK_SUBTRACT)
	{
		CameraZoomOut();

	}

	if (nChar == VK_LEFT)
	{
		if (m_opOperation == opCursor)
		{
			//m_CurrentPoint.x --;
			
		}
		else
		{
			for (int i = 0; i < m_XAxis.size(); i++)
			{
				//double fStep = m_XAxis[i].m_fRange / 10.0;
				//VERIFY(m_XAxis[i].SetCurrentRange (m_XAxis[i].m_fCurMin + fStep, m_XAxis[i].m_fCurMax + fStep));

				if(m_nSelectedMarker != -1)
				{
					int nIdx;

					double lfCurrXvalue= m_XAxis[i].m_vAxisMarkers[m_nSelectedMarker].m_lfXValue;
					nIdx= m_XAxis[i].GetCloseIndexByAxis(lfCurrXvalue, 0);

					if(0 > (nIdx-1))	// 범위를 벗어남 [11/12/2013 ChoiJunHyeok]
					{
						return;
					}

					m_XAxis[i].m_vAxisMarkers[m_nSelectedMarker].m_lfXValue= Data()->GetSeriesData()[0]->m_vXData[nIdx-1];
					m_XAxis[i].MoveXAxisMarker(m_nSelectedMarker, m_XAxis[i].m_vAxisMarkers[m_nSelectedMarker].m_lfXValue);	// notation도 이동 [11/12/2013 ChoiJunHyeok]
				}
			}
		}
		

		Invalidate();
	}
	
	if (nChar == VK_RIGHT)
	{

		if (m_opOperation == opCursor)	// 커서 모드 일 때
		{
			//m_CurrentPoint.x++;
			
		}
		else
		{
			for (int i = 0; i < m_XAxis.size(); i++)
			{
				//double fStep = m_XAxis[i].m_fRange / 10.0;
				//VERIFY(m_XAxis[i].SetCurrentRange (m_XAxis[i].m_fCurMin - fStep, m_XAxis[i].m_fCurMax - fStep));

				if(m_nSelectedMarker != -1)
				{
					size_t nIdx;					
					double lfCurrXvalue= m_XAxis[i].m_vAxisMarkers[m_nSelectedMarker].m_lfXValue;
					nIdx= m_XAxis[i].GetCloseIndexByAxis(lfCurrXvalue, 0);

					if(Data()->GetSeriesData()[0]->m_vXData.size() <= nIdx+1)	// 범위를 벗어남 [11/12/2013 ChoiJunHyeok]
					{
						return;
					}

					m_XAxis[i].m_vAxisMarkers[m_nSelectedMarker].m_lfXValue= Data()->GetSeriesData()[0]->m_vXData[nIdx+1];

					m_XAxis[i].MoveXAxisMarker(m_nSelectedMarker, m_XAxis[i].m_vAxisMarkers[m_nSelectedMarker].m_lfXValue);	// notation도 이동 [11/12/2013 ChoiJunHyeok]
				}
				
			}
		}
		

		Invalidate();
	}

	if (nChar == VK_UP)
	{
		if (m_opOperation == opCursor)
			m_CurrentPoint.y--;
		else
		for (int i = 0; i < m_YAxis.size(); i++)
		{
			double fStep = m_YAxis[i].m_fRange / 10.0;
			VERIFY(m_YAxis[i].SetCurrentRange (m_YAxis[i].m_fCurMin - fStep, m_YAxis[i].m_fCurMax - fStep));
		}

		Invalidate();
	}

	if (nChar == VK_DOWN)
	{
		if (m_opOperation == opCursor)
			m_CurrentPoint.y++;
		else
		for (int i = 0; i < m_YAxis.size(); i++)
		{
			double fStep = m_YAxis[i].m_fRange / 10.0;
			VERIFY(m_YAxis[i].SetCurrentRange (m_YAxis[i].m_fCurMin + fStep, m_YAxis[i].m_fCurMax + fStep));
		}

		Invalidate();
	}
	
	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CXGraph::ResetAll()
{
	m_XAxis.clear ();
	m_YAxis.clear ();
	m_vSeries.clear ();
	m_vGraphLines.clear();

	//CXGraphLine::s_nSliceEnd= 0;
	//CXGraphLine::s_nSliceStart= 0;
	CXGraphLine::s_nDisplayNumber= 0;
	
	// 슬라이스 초기화 [10/8/2013 ChoiJunHyeok]
	m_bSliceStartTurn= TRUE;
	m_nSliceNum= 0;

	// Delete all objects
	// Spare 1st object (CursorLabel)
	while (m_Objects.GetCount () > 1)
		delete m_Objects.RemoveTail ();

}

#ifndef _WIN32_WCE
void CXGraph::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_bRButtonDown = true;
	
	CWnd::OnRButtonDown(nFlags, point);
}
#endif

void CXGraph::Serialize( CArchive& archive, UINT nFlags)
{	
	int nHelper, i= 0;
	
	CWnd::Serialize (archive);

    if( archive.IsStoring() )
    {
		archive << nFlags;
		
		if (nFlags & PERSIST_PROPERTIES)
		{
			archive << m_crBackColor;
			archive << m_crGraphColor;
			archive << m_crInnerColor;
			archive << m_bDoubleBuffer;
			archive << m_bShowLegend;
			archive << (int) m_LegendAlignment;
			archive << m_bSnapCursor;
			archive << m_nSnapRange;
			archive << m_nLeftMargin;
			archive << m_nTopMargin;
			archive << m_nRightMargin;
			archive << m_nBottomMargin;
			archive << m_nAxisSpace;
			archive << m_nSelectedSerie;
		}

		if (nFlags & PERSIST_DATA)
		{
			archive << m_vSeries.size ();
			for (i = 0; i < m_vSeries.size(); i++)
				m_vSeries[i].Serialize (archive);

			archive << m_XAxis.size ();
			for (i = 0; i < m_XAxis.size(); i++)
				m_XAxis[i].Serialize (archive);

			archive << m_YAxis.size ();
			for (i = 0; i < m_YAxis.size(); i++)
				m_YAxis[i].Serialize (archive);
		}

		if (nFlags & PERSIST_OBJECTS)
		{

			archive << m_Measures.size();
			for (i = 0; i < m_Measures.size(); i++)
				archive.Write(&m_Measures[i], sizeof(MeasureDef));
		
			// Spare cursor label (1st object), created within constructor
			archive << m_Objects.GetCount () - 1;

			i = 0;

			for (POSITION pos = m_Objects.GetHeadPosition (); pos != NULL; i++ )
			{			
				CXGraphObject *pObject = (CXGraphObject*) m_Objects.GetNext (pos);
				if ( i > 0)
					archive << pObject;
		
			}
		}
	}
	else
    {
		archive >> nFlags;

		if (nFlags & PERSIST_PROPERTIES)
		{
			archive >> m_crBackColor;
			archive >> m_crGraphColor;
			archive >> m_crInnerColor;
			archive >> m_bDoubleBuffer;
			archive >> m_bShowLegend;
			archive >> nHelper;
			m_LegendAlignment = (EAlignment) nHelper;
			archive >> m_bSnapCursor;
			archive >> m_nSnapRange;
			archive >> m_nLeftMargin;
			archive >> m_nTopMargin;
			archive >> m_nRightMargin;
			archive >> m_nBottomMargin;
			archive >> m_nAxisSpace;
			archive >> m_nSelectedSerie;
		}

		if (nFlags & PERSIST_DATA)
		{
			archive >> nHelper;
			for (int i = 0; i < nHelper; i++)
			{
				CXGraphDataSerie serie;
				serie.Serialize (archive);
				serie.m_pGraph = this;
				m_vSeries.push_back (serie);
			}

			archive >> nHelper;
			for (i = 0; i < nHelper; i++)
			{
				CXGraphAxis axis;
				axis.Serialize (archive);
				axis.m_pGraph = this;
				m_XAxis.push_back (axis);
			}

			archive >> nHelper;
			for (i = 0; i < nHelper; i++)
			{
				CXGraphAxis axis;
				axis.Serialize (archive);
				axis.m_pGraph = this;
				m_YAxis.push_back (axis);
			}
		}

		if (nFlags & PERSIST_OBJECTS)
		{
			archive >> nHelper;

			for (int i = 0; i < nHelper; i++)
			{
				MeasureDef measure;
				archive.Read(&measure, sizeof(MeasureDef));
				m_Measures.push_back (measure);
			}
			
			archive >> nHelper;

			for (i = 0; i < nHelper; i++)
			{			
				CXGraphObject *pObject;
				archive >> pObject;
				pObject->m_pGraph = this;
				m_Objects.AddTail (pObject);
			}
		}

		Invalidate();
	}
}

bool CXGraph::Save(const CString cFile, UINT nFlags)
{
	CFile file;

	if (nFlags == 0)
		return false;

	m_pTracker = NULL;
	m_pCurrentObject = NULL;

	if (!file.Open (cFile, CFile::modeCreate | CFile::modeWrite ))
		return false;

#ifndef _WIN32_WCE
	try	
	{
#endif
		CArchive ar(&file, CArchive::store);
		Serialize(ar, nFlags);
#ifndef _WIN32_WCE
	} 
	catch (CException* e)	
	{
		e->Delete ();
		return false;
	}
#endif

	return true;
}

bool CXGraph::Load(const CString cFile)
{
	CFile file;
	
	if (!file.Open (cFile, CFile::modeRead ))
		return false;

	ResetAll();
#ifndef _WIN32_WCE
	try
#endif
	{

		CArchive ar(&file, CArchive::load);
		Serialize(ar, 0);
	} 
#ifndef _WIN32_WCE
	catch (CException* e)
	{
		e->Delete ();
		return false;
	}
#endif
	return true;
}

void CXGraph::ParentCallback()
{
	::PostMessage(GetParent()->m_hWnd, IDM_PARENTCALLBACK, 0, (long) this);		
}

#ifndef _WIN32_WCE
HANDLE CXGraph::DDBToDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* pPal ) 
{
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	HANDLE			hDIB;
	HANDLE			handle;
	HDC 			hDC;
	HPALETTE		hPal;


	ASSERT( bitmap.GetSafeHandle() );

	// The function has no arg for bitfields
	if( dwCompression == BI_BITFIELDS )
		return NULL;

	// If a palette has not been supplied use defaul palette
	hPal = (HPALETTE) pPal->GetSafeHandle();
	if (hPal==NULL)
		hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

	// Get bitmap information
	bitmap.GetObject(sizeof(bm),(LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize		= sizeof(BITMAPINFOHEADER);
	bi.biWidth		= bm.bmWidth;
	bi.biHeight 		= bm.bmHeight;
	bi.biPlanes 		= 1;
	bi.biBitCount		= bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression	= dwCompression;
	bi.biSizeImage		= 0;
	bi.biXPelsPerMeter	= 0;
	bi.biYPelsPerMeter	= 0;
	bi.biClrUsed		= 0;
	bi.biClrImportant	= 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if( nColors > 256 ) 
		nColors = 0;
	dwLen  = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = ::GetDC(NULL);
	hPal = SelectPalette(hDC,hPal,FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED,dwLen);

	if (!hDIB){
		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver 
	// will calculate the biSizeImage field 
	GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
			(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0){
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) 
						* bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else{
		GlobalFree(hDIB);

		// Reselect the original palette
		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits( hDC, (HBITMAP)bitmap.GetSafeHandle(),
				0L,				// Start scan line
				(DWORD)bi.biHeight,		// # of scan lines
				(LPBYTE)lpbi 			// address for bitmap bits
				+ (bi.biSize + nColors * sizeof(RGBQUAD)),
				(LPBITMAPINFO)lpbi,		// address of bitmapinfo
				(DWORD)DIB_RGB_COLORS);		// Use RGB for color table

	if( !bGotBits )
	{
		GlobalFree(hDIB);
		
		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	SelectPalette(hDC,hPal,FALSE);
	::ReleaseDC(NULL,hDC);
	return hDIB;
}


BOOL CXGraph::WriteDIB( LPTSTR szFile, HANDLE hDIB)
{
	BITMAPFILEHEADER	hdr;
	LPBITMAPINFOHEADER	lpbi;

	if (!hDIB)
		return FALSE;

	CFile file;
	if( !file.Open( szFile, CFile::modeWrite|CFile::modeCreate) )
		return FALSE;

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	int nColors = 1 << lpbi->biBitCount;

	// Fill in the fields of the file header 
	hdr.bfType		= ((WORD) ('M' << 8) | 'B');	// is always "BM"
	hdr.bfSize		= GlobalSize (hDIB) + sizeof( hdr );
	hdr.bfReserved1 	= 0;
	hdr.bfReserved2 	= 0;
	hdr.bfOffBits		= (DWORD) (sizeof( hdr ) + lpbi->biSize +
						nColors * sizeof(RGBQUAD));

	// Write the file header 
	file.Write( &hdr, sizeof(hdr) );

	// Write the DIB header and the bits 
	file.Write( lpbi, GlobalSize(hDIB) );

	return TRUE;
}



void CXGraph::JHExportToPicture()
{
	CString strExt = _T("JPG Files(*.jpg)|*.jpg|");
	CFileDialog dlg(FALSE, _T("JPG Files(*.jpg)|"), _T(" "), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strExt,NULL);

	if  (dlg.DoModal() == IDOK)
	{
		CString szFilePath = dlg.GetPathName();


		this->SaveBitmap(szFilePath);
	}
}


BOOL CXGraph::SaveBitmap( const CString cFile )
{
	CWnd*       pWnd = this;
	CBitmap 	bitmap;
	CWindowDC	dc(pWnd);
	CDC 		memDC;
	CRect		rect;

	
	memDC.CreateCompatibleDC(&dc); 

	pWnd->GetWindowRect(rect);

	bitmap.CreateCompatibleBitmap(&dc, rect.Width(),rect.Height() );
	
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
	memDC.BitBlt(0, 0, rect.Width(),rect.Height(), &dc, 0, 0, SRCCOPY); 

	// Create logical palette if device support a palette
	CPalette pal;
	if( dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE )
	{
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
		pLP->palVersion = 0x300;

		pLP->palNumEntries = 
			GetSystemPaletteEntries( dc, 0, 255, pLP->palPalEntry );

		// Create the palette
		pal.CreatePalette( pLP );

		delete[] pLP;
	}

	memDC.SelectObject(pOldBitmap);

	// Convert the bitmap to a DIB
	HANDLE hDIB = DDBToDIB( bitmap, BI_RGB, &pal );

	if( hDIB == NULL )
		return FALSE;

	// Write it to file
	WriteDIB( const_cast <char*>( (LPCTSTR) cFile ), hDIB );

	// Free the memory allocated by DDBToDIB for the DIB
	GlobalFree( hDIB );
	return TRUE;
}

// 스탠다드뷰에 evaluate marker 정보 보낸다.
void CXGraph::SendStandardRightViewToTime( UINT i )
{
	UINT nPage= m_pFrame->GetPageIdx();

	if(nPage == CMainFrame::PAGE_STANDARD)
	{
		CSubStandardView* pStandardView= (CSubStandardView*)m_pFrame->m_pView[CMainFrame::PAGE_STANDARD];
		CStandardRightView* PsRightView= (CStandardRightView*)pStandardView->m_pSDRightView;

		vector<double> vMarkersX;
		for (size_t e=0; e<m_XAxis[i].m_vAxisMarkers.size(); ++e)
		{
			vMarkersX.push_back( m_XAxis[i].m_vAxisMarkers[e].m_lfXValue );
		}

		PsRightView->SetTimeFromGraph(vMarkersX);
	}
}

// zoom in [10/22/2013 ChoiJunHyeok]
void CXGraph::CameraZoomIn()
{
	for (size_t i = 0; i < m_XAxis.size(); i++)
	{
		double fStep = m_XAxis[i].m_fRange / 10.0;
		VERIFY(m_XAxis[i].SetCurrentRange (m_XAxis[i].m_fCurMin + fStep, m_XAxis[i].m_fCurMax - fStep));
	}
	for (size_t i = 0; i < m_YAxis.size(); i++)
	{
		double fStep = m_YAxis[i].m_fRange / 10.0;
		VERIFY(m_YAxis[i].SetCurrentRange (m_YAxis[i].m_fCurMin + fStep, m_YAxis[i].m_fCurMax - fStep));
	}

	Invalidate();
}

// zoom out [10/22/2013 ChoiJunHyeok]
void CXGraph::CameraZoomOut()
{
	for (size_t i = 0; i < m_XAxis.size(); i++)
	{
		double fStep = m_XAxis[i].m_fRange / 10.0;
		VERIFY(m_XAxis[i].SetCurrentRange (m_XAxis[i].m_fCurMin - fStep, m_XAxis[i].m_fCurMax + fStep));
	}
	for (size_t i = 0; i < m_YAxis.size(); i++)
	{
		double fStep = m_YAxis[i].m_fRange / 10.0;
		VERIFY(m_YAxis[i].SetCurrentRange (m_YAxis[i].m_fCurMin - fStep, m_YAxis[i].m_fCurMax + fStep));
	}

	Invalidate();
}

void CXGraph::AddMaximumLine(BOOL bAllRange, double lfXstartValue, double lfXendValue, UINT nGroupNum)
{
	CXGraphDataSerie serie= m_vSeries[0];	// 첫번째 시리즈
	double lfMax= INT_MIN;
	int nMaxIdx= -1;

	CXGraphAxis& Xaxis= m_XAxis[0];
	long nXstart= -1, nXend= -1;

	if(bAllRange)	// 전체 범위면 [10/23/2013 ChoiJunHyeok]
	{
		nXstart= 0;
		nXend= serie.m_nCount;
	}
	else
	{
		Xaxis.GetIndexByXVal(nXstart, lfXstartValue, 0);	// 시작 인덱스
		Xaxis.GetIndexByXVal(nXend, lfXendValue, 0);		// 끝 인덱스 구하고
	}
	
	for (size_t i=nXstart; i<nXend; ++i)
	{
		if( lfMax < serie.m_vPointData[i].fYVal )
		{
			lfMax= serie.m_vPointData[i].fYVal;
			nMaxIdx= i;
		}
	}

	//if (m_nSnappedCurve != -1)
	{
		CXGraphAxis& yaxis = GetYAxis(serie.m_nYAxis);
		CXGraphAxis& xaxis = GetXAxis(serie.m_nXAxis);
		CPoint pt;
		pt.y= yaxis.GetPointForValue(&serie.m_vPointData[nMaxIdx]).y;
		pt.x= xaxis.GetPointForValue(&serie.m_vPointData[nMaxIdx]).x;

		CXGraphDataNotation* pNotation= InsertDataNotation(0, nMaxIdx);

		CXGraphLine line= CXGraphLine();
		line.SetStart(CPoint(pt.x, m_clInnerRect.top));
		line.SetEnd(CPoint(pt.x, m_clInnerRect.bottom));
		line.SetLineType(CXGraphLine::LINE_MAX);
		line.m_nCurrGroup= nGroupNum;	// 소속된 그룹 [10/24/2013 ChoiJunHyeok]
		line.m_pDataNotation= pNotation;

		line.m_pGraph= this;
		line.m_nCurve= 0;

		// data 추가 [10/2/2013 ChoiJunHyeok]
		line.SetX( serie.m_vPointData[nMaxIdx].fXVal );
		line.SetY( serie.m_vPointData[nMaxIdx].fYVal );

		m_vGraphLines.push_back(line);
	}
}

void CXGraph::AddMinimumLine(BOOL bAllRange, double lfXstartValue, double lfXendValue, UINT nGroupNum)
{
	CXGraphDataSerie serie= m_vSeries[0];	// 첫번째 시리즈
	double lfMin= INT_MAX;
	int nMinIdx= -1;

	CXGraphAxis& Xaxis= m_XAxis[0];
	long nXstart= -1, nXend= -1;

	if(bAllRange)	// 전체 범위면 [10/23/2013 ChoiJunHyeok]
	{
		nXstart= 0;
		nXend= serie.m_nCount;
	}
	else
	{
		Xaxis.GetIndexByXVal(nXstart, lfXstartValue, 0);	// 시작 인덱스
		Xaxis.GetIndexByXVal(nXend, lfXendValue, 0);		// 끝 인덱스 구하고
	}

	for (size_t i=nXstart; i<nXend; ++i)
	{
		if( lfMin > serie.m_vPointData[i].fYVal )
		{
			lfMin= serie.m_vPointData[i].fYVal;
			nMinIdx= i;
		}
	}

	//if (m_nSnappedCurve != -1)
	{
		CXGraphAxis& yaxis = GetYAxis(serie.m_nYAxis);
		CXGraphAxis& xaxis = GetXAxis(serie.m_nXAxis);
		CPoint pt;
		pt.y= yaxis.GetPointForValue(&serie.m_vPointData[nMinIdx]).y;
		pt.x= xaxis.GetPointForValue(&serie.m_vPointData[nMinIdx]).x;

		CXGraphDataNotation* pNotation= InsertDataNotation(0, nMinIdx);

		CXGraphLine line= CXGraphLine();
		line.SetStart(CPoint(pt.x, m_clInnerRect.top));
		line.SetEnd(CPoint(pt.x, m_clInnerRect.bottom));
		line.SetLineType(CXGraphLine::LINE_MIN);
		line.m_nCurrGroup= nGroupNum;	// 소속된 그룹 [10/24/2013 ChoiJunHyeok]
		line.m_pDataNotation= pNotation;

		line.m_pGraph= this;
		line.m_nCurve= 0;

		// data 추가 [10/2/2013 ChoiJunHyeok]
		line.SetX( serie.m_vPointData[nMinIdx].fXVal );
		line.SetY( serie.m_vPointData[nMinIdx].fYVal );

		m_vGraphLines.push_back(line);
	}
}

void CXGraph::AddAverageLabel( BOOL bAll, double lfXstartValue, double lfXendValue, UINT nGroupNum )
{
	CXGraphLabel *pLabel = new CXGraphLabel;

	CXGraphAxis& Xaxis= m_XAxis[0];
	long nXstart= -1, nXend= -1;

	Xaxis.GetIndexByXVal(nXstart, lfXstartValue, 0);
	Xaxis.GetIndexByXVal(nXend, lfXendValue, 0);

	int nGroupStartPoint =  Xaxis.GetPointForValue(lfXstartValue);
	//int nGroupEndPoint	 =	Xaxis.GetPointForValue(lfXendValue);

	CRect rect;
	if(bAll)
	{
		rect= CRect(
			m_clInnerRect.left+20,
			m_clInnerRect.top+20,
			m_clInnerRect.left + 140,
			m_clInnerRect.top+40);
	}
	else
	{
		rect= CRect(
			nGroupStartPoint+20,
			m_clInnerRect.top+20,
			nGroupStartPoint + 140,
			m_clInnerRect.top+40);
	}

	CString strAverage;
	if(bAll)
	{
		strAverage.Format("Average : %g", Data()->GetSeriesData()[0]->AverageY());
	}
	else
	{
		strAverage.Format("Average : %g", Data()->GetSeriesData()[0]->AverageRangeY(nXstart, nXend) );
	}

	pLabel->SetGroupNum(nGroupNum);	// 그룹 넘버 [10/24/2013 ChoiJunHyeok]
	pLabel->m_clRect = rect;
	pLabel->m_cText  = strAverage;
	pLabel->m_pGraph = this;

	m_Objects.AddTail (pLabel);
	Invalidate();
}

void CXGraph::RemoveInvolvedObjects( UINT nCurrMovingMarkerGroup )
{
	// 그룹에 소속된 레이블 찾아서 지움 [10/25/2013 ChoiJunHyeok]
	POSITION fordel= NULL;
	POSITION pos= m_Objects.GetHeadPosition();
	while(pos != NULL)
	{
		CXGraphLabel* pLabel= (CXGraphLabel*)m_Objects.GetAt(pos);
		if(pLabel->m_nGroupNum == nCurrMovingMarkerGroup)
		{
			fordel= pos;
			break;
		}
		m_Objects.GetNext(pos);
	}
	if(fordel!=NULL)
		m_Objects.RemoveAt(fordel);

	vector<CXGraphLine>::iterator it;
	for (it=m_vGraphLines.begin(); it<m_vGraphLines.end(); )	// 그래프 라인들 돌면서 [10/25/2013 ChoiJunHyeok]
	{						
		// 움직이는 마커에 해당하는 그룹의 라인이면 [10/24/2013 ChoiJunHyeok]
		if( nCurrMovingMarkerGroup == (*it).m_nCurrGroup )
		{							
			POSITION p= m_Objects.Find((*it).m_pDataNotation);
			if(p!=NULL)
			{
				m_Objects.RemoveAt(p);
			}
			it= m_vGraphLines.erase(it);
		}
		else
		{
			it++;
		}

	}
}

void CXGraph::AddEvaluateMarker()
{
	for (size_t i = 0; i < m_XAxis.size(); i++)
	{
		double fXValue = m_XAxis[i].GetValueForPos (m_CurrentPoint.x);
		double fYValue = m_YAxis[i].GetValueForPos (m_CurrentPoint.y);

		printf("currpoint (x) : %.3lf, (y) : %.3lf\n", fXValue, fYValue);

		m_XAxis[i].SetAxisMarker(m_nSnappedCurve, m_XAxis[i].m_vAxisMarkers.size(), fXValue, fYValue, RGB(80,80,80));
		this->SendStandardRightViewToTime(i);	// standardView일때 evaluate maker들 시간 전송함. [10/16/2013 ChoiJunHyeok]
	}
}

void CXGraph::AddEvaluateMarker(CPoint pTarget)
{
	for (size_t i = 0; i < m_XAxis.size(); i++)
	{
		double fXValue = m_XAxis[i].GetValueForPos (pTarget.x);
		double fYValue = m_YAxis[i].GetValueForPos (pTarget.y);

		printf("currpoint (x) : %.3lf, (y) : %.3lf\n", fXValue, fYValue);

		m_XAxis[i].SetAxisMarker(m_nSnappedCurve, m_XAxis[i].m_vAxisMarkers.size(), fXValue, fYValue, RGB(80,80,80));
		this->SendStandardRightViewToTime(i);	// standardView일때 evaluate maker들 시간 전송함. [10/16/2013 ChoiJunHyeok]
	}
}

void CXGraph::InputCurveData( size_t nCurveNum, BOOL bAutoScale )
{
	CXSeriesData* currSeries= Data()->GetSeriesData()[nCurveNum];	//
	SetCurveData( currSeries->TransXGraphData(), (INT_PTR)currSeries->m_vXData.size(), (INT_PTR)nCurveNum, (INT_PTR)0, (INT_PTR)0, false , bAutoScale) ;
}

#endif

#pragma warning (default : 4244)
#pragma warning (default : 4800)


