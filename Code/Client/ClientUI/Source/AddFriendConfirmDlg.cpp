/**
 * @file AddFriendConfirmDlg.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 添加好友确认对话框
 * @version 0.1
 * @date 2019-08-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#include "stdafx.h"
#include "AddFriendConfirmDlg.h"

// CAddFriendConfirmDlg实现代码
CAddFriendConfirmDlg::CAddFriendConfirmDlg()
{
	m_bShowAgreeButton = TRUE;
	m_bShowRefuseButton = TRUE;
	m_bShowOKButton = FALSE;
}

CAddFriendConfirmDlg::~CAddFriendConfirmDlg()
{
}

/**
 * @brief 设置窗口标题
 * 
 * @param pszWindowTitle 标题文本
 */
void CAddFriendConfirmDlg::SetWindowTitle(PCTSTR pszWindowTitle)
{
	m_strWindowTitle = pszWindowTitle;
}

/**
 * @brief 设置窗口内容
 * 
 * @param pszWindowInfo 窗口内容
 */
void CAddFriendConfirmDlg::SetWindowInfo(PCTSTR pszWindowInfo)
{
	m_strWindowInfo = pszWindowInfo;
}
	

/**
 * @brief 设置是否显示同意按钮
 * 
 * @param bShow 是否显示同意按钮
 */
void CAddFriendConfirmDlg::ShowAgreeButton(BOOL bShow)
{
	m_bShowAgreeButton = bShow;
}

/**
 * @brief 设置是否显示拒绝按钮
 * 
 * @param bShow 是否显示拒绝按钮
 */
void CAddFriendConfirmDlg::ShowRefuseButton(BOOL bShow)
{
	m_bShowRefuseButton = bShow;
}

/**
 * @brief 设置是否显示OK按钮
 * 
 * @param bShow 是否显示OK按钮
 */
void CAddFriendConfirmDlg::ShowOKButton(BOOL bShow)
{
	m_bShowOKButton = bShow;
}

/**
 * @brief 初始化对话框,对话框居中在此处完成
 * 
 * @param wndFocus 
 * @param lInitParam 
 * @return BOOL 
 */
BOOL CAddFriendConfirmDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	InitUI();
	SetWindowText(m_strWindowTitle);

	CenterWindow(::GetDesktopWindow());
	::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

	return TRUE;
}

/**
 * @brief 初始化界面
 * 
 * @return BOOL 
 */
BOOL CAddFriendConfirmDlg::InitUI()
{
	m_SkinDlg.SetBgPic(_T("DlgBg\\MsgBoxDlgBg.png"), CRect(4, 32, 4, 33));
	m_SkinDlg.SetCloseSysBtnPic(_T("SysBtn\\btn_close_normal.png"), _T("SysBtn\\btn_close_highlight.png"), _T("SysBtn\\btn_close_down.png"));
	m_SkinDlg.SubclassWindow(m_hWnd);
	m_SkinDlg.SetTitleText(m_strWindowTitle);
	m_SkinDlg.MoveWindow(0, 0, 400, 160, TRUE);

	HDC hDlgBgDC = m_SkinDlg.GetBgDC();

	m_btnAgree.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnAgree.SetTransparent(TRUE, hDlgBgDC);
	m_btnAgree.SetBgPic(_T("Button\\btn_normal.png"), _T("Button\\btn_focus.png"),_T("Button\\btn_focus.png"), _T("Button\\btn_focus.png"));
	m_btnAgree.SetRound(4, 4);
	m_btnAgree.SubclassWindow(GetDlgItem(ID_ADD_CONFIRM_AGREE));
	m_btnAgree.ShowWindow(m_bShowAgreeButton? SW_SHOW : SW_HIDE);

	m_btnRefuse.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnRefuse.SetTransparent(TRUE, hDlgBgDC);
	m_btnRefuse.SetBgPic(_T("Button\\btn_normal.png"), _T("Button\\btn_focus.png"),_T("Button\\btn_focus.png"), _T("Button\\btn_focus.png"));
	m_btnRefuse.SubclassWindow(GetDlgItem(ID_ADDCONFIRM_REFUSE));
	m_btnRefuse.ShowWindow(m_bShowRefuseButton? SW_SHOW : SW_HIDE);

	m_btnOK.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnOK.SetTransparent(TRUE, hDlgBgDC);
	m_btnOK.SetBgPic(_T("Button\\btn_normal.png"), _T("Button\\btn_focus.png"),_T("Button\\btn_focus.png"), _T("Button\\btn_focus.png"));
	m_btnOK.SubclassWindow(GetDlgItem(IDOK));
	m_btnOK.ShowWindow(m_bShowOKButton? SW_SHOW : SW_HIDE);

	m_staticAddConfirmInfo.SetTransparent(TRUE, hDlgBgDC);
	m_staticAddConfirmInfo.SetLinkColor(RGB(225, 0, 0));
	m_staticAddConfirmInfo.SetLinkType(SKIN_LINK_ADDNEW);
	m_staticAddConfirmInfo.SubclassWindow(GetDlgItem(IDC_ADD_CONFIRM));
	
	m_staticAddConfirmInfo.SetWindowText(m_strWindowInfo);

	return TRUE;
}

/**
 * @brief 响应关闭按钮
 * 
 */
void CAddFriendConfirmDlg::OnClose()
{
	EndDialog(IDOK);
}

/**
 * @brief 响应对话框销毁
 * 
 */
void CAddFriendConfirmDlg::OnDestroy()
{
	UninitUI();
}


/**
 * @brief 响应OK按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CAddFriendConfirmDlg::OnOK(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	EndDialog(IDOK);
}


/**
 * @brief 响应同意或者拒绝
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CAddFriendConfirmDlg::OnAgreeOrReject(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (1108 == nID)
	{
		m_bAgree = true;
	}
	else
	{
		m_bAgree = false;
	}
	EndDialog(nID);
}

/**
 * @brief 反初始化UI
 * 
 */
void CAddFriendConfirmDlg::UninitUI()
{
	if (m_btnAgree.IsWindow())
	{
		m_btnAgree.DestroyWindow();
	}	

	if (m_btnRefuse.IsWindow())
	{
		m_btnRefuse.DestroyWindow();
	}	

	if (m_staticAddConfirmInfo.IsWindow())
	{
		m_staticAddConfirmInfo.DestroyWindow();	
	}	
}