#pragma once
#include "xgraphobjectbase.h"
#include <GdiPlus.h>


using namespace Gdiplus;

class CXGraphDataNotation;
class CXGraphAxis;
class CXGraphAxisMarker : public CXGraphObject
{
public:
	enum EAxisKind
	{
		xAxis,
		yAxis
	};
	enum ELineType
	{
		LINE_EVAL,
		LINE_SLICE_START,
		LINE_SLICE_END
	};
public:
	CXGraphAxisMarker(void);
	~CXGraphAxisMarker(void);

public:
	double   m_lfXValue;
	double	 m_lfYValue;
	COLORREF m_crColor;
	bool     m_bVisible;
	bool	 m_bShowValue;
	UINT     m_nStyle;
	short    m_nSize;
	LOGFONT  m_lfFont;
	CString  m_strLabel;

	EAxisKind	m_AxisKind;	// axis에 따라 다르게 그림 [10/7/2013 ChoiJunHyeok]
	UINT		m_eLineType;	// linetype [10/7/2013 ChoiJunHyeok]
	UINT		m_nSliceNumber;	// slice line일 경우 구역 넘버 [10/23/2013 ChoiJunHyeok]
	double		m_lfOldXvalue;	// 옮기기 이전 데이터 [10/23/2013 ChoiJunHyeok]
	
	//CXGraphDataNotation* m_pOldDataNotation;	// 노테이션을 백업 [11/7/2013 ChoiJunHyeok]
	CXGraphDataNotation* m_pDataNotation;	// 연결된 데이터 노테이션  [11/7/2013 ChoiJunHyeok]
	BOOL	m_bShowNotation;	// 노테이션 보여줄 것인지 [11/7/2013 ChoiJunHyeok]

	CXGraphAxis* m_pAxis;	// member [10/7/2013 ChoiJunHyeok]
public:
	// line 그리기 [9/30/2013 ChoiJunHyeok]
	virtual void	Draw(CDCEx* pDC);
	virtual void	Draw(Graphics& graphics);

	void BackupXvalue();	// 움직이기 전 데이터 백업 [10/23/2013 ChoiJunHyeok]
	double RestoreXvalue();	// 백업한 데이터 원상복구 [10/23/2013 ChoiJunHyeok]
	void MoveNotation();	// 노테이션 이동 [11/7/2013 ChoiJunHyeok]
	double GetBackupXValue();	// 백업한 데이터 원상복구, 원상 복구한 값을 가져옴. [10/23/2013 ChoiJunHyeok]

};

