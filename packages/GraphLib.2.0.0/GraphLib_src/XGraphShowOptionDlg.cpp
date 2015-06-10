// XGraphShowOptionDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "XGraphShowOptionDlg.h"
#include "afxdialogex.h"
//#include "resource1.h"


// CXGraphShowOptionDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CXGraphShowOptionDlg, CDialogEx)

CXGraphShowOptionDlg::CXGraphShowOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CXGraphShowOptionDlg::IDD, pParent)
{
	m_eShowParam= 0;
	m_nGroupCount= 0;
	m_nSelectGroupNum= 0;
}

CXGraphShowOptionDlg::~CXGraphShowOptionDlg()
{
}

void CXGraphShowOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_XGRAPH_SHOW_SEL_GROUP, m_cboSelectTargetGroup);
	DDX_Control(pDX, IDC_XGRAPH_SHOW_MIN, m_chkShowMin);
	DDX_Control(pDX, IDC_XGRAPH_SHOW_MAX, m_chkShowMax);
	DDX_Control(pDX, IDC_XGRAPH_SHOW_AVERAGE, m_chkShowAverage);
	//DDX_Control(pDX, IDC_SHOW_GROUP_RANGE_START, m_edtRangeStart);
	//DDX_Control(pDX, IDC_SHOW_GROUP_RANGE_END, m_edtRangeEnd);
	

}


BEGIN_MESSAGE_MAP(CXGraphShowOptionDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_XGRAPH_SHOW_SEL_GROUP, &CXGraphShowOptionDlg::OnCbnSelchangeXgraphShowSelGroup)
	ON_BN_CLICKED(IDC_XGRAPH_SHOW_MIN, &CXGraphShowOptionDlg::OnBnClickedXgraphShowMin)
	ON_BN_CLICKED(IDC_XGRAPH_SHOW_MAX, &CXGraphShowOptionDlg::OnBnClickedXgraphShowMax)
	ON_BN_CLICKED(IDC_XGRAPH_SHOW_AVERAGE, &CXGraphShowOptionDlg::OnBnClickedXgraphShowAverage)
	ON_BN_CLICKED(IDOK, &CXGraphShowOptionDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CXGraphShowOptionDlg 메시지 처리기입니다.


void CXGraphShowOptionDlg::OnCbnSelchangeXgraphShowSelGroup()
{
	
}


void CXGraphShowOptionDlg::OnBnClickedXgraphShowMin()
{
}


void CXGraphShowOptionDlg::OnBnClickedXgraphShowMax()
{
}


void CXGraphShowOptionDlg::OnBnClickedXgraphShowAverage()
{
}


BOOL CXGraphShowOptionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if(m_eShowParam==SHOW_MIN)
	{
		m_chkShowMin.SetChecked(TRUE);
		m_chkShowMax.SetChecked(FALSE);
		m_chkShowAverage.SetChecked(FALSE);
	}
	else if(m_eShowParam==SHOW_MAX)
	{
		m_chkShowMax.SetChecked(TRUE);
		m_chkShowMin.SetChecked(FALSE);
		m_chkShowAverage.SetChecked(FALSE);
	}
	else if(m_eShowParam==SHOW_AVERAGE)
	{
		m_chkShowMin.SetChecked(FALSE);
		m_chkShowMax.SetChecked(FALSE);
		m_chkShowAverage.SetChecked(TRUE);
	}
	
	m_cboSelectTargetGroup.ResetContent();
	for (size_t i=0; i<m_nGroupCount; ++i)
	{
		CString strBuf;
		strBuf.Format("Group-%d", i+1);
		m_cboSelectTargetGroup.AddString(strBuf);
	}

	if(m_nGroupCount != 0)
	{
		m_cboSelectTargetGroup.SetCurSel(0);

		//CString m_strStart; m_strStart.Format("%lf", m_lfGroupStart);
		//CString m_strEnd; m_strEnd.Format("%lf", m_lfGroupEnd);

		//m_edtRangeStart.SetWindowTextA(m_strStart);
		//m_edtRangeEnd.SetWindowTextA(m_strEnd);
	}

	SetWindowTextA(m_strDialogTitle);	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CXGraphShowOptionDlg::OnBnClickedOk()
{
	m_nSelectGroupNum= m_cboSelectTargetGroup.GetCurSel() + 1;

	m_bShowMin= m_chkShowMin.GetChecked();
	m_bShowMax= m_chkShowMax.GetChecked();
	m_bAverage= m_chkShowAverage.GetChecked();
	
	CDialogEx::OnOK();
}

