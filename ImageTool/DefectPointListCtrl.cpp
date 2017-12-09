// DefectPointListCtrl.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ImageTool.h"
#include "DefectPointListCtrl.h"


// CDefectPointListCtrl

IMPLEMENT_DYNAMIC(CDefectPointListCtrl, CMFCListCtrl)

CDefectPointListCtrl::CDefectPointListCtrl()
{

}

CDefectPointListCtrl::~CDefectPointListCtrl()
{
}


BEGIN_MESSAGE_MAP(CDefectPointListCtrl, CMFCListCtrl)
    ON_WM_PAINT()
END_MESSAGE_MAP()



// CDefectPointListCtrl �޽��� ó�����Դϴ�.




void CDefectPointListCtrl::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
    // �׸��� �޽����� ���ؼ��� CMFCListCtrl::OnPaint()��(��) ȣ������ ���ʽÿ�.

    {
        Default();
        if (GetItemCount() == 0)
        {
            COLORREF clrText = ::GetSysColor(COLOR_WINDOWTEXT);
            COLORREF clrTextBk = ::GetSysColor(COLOR_WINDOW);

            CDC* pDC = GetDC();
            // Save dc state
            int nSavedDC = pDC->SaveDC();

            CRect rc;
            GetWindowRect(&rc);
            ScreenToClient(&rc);

            CMFCHeaderCtrl* pHC;
            pHC = &GetHeaderCtrl();
            if (pHC != NULL)
            {
                CRect rcH;
                pHC->GetItemRect(0, &rcH);
                rc.top += rcH.bottom;
            }
            rc.top += 10;

            CString strText(L"There are no items to show in this view. "); // The message you want!

            pDC->SetTextColor(clrText);
            pDC->SetBkColor(clrTextBk);
            pDC->FillRect(rc, &CBrush(clrTextBk));
            pDC->SelectStockObject(ANSI_VAR_FONT);
            pDC->DrawText(strText, -1, rc,
                DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP);

            // Restore dc
            pDC->RestoreDC(nSavedDC);
            ReleaseDC(pDC);
        }
        // Do not call CListCtrl::OnPaint() for painting messages
    }
}
