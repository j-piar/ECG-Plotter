#pragma once
#include "resource1.h"

// CXGraphShowOptionDlg 대화 상자입니다.

class CXGraphShowOptionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CXGraphShowOptionDlg)

public:
	CXGraphShowOptionDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CXGraphShowOptionDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_XGRAPH_SHOWOPTION };

	enum{ SHOW_MIN, SHOW_MAX, SHOW_AVERAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
	UINT m_eShowParam;		// 뭘 보여줄지 [10/24/2013 ChoiJunHyeok]
	UINT m_nGroupCount;		// 총 존재하는 그룹 개수 [10/24/2013 ChoiJunHyeok]
	UINT m_nSelectGroupNum;	// 현재 선택한 그룹 번호 [10/24/2013 ChoiJunHyeok]
	CString m_strDialogTitle;	// 다이어로그 타이틀 [10/25/2013 ChoiJunHyeok]

	// 체크한 옵션 파라매터 [10/24/2013 ChoiJunHyeok]
	BOOL m_bShowMin;
	BOOL m_bShowMax;
	BOOL m_bAverage;

	// group range [10/24/2013 ChoiJunHyeok]
	double m_lfGroupStart;
	double m_lfGroupEnd;

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
