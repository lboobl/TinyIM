/**
 * @file MsgTipDlg.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 收到消息提示的对话框，
 * @version 0.1
 * @date 2020-03-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "stdafx.h"
#include "MsgTipDlg.h"
#include "UI_USER_INFO.h"
#include "Path.h"
#include "Utils.h"
#include "IUProtocolData.h"

/**
 * @brief Construct a new CMsgTipDlg::CMsgTipDlg object
 * 
 */
CMsgTipDlg::CMsgTipDlg(void)
{
	m_hMainDlg = NULL;
	m_rcTrayIcon.SetRectEmpty();
	m_rcTrayIcon2.SetRectEmpty();
	m_dwTimerId = NULL;
	m_nListItemHeight = 20;
}

/**
 * @brief Destroy the CMsgTipDlg::CMsgTipDlg object
 * 
 */
CMsgTipDlg::~CMsgTipDlg(void)
{
}

/**
 * @brief 响应初始化对话框
 * 
 * @param wndFocus 
 * @param lInitParam 
 * @return BOOL 
 */
BOOL CMsgTipDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	InitCtrls();			// 初始化控件
	SetDlgAutoSize();		// 自动调整对话框大小
	SetCtrlsAutoSize();		// 自动调整控件大小

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE);

	return FALSE;
}

/**
 * @brief 响应背景擦除消息
 * 
 * @param dc 
 * @return BOOL 
 */
BOOL CMsgTipDlg::OnEraseBkgnd(CDCHandle dc)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	return TRUE;
}

/**
 * @brief 响应定时器消息
 * 
 * @param nIDEvent 
 */
void CMsgTipDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_dwTimerId)
	{
		RECT rcWindow;
		GetWindowRect(&rcWindow);

		POINT pt = {0};
		::GetCursorPos(&pt);

		if (!::PtInRect(&rcWindow, pt) && !::PtInRect(&m_rcTrayIcon2, pt))
		{
			KillTimer(m_dwTimerId);
			m_dwTimerId = NULL;
			DestroyWindow();
		}
	}
}

/**
 * @brief 响应对话框大小改变消息
 * 
 * @param nType 
 * @param size 
 */
void CMsgTipDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);

	SIZE szRoundCorner = {4,4};
	if (!IsIconic()) 
	{
		if (szRoundCorner.cx != 0 || szRoundCorner.cy != 0)
		{
			RECT rcWindow;
			::GetWindowRect(m_hWnd, &rcWindow);
			::OffsetRect(&rcWindow, -rcWindow.left, -rcWindow.top);
			rcWindow.right++;
			rcWindow.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn(rcWindow.left, 
				rcWindow.top, rcWindow.right, rcWindow.bottom, 
				szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn(m_hWnd, hRgn, TRUE);
			::DeleteObject(hRgn);
		}
	}
}

/**
 * @brief 响应对话框关闭消息
 * 
 */
void CMsgTipDlg::OnClose()
{
	DestroyWindow();
}

/**
 * @brief 响应对话框销毁消息
 * 
 */
void CMsgTipDlg::OnDestroy()
{
	SetMsgHandled(FALSE);

	UnInitCtrls();			// 反初始化控件

	if (m_dwTimerId != NULL)
	{
		KillTimer(m_dwTimerId);
		m_dwTimerId = NULL;
	}
}

