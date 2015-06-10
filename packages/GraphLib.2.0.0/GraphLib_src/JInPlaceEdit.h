#pragma once

#include <afxcmn.h>

#define CTRL_C	0x3
#define CTRL_V	0x16
#define CTRL_X	0x18


class CNumSpinCtrl : public CSpinButtonCtrl
{
	DECLARE_MESSAGE_MAP()

public:
	CNumSpinCtrl();
	virtual ~CNumSpinCtrl();

public:
	void GetRangeAndDelta(double &lower, double& upper, double& delta);
	void SetRangeAndDelta(double lower, double upper, double delta);
	
	void		InitSpinCtrl();
	double	GetPos();
	void	   SetPos(double val);

	int  GetDecimalPlaces ();
	void SetDecimalPlaces (int num_places);
	void SetFormatString (LPCTSTR lpszFormatString = NULL);
	
	void SetTrimTrailingZeros (BOOL bTrim)	{ m_bTrimTrailingZeros = bTrim; }
	BOOL GetTrimTrailingZeros ()					{ return m_bTrimTrailingZeros; }
	
protected:
	double 		m_MinVal;
	double 		m_MaxVal;
	double 		m_Delta;
	UINT			m_IntRange;
	int				m_NumDecPlaces;
	CString		m_strFormat;
	BOOL		m_bTrimTrailingZeros;
	char			m_DecSymbol;

	
	void		SetIntPos (double pos);
	void		SetValueForBuddy (double val);
	CString FormatValue (double val);
//	virtual void PreSubclassWindow();

protected:
	afx_msg BOOL OnDeltapos (NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);


};



class JInPlaceEdit : public CEdit
{
		DECLARE_MESSAGE_MAP()
public:
	enum eStyle{
		STYLE_USE_SPIN	=	0x00000001,
	};
	JInPlaceEdit();
	JInPlaceEdit(int iItem, int iSubItem, CString sInitText); 

	virtual ~JInPlaceEdit(){};

public:
	void		CalculateSize();
	inline		int		GetItem(){return m_iItem;};
	inline		int		GetSubItem(){return m_iSubItem;};
	void		SetValidChars(CString  str){m_stdValidChars = str;};
	void		SetParent(CWnd* pParent){m_Parent = pParent;CEdit::SetParent(pParent);}

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void	   PreSubclassWindow();

protected:
	int				MouseEditStyle; 
	int				m_iItem;
	int				m_iSubItem;
	BOOL			m_bESC;         // To indicate whether ESC key was pressed
	CString		m_sInitText;
	CString		m_stdValidChars;
	CWnd*		m_Parent;

	CNumSpinCtrl	NumSpinCtrl;	

	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int	OnCreate(LPCREATESTRUCT lpCreateStruct);

	
public:
	inline void SetItemData(int nitem,int nsubitem,CString datastr){
		m_iItem			= nitem;
		m_iSubItem	= nsubitem;
		m_sInitText	= datastr;
		SetWindowText(m_sInitText);
		SetFocus();
		SetSel(0,-1);
	}
};

