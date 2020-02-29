/**
 * @file SystemSettingDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 系统设置对话框头文件
 * @version 0.1
 * @date 2019-08-04
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#ifndef _TINY_IM_C_SYSTEM_SETTING_DLG_H_
#define _TINY_IM_C_SYSTEM_SETTING_DLG_H_

#include "resource.h"
#include "SkinLib/SkinLib.h"
#include "Proto.h"


class CSystemSettingDlg : public CDialogImpl<CSystemSettingDlg>
{
public:
	CSystemSettingDlg(void);
	virtual ~CSystemSettingDlg(void);

	enum { IDD = IDD_SYSTEM_SETTINGS };

	BEGIN_MSG_MAP_EX(CSystemSettingDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		
		COMMAND_ID_HANDLER_EX(IDC_AUTO_REPLY, OnCheckAutoReply)
		COMMAND_ID_HANDLER_EX(IDC_DESTROY_AFTER_READ, OnCheckDestroyAfterRead)
		COMMAND_ID_HANDLER_EX(IDOK, OnBtnOK)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnBtnCancel)
	END_MSG_MAP()

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();
	
	void OnCheckAutoReply(UINT uNotifyCode, int nID, CWindow wndCtl);			
	void OnCheckDestroyAfterRead(UINT uNotifyCode, int nID, CWindow wndCtl);
	
	void OnBtnOK(UINT uNotifyCode, int nID, CWindow wndCtl);					
	void OnBtnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);						
	
	BOOL InitUI();
	void UninitUI();
public:
	std::shared_ptr<CMsgProto> m_netProto;


private:
	CSkinDialog			m_SkinDlg;                  //主界面的对话框
		
	CSkinButton			m_btnMute;					//关闭所有声音
	CSkinButton			m_btnAutoReply;				//自动回复
	CSkinEdit			m_edtAutoReply;				//自动回复内容
	CSkinButton			m_btnDestroyAfterRead;		//阅后即焚
	CSkinComboBox		m_cboDurationRead;			//阅后即焚持续时间
	CSkinButton			m_btnRevokeChatMsg;			//消息撤回功能
	CSkinButton			m_btnExitPrompt;			//关闭主对话框时弹出提示对话框
	CSkinButton			m_btnExitWhenClose;			//关闭主对话框时直接退出
	CSkinButton			m_btnShowLastMsg;			//聊天窗口显示最近聊天记录

	CSkinButton			m_btnOK;					//确定
	CSkinButton			m_btnCancel;				//取消
	
	HFONT				m_hFont;                    //字体句柄

	HICON 				m_hDlgIcon;    				//对话框图标
	HICON				m_hDlgSmallIcon;			//对话框小图标
};

#endif 