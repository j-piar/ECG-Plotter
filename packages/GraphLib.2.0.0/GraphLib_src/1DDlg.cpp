
#include "stdafx.h"
#include "1DDlg.h"
#include "SetupCurvesDlg.h"
#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum DATA_FILE_FORMAT {NOFORMAT, OPT, TOR, POLAR};

IMPLEMENT_DYNAMIC(C1DDlg, ETSLayoutDialog)
BEGIN_MESSAGE_MAP(C1DDlg, ETSLayoutDialog)
	ON_WM_SIZE()
	ON_WM_RBUTTONUP()
	ON_MESSAGE(WM_KICKIDLE,			OnKickIdle)
	ON_COMMAND(IDM_EXPORT,			OnExport)
	ON_COMMAND(IDM_ZOOM,			Zoom )
	ON_COMMAND(IDM_PAN,				Pan )
	ON_COMMAND(IDM_CURSOR,			Cursor )
	ON_COMMAND(IDM_MEASURE,			Measure)
	ON_COMMAND(IDM_SELECT,			NoOp )
	ON_COMMAND(IDM_RESET,			ResetZoom )
	ON_COMMAND(IDM_INSERTLABEL,		InsertEmptyLabel)
	ON_COMMAND(IDM_PROPERTIES,		OnProperties)
	ON_COMMAND(IDM_LINEARTREND,		LinearTrend)
	ON_COMMAND(IDM_CUBICTREND,		CubicTrend)
	ON_COMMAND(IDM_PARENTCALLBACK,	ParentCallback)
	ON_COMMAND(IDM_ZOOM_BACK,		RestoreLastZoom)
	ON_COMMAND(IDM_EDITCURVE,		Edit)
	ON_COMMAND(IDM_SHOWALL,			OnShow)
	ON_COMMAND(IDM_HIDEALL,			OnHide)
	ON_COMMAND(IDM_PRINT,			OnPrint)
	ON_COMMAND(IDM_ZOOMOUT,			ZoomOut)
	ON_COMMAND(IDM_SETCURVE,		SetCurves)

	ON_UPDATE_COMMAND_UI(IDM_ZOOM, OnUpdateZoom )
	ON_UPDATE_COMMAND_UI(IDM_PAN, OnUpdatePan )
	ON_UPDATE_COMMAND_UI(IDM_CURSOR, OnUpdateCursor )
	ON_UPDATE_COMMAND_UI(IDM_MEASURE, OnUpdateMeasure)
	ON_UPDATE_COMMAND_UI(IDM_SELECT, OnUpdateNoOp )


	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	
	ON_WM_CLOSE()
END_MESSAGE_MAP()

C1DDlg::C1DDlg(CWnd* pParent /*=NULL*/)
	: ETSLayoutDialog(C1DDlg::IDD, pParent)
{
	m_xName = _T("X");
	m_yName = _T("Y");
	m_xFormat = _T("%g");
	m_yFormat = _T("%g");
	m_IsLog = FALSE;
	//m_hIcon = AfxGetApp()->LoadIcon(IDI_1DICON);

	m_MaxX = m_MinX = m_MaxY = m_MinY = 0;

	m_FileFormat = NOFORMAT;
	m_Distance = 0;
	m_bExtType = false;
	m_bLambdaTransCheck = false;
}

C1DDlg::~C1DDlg()
{
	ClearMemory();
	//GetParent()->SendMessage(wm_copydata, this);

}
void C1DDlg::ClearMemory()
{
	INT_PTR i = 0, Count = m_Values.GetSize();
	
	/*
	for(i = 0; i < Count; i++)
	{
		TDataPoint* pValue = m_Values.GetAt(i);
		if(pValue != NULL){
			delete[] pValue; 
			pValue = 0;
		}
	}*/
	m_Values.RemoveAll();
	m_ValueCount.RemoveAll();
	m_Labels.RemoveAll();
}
void C1DDlg::DoDataExchange(CDataExchange* pDX)
{
	ETSLayoutDialog::DoDataExchange(pDX);
}




// C1DDlg 메시지 처리기입니다.

