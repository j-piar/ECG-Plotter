#pragma once


#include "SetupCurveCtrl.h"
// CSetupCurvesDlg dialog


class CSetupCurvesDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetupCurvesDlg)

public:
	CSetupCurvesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetupCurvesDlg();

// Dialog Data
	enum { IDD = IDD_SETCURVE41DPLOT };

protected:

	CSetupCurveCtrl	m_ListCtrl;

	CArray<CXGraphDataSerie*,CXGraphDataSerie*>	m_CurveArray;

public:
	CXGraphDataSerie*	GetCurves(int idx){return m_CurveArray.GetAt(idx);};
	void				AddCurve(CXGraphDataSerie* pcurve){m_CurveArray.Add(pcurve);};

protected:


	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClickList(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void OnOK();
};
