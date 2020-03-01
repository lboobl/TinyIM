/**
 * @file MsgTipDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 消息盒子对话框,在托盘区显示
 * @image https://www.dennisthink.com/wp-content/uploads/2019/10/2019-10-25_101630.jpg
 * @version 0.1
 * @date 2019-08-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _TINY_IM_MSG_TIP_DLG_H_
#define _TINY_IM_MSG_TIP_DLG_H_

#include "resource.h"
#include "CustomMsgDef.h"
#include "SkinLib/SkinLib.h"
#include "ImageEx.h"
#include "UI_USER_INFO.h"

class CMsgTipDlg : public CDialogImpl<CMsgTipDlg>
{
public:
	CMsgTipDlg(void);
	~CMsgTipDlg(void);

	enum { IDD = IDD_MSG_TIP_DLG };

	BEGIN_MSG_MAP_EX(CMsgTipDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		//MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_SIZE(OnSize)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		NOTIFY_HANDLER_EX(1000, LVN_ITEMCHANGED, OnList_ItemChange)
		COMMAND_HANDLER_EX(ID_LINK_CANCEL_FLASH, BN_CLICKED, OnLnk_CancelFlash)
		COMMAND_HANDLER_EX(ID_LINK_SHOW_ALL, BN_CLICKED, OnLnk_ShowAll)
	END_MSG_MAP()

public:
	BOOL StartTrackMouseLeave();
	int  FindMsgSender(E_UI_CHAT_MSG_TYPE nType, const std::string strSenderId);
	void AddMsgSender(E_UI_CHAT_MSG_TYPE nType, const std::string nSenderId);
	void DelMsgSender(E_UI_CHAT_MSG_TYPE nType, const std::string nSenderId);

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	BOOL OnEraseBkgnd(CDCHandle dc);
	void OnTimer(UINT_PTR nIDEvent);
	void OnSize(UINT nType, CSize size);
	void OnClose();
	void OnDestroy();
	LRESULT OnList_Click(LPNMHDR pnmh);
	LRESULT OnList_ItemChange(LPNMHDR pnmh);
	void OnLnk_CancelFlash(UINT uNotifyCode, int nID, CWindow wndCtl);// “取消闪烁”超链接控件
	void OnLnk_ShowAll(UINT uNotifyCode, int nID, CWindow wndCtl);// “显示全部”超链接控件

	BOOL InitCtrls();			// 初始化控件
	BOOL UnInitCtrls();			// 反初始化控件
	void SetDlgAutoSize();		// 自动调整对话框大小
	void SetCtrlsAutoSize();	// 自动调整控件大小
	void _AddMsgSender(int nIndex, C_UI_MessageSender* lpMsgSender);
	WString GetHeadPicFullName(UINT nGroupCode, UINT nUTalkUin);
	void GetNumber(UINT nGroupCode, UINT nUTalkUin, UINT& nGroupNum, UINT& nUTalkNum);

public:
	HWND			m_hMainDlg;
	CRect			m_rcTrayIcon;
	CRect			m_rcTrayIcon2;		//外部传进来的任务栏图标区域

private:
	CSkinDialog m_SkinDlg;
	CSkinListCtrl m_ListCtrl;//消息列表
	CSkinHyperLink m_lnkCancelFlash;//取消闪烁的超链接
	CSkinHyperLink m_lnkShowAll;//显示全部的超链接
	DWORD m_dwTimerId;//定时器ID
	int m_nListItemHeight;
};
#endif