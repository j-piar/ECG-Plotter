#pragma once

#include "XGraphDataSerie.h"
#include "JInPlaceEdit.h"
#include "ColourPicker.h"

struct CurveInfo
{
	CString		name;
	bool		IsShow;		
	UINT		line;
	int			thickness;
	COLORREF	color;
};

// CSetupCurveCtrl

class CSetupCurveCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CSetupCurveCtrl)

public:
	CSetupCurveCtrl();
	virtual ~CSetupCurveCtrl();

protected:
	CFont			m_Font;
	JInPlaceEdit	m_ListEdit;
	int				m_CurItem;
	int				m_CurSubItem;
	CImageList		m_ImageList;
	CComboBox		m_LineType;
	COLORREF		m_WindowColor;
	COLORREF		m_DisableTextColor;
	
	CArray<CurveInfo,CurveInfo>	m_CurveInfoArray;

protected:
	int			IndexToOrder(int iIndex);
	int			GetItemImage(int item,int subitem);
	void		SetItemImage(int item,int subitem,int imageno);

	DECLARE_MESSAGE_MAP()


public:
	afx_msg void OnChanged();
	afx_msg LRESULT OnColorChanged(WPARAM, LPARAM);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg	void OnBeginLabelEdit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg	void OnEndLabelEdit(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg BOOL PositionInPlaceEdit(CWnd * pWnd,int iItem,int iSubItem);

public:
	CurveInfo	GetAt(int idx){return m_CurveInfoArray.GetAt(idx);};
	UINT	GetCount(){return (UINT)m_CurveInfoArray.GetCount();};
	void	Initialize(CArray<CXGraphDataSerie*,CXGraphDataSerie*> &dataarry);
};


