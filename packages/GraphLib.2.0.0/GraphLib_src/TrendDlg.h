#if !defined(AFX_TRENDDLG_H__E26907DF_8686_47C8_84AA_CB34CA652FF3__INCLUDED_)
#define AFX_TRENDDLG_H__E26907DF_8686_47C8_84AA_CB34CA652FF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrendDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CTrendDlg 

class CTrendDlg : public CDialog
{
// Konstruktion
public:
	CTrendDlg(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CTrendDlg)
	enum { IDD = IDD_TRENDDLG };
	CXTPSpinButtonCtrl	m_Degree;
	CXTPSpinButtonCtrl	m_Spin;

	CXTPEdit m_edtSize;		//  [10/15/2013 ChoiJunHyeok]
	CXTPEdit m_edtDegree;	//  [10/15/2013 ChoiJunHyeok]
	UINT	m_nSize;		//  [10/15/2013 ChoiJunHyeok]
	UINT	m_nDegree;		// Â÷¼ö [10/15/2013 ChoiJunHyeok]
	//}}AFX_DATA


// ?erschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions?erschreibungen
	//{{AFX_VIRTUAL(CTrendDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst?zung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CTrendDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f?t unmittelbar vor der vorhergehenden Zeile zus?zliche Deklarationen ein.

#endif // AFX_TRENDDLG_H__E26907DF_8686_47C8_84AA_CB34CA652FF3__INCLUDED_
