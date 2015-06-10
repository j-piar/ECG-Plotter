///////////////////////////////////////////////////////////////////////////////////////
// XGraph.h
//
// Main header file for the CXGraph charting control
//
// You are free to use, modify and distribute this source, as long as
// there is no charge, and this HEADER stays intact. This source is
// supplied "AS-IS", without WARRANTY OF ANY KIND, and the user
// holds me blameless for any or all problems that may arise
// from the use of this code.
//
// Expect bugs.
//
// Gunnar Bolle (webmaster@beatmonsters.com)
// 
// 
// History
// 
// 21 Mar 2002    1.00    First release
// 26 Mar 2002    1.01    Fixed DateTime-bug in cursor mode
//						  (thanks to Pavel Klocek for providing the fix)
//						  Fixed crash when adding trend or moving average
// 11 Apr 2002    1.02    Panning in cursor mode added
//						  Added support for very large and very small numbers
//						  Values greater/less 10E6/10E-6 are now automatically shown in 
//                        exponential format
//                        Completely redesigned autoscaling (only for normal axes, 
//                        DateTime types follow) 
//                        Added data notation marks
//                        Added Get/Set member functions
//                        Added nth-order-polynomial trend
// 25. Sep 2002   1.03    Now compiles under VC7
//                        Load/Save for properties/data/object persistence
//						  Added zoom-history
// unreleased	  1.04    Added UNICODE support
//                        Added background bitmaps
//                        Added bitmap objects
//                        Added gradient colorranges
//						  Added axismarkers
// 02.12.2004     1.05    Added support for WINDOWS CE (needs improvement)
//						  Logarithmic axes (thanks to ...)
//						  Added polygonfilling between 2 curves
//   					  Several Bugs fixed by Joerg Blattner
//						  Added Cursor-Synchronisation
//						  Curve editing mode
//						  Measuring mode
// 
//						  					
// Known issues
// 
// - clipping in print preview doesn't work
//
// Additional credits
//
// - Ishay Sivan (Bugfixing) 
// - Pavel Klocek (Bugfixing)
// - James White (color button)
// - Chris Maunder (color popup)
// - Keith Rule (MemDC)
// - J.G. Hattingh (parts of Device Context Utilities)
// - J?g Blattner (Bugfixing, improvements)
//
///////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_XGRAPH_H__6F244A3B_22E2_4F7F_802F_8FBA303058C1__INCLUDED_)
#define AFX_XGRAPH_H__6F244A3B_22E2_4F7F_802F_8FBA303058C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif

#include "resource.h"

#include <vector>
#include <list>
#include <functional>

using namespace std;

class CMainFrame;
class CXGraph;
class CFlightDataManager;
//class CFlightDataManager;

#include "XGraphObjectBase.h"
#include "XGraphDataSerie.h"
#include "XGraphAxis.h"
#include "XGraphLabel.h"
#include "XGraphBitmap.h"
#include "XGraphDataNotation.h"
#include "XGraphLine.h"

#define XG_YAXISCLICK	   WM_USER + 1
#define XG_XAXISCLICK	   WM_USER + 2
#define XG_CURVECLICK	   WM_USER + 3
#define XG_YAXISDBLCLICK   WM_USER + 4
#define XG_XAXISDBLCLICK   WM_USER + 5
#define XG_CURVEDBLCLICK   WM_USER + 6
#define XG_GRAPHDBLCLICK   WM_USER + 7
#define XG_RBUTTONUP       WM_USER + 8
#define XG_CURSORMOVED     WM_USER + 9
#define XG_ZOOMCHANGE      WM_USER + 10		// zoom range가 같이 연동 되도록 [11/12/2013 ChoiJunHyeok]
#define XG_POINTCHANGED    WM_USER + 11
#define XG_ADDSLICE	       WM_USER + 12		// 슬라이스 추가될 때 다같이 연동 [11/12/2013 ChoiJunHyeok]
#define XG_MOVEMARKER	   WM_USER + 13		// 슬라이스 라인 옮길 때 다같이 옮기도록 메세지 처리 [11/12/2013 ChoiJunHyeok]
//#define XG_ALLCLEAR		   WM_USER + 14		// all clear [11/13/2013 ChoiJunHyeok]
//#define XG_ZOOMRESET	   WM_USER + 15		// zoom reset [11/13/2013 ChoiJunHyeok]


