#include "stdafx.h"
#include "SetupCurveCtrl.h"

#define NAMESUBITEM		0
#define SHOWSUBITEM		1
#define LINESUBITEM		2
#define THICKSUBITEM	3
#define COLORSUBITEM	4

#define	SHOWIMAGE		1
#define	HIDEIMAGE		2
// CSetupCurveCtrl

IMPLEMENT_DYNAMIC(CSetupCurveCtrl, CListCtrl)

CSetupCurveCtrl::CSetupCurveCtrl()
{
	m_DisableTextColor = ::GetSysColor(COLOR_GRAYTEXT);
	m_WindowColor = ::GetSysColor(COLOR_WINDOW);	
	m_CurSubItem = 0;
	m_CurItem = 0;

	m_Font.CreatePointFont(85,_T("Arial"));
	//if(!m_Font.CreatePointFont(85,_T("Arial"))){
	//	m_Font.DeleteObject();

	//	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	//	if(pMainFrm->FontList.size()>0){
	//		CString FontName = pMainFrm->FontList[0];
	//		m_Font.CreatePointFont(85, FontName);
	//	}
	//}

	
}

CSetupCurveCtrl::~CSetupCurveCtrl()
{
	m_Font.DeleteObject();
}


BEGIN_MESSAGE_MAP(CSetupCurveCtrl, CListCtrl)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_WM_LBUTTONDOWN()
	ON_CBN_SELCHANGE(IDC_LINETYPE, OnChanged)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CSetupCurveCtrl::OnNMCustomdraw)
END_MESSAGE_MAP()



// CSetupCurveCtrl message handlers

void CSetupCurveCtrl::SetItemImage(int item, int subitem, int imageno)
{
	LVITEM lvitem;
	lvitem.iImage = imageno;
	lvitem.iItem = item;
	lvitem.iSubItem = subitem;
	lvitem.mask = LVIF_IMAGE;
	SetItem(&lvitem);
}

int CSetupCurveCtrl::GetItemImage(int item,int subitem)
{
	LVITEM lvitem;
	lvitem.iItem = item;
	lvitem.iSubItem = subitem;
	lvitem.mask = LVIF_IMAGE;
	GetItem(&lvitem);
	return lvitem.iImage;
}

int CSetupCurveCtrl::IndexToOrder(int iIndex)
{
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int colcount = pHeader->GetItemCount();
	int *orderarray = new int[colcount];
	Header_GetOrderArray(pHeader->m_hWnd,colcount,orderarray);
	int i;
	for( i=0; i<colcount; i++ )
	{
		if( orderarray[i] == iIndex )
		{
			delete orderarray;
			return i;
		}
	}
	delete orderarray;
	return -1;
}

void CSetupCurveCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	ENSURE(pNMHDR != NULL);
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;

	BYTE type = 0;
	int subitem = lplvcd->iSubItem;
	int item = (int)lplvcd->nmcd.dwItemSpec;
	switch(lplvcd->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT:	
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;
		case CDDS_ITEMPREPAINT:
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;
		case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
			if(subitem == 4)
				lplvcd->clrTextBk = m_CurveInfoArray.GetAt(item).color;
			else
				lplvcd->clrTextBk = m_WindowColor;
			*pResult = CDRF_NOTIFYSUBITEMDRAW | CDRF_NOTIFYPOSTPAINT;
			break;
		case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM:
			{
				CDC* pDC = CDC::FromHandle(lplvcd->nmcd.hdc);
				if(lplvcd->nmcd.uItemState & CDIS_FOCUS)
				{
					if(subitem == 1) return;
					CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
					if(subitem == Header_OrderToIndex(pHeader->m_hWnd,m_CurSubItem))
					{
						int offset = pDC->GetTextExtent(_T(" "),1).cx*2;
						CRect rect = lplvcd->nmcd.rc;
						CRect bounds;
						GetItemRect(item,&bounds,LVIR_BOUNDS);
						rect.top = bounds.top;
						rect.bottom = bounds.bottom;
						if(subitem == 0)
						{
							CRect lrect;
							GetItemRect(item,&lrect,LVIR_LABEL);
							rect.left = lrect.left;
							rect.right = lrect.right;
						}
						else
						{
							rect.right += bounds.left;
							rect.left  += bounds.left;
						}
						pDC->FillRect(rect, &CBrush(::GetSysColor(COLOR_3DFACE)));
						CString str;
						str = GetItemText(item,subitem);
						pDC->DrawText(str,rect,DT_SINGLELINE|DT_NOPREFIX|DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS);
						rect.DeflateRect(offset, 0);
						rect.InflateRect(offset, 0);
						pDC->Draw3dRect( &rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DFACE) );
						rect.DeflateRect(1,1);
						pDC->Draw3dRect( &rect, ::GetSysColor(COLOR_3DDKSHADOW), ::GetSysColor(COLOR_3DHILIGHT) );
					
						switch(subitem)
						{
							case COLORSUBITEM:
								{
									CBrush br(m_CurveInfoArray.GetAt(item).color);
									pDC->FillRect(rect, &br);
									m_LineType.ShowWindow(SW_HIDE);	
								}
								break;
							case LINESUBITEM:
								{
									rect.bottom += 300;
									m_LineType.MoveWindow(rect);
									m_LineType.ShowWindow(SW_SHOW);
									m_LineType.SetCurSel(m_CurveInfoArray.GetAt(item).line);
									m_CurItem = item;
								}
								break;
							default:
								m_LineType.ShowWindow(SW_HIDE);	
								break;
						}
						*pResult = CDRF_SKIPDEFAULT;
					}
					else if(subitem == COLORSUBITEM)
					{
						CRect rect = lplvcd->nmcd.rc;
						CRect bounds;
						GetItemRect( item, &bounds, LVIR_BOUNDS );
						// Get the top and bottom from the item itself.
						rect.top = bounds.top;
						rect.bottom = bounds.bottom;

						rect.right += bounds.left;
						rect.left  += bounds.left;
						CBrush br(m_CurveInfoArray.GetAt(item).color);
						pDC->FillRect(rect, &br);
						*pResult = CDRF_SKIPDEFAULT;
					}
					else if(subitem == LINESUBITEM)
					{
						CDCEx dc;
						CRect rect = lplvcd->nmcd.rc;
						CRect bounds;
						GetItemRect( item, &bounds, LVIR_BOUNDS );
						// Get the top and bottom from the item itself.
						rect.top = bounds.top;
						rect.bottom = bounds.bottom;

						rect.right += bounds.left;
						rect.left  += bounds.left;

						dc.Attach (pDC->GetSafeHdc());
						int nSaveDC = ::SaveDC(pDC->GetSafeHdc());
						dc.FillSolidRect(&rect,::GetSysColor(COLOR_WINDOW));
						{
							CPenSelector ps(0L, 1, &dc, m_CurveInfoArray.GetAt(item).line);
							dc.MoveTo(rect.left, rect.top + 10);
							dc.LineTo(rect.right, rect.top + 10);
						}
						::RestoreDC(pDC->GetSafeHdc(), nSaveDC);
						dc.Detach ();
						*pResult = CDRF_SKIPDEFAULT;
					}
				}
				else if(lplvcd->nmcd.uItemState & CDIS_SELECTED)
				{
					switch(subitem)
					{
						case LINESUBITEM:
							{
								CDCEx dc;
								CRect rect = lplvcd->nmcd.rc;
								CRect bounds;
								GetItemRect( item, &bounds, LVIR_BOUNDS );
								// Get the top and bottom from the item itself.
								rect.top = bounds.top;
								rect.bottom = bounds.bottom;

								rect.right += bounds.left;
								rect.left  += bounds.left;

								dc.Attach (pDC->GetSafeHdc());
								int nSaveDC = ::SaveDC(pDC->GetSafeHdc());
								dc.FillSolidRect(&rect,::GetSysColor(COLOR_WINDOW));
								{
									CPenSelector ps(0L, 1, &dc, m_CurveInfoArray.GetAt(item).line);
									dc.MoveTo(rect.left, rect.top + 10);
									dc.LineTo(rect.right, rect.top + 10);
								}
								::RestoreDC(pDC->GetSafeHdc(), nSaveDC);
								dc.Detach ();
								*pResult = CDRF_SKIPDEFAULT;
							}
							break;
					}
				}
				else
				{
					m_LineType.ShowWindow(SW_HIDE);
					//m_ColorSel.ShowWindow(SW_HIDE);
				}
			}
			break;
		default:
			break;
	}
}

void CSetupCurveCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	LVHITTESTINFO ht;
	ht.pt = point;
	int rval = ListView_SubItemHitTest( m_hWnd, &ht );
	int oldsubitem = m_CurSubItem;
	m_CurSubItem = IndexToOrder( ht.iSubItem );

	int state = GetItemState( ht.iItem, LVIS_FOCUSED );
	CListCtrl::OnLButtonDown(nFlags, point);
	if( !state || m_CurSubItem == -1 || oldsubitem != m_CurSubItem )
	{
		if(m_CurSubItem != -1)
			RedrawItems(ht.iItem,ht.iItem);
		return;
	}

	int doedit = 0;
	if( 0 == ht.iSubItem )
	{
		if( ht.flags & LVHT_ONITEMLABEL ) doedit = 1;
	}
	else
		doedit = 1;
	if( !doedit ) return;
	CString str;
	str = GetItemText( ht.iItem, ht.iSubItem );
    LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_BEGINLABELEDIT;
	
	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = ht.iItem;
	dispinfo.item.iSubItem = ht.iSubItem;
	dispinfo.item.pszText = (LPTSTR)((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();
	GetParent()->SendMessage( WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo );
}

void CSetupCurveCtrl::Initialize(CArray<CXGraphDataSerie*,CXGraphDataSerie*> &dataarry)
{
	SetFont(&m_Font);
	InsertColumn(NAMESUBITEM,_T("Name"));
	InsertColumn(SHOWSUBITEM,_T(""));
	InsertColumn(LINESUBITEM,_T("Line"));
	InsertColumn(THICKSUBITEM,_T("Thickness"));
	InsertColumn(COLORSUBITEM,_T("Color"));
	SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES);

	m_ImageList.Create(IDB_1DCURVESHOW,20,20,RGB(255,255,255));
	SetImageList(&m_ImageList,LVSIL_SMALL);

	CurveInfo	info;
	CString		value;
	CXGraphDataSerie* pData = NULL;
	UINT i = 0, Count = (UINT)dataarry.GetCount();
	for(i = 0;i < Count; i++)
	{
		pData = dataarry.GetAt(i);
		info.name = pData->GetLabel();
		info.IsShow = pData->GetVisible();
		info.line = pData->GetLineStyle();
		info.thickness = pData->GetLineSize();
		info.color = pData->GetColor();
		m_CurveInfoArray.Add(info);
		InsertItem(i,info.name);
		SetItemText(i,SHOWSUBITEM,_T(""));
		switch(info.line)
		{
			case 0: value = _T("__________"); break;
			case 1: value = _T("_ _ _ _ _ "); break;
			case 2: value = _T(".........."); break;
			case 3: value = _T("_._._._._."); break;
			case 4: value = _T("_.._.._.._"); break;
		}
		SetItemText(i,LINESUBITEM,value);
		value.Format(_T("%d"),info.thickness);
		SetItemText(i,THICKSUBITEM,value);
		SetItemText(i,COLORSUBITEM,_T(""));
		if(info.IsShow == true)	SetItemImage(i,1,SHOWIMAGE);
		else					SetItemImage(i,1,HIDEIMAGE);
	}
	SetColumnWidth(NAMESUBITEM,-2);
	SetColumnWidth(SHOWSUBITEM,16);
	SetColumnWidth(LINESUBITEM,-2);
	SetColumnWidth(THICKSUBITEM,-2);
	SetColumnWidth(COLORSUBITEM,-2);

	m_ListEdit.CreateEx(WS_EX_CLIENTEDGE,"EDIT",NULL,ES_LEFT|WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL,CRect(0,0,0,0),this,IDC_JINPLACEEDIT);
	m_ListEdit.SetFont(GetFont());
	m_ListEdit.SetParent(this);

	//m_ColorSel.Create(_T(""),WS_CHILD|WS_VISIBLE|WS_VSCROLL|BS_OWNERDRAW,CRect(0,0,0,0), this, IDC_CURVECOLOR);
	m_LineType.Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST|CBS_OWNERDRAWFIXED,CRect(0,0,0,0), this, IDC_LINETYPE);
	m_LineType.AddString(_T("_____"));
	m_LineType.AddString(_T("_ _ _"));
	m_LineType.AddString(_T("....."));
	m_LineType.AddString(_T("_._._"));
	m_LineType.AddString(_T("_.._."));
	m_LineType.SetFont(&m_Font);

}