BOOL C1DDlg::OnInitDialog()
{
//	kad07 20110324
////////////////////////////////////////////////////////////////////
	cSetWindowPosition WinPosition;
//	if(m_WinType == TRUE)		// maine -> child
//	{
		WinPosition.SetWindowQuardPos(GetParent(),this,1,m_nDlgCount);
//	}
//	else						//maine -> child -> child
//	{
//		WinPosition.SetWindowQuardPos(GetParent(),this,2,m_nDlgCount);
//	}
////////////////////////////////////////////////////////////////////
	
	ETSLayoutDialog::OnInitDialog();
	SetWindowText(m_Title);
	m_Graph.CreateEx(WS_EX_CLIENTEDGE,_T("XGraph"),_T(""), WS_CHILD  | WS_VISIBLE | SS_SUNKEN, CRect(0,0,0,0), this, IDC_1DPLOT);
	m_Graph.SetCursorFlags(XGC_LEGEND | XGC_VERT | XGC_ADJUSTSMOOTH);
	m_Graph.SetDlgCtrlID(IDC_1DPLOT);
	m_Graph.SetLogScale(m_IsLog);
				
	// Force cursor to snap to the first curve
	m_Graph.ForceSnap(0);
	m_Graph.SetDoubleBuffer(TRUE); 

	//SetIcon(m_hIcon, FALSE);
	//SetIcon(m_hIcon, TRUE);	

	m_Graph.SetGraphMargins(10,10,10,10);
	m_xFormat = _T("%g");
	m_yFormat = _T("%g");

	int i = 0;
	INT_PTR Count = GetCount();
	for(i = 0; i < Count; i++)
	{
		m_Graph.SetCurveData(GetDataPoint(i),GetValueCount(i),	i,	0,	0);	
		m_Graph.GetCurve(i).SetLabel(GetLabel(i));
		m_Graph.GetCurve(i).SetShowMarker(false);
		m_Graph.GetCurve(i).SetMarker(i%8);
		m_Graph.GetCurve(i).SetMarkerType(1);
		m_Graph.GetCurve(i).SetLineSize(2);
	}

	int XCount = m_Graph.GetXAxisCount();
	int YCount = m_Graph.GetYAxisCount();

	double	fMaxValue=0, fMinValue = 0, fSubValue = 0;
	for(i = 0; i < XCount; i++)
	{
		CXGraphAxis& Axis = m_Graph.GetXAxis(i);
		Axis.SetLabel(m_xName);
		Axis.SetShowMarker(false);
		Axis.SetDisplayFmt(_T(m_xFormat));
		Axis.SetShowGrid(true);

		//Axis.SetAutoScale(false);
		fMaxValue = Axis.GetMaxValue();
		
		if(!(m_MaxX == 0 && m_MinX == 0)){
			Axis.SetRange(m_MinX,m_MaxX);
		}
	}

	for(i = 0; i < YCount; i++)
	{
		m_Graph.GetYAxis(i).SetLabel(m_yName);
		m_Graph.GetYAxis(i).SetShowMarker(false);
		m_Graph.GetYAxis(i).SetDisplayFmt(_T(m_yFormat));
		m_Graph.GetYAxis(i).SetShowGrid(true);
		m_Graph.GetYAxis(i).SetMarkerSize(3);
		m_Graph.GetYAxis(i).SetPlacement(CXGraphAxis::apLeft);

		fMaxValue = m_Graph.GetYAxis(i).GetMaxValue();
		fMinValue = m_Graph.GetYAxis(i).GetMinValue();

		fSubValue = (fMaxValue-fMinValue)*0.05;
		m_Graph.GetYAxis(i).SetRange(m_MinY-fMinValue,fMaxValue);

		if(!(m_MaxY == 0 && m_MinY == 0))
		{
			fMinValue = (m_MaxY-m_MinY)*0.05;
			
			m_Graph.GetYAxis(i).SetRange(m_MinY-fMinValue,m_MaxY);
		}
	}
//----------------------------------------------------------
	//for(i = 0; i < XCount; i++)
	//{
	//	CXGraphAxis& Axis = m_Graph.GetXAxis(i);
	//	Axis.SetLabel(m_xName);
	//	Axis.SetShowMarker(false);
	//	Axis.SetDisplayFmt(_T(m_xFormat));
	//	Axis.SetShowGrid(true);
	//
	//	//if(!(m_MaxX == 0 && m_MinX == 0)){
	//	//	Axis.SetRange(m_MinX,m_MaxX);
	//	//}
	//}

	//for(i = 0; i < YCount; i++)
	//{
	//	m_Graph.GetYAxis(i).SetLabel(m_yName);
	//	m_Graph.GetYAxis(i).SetShowMarker(false);
	//	m_Graph.GetYAxis(i).SetDisplayFmt(_T(m_yFormat));
	//	m_Graph.GetYAxis(i).SetShowGrid(true);
	//	m_Graph.GetYAxis(i).SetMarkerSize(3);
	//	m_Graph.GetYAxis(i).SetPlacement(CXGraphAxis::apLeft);

	//	if(!(m_MaxY == 0 && m_MinY == 0))
	//		m_Graph.GetYAxis(i).SetRange(m_MinY,m_MaxY);
	//}
//----------------------------------------------------------

	CWnd *pWnd = GetDlgItem(IDC_STATIC);
	if(pWnd != NULL)
		pWnd->ShowWindow(SW_HIDE);

	InitLayOut();
	if (m_ToolBar.CreateEx(this,TBSTYLE_FLAT | WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
	{
		m_ToolBar.LoadToolBar(IDR_1DPLOT);
		m_ToolBar.EnableToolTips();
	}
	else
	{
		TRACE(_T("Failed to create toolbar bar\n"));
		return FALSE;
	}
	//m_Graph.ResetZoom();
	////////////////////////////////////////////////////////////////////////////////////////////////
	//SJPARK
	//RECT   ClientRect;
	//ClientRect.left	  = 50; 		ClientRect.top	  = 50;
	//ClientRect.right	  = 600;		ClientRect.bottom = 450;

	//if(CWnd *pWnd = GetParent()){
	//	pWnd->GetWindowRect(&ClientRect);
	//}

	//MoveWindow(ClientRect.left , ClientRect.top, ClientRect.left , ClientRect.top);
	////////////////////////////////////////////////////////////////////////////////////////////////
	
	ResetZoom();

	////////////////////////////////////////////////////////////////////////////////////////////////
	//SJPARK
	//m_Graph.Invalidate();
	////////////////////////////////////////////////////////////////////////////////////////////////

	m_ToolBar.LoadTrueColorToolBar(25, IDB_TB_1DPLOT, IDB_TB_1DPLOT);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	
	if(!AfxOleInit()){
	
		int a = 0;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

LRESULT C1DDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case XG_POINTCHANGED:
			{
				CXGraph::NM_POINTMOVING* pNM_POINTMOVING = (CXGraph::NM_POINTMOVING*) wParam;
			}
			break;
		case XG_CURSORMOVED:
			{
				TDataPoint point = ((CXGraph*)lParam)->GetCursorAbsolute(0);
				if (((CXGraph*)lParam) != &m_Graph)
				{
					m_Graph.Cursor();
					m_Graph.SetCursorAbsolute(0, point, true);
					m_Graph.Invalidate();
				}
			}
			break;
		case XG_RBUTTONUP:
			{
				CPoint point;
				GetCursorPos(&point);
				ScreenToClient(&point);
				CMenu menu;
				CXGraph::CreatePopupMenu(menu);
				ClientToScreen(&point);
				UINT nCmd = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY,
												point.x, point.y ,
												(CXGraph*)lParam);
			}
			break;
	}

	return ETSLayoutDialog::WindowProc(message, wParam, lParam);
}

void C1DDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	ETSLayoutDialog::OnRButtonUp(nFlags, point);
}

void C1DDlg::InitLayOut()
{
	
	CreateRoot(VERTICAL)
			<< (pane (HORIZONTAL, GREEDY)
					<< item(IDC_STATIC,NORESIZE))
			<< (pane (HORIZONTAL, GREEDY)
					<< item(IDC_1DPLOT,GREEDY));
	UpdateLayout();
	
}


void C1DDlg::SetCurves()
{
	CSetupCurvesDlg dlg;
	CXGraphDataSerie data;
	int i = 0, Count = m_Graph.GetCurveCount();
	for(i = 0; i < Count; i++)
	{
		dlg.AddCurve(&m_Graph.GetCurve(i));
	}
	if(dlg.DoModal() == IDOK)
		Invalidate();
}
//
//void C1DDlg::OnExport()
//{
//	 // exactly the same name as in the ODBC-Manager
//	//20130617 kad07 CSV Data Sort
//	CFileDialog dlg(FALSE,"XLS",NULL, OFN_OVERWRITEPROMPT,"CSV Files(*.csv)|*.csv|Excel Files(*.xls)|*.xls|Text Files(*.txt)|*.txt|",this);
//	dlg.m_ofn.lpstrTitle = "Save Data";
//	if(dlg.DoModal() == IDOK)
//	{
//		CString ext = dlg.GetFileExt();
//		ext.MakeLower();
//		if(ext == _T("xls"))
//		{
//			m_bExtType = true;
//			Export2Excel(dlg.GetPathName());
//		}
//		else if(ext == _T("csv"))	//----	20130617 kad07 CSV Data Sort
//		{
//			m_bExtType = false;
//			Export2Excel(dlg.GetPathName());
//		}
//		else
//			ExportData(dlg.GetPathName());
//	}
//}

