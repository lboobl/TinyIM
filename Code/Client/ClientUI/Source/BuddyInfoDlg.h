/**
 * @file BuddyInfoDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 好友信息对话框
 * @version 0.1
 * @date 2019-08-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#ifndef _TINY_IM_C_BUDDY_INFO_DLG_H_
#define _TINY_IM_C_BUDDY_INFO_DLG_H_
#include "resource.h"
#include "SkinLib/SkinLib.h"


class CBuddyInfoDlg : public CDialogImpl<CBuddyInfoDlg>, public CMessageFilter
{
public:
	CBuddyInfoDlg(void);
	~CBuddyInfoDlg(void);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	enum { IDD = IDD_BUDDY_INFO_DLG };

	BEGIN_MSG_MAP_EX(CBuddyInfoDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_HANDLER_EX(IDOK, BN_CLICKED, OnBtnOK)
	END_MSG_MAP()

public:
	void SetWindowTitle(PCTSTR pszWindowTitle);
	void UpdateCtrls();

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();
	void OnBtnOK(UINT uNotifyCode, int nID, CWindow wndCtl);	// “更新”按钮

	BOOL Init();		// 初始化
	void UnInit();		// 反初始化

public:

private:
	CSkinDialog			m_SkinDlg;//对话框
	HICON				m_hDlgIcon;//对话框ICON句柄
	HICON				m_hDlgSmallIcon;//对话框小ICON的Handle

	CSkinButton			m_btnOK;//OK按钮

	CString				m_strWindowTitle;//窗口标题
};
#endif