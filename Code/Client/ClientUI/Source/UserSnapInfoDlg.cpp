/**
 * @file UserSnapInfoDlg.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 显示用户或者群组查找结果的对话框的实现文件
 * @version 0.1
 * @date 2020-03-02
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "stdafx.h"
#include "UserSnapInfoDlg.h"
#include "UI_USER_INFO.h"
#include "UserMgr.h"

/**
 * @brief Construct a new CUserSnapInfoDlg::CUserSnapInfoDlg object
 * 
 */
CUserSnapInfoDlg::CUserSnapInfoDlg()
{
	m_uAccountID = 0;
	m_uFaceID = 0;
	m_bEnableOKButton = TRUE;
}

/**
 * @brief Destroy the CUserSnapInfoDlg::CUserSnapInfoDlg object
 * 
 */
CUserSnapInfoDlg::~CUserSnapInfoDlg()
{

}

/**
 * @brief 设置用户ID
 * 
 * @param uAccountID 
 */
void CUserSnapInfoDlg::SetAccountID(UINT uAccountID)
{
	m_uAccountID = uAccountID;
}

/**
 * @brief 设置用户头像ID
 * 
 * @param uFaceID 
 */
void CUserSnapInfoDlg::SetUserFaceID(UINT uFaceID)
{
	m_uFaceID = uFaceID;
}

/**
 * @brief 设置用户名
 * 
 * @param pszAccountName 
 */
void CUserSnapInfoDlg::SetAccountName(PCTSTR pszAccountName)
{
	m_strAccountName = pszAccountName;
}

/**
 * @brief 设置用户昵称
 * 
 * @param pszNickName 用户昵称字符串
 */
void CUserSnapInfoDlg::SetNickName(PCTSTR pszNickName)
{
	m_strNickName = pszNickName;
}

/**
 * @brief 
 * 
 * @param pszDesc 
 */
void CUserSnapInfoDlg::SetOperationDescription(PCTSTR pszDesc)
{
	m_strOperationDesc = pszDesc;
}

/**
 * @brief 设置OK按钮字符串
 * 
 * @param pszOKButtonText 
 */
void CUserSnapInfoDlg::SetOKButtonText(PCTSTR pszOKButtonText)
{
	m_strOKButtonText = pszOKButtonText;
}


/**
 * @brief 
 * 
 * @param bEnable 
 */
void CUserSnapInfoDlg::EnableOKButton(BOOL bEnable)
{
	m_bEnableOKButton = bEnable;
}

/**
 * @brief 初始化对话框
 * 
 * @param wndFocus 
 * @param lInitParam 
 * @return BOOL 
 */
BOOL CUserSnapInfoDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
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
BOOL CUserSnapInfoDlg::InitUI()
{
    BOOL bGroup = IsGroupTarget(m_uAccountID);
	
	m_SkinDlg.SetBgPic(_T("DlgBg\\MsgBoxDlgBg.png"), CRect(4, 69, 4, 33));
	m_SkinDlg.SetCloseSysBtnPic(_T("SysBtn\\btn_close_normal.png"), _T("SysBtn\\btn_close_highlight.png"), _T("SysBtn\\btn_close_down.png"));
	m_SkinDlg.SubclassWindow(m_hWnd);
	if(bGroup)
	{
		m_SkinDlg.SetTitleText(_T("群信息"));
	}
	else
	{
		m_SkinDlg.SetTitleText(_T("用户信息"));
	}	
	m_SkinDlg.MoveWindow(0, 0, 340, 300, FALSE);

	HDC hDlgBgDC = m_SkinDlg.GetBgDC();

	{
		m_picUserThumb.SetTransparent(TRUE, hDlgBgDC);
		TCHAR szThumbPath[MAX_PATH] = {0};
		if(bGroup)
		{
			_stprintf_s(szThumbPath, MAX_PATH, _T("%sImage\\DefGroupHeadPic.png"), g_szHomePath);
		}	
		else
		{
			_stprintf_s(szThumbPath, MAX_PATH, _T("%sImage\\UserThumbs\\%u.png"), g_szHomePath, m_uFaceID);
		}
		m_picUserThumb.SetBitmap(szThumbPath);
		m_picUserThumb.SubclassWindow(GetDlgItem(IDC_THUMB));
		m_picUserThumb.MoveWindow(50, 55, 64, 64, FALSE);
	}
	
	{
		m_staUserAccountName.SetTransparent(TRUE, hDlgBgDC);
		m_staUserAccountName.SubclassWindow(GetDlgItem(IDC_ACCOUNT_NAME));
		m_staUserAccountName.SetTextColor(RGB(0, 0, 0));
		m_staUserAccountName.SetWindowText(m_strAccountName);
		m_staUserAccountName.MoveWindow(150, 55, 270, 20, FALSE);
	}

	{
		m_staUserNickName.SetTransparent(TRUE, hDlgBgDC);
		m_staUserNickName.SubclassWindow(GetDlgItem(IDC_NICK_NAME));
		m_staUserNickName.SetTextColor(RGB(0, 0, 0));
		m_staUserNickName.SetWindowText(m_strNickName);
		m_staUserNickName.MoveWindow(150, 80, 270, 20, FALSE);
	}

	{
		m_staOperationDesc.SetTransparent(TRUE, hDlgBgDC);
		m_staOperationDesc.SubclassWindow(GetDlgItem(IDC_DESC));
		m_staOperationDesc.SetTextColor(RGB(0, 0, 0));
		m_staOperationDesc.SetWindowText(m_strOperationDesc);
	}
	//m_staOperationDesc.MoveWindow(150, 80, 270, 20, FALSE);

	{
		m_btnOK.SetButtonType(SKIN_PUSH_BUTTON);
		m_btnOK.SetTransparent(TRUE, hDlgBgDC);
		m_btnOK.SetBgPic(_T("Button\\btn_normal.png"), _T("Button\\btn_focus.png"),_T("Button\\btn_focus.png"), _T("Button\\btn_focus.png"));
		m_btnOK.SetTextColor(RGB(0, 0, 0));
		m_btnOK.SubclassWindow(GetDlgItem(IDOK));
		m_btnOK.SetWindowText(m_strOKButtonText);
		m_btnOK.EnableWindow(m_bEnableOKButton);
	}

	{
		m_btnCancel.SetButtonType(SKIN_PUSH_BUTTON);
		m_btnCancel.SetTransparent(TRUE, hDlgBgDC);
		m_btnCancel.SetBgPic(_T("Button\\btn_normal.png"), _T("Button\\btn_focus.png"),_T("Button\\btn_focus.png"), _T("Button\\btn_focus.png"));
		m_btnCancel.SetTextColor(RGB(0, 0, 0));
		m_btnCancel.SubclassWindow(GetDlgItem(IDCANCEL));
		m_btnCancel.SetWindowText(_T("关闭"));
	}

	return TRUE;
}


/**
 * @brief 响应OK按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CUserSnapInfoDlg::OnBtn_OK(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	EndDialog(IDOK);
}

/**
 * @brief 响应Cancel按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CUserSnapInfoDlg::OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	EndDialog(IDCANCEL);
}


/**
 * @brief 响应Close
 * 
 */
void CUserSnapInfoDlg::OnClose()
{
	EndDialog(IDCANCEL);
}

