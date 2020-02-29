
/**
 * @file LoginDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 用户登录对话框
 * @version 0.1
 * @date 2019-08-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _TINY_IM_LOGIN_DLG_H_ 
#define _TINY_IM_LOGIN_DLG_H_

#include "SkinLib/SkinLib.h"
#include "RegisterDialog.h"
#include "LoginSettingsDlg.h"
#include "LoginAccountList.h"
#include "Updater.h"
#include "IUProtocolData.h"
#include <string>
struct LOGIN_ACCOUNT_INFO;

class CLoginDlg : public CDialogImpl<CLoginDlg>
{
public:
    CLoginDlg();
	virtual ~CLoginDlg(void);

	enum { IDD = IDD_LOGIN_DLG };

    BEGIN_MSG_MAP_EX(CLoginDlg)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_SYSCOMMAND(OnSysCommand)
        MSG_WM_MEASUREITEM(OnMeasureItem)
        MSG_WM_DRAWITEM(OnDrawItem)
        MSG_WM_CLOSE(OnClose)
        MSG_WM_DESTROY(OnDestroy)
        COMMAND_HANDLER_EX(ID_COMBO_UID, CBN_EDITCHANGE, OnCbo_EditChange_UID)
        COMMAND_HANDLER_EX(ID_COMBO_UID, CBN_SELCHANGE, OnCbo_SelChange_UID)
        COMMAND_ID_HANDLER_EX(ID_CHECK_REMEMBER_PWD, OnBtn_RememberPwd)
        COMMAND_ID_HANDLER_EX(ID_CHECK_AUTO_LOGIN, OnBtn_AutoLogin)
        COMMAND_ID_HANDLER_EX(ID_BTN_LOGIN, OnBtn_Login)
        COMMAND_ID_HANDLER_EX(ID_BTN_SET, OnBtn_Set)
        COMMAND_ID_HANDLER_EX(ID_STATIC_REG_ACCOUNT, OnRegister)
        COMMAND_RANGE_HANDLER_EX(ID_MENU_IM_ONLINE, ID_MENU_INVISIBLE, OnMenu_LoginStatus)
		REFLECT_NOTIFICATIONS()														// 消息反射通知宏
	END_MSG_MAP()

public:
	BOOL GetLoginAccountInfo(LOGIN_ACCOUNT_INFO* lpAccount);
	void SetDefaultAccount(PCTSTR pszDefaultAccount);
	void SetDefaultPassword(PCTSTR pszDefaultPassword);

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnSysCommand(UINT nID, CPoint pt);
	void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	void OnClose();
	void OnDestroy();
	void OnCbo_EditChange_UID(UINT uNotifyCode, int nID, CWindow wndCtl);			// “UTalk帐号”组合框
	void OnCbo_SelChange_UID(UINT uNotifyCode, int nID, CWindow wndCtl);			// “UTalk帐号”组合框
	void OnBtn_RememberPwd(UINT uNotifyCode, int nID, CWindow wndCtl);				// “记住密码”复选框
	void OnBtn_AutoLogin(UINT uNotifyCode, int nID, CWindow wndCtl);				// “自动登录”复选框
	void OnBtn_Login(UINT uNotifyCode, int nID, CWindow wndCtl);					// “登录”按钮
	void OnBtn_Set(UINT uNotifyCode, int nID, CWindow wndCtl);						// “设置”按钮
	void OnMenu_LoginStatus(UINT uNotifyCode, int nID, CWindow wndCtl);				// “登录状态”菜单按钮

	void DetectNewVersion();														//检测新版本
	void OnRegister(UINT uNotifyCode, int nID, CWindow wndCtl);

	BOOL InitUI();
	void UninitUI();

	E_UI_ONLINE_STATUS GetStatusFromMenuID(int nMenuID);									// 从菜单ID获取对应的UTalk_STATUS
	void StatusMenuBtn_SetIconPic(CSkinButton& btnStatus, E_UI_ONLINE_STATUS nStatus);	// 根据指定状态设置状态菜单按钮的图标
	void SetCurUser(LPCTSTR lpszUser, BOOL bPwdInvalid = FALSE);

    static UINT WINAPI LoginThreadProc(void* pParam);

public:
	CLoginAccountList*  m_pLoginAccountList;//用户账号列表
	HWND				m_hMainDlg;//主对话框句柄
private:
	CSkinDialog			m_SkinDlg;//对话框界面
	CSkinComboBox		m_cboUid;//用户ID组合框
	CSkinEdit			m_edtPwd;//用户密码输入框
	CSkinButton			m_btnRememberPwd;//记住密码选项
	CSkinButton			m_btnAutoLogin;//自动登录选项
	CSkinHyperLink		m_lnkRegAccount;											//注册链接按钮
	CSkinHyperLink		m_lnkLostPwd;//忘记密码超链接 

	CSkinButton			m_btnSet;//设置按钮
	CSkinButton			m_btnLogin;//登录按钮
	CSkinButton			m_btnLoginStatus;//登录状态按钮
	CSkinPictureBox		m_picHead;//头像控件
	CSkinMenu			m_SkinMenu;

	HICON m_hDlgIcon;
	HICON m_hDlgSmallIcon;
	LOGIN_ACCOUNT_INFO	m_stAccountInfo;
	BOOL				m_bFirstModify;

	CLoginSettingsDlg	m_LoginSettingDlg;											// 设置对话框
	CRegisterDialog		m_RegDlg;													// 注册对话框
	CString				m_strDefaultAccount;
	CString				m_strDefaultPassword;

private:
	//Core Function
	C_WND_MSG_LoginResult DoLogin_Core(const std::string strUserName,const std::string strPassword);
};
#endif