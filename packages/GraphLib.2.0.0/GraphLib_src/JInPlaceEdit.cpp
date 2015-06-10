// InPlaceEdit.cpp : implementation file
//

#include "stdafx.h"
#include "JInPlaceEdit.h"
#include <locale.h>
#include <math.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define EPS 1.0e-9

/////////////////////////////////////////////////////////////////////////////
// CNumSpinCtrl
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CNumSpinCtrl, CSpinButtonCtrl)	
	ON_NOTIFY_REFLECT_EX(UDN_DELTAPOS, OnDeltapos)
//	ON_WM_CREATE()	
END_MESSAGE_MAP()

CNumSpinCtrl::CNumSpinCtrl()
{
	m_Delta = 1;
	m_MinVal = 0;
	m_MaxVal = 100;
	m_IntRange = 100;

	lconv* pLconv = localeconv ();
	m_DecSymbol = *pLconv->decimal_point;
	m_bTrimTrailingZeros = TRUE;
	SetDecimalPlaces (-1); // simple formatting through "%g"
	SetFormatString (NULL);
}

CNumSpinCtrl::~CNumSpinCtrl()
{
}


void CNumSpinCtrl::SetValueForBuddy (double val)
{
	CWnd* pEdit = GetBuddy();
	if (pEdit)
	{
		CString str;
		if (m_strFormat.IsEmpty ())
			str = FormatValue (val);
		else
			str.Format (m_strFormat, val);
		
		pEdit->SetWindowText (str);
	}
}

void CNumSpinCtrl::SetPos(double val)
{
	SetValueForBuddy (val);
	SetIntPos (val);
}

double CNumSpinCtrl::GetPos()
{
	CWnd* pEdit = GetBuddy();
	if (pEdit)
	{
		CString str;
		pEdit->GetWindowText (str);
		double val = atof (str);
		return val;
	}

	return 0.0;
}

void CNumSpinCtrl::SetRangeAndDelta(double lower, double upper, double delta)
{
	m_MinVal = lower;
	m_MaxVal = upper;
	m_Delta = delta;

	ASSERT (m_MaxVal > m_MinVal); // reversed min/max is not implemented, although it's probably easy

	//// avoid division by zero
	if (m_Delta == 0.0)
		return;

	/// Figure out the integer range to use, so that acceleration can work properly
	double range = fabs ((m_MaxVal - m_MinVal) / m_Delta);
	if (range > (double)UD_MAXVAL)
		m_IntRange = UD_MAXVAL;
	else
		m_IntRange = (int) range;
	CSpinButtonCtrl::SetRange32 (0, m_IntRange);

	/// Set integer position
	SetIntPos (GetPos());
}

void CNumSpinCtrl::SetIntPos (double pos)
{
	//// avoid division by zero
	if (m_MaxVal == m_MinVal)
		return;

	int int_pos;
	
	if (pos < m_MinVal)
		int_pos = 0;
	else if (pos > m_MaxVal)
		int_pos = m_IntRange;
	else
	{
		// where in the range is current position?
		double pos_in_range = (pos - m_MinVal) / (m_MaxVal - m_MinVal);
		int_pos = (int)(m_IntRange * pos_in_range + 0.5);
	}
	CSpinButtonCtrl::SetPos (int_pos);
}

void CNumSpinCtrl::GetRangeAndDelta(double& lower, double& upper, double& delta)
{
	lower = m_MinVal;
	upper = m_MaxVal;
	delta = m_Delta;
}

BOOL CNumSpinCtrl::OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_UPDOWN* pUD = (NM_UPDOWN*)pNMHDR;
	
	double val = GetPos () + m_Delta * pUD->iDelta;

	const bool can_wrap = (UDS_WRAP & GetStyle());


	if (pUD->iDelta < 0) // spin down
	{
		double abs_eps = fabs(EPS * max (val, m_MinVal));
		if (abs_eps < EPS) abs_eps = EPS;

		if (m_MinVal - val > abs_eps){  //if (val < m_MinVal){		
			if(can_wrap){
				val = m_MaxVal;
			}
			else{
				val = m_MinVal;
			}
		}
	}
	else  // spin up
	{
		double abs_eps = fabs(EPS * max (val, m_MaxVal));
		if (abs_eps < EPS) abs_eps = EPS;
		
		if (val - m_MaxVal > abs_eps){   //if (val > m_MaxVal){		
			if(can_wrap){
				val = m_MinVal;
			}
			else{
				val = m_MaxVal;
			}
		}
	}

	SetValueForBuddy (val);

	*pResult = 0;

	return FALSE; // let parent process this notification too.
}

