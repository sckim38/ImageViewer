#pragma once

class GridLayout
{
public:
    GridLayout(INT row = 1, INT col = 1)
        : m_items(row, std::vector<LayoutCell>(col))
    {
        //http://www.naver.com
    }
    LayoutCell *HitTest(POINT point)
    {
        for (auto &items : m_items)
            for (auto &item : items)
                if (item.HitTest(point))
                    return &item;
        return nullptr;
    }

    void RecalcLayout(INT width, INT height)
    {
        CSize size(LONG(width / m_items[0].size()), LONG(height / m_items.size()));

        for (INT i = 0; i < m_items.size(); i++)
            for (INT j = 0; j < m_items[0].size(); j++)
                m_items[i][j].SetLayout(size.cx * j, size.cy * i, size.cx, size.cy);
    }

public:
    std::vector<std::vector<LayoutCell>> m_items;
};

