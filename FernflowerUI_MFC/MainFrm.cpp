
// MainFrm.cpp: CMainFrame 类的实现
//

#include "stdafx.h"
#include "FernflowerUI_MFC.h"
#include "CommonWrapper.h"
#include "MainFrm.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	//ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
	//ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
	//ON_COMMAND(ID_TOOLS_OPTIONS, &CMainFrame::OnOptions)
	ON_WM_SETTINGCHANGE()
	ON_WM_DROPFILES()
	ON_WM_CLOSE()
	ON_COMMAND(ID_ENABLE_RECOVERY, &CMainFrame::OnEnableRecovery)
	ON_UPDATE_COMMAND_UI(ID_ENABLE_RECOVERY, &CMainFrame::OnUpdateEnableRecovery)
	ON_COMMAND(ID_WINDOW_CLOSE, &CMainFrame::OnWindowClose)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CLOSE, &CMainFrame::OnUpdateWindowClose)
	ON_COMMAND(ID_QUICK_DECOMP, &CMainFrame::OnQuickDecomp)
	ON_UPDATE_COMMAND_UI(ID_QUICK_DECOMP, &CMainFrame::OnUpdateQuickDecomp)
	ON_COMMAND(ID_CLEAR_CACHE, &CMainFrame::OnClearCache)
	ON_UPDATE_COMMAND_UI(ID_CLEAR_CACHE, &CMainFrame::OnUpdateClearCache)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CMainFrame::OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, &CMainFrame::OnUpdateFileSaveAs)
	ON_COMMAND(ID_WINDOW_RESET, &CMainFrame::OnWindowReset)
	ON_COMMAND(ID_AUTO_SAVE, &CMainFrame::OnEnableAutoSave)
	ON_UPDATE_COMMAND_UI(ID_AUTO_SAVE, &CMainFrame::OnUpdateEnableAutoSave)
	ON_COMMAND(ID_DECOMP_OPINIONS, &CMainFrame::OnDecompOptions)
	ON_COMMAND(ID_IGNORE_CACHE, &CMainFrame::OnEnableIgnoreCache)
	ON_UPDATE_COMMAND_UI(ID_IGNORE_CACHE, &CMainFrame::OnUpdateIgnoreCache)
	//ON_COMMAND(ID_EDIT_FIND, &CMainFrame::OnEditFind)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame():IsShowingWindowManager(false)
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 其他可用样式...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // 设置为 FALSE 会将关闭按钮放置在选项卡区域的右侧
	mdiTabParams.m_bEnableTabSwap = TRUE;
	mdiTabParams.m_bTabIcons = FALSE;    // 设置为 TRUE 将在 MDI 选项卡上启用文档图标
	mdiTabParams.m_bAutoColor = TRUE;    // 设置为 FALSE 将禁用 MDI 选项卡的自动着色
	mdiTabParams.m_bDocumentMenu = TRUE; // 在选项卡区域的右边缘启用文档菜单
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// 允许用户定义的工具栏操作: 
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 创建标题栏: 
	if (!CreateCaptionBar())
	{
		TRACE0("未能创建标题栏\n");
		return -1;      // 未能创建
	}

	// 加载菜单项图像(不在任何标准工具栏上): 
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}

	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndFileView);
	CDockablePane* pTabbedBar = NULL;
	m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);
	m_wndProperties.SetAutoHideMode(TRUE, CBRS_ALIGN_RIGHT);
	m_wndFileView.ShowPane(true, false, true);
	RecalcLayout(FALSE);

	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	// 启用增强的窗口管理对话框
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// 启用工具栏和停靠窗口菜单替换
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// 加载用户定义的工具栏图像
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// 启用菜单个性化(最近使用的命令)
	// TODO: 定义您自己的基本命令，确保每个下拉菜单至少有一个基本命令。
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_SAVE_AS);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);
	lstBasicCommands.AddTail(ID_SELECT_FONT);
	lstBasicCommands.AddTail(ID_ENABLE_RECOVERY);
	lstBasicCommands.AddTail(ID_QUICK_DECOMP);
	lstBasicCommands.AddTail(ID_EDIT_COPY);
	lstBasicCommands.AddTail(ID_EDIT_FIND);
	lstBasicCommands.AddTail(ID_AUTO_SAVE);
	lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
	lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	// 将文档名和应用程序名称在窗口标题栏上的顺序进行交换。这
	// 将改进任务栏的可用性，因为显示的文档名带有缩略图。
	ModifyStyle(0, FWS_PREFIXTITLE);

	if (CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER, IID_ITaskbarList4, (void**)&m_pTaskBar) != S_OK)
	{
		TRACE0("未能创建COM组件");
		return -1;
	}
	else if (m_pTaskBar->HrInit() != S_OK)
	{
		TRACE0("未能初始化COM组件");
		return -1;
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{

	CRect rectScreen;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rectScreen, false);
	m_rectClassViewDef = CRect(0 * rectScreen.Width() / 1366, 71 * rectScreen.Height() / 728, 274 * rectScreen.Width() / 1366, 709 * rectScreen.Height() / 728);
	m_rectFileViewDef = CRect(0 * rectScreen.Width() / 1366, 71 * rectScreen.Height() / 728, 274 * rectScreen.Width() / 1366, 709 * rectScreen.Height() / 728);
	m_rectOutputDef = CRect(278 * rectScreen.Width() / 1366, 509 * rectScreen.Height() / 728, 1170 * rectScreen.Width() / 1366, 709 * rectScreen.Height() / 728);
	m_rectPropertyDef = CRect(1174 * rectScreen.Width() / 1366, 71 * rectScreen.Height() / 728, 1366 * rectScreen.Width() / 1366, 709 * rectScreen.Height() / 728);

	BOOL bNameValid;

	// 创建类视图
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);
	if (!m_wndClassView.Create(strClassView, this, m_rectClassViewDef, TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“类视图”窗口\n");
		return FALSE; // 未能创建
	}

	// 创建文件视图
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, m_rectFileViewDef, TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“文件视图”窗口\n");
		return FALSE; // 未能创建
	}

	// 创建输出窗口
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, m_rectOutputDef, TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建输出窗口\n");
		return FALSE; // 未能创建
	}

	// 创建属性窗口
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, m_rectPropertyDef, TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“属性”窗口\n");
		return FALSE; // 未能创建
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndFileView.SetIcon(hFileViewIcon, FALSE);

	HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndClassView.SetIcon(hClassViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

	UpdateMDITabbedBarsIcons();
}

BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("未能创建标题栏\n");
		return FALSE;
	}

	BOOL bNameValid;

	CString strTemp, strTemp2;
	/*bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);*/

	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 0, 0), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);
	m_wndCaptionBar.ShowWindow(SW_HIDE);

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnWindowManager()
{
	IsShowingWindowManager = true;
	ShowWindowsDialog();
	IsShowingWindowManager = false;
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	m_wndOutput.UpdateFonts();
	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnViewCaptionBar()
{
	//m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	BOOL bNameValid;

	CString strTemp, strTemp2;
	/*bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);*/

	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 0, 0), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);
	m_wndCaptionBar.ShowWindow(SW_SHOW);
	RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}

void CMainFrame::OnOptions()
{
}


BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	// 基类将执行真正的工作

	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CMDIFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}


void CMainFrame::OnDropFiles(HDROP DropFile)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int DropCount = DragQueryFile(DropFile, -1, NULL, 0);//取得被拖动文件的数目  
	if (DropCount == 1)
	{
		WCHAR wcStr[MAX_PATH];
		DragQueryFile(DropFile, 0, wcStr, MAX_PATH);//获得拖曳的第i个文件的文件名 
		theApp.DoDecomplie(wcStr);
	}
	else
	{
		if (theApp.Md5ofFile = L"")
		{
			WCHAR ThisPath[MAX_PATH];
			GetModuleFileName(nullptr, ThisPath, MAX_PATH);
			for (int i = 1; i < DropCount; i++)
			{
				WCHAR wcStr[MAX_PATH];
				DragQueryFile(DropFile, i, wcStr, MAX_PATH);//获得拖曳的第i个文件的文件名  
				ShellExecute(nullptr, L"open", ThisPath, wcStr, nullptr, SW_SHOW);
			}
			WCHAR wcStr[MAX_PATH];
			DragQueryFile(DropFile, 0, wcStr, MAX_PATH);//获得拖曳的第i个文件的文件名  
			theApp.DoDecomplie(wcStr);
		}
		else
		{
			WCHAR ThisPath[MAX_PATH];
			GetModuleFileName(nullptr, ThisPath, MAX_PATH);
			for (int i = 0; i < DropCount; i++)
			{
				WCHAR wcStr[MAX_PATH];
				DragQueryFile(DropFile, i, wcStr, MAX_PATH);//获得拖曳的第i个文件的文件名  
				ShellExecute(nullptr, L"open", ThisPath, wcStr, nullptr, SW_SHOW);
			}
		}
	}
	DragFinish(DropFile);
}


void CMainFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_pTaskBar->Release();
	if (theApp.Md5ofFile!=L"")
	{
		HWND hWndDecomplie = ::FindWindow(L"ConsoleWindowClass", CString(L"6168218c.FernflowerUI.Decomplie.") + theApp.Md5ofFile);
		::SendMessage(hWndDecomplie, WM_CLOSE, 0, 0);
		wchar_t * buf;
		size_t size;
		if (_wdupenv_s(&buf, &size, L"USERPROFILE"))
		{
			AfxGetMainWnd()->MessageBox(IsInChinese() ? _T("搜索%USERPROFILE%失败!") : _T("Failed to access %USERPROFILE%"), IsInChinese() ? _T("错误") : _T("Error"), MB_ICONERROR);
			return;
		}
		CStringW FernflowerPath = buf;
		free(buf);
		FernflowerPath += L"\\AppData\\Local\\FernFlowerUI\\Cache\\";
		FernflowerPath += theApp.Md5ofFile;
		FernflowerPath += L"\\fernflower" + theApp.Md5ofFile + L".jar";
		DeleteFile(FernflowerPath);
	}
	CMDIFrameWndEx::OnClose();
}


void CMainFrame::OnEnableRecovery()
{
	// TODO: 在此添加命令处理程序代码
	if (WaitForSingleObject(theApp.EnableRecovery, 0) != WAIT_TIMEOUT)
	{
		ResetEvent(theApp.EnableRecovery);
	}
	else
	{
		SetEvent(theApp.EnableRecovery);
	}
}


void CMainFrame::OnUpdateEnableRecovery(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(WaitForSingleObject(theApp.EnableRecovery, 0) != WAIT_TIMEOUT);
}


CMDIChildWndEx* CMainFrame::CreateDocumentWindow(LPCTSTR lpcszDocName, CObject* pObj)
{
	// TODO: 在此添加专用代码和/或调用基类
	POSITION Pos = theApp.GetFirstDocTemplatePosition();
	CChildFrame * pRet = DYNAMIC_DOWNCAST(CChildFrame, static_cast<CMultiDocTemplate*>(theApp.GetNextDocTemplate(Pos))->CreateNewFrame(nullptr, nullptr));
	if (pRet)
	{
		pRet->m_strTitle = lpcszDocName;
		pRet->InitialUpdateFrame(nullptr, true);
		this->UpdateWindow();
	}
	return pRet;
}


void CMainFrame::OnWindowClose()
{
	// TODO: 在此添加命令处理程序代码
	if (MDIGetActive() != nullptr)
	{
		MDIGetActive()->MDIDestroy();
	}
}


void CMainFrame::OnUpdateWindowClose(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (MDIGetActive() == nullptr)
	{
		pCmdUI->Enable(false);
	}
	else
	{
		pCmdUI->Enable();
	}
}


HMENU CMainFrame::GetWindowMenuPopup(HMENU hMenuBar)
{
	// TODO: 在此添加专用代码和/或调用基类
	return CMDIFrameWndEx::GetWindowMenuPopup(hMenuBar);
}


void CMainFrame::OnQuickDecomp()
{
	// TODO: 在此添加命令处理程序代码
	theApp.IsQuickDecomp = !theApp.IsQuickDecomp;
}


void CMainFrame::OnUpdateQuickDecomp(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(theApp.IsQuickDecomp);
}


void CMainFrame::OnClearCache()
{
	// TODO: 在此添加命令处理程序代码
	if (WaitForSingleObject(theApp.EvtIsToRmCache, 0) != WAIT_TIMEOUT)
	{
		ResetEvent(theApp.EvtIsToRmCache);
	}
	else
	{
		SetEvent(theApp.EvtIsToRmCache);
	}
	OnViewCaptionBar();
}


void CMainFrame::OnUpdateClearCache(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(WaitForSingleObject(theApp.EvtIsToRmCache, 0) != WAIT_TIMEOUT);
}


void CMainFrame::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (MDIGetActive())
	{
		pCmdUI->Enable();
	}
	else
	{
		pCmdUI->Enable(false);
	}
}


void CMainFrame::OnUpdateFileSaveAs(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (MDIGetActive())
	{
		pCmdUI->Enable();
	}
	else
	{
		pCmdUI->Enable(false);
	}
}


