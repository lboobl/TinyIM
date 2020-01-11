/**
 * @file AboutDlg.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 关于对话框实现类
 * @version 0.1
 * @date 2019-08-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "stdafx.h"
#include "aboutdlg.h"
#include "UI_USER_INFO.h"
#include "MiniBuffer.h"
#include "EncodingUtil.h"
#include "Utils.h"
#include "FileTool.h"
#include "GDIFactory.h"
#include "UIText.h"
#include "IniFile.h"

/**
 * @brief 初始化对话框
 * 
 * @return LRESULT 
 */
LRESULT CAboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_SkinDlg.SetBgPic(_T("LoginPanel_window_windowBkg.png"), CRect(2, 24, 2, 0));
	m_SkinDlg.SetCloseSysBtnPic(_T("SysBtn\\btn_close_normal.png"), _T("SysBtn\\btn_close_highlight.png"), _T("SysBtn\\btn_close_down.png"));
	m_SkinDlg.SubclassWindow(m_hWnd);
	m_SkinDlg.MoveWindow(0, 0, 430, 450,FALSE);

    CIniFile iniFile;
    CString strIniFilePath(g_szHomePath);
    strIniFilePath += _T("config\\flamingo.ini");
    TCHAR szAboutDlgTitle[64] = { 0 };
    iniFile.ReadString(_T("ui"), _T("aboutdlgtitle"), UI_ABOUTWND_TITLE, szAboutDlgTitle, ARRAYSIZE(szAboutDlgTitle), strIniFilePath);
    m_SkinDlg.SetTitleText(szAboutDlgTitle);

	HDC hDlgBgDc = m_SkinDlg.GetBgDC();
	m_staAboutInfo.SubclassWindow(GetDlgItem(IDC_ABOUT_Flamingo));
	m_staAboutInfo.SetTransparent(TRUE, hDlgBgDc);

	m_hyperLinkVersion.SubclassWindow(GetDlgItem(IDC_STATIC_VERSION));
	m_hyperLinkVersion.SetTransparent(TRUE, hDlgBgDc);

	HFONT hFont = CGDIFactory::GetFont(20);

	m_hyperLinkVersion.SetNormalFont(hFont);
	m_hyperLinkVersion.SetLinkColor(RGB(22, 112, 235));
	m_hyperLinkVersion.SetHoverLinkColor(RGB(22, 112, 235));
	m_hyperLinkVersion.SetVisitedLinkColor(RGB(22, 112, 235));
    
    //加载公司网址链接
    TCHAR szCompanyURLLink[256] = { 0 };
    iniFile.ReadString(_T("ui"), _T("companyurllink"), UI_COMPANY_LINK, szCompanyURLLink, ARRAYSIZE(szCompanyURLLink), strIniFilePath);
    m_hyperLinkVersion.SetHyperLink(szCompanyURLLink);

	CenterWindow(::GetDesktopWindow());

    TCHAR szCopyrightText[256] = { 0 };
    iniFile.ReadString(_T("ui"), _T("copyrighttext"), UI_COPYRIGHT_TEXT, szCopyrightText, ARRAYSIZE(szCopyrightText), strIniFilePath);
    m_hyperLinkVersion.SetWindowText(szCopyrightText);
			
    TCHAR szCompanyname[256] = { 0 };
    iniFile.ReadString(_T("ui"), _T("defaultcompanyname"), UI_DEFAULT_COMPANYNAME, szCompanyname, ARRAYSIZE(szCompanyname), strIniFilePath);
    CString strText = szCompanyname;

    CString strAboutInfoPath;
    strAboutInfoPath.Format(_T("%sconfig\\AboutInfo.txt"), g_szHomePath);
    CTinyImFile file;
    if (file.Open(strAboutInfoPath, FALSE))
    {
        const char* pBuffer = file.Read();
        if (pBuffer != NULL)
        {
            EncodeUtil::AnsiToUnicode(pBuffer, strText.GetBuffer(file.GetSize() * 2), file.GetSize() * 2);
            strText.ReleaseBuffer();
        }
    }   

	if(strText.IsEmpty())
	{
        return FALSE;
    }

	m_staAboutInfo.SetWindowText(strText);

	return TRUE;
}

/**
 * @brief 关闭对话框
 * 
 * @param wID 
 * @return LRESULT 
 */
LRESULT CAboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);

	return 0;
}
