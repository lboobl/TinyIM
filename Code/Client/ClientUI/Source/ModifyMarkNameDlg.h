/**
 * @file ModifyMarkNameDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 修改备注对话框
 * @version 0.1
 * @date 2019-08-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#ifndef _TINY_IM_MODIFY_MARK_NAME_DLG_H_ 
#define _TINY_IM_MODIFY_MARK_NAME_DLG_H_

#include "resource.h"
#include "SkinLib/SkinLib.h"


class CFlamingoClient;

//修改备注对话框
class CModifyMarkNameDlg : public CDialogImpl<CModifyMarkNameDlg>
{
public:
	CModifyMarkNameDlg();
	virtual ~CModifyMarkNameDlg();

	//在资源文件里面增加对话框资源模板
	enum { IDD = IDD_MODIFY_MARK_NAME };

	BEGIN_MSG_MAP_EX(CModifyMarkNameDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER_EX(IDOK, OnOK)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

protected:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();
	void OnOK(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);

	BOOL InitUI();
	void UninitUI();

public:
	CFlamingoClient*		m_pFMGClient;
	UINT				m_uUserIndex; //用户唯一编号

private:
	CSkinDialog			m_SkinDlg; //对话框
	CSkinStatic			m_staMarkName; //显示备注名的控件
	CSkinEdit			m_edtMarkName; //编辑备注名的控件
	CSkinButton			m_btnOK; //OK按钮
	CSkinButton			m_btnCancel; //取消按钮
};

#endif