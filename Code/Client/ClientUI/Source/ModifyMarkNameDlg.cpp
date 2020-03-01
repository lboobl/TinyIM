/**
 * @file ModifyMarkNameDlg.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 修改备注的对话框的实现类
 * @version 0.1
 * @date 2020-03-01
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "stdafx.h"
#include "ModifyMarkNameDlg.h"
#include "UIText.h"

/**
 * @brief Construct a new CModifyMarkNameDlg::CModifyMarkNameDlg object
 * 
 */
CModifyMarkNameDlg::CModifyMarkNameDlg()
{
}

/**
 * @brief Destroy the CModifyMarkNameDlg::CModifyMarkNameDlg object
 * 
 */
CModifyMarkNameDlg::~CModifyMarkNameDlg()
{

}

/**
 * @brief 响应初始化对话框
 * 
 * @param wndFocus 
 * @param lInitParam 
 * @return BOOL 
 */
BOOL CModifyMarkNameDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	InitUI();
	CenterWindow(::GetDesktopWindow());
	return TRUE;
}

/**
 * @brief 初始化UI
 * 
 * @return BOOL 
 */
BOOL CModifyMarkNameDlg::InitUI()
{
	m_SkinDlg.SetBgPic(_T("DlgBg\\AddFriendDlgBg.png"));
	m_SkinDlg.SetCloseSysBtnPic(_T("SysBtn\\btn_close_normal.png"), _T("SysBtn\\btn_close_highlight.png"), _T("SysBtn\\btn_close_down.png"));
	m_SkinDlg.SubclassWindow(m_hWnd);
	m_SkinDlg.SetTitleText(_T("修改备注姓名"));	
	m_SkinDlg.MoveWindow(0, 0, 300, 135, TRUE);

	HDC hDlgBgDC = m_SkinDlg.GetBgDC();

	m_staMarkName.SetTransparent(TRUE, hDlgBgDC);
	m_staMarkName.SubclassWindow(GetDlgItem(IDC_STATIC_MARK_NAME));

	m_edtMarkName.SetBgNormalPic(_T("frameBorderEffect_normalDraw.png"), CRect(2,2,2,2));
	m_edtMarkName.SetBgHotPic(_T("frameBorderEffect_mouseDownDraw.png"), CRect(2,2,2,2));
	m_edtMarkName.SetTransparent(TRUE, hDlgBgDC);
	m_edtMarkName.SubclassWindow(GetDlgItem(IDC_EDIT_MARK_NAME));

	m_btnOK.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnOK.SetTransparent(TRUE, hDlgBgDC);
	m_btnOK.SetBgPic(_T("Button\\btn_normal.png"), _T("Button\\btn_focus.png"),_T("Button\\btn_focus.png"), _T("Button\\btn_focus.png"));
	m_btnOK.SetRound(4, 4);
	m_btnOK.SubclassWindow(GetDlgItem(IDOK));
	
	m_btnCancel.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnCancel.SetTransparent(TRUE, hDlgBgDC);
	m_btnCancel.SetBgPic(_T("Button\\btn_normal.png"), _T("Button\\btn_focus.png"),_T("Button\\btn_focus.png"), _T("Button\\btn_focus.png"));
	m_btnCancel.SetRound(4, 4);
	m_btnCancel.SubclassWindow(GetDlgItem(IDCANCEL));
	
	
	return TRUE;
}


/**
 * @brief 反初始UI
 * 
 */
void CModifyMarkNameDlg::UninitUI()
{
	if(m_staMarkName.IsWindow())
		m_staMarkName.DestroyWindow();

	if(m_edtMarkName.IsWindow())
		m_edtMarkName.DestroyWindow();

	if(m_btnOK.IsWindow())
		m_btnOK.DestroyWindow();

	if(m_btnCancel.IsWindow())
		m_btnCancel.DestroyWindow();
}

/**
 * @brief 响应对话框关闭
 * 
 */
void CModifyMarkNameDlg::OnClose()
{
	EndDialog(IDCANCEL);
}

/**
 * @brief 响应对话框销毁
 * 
 */
void CModifyMarkNameDlg::OnDestroy()
{
	UninitUI();
}


/**
 * @brief 响应修改按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CModifyMarkNameDlg::OnOK(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strMarkName;
	m_edtMarkName.GetWindowText(strMarkName);
	strMarkName.Trim();
	if(strMarkName.IsEmpty())
	{
		//::MessageBox(m_hWnd, _T("备注姓名不能为空。"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}
	else if(strMarkName.GetLength() >= 16)
	{
		//::MessageBox(m_hWnd, _T("备注姓名不能超过15个字符。"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}
	
	//C_UI_BuddyInfo* pBuddyInfo = m_pFMGClient->m_UserMgr.m_BuddyList.GetBuddy(m_uUserID);
	//if(pBuddyInfo != NULL)
	//	pBuddyInfo->m_strMarkName = strMarkName;

    //m_pFMGClient->ModifyFriendMarkName(m_uUserID, strMarkName);

	EndDialog(IDOK);
}

/**
 * @brief 响应取消按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CModifyMarkNameDlg::OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	EndDialog(IDCANCEL);
}