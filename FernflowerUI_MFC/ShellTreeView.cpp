// ShellTreeView.cpp: 实现文件
//

#include "stdafx.h"
#include "FernflowerUI_MFC.h"
#include "ShellTreeView.h"
#include "CommonWrapper.h"


// CShellTreeView

IMPLEMENT_DYNAMIC(CShellTreeView, CMFCShellTreeCtrl)

CShellTreeView::CShellTreeView():CMFCShellTreeCtrl()
{

}

CShellTreeView::~CShellTreeView()
{
}


BEGIN_MESSAGE_MAP(CShellTreeView, CMFCShellTreeCtrl)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_POPCOM_DECOMPLIE, &CShellTreeView::OnPopcomDecomplie)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CShellTreeView::OnTvnSelchanged)
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// CShellTreeView 消息处理程序


void CShellTreeView::OnContextMenu(CWnd * pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	if (m_pContextMenu2 != NULL)
	{
		return;
	}

	if (!m_bContextMenu)
	{
		Default();
		return;
	}

	HTREEITEM hItem = NULL;
	if (point.x == -1 && point.y == -1)
	{
		CRect rectItem;

		if ((hItem = GetSelectedItem()) != NULL && GetItemRect(hItem, rectItem, FALSE))
		{
			point.x = rectItem.left;
			point.y = rectItem.bottom + 1;

			ClientToScreen(&point);
		}
	}
	else
	{
		CPoint ptClient = point;
		ScreenToClient(&ptClient);

		UINT nFlags = 0;
		hItem = HitTest(ptClient, &nFlags);
	}

	if (hItem == NULL)
	{
		return;
	}

	TVITEM tvItem;

	ZeroMemory(&tvItem, sizeof(tvItem));
	tvItem.mask = TVIF_PARAM;
	tvItem.hItem = hItem;

	if (!GetItem(&tvItem))
	{
		return;
	}

	LPAFX_SHELLITEMINFO pInfo = (LPAFX_SHELLITEMINFO)tvItem.lParam;
	if (pInfo == NULL)
	{
		ASSERT(FALSE);
		return;
	}

	IShellFolder* psfFolder = pInfo->pParentFolder;

	if (psfFolder == NULL)
	{
		ENSURE(SUCCEEDED(SHGetDesktopFolder(&psfFolder)));
	}
	else
	{
		psfFolder->AddRef();
	}

	if (psfFolder != NULL)
	{
		HWND hwndParent = GetParent()->GetSafeHwnd();
		IContextMenu* pcm = NULL;

		HRESULT hr = psfFolder->GetUIObjectOf(hwndParent, 1, (LPCITEMIDLIST*)&pInfo->pidlRel, IID_IContextMenu, NULL, (LPVOID*)&pcm);

		if (SUCCEEDED(hr))
		{
			HMENU hPopup = CreatePopupMenu();
			if (hPopup != NULL)
			{
				hr = pcm->QueryContextMenu(hPopup, 0, 1, 0x7fff, CMF_NORMAL | CMF_EXPLORE);

				if (SUCCEEDED(hr))
				{
					pcm->QueryInterface(IID_IContextMenu2, (LPVOID*)&m_pContextMenu2);

					HWND hwndThis = GetSafeHwnd();
					CStringW ExtendFile;
					wchar_t szFolderName[MAX_PATH];
					if (SHGetPathFromIDList(pInfo->pidlFQ, szFolderName))
					{
						PathFile = szFolderName;
						bool IsJar = false;
						wchar_t * Buf = PathFile.GetBuffer();
						ExtendFile = Buf + PathFile.ReverseFind(L'.');
						PathFile.ReleaseBuffer();
						int Extend = ExtendFile.CompareNoCase(L".jar")*ExtendFile.CompareNoCase(L".zip")*ExtendFile.CompareNoCase(L".class");
						if (Extend == 0)
						{
							CMenu PopMenu;
							PopMenu.LoadMenuW(IDR_POPUP_SHELLVIEW);
							CMenu * DecompMenu = PopMenu.GetSubMenu(0);
							DecompMenu->AppendMenuW(MF_BYPOSITION | MF_POPUP | MF_STRING, (UINT_PTR)hPopup, IsInChinese()?L"文件操作":L"File Operation");
							hPopup = DecompMenu->GetSafeHmenu();
							PopMenu.Detach();
							EnableMenuItem(hPopup, ID_POPCOM_DECOMPLIE, MF_ENABLED);
						}
					}
					else
					{
						PathFile = L"";
					}
					//UINT idCmd = afxContextMenuManager->TrackPopupMenu(hPopup, point.x, point.y, this);
					UINT idCmd = TrackPopupMenu(hPopup, TPM_LEFTALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, point.x, point.y, 0, GetSafeHwnd(), NULL);

					if (::IsWindow(hwndThis))
					{
						if (m_pContextMenu2 != NULL)
						{
							m_pContextMenu2->Release();
							m_pContextMenu2 = NULL;
						}
						if (idCmd == ID_POPCOM_DECOMPLIE)
						{
							SendMessage(WM_COMMAND, idCmd);
						}
						else if (idCmd != 0)
						{
							CWaitCursor wait;

							CMINVOKECOMMANDINFO cmi;
							cmi.cbSize = sizeof(CMINVOKECOMMANDINFO);
							cmi.fMask = 0;
							cmi.hwnd = hwndParent;
							cmi.lpVerb = (LPCSTR)(INT_PTR)(idCmd - 1);
							cmi.lpParameters = NULL;
							cmi.lpDirectory = NULL;
							cmi.nShow = SW_SHOWNORMAL;
							cmi.dwHotKey = 0;
							cmi.hIcon = NULL;

							hr = pcm->InvokeCommand(&cmi);

							if (SUCCEEDED(hr) && GetParent() != NULL)
							{
								GetParent()->SendMessage(AFX_WM_ON_AFTER_SHELL_COMMAND, (WPARAM)idCmd);
							}

							SetFocus();
						}
					}
				}
			}

			if (pcm != NULL)
			{
				pcm->Release();
				pcm = NULL;
			}
		}

		if (psfFolder != NULL)
		{
			psfFolder->Release();
			psfFolder = NULL;
		}
	}
	
}


