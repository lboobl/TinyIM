/**
 * @file FindFriendDlg.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 查找好友对话框的实现文件
 * @version 0.1
 * @date 2020-02-29
 * 
 * @copyright Copyright (c) 2020
 * 
 */


#include "stdafx.h"
#include "FindFriendDlg.h"
#include "UserSnapInfoDlg.h"
#include "Utils.h"
#include "EncodingUtil.h"
#include "Proto.h"

// CFindFriendDlg实现代码

enum RELATION_TYPE
{
	RELATION_TYPE_UNKNOWN,//关系未知
	RELATION_TYPE_SELF,//是自己
	RELATION_TYPE_FRIENDS,//是好友
};


/**
 * @brief Construct a new CFindFriendDlg::CFindFriendDlg object
 * 
 */
CFindFriendDlg::CFindFriendDlg():m_userMgr(CUserMgr::GetInstance())
{
}

/**
 * @brief Destroy the CFindFriendDlg::CFindFriendDlg object
 * 
 */
CFindFriendDlg::~CFindFriendDlg()
{
}

/**
 * @brief 预处理消息
 * 
 * @param pMsg 
 * @return BOOL 
 */
BOOL CFindFriendDlg::PreTranslateMessage(MSG* pMsg)
{
	//支持回车键查找
	if ( (pMsg->hwnd==m_hWnd) && 
		 (pMsg->message==WM_KEYDOWN) && 
		 (pMsg->wParam==VK_RETURN) )
 	{
		PostMessage(WM_COMMAND, (WPARAM)IDC_BTN_ADD, 0);
		return TRUE;
 	}

	if ( (pMsg->hwnd==m_hWnd) && 
		 (pMsg->message==WM_KEYDOWN) && 
		 (pMsg->wParam==VK_ESCAPE) )
 	{
		ShowWindow(SW_HIDE);
		return TRUE;
 	}

	return CWindow::IsDialogMessage(pMsg);
}

/**
 * @brief 响应初始化对话框消息
 * 
 * @param wndFocus 
 * @param lInitParam 
 * @return BOOL 
 */
BOOL CFindFriendDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	AddMessageFilter();

	InitUI();

	CenterWindow(::GetDesktopWindow());
	return TRUE;
}


/**
 * @brief 初始化UI
 * 
 * @return BOOL 
 */
BOOL CFindFriendDlg::InitUI()
{
	//对话框整体界面
	{
		m_SkinDlg.SetBgPic(_T("DlgBg\\FindFriendDlgBg.png"));
		m_SkinDlg.SetCloseSysBtnPic(_T("SysBtn\\btn_close_normal.png"), _T("SysBtn\\btn_close_highlight.png"), _T("SysBtn\\btn_close_down.png"));
		m_SkinDlg.SubclassWindow(m_hWnd);
		m_SkinDlg.SetTitleText(_T("查找联系人或群组"));
		m_SkinDlg.MoveWindow(0, 0, 350, 200, TRUE);
	}


	HDC hDlgBgDC = m_SkinDlg.GetBgDC();

	//用户ID编辑框控件
	{
		m_edtAddId.SetBgNormalPic(_T("frameBorderEffect_normalDraw.png"), CRect(2, 2, 2, 2));
		m_edtAddId.SetBgHotPic(_T("frameBorderEffect_mouseDownDraw.png"), CRect(2, 2, 2, 2));
		m_edtAddId.SetTransparent(TRUE, hDlgBgDC);
		m_edtAddId.SubclassWindow(GetDlgItem(IDC_EDIT_ADD));
	}
		

	{
		m_btnAdd.SetButtonType(SKIN_PUSH_BUTTON);
		m_btnAdd.SetTransparent(TRUE, hDlgBgDC);
		m_btnAdd.SetBgPic(_T("Button\\btn_normal.png"), _T("Button\\btn_focus.png"), _T("Button\\btn_focus.png"), _T("Button\\btn_focus.png"));
		m_btnAdd.SetRound(4, 4);
		m_btnAdd.SubclassWindow(GetDlgItem(IDC_BTN_ADD));
		m_btnAdd.SetWindowText(_T("查找"));
		m_btnAdd.SetToolTipText(_T("点击按钮进行查找"));
	}

	//单选按钮绑定
	{
		m_btnFindTypeSingle.Attach(GetDlgItem(IDC_RADIO_FIND_TYPE1));
		m_btnFindTypeSingle.SetCheck(TRUE);
		//m_btnFindTypeSingle.SetTransparent(TRUE, hDlgBgDC);

		m_btnFindTypeGroup.Attach(GetDlgItem(IDC_RADIO_FIND_TYPE2));
		//m_btnFindTypeGroup.SetTransparent(TRUE, hDlgBgDC)
	}
	//TODO 
	{
		m_staticAddInfo.SetTransparent(TRUE, hDlgBgDC);
		m_staticAddInfo.SetLinkColor(RGB(225, 0, 0));
		m_staticAddInfo.SetLinkType(SKIN_LINK_ADDNEW);
		m_staticAddInfo.SubclassWindow(GetDlgItem(IDC_STATIC_ADD_INFO));
	}

	return TRUE;
}


