#pragma once
#include "resource1.h"

// CXGraphShowOptionDlg ��ȭ �����Դϴ�.

class CXGraphShowOptionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CXGraphShowOptionDlg)

public:
	CXGraphShowOptionDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CXGraphShowOptionDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_XGRAPH_SHOWOPTION };

	enum{ SHOW_MIN, SHOW_MAX, SHOW_AVERAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeXgraphShowSelGroup();
	afx_msg void OnBnClickedXgraphShowMin();
	afx_msg void OnBnClickedXgraphShowMax();
	afx_msg void OnBnClickedXgraphShowAverage();

protected:
	CXTPComboBox m_cboSelectTargetGroup;
	CXTPButton m_chkShowMin;
	CXTPButton m_chkShowMax;
	CXTPButton m_chkShowAverage;
	//CXTPEdit   m_edtRangeStart;
	//CXTPEdit   m_edtRangeEnd;

public:
	UINT m_eShowParam;		// �� �������� [10/24/2013 ChoiJunHyeok]
	UINT m_nGroupCount;		// �� �����ϴ� �׷� ���� [10/24/2013 ChoiJunHyeok]
	UINT m_nSelectGroupNum;	// ���� ������ �׷� ��ȣ [10/24/2013 ChoiJunHyeok]
	CString m_strDialogTitle;	// ���̾�α� Ÿ��Ʋ [10/25/2013 ChoiJunHyeok]

	// üũ�� �ɼ� �Ķ���� [10/24/2013 ChoiJunHyeok]
	BOOL m_bShowMin;
	BOOL m_bShowMax;
	BOOL m_bAverage;

	// group range [10/24/2013 ChoiJunHyeok]
	double m_lfGroupStart;
	double m_lfGroupEnd;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
