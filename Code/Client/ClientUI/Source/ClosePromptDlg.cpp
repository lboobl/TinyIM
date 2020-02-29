/**
 * @file ClosePromptDlg.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 关闭通知对话框的实现类
 * @version 0.1
 * @date 2020-02-29
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "stdafx.h"
#include "ClosePromptDlg.h"
#include "UI_USER_INFO.h"

/**
 * @brief Construct a new CClosePromptDlg::CClosePromptDlg object
 * 
 */
CClosePromptDlg::CClosePromptDlg()
{
	//m_pFMGClient = NULL;
}

/**
 * @brief Destroy the CClosePromptDlg::CClosePromptDlg object
 * 
 */
CClosePromptDlg::~CClosePromptDlg()
{

}

/**
 * @brief 响应初始化对话框
 * 
 * @param wndFocus 
 * @param lInitParam 
 * @return BOOL 
 */
BOOL CClosePromptDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
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
BOOL CClosePromptDlg::InitUI()
{
	m_SkinDlg.SetBgPic(_T("DlgBg\\MsgBoxDlgBg.png"), CRect(4, 69, 4, 33));
	m_SkinDlg.SetCloseSysBtnPic(_T("SysBtn\\btn_close_normal.png"), _T("SysBtn\\btn_close_highlight.png"), _T("SysBtn\\btn_close_down.png"));
	m_SkinDlg.SubclassWindow(m_hWnd);
	m_SkinDlg.SetTitleText(_T("退出提示"));
	m_SkinDlg.MoveWindow(0, 0, 330, 160, FALSE);

	HDC hDlgBgDC = m_SkinDlg.GetBgDC();

	m_picInfoIcon.SetTransparent(TRUE, hDlgBgDC);
	CString strPath;
	strPath.Format(_T("%sSkins\\Skin0\\MsgBoxIcon\\question.png"), g_szHomePath);
	m_picInfoIcon.SetBitmap(strPath);
	m_picInfoIcon.SubclassWindow(GetDlgItem(IDC_INFO_ICON));
	m_picInfoIcon.MoveWindow(50, 35, 32, 32, FALSE);
	
	m_staInfoText.SetTransparent(TRUE, hDlgBgDC);
	m_staInfoText.SubclassWindow(GetDlgItem(IDC_INFO_TEXT));
	m_staInfoText.SetTextColor(RGB(0, 0, 0));
	m_staInfoText.MoveWindow(85, 40, 270, 20, FALSE);
	
	m_btnRememberChoice.SetButtonType(SKIN_CHECKBOX);
	m_btnRememberChoice.SetTransparent(TRUE, hDlgBgDC);
	m_btnRememberChoice.SubclassWindow(GetDlgItem(IDC_REMEMBER_CHOICE));
	m_btnRememberChoice.SetCheckBoxPic(_T("CheckBox\\checkbox_normal.png"), _T("CheckBox\\checkbox_hightlight.png"), _T("CheckBox\\checkbox_tick_normal.png"), _T("CheckBox\\checkbox_tick_highlight.png"));
	//m_btnRememberChoice.SetCheck(!m_pFMGClient->m_UserConfig.IsEnableExitPrompt());

	m_btnMinimize.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnMinimize.SetTransparent(TRUE, hDlgBgDC);
	m_btnMinimize.SetBgPic(_T("Button\\btn_normal.png"), _T("Button\\btn_focus.png"),_T("Button\\btn_focus.png"), _T("Button\\btn_focus.png"));
	m_btnMinimize.SetTextColor(RGB(0, 0, 0));
	m_btnMinimize.SubclassWindow(GetDlgItem(IDC_MINIMIZE));
	m_btnMinimize.SetWindowText(_T("最小化"));

	m_btnExit.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnExit.SetTransparent(TRUE, hDlgBgDC);
	m_btnExit.SetBgPic(_T("Button\\btn_normal.png"), _T("Button\\btn_focus.png"),_T("Button\\btn_focus.png"), _T("Button\\btn_focus.png"));
	m_btnExit.SetTextColor(RGB(0, 0, 0));
	m_btnExit.SubclassWindow(GetDlgItem(IDC_EXIT));
	m_btnExit.SetWindowText(_T("退出"));

	return TRUE;
}

/**
 * @brief 反初始化UI
 * 
 */
void CClosePromptDlg::UninitUI()
{
	if(m_picInfoIcon.IsWindow())
		m_picInfoIcon.DestroyWindow();
}

/**
 * @brief 响应最小化按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CClosePromptDlg::OnMinimize(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	//->m_UserConfig.EnableExitPrompt(!m_btnRememberChoice.GetCheck());
	//m_pFMGClient->m_UserConfig.EnableExitWhenCloseMainDlg(FALSE);
	EndDialog(IDC_MINIMIZE);
}

/**
 * @brief 响应退出按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CClosePromptDlg::OnExit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	//m_pFMGClient->m_UserConfig.EnableExitPrompt(!m_btnRememberChoice.GetCheck());
	//m_pFMGClient->m_UserConfig.EnableExitWhenCloseMainDlg(TRUE);
	EndDialog(IDC_EXIT);
}

/**
 * @brief 响应关闭按钮
 * 
 */
void CClosePromptDlg::OnClose()
{
	EndDialog(IDCANCEL);
}

/**
 * @brief 响应销毁按钮
 * 
 */
void CClosePromptDlg::OnDestroy()
{
	UninitUI();
}