/**
 * @brief 响应列表点击操作
 * 
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CMsgTipDlg::OnList_Click(LPNMHDR pnmh)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pnmh;
	if (pNMListView->iItem != -1)
	{
		CString strType = m_ListCtrl.GetItemText(pNMListView->iItem, 3);
		E_UI_CHAT_MSG_TYPE nType = static_cast<E_UI_CHAT_MSG_TYPE>(_tcstol(strType, NULL, 10));
		UINT nSenderId = m_ListCtrl.GetItemData(pNMListView->iItem, 0);

		switch (nType)
		{
		case E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_BUDDY:
			::SendMessage(m_hMainDlg, WM_SHOW_BUDDY_CHAT_DLG, 0, nSenderId);
			break;
		case E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_GROUP:
			::SendMessage(m_hMainDlg, WM_SHOW_GROUP_CHAT_DLG, nSenderId, 0);
			break;
		case E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_SESS:
			{
				//if (m_lpFMGClient != NULL)
				//{
				//	C_UI_MessageList* lpMsgList = NULL;// m_lpFMGClient->GetMessageList();
				//	if (lpMsgList != NULL)
				//	{
				//		C_UI_MessageSender* lpMsgSender = lpMsgList->GetMsgSender(E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_SESS, nSenderId);
				//		if (lpMsgSender != NULL)
				//			::SendMessage(m_hMainDlg, WM_SHOW_SESS_CHAT_DLG, lpMsgSender->GetGroupCode(), nSenderId);
				//	}
				//}	
			}
			break;
		}
	}
	return 0;
}

/**
 * @brief 响应列表每一项改变
 * 
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CMsgTipDlg::OnList_ItemChange(LPNMHDR pnmh)
{
	int nIndex = m_ListCtrl.GetCurSelItemIndex();
	if (nIndex != -1)
	{
		CString strType = m_ListCtrl.GetItemText(nIndex, 3);
		E_UI_CHAT_MSG_TYPE nType = static_cast<E_UI_CHAT_MSG_TYPE>(_tcstol(strType, NULL, 10));
		UINT nSenderId = m_ListCtrl.GetItemData(nIndex, 0);

		switch (nType)
		{
		case E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_BUDDY:
			::SendMessage(m_hMainDlg, WM_SHOW_BUDDY_CHAT_DLG, 0, nSenderId);
			break;
		case E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_GROUP:
			::SendMessage(m_hMainDlg, WM_SHOW_GROUP_CHAT_DLG, nSenderId, 0);
			break;
		case E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_SESS:
			{
				//if (m_lpFMGClient != NULL)
				//{
				//	C_UI_MessageList* lpMsgList = NULL;// m_lpFMGClient->GetMessageList();
				//	if (lpMsgList != NULL)
				//	{
				//		C_UI_MessageSender* lpMsgSender = lpMsgList->GetMsgSender(E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_SESS, nSenderId);
				//		if (lpMsgSender != NULL)
				//			::SendMessage(m_hMainDlg, WM_SHOW_SESS_CHAT_DLG, lpMsgSender->GetGroupCode(), nSenderId);
				//	}
				//}	
			}
			break;
		//case FMG_MSG_TYPE_SYSGROUP:
		//	::SendMessage(m_hMainDlg, WM_SHOW_SYSGROUPCHATDLG, nSenderId, 0);
		//	break;
		}
	}
	return 0;
}


/**
 * @brief 响应“取消闪烁”超链接控件
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMsgTipDlg::OnLnk_CancelFlash(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	::PostMessage(m_hMainDlg, WM_CANCEL_FLASH, 0, 0);
	//SendMessage(WM_CLOSE);
}


/**
 * @brief 响应“显示全部”超链接控件
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMsgTipDlg::OnLnk_ShowAll(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCount = m_ListCtrl.GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		CString strType = m_ListCtrl.GetItemText(i, 3);
		UINT nType = _tcstol(strType, NULL, 10);
		E_UI_CHAT_MSG_TYPE eType = static_cast<E_UI_CHAT_MSG_TYPE>(nType);
		UINT nSenderId = m_ListCtrl.GetItemData(i, 0);

		switch (eType)
		{
		case E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_BUDDY:
			::SendMessage(m_hMainDlg, WM_SHOW_BUDDY_CHAT_DLG, 0, nSenderId);
			break;
		case E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_GROUP:
			::SendMessage(m_hMainDlg, WM_SHOW_GROUP_CHAT_DLG, nSenderId, 0);
			break;
		case E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_SESS:
			{
				//if (m_lpFMGClient != NULL)
				{
					C_UI_MessageList* lpMsgList = NULL;// m_lpFMGClient->GetMessageList();
					if (lpMsgList != NULL)
					{
						C_UI_MessageSender* lpMsgSender = lpMsgList->GetMsgSender(E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_SESS, nSenderId);
						if (lpMsgSender != NULL)
							::SendMessage(m_hMainDlg, WM_SHOW_SESS_CHAT_DLG, lpMsgSender->GetGroupCode(), nSenderId);
					}
				}	
			}
			break;
		//case FMG_MSG_TYPE_SYSGROUP:
		//	::SendMessage(m_hMainDlg, WM_SHOW_SYSGROUPCHATDLG, nSenderId, 0);
		//	break;
		}
	}
}

/**
 * @brief 开始追踪鼠标移开
 * 
 * @return BOOL 
 */
