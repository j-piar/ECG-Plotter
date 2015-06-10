// GraphTrendLineToolDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "GraphTrendLineToolDlg.h"
#include "afxdialogex.h"
#include "../XGraph/XGRAPH.h"
#include "../XGraph/XGraphAxis.h"
#include "../XGraph/XGraphAxisMaker.h"

// CGraphTrendLineToolDlg 대화 상자입니다.


IMPLEMENT_DYNAMIC(CGraphTrendLineToolDlg, CDialogEx)

CGraphTrendLineToolDlg::CGraphTrendLineToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGraphTrendLineToolDlg::IDD, pParent)
{
	m_nGroupCount= 0;

	m_pGraph = NULL;
}

CGraphTrendLineToolDlg::~CGraphTrendLineToolDlg()
{
}

void CGraphTrendLineToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CBO_SO_RANGE,		m_cboRange);
	DDX_Control(pDX, IDC_CBO_SO_MODE,		m_cboMode);

	DDX_Control(pDX, IDC_EDT_SO_FREQ,		m_edtFreq);
	DDX_Control(pDX, IDC_EDT_SO_DAMPING,	m_edtDamp);
	DDX_Control(pDX, IDC_EDT_SO_DISPLACE,	m_edtDisplace);
	DDX_Control(pDX, IDC_EDT_SO_VEL	,		m_edtVelo);

	DDX_Control(pDX, IDOK,					m_btnApply);
	DDX_Control(pDX, IDCANCEL	,			m_btnClose);
}


BEGIN_MESSAGE_MAP(CGraphTrendLineToolDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CGraphTrendLineToolDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CGraphTrendLineToolDlg::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_CBO_SO_RANGE, &CGraphTrendLineToolDlg::OnCbnSelchangeCboSoRange)
	ON_CBN_SELCHANGE(IDC_CBO_SO_MODE, &CGraphTrendLineToolDlg::OnCbnSelchangeCboSoMode)
END_MESSAGE_MAP()


// CGraphTrendLineToolDlg 메시지 처리기입니다.


void CGraphTrendLineToolDlg::OnBnClickedOk()
{
	OnCbnSelchangeCboSoMode();


	CString cPoly = "f(x)=";

	for (int i = 0; i < m_PS.m_nGlobalO; i++)
	{
		if (m_PS.m_fC[i] == 0.0)
			continue;

		CString cCoeff;
		cCoeff.Format("%5.4lf*X^%d", m_PS.m_fC[i], i);

		if(cCoeff.Left(6) == "0.0000"  ||  cCoeff.Left(7) == "-0.0000")
			cCoeff.Format("%g*X^%d", m_PS.m_fC[i], i);


		if (i < (m_PS.m_nGlobalO-1))
			cCoeff += "+\n";
		cPoly += cCoeff;
	}

	CDC *pDC = m_pGraph->GetDC();
	CRect rect(200,200,210,210);
	pDC->DrawText(cPoly, rect, DT_LEFT | DT_CALCRECT);
	
	m_pGraph->ReleaseDC (pDC);
	m_pGraph->InsertLabel (rect, cPoly);
	m_pGraph->Invalidate();

	CDialogEx::OnOK();
}


void CGraphTrendLineToolDlg::OnBnClickedCancel()
{
	int nCount = m_pGraph->GetCurveCount();

	m_pGraph->DeleteCurve(nCount-1);
	m_pGraph->Invalidate();
	
	CDialogEx::OnCancel();
}


void CGraphTrendLineToolDlg::OnOK()
{
	
}


