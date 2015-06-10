#include "StdAfx.h"
#include "XGraphAxisMaker.h"
#include "XGraphAxis.h"
#include "XGraphDataNotation.h"
#include "XGRAPH.h"

extern wchar_t* char2wchar(const char* str);
extern Color	GetGDIPlusColor(COLORREF color);

CXGraphAxisMarker::CXGraphAxisMarker(void)
{
}


CXGraphAxisMarker::~CXGraphAxisMarker(void)
{
}

void CXGraphAxisMarker::BackupXvalue()
{
	m_lfOldXvalue= m_lfXValue;

	/*m_pOldDataNotation= new CXGraphDataNotation();
	m_pOldDataNotation->m_fXVal= m_pDataNotation->m_fXVal;
	m_pOldDataNotation->m_fYVal= m_pDataNotation->m_fYVal;*/
}

//  백업한 데이터 원상복구, 원상 복구한 값을 가져옴. [10/23/2013 ChoiJunHyeok]
double CXGraphAxisMarker::RestoreXvalue()
{
	m_lfXValue= m_lfOldXvalue;
	//m_pDataNotation= &m_OldDataNotation;

	return m_lfOldXvalue;
}

// 옮기기전 백업한 x값 가져옴 [11/12/2013 ChoiJunHyeok]
double CXGraphAxisMarker::GetBackupXValue()
{
	return m_lfOldXvalue;
}

// m [11/7/2013 ChoiJunHyeok]
void CXGraphAxisMarker::MoveNotation()
{
	m_pDataNotation->m_fXVal= this->m_lfXValue;
	m_pDataNotation->m_fYVal= this->m_lfYValue;

	m_pDataNotation->m_nIndex= m_pAxis->GetCloseIndexByAxis(this->m_lfXValue, 0); 

	CString strText;//, strX, strY, strFmt;
	//strX.Format("%g", m_lfXValue);
	//strY.Format("%g", m_lfYValue);
	//strFmt  = "X : %s, Y : %s";
	//strText.Format(strFmt, strX, strY);
	strText= "Moving...";

	m_pDataNotation->m_cText= strText;

}

