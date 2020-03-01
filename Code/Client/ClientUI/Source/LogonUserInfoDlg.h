/**
 * @file LogonUserInfoDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 显示用户登录以后的信息的对话框
 * @version 0.1
 * @date 2019-08-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef _TINY_IM_C_LOGON_USER_INFO_DLG_H_ 
#define _TINY_IM_C_LOGON_USER_INFO_DLG_H_
#include "resource.h"
#include "SkinLib/SkinLib.h"
#include "UI_USER_INFO.h"
#include "Proto.h"
#include "UserConfig.h"
#include "UserMgr.h"
#include "UIWndMsgDef.h"
class CLogonUserInfoDlg : public CDialogImpl<CLogonUserInfoDlg>, public CMessageFilter
{
public:
	CLogonUserInfoDlg(void);
	~CLogonUserInfoDlg(void);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	enum { IDD = IDD_LOGON_USER_INFO };

	BEGIN_MSG_MAP_EX(CLogonUserInfoDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_HANDLER_EX(IDC_SYS_HEAD, BN_CLICKED, OnSysHead)		//更改用户头像
		COMMAND_HANDLER_EX(IDC_CUSTOM_HEAD, BN_CLICKED, OnCustomHead)	//更改用户头像
		COMMAND_HANDLER_EX(IDOK, BN_CLICKED, OnBtn_OK)
		COMMAND_HANDLER_EX(IDCANCEL, BN_CLICKED, OnBtn_Cancel)
		MESSAGE_HANDLER_EX(FMG_MSG_UPLOAD_USER_THUMB, OnUploadUserThumbResult)
	END_MSG_MAP()

public:
	void UpdateCtrlData();

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnClose();
	void OnDestroy();
	void OnSysHead(UINT uNotifyCode, int nID, CWindow wndCtl);//更改用户头像
	void OnCustomHead(UINT uNotifyCode, int nID, CWindow wndCtl);//更改用户头像
	void OnBtn_OK(UINT uNotifyCode, int nID, CWindow wndCtl);	
	void OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnUploadUserThumbResult(UINT uMsg, WPARAM wParam, LPARAM lParam);	

	BOOL ValidatePhone(const CString& strPhone);


	BOOL Init();		// 初始化
	void UnInit();		// 反初始化

public:
	std::shared_ptr<CMsgProto>		m_netProto;
	UINT				m_uUserIndex;
	UINT				m_uSysFaceID;	//系统头像ID
	TCHAR				m_szCustomFacePath[MAX_PATH];

private:
	CSkinDialog			m_SkinDlg;

	CSkinEdit			m_edtNickName;//用户昵称编辑框
	CSkinButton			m_btnSysHead;//系统头像按钮
	CSkinButton			m_btnCustomHead;//用户自定义头像按钮
	CSkinEdit			m_edtSign;//用户签名编辑框
	CSkinPictureBox		m_picHead;//头像图片显示窗
	CButton				m_btnGenderMale;//男性性别按钮
	CButton				m_btnGenderFemale;//女性性别按钮
	CDateTimePickerCtrl	m_dtpBirthday;//出生日期选择控件
	CSkinEdit			m_edtAddress;//用户地址编辑控件
	CSkinEdit			m_edtPhone;//用户电话编辑控件
	CSkinEdit			m_edtMail;//用户邮箱地址编辑控件

	CSkinButton			m_btnOK;//确定按钮
	CSkinButton			m_btnCancel;//取消按钮

	HICON				m_hDlgIcon;//标准ICON句柄
	HICON				m_hDlgSmallIcon;//小ICON句柄

	BOOL				m_bUseCustomThumb;	//是否使用自定义头像
	TCHAR				m_szCustomFaceRemotePath[MAX_PATH];
	CUserMgr&	m_userMgr;
private:
	void UpdateLogonUserInfo(CString strNickName,
		                     CString strSignature,
		                     UINT uGender, 
		                     UINT uBirthday, 
		                     CString strAddress, 
							 CString strPhone,
		                     CString strMail,
		                     UINT m_uSysFaceId, 
		                     CString strCustomFaceRemotePath, 
		                     BOOL bUseCustomThub);
};
#endif