/**
 * @brief 响应确认添加按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CFindFriendDlg::OnAddFriend(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strAccountToAdd;
	m_edtAddId.GetWindowText(strAccountToAdd);
	strAccountToAdd.Trim();
	if(strAccountToAdd.GetLength()>32 || strAccountToAdd.GetLength()<=0)
	{
		m_staticAddInfo.SetWindowText(_T("查找的账号长度必须在1～30个字符之间！"));
		m_edtAddId.SetFocus();
		return;
	}

	//TODO  此处代码待重构
	long nRelationType = 0;
	if (m_userMgr.IsSelf(strAccountToAdd))
	{
		nRelationType = 1;
	}
	else if (m_userMgr.IsFriend(strAccountToAdd))
	{
		nRelationType = 2;
	}

	if(nRelationType != 0)
	{
		TCHAR szData[64];
		CUserSnapInfoDlg userSnapInfoDlg;
		//userSnapInfoDlg.SetUserFaceID(m_pFMGClient->m_UserMgr.GetFaceID(strAccountToAdd));
		memset(szData, 0, sizeof(szData));
		_stprintf_s(szData, ARRAYSIZE(szData), _T("账户：%s"), strAccountToAdd.GetBuffer());
		strAccountToAdd.ReleaseBuffer();
		userSnapInfoDlg.SetAccountName(szData);
		memset(szData, 0, sizeof(szData));
		//_stprintf_s(szData, ARRAYSIZE(szData), _T("昵称：%s"), m_pFMGClient->m_UserMgr.GetNickName(strAccountToAdd).c_str());
		userSnapInfoDlg.SetNickName(szData);
		if (nRelationType == 1)
		{
			userSnapInfoDlg.SetOperationDescription(_T("您不能添加自己为好友。"));
		}
		else if (nRelationType == 2)
		{
			userSnapInfoDlg.SetOperationDescription(_T("该账户已经是您的好友，无需添加。"));
		}

		userSnapInfoDlg.SetOKButtonText(_T("加为好友"));
		userSnapInfoDlg.EnableOKButton(FALSE);

		userSnapInfoDlg.DoModal(m_hWnd, NULL);
	
		return;
	}
	

	//添加好友
	if (m_btnFindTypeSingle.GetCheck())
	{
		long nType = m_btnFindTypeSingle.GetCheck() ? 1 : 2;
		//查找账户并添加好友	
		//m_pFMGClient->FindFriend(strAccountToAdd, nType, m_hWnd);

		auto pUtil = CMsgProto::GetInstance();
		char szData[64] = { 0 };
		EncodeUtil::UnicodeToUtf8(strAccountToAdd, szData, ARRAYSIZE(szData));
		pUtil->SendFindFriendAsnyc(szData, szData, m_hWnd);
		m_staticAddInfo.SetWindowText(_T("正在查找用户，请稍等..."));
	}
	//加入群组 TODO
	else
	{
		long nType = m_btnFindTypeSingle.GetCheck() ? 1 : 2;
		//查找账户并添加好友	
		//m_pFMGClient->FindFriend(strAccountToAdd, nType, m_hWnd);

		auto pUtil = CMsgProto::GetInstance();
		char szData[64] = { 0 };
		EncodeUtil::UnicodeToUtf8(strAccountToAdd, szData, ARRAYSIZE(szData));
		pUtil->AddMap(E_MsgType::FindGroupRsp_Type, m_hWnd);
		pUtil->SendFindGroupReq(szData);
		m_staticAddInfo.SetWindowText(_T("正在查找群组，请稍等..."));
	}
}

/**
 * @brief 响应查找好友消息的回复
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @param bHandled 
 * @return LRESULT 
 */
