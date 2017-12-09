#pragma once


// CDefectPointListCtrl

class CDefectPointListCtrl : public CMFCListCtrl
{
	DECLARE_DYNAMIC(CDefectPointListCtrl)

public:
	CDefectPointListCtrl();
	virtual ~CDefectPointListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnPaint();
};


