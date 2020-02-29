/**
 * @file FindFriendDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 查找并添加好友对话框
 * @version 0.1
 * @date 2019-08-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef _TINY_IM_C_FIND_FRIEND_DLG_
#define _TINY_IM_C_FIND_FRIEND_DLG_


#include "resource.h"
#include "SkinLib/SkinLib.h"
#include "UI_USER_INFO.h"
#include "Proto.h"
#include "UIWndMsgDef.h"
//查找还有并添加好友对话框
class CFindFriendDlg : public CDialogImpl<CFindFriendDlg>, public CMessageFilter
{
public:
	CFindFriendDlg();
	virtual ~CFindFriendDlg();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//在资源文件里面增加对话框资源模板
	enum { IDD = IDD_FIND_FRIEND_DLG };

	BEGIN_MSG_MAP_EX(CFindFriendDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		COMMAND_ID_HANDLER_EX(IDC_BTN_ADD, OnAddFriend)
		MESSAGE_HANDLER(FMG_MSG_FINDFREIND, OnFindFriendResult)
	END_MSG_MAP()

protected:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnClose();
	void OnDestroy();
	void OnAddFriend(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnFindFriendResult(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HBRUSH OnCtlColorStatic(CDCHandle dc, CStatic wndStatic);

	BOOL InitUI();
	void UninitUI();

	void AddMessageFilter();
	void RemoveMessageFilter();

public:
	std::shared_ptr<CMsgProto> m_netProto;  //网络协议指针
	CUserMgr& m_userMgr;					//用户管理类

private:
	CSkinDialog			m_SkinDlg;					//对话框
	CSkinEdit			m_edtAddId;					//好友ID或群ID编辑框
	CButton				m_btnFindTypeSingle;		//找人RadioBox
	CButton				m_btnFindTypeGroup;			//找群RadioBox
	CSkinButton			m_btnAdd;				    //加为好友按钮
	CSkinHyperLink		m_staticAddInfo;            //TODO
};

#endif 