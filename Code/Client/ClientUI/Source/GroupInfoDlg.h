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

	HWND m_hMainDlg;
	UINT m_nGroupCode;//TODO: 待删除
	std::string m_strGroupId;

private:
	CSkinDialog m_SkinDlg;

	CSkinStatic m_staName;
	CSkinStatic m_staCreater;
	CSkinStatic m_staClass;

	CSkinStatic m_staNumber; 
	CSkinStatic m_staCreateTime; 
	CSkinStatic m_staRemark;
	CSkinStatic m_staMemo;
	CSkinStatic m_staFingerMemo;
	CSkinStatic m_staCardName;
	CSkinStatic m_staPhone;
	CSkinStatic m_staGender;
	CSkinStatic m_staEmail;
	CSkinStatic m_staRemark2;

	CSkinEdit m_edtName;
	CSkinEdit m_edtCreater;
	CSkinEdit m_edtClass;
	CSkinEdit m_edtNumber;
	CSkinEdit m_edtCreateTime;
	CSkinEdit m_edtRemark;
	CSkinEdit m_edtMemo;
	CSkinEdit m_edtFingerMemo;
	CSkinEdit m_edtCardName;
	CSkinEdit m_edtPhone;
	CSkinEdit m_edtGender;
	CSkinEdit m_edtEmail;
	CSkinEdit m_edtRemark2;

	CSkinButton m_btnOk;
	CSkinButton m_btnCancel;

	HICON m_hDlgIcon;
	HICON m_hDlgSmallIcon;
};

#endif 