BOOL CMsgTipDlg::StartTrackMouseLeave()
{
	m_dwTimerId = SetTimer(990, 160, NULL);
	return m_dwTimerId != NULL ? TRUE : FALSE;
}

/**
 * @brief 查找消息发送者
 * 
 * @param nType 
 * @param nSenderId 
 * @return int 
 */
int CMsgTipDlg::FindMsgSender(E_UI_CHAT_MSG_TYPE nType, const std::string nSenderId)
{
	CString strType;
	UINT nSenderId2=0;

	int nCount = m_ListCtrl.GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		strType = m_ListCtrl.GetItemText(i, 3);
		E_UI_CHAT_MSG_TYPE nType2 = static_cast<E_UI_CHAT_MSG_TYPE>(_tcstol(strType, NULL, 10));
		nSenderId2 = m_ListCtrl.GetItemData(i, 0);

		/*if (nType == nType2 && nSenderId == nSenderId2)
			return i;*/
	}
	return -1;
}

/**
 * @brief 添加消息发送者
 * 
 * @param nType 
 * @param nSenderId 
 */
void CMsgTipDlg::AddMsgSender(E_UI_CHAT_MSG_TYPE nType, const std::string nSenderId)
{
	/*C_UI_MessageSender* lpMsgSender = NULL;
	if (m_lpFMGClient != NULL)
	{
		C_UI_MessageList* lpMsgList = NULL;// m_lpFMGClient->GetMessageList();
		if (lpMsgList != NULL)
			lpMsgSender = lpMsgList->GetMsgSender(nType, nSenderId);
	}
	if (NULL == lpMsgSender)
		return;*/

	int nItemIndex = FindMsgSender(nType, nSenderId);
	if (nItemIndex != -1)
	{
		//int nMsgCnt = lpMsgSender->GetMsgCount();
		int nMsgCnt = 1;
		CString strMsgCnt;
		strMsgCnt.Format(_T("(%d)"), nMsgCnt);
		m_ListCtrl.SetItemText(nItemIndex, 2, strMsgCnt);
	}
	else
	{
		SetDlgAutoSize();		// 自动调整对话框大小
		SetCtrlsAutoSize();		// 自动调整控件大小
		//_AddMsgSender(0, lpMsgSender);
	}
}

/**
 * @brief 删除消息发送者
 * 
 * @param nType 
 * @param nSenderId 
 */
void CMsgTipDlg::DelMsgSender(E_UI_CHAT_MSG_TYPE nType, const std::string nSenderId)
{
	int nItemIndex = FindMsgSender(nType, nSenderId);
	if (nItemIndex != -1)
		m_ListCtrl.DeleteItem(nItemIndex);
	SetDlgAutoSize();	// 自动调整对话框大小
	SetCtrlsAutoSize();		// 自动调整控件大小
}


/**
 * @brief 初始化控件
 * 
 * @return BOOL 
 */
