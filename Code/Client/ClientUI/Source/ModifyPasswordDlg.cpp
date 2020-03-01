/**
 * @file ModifyPasswordDlg.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 修改密码对话框的实现
 * @version 0.1
 * @date 2019-08-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "stdafx.h"
#include "ModifyPasswordDlg.h"
#include "UIText.h"

/**
 * @brief Construct a new CModifyPasswordDlg::CModifyPasswordDlg object
 * 
 */
CModifyPasswordDlg::CModifyPasswordDlg() //: //m_pFMGClient(NULL)
{

}

/**
 * @brief Destroy the CModifyPasswordDlg::CModifyPasswordDlg object
 * 
 */
CModifyPasswordDlg::~CModifyPasswordDlg()
{

}

/**
 * @brief 预处理消息
 * 
 * @param pMsg 
 * @return BOOL 
 */
BOOL CModifyPasswordDlg::PreTranslateMessage(MSG* pMsg)
{
	//支持Esc关闭对话框
	if (pMsg->hwnd==m_hWnd && pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)
 	{
		PostMessage(WM_CLOSE, 0, 0);
		return TRUE;
 	}
	else if(pMsg->hwnd==m_hWnd && pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		PostMessage(WM_COMMAND, IDOK, 0);
		return TRUE;
	}

	return CWindow::IsDialogMessage(pMsg);
}

/**
 * @brief 初始化对话框
 * 
 * @param wndFocus 
 * @param lInitParam 
 * @return BOOL 
 */
BOOL CModifyPasswordDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	InitUI();

	CenterWindow();

	return TRUE;
}

/**
 * @brief 响应显示窗口
 * 
 * @param bShow 
 * @param nStatus 
 */
void CModifyPasswordDlg::OnShowWindow(BOOL bShow, UINT nStatus)
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

/**
 * @brief 初始化UI
 * 
 * @return BOOL 
 */
BOOL CModifyPasswordDlg::InitUI()
{
	m_SkinDlg.SetBgPic(_T("DlgBg\\GeneralBg.png"));
	m_SkinDlg.SetCloseSysBtnPic(_T("SysBtn\\btn_close_normal.png"), _T("SysBtn\\btn_close_highlight.png"), _T("SysBtn\\btn_close_down.png"));
	m_SkinDlg.SubclassWindow(m_hWnd);
	m_SkinDlg.SetTitleText(_T("密码修改"));
	m_SkinDlg.MoveWindow(0, 0, 427, 320, TRUE);

	HDC hDlgBgDC = m_SkinDlg.GetBgDC();

	m_edtOldPassword.SetBgNormalPic(_T("frameBorderEffect_normalDraw.png"), CRect(2,2,2,2));
	m_edtOldPassword.SetBgHotPic(_T("frameBorderEffect_mouseDownDraw.png"), CRect(2,2,2,2));
	m_edtOldPassword.SetTransparent(TRUE, hDlgBgDC);
	m_edtOldPassword.SubclassWindow(GetDlgItem(IDC_EDIT_OLD_PASS_WORD));
	m_edtOldPassword.SetPasswordChar(0x25CF);

	m_edtNewPassword.SetBgNormalPic(_T("frameBorderEffect_normalDraw.png"), CRect(2,2,2,2));
	m_edtNewPassword.SetBgHotPic(_T("frameBorderEffect_mouseDownDraw.png"), CRect(2,2,2,2));
	m_edtNewPassword.SubclassWindow(GetDlgItem(IDC_EDIT_NEW_PASS_WORD));
	m_edtNewPassword.SetPasswordChar(0x25CF);

	m_edtNewPasswordConfirm.SetBgNormalPic(_T("frameBorderEffect_normalDraw.png"), CRect(2,2,2,2));
	m_edtNewPasswordConfirm.SetBgHotPic(_T("frameBorderEffect_mouseDownDraw.png"), CRect(2,2,2,2));
	m_edtNewPasswordConfirm.SubclassWindow(GetDlgItem(IDC_EDIT_NEW_PASS_CONFIRM));
	m_edtNewPasswordConfirm.SetPasswordChar(0x25CF);

	m_btnOK.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnOK.SetTransparent(TRUE, hDlgBgDC);
	m_btnOK.SetBgPic(_T("Button\\btn_normal.png"), _T("Button\\btn_highlight.png"),
		_T("Button\\btn_down.png"), _T("Button\\btn_focus.png"));
	m_btnOK.SetRound(4, 4);
	m_btnOK.SubclassWindow(GetDlgItem(IDOK));
	//m_btnOK.MoveWindow(165, 265 + 30, 94, 30, TRUE);

	m_btnCancel.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnCancel.SetTransparent(TRUE, hDlgBgDC);
	m_btnCancel.SetBgPic(_T("Button\\btn_normal.png"), _T("Button\\btn_highlight.png"),
		_T("Button\\btn_down.png"), _T("Button\\btn_focus.png"));
	m_btnCancel.SetRound(4, 4);
	m_btnCancel.SubclassWindow(GetDlgItem(IDCANCEL));
	//m_btnCancel.MoveWindow(290, 265 + 30, 94, 30, TRUE);

	return TRUE;
}

