#if !defined(AFX_CURVEDLG_H__98B20941_D604_4120_8561_3902741205F0__INCLUDED_)
#define AFX_CURVEDLG_H__98B20941_D604_4120_8561_3902741205F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CurveDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CCurveDlg 

#include "XGraphDataSerie.h"
#include "ColourPicker.h"
#include "ColorButton.h"

class CCurveDlg : public CDialog
{
// Konstruktion
public:
	CCurveDlg(CWnd* pParent = NULL);   // Standardkonstruktor

	void PresetValues();
// Dialogfelddaten
	//{{AFX_DATA(CCurveDlg)
	enum { IDD = IDD_CURVE };

	CXTPComboBox		m_cbFillCurve;
	CXTPSpinButtonCtrl	m_SpinLineSize;
	CXTPSpinButtonCtrl	m_SpinMarkerSize;
	CXTPComboBox		m_cbMarker;
	CColorButton		m_Color;

	CXTPButton			m_btnOk;
	CXTPButton			m_btnCancel;
	CXTPButton			m_btnApply;

	CXTPComboBox		m_cboCurveType;
	CXTPEdit			m_edtLineSize;
	CXTPComboBox		m_cboCurveStyle;
	CXTPButton			m_chkShowMaker;
	CXTPButton			m_chkCurveVisible;
	CXTPButton			m_chkFillBeneath;
	CXTPButton			m_rdoFillStyle;
	CXTPButton			m_chkFillTransparent;
	CXTPEdit			m_edtLabel;
	CXTPButton			m_rdoMarker;
	CXTPEdit			m_edtMarkerSize;
	CXTPButton			m_chkFillCurve;

	CXTPButton			m_btnMavg;
	CXTPButton			m_btnLineTrend;
	CXTPButton			m_btnCurveTrend;
	CXTPButton			m_btnPolyTrend;
	CXTPButton			m_btnSecondOrderSys;


		
	int				m_nType;
	UINT			m_nLineSize;
	int				m_nStyle;
	BOOL			m_bShowMarker;
	BOOL			m_bVisible;
	BOOL			m_bFillBeneath;
	int				m_nFillStyle;
	BOOL			m_bFillTransparent;
	CString			m_cLabel;
	int				m_nMarkerType;
	int				m_nMarker;
	UINT			m_nMarkerSize;
	int				m_nFillCurve;
	//}}AFX_DATA


	CXGraphDataSerie* m_pSerie;

// ?erschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions?erschreibungen
	//{{AFX_VIRTUAL(CCurveDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst?zung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CCurveDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnApply();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnChanged();
	afx_msg LRESULT OnColorChanged(WPARAM, LPARAM);
	virtual void OnOK();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnMavg();
	afx_msg void OnLintrend();
	afx_msg void OnCubtrend();
	afx_msg void OnPolytrend();
	virtual void OnCancel();
	afx_msg void OnSelchangeCbfillcurve();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSecondOrderSys();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f?t unmittelbar vor der vorhergehenden Zeile zus?zliche Deklarationen ein.

#endif // AFX_CURVEDLG_H__98B20941_D604_4120_8561_3902741205F0__INCLUDED_
