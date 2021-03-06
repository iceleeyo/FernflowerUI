
// ChildFrm.cpp: CChildFrame 类的实现
//

#include "stdafx.h"
#include "FernflowerUI_MFC.h"
#include "FernflowerUI_MFCView.h"
#include "CommonWrapper.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
	ON_WM_CREATE()
	ON_WM_MDIACTIVATE()
	ON_UPDATE_COMMAND_UI(ID_ENABLE_RECOVERY, &CChildFrame::OnUpdateEnableRecovery)
	ON_COMMAND(ID_ENABLE_RECOVERY, &CChildFrame::OnEnableRecovery)
	ON_WM_DESTROY()
	ON_COMMAND(ID_SELECT_FONT, &CChildFrame::OnSelectFont)
	ON_COMMAND(ID_QUICK_DECOMP, &CChildFrame::OnQuickDecomp)
	ON_UPDATE_COMMAND_UI(ID_QUICK_DECOMP, &CChildFrame::OnUpdateQuickDecomp)
	ON_COMMAND(ID_CLEAR_CACHE, &CChildFrame::OnClearCache)
	ON_UPDATE_COMMAND_UI(ID_CLEAR_CACHE, &CChildFrame::OnUpdateClearCache)
	ON_WM_DROPFILES()
	ON_REGISTERED_MESSAGE(WM_FINDSTRING,&CChildFrame::OnFindString)
	ON_COMMAND(ID_EDIT_FIND, &CChildFrame::OnEditFind)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()

// CChildFrame 构造/析构

CChildFrame::CChildFrame():m_pFindDialog(nullptr),IsDialogTransparented(FALSE)
{
	EnableActiveAccessibility();
	m_pFindDialog = nullptr;
	// TODO: 在此添加成员初始化代码
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	return CMDIChildWndEx::OnCreateClient(lpcs, pContext);
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式
	if (!CMDIChildWndEx::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}

// CChildFrame 诊断

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CChildFrame 消息处理程序


void CChildFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (m_strTitle != L"")
	{
		SetWindowText(m_strTitle);
		if (theApp.Flag == CFernflowerUIMFCApp::DecompFlags::DecompJar)
		{
			CommonWrapper::GetMainFrame()->SetWindowText(theApp.JarFilePath + L" => " + m_strTitle + _T(" - FernFlowerUI"));
		}
		else
		{
			CommonWrapper::GetMainFrame()->SetWindowText(m_strTitle + _T(" - FernFlowerUI"));
		}
	}
	else
	{
		CommonWrapper::GetMainFrame()->SetWindowText(L"FernFlowerUI");
	}
	//Calling base class' method is useless.
	//CMDIChildWndEx::OnUpdateFrameTitle(bAddToTitle);
}


int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}


void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CMDIChildWndEx::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	// TODO: 在此处添加消息处理程序代码
	if ((pDeactivateWnd == this) && (m_pFindDialog))
	{
		m_pFindDialog->ShowWindow(SW_HIDE);
	}
	if ((pActivateWnd == this) && (m_pFindDialog))
	{
		m_pFindDialog->ShowWindow(SW_SHOW);
	}
}


void CChildFrame::OnUpdateEnableRecovery(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(WaitForSingleObject(theApp.EnableRecovery,0)!=WAIT_TIMEOUT);
}


void CChildFrame::OnEnableRecovery()
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


void CChildFrame::OnDestroy()
{
	for (auto it = CommonWrapper::GetMainFrame()->m_MDIChildWndMap.begin(); it != CommonWrapper::GetMainFrame()->m_MDIChildWndMap.end(); it++)
	{
		if (it->second==this)
		{
			CommonWrapper::GetMainFrame()->m_MDIChildWndMap.erase(it);
			break;
		}
	}
	CMDIChildWndEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}


void CChildFrame::OnSelectFont()
{
	// TODO: 在此添加命令处理程序代码
	CHARFORMAT2 Format;
	static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetDefaultCharFormat(Format);
	CFontDialog FontSelect(Format);
	if (FontSelect.DoModal() == IDOK)
	{
		lstrcpy(Format.szFaceName, theApp.FontFaceName = FontSelect.GetFaceName());
		theApp.FontSize = (Format.yHeight = FontSelect.GetSize() * 2) / 20;
		Format.dwEffects = Format.dwEffects&(~CFE_BOLD);
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetSel(0, -1);
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetDefaultCharFormat(Format);
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetSelectionCharFormat(Format);
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.m_wndLineBox.GetDefaultCharFormat(Format);
//		lstrcpy(Format.szFaceName, theApp.FontFaceName = FontSelect.GetFaceName());
//		theApp.FontSize = (Format.yHeight = FontSelect.GetSize() * 2) / 20;
		Format.dwEffects = Format.dwEffects&(~CFE_BOLD);
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.m_wndLineBox.SetSel(0, -1);
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.m_wndLineBox.SetDefaultCharFormat(Format);
		CFernflowerUIMFCView * pView = static_cast<CFernflowerUIMFCView*>(GetActiveView());
		pView->FinishHighLight = false;
		AfxGetMainWnd()->BeginWaitCursor();
		CStringW Contact;
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetWindowTextW(Contact);
		std::future<void> SetHighLight = std::async(std::launch::async,[&](const CStringW & Str, CWnd * MainWnd) {
			pView->SetViewText(Str);
		}, Contact, AfxGetMainWnd());
		CommonWrapper::CProgressBar Progress(AfxGetMainWnd(),
			[&]()->int {AfxGetMainWnd()->RestoreWaitCursor(); return pView->FinishHighLight; },
			5, IsInChinese()?L"正在设置字体":L"Setting the new font", 0, 77);
		Progress.DoModal();
		pView->FinishHighLight = false;
		AfxGetMainWnd()->EndWaitCursor();
	}
}


