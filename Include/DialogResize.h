#pragma once
// use MFC

class CDialogResize : public CDialogEx
{
public:
// Data declarations and members
	enum
	{
		DLSZ_SIZE_X		    = 0x00000001,
		DLSZ_SIZE_Y		    = 0x00000002,
		DLSZ_MOVE_X		    = 0x00000004,
		DLSZ_MOVE_Y		    = 0x00000008,
		DLSZ_REPAINT	    = 0x00000010,
		DLSZ_CENTER_X	    = 0x00000020,
		DLSZ_CENTER_Y	    = 0x00000040,
	};

	struct _AtlDlgResizeMap
	{
		int m_nCtlID;
		DWORD m_dwResizeFlags;
	};

    struct DlgResizeData
	{
		int m_nCtlID;
		DWORD m_dwResizeFlags;
		RECT m_rect;

		int GetGroupCount() const
		{
			return (int)LOBYTE(HIWORD(m_dwResizeFlags));
		}

		void SetGroupCount(int nCount)
		{
			ATLASSERT(nCount > 0 && nCount < 256);
			DWORD dwCount = (DWORD)MAKELONG(0, MAKEWORD(nCount, 0));
			m_dwResizeFlags &= 0xFF00FFFF;
			m_dwResizeFlags |= dwCount;
		}

        bool operator ==(const DlgResizeData& r) const
		{ return (m_nCtlID == r.m_nCtlID && m_dwResizeFlags == r.m_dwResizeFlags); }
	};

	//ATL::CSimpleArray<_AtlDlgResizeData> m_arrData;
    std::vector<std::vector<DlgResizeData>> m_data;

    CSize m_sizeDialog;
    CSize m_sizeMinTrack;
	//POINT m_ptMinTrackSize;

// Constructor
    template<typename ...Args>
	CDialogResize(Args ...args)
        : CDialogEx(args...)
	{
	}

	~CDialogResize()
	{
	}

// Operations
	void DlgResize_Init(
        bool bUseMinTrackSize = true,
        DWORD dwForceStyle = WS_CLIPCHILDREN)
	{
		ATLASSERT(::IsWindow(m_hWnd));

		DWORD dwStyle = GetStyle();

#ifdef _DEBUG
		// Debug only: Check if top level dialogs have a resizeable border.
		if(((dwStyle & WS_CHILD) == 0) && ((dwStyle & WS_THICKFRAME) == 0))
			ATLTRACE2(_T("DlgResize_Init - warning: top level dialog without the WS_THICKFRAME style - user cannot resize it\n"));//ATLTRACE2(atlTraceUI, 0, _T("DlgResize_Init - warning: top level dialog without the WS_THICKFRAME style - user cannot resize it\n"));
#endif // _DEBUG

		// Force specified styles (default WS_CLIPCHILDREN reduces flicker)
		if((dwStyle & dwForceStyle) != dwForceStyle)
			ModifyStyle(0, dwForceStyle);

		// Adding this style removes an empty icon that dialogs with WS_THICKFRAME have.
		// Setting icon to NULL is required when XP themes are active.
		// Note: This will not prevent adding an icon for the dialog using SetIcon()
		if((dwStyle & WS_CHILD) == 0)
		{
			ModifyStyleEx(0, WS_EX_DLGMODALFRAME);
			if (GetIcon(FALSE) == NULL)
				SetIcon(NULL, FALSE);
		}

		// clear out everything else
        m_sizeDialog = CSize();
        m_sizeMinTrack = CSize();

        // Get initial dialog client size
        {
            CRect rect;
            GetClientRect(&rect);
            m_sizeDialog = rect.Size();
        }

		// Get min track position if requested
        if (bUseMinTrackSize)
        {
            if ((dwStyle & WS_CHILD) != 0)
            {
                CRect rect = { 0 };
                GetClientRect(rect);
                m_sizeMinTrack = rect.Size();
            }
            else
            {
                CRect rect = { 0 };
                GetWindowRect(rect);
                m_sizeMinTrack = rect.Size();
            }
        }

		// Walk the map and initialize data
        for (auto &items : m_data)
            for (auto &item : items)
            {
                CWnd* pCtrl = GetDlgItem(item.m_nCtlID);
                ATLASSERT(pCtrl != NULL);
                pCtrl->GetWindowRect(&item.m_rect);
                ::MapWindowPoints(NULL, m_hWnd, (LPPOINT)&item.m_rect, 2);
            }
	}

