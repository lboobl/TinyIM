/**
 * @file RegisterDialog.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 注册账号的对话框
 * @version 0.1
 * @date 2019-08-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _TINY_IM_REGISTER_DIALOG_H_
#define _TINY_IM_REGISTER_DIALOG_H_
#include "resource.h"
#include "SkinLib/SkinLib.h"
#include "UI_USER_INFO.h"
#include "UIWndMsgDef.h"

class CRegisterDialog : public CDialogImpl<CRegisterDialog>
{
public:
	CRegisterDialog();
	virtual ~CRegisterDialog();

	
	//在资源文件里面增加对话框资源模板
	enum { IDD = IDD_REGISTER };

	BEGIN_MSG_MAP_EX(CRegisterDialog)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER_EX(ID_BTN_REGIST, OnBtn_Regist)
		COMMAND_ID_HANDLER_EX(ID_BTN_CANCEL, OnBtn_Cancel)
		MESSAGE_HANDLER(FMG_MSG_REGISTER, OnRegisterResult);
	END_MSG_MAP()

public:
	PCTSTR GetAccountName();
	PCTSTR GetPassword();

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();
	void OnBtn_Regist(UINT uNotifyCode, int nID, CWindow wndCtl);		// “注册”按钮
	void OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl);		// “取消注册”按钮
	LRESULT OnRegisterResult(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	BOOL InitUI();
	void UninitUI();

    static UINT WINAPI RegisterThreadProc(void* pParam);

public:
	HWND m_MainWnd;
private:
	CSkinDialog			m_RegDlg;//注册账号的对话框
	CSkinEdit			m_edtRegId;//用户ID编辑框
	CSkinEdit           m_edtRegName;//用户名编辑框
	CSkinEdit 			m_edtPwd;//用户密码编辑框
	CSkinEdit  			m_edtCheckPwd;//密码确认编辑框
	CSkinButton			m_btnRegist;//注册按钮
	CSkinButton         m_btnCancel;//取消按钮

	CString				m_strMobile;//用户手机号
	CString				m_strAccount;//用户账号
	CString				m_strNickName;//用户昵称
	CString				m_strPassword;//用户密码
	CString				m_strCheckPassword;//确认密码
};
#endif