void C1DDlg::ExportData(CString fname)
{
	FILE* dat = NULL;
	errno_t result = fopen_s(&dat,fname,_T("w"));
	if(dat != NULL)
	{
		fprintf(dat,_T("%s %s\n"), m_xName, m_yName);
		INT_PTR i = 0,Count = GetCount();
		int j=0;
		TDataPoint* pValueData;
		for(i = 0; i < Count; i++)
		{
			pValueData = m_Values.GetAt(i);
			for(j = 0; j < m_ValueCount.GetAt(i); j++)
				fprintf(dat, _T("%g %g\n"), pValueData[j].fXVal,pValueData[j].fYVal);
			fprintf(dat,_T("\n"));
		}
		fclose(dat);
	}
}
//
//void C1DDlg::Export2Excel(CString filename)
//{
//	ExcelDataSet excelData;
//	excelData.SetNewDB();
//	excelData.StartTable(_T("ELCD 1D Plot"));
//	CFileStatus fs;
//	if(CFile::GetStatus(filename,fs) == TRUE)
//		DeleteFile(filename);
//
//	int iLine	= 0;	
//	int iLow	= 0;
//	INT_PTR i = 0, Count =  GetCount() ,x = 0;
//	//excelData.SetData(iLine, 0, m_xName);
//
//	//String strFileFormat = GetFileName();
//	if(!m_vctSplitData.size() == 0)
//	{
////20120309 kad07 1DExcel Export Grid Setting
////------------------------------------------------------------------------------
//		if(ChangeExportGridUnitTor())			// 
//		{
//			CString Label;
//			INT_PTR ValueCount = m_GridChangeValueCount.GetCount();
//
//			excelData.SetData(iLine, 0, m_xName);
//
//			float	fSearchSDataIndex = (float)m_vctSplitData.size() / (float)Count;			
//			for(i = 0; i < Count; i++)
//			{
//				Label.Format(_T("(%.2f)"),m_vctSplitData.at((int)((float)i*fSearchSDataIndex)));	
//				Label = GetLabel(i) + Label;
//
//				//Label.Format(_T("(%.2f)"),m_vctSplitData.at(i));
//				//Label = GetLabel(i) + Label;
//
//				excelData.SetData(iLine,(int)(i+1), Label);
//				//excelData.SetData(iLine, x, m_xName);
//			}
//
//			iLine++;
//
//			if(m_Values.GetCount() > 0)
//			{
//				int didx = 0;
//				INT_PTR idx = 0;//, PTCount = m_ValueCount.GetCount();
//
//
////----------------------------------------------------------------------- 20130617 kad07 CSV Data Sort
//				//for(idx = 0,x=0; idx < Count; idx++,x++)
//				//{
//				//	iLine = 1;
//				//	//int nValueCount = m_ValueCount.GetAt(idx);
//				//	int nValueCount = m_GridChangeValueCount.GetAt(idx);
//
//				//	for(didx=0;didx < nValueCount;didx++,iLine++)
//				//	{
//				//		if(idx == 0)
//				//			excelData.SetData(iLine,x,m_GridChangeValues.GetAt(idx)[didx].fXVal);
//
//				//		excelData.SetData(iLine,x+1,m_GridChangeValues.GetAt(idx)[didx].fYVal);
//				//	}
//				//}
////----------------
//				int nValueCount = m_GridChangeValueCount.GetAt(0);
//
//				for(didx=0;didx < nValueCount;didx++,iLine++)
//				{
//					for(idx = 0,x=0; idx < Count; idx++,x++)
//					{
//						if(idx == 0)
//							excelData.SetData(iLine,x,m_GridChangeValues.GetAt(idx)[didx].fXVal);
//
//						excelData.SetData(iLine,x+1,m_GridChangeValues.GetAt(idx)[didx].fYVal);
//					}
//				}
////-----------------------------------------------------------------------------------------------------
//			}
//		}
//		else
//		{
//			CString Label;
//
//			for(i = 0; i < Count; i++)
//			{
//				Label.Format(_T("(%.2f)"),m_vctSplitData.at(i));
//				Label = GetLabel(i) + Label;
//				
//				excelData.SetData(iLine, x, m_xName);
//				excelData.SetData(iLine,(int)(x+1), Label);
//				x = x+2;
//			}
//
//			if(m_Values.GetCount() > 0)
//			{
//				int didx = 0;
//				INT_PTR idx = 0, PTCount = m_ValueCount.GetCount();//PTCount = m_ValueCount.GetAt(0);
//				//INT_PTR ValueCount = m_Values.GetCount();
//
//				for(idx = 0,x=0; idx < PTCount; idx++)
//				{
//					iLine = 1;
//					int nValueCount = m_ValueCount.GetAt(idx);
//					
//					for(didx=0;didx < nValueCount;didx++,iLine++)
//					{
//						excelData.SetData(iLine,x,m_Values.GetAt(idx)[(nValueCount-1)-didx].fXVal);
//						excelData.SetData(iLine,x+1,m_Values.GetAt(idx)[(nValueCount-1)-didx].fYVal);
//					}
//					x = x+2;
//					//excelData.SetData(iLine,0,m_Values.GetAt(0)[idx].fXVal);
//				}
//			}
//		}
////------------------------------------------------------------------------------
//	}
////kad07 20120117 1D2DViewer 1D Cut excel File out
////------------------------------------------------------------------------------------------------------------------
//	//if(!m_vctSplitData.size() == 0)
//	//{
//	//	CString Label;
//
//	//	for(i = 0; i < Count; i++)
//	//	{
//	//		Label.Format(_T("(%.2f)"),m_vctSplitData.at(i));
//	//		Label = GetLabel(i) + Label;
//	//		
//	//		excelData.SetData(iLine, x, m_xName);
//	//		excelData.SetData(iLine,(int)(x+1), Label);
//	//		x = x+2;
//	//	}
//
//	//	if(m_Values.GetCount() > 0)
//	//	{
//	//		int didx = 0;
//	//		INT_PTR idx = 0, PTCount = m_ValueCount.GetCount();//PTCount = m_ValueCount.GetAt(0);
//	//		//INT_PTR ValueCount = m_Values.GetCount();
//
//	//		for(idx = 0,x=0; idx < PTCount; idx++)
//	//		{
//	//			iLine = 1;
//	//			int nValueCount = m_ValueCount.GetAt(idx);
//	//			
//	//			for(didx=0;didx < nValueCount;didx++,iLine++)
//	//			{
//	//				excelData.SetData(iLine,x,m_Values.GetAt(idx)[(nValueCount-1)-didx].fXVal);
//	//				excelData.SetData(iLine,x+1,m_Values.GetAt(idx)[(nValueCount-1)-didx].fYVal);
//	//			}
//	//			x = x+2;
//	//			//excelData.SetData(iLine,0,m_Values.GetAt(0)[idx].fXVal);
//	//		}
//	//	}
//	//}
//	else if(m_FileFormat == NOFORMAT)
//	{
//		CString LabelTemp;
//		CString Lable;
//
//		INT_PTR ValueCount = m_Values.GetCount();
//		//LabelTemp.Format(_T(" (X:%.2f,Y:%.2f)"),m_StartX, m_StartY);
//		//Lable = GetLabel(0) + LabelTemp;
//
//		excelData.SetData(iLine, 0, m_xName);
//		//excelData.SetData(iLine, 1, Lable);
//
//		int nLabelCount = m_Labels.GetSize();
//		for(int i=0;i<nLabelCount;i++)
//		{
//			Lable = GetLabel(i);
//			excelData.SetData(iLine, i+1, Lable);
//		}
//
//		iLine++;
//
//		int didx = 0;
//		INT_PTR idx = 0, PTCount = m_ValueCount.GetAt(0);
//		//INT_PTR ValueCount = m_Values.GetCount();
//		for(idx = 0; idx < PTCount; idx++)
//		{
//			excelData.SetData(iLine,0,m_Values.GetAt(0)[idx].fXVal);
//			//excelData.SetData(iLine,0,m_Values.GetAt(0)[(PTCount-1)-idx].fXVal);
//			for(didx = 0;didx < ValueCount; didx++)
//				excelData.SetData(iLine,(int)(didx+1),m_Values.GetAt(didx)[idx].fYVal);
//				//excelData.SetData(iLine,(int)(didx+1),m_Values.GetAt(didx)[(PTCount-1)-idx].fYVal);
//			iLine++;
//		}
//	}
//	else if(m_FileFormat == OPT)
//	{
//		//20130624 Lambda trans
//		if(m_bLambdaTransCheck)
//		{
//			if(!ChangeExportGridUnitOptLambda())
//				m_bLambdaTransCheck = false;
//		}
//
//		if(ChangeExportGridUnitOpt())	//20120309 kad07 1DExcel Export Grid Setting
//		{
////20120309 kad07 1DExcel Export Grid Setting
////--------------------------------------------------------------------------------------
//			if(m_GridChangeValueCount.GetCount() > 0)
//			{
//				int didx = 0;
//				INT_PTR idx = 0;
//				INT_PTR ValueCount = m_GridChangeValueCount.GetCount();
//
//				CString Lable;
//				int x=0;
//
//				excelData.SetData(iLine, 0, m_xName);
//
//				for(int i=0;i<ValueCount;i++)
//				{
//					Lable = GetLabel(i);
//					excelData.SetData(iLine, i+1/*x+1*/, Lable);
//					iLow = i+2;
//				}
//
//				if(m_bLambdaTransCheck)
//				{
//					int nLambdaCount = m_LambdaToTransLavel.GetCount();
//
//					for(int i=0;i<nLambdaCount;i++,iLow++)
//						excelData.SetData(iLine,iLow,m_LambdaToTransLavel.GetAt(i));
//				}
//
//				iLine++;
//
//				//for(idx = 0,x=0; idx < ValueCount; idx++)
//				//{
//				//	iLine = 1;
//				//	int nValueCount = m_ValueCount.GetAt(idx);
//				//}
////----------------------------------------------------------------------- 20130617 kad07 CSV Data Sort
//				//for(idx = 0,x=0; idx < ValueCount; idx++,x++)
//				//{
//				//	iLine = 1;
//				//	int nValueCount = m_GridChangeValueCount.GetAt(idx);
//				//	
//				//	for(didx=0;didx < nValueCount;didx++,iLine++)
//				//	{
//				//		if(idx == 0)
//				//			excelData.SetData(iLine,x,m_GridChangeValues.GetAt(idx)[didx].fXVal);
//
//				//		excelData.SetData(iLine,x+1,m_GridChangeValues.GetAt(idx)[didx].fYVal);
//				//	}
//				//	//x = x+2;
//				//}
////------------
//				int nValueCount = m_GridChangeValueCount.GetAt(0);	// Item 수 
//				//didx : 행, idx : 열  
//				for(didx=0;didx<nValueCount;didx++,iLine++)	
//				{
//					for(idx=0, x=0;idx<ValueCount;idx++,x++)	
//					{
//						if(idx == 0)
//							excelData.SetData(iLine,x,m_GridChangeValues.GetAt(idx)[didx].fXVal);
//
//						excelData.SetData(iLine,x+1,m_GridChangeValues.GetAt(idx)[didx].fYVal);
//					}
//
//					if(m_bLambdaTransCheck)
//					{
//						int nLambdaCount = m_LambdaToTransLavel.GetCount();
//						for(int i=0;i<nLambdaCount;i++)
//							excelData.SetData(iLine,(i+2),m_LambdaToTransNewDataSet.GetAt(i)[didx].fYVal);
//					}
//
//				}
////------------------------------------------------------------------------------------------------------
//			}
//			else
//				AfxMessageBox(_T("New Grid Data is Empty"));
////--------------------------------------------------------------------------------------
//		}
//		else
//		{
//			if(m_Values.GetCount() > 0)
//			{
//				int didx = 0;
//				INT_PTR idx = 0, PTCount = m_ValueCount.GetAt(0);	
//				INT_PTR ValueCount = m_Values.GetCount();
//
//				CString Lable;
//				int x=0;
//
//				for(int i=0;i<ValueCount;i++)
//				{
//					excelData.SetData(iLine, x, m_xName);
//					Lable = GetLabel(i);
//					excelData.SetData(iLine, x+1, Lable);
//					x=x+2;
//				}
//				iLine++;
//
//				//for(idx = 0,x=0; idx < ValueCount; idx++)
//				//{
//				//	iLine = 1;
//				//	int nValueCount = m_ValueCount.GetAt(idx);
//				//}
//
//				for(idx = 0,x=0; idx < ValueCount; idx++)
//				{
//					iLine = 1;
//					int nValueCount = m_ValueCount.GetAt(idx);
//					
//					for(didx=0;didx < nValueCount;didx++,iLine++)
//					{
//						excelData.SetData(iLine,x,m_Values.GetAt(idx)[didx].fXVal);
//						excelData.SetData(iLine,x+1,m_Values.GetAt(idx)[didx].fYVal);
//					}
//					x = x+2;
//				}
//			}
//			else
//				AfxMessageBox(_T("New Grid Data is Empty"));
//		}
//
//	}
//	else if(m_FileFormat == POLAR)
//	{
//		int nGraphCount		= m_Values.GetSize();
//		int nGraphItemCount	= 0;
//		int nType	= GetCheckType();
//		int nLine	= 0, nLow = 0;
//		TDataPoint*	pTData;
//
//	switch(nType)
//		{
//		case 0:		//Transmittance vs. Phi
//		case 1:		//Transmittance vs. Theta
//		case 2:		//Transmittance vs. Voltage
////----------------------------------------------------------------------- 20130617 kad07 CSV Data Sort
//			//excelData.SetData(nLine, nLow,m_xName);		// Type Name
//
//			//for(int i=0;i<nGraphCount;i++)
//			//{
//			//	nGraphItemCount = m_ValueCount.GetAt(i);
//			//	if(i==0)
//			//	{
//			//		for(int y=0;y<nGraphItemCount;y++)
//			//			excelData.SetData(y+1, nLow, m_Values.GetAt(i)[y].fXVal); 
//
//			//		nLow++;
//			//	}
//
//			//	for(int y=0;y<=nGraphItemCount;y++)
//			//	{
//			//		if(y==0)
//			//			excelData.SetData(nLine, nLow, m_Labels.GetAt(i)); 
//			//		else
//			//			excelData.SetData(nLine, nLow, m_Values.GetAt(i)[y-1].fYVal); 
//
//			//		nLine++;
//			//	}
//			//	nLow++;
//			//	nLine = 0;
//			//}
////------------------
//			excelData.SetData(nLine, nLow,m_xName);		// Type Name
//
//			for(int i=0;i<nGraphCount;i++)	// curve Labels
//				excelData.SetData(nLine, i+1, m_Labels.GetAt(i)); 
//
//			nLine++;
//
//			nGraphItemCount = m_ValueCount.GetAt(0);
//			for(int y=0;y<nGraphItemCount;y++)
//			{
//				for(int i=0;i<nGraphCount;i++)
//				{
//					if(i == 0)
//						excelData.SetData(nLine, i, m_Values.GetAt(i)[y].fXVal); 
//
//					excelData.SetData(nLine, i+1, m_Values.GetAt(i)[y].fYVal); 
//				}
//				nLine++;
//			}
//
////------------------------------------------------------------------------------------------------------
//
//			break;
//
//		case 3:		//Gamma Graph
////----------------------------------------------------------------------- 20130617 kad07 CSV Data Sort
//			//excelData.SetData(nLine, nLow,m_xName);		// Type Name
//
//			//for(int i=1;i<nGraphCount;i++)
//			//{
//			//	nGraphItemCount = m_ValueCount.GetAt(i);
//			//	if(i==1)
//			//	{
//			//		for(int y=0;y<nGraphItemCount;y++)
//			//			excelData.SetData(y+1, nLow, m_Values.GetAt(i)[y].fXVal); 
//
//			//		nLow++;
//			//	}
//
//			//	for(int y=0;y<=nGraphItemCount;y++)
//			//	{
//			//		if(y==0)
//			//			excelData.SetData(nLine, nLow, m_Labels.GetAt(i)); 
//			//		else
//			//			excelData.SetData(nLine, nLow, m_Values.GetAt(i)[y-1].fYVal); 
//
//			//		nLine++;
//			//	}
//			//	nLow++;
//			//	nLine = 0;
//			//}
////----------------------------
//			//for(int i=1;i<nGraphCount;i++)
//			//{
//			//	nGraphItemCount = m_ValueCount.GetAt(i);
//			//	if(i==1)
//			//	{
//			//		for(int y=0;y<nGraphItemCount;y++)
//			//			excelData.SetData(y+1, nLow, m_Values.GetAt(i)[y].fXVal); 
//
//			//		nLow++;
//			//	}
//
//			//	for(int y=0;y<=nGraphItemCount;y++)
//			//	{
//			//		if(y==0)
//			//			excelData.SetData(nLine, nLow, m_Labels.GetAt(i)); 
//			//		else
//			//			excelData.SetData(nLine, nLow, m_Values.GetAt(i)[y-1].fYVal); 
//
//			//		nLine++;
//			//	}
//			//	nLow++;
//			//	nLine = 0;
//			//}
//
//			////---
//
//			excelData.SetData(nLine, nLow,m_xName);		// Type Name
//			for(int i=1;i<nGraphCount;i++)	
//				excelData.SetData(nLine, i, m_Labels.GetAt(i)); // curve Labels
//
//			nLine++;
//
//			nGraphItemCount = m_ValueCount.GetAt(1);
//			//for(int i=1;i<nGraphCount;i++)	//Get max value count
//			//{
//			//	if(nGraphItemCount < m_ValueCount.GetAt(i))
//			//		nGraphItemCount = m_ValueCount.GetAt(i);
//			//}
//
//			for(int i=0;i<nGraphItemCount;i++)
//			{
//				for(int y=1;y<nGraphCount;y++,nLow++)
//				{
//					
//					if(y==1)
//						excelData.SetData(nLine, nLow, m_Values.GetAt(y)[i].fXVal);
//					
//					excelData.SetData(nLine, nLow+1, m_Values.GetAt(y)[i].fYVal); 
//				}
//
//				nLine++;
//				nLow = 0;
//			}
////------------------------------------------------------------------------------------------------------
//			break;
//
//			//for(int i=1;i<nGraphCount;i++)
//			//{
//			//	nGraphItemCount = m_ValueCount.GetAt(i);
//
//			//	for(int y=0;y<=nGraphItemCount;y++)
//			//	{
//			//		if(y==0)
//			//		{
//			//			excelData.SetData(nLine, nLow,	"Color Level");		// Type Name
//			//			excelData.SetData(nLine, nLow+1,	m_Labels.GetAt(i)); 
//			//		}
//			//		else
//			//		{
//			//			excelData.SetData(nLine,	nLow	, m_Values.GetAt(i)[y-1].fXVal); 
//			//			excelData.SetData(nLine,	nLow+1	, m_Values.GetAt(i)[y-1].fYVal); 
//			//		}
//
//			//		nLine++;
//			//	}
//			//	nLow++;
//			//	nLow++;
//			//	nLine = 0;
//			//}
//			break;
//
//		default:
//			break;
//		}
//
//
//	}
//
////------------------------------------------------------------------------------------------------------------------
////	if(m_Values.GetCount() > 0)
////	{
////		int didx = 0;
////		INT_PTR idx = 0, PTCount = m_ValueCount.GetAt(0);
////		INT_PTR ValueCount = m_Values.GetCount();
////		for(idx = 0; idx < PTCount; idx++)
////		{
////			if(idx > 0)
////			{
//////------------------------------------------------------------------------------------------------------------------
////		////sjpark
////		////		if(m_Values.GetAt(0)[idx].fXVal - m_Values.GetAt(0)[idx-1].fXVal > 1e-10)
////		////		{
////		//			excelData.SetData(iLine,0,m_Values.GetAt(0)[idx].fXVal);
////		//			for(didx = 0;didx < ValueCount; didx++)
////		//				excelData.SetData(iLine,(int)(didx+1),m_Values.GetAt(didx)[idx].fYVal);
////		//			iLine++;
////		////		}
//////------------------------------------------------------------------------------------------------------------------
////		//kad07 20110607 1D2DViewer 1D Cut excel 
////				if(m_Values.GetAt(0)[idx].fXVal - m_Values.GetAt(0)[idx-1].fXVal < 1e-10)
////				{
////					TRACE("[X] : %f ",m_Values.GetAt(0)[idx].fXVal);
////				
////					excelData.SetData(iLine,0,m_Values.GetAt(0)[idx].fXVal);
////					for(didx = 0;didx < ValueCount; didx++)
////					{
////						excelData.SetData(iLine,(int)(didx+1),m_Values.GetAt(didx)[idx].fYVal);
////
////						TRACE("[Y] : %f " ,m_Values.GetAt(didx)[idx].fYVal);
////					}
////					TRACE("\n");
////					iLine++;
////				}
//////------------------------------------------------------------------------------------------------------------------
////			}
////			else
////			{
////				excelData.SetData(iLine,0,m_Values.GetAt(0)[idx].fXVal);
////				for(didx = 0;didx < ValueCount; didx++)
////					excelData.SetData(iLine,(int)(didx+1),m_Values.GetAt(didx)[idx].fYVal);
////				iLine++;
////			}
////		}
////	}
//	excelData.FinishTable();
//	//excelData.ExcuteXLSMake(filename);
//	if(m_bExtType)
//		excelData.ExcuteXLSMake(filename);
//	else
//		excelData.ExcuteCSVMake(filename);
//}