void CMainFrame::OnWindowReset()
{
	// TODO: 在此添加命令处理程序代码
	CRect rectScreen;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rectScreen, false);
	m_rectClassViewDef = CRect(0 * rectScreen.Width() / 1366, 71 * rectScreen.Height() / 728, 274 * rectScreen.Width() / 1366, 709 * rectScreen.Height() / 728);
	m_rectFileViewDef = CRect(0 * rectScreen.Width() / 1366, 71 * rectScreen.Height() / 728, 274 * rectScreen.Width() / 1366, 709 * rectScreen.Height() / 728);
	m_rectOutputDef = CRect(278 * rectScreen.Width() / 1366, 509 * rectScreen.Height() / 728, 1170 * rectScreen.Width() / 1366, 709 * rectScreen.Height() / 728);
	m_rectPropertyDef = CRect(1174 * rectScreen.Width() / 1366, 71 * rectScreen.Height() / 728, 1366 * rectScreen.Width() / 1366, 709 * rectScreen.Height() / 728);
	std::set<CMFCTabCtrl*> TabPaneSet;
	if (DYNAMIC_DOWNCAST(CMFCTabCtrl, m_wndClassView.GetParent()))
	{
		TabPaneSet.insert(DYNAMIC_DOWNCAST(CMFCTabCtrl, m_wndClassView.GetParent()));
	}
	if (DYNAMIC_DOWNCAST(CMFCTabCtrl, m_wndFileView.GetParent()))
	{
		TabPaneSet.insert(DYNAMIC_DOWNCAST(CMFCTabCtrl, m_wndFileView.GetParent()));
	}
	if (DYNAMIC_DOWNCAST(CMFCTabCtrl, m_wndOutput.GetParent()))
	{
		TabPaneSet.insert(DYNAMIC_DOWNCAST(CMFCTabCtrl, m_wndOutput.GetParent()));
	}
	if (DYNAMIC_DOWNCAST(CMFCTabCtrl, m_wndProperties.GetParent()))
	{
		TabPaneSet.insert(DYNAMIC_DOWNCAST(CMFCTabCtrl, m_wndOutput.GetParent()));
	}
	for (std::set<CMFCTabCtrl*>::iterator it = TabPaneSet.begin(); it != TabPaneSet.end(); it++)
	{
		(*it)->GetParent()->ShowWindow(SW_HIDE);
	}
	m_wndProperties.Slide(TRUE);
	m_wndProperties.SetAutoHideMode(FALSE, CBRS_ALIGN_ANY);
	m_wndClassView.UndockPane();
	m_wndFileView.UndockPane();
	m_wndOutput.UndockPane();
	m_wndProperties.UndockPane();
	m_wndClassView.SetWindowPos(0, m_rectClassViewDef.left, m_rectClassViewDef.top, m_rectClassViewDef.Width(), m_rectClassViewDef.Height(), SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	m_wndFileView.SetWindowPos(0, m_rectFileViewDef.left, m_rectFileViewDef.top, m_rectFileViewDef.Width(), m_rectFileViewDef.Height(), SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	m_wndOutput.SetWindowPos(0, m_rectOutputDef.left, m_rectOutputDef.top, m_rectOutputDef.Width(), m_rectOutputDef.Height(), SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	m_wndProperties.SetWindowPos(0, m_rectPropertyDef.left, m_rectPropertyDef.top, m_rectPropertyDef.Width(), m_rectPropertyDef.Height(), SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndFileView.SetPaneAlignment(CBRS_ALIGN_LEFT);
	m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndFileView);
	CDockablePane* pTabbedBar = NULL;
	m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	m_wndProperties.SetPaneAlignment(CBRS_ALIGN_RIGHT);
	DockPane(&m_wndProperties);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	m_wndOutput.SetPaneAlignment(CBRS_ALIGN_BOTTOM);
	DockPane(&m_wndOutput);
	m_wndProperties.SetAutoHideMode(TRUE, CBRS_ALIGN_RIGHT);
	m_wndProperties.Slide(FALSE);
	for (std::set<CMFCTabCtrl*>::iterator it = TabPaneSet.begin(); it != TabPaneSet.end(); it++)
	{
		(*it)->GetParent()->DestroyWindow();
	}
	if (theApp.Md5ofFile == L"")
	{
		m_wndFileView.ShowPane(true, false, true);
	}
	else
	{
		m_wndClassView.ShowPane(true, false, true);
	}
	RecalcLayout(FALSE);
	Invalidate();
}


void CMainFrame::OnEnableAutoSave()
{
	// TODO: 在此添加命令处理程序代码
	theApp.EnableAutoSave = !theApp.EnableAutoSave;
}


void CMainFrame::OnUpdateEnableAutoSave(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(theApp.EnableAutoSave);
}


void CMainFrame::OnDecompOptions()
{
	// TODO: 在此添加命令处理程序代码
	//CDecompOptionBox OptionBox;
	//OptionBox.DoModal();
	m_wndProperties.ShowPane(TRUE, FALSE, TRUE);
	RecalcLayout(FALSE);
}


void CMainFrame::OnEnableIgnoreCache()
{
	// TODO: 在此添加命令处理程序代码
	theApp.EnableIgnoreCache = !theApp.EnableIgnoreCache;
}


void CMainFrame::OnUpdateIgnoreCache(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(theApp.EnableIgnoreCache);
}

/*
void CMainFrame::OnEditFind()
{
	// TODO: 在此添加命令处理程序代码

}
*/