#define BITMAP_CHART	   0x01
#define BITMAP_INNER       0x02
#define BITMAP_NOSIZE      0x04
#define BITMAP_STRETCH     0x08
#define BITMAP_TILE        0x10

#define PERSIST_PROPERTIES 0x01  
#define PERSIST_DATA       0x02
#define PERSIST_OBJECTS    0x04

#define XGC_HORIZ		   0x01
#define XGC_VERT		   0x02
#define XGC_LEGEND		   0x04
#define XGC_ADJUSTSMOOTH   0x08

#define MIN_ZOOM_PIXELS    15


// Custom clipboard formats

#define CF_XBITMAP		   _T("XGRAPHBITMAP")
#define CF_XLABEL		   _T("XGRAPHLABEL")

const COLORREF BASECOLORTABLE[12] = 
{
	RGB(128,0,0),
	RGB(255,0,0),
	RGB(128,128,0),
	RGB(255,255,0),
	RGB(0,128,0),
	RGB(0,255,0),
	RGB(0,128,128),
	RGB(0,255,255),
	RGB(0,0,128),
	RGB(0,0,255),
	RGB(128,0,128),
	RGB(255,0,255)
};

#define MAX_MARKERS 8

// 마커 정보 [11/12/2013 ChoiJunHyeok]
class CMarkerInfo
{
public:
	CMarkerInfo::CMarkerInfo()
	{
		lfOldxValue= 0;
		lfCurrXValue= 0;
		nMarkerType= -1;
	}
	CMarkerInfo::~CMarkerInfo()
	{

	}

public:
	double lfOldxValue;
	double lfCurrXValue;
	int nMarkerType;
};

//#ifdef _AFXDLL
//class __declspec(dllexport)  CXGraph : public CWnd
//#else
//class __declspec(dllimport)  CXGraph : public CWnd
//#endif
class CXGraphData;
class CXGraph : public CWnd
{
public:
	static UINT s_ID;

protected:
	UINT	m_nSelMode;	// 마우스 모드 [9/12/2013 ChoiJunHyeok]

	//DECLARE_SERIAL( CXGraph )

private:
	friend class CXGraphAxis;
	friend class CXGraphLogAxis;
	friend class CXGraphDataSerie;
	friend class CXGraphLabel;

public:

	enum { VERSION = 105 };

	CXGraph();

public:

	enum EOperation {

		opNone,
		opZoom,
		opReset,
		opPan,
		opCursor,
		opMoveObject,
		opEditObject,
		opEditCurve,
		opSlice,
		opRange,
		opPeak
	};

	enum EAlignment
	{
		left,
		right,
		top,
		bottom
	};

	enum ELineType
	{
		LINE_EVAL,
		LINE_SLICE_START,
		LINE_SLICE_END
	};


	

	typedef struct tagNM_POINTMOVING
	{
		long    nCurve;
		long	nIndex;
		double  dOldVal;
		double  dNewVal;
	} NM_POINTMOVING;

	typedef struct tagMeasureDef
	{
		long nCurve1;
		long nIndex1;
		long nCurve2;
		long nIndex2;
	} MeasureDef;






public:

	//{{AFX_VIRTUAL(CXGraph)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
	virtual ~CXGraph();

	CString GetPlotNameX() const { return m_strPlotX; }
	void SetPlotX(CString _PlotX) { m_strPlotX = _PlotX; }
	CString GetPlotNameY() const { return m_strPlotY; }
	void SetPlotY(CString _PlotY) { m_strPlotY = _PlotY; }

	// graph data get set [10/2/2013 ChoiJunHyeok]
	CXGraphData* Data() const { return m_pGraphData; }
	void SetGraphData(CXGraphData* _GraphData) { m_pGraphData = _GraphData; }

	void MinimumMessage();	// minimum 추가 [10/1/2013 ChoiJunHyeok]
	void AddMinimumLine(BOOL bAllRange, double lfXstartValue, double lfXendValue, UINT nGroupNum);

	void MaximumMessage();	// maximum 추가 [10/1/2013 ChoiJunHyeok]
	void AddMaximumLine(BOOL bAllRange, double lfXstartValue, double lfXendValue, UINT nGroupNum);

	
	void AddSliceStart();
	void AddSliceEnd();
	void AddSliceStart(CPoint);
	void AddSliceEnd(CPoint);