BOOL CGraphTrendLineToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	RefreshGroupCount();


	m_cboMode.ResetContent();
	m_cboMode.AddString("Polynomial 1st");
	m_cboMode.AddString("Polynomial 2nd");
	m_cboMode.AddString("Polynomial 3rd");
	m_cboMode.AddString("Polynomial 4th");
	m_cboMode.AddString("Polynomial 5th");
	m_cboMode.SetCurSel(0);


	m_cboRange.SetTheme(xtpControlThemeOfficeXP);
	m_cboMode.SetTheme(xtpControlThemeOfficeXP);
	m_edtFreq.SetTheme(xtpControlThemeOfficeXP);
	m_edtDamp.SetTheme(xtpControlThemeOfficeXP);
	m_edtDisplace.SetTheme(xtpControlThemeOfficeXP);
	m_edtVelo.SetTheme(xtpControlThemeOfficeXP);
	m_btnApply.SetTheme(xtpControlThemeOfficeXP);
	m_btnClose.SetTheme(xtpControlThemeOfficeXP);

	m_edtFreq.EnableWindow(FALSE);
	m_edtDamp.EnableWindow(FALSE);
	m_edtDisplace.EnableWindow(FALSE);
	m_edtVelo.EnableWindow(FALSE);

	m_edtFreq.SetWindowText(_T("0.245"));
	m_edtDamp.SetWindowText(_T("0.24"));
	m_edtDisplace.SetWindowText(_T("0.0"));
	m_edtVelo.SetWindowText(_T("0.0"));

	m_iMaxDataIdx = m_pGraph->GetCurveCount();

	OnCbnSelchangeCboSoRange();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CGraphTrendLineToolDlg::OnCbnSelchangeCboSoRange()
{
	int iSelPos = m_cboRange.GetCurSel();

	m_vecPoints.clear();
		

	for(int i = 0; i < m_iMaxDataIdx; i++ )
	{
		CXGraphDataSerie& pSerie = m_pGraph->GetCurve(i);

		vector<TDataPoint> Points = pSerie.GetSortData();
		m_vecPoints.insert(m_vecPoints.end(), Points.begin(), Points.end());
	}
	
	std::sort(m_vecPoints.begin(), m_vecPoints.end(), [](TDataPoint a, TDataPoint b)
	{
		return a.fXVal < b.fXVal;
	});

	vector<CXGraphAxisMarker> vecMarkers = m_pGraph->GetXAxis(0).GetAxisMarkers();

	double fTagStart = 1e10, fTagEnd = -1e10;

	for(size_t i = 0; i < vecMarkers.size(); i++)
	{
		if(vecMarkers[i].m_nSliceNumber == iSelPos + 1)
		{
			if(vecMarkers[i].m_eLineType == CXGraphAxis::LINE_SLICE_START) fTagStart = vecMarkers[i].m_lfXValue;
			if(vecMarkers[i].m_eLineType == CXGraphAxis::LINE_SLICE_END) fTagEnd = vecMarkers[i].m_lfXValue;
		}
	}

	if(fTagStart >= fTagEnd) return;

	vector<TDataPoint>::iterator iterStart, iterEnd;

	for(size_t i = 0; i < m_vecPoints.size(); i++)
	{
		if(m_vecPoints[i].fXVal >= fTagStart)
		{
			if( i == 0 ) break;

			vector<TDataPoint>::iterator iter = m_vecPoints.begin() + (i-1);
			
			m_vecPoints.erase(m_vecPoints.begin(), iter);

			break;
		}
	}

	for(size_t i = m_vecPoints.size()-1; i >= 0; i--)
	{
		if(m_vecPoints[i].fXVal <= fTagEnd)
		{
			if( i == m_vecPoints.size()-1 ) break;

			vector<TDataPoint>::iterator iter = m_vecPoints.begin() + (i+1);

			m_vecPoints.erase(iter, m_vecPoints.end());

			break;
		}
	}

	MakeTrendMain(fTagStart, fTagEnd);	
}

void	CGraphTrendLineToolDlg::MakeTrendMain(double fTimeStart, double fTimeEnd)
{
	int iOrderSel = m_cboMode.GetCurSel();

	m_fTimeStart = fTimeStart;
	m_fTimeEnd = fTimeEnd;
		
	switch(iOrderSel)
	{
	case 0:case 1:case 2:case 3:case 4:
		MakeTrendPoly(iOrderSel+2);;
		break;
	case 5:
		MakeTrend2ndOrder();
		break;
	default:
		break;
	}
}

void	CGraphTrendLineToolDlg::MakeTrendPoly(int iDec)
{
	if(m_pGraph->GetCurveCount() != m_iMaxDataIdx)	// 삭제 후
	{
		for(int i = m_pGraph->GetCurveCount()-1; i >= m_iMaxDataIdx; i--)
		{
			m_pGraph->DeleteCurve(i);
		}
	}

	size_t nCount = m_vecPoints.size();

	TDataPoint* pData = new TDataPoint[nCount];
	TDataPoint* pSrcData = new TDataPoint[nCount];

	std::copy(m_vecPoints.begin(), m_vecPoints.end(), pSrcData);

	m_PS.m_nGlobalO = iDec;

	if (!m_PS.Polyfit (nCount, iDec, pSrcData))
	{
		delete pData;
		delete pSrcData;
		return;
	}

	double fDiff = (m_fTimeEnd - m_fTimeStart) / (double)(nCount - 1);

	for (size_t i = 0; i < nCount; i ++)
	{
		pData[i].fXVal = m_fTimeStart + (double)i * fDiff;

		pData[i].fYVal = m_PS.GetValue (pData[i].fXVal);
	}

	delete pSrcData;

	int	nCurveCount = m_pGraph->GetCurveCount();
	CXGraphDataSerie &ret = m_pGraph->SetCurveData (pData, nCount, nCurveCount, m_pGraph->GetCurve(0).GetXAxis(), m_pGraph->GetCurve(0).GetYAxis(), true);
	m_pGraph->GetCurve(nCurveCount).SetType(CXGraphDataSerie::gtLine);

	CString strName; strName.Format("Group%d_Poly%d",m_cboRange.GetCurSel()+1, iDec-1);
	m_pGraph->GetCurve(nCurveCount).SetLabel(strName);


}