int C1DDlg::GetCheckType()
{
	if(m_Title.Compare("Transmittance vs. Phi") == 0)	//
		return 0;
	if(m_Title.Compare("Transmittance vs. Theta") == 0)
		return 1;
	if(m_Title.Compare("Transmittance vs. Voltage") == 0)
		return 2;
	if(m_Title.Compare("Gamma Graph") == 0)
		return 3;

}

/*
void C1DDlg::Export2Excel(CString fname)
{
	CString sDriver = _T("MICROSOFT EXCEL DRIVER (*.XLS)");
	TRY
	{
		CDatabase database;
		CFileStatus fs;
		if(CFile::GetStatus(fname,fs) == TRUE)
			DeleteFile(fname);

		CString sSql, keysql;
		CString xname = m_xName;
		CString yname = m_yName;

		sSql.Format(_T("DRIVER={%s};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s"), sDriver,fname,fname);
		// Create the database (i.e. Excel sheet)

		TDataPoint*	pXPoint = NULL;
		TDataPoint*	pYPoint = NULL;
		int x = 0;
		INT_PTR xcnt = 0;
		INT_PTR i = 0, Count =  GetCount();

		if(database.OpenEx(sSql,CDatabase::noOdbcDialog) )
		{
			sSql.Format(_T("CREATE TABLE ELD1DEXPORT (\'%s\' NUMBER"),xname);
			for(i = 0; i < Count; i++)
			{
				keysql.Format(_T(",\'%s\' NUMBER"),GetLabel(i));
				sSql += keysql;
			}
			sSql += _T(')');
			
			database.ExecuteSQL(sSql);

			pXPoint = m_Values.GetAt(0);
			xcnt = m_ValueCount.GetAt(0);
			
			for(x = 0; x < xcnt; x++)
			{
				sSql.Format(_T("INSERT INTO ELD1DEXPORT (\'%s\'"),xname);
				
				for(i = 0; i < Count; i++)
				{
					keysql.Format(_T(",\'%s\'"),GetLabel(i));
					sSql += keysql;
				}
				sSql += _T(") VALUES ( ");
				
				keysql.Format("%g",m_Values.GetAt(0)[x].fXVal);
				sSql += keysql;
				for(i = 0; i < Count; i++)
				{
					keysql.Format(_T(",%g"),m_Values.GetAt(i)[x].fYVal);
					sSql += keysql;
				}
				sSql += ')';
				database.ExecuteSQL(sSql);
			}
			//for(i = 0; i < Count; i++)
			//{
			//	xcnt = m_ValueCount.GetAt(i);
			//	pDataPoint = m_Values.GetAt(i);
			//	sSql.Format(_T("INSERT INTO ELD1DEXPORT (Title,\'%s\',\'%s\') VALUES (\'%s\',0,0)"),xname,yname,GetLabel(i));
			//	database.ExecuteSQL(sSql);
			//	for(x = 0; x < xcnt; x++)
			//	{
			//		sSql.Format(_T("INSERT INTO ELD1DEXPORT (Title,\'%s\',\'%s\') VALUES (\'\',%g,%g)"),xname,yname,pDataPoint[x].fXVal,pDataPoint[x].fYVal);
			//		database.ExecuteSQL(sSql);
			//	}
			//}
			// Close database
			database.Close();
		}
	}
	CATCH_ALL(e)
	{
		TRACE1("Driver not installed: %s",sDriver);
		CString errormsg;
		errormsg.Format(_T("Driver not installed: %s"),sDriver);
		AfxMessageBox(errormsg);
	}
	END_CATCH_ALL;
}*/

