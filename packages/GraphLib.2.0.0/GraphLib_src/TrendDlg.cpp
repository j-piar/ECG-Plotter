#include "stdafx.h"
#include "XGraph.h"
#include "TrendDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTrendDlg 


CTrendDlg::CTrendDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTrendDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrendDlg)
	m_nSize = 0;
	m_nDegree = 5;
	//}}AFX_DATA_INIT
}


void CTrendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrendDlg)
	DDX_Control(pDX, IDC_EDT_TREND_SIZE, m_edtSize);
	DDX_Control(pDX, IDC_EDT_TREND_DEGREE, m_edtDegree);

	DDX_Control(pDX, IDC_SPIN2, m_Degree);
	DDX_Control(pDX, IDC_SPIN1, m_Spin);
	DDX_Text(pDX, IDC_EDT_TREND_SIZE, m_nSize);
	DDV_MinMaxUInt(pDX, m_nSize, 0, 100000000);
	DDX_Text(pDX, IDC_EDT_TREND_DEGREE, m_nDegree);
	DDV_MinMaxUInt(pDX, m_nDegree, 2, 20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTrendDlg, CDialog)
	//{{AFX_MSG_MAP(CTrendDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen f? Nachrichten CTrendDlg 

BOOL CTrendDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_edtDegree.SetTheme(xtpControlThemeOfficeXP);
	m_edtSize.SetTheme(xtpControlThemeOfficeXP);

	m_Spin.SetRange32(0,100000000);
	m_Degree.SetRange(2,20);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zur?kgeben
}