LRESULT CFindFriendDlg::OnFindFriendResult(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	C_WND_MSG_FindFriendResult* pResult = (C_WND_MSG_FindFriendResult*)lParam;
	if (pResult == NULL)
	{
		return 0;
	}
	
	E_UI_ADD_FRIEND_RESULT nResultCode = pResult->m_nResultCode;
	
	if (nResultCode == E_UI_ADD_FRIEND_RESULT::FIND_FRIEND_NOT_FOUND)
	{
		m_staticAddInfo.SetWindowText(_T("您查找的账户不存在！"));
	}
	else if (nResultCode == E_UI_ADD_FRIEND_RESULT::FIND_FRIEND_FAILED)
	{
		m_staticAddInfo.SetWindowText(_T("网络故障，查找账户失败！"));
	}
	else if(nResultCode == E_UI_ADD_FRIEND_RESULT::FIND_FRIEND_FOUND)
	{
		m_staticAddInfo.SetWindowText(_T(""));
		
		CUserSnapInfoDlg userSnapInfoDlg;

        //BOOL bGroup = IsGroupTarget(pResult->m_uAccountID);
		
		//userSnapInfoDlg.SetAccountID(pResult->m_uAccountID);
		BOOL bGroup = (m_btnFindTypeSingle.GetCheck() == 0);
		TCHAR szInfo[32] = {0};
		userSnapInfoDlg.SetUserFaceID(0);
		TCHAR szAccount[32] = {0};
        EncodeUtil::Utf8ToUnicode(pResult->m_szAccountName, szAccount, ARRAYSIZE(szAccount));
		if(bGroup)
		{
			_stprintf_s(szInfo, ARRAYSIZE(szInfo), _T("群账户：%s"), szAccount);
		}	
		else
		{
			_stprintf_s(szInfo, ARRAYSIZE(szInfo), _T("账户：%s"), szAccount);
		}
		userSnapInfoDlg.SetAccountName(szInfo);

		TCHAR szNickName[32] = {0};
		memset(szInfo, 0, sizeof(szInfo));
        EncodeUtil::Utf8ToUnicode(pResult->m_szNickName, szNickName, ARRAYSIZE(szNickName));
		if (bGroup)
		{
			_stprintf_s(szInfo, ARRAYSIZE(szInfo), _T("群名称：%s"), szNickName);
		}
		else
		{
			_stprintf_s(szInfo, ARRAYSIZE(szInfo), _T("昵称：%s"), szNickName);
		}
		userSnapInfoDlg.SetNickName(szInfo);

		if (bGroup)
		{
			userSnapInfoDlg.SetOKButtonText(_T("加入该群"));
		}
		else
		{
			userSnapInfoDlg.SetOKButtonText(_T("加为好友"));
		}
		
		//if(bGroup)
		/*{
			userSnapInfoDlg.SetOperationDescription(_T("您已经是该群成员，不能重复添加。"));
			userSnapInfoDlg.EnableOKButton(FALSE);
		}*/

		if(userSnapInfoDlg.DoModal(m_hWnd, NULL) == IDOK)
		{
			if (bGroup)
			{
				m_staticAddInfo.SetWindowText(_T("您的请求已经发送，请等待对方回应。"));
				auto pUtil = CMsgProto::GetInstance();
				//char szData[64] = { 0 };
				//EncodeUtil::UnicodeToUtf8(strAccountToAdd, szData, ARRAYSIZE(szData));
				//pUtil->SendFindFriendAsnyc(szData, szData, m_hWnd);
				//m_pFMGClient->AddFriend(pResult->m_uAccountID);
				//pUtil->(pResult->m_szAccountId, pResult->m_szAccountName, m_hWnd);
				pUtil->SendAddToGroupReq(pResult->m_szAccountId);
			}
			else
			{
				m_staticAddInfo.SetWindowText(_T("您的请求已经发送，请等待对方回应。"));
				auto pUtil = CMsgProto::GetInstance();
				//char szData[64] = { 0 };
				//EncodeUtil::UnicodeToUtf8(strAccountToAdd, szData, ARRAYSIZE(szData));
				//pUtil->SendFindFriendAsnyc(szData, szData, m_hWnd);
				//m_pFMGClient->AddFriend(pResult->m_uAccountID);
				pUtil->SendAddFriendAsnyc(pResult->m_szAccountId, pResult->m_szAccountName, m_hWnd);
			}
		}
			
	}
	
	delete pResult;

	m_btnAdd.EnableWindow(TRUE);
	m_edtAddId.EnableWindow(TRUE);
		
	return (LRESULT)1;

}

