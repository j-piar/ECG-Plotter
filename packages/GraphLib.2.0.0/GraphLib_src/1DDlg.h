#pragma once

#include "xgraph.h"
#include "xgraphaxis.h"
#include "ETSLayOut.h"
#include "JDlgToolBar.h"
#include "resource1.h"
// C1DDlg 대화 상자입니다.

//#include "../../DefineModule.h"

#ifdef VIEWER
	#include "../../Viewer/resource.h"
#endif

#ifdef EPAPER_VIEWER
	#include "../../EPaperViewer/resource.h"
#endif 

#ifdef ELCD1D2D_VIEWER
	#include "../../ELCD1D2DViewer/resource.h"
#endif 

#ifdef OLED_VIEWER
	#include "../../OLEDViewer/resource.h"
#endif 

#ifdef LCDMAX_VIEWER
	#include "../../LCDMaxViewer/resource.h"
#endif 

//kad07 20110324

#define WMUSER_DESTROY1D WM_USER+05

class cSetWindowPosition;			//kad07 20110324
class C2DView;
class C1DDlg : public ETSLayoutDialog
{
	DECLARE_DYNAMIC(C1DDlg)
	DECLARE_MESSAGE_MAP()
	DECLARE_LAYOUT();

public:
	enum { IDD = IDD_1DDLG };

	
	virtual BOOL OnInitDialog();
	C1DDlg(CWnd* pParent = NULL);  
	virtual ~C1DDlg();

public:
	CXGraph		m_Graph;
	CString		m_xName;
	CString 	m_yName;
	CString 	m_xFormat;
	CString 	m_yFormat;
	BOOL		m_IsLog;

	JDlgToolBar	m_ToolBar;
	TDataPoint	m_data[3];

	CArray<TDataPoint*,TDataPoint*>	m_Values;
	CArray<INT_PTR,INT_PTR>			m_ValueCount;
	int								m_ValueBigCount;	//20120117 kad07
	CStringArray					m_Labels;
	CString							m_Title;
	double							m_MaxX,m_MinX;
	double							m_MaxY,m_MinY;

public:
	void		SetRangeX(double min,double max){m_MinX = min;m_MaxX = max;};
	void		SetRangeY(double min,double max){m_MinY = min;m_MaxY = max;};

	
	INT_PTR			GetCount(){return m_Values.GetCount();};
	TDataPoint*		GetDataPoint(INT_PTR idx){return m_Values.GetAt(idx);};
	INT_PTR			GetValueCount(INT_PTR idx){return m_ValueCount.GetAt(idx);};
	CString			GetLabel(INT_PTR idx){return m_Labels.GetAt(idx);};
	void			AddDataCurve(CString szLabel, TDataPoint* pData,int nDataCount);

	void ExportData(CString fname);
	//void Export2Excel(CString fname);

protected:
	//HICON		m_hIcon;

	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    
	void	  InitLayOut();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	
	afx_msg void OnClose();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShow(){m_Graph.OnShow();};
	afx_msg void OnHide(){m_Graph.OnHide();};
	afx_msg void Edit(){m_Graph.Edit();};
	afx_msg void OnPrint(){m_Graph.OnPrint();};

	//afx_msg void OnExport();
	afx_msg void SetCurves();
	afx_msg void Zoom(){m_Graph.Zoom();};
	afx_msg void ZoomOut(){m_Graph.ZoomOut();};
	afx_msg void RestoreLastZoom(){m_Graph.RestoreLastZoom();};

	afx_msg void Pan(){m_Graph.Pan();};
	afx_msg void Cursor(){m_Graph.Cursor();};
	afx_msg void Measure(){m_Graph.Measure();};
	afx_msg void NoOp(){m_Graph.NoOp();};
	afx_msg void ResetZoom(){m_Graph.ResetZoom();};
	afx_msg void OnProperties(){m_Graph.OnProperties();};
	afx_msg void InsertEmptyLabel(){m_Graph.InsertEmptyLabel();};
	afx_msg void LinearTrend(){m_Graph.LinearTrend();};
	afx_msg void CubicTrend(){m_Graph.CubicTrend();};
	afx_msg void ParentCallback(){m_Graph.ParentCallback();};
	afx_msg LRESULT OnKickIdle(WPARAM, LPARAM lCount);
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnUpdateZoom(CCmdUI *pCmdUI){pCmdUI->SetCheck(m_Graph.GetOperation() == CXGraph::opZoom);};
	afx_msg void OnUpdatePan(CCmdUI *pCmdUI){pCmdUI->SetCheck(m_Graph.GetOperation() == CXGraph::opPan);};
	afx_msg void OnUpdateCursor(CCmdUI *pCmdUI){pCmdUI->SetCheck(m_Graph.GetOperation() == CXGraph::opCursor);};
	afx_msg void OnUpdateMeasure(CCmdUI *pCmdUI){pCmdUI->SetCheck(m_Graph.GetOperation() == CXGraph::opMeasure);};
	afx_msg void OnUpdateNoOp(CCmdUI *pCmdUI){pCmdUI->SetCheck(m_Graph.GetOperation() == CXGraph::opNone);};

private:
	void	ClearMemory();
//kad07
////////////////////////////////////////////////////////
public:
	int m_nDlgCount;
	BOOL m_WinType;
////////////////////////////////////////////////////////

//20120126 kad07 1D Graph Lable
//----------------------------------------------------------------------------------
private:
	vector<float>	m_vctSplitData;
	float			m_StartX;
	float			m_StartY;
	int				m_FileFormat;

public:
	void	SetSpitLable(float data);
	void	SetSpitLableClear();
	void	SetStartPoint(float x, float y);
	void	SetFileFormat(CString format);

//----------------------------------------------------------------------------------

//20120309 kad07 1DExcel Export Grid Setting
//----------------------------------------------------------------------
private:
	float m_fExportGridUnit;
	float m_Distance;

public:
	void SetExportGridUnit(float fGridUnit);
	bool ChangeExportGridUnitOpt();
	bool ChangeExportGridUnitTor();	

	CArray<TDataPoint*,TDataPoint*>	m_GridChangeValues;
	CArray<INT_PTR,INT_PTR>			m_GridChangeValueCount;

	void SetDistance(float distace);


//----------------------------------------------------------------------

	void CopyGraphDrawData();
	void SetDlgClose()	{	OnClose();	};
	void SetRedrawGraph();
	int	 GetCheckType();

	bool m_bExtType;
	//void FullFitZoom()	{	RestoreLastZoom();	Invalidate();	};//ResetZoom();		};

		
	//std::vector<int>	m_vctItemCount;
	CArray<TDataPoint*,TDataPoint*>	m_LambdaToTransSet;
	CArray<INT_PTR,INT_PTR>			m_LambdaToTransCount;
	CArray<double,double>			m_LambdaToTransLavel;
	bool	m_bLambdaTransCheck;

	bool ChangeExportGridUnitOptLambda();
	CArray<TDataPoint*,TDataPoint*>	m_LambdaToTransNewDataSet;
	CArray<INT_PTR,INT_PTR>			m_LambdaToTransNewDataCount;

	//int	 GetCheckType();
	//bool m_bExtType;

};