void	CGraphTrendLineToolDlg::MakeTrend2ndOrder()
{
	if(m_pGraph->GetCurveCount() != m_iMaxDataIdx)
	{
		for(int i = m_pGraph->GetCurveCount()-1; i >= m_iMaxDataIdx; i--)
		{
			m_pGraph->DeleteCurve(i);
		}
	}

	CString szData;

	double fFreq, fDamping, fInitDisp, fInitVel;

	m_edtFreq.GetWindowText(szData);
	fFreq = atof(szData.GetBuffer(0));

	m_edtDamp.GetWindowText(szData);
	fDamping = atof(szData.GetBuffer(0));

	m_edtDisplace.GetWindowText(szData);
	fInitDisp = atof(szData.GetBuffer(0));

	m_edtVelo.GetWindowText(szData);
	fInitVel = atof(szData.GetBuffer(0));

	size_t nCount = m_vecPoints.size();

	TDataPoint* pSrcData = new TDataPoint[nCount];

	vector<double> vecX, vecY;

	for(size_t i = 0; i < nCount; i++)
	{
		vecX.push_back(pSrcData[i].fXVal);
		vecY.push_back(pSrcData[i].fYVal);
	}

	diff_t1 difft1;
	diff_t2 difft2;
	difft2.fFreq = fFreq;
	difft2.fDamping = fDamping;

	vector<diff_function*> function;
	function.push_back(&difft1);
	function.push_back(&difft2);

	vector<double> initial;
	initial.push_back(pSrcData[0].fXVal);
	initial.push_back(pSrcData[0].fYVal);


	vector<vector<double> > result;

	ode_solve solver;
	solver.solve(pSrcData[0].fXVal, pSrcData[nCount-1].fXVal, nCount, vecY, &function, &result);

	for(size_t i = 0; i < nCount; i++)
	{
		pSrcData[i].fYVal = result[1][i];
	}
	

	/*TDataPoint* pSrcData1 = new TDataPoint[nCount-1];
	TDataPoint* pSrcData2 = new TDataPoint[nCount-3];
	TDataPoint* pData = new TDataPoint[nCount-3];
	
	std::copy(m_vecPoints.begin(), m_vecPoints.end(), pSrcData);

	double fPi = 3.141592;

	for(size_t i = 0; i < nCount-1; i++)
	{
		double x[3], y[3];

		if(i == 0)
		{
			x[0] = fInitDisp;
			y[0] = fInitVel;
		}
		else
		{
			x[0] = pSrcData[i-1].fXVal;
			y[0] = pSrcData[i-1].fYVal;
		}

		x[1] = pSrcData[i].fXVal;
		y[1] = pSrcData[i].fYVal;
		x[2] = pSrcData[i+1].fXVal;
		y[2] = pSrcData[i+1].fYVal;

		pSrcData1[i].fXVal = x[1];
		pSrcData1[i].fYVal = ((y[1]-y[0])/(x[1]-x[0]) + (y[2]-y[1])/(x[2]-x[1])) * 0.5;
	}

	for(size_t i = 1; i < nCount-2; i++)
	{
		double x[3] = {pSrcData1[i-1].fXVal, pSrcData1[i].fXVal, pSrcData1[i+1].fXVal};
		double y[3] = {pSrcData1[i-1].fYVal, pSrcData1[i].fYVal, pSrcData1[i+1].fYVal};


		pSrcData2[i-1].fXVal = pSrcData1[i].fXVal;
		pSrcData2[i-1].fYVal = ((y[1]-y[0])/(x[1]-x[0]) + (y[2]-y[1])/(x[2]-x[1])) * 0.5;
	}

	for(size_t i = 0; i < nCount-3; i++)
	{
		pData[i].fXVal = pSrcData[i+1].fXVal;
		pData[i].fYVal = -4*fPi*fPi*fFreq*fFreq*pSrcData1[i+1].fYVal - 4*fPi*fDamping*fFreq*pSrcData2[i].fYVal;
	}

	delete pSrcData;

	int	nCurveCount = m_pGraph->GetCurveCount();
	CXGraphDataSerie &ret = m_pGraph->SetCurveData (pSrcData, nCount, nCurveCount, m_pGraph->GetCurve(0).GetXAxis(), m_pGraph->GetCurve(0).GetYAxis(), true);
	m_pGraph->GetCurve(nCurveCount).SetType(CXGraphDataSerie::gtLine);

	CString strName; strName.Format("Group%d_2ndOrder",m_cboRange.GetCurSel()+1);
	m_pGraph->GetCurve(nCurveCount).SetLabel(strName);

	m_pGraph->Invalidate();*/


	

	
}


