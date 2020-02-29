/**
 * @file PicBarDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 图片显示对话框
 * @version 0.1
 * @date 2019-08-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#ifndef _TINY_IM_PIC_BAR_DLG_H_
#define _TINY_IM_PIC_BAR_DLG_H_

#include "resource.h"
#include "SkinLib/SkinLib.h"


class CPicBarDlg : public CDialogImpl<CPicBarDlg>
{
public:
	CPicBarDlg(void);
	~CPicBarDlg(void);

	enum { IDD = IDD_PIC_BAR_DLG };

	BEGIN_MSG_MAP_EX(CPicBarDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER_EX(301, OnBtn_Clicked)
		COMMAND_ID_HANDLER_EX(302, OnBtn_Clicked)
		COMMAND_ID_HANDLER_EX(303, OnBtn_Clicked)
		COMMAND_ID_HANDLER_EX(304, OnBtn_Clicked)
		COMMAND_ID_HANDLER_EX(305, OnBtn_Clicked)
	END_MSG_MAP()

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose()
	{
		ShowWindow(SW_HIDE);
		DestroyWindow();
	}
	void OnDestroy();

	void OnBtn_Clicked(UINT uNotifyCode, int nID, CWindow wndCtl);

private:
	CSkinToolBar m_ToolBar; //工具栏按钮
};

#endif