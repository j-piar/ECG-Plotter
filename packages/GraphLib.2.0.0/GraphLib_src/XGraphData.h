#pragma once
#include <vector>
using namespace std;


// 시리즈 한개의 데이터 [7/2/2013 ChoiJunHyeok]
class CXSeriesData
{
public:
	CXSeriesData() {}
	~CXSeriesData() 
	{
		this->Clear();
	}

public:
	vector<double> m_vXData;			// X 데이터 [point]				[7/2/2013 ChoiJunHyeok]
	vector<double> m_vYData;			// Y 데이터 [각rect별][point]   [7/2/2013 ChoiJunHyeok]
	vector<UINT64> m_vKey;				// IRIG TIME 데이터 [10/25/2013 ChoiJunHyeok]
	CString m_strLabel;			// 시리즈 레이블 [9/16/2013 ChoiJunHyeok]

public:
	// Xgraph 에 맞는 데이터로 변경 [9/16/2013 ChoiJunHyeok]
	TDataPoint* TransXGraphData()
	{
		TDataPoint* pValueData = new TDataPoint[m_vXData.size()];

		for(size_t i=0; i<m_vXData.size(); i++)
		{
			pValueData[i].fXVal = m_vXData[i];
			pValueData[i].fYVal = m_vYData[i];
		}
		return pValueData;
	}

	void Clear()
	{
		m_vXData.clear();
		m_vYData.clear();
		m_vKey.clear();
	}
	double AverageY()	// Y평균값 [8/27/2013 ChoiJunHyeok]
	{
		double lfSum= 0.0f;
		for (size_t i=0; i<m_vYData.size(); ++i)
		{
			lfSum += m_vYData[i];
		}

		return (lfSum / m_vYData.size());
	}
	double AverageRangeY(size_t idxStart, size_t idxEnd)	// Range모드에서 범위 만큼의 평균값  [10/8/2013 ChoiJunHyeok]
	{
		double lfSum= 0.0f;
		for (size_t i=idxStart; i<idxEnd; ++i)
		{
			lfSum += m_vYData[i];
		}

		return (lfSum / m_vYData.size());
	}
	void InsertXY(double x, double y)	// x,y값 한꺼번에 insert [8/28/2013 ChoiJunHyeok]
	{
		m_vXData.push_back(x);
		m_vYData.push_back(y);
	}
	void InsertXYK(double x, double y, UINT64 key)	// x,y값 한꺼번에 insert [8/28/2013 ChoiJunHyeok]
	{
		m_vXData.push_back(x);
		m_vYData.push_back(y);
		m_vKey.push_back(key);
	}
	void PlusY(double lfFactor)
	{
		for (size_t i=0; i<m_vYData.size(); ++i)
		{
			m_vYData[i] = m_vYData[i] + lfFactor;	// 모든 데이터에 더한다. [8/29/2013 ChoiJunHyeok]
		}
	}

	void MultiplicationY(double lfFactor)
	{
		for (size_t i=0; i<m_vYData.size(); ++i)
		{
			m_vYData[i] = m_vYData[i] * lfFactor;	// 모든 데이터에 곱한다.  [8/30/2013 ChoiJunHyeok]
		}
	}
};

// 그래프 1개의 데이터 [7/2/2013 ChoiJunHyeok]
class CXGraphData
{
public:
	CXGraphData(){};
	~CXGraphData()
	{
		m_vSeriesData.clear();
	};
protected:
	vector<CXSeriesData*> m_vSeriesData;
public:
	vector<CXSeriesData*> GetSeriesData() const { return m_vSeriesData; }
	void PushSeriesData(CXSeriesData* _pData) { m_vSeriesData.push_back(_pData); }
	void SetSeriesData(vector<CXSeriesData*> val) { m_vSeriesData = val; }
};
