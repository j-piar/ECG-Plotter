// JDlgToolBar.cpp : implementation file
//

#include "stdafx.h"
#include <afxpriv.h>
#include <afxcmn.h>
#include "JDlgToolBar.h"


// JDlgToolBar

//IMPLEMENT_DYNAMIC(JDlgToolBar, CTrueColorToolBar)

JDlgToolBar::JDlgToolBar()
{

}

JDlgToolBar::~JDlgToolBar()
{
}


BEGIN_MESSAGE_MAP(JDlgToolBar, CTrueColorToolBar)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
END_MESSAGE_MAP()



// JDlgToolBar message handlers

LRESULT JDlgToolBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM) 
{
	if (!m_ButtonStates.GetCount()) {
		if (IsWindowVisible()) {
			CFrameWnd *pParent = (CFrameWnd *)GetParent();
			if (pParent)
				OnUpdateCmdUI(pParent, (BOOL)wParam);
		}
	}

	if (IsWindowVisible()) {
		POSITION pos = m_ButtonStates.GetStartPosition();
		int id;
		BOOL val;

		while (pos != NULL) {
			m_ButtonStates.GetNextAssoc(pos,id,val);
			GetToolBarCtrl().EnableButton(id,val);
		}
	}
		
	return 0L;
}

void JDlgToolBar::EnableButton(int id, BOOL bEnable)
{
	GetToolBarCtrl().EnableButton(id,bEnable);

	if (bEnable) m_ButtonStates.RemoveKey(id);
	else m_ButtonStates.SetAt(id,bEnable);
}
