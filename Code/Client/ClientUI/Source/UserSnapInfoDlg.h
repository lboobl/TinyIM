/**
 * @file UserSnapInfoDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 显示用户或者群组查找结果的对话框
 * @version 0.1
 * @date 2020-03-02
 * 
 * @copyright Copyright (c) 2020
 * 
 */


#ifndef _TINY_IM_USER_SNAP_INFO_DLG_H_
#define _TINY_IM_USER_SNAP_INFO_DLG_H_

#include "resource.h"
#include "SkinLib/SkinLib.h"

//查找联系人或者群时弹出的用户快照信息对话框
class CUserSnapInfoDlg : public CDialogImpl<CUserSnapInfoDlg>
{
public:
	CUserSnapInfoDlg();
	~CUserSnapInfoDlg();
	//在资源文件里面增加对话框资源模板
	enum { IDD = IDD_USER_SNAP_INFO };

	BEGIN_MSG_MAP_EX(CUserSnapInfoDlg)
		MSG_WM_INITDIALOG(OnInitDialog)

		MSG_WM_CLOSE(OnClose)

		COMMAND_ID_HANDLER_EX(IDOK, OnBtn_OK)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnBtn_Cancel)
	END_MSG_MAP()

public:
	void SetAccountID(UINT uAccountID);
	void SetUserFaceID(UINT uFaceID);
	void SetAccountName(PCTSTR pszAccountName);
	void SetNickName(PCTSTR pszNickName);
	void SetOperationDescription(PCTSTR pszDesc);
	void SetOKButtonText(PCTSTR pszOKButtonText);
	void EnableOKButton(BOOL bEnable);

protected:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	
	void OnClose();
	void OnBtn_OK(UINT uNotifyCode, int nID, CWindow wndCtl);		
	void OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl);

	BOOL InitUI();

private:
	CSkinDialog			m_SkinDlg;
	CSkinPictureBox		m_picUserThumb; //用户或群组头像控件
	CSkinStatic			m_staUserAccountName; //用户账号名控件
	CSkinStatic			m_staUserNickName;//用户昵称控件
	CSkinStatic			m_staOperationDesc;		//操作描述控件

	CSkinButton			m_btnOK;  //确定按钮
	CSkinButton			m_btnCancel;//取消按钮
	
	UINT				m_uAccountID;//账号ID(TODO:待删除)
	UINT				m_uFaceID;//头像ID
	CString				m_strAccountName;//用户账号
	CString				m_strNickName;//用户昵称
	CString				m_strOperationDesc;//操作描述字符串(同意/拒绝)

	CString				m_strOKButtonText;//OK按钮字符串

	BOOL				m_bEnableOKButton; //是否允许OK按钮,业务逻辑是什么
	
};
#endif