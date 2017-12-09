#pragma once

#include <array>

// CSelectMap 대화 상자입니다.
struct unit
{
    CRect rect;
    INT pcb = 0;
    bool enable = true;
};
using pcb_row = std::vector<unit>;
using pcb_map = std::vector<pcb_row>;

class CSelectMap : public CDialogResize
{
	DECLARE_DYNAMIC(CSelectMap)

public:
	CSelectMap(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSelectMap();

    void InitUnitMap(INT row, INT col)
    {
        m_pcb_map = pcb_map(row, pcb_row(col));
        RecalcLayout(CSize(20, 20));
    }
    void DrawUnitMap(ID2D1RenderTarget *pRT, RenderData &data)
    {
        ComPtr<ID2D1SolidColorBrush> pBrush;
        pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), pBrush.GetAddressOf());
        for (auto &row : m_pcb_map)
        {
            for (auto &col : row)
            {
                if (!col.enable)
                {
                    pRT->FillRectangle(
                        D2D1::RectF(col.rect),
                        pBrush.Get());
                }
                pRT->DrawRectangle(
                    D2D1::RectF(col.rect),
                    data.m_pLineBrush.Get());
            }
        }
        for (auto &quad : m_quad)
        {
            pRT->DrawRectangle(
                D2D1::RectF(quad),
                pBrush.Get());
        }
    }
    void SetLayout(CRect &rect, INT x, INT y, INT cx, INT cy)
    {
        rect.SetRect(x, y, x + cx, y + cy);
    }
    void SetQuad(INT index, CRect &rect1, CRect &rect2)
    {
        m_quad[index].left = rect1.left - 2;
        m_quad[index].top = rect1.top - 2;
        m_quad[index].right = rect2.right + 2;
        m_quad[index].bottom = rect2.bottom + 2;
    }
    void RecalcLayout(CSize size)
    {
        INT quad_col = INT(m_pcb_map[0].size() / 2);
        INT quad_row = INT(m_pcb_map.size() / 2);

        for (INT i = 0; i < m_pcb_map.size(); i++)
        {
            for (INT j = 0; j < m_pcb_map[0].size(); j++)
            {
                SetLayout(
                    m_pcb_map[i][j].rect,
                    (size.cx + m_margin.cx) * j + m_margin.cx * (j / quad_col + 1),
                    (size.cy + m_margin.cy) * i + m_margin.cy * (i / quad_row + 1),
                    size.cx,
                    size.cy);
            }
        }

        SetQuad(0, m_pcb_map[0][0].rect,                m_pcb_map[quad_row - 1][quad_col - 1].rect);
        SetQuad(1, m_pcb_map[0][quad_col].rect,         m_pcb_map[quad_row - 1][m_pcb_map[0].size() - 1].rect);
        SetQuad(2, m_pcb_map[quad_row][0].rect,         m_pcb_map[m_pcb_map.size() - 1][quad_col - 1].rect);
        SetQuad(3, m_pcb_map[quad_row][quad_col].rect,  m_pcb_map[m_pcb_map.size() - 1][m_pcb_map[0].size() - 1].rect);

        m_bound_rect = CSize(
            (size.cx + m_margin.cx) * m_row + m_margin.cx,
            (size.cy + m_margin.cy) * m_col + m_margin.cy);
    }

    struct Unitmap : CImageWnd
    {
        void OnRenderItem(ID2D1RenderTarget *pRT, LayoutCell &item)
        {
            ASSERT(pThis);
            pRT->Clear(m_data.background_color);
            pRT->SetTransform(item.matrix_);

            pThis->DrawUnitMap(pRT, m_data);
            item.RenderDrawObject(pRT, m_data);

            pRT->SetTransform(D2D1::Matrix3x2F::Identity());
        }
        CSelectMap* pThis = nullptr;
    } m_wndUnitMap;

    INT m_row = 1;
    INT m_col = 1;

    CSize m_margin = CSize(5, 5);
    CSize m_bound_rect;

    std::array<CRect, 4> m_quad;
    pcb_map m_pcb_map;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SELECT_UNIT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnBnClickedSelectMap();
    afx_msg void OnBnClickedDisablePcb();
    afx_msg void OnBnClickedEnablePcb();
};
