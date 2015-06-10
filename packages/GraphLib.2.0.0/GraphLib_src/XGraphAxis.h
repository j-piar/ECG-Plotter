// XGraphAxis.h: Schnittstelle f? die Klasse CXGraphAxis.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XGRAPHAXIS_H__C1683795_71B2_4784_BAB6_CEBEB78C3723__INCLUDED_)
#define AFX_XGRAPHAXIS_H__C1683795_71B2_4784_BAB6_CEBEB78C3723__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XGraphDataSerie.h"
#include "XGraphAxisMaker.h"

typedef struct tagZOOM
{
	double fMin;
	double fMax;

} ZOOM;

typedef struct tagCOLORRANGE
{
	double   fMin;
	double   fMax;
	COLORREF crMinColor;
	COLORREF crMaxColor;
	UINT     nStyle;
#ifndef _WIN32_WCE
	_TCHAR   szLabel[_MAX_PATH];
#else
	char     szLabel[_MAX_PATH];
#endif
} COLORRANGE;

class CFontInfo
{
public:
	CString name;
	REAL size;
	UINT style;
};


//#ifdef _AFXDLL
//class __declspec(dllexport)  CXGraphAxis : public CXGraphObject 
//#else
//class __declspec(dllimport)  CXGraphAxis : public CXGraphObject 
//#endif
class CXGraphAxis : public CXGraphObject
{
	DECLARE_SERIAL( CXGraphAxis )

	friend class CXGraph;
	friend class CXGraphDataSerie;

public:

	enum EAxisPlacement
	{
		apLeft,
		apRight,
		apAutomatic
	};

	enum EAxisType 
	{
		atLog,
		atLinear
	};

	enum ELineType
	{
		LINE_EVAL,
		LINE_SLICE_START,
		LINE_SLICE_END
	};


	enum EAxisKind {
		xAxis,
		yAxis
	};

	enum EDirection {

		left,
		up,
		right,
		down,
		circle,
		circleClosed,
		rect,
		rectClosed
	};

	CXGraphAxis();
	CXGraphAxis(const CXGraphAxis& copy);
	virtual ~CXGraphAxis();

	CXGraphAxis& operator = (const CXGraphAxis& copy);

protected:
	

	vector <COLORRANGE> m_ColorRanges;
	vector <ZOOM>       m_ZoomHistory;
	vector <CXGraphAxisMarker> m_vAxisMarkers;
	


	double m_fMin;
	double m_fMax;
	double m_fCurMin;
	double m_fCurMax;
	double m_fStep;
	double m_fSpareRange;
	double m_fMarkerSpareSize;
	double m_fRange;
	
	static double TimeStepTable[];
	
	int   m_nTop;
	int   m_nLeft;
	int   m_nRange;
	CRect m_clChart;
	CFont m_Font;
	CFont m_TitleFont;
	CFontInfo*	m_pTitleFontInfo;	
	CFontInfo*	m_pFontInfo;

	EAxisKind m_AxisKind;	
	EAxisType m_AxisType;


	
	virtual void Draw(CDCEx* pDC);
	void   DrawLog(CDCEx* pDC);

	double RoundDouble(double doValue, int nPrecision);
	void   AdaptAxis(double* fStart, double* fEnd, double fStep);
	void   AdaptTimeAxis(double * pfStart, double * pfEnd, double fStep);
	void   FitTimeScale(double *fStepWidth, int nBestCount, double fStart, double fEnd);
	void   FitScale (double *fStepWidth, int nBestCount, double fStart, double fEnd);

	void   DrawAxisMarkers(CDCEx *pDC);
	void   DrawColorRanges(CDCEx *pDC);
	void   DrawArc(CDCEx *pDC, CPoint point, EDirection direction, int nSize);
	void   DrawMarker(CDCEx *pDC, CPoint point, int nMarker, int nSize, COLORREF crColor, bool bSymbol = false);
	int	   DrawCurveMarkers(CDCEx *pDC, bool bDraw = true);

	int    GetMaxLabelWidth(CDCEx *pDC);
	int    GetMaxLabelHeight(CDCEx *pDC);

	// GDI+ [9/27/2013 ChoiJunHyeok]
	virtual void Draw(Graphics& graphics);
	void   DrawLog(Graphics& graphics);

		double	GetFirstTickValue();

	// GDI+ [9/27/2013 ChoiJunHyeok]
	void   DrawAxisMarkers(Graphics& graphics);
	void   DrawColorRanges(Graphics& graphics);
	int	   DrawCurveMarkers(Graphics& graphics, bool bDraw = true);
	void   DrawArc(Graphics& graphics, CPoint point, EDirection direction, int nSize);
	void   DrawMarker(Graphics& graphics, CPoint point, int nMarker, int nSize, COLORREF crColor, bool bSymbol = false);

	// GDI+ [9/27/2013 ChoiJunHyeok]
	int    GetMaxLabelWidth(Graphics& graphics);
	int    GetMaxLabelHeight(Graphics& graphics);


	




	void   GetIndexByXVal(long& nIndex, double x, int nCurve);
	void   AutoScale(CDCEx *pDC);
	virtual void   AutoScale(Graphics& graphics);
	void   SetBestStep();

	CSize  GetTitleSize(CDCEx *pDC);


protected:

	EAxisPlacement m_Placement;

	bool     m_bShowMarker;
	bool     m_bDateTime;
	BOOL	 m_bIRIGTime;	//  [10/8/2013 ChoiJunHyeok]
	
	bool     m_bShowGrid;
	bool     m_bAutoScale;
	CString  m_cLabel;
	CString  m_cDisplayFmt;
	
