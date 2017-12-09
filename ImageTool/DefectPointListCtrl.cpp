// DefectPointListCtrl.cpp : 구현 파일입니다.
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



// CDefectPointListCtrl 메시지 처리기입니다.




void CDefectPointListCtrl::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
    // 그리기 메시지에 대해서는 CMFCListCtrl::OnPaint()을(를) 호출하지 마십시오.

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
