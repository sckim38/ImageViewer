// ImageTool.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ImageTool.h"
#include "MainFrm.h"
#include "ImageToolDoc.h"
#include "ImageToolView.h"
#include "string.hpp"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ULONG_PTR gdiplusToken;

/////////////////////////////////////////////////////////////////////////////
// CImageToolApp

BEGIN_MESSAGE_MAP(CImageToolApp, CWinAppEx)
	//{{AFX_MSG_MAP(CImageToolApp)
	//}}AFX_MSG_MAP
	ON_COMMAND( ID_FILE_NEW,         CWinAppEx::OnFileNew        )
	ON_COMMAND( ID_FILE_OPEN,        CWinAppEx::OnFileOpen       )
	ON_COMMAND( ID_FILE_PRINT_SETUP, CWinAppEx::OnFilePrintSetup )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageToolApp construction
CImageToolApp::CImageToolApp()
{
    parse_utility::TokenTest();
    std::string time_str = time_helper::GetCurrentDateTime();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CImageToolApp object
CImageToolApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CImageToolApp initialization
BOOL CImageToolApp::InitInstance()
{
	AfxEnableControlContainer();

//begin: initialize GDI+  
	GdiplusStartupInput gdiplusStartupInput;
    VERIFY(GdiplusStartup( &gdiplusToken, &gdiplusStartupInput, NULL ) == Ok );
//end: initialize GDI+

//#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
//#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
//#endif

	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();

    m_haccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

    InitContextMenuManager();

    InitKeyboardManager();

    InitTooltipManager();
    CMFCToolTipInfo ttParams;
    ttParams.m_bVislManagerTheme = TRUE;
    theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
        RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate( IDR_MAINFRAME,
		                                   RUNTIME_CLASS(CImageToolDoc),
										   RUNTIME_CLASS(CMainFrame),
										   RUNTIME_CLASS(CImageToolView) );
	AddDocTemplate(pDocTemplate);

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (!ProcessShellCommand(cmdInfo))
	{
		return FALSE;
	}

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

int CImageToolApp::ExitInstance() 
{
//begin: shutdown GDI+ 
	GdiplusShutdown(gdiplusToken);
//end: shutdown GDI+
	
	return CWinAppEx::ExitInstance();
}


BOOL CImageToolApp::ProcessMessageFilter(int code, LPMSG lpMsg)
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    if (m_haccel)
    {
        if (::TranslateAccelerator(m_pMainWnd->m_hWnd, m_haccel, lpMsg))
            return(TRUE);
    }

    return CWinAppEx::ProcessMessageFilter(code, lpMsg);
}