//int CNumSpinCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
//{
//	if (CSpinButtonCtrl::OnCreate(lpCreateStruct) == -1)
//		return -1;
//	
//	InitSpinCtrl();
//	
//	return 0;
//}

//void CNumSpinCtrl::PreSubclassWindow() 
//{
//	CSpinButtonCtrl::PreSubclassWindow();
//	InitSpinCtrl();
//}

void CNumSpinCtrl::InitSpinCtrl()
{
	ASSERT ((GetStyle () & UDS_SETBUDDYINT) != UDS_SETBUDDYINT); // Otherwise control won't work properly!
	//ModifyStyle (UDS_SETBUDDYINT, UDS_ARROWKEYS);
	SetRangeAndDelta (m_MinVal, m_MaxVal, m_Delta); // set default values
}

CString CNumSpinCtrl::FormatValue (double val)
{
	CString str;
	
	if (m_NumDecPlaces == -1) // no precision specified
	{
		str.Format (_T("%g"), val);
		return str;
	}

	CString fstr;

	/// Check which format to use ('e' or 'f')
	bool bExponential;
	fstr.Format (_T("%g"), val);
	if (fstr.Find (_T('e')) != -1)
	{
		fstr.Format (_T("%%.%de"), m_NumDecPlaces);
		bExponential = true;
	}
	else
	{
		fstr.Format (_T("%%.%df"), m_NumDecPlaces);
		bExponential = false;
	}

	/// Output the number with given format
	str.Format (fstr, val);

	/// Check for trailing zeros and remove them
	if (m_bTrimTrailingZeros)
	{
		if (bExponential)
		{
			// separate base and exponent
			CString strBase, strExponent;
			int e_pos = str.Find (_T('e'));
			strBase = str.Left (e_pos);
			strExponent = str.Right (str.GetLength () - e_pos);
			
			if (str.Find (m_DecSymbol) != -1){
				strBase.TrimRight (_T('0'));
				strBase.TrimRight (m_DecSymbol); // if dot is all that's left
			}
			
			str = strBase + strExponent; //reconstruct
		}
		else
		{
			if (str.Find (m_DecSymbol) != -1){
				str.TrimRight (_T('0'));
				str.TrimRight (m_DecSymbol); // if dot is all that's left
			}
		}
	}
	
	return str;
}

void CNumSpinCtrl::SetDecimalPlaces(int number)
{
	m_NumDecPlaces = number;
}

int CNumSpinCtrl::GetDecimalPlaces()
{
	return m_NumDecPlaces;
}

void CNumSpinCtrl::SetFormatString (LPCTSTR lpszFormatString /*= NULL*/)
{
	if (lpszFormatString == NULL)
		m_strFormat.Empty ();
	else
		m_strFormat = lpszFormatString;
}



/////////////////////////////////////////////////////////////////////////////
// JInPlaceEdit
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(JInPlaceEdit, CEdit)
        ON_WM_KILLFOCUS()
        ON_WM_CHAR()
        ON_WM_CREATE()
END_MESSAGE_MAP()

JInPlaceEdit::JInPlaceEdit()
{	
	MouseEditStyle = STYLE_USE_SPIN;
	m_bESC = FALSE;
	m_Parent = NULL;
}

JInPlaceEdit::JInPlaceEdit(int iItem, int iSubItem, CString sInitText):m_sInitText( sInitText )
{
	MouseEditStyle = STYLE_USE_SPIN;
	m_iItem		  = iItem;
	m_iSubItem = iSubItem;
	m_bESC 	  = FALSE;
	m_Parent	  = NULL;
}

void  JInPlaceEdit::PreSubclassWindow()
{
	CEdit::PreSubclassWindow();	

	if(CWnd* pEdit = GetWindow(GW_CHILD)){
		pEdit->ModifyStyle(0, ES_RIGHT);
	}
}