	int      m_nArcSize;
	int      m_nMarkerSize;
	int      m_nTickSize;
	COLORREF m_crGridColor;
	UINT     m_nGridStyle;
	int		m_DecCount;
	
public:
	vector<CXGraphAxisMarker> GetAxisMarkers() const { return m_vAxisMarkers; }

	CFontInfo* GetFontInfo() const { return m_pFontInfo; }
	CFontInfo* GetTitleFontInfo() const { return m_pTitleFontInfo; }
	CXGraphAxis::EAxisKind GetAxisKind() const { return m_AxisKind; }

	//  [10/8/2013 ChoiJunHyeok]
	BOOL GetIRIGTime() const { return m_bIRIGTime; }
	void SetIRIGTime(BOOL _IRIGTime) { m_bIRIGTime = _IRIGTime; }


	double GetMaxValue()	{	return m_fMax;	};
	double GetMinValue()		{	return m_fMin;	};

	inline void SetAxisType(EAxisType type){ m_AxisType = type; };
	inline EAxisType GetAxisType(){ return m_AxisType; };

	inline void SetShowMarker(bool bValue) { m_bShowMarker = bValue; };
	inline void SetDateTime(bool bValue) { m_bDateTime = bValue; };
	inline void SetShowGrid(bool bValue) { m_bShowGrid = bValue; };
	inline void SetAutoScale(bool bValue) { m_bAutoScale = bValue; };
	inline void SetArcSize(int nValue) { m_nArcSize = nValue; };
	inline void SetMarkerSize(int nValue) { m_nMarkerSize = nValue; };
	inline void SetTickSize(int nValue) { m_nTickSize = nValue; };
	inline void SetGridStyle(UINT nValue) { m_nGridStyle = nValue; };
	inline void SetLabel(CString cValue) { m_cLabel = cValue; };
	inline void SetDisplayFmt(CString cValue) { m_cDisplayFmt = cValue; };
	inline void SetGridColor(COLORREF color) { m_crGridColor = color; };
	inline void SetPlacement(EAxisPlacement placement) { m_Placement = placement; };
	 
	inline EAxisPlacement GetPlacement() const { return m_Placement; };
	inline bool GetShowMarker() const { return m_bShowMarker; };
	inline bool GetDateTime() const { return m_bDateTime; };
	inline bool GetShowGrid() const { return m_bShowGrid; };
	inline bool GetAutoScale() const { return m_bAutoScale; };
	inline int  GetArcSize() const { return m_nArcSize; };
	inline int  GetMarkerSize() const { return m_nMarkerSize; };
	inline int  GetTickSize() const { return m_nTickSize; };
	inline UINT GetGridStyle() const { return m_nGridStyle; };
	inline CString GetLabel() const { return m_cLabel; };
	inline CString GetDisplayFmt() const { return m_cDisplayFmt; };
	inline COLORREF GetGridColor() const { return m_crGridColor; };
	inline long GetAxisMarkerCount() { return m_vAxisMarkers.size(); };


	size_t GetCloseIndexByAxis(double, UINT);	// 가장 가까운 인덱스를 가져옴 [10/15/2013 ChoiJunHyeok]
	CPoint GetPointForValue(TDataPoint* point);
	virtual int		GetPointForValue(double value);
	double GetValueForPos(int nPos);

	BOOL	SearchPairValue(UINT nFindNumber, double& lfvalue); // 현재 라인의 짝의 값을 구한다. [10/23/2013 ChoiJunHyeok]
	BOOL	SearchPairValue(UINT nFindNumber, BOOL bCurrLineIsStartLine, double& lfvalue);	//  [10/23/2013 ChoiJunHyeok]
	BOOL	SearchGroup(UINT nSliceNumber, double& lfstart, double& lfend);	// 조건에 해당하는 라인을 가져온다. [10/23/2013 ChoiJunHyeok]

	void   MoveXAxisMarker(int nMarker, double lfXValue);	// 마커를 옮긴다 [10/16/2013 ChoiJunHyeok]
	void   SetAxisMarker(int nSnappedCurve, int nMarker, double lfXValue, double lfYValue, COLORREF crColor, UINT nLineType= LINE_EVAL, UINT nStyle = PS_SOLID);
	void   DeleteAxisMarker(int nMarker);
	CXGraphAxisMarker& GetAxisMarker(int nMarker);
	

	void   SetColorRange(int nRange, double fMin, double fMax, COLORREF crMinColor, COLORREF crMaxColor, CString cLabel, UINT nStyle = HS_SOLID);
	void   DeleteColorRange(int nRange);
	void   DeleteAllColorRanges();
		
	void   Reset();
	virtual bool   SetRange(double fMin, double fMax,BOOL IsNotAssign = TRUE);
	bool   SetCurrentRange(double fMin, double fMax, double fStep = 0.0);
	void	SetDecimals(int NewValue){ m_DecCount = NewValue;}

	void   SetFont (CString cFontName, int nFontHeight, int nFontStyle);
	void   SetFont (LPLOGFONT pLogFont);
	void   SetTitleFont (LPLOGFONT pLogFont);
	void   SetTitleFont (CString cFontName, int nFontHeight, int nFontStyle);
	void   GetRange(double&fMin, double& fMax, double& fStep);
	void   Scale(double fFactor);

	virtual void Serialize( CArchive& archive );
};

#endif // !defined(AFX_XGRAPHAXIS_H__C1683795_71B2_4784_BAB6_CEBEB78C3723__INCLUDED_)