void CSetupCurveCtrl::OnBeginLabelEdit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	CString str = pDispInfo->item.pszText;
	int item = pDispInfo->item.iItem;
	int subitem = pDispInfo->item.iSubItem;
	switch(subitem)
	{
		case NAMESUBITEM:
		case THICKSUBITEM:
			{
				m_ListEdit.SetItemData(item,subitem,str);
				PositionInPlaceEdit(&m_ListEdit,item,subitem);
				*pResult = 1;
			}
			break;
		case SHOWSUBITEM:
			{
				CurveInfo info = m_CurveInfoArray.GetAt(item);
				info.IsShow = !info.IsShow;
				if(info.IsShow == true)	SetItemImage(item,1,SHOWIMAGE);
				else					SetItemImage(item,1,HIDEIMAGE);
				m_CurveInfoArray.SetAt(item,info);
			}
			break;
		case LINESUBITEM:
			break;
		case COLORSUBITEM:
			{
				CurveInfo info = m_CurveInfoArray.GetAt(item);
				CColorDialog colorDlg(info.color, CC_FULLOPEN , this );
				if(colorDlg.DoModal() == IDOK)
					info.color = colorDlg.GetColor();
				m_CurveInfoArray.SetAt(item,info);
				SetItemText(item,COLORSUBITEM,_T(""));
			}
			break;
	}
}

void CSetupCurveCtrl::OnEndLabelEdit(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(m_ListEdit.IsWindowVisible() == TRUE)
	{
		CString str;
		m_ListEdit.GetWindowText(str);
		int item = m_ListEdit.GetItem();
		int subitem = m_ListEdit.GetSubItem();
		CurveInfo info = m_CurveInfoArray.GetAt(item);
		if(subitem == NAMESUBITEM)
		{
			info.name = str;
			m_CurveInfoArray.SetAt(item,info);
			SetItemText(item,NAMESUBITEM,info.name);
		}
		else if(subitem == THICKSUBITEM)
		{
			info.thickness = atoi(str); 
			m_CurveInfoArray.SetAt(item,info);
			str.Format(_T("%d"),info.thickness);
			SetItemText(item,THICKSUBITEM,str);
		}
		m_ListEdit.ShowWindow(SW_HIDE);
	}
}

void CSetupCurveCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if(nIDCtl == IDC_LINETYPE)
	{
		CDCEx dc;
		if (lpDrawItemStruct->itemID < 0 || lpDrawItemStruct->itemID >= 5)
			return;

		dc.Attach (lpDrawItemStruct->hDC);
		int nSaveDC = ::SaveDC(lpDrawItemStruct->hDC);
		dc.FillSolidRect (&lpDrawItemStruct->rcItem,::GetSysColor(COLOR_WINDOW));
		{
			CPenSelector ps(0L, 1, &dc, lpDrawItemStruct->itemID);
			dc.MoveTo(lpDrawItemStruct->rcItem.left, lpDrawItemStruct->rcItem.top + 6);
			dc.LineTo(lpDrawItemStruct->rcItem.right, lpDrawItemStruct->rcItem.top + 6);
		}
		::RestoreDC (lpDrawItemStruct->hDC, nSaveDC);
		dc.Detach ();
	}
	else
		CListCtrl::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


void CSetupCurveCtrl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	if (nIDCtl == IDC_LINETYPE)
	{
		lpMeasureItemStruct->itemHeight = 16;
		lpMeasureItemStruct->itemWidth  = 100;
	}
	else
		CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CSetupCurveCtrl::OnChanged() 
{
	int Sel = m_LineType.GetCurSel();
	if(Sel >= 0)
	{
		CurveInfo info = m_CurveInfoArray.GetAt(m_CurItem);
		info.line = (UINT)Sel;
		
		CString value;
		switch(info.line)
		{
			case 0: value = _T("__________"); break;
			case 1: value = _T("_ _ _ _ _ "); break;
			case 2: value = _T(".........."); break;
			case 3: value = _T("_._._._._."); break;
			case 4: value = _T("_.._.._.._"); break;
		}
		SetItemText(m_CurItem,LINESUBITEM,value);
		m_CurveInfoArray.SetAt(m_CurItem,info);
	}
}

LRESULT CSetupCurveCtrl::OnColorChanged(WPARAM, LPARAM)
{
	
	return 0;
}

BOOL CSetupCurveCtrl::PositionInPlaceEdit(CWnd * pWnd,int iItem,int iSubItem)
{
	ASSERT( pWnd && pWnd->m_hWnd );
	ASSERT( iItem >= 0 );
	// Make sure that the item is visible
	if( !EnsureVisible( iItem, TRUE ) ) return NULL;
	
	// Make sure that nCol is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	ASSERT( iSubItem >= 0 && iSubItem < nColumnCount );
	if( iSubItem >= nColumnCount || 
		// We have to take into account that the header may be reordered
		GetColumnWidth(Header_OrderToIndex( pHeader->m_hWnd,iSubItem)) < 5 )
	{
		return 0;
	}
	
	// Get the header order array to sum the column widths up to the selected cell.
	int *orderarray = new int[ nColumnCount ];
	Header_GetOrderArray( pHeader->m_hWnd, nColumnCount, orderarray );
	int offset = 0;
	int i;
	for( i = 0; orderarray[i] != iSubItem; i++ )
		offset += GetColumnWidth( orderarray[i] );
	int colwidth = GetColumnWidth( iSubItem );
	delete[] orderarray;
	
	CRect rect;
	GetItemRect( iItem, &rect, LVIR_BOUNDS );
	
	// Scroll if we need to expose the column
	CRect rcClient;
	GetClientRect( &rcClient );
	if( offset + rect.left < 0 || offset + colwidth + rect.left > rcClient.right )
    {
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll( size );
		rect.left -= size.cx;
    }
	
	rect.left += offset+4;
	rect.right = rect.left + colwidth - 3 ;
	// The right end of the control should not go past the edge 
	// of the grid control.
	if( rect.right > rcClient.right) 
		rect.right = rcClient.right;

	rect.top -= 4;
	rect.left -= 4;
	pWnd->MoveWindow(&rect);
	pWnd->ShowWindow(SW_SHOW);
/*	CString message;
	message.Format("top %d, bottom %d, height %d",rect.top,rect.bottom,rect.Height());
	TRACE(message);*/
	return 1;	
}