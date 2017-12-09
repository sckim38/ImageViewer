// ImageToolView.cpp : implementation of the CImageToolView class
//

#include "stdafx.h"
#include "ImageTool.h"
#include "ImageToolDoc.h"
#include "ImageToolView.h"
#include "MainFrm.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif

/////////////////////////////////////////////////////////////////////////////
// CImageToolView
IMPLEMENT_DYNCREATE(CImageToolView, CListView)

BEGIN_MESSAGE_MAP(CImageToolView, CListView)
	//{{AFX_MSG_MAP(CImageToolView)
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT( LVN_ITEMCHANGED,       OnItemchanged                 )
	ON_COMMAND(        ID_FILE_PRINT,         CListView::OnFilePrint        )
	ON_COMMAND(        ID_FILE_PRINT_DIRECT,  CListView::OnFilePrint        )
	ON_COMMAND(        ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageToolView construction/destruction
CImageToolView::CImageToolView()
{
}

CImageToolView::~CImageToolView()
{
    CImageToolDoc  *pDoc = GetDocument();

    pDoc->m_DrawingTasks.cancel();
    // Wait for any existing tasks to finish.
    pDoc->m_DrawingTasks.wait();
}

BOOL CImageToolView::PreCreateWindow( CREATESTRUCT& cs )
{
	return CListView::PreCreateWindow( cs );
}

/////////////////////////////////////////////////////////////////////////////
// CImageToolView drawing
void CImageToolView::OnDraw(CDC* pDC)
{
	CImageToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	BOOL   bPrinting = pDC->IsPrinting();
    Gdiplus::Bitmap *imaView = pDoc->GetImage();

	if( imaView && imaView->GetFlags() != 0 )
	{
		if( bPrinting )
		{
			int cxPage = pDC->GetDeviceCaps(HORZRES);
			int cyPage = pDC->GetDeviceCaps(VERTRES);
		
			int cxInch = pDC->GetDeviceCaps(LOGPIXELSX);
			int cyInch = pDC->GetDeviceCaps(LOGPIXELSY);

			int cx = cxPage-200;
			int cy = (int)(((double)imaView->GetHeight()*cyInch*cx)
						  /((double)imaView->GetWidth()*cxInch));
		
			Graphics graphics(pDC->GetSafeHdc());
			graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
			graphics.DrawImage(imaView, Rect(100, 100, cx, cy));
		}
	}
}

void CImageToolView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	CListCtrl& ListCtrl = GetListCtrl();

	m_ImageListThumb.Create( THUMBNAIL_WIDTH,
		                     THUMBNAIL_HEIGHT,
							 ILC_COLOR32,
							 0,
							 1 );

	ListCtrl.SetImageList( &m_ImageListThumb, LVSIL_NORMAL );

	m_previewDlg.Create( IDD_PREVIEW_DLG );
	m_previewDlg.SetWindowPos( &wndTopMost,
								 20, 320,
								 500, 430,
								 SWP_DRAWFRAME );

	m_foldersDlg.Create( IDD_BROWSER_DLG );
	m_foldersDlg.SetWindowPos( &wndTopMost,
								 20, 10,
								 200, 300,
								 SWP_DRAWFRAME );
}

/////////////////////////////////////////////////////////////////////////////
// CImageToolView printing
BOOL CImageToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->SetMaxPage(1);
	pInfo->m_bContinuePrinting = FALSE;

	return DoPreparePrinting( pInfo );
}

void CImageToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* pInfo )
{
}

void CImageToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* pInfo)
{
}

/////////////////////////////////////////////////////////////////////////////
// CImageToolView diagnostics
#ifdef _DEBUG
void CImageToolView::AssertValid() const
{
	CListView::AssertValid();
}

