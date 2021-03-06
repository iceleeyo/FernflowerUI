
// MainFrm.h: CMainFrame 类的接口
//

#pragma once
#include "FileView.h"
#include "ClassView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// 特性
public:
	friend class COutputWnd;
	friend class CChildFrame;
	friend class CProgressDlg;
	friend class CShellTreeView;
	friend class CFernflowerUIMFCApp;
	friend class CFernflowerUIMFCView;
	friend void UnZipJar(CWnd * MainWnd, const CStringW & JarPath);
	ITaskbarList4 * m_pTaskBar;
protected:
	bool IsShowingWindowManager;
	CRect m_rectClassViewDef;
	CRect m_rectFileViewDef;
	CRect m_rectOutputDef;
	CRect m_rectPropertyDef;
// 操作
public:
	COutputWnd * GetOutput()
	{
		return &m_wndOutput;
	}
	CMFCStatusBar * GetStatusBar()
	{
		return &m_wndStatusBar;
	}
	CClassView * GetClassView()
	{
		return &m_wndClassView;
	}
	std::map<CStringW, CMDIChildWndEx*> m_MDIChildWndMap;
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	CFileView         m_wndFileView;
	CClassView        m_wndClassView;
	COutputWnd        m_wndOutput;
	CMFCCaptionBar    m_wndCaptionBar;
	CPropertiesWnd    m_wndProperties;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
public:
	afx_msg void OnViewCaptionBar();
protected:
	afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	BOOL CreateCaptionBar();
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnClose();
	afx_msg void OnEnableRecovery();
	afx_msg void OnUpdateEnableRecovery(CCmdUI *pCmdUI);
	virtual CMDIChildWndEx* CreateDocumentWindow(LPCTSTR lpcszDocName, CObject* pObj = nullptr);
	afx_msg void OnWindowClose();
	afx_msg void OnUpdateWindowClose(CCmdUI *pCmdUI);
	virtual HMENU GetWindowMenuPopup(HMENU hMenuBar);
	afx_msg void OnQuickDecomp();
	afx_msg void OnUpdateQuickDecomp(CCmdUI *pCmdUI);
	afx_msg void OnClearCache();
	afx_msg void OnUpdateClearCache(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI *pCmdUI);
	afx_msg void OnWindowReset();
	afx_msg void OnEnableAutoSave();
	afx_msg void OnUpdateEnableAutoSave(CCmdUI *pCmdUI);
	afx_msg void OnDecompOptions();
	afx_msg void OnEnableIgnoreCache();
	afx_msg void OnUpdateIgnoreCache(CCmdUI *pCmdUI);
	//afx_msg void OnEditFind();
};


