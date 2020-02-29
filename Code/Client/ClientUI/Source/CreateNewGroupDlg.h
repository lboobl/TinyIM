/**
 * @file CreateNewGroupDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 新建群组对话框
 * @version 0.1
 * @date 2019-08-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _TINY_IM_C_CREATE_NEW_GROUP_DLG_H_
#define _TINY_IM_C_CREATE_NEW_GROUP_DLG_H_
#include "resource.h"
#include "SkinLib/SkinLib.h"
#include "UI_USER_INFO.h"
#include "UIWndMsgDef.h"

//查找还有并添加好友对话框
class CCreateNewGroupDlg : public CDialogImpl<CCreateNewGroupDlg>, public CMessageFilter
{
public:
	CCreateNewGroupDlg();
	virtual ~CCreateNewGroupDlg();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//在资源文件里面增加对话框资源模板
	enum { IDD = IDD_CREATE_NEW_GROUP };

	BEGIN_MSG_MAP_EX(CCreateNewGroupDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER_EX(FMG_MSG_CREATE_NEW_GROUP_RESULT, OnCreateNewGroupResult)
		COMMAND_ID_HANDLER_EX(IDOK, OnOK)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

protected:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();
	void OnOK(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnCreateNewGroupResult(UINT uMsg, WPARAM wParam, LPARAM lParam);

	BOOL InitUI();
	void UninitUI();

	void AddMessageFilter();
	void RemoveMessageFilter();

public:
	//CFlamingoClient*		m_pFMGClient;

private:
	CSkinDialog			m_SkinDlg;//主对话框
	CSkinEdit			m_edtGroupName;//群组名称编辑框
	CSkinButton			m_btnOK;//OK按钮
	CSkinButton			m_btnCancel;//取消按钮
};
#endif