	void AddEvaluateMarker();	// marker 추가 부분[11/24/2013 ChoiJunHyeok]
	void AddEvaluateMarker(CPoint pTarget);

	void GetYAxisRange(double& lfMin, double& lfMax);	// Y axis 가져옴 [10/2/2013 ChoiJunHyeok]
	void YAxisReset(double lfMin, double lfMax);	// 데이터 range를 변경한다. [8/29/2013 ChoiJunHyeok]
	void DataMultiplication(double _lfFactor);	// 데이터 곱한다. [8/29/2013 hoiJunHyeok]
	void DataOffset(double _lfOffsetValue);	// 데이터를 offset만큼 더한다. [8/29/2013 ChoiJunHyeok]

	vector<CXGraphLine> GetGraphLines() const { return m_vGraphLines; }	// 라인벡터 가져옴 [10/2/2013 ChoiJunHyeok]

	void ExportCSV(CString strPath, CFlightDataManager* pData);	// export csv [10/25/2013 ChoiJunHyeok]

	// 데이터 세팅시 이용
	CXGraphDataSerie& SetCurveData(TDataPoint* pValues, long nCount, int nCurve = 0, int nXAxis = 0, int nYAxis = 0, bool bAutoDelete = false, bool bAutoScale= true);
	
	CXGraphAxis& GetXAxis(int nAxis);
	CXGraphAxis& GetYAxis(int nAxis);
	CXGraphDataSerie& GetCurve(int nCurve);

	bool DeleteCurve(int nCurve);
	bool DeleteXAxis(int nAxis);
	bool DeleteYAxis(int nAxis);
	
	void ShowChartProperty();	//  [10/14/2013 ChoiJunHyeok]

	CXGraphDataNotation* InsertDataNotation(int nCurve, int nIndex);
	void AddLinearTrend(CXGraphDataSerie& serie);
	void AddCubicTrend(CXGraphDataSerie& serie);

	int  GetXAxisCount() { return m_XAxis.size(); };
	int  GetYAxisCount() { return m_YAxis.size(); };
	int  GetCurveCount() { return m_vSeries.size(); };

	bool SelectCurve(int nCurve);
	bool SelectXAxis(int nAxis);
	bool SelectYAxis(int nAxis);

	void SetGraphMargins(int nLeft, int nTop, int nRight, int nBottom);

	BOOL SaveBitmap(const CString cFile);
	void JHExportToPicture();	//  [10/10/2013 ChoiJunHyeok]

	// Graph operations
	void Range();
	void Edit();
	void Zoom();
	void ResetZoom();
	void ResetZoom2();

	void Pan();
	void NoOp();
	void Cursor();
	void SliceMode();
	void Peak();

	void DoZoom(BOOL bSyncMessage, BOOL bYautoScale);
	void DoPan(CPoint point);

	void MoveXMarker(double lfCurrValue, double lfMoveValue, UINT nMarkerType);	// curr->move + 타입확인 마커를 옮긴다. [11/12/2013 ChoiJunHyeok]


	void PeakRemove();	// 범위 해당 되는 부분을 지움 [10/xx/2013 ChoiJunHyeok]
	void RePlot(BOOL bFristPlot=TRUE);	// replot 차트 안으로 뺌 [10/10/2013 ChoiJunHyeok]

	void InputCurveData( size_t nCurveNum, BOOL bAutoScale );	// Data에서 Graph쪽으로 옮김. [11/25/2013 ChoiJunHyeok]

	void PlotDataSet(UINT nSeriesNumber, CString strX, CString strY, vector<CString> vSerieNames);
	// 몇번째리시즈인지, X이름, Y이름, 시리즈별이름벡터 [10/23/2013 ChoiJunHyeok]
	void PlotRectSet(UINT nRectPos, UINT nRow, UINT nCol);
	
