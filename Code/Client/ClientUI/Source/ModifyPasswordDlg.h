/**
 * @file ModifyPasswordDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 修改密码对话框类
 * @version 0.1
 * @date 2019-08-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#ifndef _TINY_IM_MODIFY_PASSWORD_DLG_H_
#define _TINY_IM_MODIFY_PASSWORD_DLG_H_

#include "resource.h"
#include "SkinLib/SkinLib.h"
#include "UI_USER_INFO.h"
#include "Proto.h"
#include "UIWndMsgDef.h"

class CModifyPasswordDlg : public CDialogImpl<CModifyPasswordDlg>, public CMessageFilter
{
public:
	CModifyPasswordDlg();
	virtual ~CModifyPasswordDlg();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	enum { IDD = IDD_MODIFY_PASS_WORD };

	BEGIN_MSG_MAP_EX(CModifyPasswordDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER_EX(IDOK, OnBtn_OK)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnBtn_Cancel)
		MESSAGE_HANDLER(FMG_MSG_MODIFY_PASSWORD_RESULT, OnModifyPasswordResult);
	END_MSG_MAP()


private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnClose();
	void OnDestroy();
	void OnBtn_OK(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnModifyPasswordResult(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

	BOOL InitUI();
	void UninitUI();

	void AddMessageFilter();
	void Reset();
	void RemoveMessageFilter();

public:

	std::shared_ptr<CMsgProto> m_netProto;
private:
	CSkinDialog			m_SkinDlg;
	CSkinEdit			m_edtOldPassword;  //旧密码输入框
	CSkinEdit			m_edtNewPassword;   //新密码输入框
	CSkinEdit			m_edtNewPasswordConfirm;//新密码确认框
	CSkinButton			m_btnOK; //确认按钮
	CSkinButton			m_btnCancel; //取消按钮
};

#endif