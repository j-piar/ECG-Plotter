#pragma once
#include "xgraphobjectbase.h"
#include <GdiPlus.h>

using namespace Gdiplus;


class CXGraphDataNotation;
class CXGraph;
class CXGraphLine :	public CXGraphObject
{
public:
	CXGraphLine(void);
	virtual ~CXGraphLine(void);

public:
	enum
	{
		LINE_NORMAL, LINE_MAX, LINE_MIN //, LINE_EVAL, LINE_SLICE_START, LINE_SLICE_END
	};

protected:

	UINT m_nLineType;		// line type [9/30/2013 ChoiJunHyeok]

	Point m_ptStart;		// 시작점 [9/30/2013 ChoiJunHyeok]
	Point m_ptEnd;			// 끝점 [9/30/2013 ChoiJunHyeok]
	CPoint  m_ptStartDC;	// 시작점 [9/30/2013 ChoiJunHyeok]
	CPoint m_ptEndDC;		// 끝점 [9/30/2013 ChoiJunHyeok]

	double m_lfX;			// line의 x value [10/2/2013 ChoiJunHyeok]
	double m_lfY;			// 설정된 부분 포인트의 y value [10/2/2013 ChoiJunHyeok]	

//public:
	CFont* m_pFont;			// 사용하는 폰트 [9/30/2013 ChoiJunHyeok]
public:
	static UINT s_nDisplayNumber; // 라인넘버 [9/30/2013 choiJunHyeok]
	UINT m_nCurrGroup;	// 현재 묶여있는 그룹 (전체이면 0) [10/24/2013 ChoiJunHyeok]

	CXGraph* m_pGraph;	// member [10/2/2013 ChoiJunHyeok]
	int		m_nCurve;	// series number [10/2/2013 ChoiJunHyeok]

	CXGraphDataNotation* m_pDataNotation;	// 연결된 데이터 노테이션 [10/24/2013 ChoiJunHyeok]

	//void	UnLinkNotation();	// 연결된 노티 삭제 [10/24/2013 ChoiJunHyeok]

	virtual void	Draw(CDCEx *pDC);
	virtual void	Draw(Graphics& graphics);

	void SetStart(CPoint _Start)
	{
		m_ptStartDC= _Start;
		m_ptStart.X= _Start.x;
		m_ptStart.Y= _Start.y;
	}

	void SetEnd(CPoint _End)
	{
		m_ptEndDC= _End;
		m_ptEnd.X= _End.x;
		m_ptEnd.Y= _End.y;
	}

	// X값 [10/2/2013 ChoiJunHyeok]
	double GetXValue() const { return m_lfX; }
	void SetX(double _X) { m_lfX = _X; }

	// Y값 [10/2/2013 ChoiJunHyeok]
	double GetY() const { return m_lfY; }
	void SetY(double _Y) { m_lfY = _Y; }

	UINT GetLineType() const { return m_nLineType; }
	void SetLineType(UINT _LineType)
	{
		m_nLineType = _LineType;

		/*if((_LineType==LINE_EVAL) || (_LineType==LINE_NORMAL))
		{
		++s_nDisplayNumber;
		m_nCurrDisplay= s_nDisplayNumber;
		}
		else if(_LineType==LINE_SLICE_START)
		{
		++s_nSliceStart;
		m_nCurrStart= s_nSliceStart;
		}
		else if(_LineType==LINE_SLICE_END)
		{
		++s_nSliceEnd;
		m_nCurrEnd= s_nSliceEnd;
		}*/
	}
};