	void DlgResize_UpdateLayout(int cxWidth, int cyHeight)
	{
		ATLASSERT(::IsWindow(m_hWnd));

		// Restrict minimum size if requested
        if (m_sizeDialog.cx > 0 && m_sizeDialog.cy > 0)
        {
            cxWidth = (std::max)(cxWidth, (INT)m_sizeDialog.cx);
            cyHeight = (std::max)(cyHeight, (INT)m_sizeDialog.cy);
        }

		BOOL bVisible = IsWindowVisible();
		if(bVisible)
			SetRedraw(FALSE);

		for(auto &items:m_data)
		{
            if (items.size() > 1)
            {
                RECT rectGroup{ 0 };

                for (auto &item : items)
                {
                    rectGroup.left = (std::min)(rectGroup.left, item.m_rect.left);
                    rectGroup.top = (std::min)(rectGroup.top, item.m_rect.top);
                    rectGroup.right = (std::max)(rectGroup.right, item.m_rect.right);
                    rectGroup.bottom = (std::max)(rectGroup.bottom, item.m_rect.bottom);
                }

                DlgResizeData* pDataPrev = nullptr;
                for (auto &item : items)
                {
                    DlgResize_PositionControl(cxWidth, cyHeight, rectGroup, item, true, pDataPrev);
                    pDataPrev = &item;
                }
            }
			else // one control entry
			{
                DlgResize_PositionControl(cxWidth, cyHeight, RECT{ 0 }, items[0], false);
			}
		}

		if(bVisible)
			SetRedraw(TRUE);

        RedrawWindow(nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
	}

	void OnSize(UINT nType, INT cx, INT cy)
	{
        if (nType != SIZE_MINIMIZED)
		{
			ATLASSERT(::IsWindow(m_hWnd));
            DlgResize_UpdateLayout(cx, cy);// GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		}
	}

    LRESULT OnGetMinMaxInfo(MINMAXINFO* lpMMI)
	{
        if (m_sizeMinTrack.cx != -1 && m_sizeMinTrack.cy != -1)
		{
            lpMMI->ptMinTrackSize = CPoint(m_sizeMinTrack);
		}
		return 0;
	}

    void SetSize(INT cx, INT cy)
    {
        m_sizeMinTrack.cx = cx;
        m_sizeMinTrack.cy = cy;

        SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
    }

// Implementation
	bool DlgResize_PositionControl(
        int cxWidth,
        int cyHeight,
        RECT& rectGroup,
        DlgResizeData& data,
        bool bGroup,
        DlgResizeData* pDataPrev = NULL)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		CWnd* pCtrl; //ATL::CWindow ctl;
		RECT rectCtl = { 0 };

		pCtrl = GetDlgItem(data.m_nCtlID);
		pCtrl->GetWindowRect(&rectCtl);
		if(((rectCtl.bottom == 0)&(rectCtl.top == 0)&(rectCtl.left == 0)&(rectCtl.right == 0)) ) return false;
		::MapWindowPoints(NULL, m_hWnd, (LPPOINT)&rectCtl, 2);

		if(bGroup)
		{
			if((data.m_dwResizeFlags & DLSZ_CENTER_X) != 0)
			{
				int cxRight = rectGroup.right + cxWidth - m_sizeDialog.cx;
				int cxCtl = data.m_rect.right - data.m_rect.left;
				rectCtl.left = rectGroup.left + (cxRight - rectGroup.left - cxCtl) / 2;
				rectCtl.right = rectCtl.left + cxCtl;
			}
			else if((data.m_dwResizeFlags & (DLSZ_SIZE_X | DLSZ_MOVE_X)) != 0)
			{
				rectCtl.left = rectGroup.left + ::MulDiv(data.m_rect.left - rectGroup.left, rectGroup.right - rectGroup.left + (cxWidth - m_sizeDialog.cx), rectGroup.right - rectGroup.left);

				if((data.m_dwResizeFlags & DLSZ_SIZE_X) != 0)
				{
					rectCtl.right = rectGroup.left + ::MulDiv(data.m_rect.right - rectGroup.left, rectGroup.right - rectGroup.left + (cxWidth - m_sizeDialog.cx), rectGroup.right - rectGroup.left);

					if(pDataPrev != NULL)
					{
						CWnd* pctlPrev = GetDlgItem(pDataPrev->m_nCtlID);
						RECT rcPrev = { 0 };
						pctlPrev->GetWindowRect(&rcPrev);
						::MapWindowPoints(NULL, m_hWnd, (LPPOINT)&rcPrev, 2);
						int dxAdjust = (rectCtl.left - rcPrev.right) - (data.m_rect.left - pDataPrev->m_rect.right);
						rcPrev.right += dxAdjust;
						pctlPrev->SetWindowPos(NULL, rcPrev.left, rcPrev.top, (rcPrev.right-rcPrev.left), (rcPrev.bottom-rcPrev.top), SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE); //SetWindowPos(NULL, &rcPrev, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
					}
				}
				else
				{
					rectCtl.right = rectCtl.left + (data.m_rect.right - data.m_rect.left);
				}
			}

			if((data.m_dwResizeFlags & DLSZ_CENTER_Y) != 0)
			{
				int cyBottom = rectGroup.bottom + cyHeight - m_sizeDialog.cy;
				int cyCtl = data.m_rect.bottom - data.m_rect.top;
				rectCtl.top = rectGroup.top + (cyBottom - rectGroup.top - cyCtl) / 2;
				rectCtl.bottom = rectCtl.top + cyCtl;
			}
			else if((data.m_dwResizeFlags & (DLSZ_SIZE_Y | DLSZ_MOVE_Y)) != 0)
			{
				rectCtl.top = rectGroup.top + ::MulDiv(data.m_rect.top - rectGroup.top, rectGroup.bottom - rectGroup.top + (cyHeight - m_sizeDialog.cy), rectGroup.bottom - rectGroup.top);

				if((data.m_dwResizeFlags & DLSZ_SIZE_Y) != 0)
				{
					rectCtl.bottom = rectGroup.top + ::MulDiv(data.m_rect.bottom - rectGroup.top, rectGroup.bottom - rectGroup.top + (cyHeight - m_sizeDialog.cy), rectGroup.bottom - rectGroup.top);

					if(pDataPrev != NULL)
					{
						CWnd* pctlPrev = GetDlgItem(pDataPrev->m_nCtlID);
						RECT rcPrev = { 0 };
						pctlPrev->GetWindowRect(&rcPrev);
						::MapWindowPoints(NULL, m_hWnd, (LPPOINT)&rcPrev, 2);
						int dxAdjust = (rectCtl.top - rcPrev.bottom) - (data.m_rect.top - pDataPrev->m_rect.bottom);
						rcPrev.bottom += dxAdjust;
						pctlPrev->SetWindowPos(NULL, rcPrev.left, rcPrev.top, (rcPrev.right-rcPrev.left), (rcPrev.bottom-rcPrev.top), SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);//ctlPrev.SetWindowPos(NULL, &rcPrev, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
					}
				}
				else
				{
					rectCtl.bottom = rectCtl.top + (data.m_rect.bottom - data.m_rect.top);
				}
			}
		}
		else // no group
		{
			if((data.m_dwResizeFlags & DLSZ_CENTER_X) != 0)
			{
				int cxCtl = data.m_rect.right - data.m_rect.left;
				rectCtl.left = (cxWidth - cxCtl) / 2;
				rectCtl.right = rectCtl.left + cxCtl;
			}
			else if((data.m_dwResizeFlags & (DLSZ_SIZE_X | DLSZ_MOVE_X)) != 0)
			{
				rectCtl.right = data.m_rect.right + (cxWidth - m_sizeDialog.cx);

				if((data.m_dwResizeFlags & DLSZ_MOVE_X) != 0)
					rectCtl.left = rectCtl.right - (data.m_rect.right - data.m_rect.left);
			}

			if((data.m_dwResizeFlags & DLSZ_CENTER_Y) != 0)
			{
				int cyCtl = data.m_rect.bottom - data.m_rect.top;
				rectCtl.top = (cyHeight - cyCtl) / 2;
				rectCtl.bottom = rectCtl.top + cyCtl;
			}
			else if((data.m_dwResizeFlags & (DLSZ_SIZE_Y | DLSZ_MOVE_Y)) != 0)
			{
				rectCtl.bottom = data.m_rect.bottom + (cyHeight - m_sizeDialog.cy);

				if((data.m_dwResizeFlags & DLSZ_MOVE_Y) != 0)
					rectCtl.top = rectCtl.bottom - (data.m_rect.bottom - data.m_rect.top);
			}
		}

		if((data.m_dwResizeFlags & DLSZ_REPAINT) != 0)
			pCtrl->Invalidate(); //ctl.Invalidate();

		if((data.m_dwResizeFlags & (DLSZ_SIZE_X | DLSZ_SIZE_Y | DLSZ_MOVE_X | DLSZ_MOVE_Y | DLSZ_REPAINT | DLSZ_CENTER_X | DLSZ_CENTER_Y)) != 0)
			pCtrl->SetWindowPos(NULL, rectCtl.left, rectCtl.top, (rectCtl.right-rectCtl.left), (rectCtl.bottom-rectCtl.top), SWP_NOZORDER | SWP_NOACTIVATE);//ctl.SetWindowPos(NULL, &rectCtl, SWP_NOZORDER | SWP_NOACTIVATE);

		return true;
	}
};