BOOL JInPlaceEdit::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
	{
		SHORT sKey = GetKeyState( VK_CONTROL);
		if(pMsg->wParam == VK_RETURN ||	pMsg->wParam == VK_DELETE ||
    		pMsg->wParam == VK_ESCAPE || sKey)
		{
			::TranslateMessage(pMsg);
			/* Strange but true:
			If the edit control has ES_MULTILINE and ESC
			is pressed the parent is destroyed if the 
			message is dispatched.  In this 
			case the parent is the list control. */
			if( !(GetStyle() & ES_MULTILINE) || pMsg->wParam != VK_ESCAPE )
				::DispatchMessage(pMsg);
			return TRUE;                    // DO NOT process further
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}

void	JInPlaceEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	if(m_Parent == NULL) return;
	CString str;
	GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = m_Parent->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = m_bESC ? 0 : str.GetLength();
	if(m_Parent != NULL)
		m_Parent->GetParent()->SendMessage( WM_NOTIFY, m_Parent->GetDlgCtrlID(),(LPARAM)&dispinfo );

}


void	JInPlaceEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(m_stdValidChars.GetLength() == 0)
	{
		if(nChar == VK_ESCAPE || nChar == VK_RETURN)
		{
			if( nChar == VK_ESCAPE )
				m_bESC = TRUE;
			GetParent()->SetFocus();
			return;
		}

		CEdit::OnChar(nChar, nRepCnt, nFlags);
		// Resize edit control if needed
		CalculateSize();
	}
	else
	{
		if(nChar == VK_ESCAPE || nChar == VK_RETURN)
		{
			if( nChar == VK_ESCAPE )
				m_bESC = TRUE;
			GetParent()->SetFocus();
			return;
		}
		else if(m_stdValidChars.Find(static_cast<TCHAR> (nChar)) != -1 ||
			nChar == VK_BACK || 
			nChar == CTRL_C ||
			nChar == CTRL_V ||
			nChar == CTRL_X)
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			// Resize edit control if needed
			CalculateSize();
		}
		else
			MessageBeep(MB_ICONEXCLAMATION);
	}
}

int		JInPlaceEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//ModifyStyle(0, WS_EX_TRANSPARENT);

	//DWORD dwStyle = WS_CHILD | WS_OVERLAPPED | UDS_ARROWKEYS | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_NOTHOUSANDS;
	//if(GetStyle() & WS_VISIBLE)
	//	dwStyle |= WS_VISIBLE;

	//if(MouseEditStyle & STYLE_USE_SPIN)
	//{
	//	NumSpinCtrl.Create(dwStyle, CRect(0, 0, 100, 100), GetParent(), 0);
	//	NumSpinCtrl.InitSpinCtrl();

	//	CRect EditRect;
	//	GetClientRect(&EditRect);
	//	NumSpinCtrl.SetWindowPos(this, EditRect.left + EditRect.Width() - 50, EditRect.top, 50, EditRect.Height(), SWP_NOMOVE | SWP_NOSIZE);
	//	
	//	NumSpinCtrl.SetBase(10);
	//	NumSpinCtrl.SetBuddy(this);
	//	NumSpinCtrl.EnableWindow();
	//}

	// Set the proper font  
	CFont* font = GetParent()->GetFont();
	SetFont(font);

	SetWindowText( m_sInitText );
	SetFocus();
//	CalculateSize();
	SetSel( 0, -1 );
	
	return 0;
}


void   JInPlaceEdit::CalculateSize()
{
	CRect rect;
	GetClientRect(&rect);
	ClientToScreen(&rect);
	GetParent()->ScreenToClient(&rect);
	

	CWindowDC dc(this);
	CString str;
	GetWindowText(str);
	CSize size = dc.GetTextExtent(str);
	BYTE changed = 0;
	rect.top -= 2;
	rect.bottom += 2;
	rect.left -= 2;
	rect.right += 2;
	if(rect.Width() < size.cx)
	{
		rect.right = rect.left + size.cx + 10;
		changed = 1;
	}
	if(changed)
	{
		MoveWindow(rect);
	/*	CString message;
		message.Format("top %d, bottom %d, height %d\n",rect.top,rect.bottom,rect.Height());
		TRACE(message);*/
	}
}