/**
 * @brief 
 * TODO: 具体作用不清楚
 * @param dc 
 * @param wndStatic 
 * @return HBRUSH 
 */
HBRUSH CFindFriendDlg::OnCtlColorStatic(CDCHandle dc, CStatic wndStatic)
{
	if (wndStatic == m_staticAddInfo) 
	{
		::SetTextColor(dc, RGB(255, 0, 0));
		::SetBkMode(dc, TRANSPARENT); 
		return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	}

	SetMsgHandled(FALSE);

	return 0;
}

/**
 * @brief 响应对话框关闭
 * 
 */
void CFindFriendDlg::OnClose()
{
	m_staticAddInfo.SetWindowText(_T(""));
	ShowWindow(SW_HIDE);
}

/**
 * @brief 响应对话框销毁
 * 
 */
void CFindFriendDlg::OnDestroy()
{
	UninitUI();
}


/**
 * @brief 反初始化UI
 * 
 */
void CFindFriendDlg::UninitUI()
{
	if (m_edtAddId.IsWindow())
	{
		m_edtAddId.DestroyWindow();
	}



	if (m_btnAdd.IsWindow())
	{
		m_btnAdd.DestroyWindow();
	}

	if (m_btnFindTypeSingle.IsWindow())
	{
		m_btnFindTypeSingle.DestroyWindow();
	}

	if (m_btnFindTypeGroup.IsWindow())
	{
		m_btnFindTypeGroup.DestroyWindow();
	}

	if (m_staticAddInfo.IsWindow())
	{
		m_staticAddInfo.DestroyWindow();
	}
}


/**
 * @brief 加入到消息循环
 * 
 */
void CFindFriendDlg::AddMessageFilter()
{
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
}


/**
 * @brief 从消息循环移除
 * 
 */
void CFindFriendDlg::RemoveMessageFilter()
{
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
}


/**
 * @brief 显示窗口 TODO函数的调用需要确定
 * 
 * @param bShow 
 * @param nStatus 
 */
void CFindFriendDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (bShow)
	{
		AddMessageFilter();
	}
	else
	{
		RemoveMessageFilter();
	}
}