BOOL CMsgTipDlg::InitCtrls()
{
	m_SkinDlg.SubclassWindow(m_hWnd);
	WString strFileName = Hootina::CPath::GetAppPath() + _T("Image\\MsgTipBg.png");	// 加载背景图片
	m_SkinDlg.SetBgPic(strFileName.c_str(), CRect(8,28,8,8));

	HDC hDlgBgDC = m_SkinDlg.GetBgDC();

	HFONT hFont = (HFONT)::SendMessage(m_hWnd, WM_GETFONT, 0, 0L);

	m_lnkCancelFlash.SubclassWindow(GetDlgItem(ID_LINK_CANCEL_FLASH));
	m_lnkCancelFlash.SetTransparent(TRUE, hDlgBgDC);
	m_lnkCancelFlash.SetLinkColor(RGB(0,114,193));
	m_lnkCancelFlash.SetHoverLinkColor(RGB(0,114,193));
	m_lnkCancelFlash.SetVisitedLinkColor(RGB(0,114,193));
	m_lnkCancelFlash.SetLabel(_T("取消闪烁"));
	m_lnkCancelFlash.SetHoverFont(hFont);

	m_lnkShowAll.SubclassWindow(GetDlgItem(ID_LINK_SHOW_ALL));
	m_lnkShowAll.SetTransparent(TRUE, hDlgBgDC);
	m_lnkShowAll.SetLinkColor(RGB(0,114,193));
	m_lnkShowAll.SetHoverLinkColor(RGB(0,114,193));
	m_lnkShowAll.SetVisitedLinkColor(RGB(0,114,193));
	m_lnkShowAll.SetLabel(_T("查看全部"));
	m_lnkShowAll.SetHoverFont(hFont);

	m_ListCtrl.Create(m_hWnd, CRect(0,0,1,1), NULL, 
		WS_CHILD|WS_VISIBLE, NULL, 1000, NULL);
	m_ListCtrl.SetTransparent(TRUE, hDlgBgDC);
	m_ListCtrl.SetItemHoverBgPic(_T("MsgTipListItemBg.png"));
	m_ListCtrl.SetItemTextColor(RGB(0,0,0));
	m_ListCtrl.SetSelItemTextColor(RGB(0,0,0));
	m_ListCtrl.SetHeadVisible(FALSE);
	m_ListCtrl.SetItemImageSize(16, 16);

//	m_ListCtrl.AddColumn(_T("空格"), NULL, DT_LEFT, 7);
	m_ListCtrl.AddColumn(_T("图标"), NULL, DT_LEFT, 30);
	m_ListCtrl.AddColumn(_T("发送者"), NULL, DT_LEFT, 136);
	m_ListCtrl.AddColumn(_T("消息数"), NULL, DT_RIGHT, 38);
	m_ListCtrl.AddColumn(_T("类型"), NULL, DT_RIGHT, 0);

	m_ListCtrl.DeleteAllItems();
	/*if (m_lpFMGClient != NULL)
	{
		C_UI_MessageList* lpMsgList = NULL;// m_lpFMGClient->GetMessageList();
		if (lpMsgList != NULL)
		{
			int nMsgSenderCnt = lpMsgList->GetMsgSenderCount();
			for (int i = 0; i < nMsgSenderCnt; i++)
			{
				C_UI_MessageSender* lpMsgSender = lpMsgList->GetMsgSender(i);
				if (lpMsgSender != NULL)
				{
					_AddMsgSender(0, lpMsgSender);
				}
			}
		}
	}*/

	return TRUE;
}


/**
 * @brief 反初始化控件
 * 
 * @return BOOL 
 */
BOOL CMsgTipDlg::UnInitCtrls()
{
	if (m_ListCtrl.IsWindow())
		m_ListCtrl.DestroyWindow();

	if (m_lnkCancelFlash.IsWindow())
		m_lnkCancelFlash.DestroyWindow();

	if (m_lnkShowAll.IsWindow())
		m_lnkShowAll.DestroyWindow();

	return TRUE;
}


/**
 * @brief 自动调整对话框大小
 * 
 */