void CImageToolView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CImageToolDoc* CImageToolView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageToolDoc)));
	return (CImageToolDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImageToolView message handlers
BOOL CImageToolView::Create( LPCTSTR        lpszClassName, 
							 LPCTSTR        lpszWindowName, 
							 DWORD          dwStyle, 
							 const RECT&    rect, 
							 CWnd           *pParentWnd,
							 UINT           nID, 
							 CCreateContext *pContext) 
{
	return CListView::Create( lpszClassName, 
		                      _T("ListView"),
							  dwStyle           | 
							  LVS_SHOWSELALWAYS | 
							  LVS_ALIGNTOP      |
							  LVS_ICON          |
							  LVS_SINGLESEL     |
							  LVS_AUTOARRANGE, 
							  rect, 
							  pParentWnd, 
							  nID,
							  pContext );
}


struct elapse_time
{
    ~elapse_time()
    {
        DWORD elapsed = GetTickCount() - start_;
        ATLTRACE(L"%s:%5.3f\n", message_.c_str(), elapsed / 1000.F);
    }
    std::wstring message_;
    DWORD start_ = GetTickCount();
};

struct load_image_time : elapse_time
{
    load_image_time()
    {
        message_ = L"load Image time";
    }
};

//_AFX_D2D_STATE* pD2DState = AfxGetD2DState();
//
//ID2D1Factory* pDirect2dFactory = pD2DState->GetDirect2dFactory();
//IDWriteFactory* pDirectWriteFactory = pD2DState->GetWriteFactory();
//IWICImagingFactory* pImagingFactory = pD2DState->GetWICFactory();

void CImageToolView::LoadThumbnail2(int nIndex, const CString &path)
{
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    ATLENSURE_SUCCEEDED(hr);

    auto bitmap = GdiplusHelper::LoadGdiplusBitmap(path, THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT);

    INT destX = (int)((THUMBNAIL_WIDTH - bitmap->GetWidth()) / 2);
    INT destY = (int)((THUMBNAIL_HEIGHT - bitmap->GetHeight()) / 2);

    Gdiplus::Bitmap  *bmPhoto = NULL;
    bmPhoto = new Gdiplus::Bitmap(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, PixelFormat24bppRGB);
    bmPhoto->SetResolution(bitmap->GetHorizontalResolution(), bitmap->GetVerticalResolution());

    Graphics *grPhoto = Graphics::FromImage(bmPhoto);
    Gdiplus::Color color(255, 255, 255, 255);
    grPhoto->Clear(color);
    grPhoto->SetInterpolationMode(InterpolationModeNearestNeighbor);
    //grPhoto->SetInterpolationMode(InterpolationModeHighQualityBicubic);
    {
        grPhoto->DrawImage(bitmap.get(), Rect(destX, destY, bitmap->GetWidth(), bitmap->GetHeight()));
    }

    HBITMAP hbmReturn = nullptr;//WIC::PictureHelper::CreateHBITMAP(source.Get());
    bmPhoto->GetHBITMAP(color, &hbmReturn);

    CBitmap Bmp1;
    Bmp1.Attach(hbmReturn);
    m_ImageListThumb.Replace(nIndex, &Bmp1, NULL);

    delete grPhoto;
    delete bmPhoto;
    Bmp1.Detach();
    DeleteObject(hbmReturn);

    CoUninitialize();
}

void CImageToolView::OnItemchanged( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	CImageToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->SelectItem( pNMListView->iItem );
	*pResult = 0;
}

void CImageToolView::LoadThumbnailImages()
{
    int nIndex = 0;
    std::vector<CString>::iterator iter;

    CImageToolDoc  *pDoc = GetDocument();

    CListCtrl& ListCtrl = GetListCtrl();
    CImageList *pImgList = &m_ImageListThumb;

    int iImageCount = pImgList->GetImageCount();

    for (int i = 0; i < iImageCount; i++)
    {
        pImgList->Remove(i);
    }

    ListCtrl.DeleteAllItems();
    pImgList->SetImageCount((UINT)pDoc->m_vFileName.size());

    ListCtrl.SetRedraw(FALSE);

    auto &values = pDoc->m_vFileName;

    for (int i = 0; i < values.size(); i++)
    {
        ListCtrl.InsertItem(i, values[i], i);
    }
    //for(int i=0;i<(INT)values.size();i++)
    //{
    //    CString path; path.Empty();
    //    path.Format(L"%s\\%s", pDoc->m_strCurrentDirectory, values[i]);

    //    LoadThumbnail2(i, path);
    //}
    //CImage *pImage;
    concurrency::parallel_for(0, (INT)values.size(), [pDoc, this, &values](int i){
        CString path; path.Empty();
        path.Format(L"%s\\%s", pDoc->m_strCurrentDirectory, values[i]);

        LoadThumbnail2(i, path);
    });

    ListCtrl.SetRedraw(TRUE);
    ListCtrl.Invalidate();

}