	// selection Mode Setting [9/12/2013 ChoiJunHyeok]
	UINT GetMouseMode() const { return m_nSelMode; }
	void SetMouseMode(UINT val)
	{
		if(val == m_nSelMode)	// 기존 모드와 같으면 모드 안바꿈 [10/8/2013 ChoiJunHyeok]
			return;

		if((m_nSelMode==GRAPH_MODE_RANGE) && (val!=GRAPH_MODE_RANGE))	// range에서 다른 모드로 넘어오면 라인들 초기화 [10/8/2013 ChoiJunHyeok]
		{
			m_vGraphLines.clear();
			m_Objects.RemoveAll();
		}

		m_nSelMode = val;

		if(m_nSelMode==GRAPH_MODE_EVAL)
			this->Cursor();
		else if(m_nSelMode==GRAPH_MODE_RANGE)
			this->Range();
		else if(m_nSelMode==GRAPH_MODE_SLICE)
			this->SliceMode();
		else if(m_nSelMode==GRAPH_MODE_PAN)
			this->Pan();
		else if(m_nSelMode==GRAPH_MODE_ZOOM)
			this->Zoom();
		else if(m_nSelMode==GRAPH_MODE_EDIT)
			this->Edit();
		else if(m_nSelMode==GRAPH_MODE_SELECT)
			this->NoOp();
		else if(m_nSelMode==GRAPH_MODE_PEAK)
			this->Peak();
	}
	enum {  GRAPH_MODE_NONE, GRAPH_MODE_ZOOM, GRAPH_MODE_EDIT, GRAPH_MODE_SELECT, GRAPH_MODE_SLICE, GRAPH_MODE_PAN, GRAPH_MODE_EVAL, GRAPH_MODE_RANGE, GRAPH_MODE_PEAK };

	void OnDraw(CDC *pDC);

#ifndef _WIN32_WCE
	void PrintGraph(CDC *pDC);
	void PrintGraph(CDC *pDC, CRect printRect);
#endif

	int  GetZoomDepth();
	
	void ResetAll();

	bool SetBackgroundBitmap(CBitmap* pBitmap, UINT nFlags = BITMAP_CHART | BITMAP_STRETCH);

	CXGraphLabel&  InsertLabel(CString cText = _T(""));
	CXGraphLabel&  InsertLabel(CRect rect, CString cText = _T(""));
	CXGraphBitmap& InsertBitmap(CRect rect, CBitmapEx* pBitmap);
	void AverageMessage();	// average [10/8/2013 ChoiJunHyeok]

	void AddAverageLabel( BOOL bAll, double lfXstartValue, double lfXendValue, UINT nGroupNum );

	inline void SetBackColor(COLORREF color)		 { m_crBackColor = color; };
	inline void SetGraphColor(COLORREF color)		 { m_crGraphColor = color; };
	inline void SetInnerColor(COLORREF color)		 { m_crInnerColor = color; };
	inline void SetDoubleBuffer(bool bValue)		 { m_bDoubleBuffer = bValue; };
	inline void SetShowLegend(bool bValue)			 { m_bShowLegend = bValue; };
	inline void SetInteraction(bool bValue)		     { m_bInteraction = bValue; };
	inline void SetLegendAlignment(EAlignment align) { m_LegendAlignment = align; };
	inline void SetSnapCursor(bool bValue)			 { m_bSnapCursor = bValue; };
	inline void SetSnapRange(int nValue)			 { m_nSnapRange = nValue; };
	inline void SetPrintHeader(CString cHeader)      { m_cPrintHeader = cHeader; };
	inline void SetPrintFooter(CString cFooter)      { m_cPrintFooter = cFooter; };
	inline void SetCursor(CPoint point)              { m_CurrentPoint = point; };
	inline void SetCursorFlags(UINT nFlags)			 { m_nCursorFlags = nFlags; };
	
	void		Autoscale(int nXAxis, int nYAxis, int nCurve);

	inline void DeleteMeasures()                     { m_Measures.clear(); };

	inline void ForceSnap (UINT nCurve)              { m_nForcedSnapCurve = nCurve; };


	inline COLORREF   GetBackColor()		{ return m_crBackColor; };
	inline COLORREF   GetGraphColor()		{ return m_crGraphColor; };
	inline COLORREF   GetInnerColor()		{ return m_crInnerColor; };
	inline bool		  GetDoubleBuffer()		{ return m_bDoubleBuffer; };
	inline bool		  GetShowLegend()		{ return m_bShowLegend; };
	inline bool       GetInteraction()      { return m_bInteraction; };
	inline EAlignment GetLegendAlignment()	{ return m_LegendAlignment; };
	inline bool		  GetSnapCursor()		{ return m_bSnapCursor; };
	inline int		  GetSnapRange()		{ return m_nSnapRange; };
	inline CPoint     GetCursor()           { return m_CurrentPoint; };
	inline EOperation GetOperation()        { return m_opOperation; };
	