void C1DDlg::OnOK()
{
	m_ValueCount.RemoveAll();
	m_Labels.RemoveAll();
	ETSLayoutDialog::OnOK();
}

void C1DDlg::OnSize(UINT nType, int cx, int cy) 
{
	ETSLayoutDialog::OnSize(nType, cx, cy);
}


BOOL C1DDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	TCHAR szFullText[256];
	CString strTipText;
	CString strPromtpText;
	UINT_PTR nID = pNMHDR->idFrom;

	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND)) {
		nID = ((UINT)(WORD)::GetDlgCtrlID((HWND)nID));
	}

	if ((nID != 0) && (nID != ID_VIEW_STATUS_BAR)) {
		AfxLoadString((UINT)nID, szFullText);
		AfxExtractSubString(strTipText, szFullText, 1, '\n');
		AfxExtractSubString(strPromtpText, szFullText, 0, '\n');
	}
#ifndef _UNICODE
	if (pNMHDR->code == TTN_NEEDTEXTA)
		lstrcpyn(pTTTA->szText, strTipText,(sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0])));
	else
		_mbstowcsz(pTTTW->szText, strTipText,(sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0])));
#else
	if (pNMHDR->code == TTN_NEEDTEXTA)
		_wcstombsz(pTTTA->szText, strTipText,(sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0])));
	else
		lstrcpyn(pTTTW->szText, strTipText,(sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0])));
