#pragma once
#include "xgraphaxis.h"

//#ifdef  _AFXDLL
//class __declspec(dllexport) CXGraphLogAxis : public CXGraphAxis
//#else
//class __declspec(dllimport) CXGraphLogAxis : public CXGraphAxis
//#endif
class CXGraphLogAxis : public CXGraphAxis
{
	DECLARE_SERIAL(CXGraphLogAxis)
public:
	CXGraphLogAxis(void);
	CXGraphLogAxis(const CXGraphLogAxis& copy);
	~CXGraphLogAxis(void);

// Attribute
	enum EAxisType {LOG, LINEAR};
private:
	EAxisType m_AxisType;

// Operation
public:
	virtual bool	SetRange(double fMin, double fMax);
	virtual void	AutoScale(CDCEx* pDC);
	virtual void	Draw(CDCEx* pDC);
	virtual double	GetValueForPos(int nPos);
	virtual CPoint	GetPointForValue(TDataPoint* point);
	virtual inline void	SetAxisType(EAxisType type){m_AxisType = type;};

	CXGraphLogAxis& operator = (const CXGraphLogAxis& copy);
};