/**
 * @brief 响应关闭消息
 * 
 */
void CModifyPasswordDlg::OnClose()
{
	Reset();
	ShowWindow(SW_HIDE);
}


/**
 * @brief 响应销毁消息
 * 
 */
void CModifyPasswordDlg::OnDestroy()
{
	UninitUI();
}

/**
 * @brief 反初始化UI
 * 
 */
void CModifyPasswordDlg::UninitUI()
{
	if (m_edtOldPassword.IsWindow())
		m_edtOldPassword.DestroyWindow();

	if (m_edtNewPassword.IsWindow())
		m_edtNewPassword.DestroyWindow();

	if (m_edtNewPasswordConfirm.IsWindow())
		m_edtNewPasswordConfirm.DestroyWindow();

	if (m_btnOK.IsWindow())
		m_btnOK.DestroyWindow();

	if (m_btnCancel.IsWindow())
		m_btnCancel.DestroyWindow();

}

/**
 * @brief 添加到消息处理
 * 
 */
void CModifyPasswordDlg::AddMessageFilter()
{
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
}

/**
 * @brief 重设所有参数
 * 
 */
void CModifyPasswordDlg::Reset()
{
	m_edtOldPassword.SetWindowText(_T(""));
	m_edtNewPassword.SetWindowText(_T(""));
	m_edtNewPasswordConfirm.SetWindowText(_T(""));

	m_btnOK.EnableWindow(TRUE);
	m_btnCancel.EnableWindow(TRUE);
	m_SkinDlg.SetTitleText(_T("密码修改"));
}

/**
 * @brief 从消息循环中移除
 * 
 */
void CModifyPasswordDlg::RemoveMessageFilter()
{
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
}

/**
 * @brief 响应OK按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CModifyPasswordDlg::OnBtn_OK(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strOldPassword;
	m_edtOldPassword.GetWindowText(strOldPassword);
	//if(strOldPassword != m_pFMGClient->m_UserMgr.m_UserInfo.m_strPassword.c_str())
	{
		::MessageBox(m_hWnd, _T("旧密码不正确！"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		m_edtOldPassword.SetFocus();
		//return;
	}
	
	CString strNewPassword;
	m_edtNewPassword.GetWindowText(strNewPassword);
	if(strNewPassword.IsEmpty() || strNewPassword.GetLength()<3 || strNewPassword.GetLength()>15)
	{
		::MessageBox(m_hWnd, _T("新密码长度必须在3～15个字符之间！"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		m_edtNewPassword.SetFocus();
		return;
	}

	CString strNewPasswordConfirm;
	m_edtNewPasswordConfirm.GetWindowText(strNewPasswordConfirm);
	if (strNewPassword != strNewPasswordConfirm)
	{
		::MessageBox(m_hWnd, _T("两次输入的密码不相同，请确认！"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}

	if(strOldPassword == strNewPassword)
	{
		::MessageBox(m_hWnd, _T("新旧密码必须不相同！"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}
	
	m_SkinDlg.SetTitleText(_T("密码修改(正在修改密码，请稍等...)"));
	m_btnOK.EnableWindow(FALSE);	
	m_btnCancel.EnableWindow(FALSE);
	//m_pFMGClient->SetModifyPasswordWindow(m_hWnd);
	//m_pFMGClient->ModifyPassword(strOldPassword, strNewPassword);
	
}

/**
 * @brief 响应Cancel按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CModifyPasswordDlg::OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl)	
{
	PostMessage(WM_CLOSE, 0, 0);
}


/**
 * @brief 响应修改密码的结果
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @param bHandled 
 * @return LRESULT 
 */
LRESULT CModifyPasswordDlg::OnModifyPasswordResult(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	Reset();

	if(wParam == 0)
	{
		::MessageBox(m_hWnd, _T("密码修改成功，您下次可以使用新密码登录了！"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		ShowWindow(SW_HIDE);
	}
    else if (wParam == 103)
        ::MessageBox(m_hWnd, _T("旧密码错误！"), g_strAppTitle.c_str(), MB_OK | MB_ICONINFORMATION);
	else/* if (wParam == 105)*/
		::MessageBox(m_hWnd, _T("密码修改失败，请稍后重试！"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);

	return (LRESULT)1;
}