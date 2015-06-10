#pragma once
#include "resource1.h"
#include "../XGraph/XGraphDataSerie.h"
#include <vector>
#include <algorithm>

using namespace std;
// CGraphTrendLineToolDlg 대화 상자입니다.
class CXGraph;

class CGraphTrendLineToolDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGraphTrendLineToolDlg)

public:
	CGraphTrendLineToolDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CGraphTrendLineToolDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TRENDLINE_TOOL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	void RefreshGroupCount();	// 그룹 카운트 다시 [11/19/2013 ChoiJunHyeok]

	void	SetGraph(CXGraph* pGraph){m_pGraph = pGraph;}

	void	MakeTrendMain(double fTimeStart, double fTimeEnd);
	void	MakeTrendPoly(int iDec);
	void	MakeTrend2ndOrder();

protected:
	CXTPComboBox m_cboRange;
	CXTPComboBox m_cboMode;
	CXTPEdit m_edtFreq;
	CXTPEdit m_edtDamp;
	CXTPEdit m_edtDisplace;
	CXTPEdit m_edtVelo;
	CXTPButton m_btnApply;
	CXTPButton m_btnClose;

	CXGraph*	m_pGraph;
	int			m_iMaxDataIdx;
	vector<TDataPoint> m_vecPoints;
	double		m_fTimeStart;
	double		m_fTimeEnd;
	CPolynomialSolver m_PS;

public:
	UINT m_nGroupCount;		// 총 존재하는 그룹 개수  [11/15/2013 ChoiJunHyeok]

public:
	afx_msg void OnCbnSelchangeCboSoRange();
	afx_msg void OnCbnSelchangeCboSoMode();
};

class diff_function{
public:
	virtual double operator()(double time, vector<double> *param) = 0;
};

class ode_solve{
private:
	char    _termination_code;
public:
	bool solve(double start_point, double end_point, int point_number, vector<double> initial, vector<diff_function*> *function, vector<vector<double> > *result);
	virtual bool preprocessing(double time, std::vector<double>* initial, std::vector<double> *approximate);
	virtual bool postprocessing(double time, std::vector<double>* initial, std::vector<double> *approximate);
	char get_termination_code(){ return _termination_code; };

};

class diff_t1 : public diff_function{
public:
	double operator()(double time, std::vector<double>* p){
	    return (*p)[1];
	}
};

class diff_t2 : public diff_function{
public:
    double operator()(double time, std::vector<double>* p)
	{
		double fPi = 3.141592;
         return -4*fPi*fPi*fFreq*fFreq*(*p)[0] - 4*fPi*fDamping*fFreq*(*p)[1];
     }

public:
	double fFreq;
	double fDamping;
};