void CMsgTipDlg::SetDlgAutoSize()
{
	int nMsgSenderCnt = 0;
	//if (m_lpFMGClient != NULL)
	//{
	//	C_UI_MessageList* lpMsgList = NULL;// m_lpFMGClient->GetMessageList();
	//	if (lpMsgList != NULL)
	//		nMsgSenderCnt = lpMsgList->GetMsgSenderCount();
	//}

	int cyListCtrl = nMsgSenderCnt* m_nListItemHeight + 5;

	int cxDlg = 212;
	int cyDlg = cyListCtrl + 36 + 22;

	int cxScreen = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int cyScreen = ::GetSystemMetrics(SM_CYFULLSCREEN);

	CRect rcTrayWnd;
	GetTrayWndRect(&rcTrayWnd);

	int nLeft, nTop;

	if (rcTrayWnd.top == rcTrayWnd.left && rcTrayWnd.bottom > rcTrayWnd.right)	// 左边
	{
		nLeft = rcTrayWnd.right + 2;
		nTop = m_rcTrayIcon.top - (cyDlg - m_rcTrayIcon.Width()) / 2;
		if (nTop + cyDlg > cyScreen)
			nTop = cyScreen - cyDlg;
	}
	else if (rcTrayWnd.top == rcTrayWnd.left && rcTrayWnd.bottom < rcTrayWnd.right)	// 上边
	{
		nLeft = m_rcTrayIcon.left - (cxDlg - m_rcTrayIcon.Width()) / 2;
		nTop = rcTrayWnd.bottom + 2;
		if (nLeft + cxDlg > cxScreen)
			nLeft = cxScreen - cxDlg;
	}
	else if (rcTrayWnd.top > rcTrayWnd.left)	// 下边
	{
		nLeft = m_rcTrayIcon.left - (cxDlg - m_rcTrayIcon.Width()) / 2;
		nTop = rcTrayWnd.top - cyDlg - 2;
		if (nLeft + cxDlg > cxScreen)
			nLeft = cxScreen - cxDlg;
	}
	else	// 右边
	{
		nLeft = rcTrayWnd.left - cxDlg - 2;
		nTop = m_rcTrayIcon.top - (cyDlg - m_rcTrayIcon.Width()) / 2;
		if (nTop + cyDlg > cyScreen)
			nTop = cyScreen - cyDlg;
	}

	SetWindowPos(HWND_TOPMOST, nLeft, nTop, cxDlg, cyDlg, SWP_NOACTIVATE);
	Invalidate();
}


/**
 * @brief  自动调整控件大小
 * 
 */
void CMsgTipDlg::SetCtrlsAutoSize()
{
	CRect rcClient;
	GetClientRect(&rcClient);

	int nMsgSenderCnt = 0;
	//if (m_lpFMGClient != NULL)
	//{
	//	C_UI_MessageList* lpMsgList = NULL;// m_lpFMGClient->GetMessageList();
	//	if (lpMsgList != NULL)
	//		nMsgSenderCnt = lpMsgList->GetMsgSenderCount();
	//}

	int cxListCtrl = 204;
	int cyListCtrl = nMsgSenderCnt* m_nListItemHeight + 5;

	if (m_ListCtrl.IsWindow())
		m_ListCtrl.MoveWindow(4, 36, cxListCtrl, cyListCtrl);
	if (m_lnkCancelFlash.IsWindow())
		m_lnkCancelFlash.MoveWindow(12, rcClient.bottom - 22, 48, 14);
	if (m_lnkShowAll.IsWindow())
		m_lnkShowAll.MoveWindow(156, rcClient.bottom - 22, 48, 14);
}

/**
 * @brief 添加消息发送者到列表
 * 
 * @param nIndex 
 * @param lpMsgSender 
 */