	TDataPoint GetCursorAbsolute(int nCurve);
	void	 SetCursorAbsolute(int nCurve, TDataPoint vPoint, bool bForceVisible = false);


	bool   Save(const CString cFile, UINT nFlags = PERSIST_PROPERTIES | PERSIST_DATA | PERSIST_OBJECTS);
	bool   Load(const CString cFile);

	lpDrawFunc		m_pDrawFn[MAX_MARKERS];
	lpDrawGDIFunc	m_pDrawGDIFn[MAX_MARKERS];

protected:

#ifndef _WIN32_WCE
	BOOL   WriteDIB( LPTSTR szFile, HANDLE hDIB);
	HANDLE DDBToDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* pPal );
#endif

	void   Serialize( CArchive& archive, UINT nFlags);

	//{{AFX_MSG(CXGraph)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
#if _MFC_VER >= 0x0700
	afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
#else
	afx_msg void OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
#endif
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);




	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
#ifndef _WIN32_WCE
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
#endif
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	void CameraZoomOut();
	void CameraZoomIn();
	void ApplyZoom(ZOOM zoom);

#ifndef _WIN32_WCE
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
#endif
	afx_msg void ParentCallback();
	afx_msg void RestoreLastZoom();

#ifndef _WIN32_WCE
	afx_msg void OnCopy();
	afx_msg void OnPaste();
#endif

	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	CRect GetInnerRect() const { return m_clInnerRect; }
	void SetInnerRect(CRect _InnerRect) { m_clInnerRect = _InnerRect; }

	CPoint GetCurrentPoint() const { return m_CurrentPoint; }
	void SetCurrentPoint(CPoint _CurrentPoint) { m_CurrentPoint = _CurrentPoint; }	// 포인트 get set [11/12/2013 ChoiJunHyeok]

	CRect GetCurrentZoom() const { return m_clCurrentZoom; }
	void SetCurrentZoom(CRect _CurrentZoom) { m_clCurrentZoom = _CurrentZoom; }	// zoom rect get set [11/13/2013 ChoiJunHyeok]

public:
	BOOL m_bSliceStartTurn;		// 슬라이스 순서대로 [10/7/2013 ChoiJunHyeok]
	UINT m_nSliceNum;			// 슬라이스 짝개수 [10/8/2013 ChoiJunHyeok]
	
	int m_nSelectedMarker;		// 선택된 마커 번호 [10/21/2013 ChoiJunHyeok]
	UINT m_nPlottingPosRow;			// 현재 위치 [10/23/2013 ChoiJunHyeok]
	UINT m_nPlottingPosCol;			// 현재 위치 [10/23/2013 ChoiJunHyeok]
	BOOL m_bPlottingBottom;			// 플로팅 위치가 최하단일 때만 x 레이블 띄워줌 [10/23/2013 ChoiJunHyeok]

	vector<UINT64> m_vnPeakRemoveKey;	// 지운 포인트 리스트 [10/25/2013 ChoiJunHyeok]

	CMainFrame* m_pFrame;		// main window [10/24/2013 ChoiJunHyeok]