#endif
	*pResult = 0;

//	::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

	return TRUE;
}

LRESULT C1DDlg::OnKickIdle(WPARAM, LPARAM lCount)
{
	m_ToolBar.SendMessage(WM_IDLEUPDATECMDUI,TRUE);
	return (lCount <= 2);
}


void C1DDlg::OnClose()
{
	ClearMemory();
	ETSLayoutDialog::OnClose();
	
	GetParent()->SendMessage(WMUSER_DESTROY1D, WPARAM(this), 0);
}

//20120126 kad07 1D Graph Lable
//----------------------------------------------------------------------------------
void C1DDlg::SetSpitLable(float data)
{
	m_vctSplitData.push_back(data);
}

void C1DDlg::SetSpitLableClear()
{
	m_vctSplitData.clear();
}

void C1DDlg::SetStartPoint(float x, float y)
{
	m_StartX = x;
	m_StartY = y;
}

void C1DDlg::SetFileFormat(CString format)
{
	if(format.CompareNoCase(_T("OPT")) == 0)
		m_FileFormat = OPT;
	else if(format.CompareNoCase(_T("POLAR")) == 0)
		m_FileFormat = POLAR;
	else 
		m_FileFormat = NOFORMAT;
}
//----------------------------------------------------------------------------------

//20120309 kad07 1DExcel Export Grid Setting
//----------------------------------------------------------------------
void C1DDlg::SetExportGridUnit(float fGridUnit)
{
	m_fExportGridUnit = fGridUnit;
}

