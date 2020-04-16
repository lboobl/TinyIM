/**
 * @file AboutDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 关于对话框
 * @version 0.1
 * @date 2019-08-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#ifndef _TINY_IM_C_ABOUT_DLG_H_ 
#define _TINY_IM_C_ABOUT_DLG_H_
#include "resource.h"
#include "SkinLib/SkinLib.h"
#include "TinyIMConfig.h"

class CAboutDlg : public CDialogImpl<CAboutDlg>
{
public:
	enum { IDD = IDD_ABOUT_BOX };

	BEGIN_MSG_MAP(CAboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()
	void SetAboutConfig(const CAboutDlgMsgConfig& config);

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
	CSkinDialog		m_SkinDlg;//对话框类
	CSkinStatic		m_staAboutInfo;//关于信息
	CSkinHyperLink	m_hyperLinkVersion;//公司或者版本的链接
	CAboutDlgMsgConfig m_config;//关于对话框的配置信息
};
#endif