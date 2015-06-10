#pragma once
#include "TrueColorToolbar.h"
#include <afxtempl.h>
// JDlgToolBar

class JDlgToolBar : public CTrueColorToolBar
{
//	DECLARE_DYNAMIC(JDlgToolBar)

public:
	JDlgToolBar();
	virtual ~JDlgToolBar();

	void EnableButton(int id, BOOL bEnable);
protected:
	CMap <int, int &, BOOL, BOOL &> m_ButtonStates;
	// Generated message map functions
	//{{AFX_MSG(CDlgToolBar)
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


