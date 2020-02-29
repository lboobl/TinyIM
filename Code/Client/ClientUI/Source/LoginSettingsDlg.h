/**
 * @file LoginSettingsDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 登录设置对话框，设置服务器的IP地址等
 * @version 0.1
 * @date 2019-08-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _TINY_IM_C_LOGIN_SETTINGS_DLG_H_ 
#define _TINY_IM_C_LOGIN_SETTINGS_DLG_H_
#include "resource.h"
#include "SkinLib/SkinLib.h"

#define MAX_SRV_ADDR	64
#define MAX_SRV_PORT	8


class CLoginSettingsDlg : public CDialogImpl<CLoginSettingsDlg>, public CMessageFilter
{
public:
	CLoginSettingsDlg();
	virtual ~CLoginSettingsDlg();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//在资源文件里面增加对话框资源模板
	enum { IDD = IDD_LOG_SET_DLG };

	BEGIN_MSG_MAP_EX(CLoginSettingsDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER_EX(IDC_COMBO_PROTYPE, OnComboBox_Select)
		COMMAND_ID_HANDLER_EX(ID_LOGIN_SETTING, OnBtn_OK)
		COMMAND_ID_HANDLER_EX(ID_LOGIN_SETTING_CANCEL, OnBtn_Cancel)
		//REFLECT_NOTIFICATIONS()      // 消息反射通知宏
	END_MSG_MAP()

protected:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();
	void OnComboBox_Select(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtn_OK(UINT uNotifyCode, int nID, CWindow wndCtl);		
	void OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl);

	BOOL InitUI();
	void UninitUI();

public:

private:
	CSkinDialog		m_SkinDlg;
	//CSkinStatic		m_staticSrvAddr;
	//CSkinStatic		m_staticFileSrvAddr;
	//CSkinStatic		m_staticSrvPort;
	//CSkinStatic		m_staticFilePort;

	CSkinEdit		m_editSrvAddr;//聊天服务器IP
	CSkinEdit		m_editSrvPort;//聊天服务器端口

	CSkinEdit		m_editFileSrvAddr;//文件服务器IP
	CSkinEdit		m_editFilePort;//文件服务器端口


    CSkinEdit		m_editImgSrvAddr;//图片服务器IP
    CSkinEdit		m_editImgPort;//图片服务器端口

	CSkinComboBox	m_comboProxyType;//代理类型
	CSkinEdit		m_editProxyAddr;//代理服务器IP
	CSkinEdit		m_editProxyPort;//代理服务器端口

	CSkinButton		m_btnOK; //确定按钮
	CSkinButton		m_btnCancel;//取消按钮
	
	TCHAR			m_szSrvAddr[MAX_SRV_ADDR];	
	TCHAR			m_szFileSrvAddr[MAX_SRV_ADDR];
    TCHAR			m_szImgSrvAddr[MAX_SRV_ADDR];
	TCHAR			m_szSrvPort[MAX_SRV_PORT];	
	TCHAR			m_szFilePort[MAX_SRV_PORT];
    TCHAR			m_szImgPort[MAX_SRV_PORT];
	TCHAR			m_szProxyAddr[MAX_SRV_ADDR];
	TCHAR			m_szProxyPort[MAX_SRV_PORT];
};

#endif