void CGraphTrendLineToolDlg::OnCbnSelchangeCboSoMode()
{
	UINT nMode= m_cboMode.GetCurSel();

	if(nMode == 5 )
	{
		m_edtFreq.EnableWindow(TRUE);
		m_edtDamp.EnableWindow(TRUE);
		m_edtDisplace.EnableWindow(TRUE);
		m_edtVelo.EnableWindow(TRUE);

		MakeTrend2ndOrder();
	}
	else
	{
		m_edtFreq.EnableWindow(FALSE);
		m_edtDamp.EnableWindow(FALSE);
		m_edtDisplace.EnableWindow(FALSE);
		m_edtVelo.EnableWindow(FALSE);

		MakeTrendPoly(nMode+2);		
	}
	
}

void CGraphTrendLineToolDlg::RefreshGroupCount()
{
	m_cboRange.ResetContent();
	for (size_t i=0; i<m_nGroupCount; ++i)
	{
		CString strBuf; strBuf.Format("Group-%d", i);
		m_cboRange.AddString(strBuf);
	}
	m_cboRange.SetCurSel(0);
}


bool ode_solve::solve(
	double start_point,
	double end_point,
	int point_number,
	vector<double> initial,
	vector<diff_function*> *function,
	vector<vector<double> > *result)
{
	int p = 0;

	int m = function->size(); // number of equations

	// Set h = (b-a)/N;
	double h = (end_point - start_point) / point_number; //(b - a) / N;
	// t = a;
	double t = start_point; //a;

	// For j = 1, 2, ..., m set wj = aj
	vector<double> w(initial.begin(), initial.end());

	vector<double> k_1(m,0);
	vector<double> k_2(m,0);
	vector<double> k_3(m,0);
	vector<double> k_4(m,0);


	vector<double> w1(m);
	vector<double> w2(m);
	vector<double> w3(m);

	// For i = 1, 2, ... N do steps 5 - 11
	for(int i = 1; i <= point_number; i++){
		if(preprocessing(t, &initial, &w) == false){
			_termination_code = -1;
			break;
		}
		// Step 5 - For j = 1, 2, ... , m set
		// k_1,j = hf_j(t,w_1, ..., w_m).
		int j = 0;
		for(int j = 1; j<=m; j++){
			k_1[j-1] = h* (*(function->operator [](j-1)))(t, &w);
		}


		// Step 6 - For j = 1, 2, ... , m set
		// k_2,j = hf_j(t+h/2, w1+k_11/2, w2 + k_12/2, ... , wm + k_1m/2);
		for(p = 0; p<m; p++){
			w1[p] = w[p] + k_1[p] / 2;       
		}
		for(j = 1; j<=m; j++){
			k_2[j-1] = h* (*(function->operator [](j-1)))(t + h/2, &w1);
		}

		// Step 7 - For j = 1, 2, ... , m set
		// k_3,j = hf_j(t+h/2, w1+k_21/2, w2 + k_22/2, ... , wm + k_2m/2);
		for(p = 0; p<m; p++) w2[p] = w[p] + k_2[p]/2;
		for(j = 1; j<=m; j++){
			k_3[j-1] = h* (*(function->operator [](j-1)))(t + h/2, &w2);
		}

		// Step 8 - For j = 1, 2, ... , m set
		// k_4,j = hf_j(t+h/2, w1+k_31/2, w2 + k_32/2, ... , wm + k_3m/2);
		for(p = 0; p<m; p++) w3[p] = w[p] + k_3[p]/2;
		for(j = 1; j<=m; j++){
			k_4[j-1] = h* (*(function->operator [](j-1)))(t + h/2, &w3);
		}

		// Step 9 - For j = 1, 2, ... , m set
		// w[j] = w[j] + (k_1[j] + 2k_2[j] + 2k_3[j] + k_4[j])/6;
		for(j = 1; j<=m; j++){
			w[j-1] = w[j-1] + ( k_1[j-1] + 2*k_2[j-1] + 2*k_3[j-1] + k_4[j-1])/6;
		}

		// Step 10 - Set t = a + ih;
		t = start_point + i*h;


		//In cases wi are concentration, set them to 0. 
		for(p = 0; p<m; p++){
			if(w[p] < 0.0000005) w[p] = 0;
		}

		// CALLBACK function.
		// Process appropriate works at each step.
		if(postprocessing(t, &initial, &w) == false){
			_termination_code = 1;
			break;
		}

		// Step 11 - OUTPUT(t,w1, w2, ... , wm);
		result->push_back(w);
	}

	_termination_code = 0;


	return true;
}

bool ode_solve::preprocessing(double time, std::vector<double>* initial, std::vector<double> *approximate)
{
	return true;
}

bool ode_solve::postprocessing(double time, std::vector<double>* initial, std::vector<double> *approximate)
{
	return true;
}