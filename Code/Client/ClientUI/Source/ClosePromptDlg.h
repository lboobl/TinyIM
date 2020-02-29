/**
 * @file ClosePromptDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 退出提示对话框
 * @version 0.1
 * @date 2019-08-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef _TINY_IM_C_CLOSE_PROMPT_DLG_H_ 
#define _TINY_IM_C_CLOSE_PROMPT_DLG_H_
#include "resource.h"
#include "SkinLib/SkinLib.h"

//退出提示对话框
class CClosePromptDlg : public CDialogImpl<CClosePromptDlg>
{
public:
	CClosePromptDlg();
	virtual ~CClosePromptDlg();

	//在资源文件里面增加对话框资源模板
	enum { IDD = IDD_CLOSE_PROMPT };

	BEGIN_MSG_MAP_EX(CClosePromptDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		
		COMMAND_ID_HANDLER_EX(IDC_MINIMIZE, OnMinimize)
		COMMAND_ID_HANDLER_EX(IDC_EXIT, OnExit)
	END_MSG_MAP()

protected:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();

	void OnMinimize(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnExit(UINT uNotifyCode, int nID, CWindow wndCtl);

	BOOL InitUI();
	void UninitUI();

public:

private:
	CSkinDialog			m_SkinDlg;
	
	CSkinPictureBox		m_picInfoIcon;//信息图标
	CSkinStatic			m_staInfoText;//信息静态提示控件
	CSkinButton			m_btnRememberChoice;//记住选择按钮
	CSkinButton			m_btnMinimize;//最小化按钮
	CSkinButton			m_btnExit;//退出按钮
};

#endif //