void CMsgTipDlg::_AddMsgSender(int nIndex, C_UI_MessageSender* lpMsgSender)
{
	//if (NULL == lpMsgSender || NULL == m_lpFMGClient)
	//	return;

	CString strHeadPicFileName;
	CString strSenderName, strMsgCnt, strMsgType;

	E_UI_CHAT_MSG_TYPE nMsgType = lpMsgSender->GetMsgType();
	UINT nMsgCnt = lpMsgSender->GetMsgCount();
	UINT nSenderId = lpMsgSender->GetSenderId();
	UINT nGroupCode = lpMsgSender->GetGroupCode();

	switch (nMsgType)
	{
	case E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_BUDDY:
		{
			//strHeadPicFileName = GetHeadPicFullName(0, nSenderId);
			//if (!Hootina::CPath::IsFileExist(strHeadPicFileName.c_str()))
			//	strHeadPicFileName = Hootina::CPath::GetAppPath() + _T("Image\\DefBuddyHeadPic.png");
		C_UI_BuddyInfo* pBuddyInfo = NULL;// m_lpFMGClient->m_UserMgr.m_BuddyList.GetBuddy(nSenderId);
			if(pBuddyInfo != NULL)
			{
				if(pBuddyInfo!=NULL && pBuddyInfo->m_bUseCustomFace && pBuddyInfo->m_bCustomFaceAvailable)
				{
					//strHeadPicFileName.Format(_T("%s%d.png"), m_lpFMGClient->m_UserMgr.GetCustomUserThumbFolder().c_str(), pBuddyInfo->m_uUserID);
					if(!Hootina::CPath::IsFileExist(strHeadPicFileName))
						strHeadPicFileName.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, pBuddyInfo->m_nFace);
				}
				else
					strHeadPicFileName.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, pBuddyInfo->m_nFace);
			}

			C_UI_BuddyList* lpBuddyList = NULL;// m_lpFMGClient->GetBuddyList();
			if (lpBuddyList != NULL)
			{
				C_UI_BuddyInfo* lpBuddyInfo = NULL;// lpBuddyList->GetBuddy(nSenderId);
				if (lpBuddyInfo != NULL)
				{
					CString strBuddyName;

					if (!lpBuddyInfo->m_strMarkName.empty())
						strBuddyName = lpBuddyInfo->m_strMarkName.c_str();
					else
						strBuddyName = lpBuddyInfo->m_strNickName.c_str();

					if (lpBuddyInfo->m_uUserIndex != 0)
						strSenderName.Format(_T("%s"), strBuddyName);
				}
			}
		}
		break;
	case E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_GROUP:
		{
			//strHeadPicFileName = GetHeadPicFullName(nSenderId, 0);
			//if (!Hootina::CPath::IsFileExist(strHeadPicFileName.c_str()))
				//strHeadPicFileName = Hootina::CPath::GetAppPath() + _T("Image\\DefGroupHeadPic.png");
			strHeadPicFileName.Format(_T("%sImage\\DefGroupHeadPic.png"), g_szHomePath);

			CGroupList* lpGroupList = NULL;// m_lpFMGClient->GetGroupList();
			if (lpGroupList != NULL)
			{
				//C_UI_GroupInfo* lpGroupInfo = lpGroupList->GetGroupByCode(nSenderId);
				//if (lpGroupInfo != NULL)
				//{
				//	//if (lpGroupInfo->m_nGroupCode != 0)
				//	//	strSenderName.Format(_T("%s"), lpGroupInfo->m_strName.c_str());
				//}
			}
		}
		break;
	case E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_SESS:
		{
			//strHeadPicFileName = GetHeadPicFullName(nGroupCode, nSenderId);
			//if (!Hootina::CPath::IsFileExist(strHeadPicFileName.c_str()))
			//	strHeadPicFileName = Hootina::CPath::GetAppPath() + _T("Image\\DefBuddyHeadPic.png");

			strHeadPicFileName.Format(_T("%sImage\\DefBuddyHeadPic.png"), g_szHomePath);

			CGroupList* lpGroupList = NULL;// m_lpFMGClient->GetGroupList();
			if (lpGroupList != NULL)
			{
				C_UI_BuddyInfo* lpBuddyInfo = NULL;// lpGroupList->GetGroupMemberByCode(nGroupCode, nSenderId);
				if (lpBuddyInfo != NULL)
				{
					CString strBuddyName;

					strBuddyName = lpBuddyInfo->m_strNickName.c_str();

					if (lpBuddyInfo->m_uUserIndex != 0)
						strSenderName.Format(_T("%s(%u)"), strBuddyName, lpBuddyInfo->m_uUserIndex);
					else
						strSenderName.Format(_T("%s"), strBuddyName);
				}
			}
		}
		break;
	}
	long nMsgDisplayCnt = lpMsgSender->GetDisplayMsgCount();
	strMsgCnt.Format(_T("(%d)"), nMsgDisplayCnt);
	strMsgType.Format(_T("%d"), nMsgType);

	int nItem = m_ListCtrl.InsertItem(nIndex, _T(""), _T(""), FALSE, DT_LEFT, nSenderId);
	m_ListCtrl.SetItemFormat(nIndex, 0, DT_LEFT);
	m_ListCtrl.SetItemFormat(nIndex, 1, DT_LEFT);
	m_ListCtrl.SetItemFormat(nIndex, 2, DT_RIGHT);
	m_ListCtrl.SetItemFormat(nIndex, 3, DT_RIGHT);
	m_ListCtrl.SetItemImage(nIndex, 0, strHeadPicFileName);
	m_ListCtrl.SetItemText(nIndex, 1, strSenderName);
	m_ListCtrl.SetItemText(nIndex, 2, strMsgCnt);
	m_ListCtrl.SetItemText(nIndex, 3, strMsgType);
}

