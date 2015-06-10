#include "stdafx.h"
#include "SetupCurvesDlg.h"


// CSetupCurvesDlg dialog

IMPLEMENT_DYNAMIC(CSetupCurvesDlg, CDialog)

CSetupCurvesDlg::CSetupCurvesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupCurvesDlg::IDD, pParent)
{

}

CSetupCurvesDlg::~CSetupCurvesDlg()
{
	m_CurveArray.RemoveAll();
}

void CSetupCurvesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_CURVECTRL,m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CSetupCurvesDlg, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_CURVECTRL, &CSetupCurvesDlg::OnClickList)
END_MESSAGE_MAP()


// CSetupCurvesDlg message handlers

BOOL CSetupCurvesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ListCtrl.Initialize(m_CurveArray);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



void CSetupCurvesDlg::OnClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CSetupCurvesDlg::OnOK()
{
	CurveInfo info;
	CXGraphDataSerie* pData = NULL;
	UINT i = 0, Count = m_ListCtrl.GetCount();
	for(i = 0; i < Count; i++)
	{
		info = m_ListCtrl.GetAt(i);
		pData = m_CurveArray.GetAt(i);
		pData->SetLabel(info.name);
		pData->SetColor(info.color);
		pData->SetVisible(info.IsShow);
		pData->SetLineStyle(info.line);
		pData->SetLineSize(info.thickness);
	}
	CDialog::OnOK();
}
