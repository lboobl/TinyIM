
/**
 * @file AddFriendConfirmDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 添加好友确认对话框
 * @version 0.1
 * @date 2019-08-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _TINY_IM_C_ADD_FRIEND_CONFIRM_DLG_H_
#define _TINY_IM_C_ADD_FRIEND_CONFIRM_DLG_H_
#include "resource.h"
#include "SkinLib/SkinLib.h"

class CAddFriendConfirmDlg : public CDialogImpl<CAddFriendConfirmDlg>
{
public:
	CAddFriendConfirmDlg();
	virtual ~CAddFriendConfirmDlg();

	void SetWindowTitle(PCTSTR pszWindowTitle);
	void SetWindowInfo(PCTSTR pszWindowInfo);
	void ShowAgreeButton(BOOL bShow);
	void ShowRefuseButton(BOOL bShow);
	void ShowOKButton(BOOL bShow);

	bool IsAgree() { return m_bAgree; }
	//资源ID
	enum { IDD = IDD_ADD_FRIEND_CONFIRM_DLG };

	BEGIN_MSG_MAP_EX(CFindFriendDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		
		COMMAND_ID_HANDLER_EX(IDOK, OnOK)
		COMMAND_ID_HANDLER_EX(ID_ADD_CONFIRM_AGREE, OnAgreeOrReject)
		COMMAND_ID_HANDLER_EX(ID_ADDCONFIRM_REFUSE, OnAgreeOrReject)
	END_MSG_MAP()

protected:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();

	void OnOK(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnAgreeOrReject(UINT uNotifyCode, int nID, CWindow wndCtl);

	BOOL InitUI();
	void UninitUI();

public:


private:
	CSkinDialog			m_SkinDlg;//对话框
	CSkinButton			m_btnAgree;//同意按钮
	CSkinButton			m_btnRefuse;//拒绝按钮
	CSkinButton			m_btnOK;//OK按钮
	CSkinHyperLink		m_staticAddConfirmInfo;//确认链接

	CString				m_strWindowTitle;//窗口标题字符串
	CString				m_strWindowInfo;//窗口具体的信息
	BOOL				m_bShowAgreeButton;//显示同意按钮
	BOOL				m_bShowRefuseButton;//显示拒绝按钮
	BOOL				m_bShowOKButton;//显示OK按钮
	bool		        m_bAgree;//同意添加
};
#endif