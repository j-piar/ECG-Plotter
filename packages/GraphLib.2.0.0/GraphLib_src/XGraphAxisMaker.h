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

	EAxisKind	m_AxisKind;	// axis�� ���� �ٸ��� �׸� [10/7/2013 ChoiJunHyeok]
	UINT		m_eLineType;	// linetype [10/7/2013 ChoiJunHyeok]
	UINT		m_nSliceNumber;	// slice line�� ��� ���� �ѹ� [10/23/2013 ChoiJunHyeok]
	double		m_lfOldXvalue;	// �ű�� ���� ������ [10/23/2013 ChoiJunHyeok]
	
	//CXGraphDataNotation* m_pOldDataNotation;	// �����̼��� ��� [11/7/2013 ChoiJunHyeok]
	CXGraphDataNotation* m_pDataNotation;	// ����� ������ �����̼�  [11/7/2013 ChoiJunHyeok]
	BOOL	m_bShowNotation;	// �����̼� ������ ������ [11/7/2013 ChoiJunHyeok]

	CXGraphAxis* m_pAxis;	// member [10/7/2013 ChoiJunHyeok]
public:
	// line �׸��� [9/30/2013 ChoiJunHyeok]
	virtual void	Draw(CDCEx* pDC);
	virtual void	Draw(Graphics& graphics);

	void BackupXvalue();	// �����̱� �� ������ ��� [10/23/2013 ChoiJunHyeok]
	double RestoreXvalue();	// ����� ������ ���󺹱� [10/23/2013 ChoiJunHyeok]
	void MoveNotation();	// �����̼� �̵� [11/7/2013 ChoiJunHyeok]
	double GetBackupXValue();	// ����� ������ ���󺹱�, ���� ������ ���� ������. [10/23/2013 ChoiJunHyeok]

};