void CShellTreeView::OnPopcomDecomplie()
{
	// TODO: 在此添加命令处理程序代码
	if (!(PathFile == L""))
		theApp.DoDecomplie(PathFile);
}

static bool Finished;

HRESULT CShellTreeView::EnumObjects(HTREEITEM hParentItem, LPSHELLFOLDER pParentFolder, LPITEMIDLIST pidlParent)
{
	Finished = false;
	if (!IsWindow(CommonWrapper::GetMainFrame()->GetSafeHwnd()))
	{
		return this->CMFCShellTreeCtrl::EnumObjects(hParentItem, pParentFolder, pidlParent);
	}
	auto EnumObj = [&]()->HRESULT {
		Finished = false;
		//Because of the AFX doesn't work well with std::future,I have to copy the source code.
		LPENUMIDLIST pEnum = NULL;

		HRESULT hr = pParentFolder->EnumObjects(NULL, m_dwFlags, &pEnum);
		if (FAILED(hr) || pEnum == NULL)
		{
			Finished = true;
			return hr;
		}

		LPITEMIDLIST pidlTemp;
		DWORD dwFetched = 1;

		//The async version , maybe
		
		auto InsertTreeItem = [hParentItem, pParentFolder, pidlParent](CShellTreeView * ShellTree,LPITEMIDLIST pidlTemp)->void {
			TVITEM tvItem;
			ZeroMemory(&tvItem, sizeof(tvItem));

			// Fill in the TV_ITEM structure for this item:
			tvItem.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;

			// AddRef the parent folder so it's pointer stays valid:
			pParentFolder->AddRef();

			// Put the private information in the lParam:
			LPAFX_SHELLITEMINFO pItem = (LPAFX_SHELLITEMINFO)GlobalAlloc(GPTR, sizeof(AFX_SHELLITEMINFO));
			ENSURE(pItem != NULL);

			pItem->pidlRel = pidlTemp;
			pItem->pidlFQ = afxShellManager->ConcatenateItem(pidlParent, pidlTemp);

			pItem->pParentFolder = pParentFolder;
			tvItem.lParam = (LPARAM)pItem;

			CString strItem = ShellTree->OnGetItemText(pItem);
			tvItem.pszText = strItem.GetBuffer(strItem.GetLength());
			tvItem.iImage = ShellTree->OnGetItemIcon(pItem, FALSE);
			tvItem.iSelectedImage = ShellTree->OnGetItemIcon(pItem, TRUE);

			// Determine if the item has children:
			//DWORD dwAttribs = SFGAO_HASSUBFOLDER | SFGAO_FOLDER | SFGAO_DISPLAYATTRMASK | SFGAO_CANRENAME | SFGAO_FILESYSANCESTOR;
			DWORD dwAttribs = SFGAO_HASSUBFOLDER | SFGAO_FOLDER | SFGAO_FILESYSANCESTOR;

			pParentFolder->GetAttributesOf(1, (LPCITEMIDLIST*)&pidlTemp, &dwAttribs);
			tvItem.cChildren = (dwAttribs & (SFGAO_HASSUBFOLDER | SFGAO_FILESYSANCESTOR));

			// Determine if the item is shared:
			if (dwAttribs & SFGAO_SHARE)
			{
				tvItem.mask |= TVIF_STATE;
				tvItem.stateMask |= TVIS_OVERLAYMASK;
				tvItem.state |= INDEXTOOVERLAYMASK(1); //1 is the index for the shared overlay image
			}

			// Fill in the TV_INSERTSTRUCT structure for this item:
			TVINSERTSTRUCT tvInsert;

			tvInsert.item = tvItem;
			tvInsert.hInsertAfter = TVI_LAST;
			tvInsert.hParent = hParentItem;

			ShellTree->InsertItem(&tvInsert); 
		};

		std::future<void> InsertThread;

		if (SUCCEEDED(pEnum->Next(1, &pidlTemp, &dwFetched)) && dwFetched)
		{
			InsertThread = std::async(std::launch::async, InsertTreeItem, this, pidlTemp);
			// Enumerate the item's PIDLs:
			while (SUCCEEDED(pEnum->Next(1, &pidlTemp, &dwFetched)) && dwFetched)
			{
				InsertThread.get();
				InsertThread = std::async(std::launch::async, InsertTreeItem, this, pidlTemp);
				dwFetched = 0;
			}
			InsertThread.get();
		}
		/*
		// Enumerate the item's PIDLs:
		while (SUCCEEDED(pEnum->Next(1, &pidlTemp, &dwFetched)) && dwFetched)
		{
			TVITEM tvItem;
			ZeroMemory(&tvItem, sizeof(tvItem));

			// Fill in the TV_ITEM structure for this item:
			tvItem.mask = TVIF_PARAM | TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;

			// AddRef the parent folder so it's pointer stays valid:
			pParentFolder->AddRef();

			// Put the private information in the lParam:
			LPAFX_SHELLITEMINFO pItem = (LPAFX_SHELLITEMINFO)GlobalAlloc(GPTR, sizeof(AFX_SHELLITEMINFO));
			ENSURE(pItem != NULL);

			pItem->pidlRel = pidlTemp;
			pItem->pidlFQ = afxShellManager->ConcatenateItem(pidlParent, pidlTemp);

			pItem->pParentFolder = pParentFolder;
			tvItem.lParam = (LPARAM)pItem;

			CString strItem = OnGetItemText(pItem);
			tvItem.pszText = strItem.GetBuffer(strItem.GetLength());
			tvItem.iImage = OnGetItemIcon(pItem, FALSE);
			tvItem.iSelectedImage = OnGetItemIcon(pItem, TRUE);

			// Determine if the item has children:
			//DWORD dwAttribs = SFGAO_HASSUBFOLDER | SFGAO_FOLDER | SFGAO_DISPLAYATTRMASK | SFGAO_CANRENAME | SFGAO_FILESYSANCESTOR;
			DWORD dwAttribs = SFGAO_HASSUBFOLDER | SFGAO_FOLDER ;

			pParentFolder->GetAttributesOf(1, (LPCITEMIDLIST*)&pidlTemp, &dwAttribs);
			tvItem.cChildren = (dwAttribs & (SFGAO_HASSUBFOLDER | SFGAO_FILESYSANCESTOR));

			// Determine if the item is shared:
			if (dwAttribs & SFGAO_SHARE)
			{
				tvItem.mask |= TVIF_STATE;
				tvItem.stateMask |= TVIS_OVERLAYMASK;
				tvItem.state |= INDEXTOOVERLAYMASK(1); //1 is the index for the shared overlay image
			}

			// Fill in the TV_INSERTSTRUCT structure for this item:
			TVINSERTSTRUCT tvInsert;

			tvInsert.item = tvItem;
			tvInsert.hInsertAfter = TVI_LAST;
			tvInsert.hParent = hParentItem;

			InsertItem(&tvInsert);
			dwFetched = 0;
		}*/

		pEnum->Release();
		Finished = true;
		return S_OK;
	};
	std::future<HRESULT> Search = std::async(std::launch::async, EnumObj);
	auto Scanner = []()->bool {return Finished; };
	if (theApp.Initing)
	{
		CommonWrapper::CWaitDlg Wait(AfxGetMainWnd(), Scanner, 5, IsInChinese()?L"正在初始化":L"Initing", 20, IDD_WAITLAUNCH, true);
		Wait.DoModal();
	}
	else
	{
		CommonWrapper::CWaitDlg Wait(AfxGetMainWnd(), Scanner, 5, IsInChinese()?L"正在打开文件夹...":L"Opening the folder...");
		Wait.DoModal();
	}
	GetItemPath(theApp.PathToSave, hParentItem);
	return Search.get();
}


void CShellTreeView::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (GetItemPath(theApp.PathToSave))
	{
		*pResult = 0;
		return;
	}
	*pResult = !GetItemPath(theApp.PathToSave,GetParentItem(GetSelectedItem()));
}



void CShellTreeView::OnDestroy()
{
	CMFCShellTreeCtrl::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	//EndModalLoop(0);
}


