#if !defined(AFX_CHARTDLG_H__D0E86027_EA50_43B7_B81C_6187862BABD5__INCLUDED_)
#define AFX_CHARTDLG_H__D0E86027_EA50_43B7_B81C_6187862BABD5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChartDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CChartDlg 

#include "XGraph.h"
//#include "ColourPicker.h"
#include "ColorButton.h"



class CChartDlg : public CDialog
{
// Konstruktion
public:
	CChartDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CChartDlg)
	enum { IDD = IDD_CHART };
	
	CXTPSpinButtonCtrl	m_SpinTol;
	CXTPComboBox	m_cbCurve;
	CXTPComboBox	m_cbYAxis;
	CXTPComboBox	m_cbXAxis;
	CXTPComboBox	m_Alignment;

	CColorButton	m_InnerColor;
	CColorButton	m_CharColor;
	CColorButton	m_BkColor;
	
	CXTPButton		m_chkShowLegend;
	CXTPButton		m_chkDoubleBuffer;
	CXTPButton		m_chkSnapCursor;
	CXTPEdit		m_edtTolerance;

	CXTPButton		m_btnOk;
	CXTPButton		m_btnCancel;
	CXTPButton		m_btnApply;


	//CColourPicker	m_InnerColor;
	//CColourPicker	m_CharColor;
	//CColourPicker	m_BkColor;
	BOOL	m_bShowLegend;
	BOOL	m_bDoubleBuffer;
	int		m_nXAxis;
	int		m_nYAxis;
	int		m_nCurve;
	BOOL	m_bSnapCursor;
	UINT	m_nTolerance;
	//}}AFX_DATA


	CXGraph *m_pGraph;

// ?erschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions?erschreibungen
	//{{AFX_VIRTUAL(CChartDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst?zung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CChartDlg)
	afx_msg void OnChanged();
	afx_msg LRESULT OnColorChanged(WPARAM, LPARAM);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnApply();
	virtual void OnCancel();
	afx_msg void OnSelchangeCbxaxis();
	afx_msg void OnSelchangeCbyaxis();
	afx_msg void OnSelchangeCbcurve();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f?t unmittelbar vor der vorhergehenden Zeile zus?zliche Deklarationen ein.

#endif // AFX_CHARTDLG_H__D0E86027_EA50_43B7_B81C_6187862BABD5__INCLUDED_