/**
 * @brief 获取头像图片全路径
 * 
 * @param nGroupCode 
 * @param nUTalkUin 
 * @return WString 
 */
WString CMsgTipDlg::GetHeadPicFullName(UINT nGroupCode, UINT nUTalkUin)
{
	UINT nGroupNum, nUTalkNum;
	GetNumber(nGroupCode, nUTalkUin, nGroupNum, nUTalkNum);

	/*if (nGroupCode != 0 && nUTalkUin != 0)	// 群成员
		return m_lpFMGClient->GetSessHeadPicFullName(nUTalkNum).c_str();
	else if (nGroupCode != 0)			// 群
		return m_lpFMGClient->GetGroupHeadPicFullName(nGroupNum).c_str();
	else								// 好友
		return m_lpFMGClient->GetBuddyHeadPicFullName(nUTalkNum).c_str();*/

	WString tresult;
	return tresult;
}

/**
 * @brief 获取好友ID或者群组ID
 * TODO: 具体流程需要详细梳理
 * @param nGroupCode 
 * @param nUTalkUin 
 * @param nGroupNum 
 * @param nUTalkNum 
 */
void CMsgTipDlg::GetNumber(UINT nGroupCode, UINT nUTalkUin, UINT& nGroupNum, UINT& nUTalkNum)
{
	nGroupNum = nUTalkNum = 0;

	if (nGroupCode != 0 && nUTalkUin != 0)
	{
		CGroupList* lpGroupList = NULL;// m_lpFMGClient->GetGroupList();
		if (lpGroupList != NULL)
		{
			/*C_UI_BuddyInfo* lpBuddyInfo = lpGroupList->GetGroupMemberByCode(nGroupCode, nUTalkUin);
			if (lpBuddyInfo != NULL)
				nUTalkNum = lpBuddyInfo->m_uUserIndex;*/
		}
	}
	else if (nGroupCode != 0)
	{
		CGroupList* lpGroupList = NULL;// m_lpFMGClient->GetGroupList();
		if (lpGroupList != NULL)
		{
			//C_UI_GroupInfo* lpGroupInfo = lpGroupList->GetGroupByCode(nGroupCode);
			//if (lpGroupInfo != NULL)
			//	nGroupNum = lpGroupInfo->m_nGroupNumber;
		}
	}
	else if (nUTalkUin != 0)
	{
		C_UI_BuddyList* lpBuddyList = NULL;// m_lpFMGClient->GetBuddyList();
		if (lpBuddyList != NULL)
		{
			C_UI_BuddyInfo* lpBuddyInfo = NULL;// lpBuddyList->GetBuddy(nUTalkUin);
			if (lpBuddyInfo != NULL)
				nUTalkNum = lpBuddyInfo->m_uUserIndex;
		}
	}
}