bool C1DDlg::ChangeExportGridUnitOpt()
{
	if(m_Values.GetCount() > 0)
	{
		int didx = 0, nNewDataIndex=0;
		INT_PTR idx = 0;
		INT_PTR ValueCount = m_Values.GetCount();
		float fGrid=0;
		m_GridChangeValues.RemoveAll();
		m_GridChangeValueCount.RemoveAll();

		for(idx = 0; idx < ValueCount; idx++)	// 줄수만큼 반복
		{
			int nValueCount = m_ValueCount.GetAt(idx);
			TDataPoint* pData = new TDataPoint[_MAX_PATH];
			nNewDataIndex = 0;

			fGrid = m_fExportGridUnit;

			//Interval 0
			pData[nNewDataIndex].fXVal = 0;
			pData[nNewDataIndex].fYVal = m_Values.GetAt(idx)[0].fYVal;
			nNewDataIndex++;

			//앞쪽 Data 채움
			TRACE(_T("1:%f\n"),m_Values.GetAt(idx)[0].fXVal);	
			if(fGrid<m_Values.GetAt(idx)[0].fXVal)
			{
				int nEmptyData = m_Values.GetAt(idx)[0].fXVal/fGrid;

				for(int i=0;i<nEmptyData;i++)
				{
					pData[nNewDataIndex].fXVal = fGrid;
					pData[nNewDataIndex].fYVal = m_Values.GetAt(idx)[0].fYVal;
					nNewDataIndex++;
					fGrid = fGrid+m_fExportGridUnit;
				}
			}
			
			for(didx=0;didx < nValueCount;didx++)
			{
				TRACE(_T("2:%f\n"),m_Values.GetAt(idx)[didx].fXVal);	
				if(fGrid<m_Values.GetAt(idx)[0].fXVal)
				{
					fGrid=fGrid+m_fExportGridUnit;
					continue;
				}

				//Unit Grid 앞뒤의 Random Grid를 이용하여 기울기를 구한다. 
				TRACE(_T("3:%f\n"),m_Values.GetAt(idx)[didx].fXVal);	
				if(fGrid < m_Values.GetAt(idx)[didx].fXVal && didx != 0)
				{
					float fGrid_1, fGrid_2, fData_1, fData_2;
					float fSlope, fNewData;
					
					fGrid_1 = m_Values.GetAt(idx)[didx-1].fXVal;
					fGrid_2 = m_Values.GetAt(idx)[didx].fXVal;
					fData_1 = m_Values.GetAt(idx)[didx-1].fYVal;
					fData_2 = m_Values.GetAt(idx)[didx].fYVal;
					
					//기울기
					fSlope = (fData_2 - fData_1) / (fGrid_2 - fGrid_1);
				
					//New Grid의 Data
					fNewData = (fSlope * (fGrid - fGrid_1)) + fData_1;
					
					pData[nNewDataIndex].fXVal =  fGrid;
					pData[nNewDataIndex].fYVal =  fNewData;
					
					fGrid = fGrid + m_fExportGridUnit;
					nNewDataIndex++;
				}
			}
			
			
			//뒤쪽 Data 채움
			TRACE(_T("4:%f\n"),m_Values.GetAt(idx)[nValueCount-1].fXVal);
			//if(fGrid < m_Values.GetAt(idx)[nValueCount-1].fXVal && didx == nValueCount)
			if(fGrid <= m_Distance && didx == nValueCount)
			{
				//for(int i=0;fGrid < m_Values.GetAt(idx)[nValueCount-1].fXVal;i++)
				//for(int i=0;fGrid <= m_Distance;i++)
				while(fGrid <= m_Distance)
				{
					pData[nNewDataIndex].fXVal	= fGrid;
					TRACE(_T("4:%f\n"),m_Values.GetAt(idx)[nValueCount-1].fXVal);
					TRACE(_T("4:%f\n"),m_Values.GetAt(idx)[0].fXVal);
					pData[nNewDataIndex].fYVal	= m_Values.GetAt(idx)[nValueCount-1].fYVal;
					fGrid = fGrid + m_fExportGridUnit;
					nNewDataIndex++;
				}
			}

			//NewData ValiueCount
			m_GridChangeValues.Add(pData);
			m_GridChangeValueCount.Add(nNewDataIndex);
		}
	}
	else
	{
		AfxMessageBox(_T("date is empty"));
		return false;
	}
	
	return true;
}

//20130624 Lambda trans
bool C1DDlg::ChangeExportGridUnitOptLambda()
{
	int LambdaCount = m_LambdaToTransSet.GetCount();

	if(LambdaCount<1)
	{
		AfxMessageBox("Lambda Data is Empty");
		return false;
	}

	float	fGrid = 0;
	int		nValueCount = 0, nDataIndex = 0, idx=0, didx=0;
	TDataPoint* pData = NULL;

	for(idx=0;idx<LambdaCount;idx++)
	{
		nValueCount = m_LambdaToTransCount.GetAt(idx);
		pData = new TDataPoint[_MAX_PATH];

		nDataIndex = 0;
		fGrid = m_fExportGridUnit;

		//interval
		pData[nDataIndex].fXVal = 0;
		pData[nDataIndex].fYVal = m_LambdaToTransSet.GetAt(idx)[0].fYVal;
		nDataIndex++;

		if(fGrid < m_LambdaToTransSet.GetAt(idx)[0].fXVal)
		{
			int nEmptyData = m_LambdaToTransSet.GetAt(idx)[0].fXVal / fGrid;

			for(int x=0;x<nEmptyData;x++)
			{
				pData[nDataIndex].fXVal = fGrid;
				pData[nDataIndex].fYVal = m_LambdaToTransSet.GetAt(idx)[0].fYVal;
				nDataIndex++;

				fGrid = fGrid + m_fExportGridUnit;
			}
		}

		for(didx=0;didx<nValueCount;didx++)
		{
			if(fGrid<m_LambdaToTransSet.GetAt(idx)[0].fXVal)
			{
				fGrid = fGrid + m_fExportGridUnit;
				continue;
			}


			if(fGrid<m_LambdaToTransSet.GetAt(idx)[didx].fXVal && didx != nValueCount)
			{
				float fGrid_1, fGrid_2, fData_1, fData_2;
				float fSlope, fNewData;

				fGrid_1 = m_LambdaToTransSet.GetAt(idx)[didx-1].fXVal;
				fGrid_2 = m_LambdaToTransSet.GetAt(idx)[didx].fXVal;
				fData_1 = m_LambdaToTransSet.GetAt(idx)[didx-1].fYVal;
				fData_2 = m_LambdaToTransSet.GetAt(idx)[didx].fYVal;

				fSlope = (fData_2 - fData_1) / (fGrid_2 - fGrid_1);

				fNewData = (fSlope * (fGrid - fGrid_1)) + fData_1;

				pData[nDataIndex].fXVal = fGrid;
				pData[nDataIndex].fYVal = fNewData;

				fGrid = fGrid + m_fExportGridUnit;
				nDataIndex++;
			}
		}

		if(fGrid <= m_Distance && didx == nValueCount)
		{
			while(fGrid <= m_Distance)
			{
				pData[nDataIndex].fXVal	= fGrid;
				pData[nDataIndex].fYVal	= m_Values.GetAt(idx)[nValueCount-1].fYVal;

				fGrid = fGrid + m_fExportGridUnit;
				nDataIndex++;
			}
		}

		m_LambdaToTransNewDataSet.Add(pData);
		m_LambdaToTransNewDataCount.Add(nDataIndex);
	}

	return true;
}

