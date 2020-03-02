/**
 * @file SystemSettingDlg.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 系统设置对话框的实现文件
 * @version 0.1
 * @date 2019-08-04
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "stdafx.h"
#include "SystemSettingDlg.h"
#include <assert.h>

/**
 * @brief Construct a new CSystemSettingDlg::CSystemSettingDlg object
 * 
 */
CSystemSettingDlg::CSystemSettingDlg(void)
{
	m_hDlgIcon = NULL;
	m_hDlgSmallIcon = NULL;
	m_hFont = NULL;
}

/**
 * @brief Destroy the CSystemSettingDlg::CSystemSettingDlg object
 * 
 */
CSystemSettingDlg::~CSystemSettingDlg(void)
{
}

/**
 * @brief 响应初始化对话框
 * 
 * @param wndFocus 
 * @param lInitParam 
 * @return BOOL 
 */
BOOL CSystemSettingDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	SetWindowPos(NULL, 0, 0, 480, 450, SWP_NOMOVE);
	::SetForegroundWindow(m_hWnd);

	// set icons
	m_hDlgIcon = AtlLoadIconImage(IDR_MAIN_FRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(m_hDlgIcon, TRUE);
	m_hDlgSmallIcon = AtlLoadIconImage(IDR_MAIN_FRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(m_hDlgSmallIcon, FALSE);

	InitUI();

	CenterWindow(::GetDesktopWindow());

	return TRUE;
}


/**
 * @brief 响应关闭对话框
 * 
 */
void CSystemSettingDlg::OnClose()
{
	EndDialog(IDCANCEL);
}

/**
 * @brief 响应销毁对话框
 * 
 */
void CSystemSettingDlg::OnDestroy()
{
	if (m_hDlgIcon != NULL)
	{
		::DestroyIcon(m_hDlgIcon);
		m_hDlgIcon = NULL;
	}

	if (m_hDlgSmallIcon != NULL)
	{
		::DestroyIcon(m_hDlgSmallIcon);
		m_hDlgSmallIcon = NULL;
	}

	UninitUI();

	if(m_hFont != NULL)
		::DeleteObject(m_hFont);
}

/**
 * @brief 初始化UI
 * 
 * @return BOOL 
 */
BOOL CSystemSettingDlg::InitUI()
{
	
	//主界面对话框
	m_SkinDlg.SetBgPic(_T("DlgBg\\GeneralBg.png"));
	m_SkinDlg.SetCloseSysBtnPic(_T("SysBtn\\btn_close_normal.png"), _T("SysBtn\\btn_close_highlight.png"), _T("SysBtn\\btn_close_down.png"));
	m_SkinDlg.SubclassWindow(m_hWnd);
	m_SkinDlg.SetTitleText(_T("ÏµÍ³ÉèÖÃ"));

	HDC hDlgBgDC = m_SkinDlg.GetBgDC();

	//静音按钮
	m_btnMute.SetButtonType(SKIN_CHECKBOX);
	m_btnMute.SetTransparent(TRUE, hDlgBgDC);
	m_btnMute.SetCheckBoxPic(_T("CheckBox\\checkbox_normal.png"), _T("CheckBox\\checkbox_hightlight.png"), _T("CheckBox\\checkbox_tick_normal.png"), _T("CheckBox\\checkbox_tick_highlight.png"));
	m_btnMute.SubclassWindow(GetDlgItem(IDC_MUTE));
    //m_btnMute.SetCheck(m_pFMGClient->m_UserConfig.IsEnableMute());

	//自动回复选择按钮
	m_btnAutoReply.SetButtonType(SKIN_CHECKBOX);
	m_btnAutoReply.SetTransparent(TRUE, hDlgBgDC);
	m_btnAutoReply.SetCheckBoxPic(_T("CheckBox\\checkbox_normal.png"), _T("CheckBox\\checkbox_hightlight.png"), _T("CheckBox\\checkbox_tick_normal.png"), _T("CheckBox\\checkbox_tick_highlight.png"));
	m_btnAutoReply.SubclassWindow(GetDlgItem(IDC_AUTO_REPLY));
	//m_btnAutoReply.SetCheck(m_pFMGClient->m_UserConfig.IsEnableAutoReply());

	//自动回复内容编辑框
	m_edtAutoReply.SetBgNormalPic(_T("frameBorderEffect_normalDraw.png"), CRect(2,2,2,2));
	m_edtAutoReply.SetBgHotPic(_T("frameBorderEffect_mouseDownDraw.png"), CRect(2,2,2,2));
	m_edtAutoReply.SetTransparent(TRUE, hDlgBgDC);
	m_edtAutoReply.SubclassWindow(GetDlgItem(IDC_AUTO_REPLY_CONTENT));
	//m_edtAutoReply.SetWindowText(m_pFMGClient->m_UserConfig.GetAutoReplyContent());
	m_edtAutoReply.EnableWindow(m_btnAutoReply.GetCheck());

	//阅后即焚按钮
	m_btnDestroyAfterRead.SetButtonType(SKIN_CHECKBOX);
	m_btnDestroyAfterRead.SetTransparent(TRUE, hDlgBgDC);
	m_btnDestroyAfterRead.SetCheckBoxPic(_T("CheckBox\\checkbox_normal.png"), _T("CheckBox\\checkbox_hightlight.png"), _T("CheckBox\\checkbox_tick_normal.png"), _T("CheckBox\\checkbox_tick_highlight.png"));
	m_btnDestroyAfterRead.SubclassWindow(GetDlgItem(IDC_DESTROY_AFTER_READ));

	
	m_cboDurationRead.SetTransparent(TRUE, hDlgBgDC);
	m_cboDurationRead.SetBgNormalPic(_T("frameBorderEffect_normalDraw.png"), CRect(2,2,2,2));
	m_cboDurationRead.SetBgHotPic(_T("frameBorderEffect_mouseDownDraw.png"), CRect(2,2,2,2));
	m_cboDurationRead.SubclassWindow(GetDlgItem(IDC_DESTROY_DURATION));
	m_cboDurationRead.SetArrowNormalPic(_T("ComboBox\\inputbtn_normal_bak.png"));
	m_cboDurationRead.SetArrowHotPic(_T("ComboBox\\inputbtn_highlight.png"));
	m_cboDurationRead.SetArrowPushedPic(_T("ComboBox\\inputbtn_down.png"));
	m_cboDurationRead.SetArrowWidth(20);
	m_cboDurationRead.SetTransparent(TRUE, hDlgBgDC);
	m_cboDurationRead.SetItemHeight(-1, 18);
	m_cboDurationRead.SetReadOnly(TRUE);
	::EnableWindow(GetDlgItem(IDC_DESTROY_AFTER_READ_LABEL), m_btnDestroyAfterRead.GetCheck());
	m_cboDurationRead.EnableWindow(m_btnDestroyAfterRead.GetCheck());
	
	const TCHAR szDuration[][8] = {_T("5Ãë"), _T("6Ãë"), _T("7Ãë"), _T("8Ãë"), _T("9Ãë"), _T("10Ãë")};
	for (long i = 0; i < ARRAYSIZE(szDuration); ++i)
	{
		m_cboDurationRead.InsertString(i, szDuration[i]);
	}
	m_cboDurationRead.SetCurSel(0);
	
	//允许撤回消息按钮
	m_btnRevokeChatMsg.SetButtonType(SKIN_CHECKBOX);
	m_btnRevokeChatMsg.SetTransparent(TRUE, hDlgBgDC);
	m_btnRevokeChatMsg.SetCheckBoxPic(_T("CheckBox\\checkbox_normal.png"), _T("CheckBox\\checkbox_hightlight.png"), _T("CheckBox\\checkbox_tick_normal.png"), _T("CheckBox\\checkbox_tick_highlight.png"));
	m_btnRevokeChatMsg.SubclassWindow(GetDlgItem(IDC_ENABLE_REVOKE_CHAT_MSG));

	//立即退出按钮
	m_btnExitPrompt.SetButtonType(SKIN_CHECKBOX);
	m_btnExitPrompt.SetTransparent(TRUE, hDlgBgDC);
	m_btnExitPrompt.SetCheckBoxPic(_T("CheckBox\\checkbox_normal.png"), _T("CheckBox\\checkbox_hightlight.png"), _T("CheckBox\\checkbox_tick_normal.png"), _T("CheckBox\\checkbox_tick_highlight.png"));
	m_btnExitPrompt.SubclassWindow(GetDlgItem(IDC_EXIT_PROMP));
	//m_btnExitPrompt.SetCheck(m_pFMGClient->m_UserConfig.IsEnableExitPrompt());

	//关闭时退出按钮设置
	m_btnExitWhenClose.SetButtonType(SKIN_CHECKBOX);
	m_btnExitWhenClose.SetTransparent(TRUE, hDlgBgDC);
	m_btnExitWhenClose.SetCheckBoxPic(_T("CheckBox\\checkbox_normal.png"), _T("CheckBox\\checkbox_hightlight.png"), _T("CheckBox\\checkbox_tick_normal.png"), _T("CheckBox\\checkbox_tick_highlight.png"));
	m_btnExitWhenClose.SubclassWindow(GetDlgItem(IDC_MINIMIZE_WHEN_CLOSE));
	//m_btnExitWhenClose.SetCheck(!m_pFMGClient->m_UserConfig.IsEnableExitWhenCloseMainDlg());

	//显示最近的消息按钮设置
	m_btnShowLastMsg.SetButtonType(SKIN_CHECKBOX);
	m_btnShowLastMsg.SetTransparent(TRUE, hDlgBgDC);
	m_btnShowLastMsg.SetCheckBoxPic(_T("CheckBox\\checkbox_normal.png"), _T("CheckBox\\checkbox_hightlight.png"), _T("CheckBox\\checkbox_tick_normal.png"), _T("CheckBox\\checkbox_tick_highlight.png"));
	m_btnShowLastMsg.SubclassWindow(GetDlgItem(IDC_CHECK_SHOW_LAST_MSG));
	//m_btnShowLastMsg.SetCheck(m_pFMGClient->m_UserConfig.IsEnableShowLastMsgInChatDlg());

	//OK按钮设置
	m_btnOK.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnOK.SetTransparent(TRUE, hDlgBgDC);
	m_btnOK.SetBgPic(_T("Button\\login_btn_normal.png"), _T("Button\\login_btn_highlight.png"), _T("Button\\login_btn_down.png"), _T("Button\\login_btn_focus.png"));
	m_btnOK.SetTextColor(RGB(255, 255, 255));
	m_btnOK.SetTextBoldStyle(TRUE);
	m_btnOK.SubclassWindow(GetDlgItem(IDOK));

	//取消按钮设置
	m_btnCancel.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnCancel.SetTransparent(TRUE, hDlgBgDC);
	m_btnCancel.SetBgPic(_T("Button\\login_btn_normal.png"), _T("Button\\login_btn_highlight.png"), _T("Button\\login_btn_down.png"), _T("Button\\login_btn_focus.png"));
	m_btnCancel.SetTextColor(RGB(255, 255, 255));
	m_btnCancel.SetTextBoldStyle(TRUE);
	m_btnCancel.SubclassWindow(GetDlgItem(IDCANCEL));
	
	return TRUE;
}

/**
 * @brief 响应勾选自动回复
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CSystemSettingDlg::OnCheckAutoReply(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_edtAutoReply.EnableWindow(m_btnAutoReply.GetCheck());
}


/**
 * @brief 响应阅后即焚勾选
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CSystemSettingDlg::OnCheckDestroyAfterRead(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	::EnableWindow(GetDlgItem(IDC_DESTROY_AFTER_READ_LABEL), m_btnDestroyAfterRead.GetCheck());
	m_cboDurationRead.EnableWindow(m_btnDestroyAfterRead.GetCheck());
}

/**
 * @brief 响应OK按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CSystemSettingDlg::OnBtnOK(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	//assert(m_pFMGClient != NULL);
    //m_pFMGClient->m_UserConfig.EnableMute(m_btnMute.GetCheck());
	//m_pFMGClient->m_UserConfig.EnableExitPrompt(m_btnExitPrompt.GetCheck());
	//m_pFMGClient->m_UserConfig.EnableExitWhenCloseMainDlg(!m_btnExitWhenClose.GetCheck());

	//m_pFMGClient->m_UserConfig.EnableAutoReply(m_btnAutoReply.GetCheck());
	
	CString strAutoReplyContent;
	m_edtAutoReply.GetWindowText(strAutoReplyContent);
	strAutoReplyContent.Trim();
	//if(strAutoReplyContent != "")
	//	m_pFMGClient->m_UserConfig.SetAutoReplyContent(strAutoReplyContent);

	//m_pFMGClient->m_UserConfig.EnableShowLastMsgInChatDlg(m_btnShowLastMsg.GetCheck());
	
	EndDialog(IDOK);
}

/**
 * @brief 响应取消按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CSystemSettingDlg::OnBtnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	EndDialog(IDCANCEL);
}

/**
 * @brief 反初始化UI
 * 
 */
void CSystemSettingDlg::UninitUI()
{
	if (m_btnMute.IsWindow())
	{
		m_btnMute.DestroyWindow();
	}	

	if (m_btnAutoReply.IsWindow())
	{
		m_btnAutoReply.DestroyWindow();
	}	

	if (m_edtAutoReply.IsWindow())
	{
		m_edtAutoReply.DestroyWindow();
	}	

	if (m_btnDestroyAfterRead.IsWindow())
	{
		m_btnDestroyAfterRead.DestroyWindow();
	}	

	if (m_cboDurationRead.IsWindow())
	{
		m_cboDurationRead.DestroyWindow();
	}
	if (m_btnRevokeChatMsg.IsWindow())
	{
		m_btnRevokeChatMsg.DestroyWindow();
	}	

	if(m_btnExitPrompt.IsWindow())
	{
		m_btnExitPrompt.DestroyWindow();
	}	
	if(m_btnExitWhenClose.IsWindow())
	{
		m_btnExitWhenClose.DestroyWindow();
	}	

	if (m_btnOK.IsWindow())
	{
		m_btnOK.DestroyWindow();
	}

	if (m_btnCancel.IsWindow())
	{
		m_btnCancel.DestroyWindow();
	}	
}