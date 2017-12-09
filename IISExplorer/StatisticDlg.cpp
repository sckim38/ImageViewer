// StatisticDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "IISExplorer.h"
#include "StatisticDlg.h"
#include "afxdialogex.h"
#include <string.hpp>

// CStatisticDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CStatisticDlg, CDialogEx)

CStatisticDlg::CStatisticDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStatisticDlg::IDD, pParent)
{

}

CStatisticDlg::~CStatisticDlg()
{
}

void CStatisticDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT1, m_statisticText);
}


BEGIN_MESSAGE_MAP(CStatisticDlg, CDialogEx)
END_MESSAGE_MAP()


// CStatisticDlg �޽��� ó�����Դϴ�.


BOOL CStatisticDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

    m_statisticText.SetWindowText( L"// CStatisticDlg ��ȭ �����Դϴ�.\r\n\
        \r\n\
        IMPLEMENT_DYNAMIC(CStatisticDlg, CDialogEx)\r\n\
        \r\n\
        CStatisticDlg::CStatisticDlg(CWnd* pParent )\r\n\
        : CDialogEx(CStatisticDlg::IDD, pParent)\r\n\
    {\r\n\
        \r\n\
    }\r\n\
    \r\n\
    CStatisticDlg::~CStatisticDlg()\r\n\
    {\r\n\
    }\r\n\
    ");

    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

#include <iostream>
#include <string>
#include <set>
#include <tuple>

struct S {
    int n;
    std::string s;
    float d;
    bool operator<(const S& rhs) const
    {
        // compares n to rhs.n,
        // then s to rhs.s,
        // then d to rhs.d
        return std::tie(n, s, d) < std::tie(rhs.n, rhs.s, rhs.d);
    }
};

void tie_test()
{
    std::set<S> set_of_s; // S is LessThanComparable

    S value{ 42, "Test", 3.14F };
    std::set<S>::iterator iter;
    bool inserted;

    // unpacks the return value of insert into iter and inserted
    std::tie(iter, inserted) = set_of_s.insert(value);

    if (inserted)
        std::cout << "Value was inserted successfully\n";
}
