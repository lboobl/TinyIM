
/**
 * @file TeamDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 分组对话框头文件
 * @version 0.1
 * @date 2019-08-04
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _TINY_IM_TEAM_DLG_H_
#define _TINY_IM_TEAM_DLG_H_

#include "resource.h"
#include "SkinLib/SkinLib.h"
#include "Proto.h"

enum TEAM_OPERATION_TYPE
{
	TEAM_OPERATION_ADD_TEAM,//增加分组
	TEAM_OPERATION_DELETE_TEAM,//删除分组
	TEAM_OPERATION_MODIFY_TEAM_NAME,//修改分组名称
};


//新建分组对话框
class CTeamDlg : public CDialogImpl<CTeamDlg>
{
public:
	CTeamDlg();
	virtual ~CTeamDlg();

	//在资源文件里面增加对话框资源模板
	enum { IDD = IDD_TEAM };

	BEGIN_MSG_MAP_EX(CTeamDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER_EX(IDOK, OnOK)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

public:
	void SetType(TEAM_OPERATION_TYPE nType);
private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();
	void OnOK(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);

	BOOL InitUI();
	void UninitUI();

public:
	std::shared_ptr<CMsgProto> m_netProto;
	long				m_nTeamIndex; //分组索引

private:
	CSkinDialog			m_SkinDlg;
	CSkinStatic			m_staTeamName;  //群组名静态控件
	CSkinEdit			m_edtTeamName;  //群组名编辑控件
	CSkinButton			m_btnOK;        //OK按钮
	CSkinButton			m_btnCancel;    //取消按钮

	long				m_nOperationType; //操作类型
};
#endif