void	CXGraphAxisMarker::Draw(Graphics& graphics)
{
	CPoint pPoint;
	TDataPoint dPoint;
	wchar_t *pwstr = char2wchar("Arial");
	Gdiplus::Font axisfont(pwstr, m_pAxis->GetFontInfo()->size, m_pAxis->GetFontInfo()->style, UnitPixel);
	free(pwstr);

	//if (m_pAxis->GetAxisKind() == yAxis)	// y Axis이면 세로선
	//{
	//	//	CPenSelector ps(m_AxisMarkers[i].crColor, m_AxisMarkers[i].nSize, pDC, m_AxisMarkers[i].nStyle);
	//	Pen pen(GetGDIPlusColor(m_crColor),m_nSize);
	//	pen.SetDashStyle((DashStyle)m_nStyle);

	//	SolidBrush brush(GetGDIPlusColor(m_crColor));

	//	dPoint.fXVal = m_lfXValue;
	//	dPoint.fYVal = m_lfYValue;
	//	CPoint point = m_pAxis->GetPointForValue(&dPoint);

	//	if (m_pGraph->GetInnerRect().top > point.y || 
	//		m_pGraph->GetInnerRect().bottom < point.y)
	//		return;

	//	graphics.DrawLine(&pen,m_pGraph->GetInnerRect().left, point.y,
	//		m_pGraph->GetInnerRect().right, point.y);


	//	pPoint.y = point.y;
	//	pPoint.x = m_pGraph->GetInnerRect().left;

	//	if (m_bShowValue)	// value 보일꺼면
	//	{
	//		CString strVal;
	//		if (m_pAxis->GetDateTime())   
	//			strVal = COleDateTime(dPoint.fYVal).Format(m_pAxis->GetDisplayFmt());
	//		else
	//			strVal.Format(m_pAxis->GetDisplayFmt(), dPoint.fYVal);

	//		pwstr = char2wchar(strVal);
	//		graphics.DrawString(pwstr,strVal.GetLength(),&axisfont,PointF(pPoint.x,pPoint.y),&brush);
	//		free(pwstr);
	//	}

	//	if(m_strLabel != _T(""))
	//	{
	//		RectF sizeRect(0,0,0,0);
	//		RectF outRect;
	//		pwstr = char2wchar("W");
	//		graphics.MeasureString(pwstr,1,&axisfont,sizeRect,&outRect);
	//		free(pwstr);
	//		RectF labelRect(m_pGraph->GetInnerRect().left, point.y, 
	//			m_pGraph->GetInnerRect().Width(),outRect.Height);
	//		pwstr = char2wchar(m_strLabel);
	//		graphics.DrawString(pwstr,m_strLabel.GetLength(),
	//			&axisfont,PointF(labelRect.X,labelRect.Y),&brush);
	//		free(pwstr);
	//	}
	//}
	//else	
	{ // x만 그리자 [10/29/2013 ChoiJunHyeok]
		//m_crColor= RGB(0,0,0);
		Pen pen(GetGDIPlusColor(m_crColor),m_nSize);
		pen.SetDashStyle((DashStyle)m_nStyle);

		SolidBrush brush(GetGDIPlusColor(m_crColor));

		dPoint.fXVal = m_lfXValue;
		dPoint.fYVal = m_lfYValue;
		CPoint point = m_pAxis->GetPointForValue(&dPoint);

		if (m_pGraph->GetInnerRect().left > point.x || m_pGraph->GetInnerRect().right < point.x)
			return;

		graphics.DrawLine(&pen,
			point.x, m_pGraph->GetInnerRect().top,
			point.x, m_pGraph->GetInnerRect().bottom);
		pPoint.x = point.x;
		pPoint.y = m_pGraph->GetInnerRect().bottom-15;


		if (m_bShowValue)
		{
			CString strVal;
			if (m_pAxis->GetIRIGTime())
			{
				//strVal.Format("X : "+m_pAxis->GetDisplayFmt()+"\nY : "+m_pAxis->GetDisplayFmt(), dPoint.fXVal, dPoint.fYVal);
				//strVal.Format("X : "+m_pAxis->GetDisplayFmt(), dPoint.fXVal);
			}
			else
			//	strVal.Format("X : "+m_pAxis->GetDisplayFmt()+"\nY : "+m_pAxis->GetDisplayFmt(), dPoint.fXVal, dPoint.fYVal);
				strVal.Format("X : "+m_pAxis->GetDisplayFmt(), dPoint.fXVal);

			pwstr = char2wchar(strVal);
			graphics.DrawString(pwstr, strVal.GetLength(), &axisfont, PointF(pPoint.x,pPoint.y), &brush);
			free(pwstr);
		}

		if (m_strLabel != _T(""))
		{
			RectF sizeRect(0,0,0,0);
			RectF outRect;
			pwstr = char2wchar("W");
			graphics.MeasureString(pwstr,1,&axisfont,sizeRect,&outRect);
			free(pwstr);
			RectF labelRect(point.x, m_pGraph->GetInnerRect().top + outRect.Height, point.x + outRect.Width, m_pGraph->GetInnerRect().bottom);
			pwstr = char2wchar(m_strLabel);
			graphics.DrawString(pwstr, m_strLabel.GetLength(), &axisfont, PointF(pPoint.x, 5), &brush);
			free(pwstr);
		}

		/*if(m_eLineType==LINE_SLICE_START)
		{
			pwstr = char2wchar("S");
			graphics.DrawString(pwstr,m_strLabel.GetLength(),&axisfont,PointF(pPoint.x, pPoint.y),&brush);
			free(pwstr);
		}
		else if(m_eLineType==LINE_SLICE_END)
		{
			pwstr = char2wchar("E");
			graphics.DrawString(pwstr,m_strLabel.GetLength(),&axisfont,PointF(pPoint.x, pPoint.y),&brush);
			free(pwstr);
		}*/
	}
}

void	CXGraphAxisMarker::Draw(CDCEx* pDC)
{

}