bool C1DDlg::ChangeExportGridUnitTor()
{
	if(m_Values.GetCount() > 0)
	{
		int didx = 0, nNewDataIndex=0;
		INT_PTR idx = 0;
		INT_PTR ValueCount = m_Values.GetCount();
		float fGrid=0;

		for(idx = 0; idx < ValueCount; idx++)	// 줄수만큼 반복
		{
			int nValueCount = m_ValueCount.GetAt(idx);
			TDataPoint* pData = new TDataPoint[_MAX_PATH];
			nNewDataIndex = 0;

			fGrid = m_fExportGridUnit;

			//interval 0
			pData[nNewDataIndex].fXVal = 0;
			pData[nNewDataIndex].fYVal = m_Values.GetAt(idx)[nValueCount-1].fYVal;
			nNewDataIndex++;

			//앞쪽 Data 채움
			TRACE(_T("1:%f\n"),m_Values.GetAt(idx)[nValueCount-1].fXVal);	
			if(fGrid<m_Values.GetAt(idx)[nValueCount-1].fXVal)
			{
				int nEmptyData = m_Values.GetAt(idx)[nValueCount-1].fXVal/fGrid;

				for(int i=0;i<nEmptyData;i++)
				{
					pData[nNewDataIndex].fXVal = fGrid;
					pData[nNewDataIndex].fYVal = m_Values.GetAt(idx)[nValueCount-1].fYVal;
					nNewDataIndex++;
					fGrid = fGrid+m_fExportGridUnit;
				}
			}
			
			for(didx=0;didx < nValueCount;didx++)
			{
				TRACE(_T("2:%f\n"),m_Values.GetAt(idx)[nValueCount-1].fXVal);	
				if(fGrid<m_Values.GetAt(idx)[nValueCount-1].fXVal)
				{
					fGrid=fGrid+m_fExportGridUnit;
					continue;
				}

				//Unit Grid 앞뒤의 Random Grid를 이용하여 기울기를 구한다. 
				TRACE(_T("3:%f\n"),m_Values.GetAt(idx)[nValueCount-didx].fXVal);	
				if(fGrid < m_Values.GetAt(idx)[nValueCount-didx].fXVal && (nValueCount-didx) != nValueCount)
				{
					float fGrid_1, fGrid_2, fData_1, fData_2;
					float fSlope, fNewData;
					
					fGrid_1 = m_Values.GetAt(idx)[(nValueCount-didx)+1].fXVal;
					fGrid_2 = m_Values.GetAt(idx)[nValueCount-didx].fXVal;
					fData_1 = m_Values.GetAt(idx)[(nValueCount-didx)+1].fYVal;
					fData_2 = m_Values.GetAt(idx)[nValueCount-didx].fYVal;
					
					//기울기
					fSlope = (fData_2 - fData_1) / (fGrid_2 - fGrid_1);
				
					//New Grid의 Data
					fNewData = (fSlope * (fGrid - fGrid_1)) + fData_1;
					
					pData[nNewDataIndex].fXVal =  fGrid;
					pData[nNewDataIndex].fYVal =  fNewData;
					
					fGrid = fGrid + m_fExportGridUnit;
					nNewDataIndex++;
				}
			}
			
			TRACE(_T("Last Grid: %f\n"),m_Distance);//m_Values.GetAt(idx)[didx].fXVal);
			//뒤쪽 Data 채움
			//if(fGrid < m_Values.GetAt(idx)[didx].fXVal && didx == nValueCount)m_Distance
			if(fGrid <= m_Distance && didx == nValueCount)
			{
				//for(int i=0;fGrid < m_Values.GetAt(idx)[didx].fXVal;i++)
				for(int i=0;fGrid <= m_Distance;i++)
				{
					pData[nNewDataIndex].fXVal	= fGrid;
					TRACE(_T("data:%f\n"),m_Values.GetAt(idx)[didx-1].fYVal);
					TRACE(_T("data:%f\n"),m_Values.GetAt(idx)[0].fYVal);
					//pData[nNewDataIndex].fYVal	= m_Values.GetAt(idx)[didx-1].fYVal;
					pData[nNewDataIndex].fYVal	= m_Values.GetAt(idx)[0].fYVal;
					fGrid = fGrid + m_fExportGridUnit;
					nNewDataIndex++;
				}
			}

			//NewData ValiueCount
			m_GridChangeValues.Add(pData);
			m_GridChangeValueCount.Add(nNewDataIndex);
		}
	}
	else
	{
		AfxMessageBox(_T("date is empty"));
		return false;
	}
	
	return true;
}

void C1DDlg::SetDistance(float distance)
{
	m_Distance = distance;
}

void C1DDlg::AddDataCurve(CString szLabel, TDataPoint* pData,int nDataCount)
{
	TDataPoint* pValueData = new TDataPoint[nDataCount];

	for(int i=0;i<nDataCount;i++)
	{
		pValueData[i].fXVal = pData[i].fXVal;
		pValueData[i].fYVal = pData[i].fYVal;
	}

	m_Values.Add(pValueData);
	m_ValueCount.Add(nDataCount);
	m_Labels.Add(szLabel);
	
}

void C1DDlg::SetRedrawGraph()
{
	m_Graph.ResetAll();
	
	SetWindowText(m_Title);

	//m_Graph.CreateEx(WS_EX_CLIENTEDGE,_T("XGraph"),_T(""), WS_CHILD  | WS_VISIBLE | SS_SUNKEN, CRect(0,0,0,0), this, IDC_1DPLOT);
	//m_Graph.SetCursorFlags(XGC_LEGEND | XGC_VERT | XGC_ADJUSTSMOOTH);
	//m_Graph.SetDlgCtrlID(IDC_1DPLOT);
	m_Graph.SetLogScale(m_IsLog);
				
	// Force cursor to snap to the first curve
	m_Graph.ForceSnap(0);
	m_Graph.SetDoubleBuffer(TRUE); 

	m_Graph.SetGraphMargins(10,10,10,10);
	m_xFormat = _T("%g");
	m_yFormat = _T("%g");

	int i = 0;
	INT_PTR Count = GetCount();
	for(i = 0; i < Count; i++)
	{
		m_Graph.SetCurveData(GetDataPoint(i),GetValueCount(i),	i,	0,	0);	
		m_Graph.GetCurve(i).SetLabel(GetLabel(i));
		m_Graph.GetCurve(i).SetShowMarker(false);
		m_Graph.GetCurve(i).SetMarker(i%8);
		m_Graph.GetCurve(i).SetMarkerType(1);
		m_Graph.GetCurve(i).SetLineSize(2);
	}

	int XCount = m_Graph.GetXAxisCount();
	int YCount = m_Graph.GetYAxisCount();

	double	fMaxValue=0, fMinValue = 0, fSubValue = 0;

	for(i = 0; i < XCount; i++)
	{
		CXGraphAxis& Axis = m_Graph.GetXAxis(i);
		Axis.SetLabel(m_xName);
		Axis.SetShowMarker(false);
		Axis.SetDisplayFmt(_T(m_xFormat));
		Axis.SetShowGrid(true);

		Axis.SetAutoScale(false);
		fMaxValue = Axis.GetMaxValue();
	}

	for(i = 0; i < YCount; i++)
	{
		m_Graph.GetYAxis(i).SetLabel(m_yName);
		m_Graph.GetYAxis(i).SetShowMarker(false);
		m_Graph.GetYAxis(i).SetDisplayFmt(_T(m_yFormat));
		m_Graph.GetYAxis(i).SetShowGrid(true);
		m_Graph.GetYAxis(i).SetMarkerSize(3);
		m_Graph.GetYAxis(i).SetPlacement(CXGraphAxis::apLeft);

		fMaxValue = m_Graph.GetYAxis(i).GetMaxValue();
		fMinValue = m_Graph.GetYAxis(i).GetMinValue();

		fSubValue = (fMaxValue-fMinValue)*0.05;
		m_Graph.GetYAxis(i).SetRange(m_MinY-fMinValue,fMaxValue);

	}	

	m_Graph.ResetZoom();
}

//----------------------------------------------------------------------
