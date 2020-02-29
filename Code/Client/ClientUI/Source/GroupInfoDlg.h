/**
 * @file GroupInfoDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 显示群组信息的对话框
 * @version 0.1
 * @date 2019-08-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef _TINY_IM_C_GROUP_INFO_DLG_H_ 
#define _TINY_IM_C_GROUP_INFO_DLG_H_

#include "resource.h"
#include "CustomMsgDef.h"
#include "Utils.h"
#include "SkinLib/SkinLib.h"
#include "UI_USER_INFO.h"
//#include "FlamingoClient.h"

class CGroupInfoDlg : public CDialogImpl<CGroupInfoDlg>, public CMessageFilter
{
public:
	CGroupInfoDlg(void);
	~CGroupInfoDlg(void);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	enum { IDD = IDD_GROUP_INFO_DLG };

	BEGIN_MSG_MAP_EX(CGroupInfoDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_HANDLER_EX(ID_BTN_OK, BN_CLICKED, OnBtn_Ok)
		COMMAND_HANDLER_EX(ID_BTN_CANCEL, BN_CLICKED, OnBtn_Cancel)
	END_MSG_MAP()

public:
	void OnUpdateGroupInfo();	// 更新群信息
	void OnUpdateGroupNumber();	// 更新群号码

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();
	void OnBtn_Ok(UINT uNotifyCode, int nID, CWindow wndCtl);		// “确定”按钮
	void OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl);	// “取消”按钮

	C_UI_GroupInfo* GetGroupInfoPtr();
	void UpdateCtrls();

	BOOL Init();		// 初始化
	void UnInit();		// 反初始化

public:
	//CFlamingoClient* m_lpFMGClient;
	HWND m_hMainDlg;
	UINT m_nGroupCode;
	std::string m_strGroupId;

private:
	CSkinDialog m_SkinDlg;

	CSkinStatic m_staName, m_staCreater, m_staClass;
	CSkinStatic m_staNumber, m_staCreateTime, m_staRemark;
	CSkinStatic m_staMemo, m_staFingerMemo;
	CSkinStatic m_staCardName, m_staPhone, m_staGender, m_staEmail;
	CSkinStatic m_staRemark2;

	CSkinEdit m_edtName, m_edtCreater, m_edtClass;
	CSkinEdit m_edtNumber, m_edtCreateTime, m_edtRemark;
	CSkinEdit m_edtMemo, m_edtFingerMemo;
	CSkinEdit m_edtCardName, m_edtPhone, m_edtGender, m_edtEmail;
	CSkinEdit m_edtRemark2;

	CSkinButton m_btnOk, m_btnCancel;

	HICON m_hDlgIcon, m_hDlgSmallIcon;
};

#endif 