protected:

	typedef struct tagSelectByMarker {

		CRect markerRect;
		CXGraphObject* pObject;

	} SelectByMarker;

	static NM_POINTMOVING		m_nmCurrentPointMoving;
	COLORREF					m_crBackColor;
	COLORREF					m_crGraphColor;
	COLORREF					m_crInnerColor;
	bool						m_bDoubleBuffer;
	bool						m_bShowLegend;
	bool						m_bInteraction;
	bool						m_bTmpDC;
	EAlignment					m_LegendAlignment;
	bool						m_bSnapCursor;
	int							m_nSnapRange;
	UINT						m_nCursorFlags;

	CDCEx*                      m_pPrintDC;
	CDCEx*                      m_pDrawDC;
	CRectTracker*               m_pTracker;
	CRect                       m_clInnerRect;

	CRect                       m_clCurrentMeasure;
	CRect                       m_OldCursorRect;
	CRect                       m_clPrintRect;
	CPoint                      m_OldPoint;
	EOperation                  m_opOperation;
	int                         m_nSnappedCurve;
	int                         m_nSnappedCurve1;
	int                         m_nLeftMargin,
								m_nTopMargin,
								m_nRightMargin,
								m_nBottomMargin;
	int                         m_nAxisSpace;
	int                         m_nBitmapFlags;
	int							m_nForcedSnapCurve;
	CRect                       m_clCurrentZoom;

	CBitmap*                    m_pBitmap;
	CPoint						m_MouseDownPoint,
								m_oldCursorPoint,
								m_MouseUpPoint,
								m_CurrentPoint;

	bool 						m_bLButtonDown;
	bool 						m_bRButtonDown;
	bool						m_bObjectSelected;
	double						m_fCurrentEditValue;
	bool						m_bDataPointMoving;
	int				            m_nSelectedSerie;
	double                      m_fSnappedXVal,
		                        m_fSnappedYVal;
	CString                     m_cPrintHeader;
	CString                     m_cPrintFooter;
	CXGraphLabel                m_CursorLabel;
	MeasureDef                  m_MeasureDef;

	vector <MeasureDef>         m_Measures;
	vector <SelectByMarker>     m_SelectByMarker;
	
	vector <CXGraphDataSerie>	m_vSeries;
	vector <CXGraphAxis>		m_XAxis;
	vector <CXGraphAxis>		m_YAxis;
	CObList				        m_Objects;
	CXGraphObject*              m_pCurrentObject;
	UINT                        m_nCFBitmap;

	vector<CXGraphLine>			m_vGraphLines;	// 라인들 [9/30/2013 ChoiJunHyeok]		
	CXGraphData*				m_pGraphData;	// 데이터들 [10/2/2013 ChoiJunHyeok]

	UINT						m_nSerieCount;	// 시리즈 갯수 [10/14/2013 ChoiJunHyeok]
	//vector<CFlightDataManager*> m_vpDataMgr;
	vector<CString>				m_vSerieNames;	// 이름들 저장 [10/11/2013 ChoiJunHyeok]
	
	CString m_strPlotX;
	CString m_strPlotY;




	void AdjustPointToData(CPoint& point);
	void InsertEmptyLabel();
	bool CheckObjectSelection(bool bEditAction = false, bool bCheckFocusOnly = false);

	void RemoveInvolvedObjects( UINT nCurrMovingMarkerGroup );

	void SendStandardRightViewToTime( UINT i );

	void GetMinMaxForData (CXGraphDataSerie& serie, double& fXMin, double& fXMax, double& fYMin, double& fYMax);
	int  GetAxisIndex(CXGraphAxis* pAxis);
	int  GetCurveIndex(CXGraphDataSerie* pSerie);

	void DrawLegend(Graphics& graphics, CRect& ChartRect);
	void DrawLegend(CDCEx *pDC, CRect& ChartRect);

	void DrawCursorLegend (CDCEx* pDC);
	void DrawBackgroundBitmap(CDCEx* pDC, CRect clChartRect);
	void DrawZoom(CDCEx* pDC);
	void DrawCursor(CDCEx* pDC);

	// GDI 추가 [9/27/2013 ChoiJunHyeok]
	void DrawCursor(Graphics &graphics);
	void DrawZoom(Graphics &graphics);
	void DrawCursorLegend(Graphics &graphics);
	void DrawRange(Graphics& graphics);
	


	void LinearTrend();
	void CubicTrend();
	void OnProperties();
#ifndef _WIN32_WCE
	void OnPrint();
#endif
};

#if _MSC_VER < 1300

	// CArchive operators for C++ bool, guess the guys at microsoft have forgotten this
	// after introducing the "bool" type
	inline CArchive& operator <<(CArchive& ar, bool b) 
	{
		ar << (b ? TRUE : FALSE);
		return ar;
	};

	inline CArchive& operator >>(CArchive& ar, bool& b) 
	{
		BOOL old_b;
		ar >> old_b;
		(old_b ? b = true : b = false);
		return ar;
	};

#endif


//{{AFX_INSERT_LOCATION}}


#endif // AFX_XGRAPH_H__6F244A3B_22E2_4F7F_802F_8FBA303058C1__INCLUDED_