void CChildFrame::OnQuickDecomp()
{
	// TODO: 在此添加命令处理程序代码
	theApp.IsQuickDecomp = !theApp.IsQuickDecomp;
}


void CChildFrame::OnUpdateQuickDecomp(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(theApp.IsQuickDecomp);
}


void CChildFrame::OnClearCache()
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
	CommonWrapper::GetMainFrame()->OnViewCaptionBar();
	OnUpdateFrameTitle(FALSE);
}


void CChildFrame::OnUpdateClearCache(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(WaitForSingleObject(theApp.EvtIsToRmCache, 0) != WAIT_TIMEOUT);
}


void CChildFrame::OnDropFiles(HDROP DropFile)
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
	DragFinish(DropFile);
}

LRESULT CChildFrame::OnFindString(WPARAM wParam, LPARAM lParam)
{
	auto Available = [](const FINDTEXTEX & Fnd)
	{return !((Fnd.chrgText.cpMin == -1) && (Fnd.chrgText.cpMax == -1)); };
	FINDREPLACE * pFind = reinterpret_cast<LPFINDREPLACE>(lParam);
	if (m_pFindDialog->IsTerminating())
	{
		m_pFindDialog->ShowWindow(SW_HIDE);
		m_pFindDialog = nullptr;
		return 0;
	}
	if (m_pFindDialog->FindNext())
	{
		FINDTEXTEX Find;
		DWORD dwFlags = 0;
		long nStart, nEnd;
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetSel(nStart, nEnd);
		if (m_pFindDialog->SearchDown())
		{
			Find.chrg.cpMin = nEnd;
			Find.chrg.cpMax = -1;
			dwFlags = FR_DOWN;
		}
		else
		{
			Find.chrg.cpMin = nStart;
			Find.chrg.cpMax = 0;
		}
		if (m_pFindDialog->MatchCase())
		{
			dwFlags |= FR_MATCHCASE;
		}
		if (m_pFindDialog->MatchWholeWord())
		{
			dwFlags |= FR_WHOLEWORD;
		}
		CString Str = m_pFindDialog->GetFindString();
		Find.lpstrText = Str;
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.FindText(dwFlags, &Find);
		if (!Available(Find))
		{
			AfxGetMainWnd()->MessageBox(IsInChinese() ? L"该文档中找不到其他搜索项!" : L"Could not find other results in the document!",
				IsInChinese() ? L"搜索完毕" : L"Done", MB_ICONINFORMATION);
		}
		else
		{
			static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetSel(Find.chrgText);
		}
	}
	return 0;
}


void CChildFrame::OnEditFind()
{
	// TODO: 在此添加命令处理程序代码
	for (auto it = CommonWrapper::GetMainFrame()->m_MDIChildWndMap.begin(); it != CommonWrapper::GetMainFrame()->m_MDIChildWndMap.end(); it++)
	{
		if (static_cast<CChildFrame*>(it->second)->m_pFindDialog!=nullptr&&it->second!=this)
		{
			static_cast<CChildFrame*>(it->second)->m_pFindDialog->DestroyWindow();
			static_cast<CChildFrame*>(it->second)->m_pFindDialog = nullptr;
		}
	}
	if (m_pFindDialog)
	{
		if (static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetSelText().GetLength()==0||static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetSelText().Find(L'\r') != -1)
		{
			m_pFindDialog->ShowWindow(SW_SHOW);
//			static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetFocus();
			return;
		}
		else
		{
			m_pFindDialog->DestroyWindow();
			m_pFindDialog = nullptr;
		}
	}
	m_pFindDialog = new CFindReplaceDialog;
	if (!m_pFindDialog->Create(TRUE, static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetSelText(), nullptr, FR_DOWN, this))
	{
		AfxGetMainWnd()->MessageBox(IsInChinese() ? L"搜索对话框启动失败!" : L"Failed to create the Find Dialog!", IsInChinese() ? L"错误" : L"Error", MB_ICONERROR);
		delete m_pFindDialog;
		m_pFindDialog = nullptr;
		return;
	}
	CRect Rect;
	m_pFindDialog->GetWindowRect(&Rect);
	CRect ViewRect;
	GetActiveView()->GetWindowRect(&ViewRect);
	m_pFindDialog->SetWindowPos(&wndTop, ViewRect.right - Rect.Width(), ViewRect.top, Rect.Width(), Rect.Height(), SWP_HIDEWINDOW);
	m_pFindDialog->ShowWindow(SW_SHOW);
//	static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetFocus();
}


void CChildFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_pFindDialog)
	{
		if (nChar==VK_CONTROL)
		{
			m_pFindDialog->ModifyStyleEx(0, WS_EX_TRANSPARENT);
		}
		else
		{
			m_pFindDialog->ModifyStyleEx(WS_EX_TRANSPARENT, 0);
		}
	}
	CMDIChildWndEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CChildFrame::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_pFindDialog)
	{
		m_pFindDialog->ModifyStyleEx(WS_EX_TRANSPARENT, 0);
	}
	CMDIChildWndEx::OnKeyUp(nChar, nRepCnt, nFlags);
}


BOOL CFindDialog::OnInitDialog()
{
	CFindReplaceDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	VERIFY(m_pComboBox = static_cast<CComboBox*>(GetDlgItem(IDC_CHOOSEAREA)));

	UpdateComboBox();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CFindDialog::UpdateComboBox()
{
	CLineNumEdit & rEdit = static_cast<CFernflowerUIMFCView*>(static_cast<CChildFrame*>(GetParent())->GetActiveView())->m_wndEdit;

}
