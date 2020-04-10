/**
 * @file MainDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 
 * @version 0.1
 * @date 2019-08-05
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _TINY_IM_MAIN_DLG_H_
#define _TINY_IM_MAIN_DLG_H_

#include "CustomMsgDef.h"
#include "SkinLib/SkinLib.h"
#include "TrayIcon.h"
#include "FaceList.h"
#include "LoginAccountList.h"
#include "CascadeWinManager.h"
#include "RecentListCtrl.h"
#include "BuddyListCtrl.h"
#include "LoginDlg.h"
#include "MsgTipDlg.h"
#include "LogonUserInfoDlg.h"
#include "ModifyPasswordDlg.h"
#include "IUProtocolData.h"
#include "IULog.h"
#include "Proto.h"
#include <vector>
#include "UIWndMsgDef.h"
//分组菜单ID基数
#define TEAM_MENU_ITEM_BASE       0xF001

const int MAX_LOGIN_TRY_TIMES = 3;

//主面板状态
enum MAINPANEL_STATUS
{
    MAIN_PANEL_STATUS_NOT_LOGIN,     //主面板未登录
    MAINPANEL_STATUS_LOGINING,     //主面板正在登录
    MAINPANEL_STATUS_LOGIN,        //主面板已经登录
    MAINPANEL_STATUS_RECONNECTING, //主面板正在重连
    MAINPANEL_STATUS_USERGOOFFLINE //主面板用户主动下线
};

class CGMemberInfoMapKey
{
public:
	CGMemberInfoMapKey(void):m_nGroupCode(0), m_nUTalkUin(0){}
	~CGMemberInfoMapKey(void){}

public:
	bool operator < (const CGMemberInfoMapKey& rhs) const
	{
		if(m_nGroupCode < rhs.m_nGroupCode) return true;
		if(m_nGroupCode > rhs.m_nGroupCode) return false;
		return  m_nUTalkUin < rhs.m_nUTalkUin;
	}

public:
	UINT m_nGroupCode;
	UINT m_nUTalkUin;
};

class CBuddyChatDlg;
class CBuddyInfoDlg;
class CGroupChatDlg;
class CGroupInfoDlg;

class CFindFriendDlg;
class CRemoteDesktopDlg;

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler
{
public:
	CMainDlg(void);
	~CMainDlg(void);

	enum { IDD = IDD_MAIN_DLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SYSCOMMAND(OnSysCommand)
		MSG_WM_MEASUREITEM(OnMeasureItem)
		MSG_WM_DRAWITEM(OnDrawItem)
		MSG_WM_GETMINMAXINFO(OnGetMinMaxInfo)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_SIZE(OnSize)
		MSG_WM_HOTKEY(OnHotKey)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER_EX(IDM_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER_EX(ID_BTN_SIGN, OnBtn_Sign)
		COMMAND_ID_HANDLER_EX(IDC_UNLOCK, OnBtn_UnlockPanel)				    //解锁主窗口
		COMMAND_ID_HANDLER_EX(ID_MENU_SHOW_MAIN_PANEL, OnMenu_ShowMainPanel)
		COMMAND_ID_HANDLER_EX(ID_MENU_LOCK, OnMenu_LockPanel)					//锁定主窗口
		COMMAND_ID_HANDLER_EX(ID_MENU_EXIT, OnMenu_Exit)
		COMMAND_ID_HANDLER_EX(ID_MENU_MUTE, OnMenu_Mute)						//关闭所有声音
		COMMAND_ID_HANDLER_EX(ID_BTN_FIND, OnBtn_ShowAddFriendDlg)
		COMMAND_ID_HANDLER_EX(IDM_SYSTEM_SETTINGS, OnBtn_ShowSystemSettingDlg)	//打开系统设置对话框
		COMMAND_ID_HANDLER_EX(IDM_MODIFY_PASS_WORD, OnBtn_ModifyPassword)			//修改密码
        COMMAND_ID_HANDLER_EX(IDC_BTN_MAIL, OnBtn_OpenMail)      
		COMMAND_ID_HANDLER_EX(ID_BTN_MAIN_MENU, OnBtn_MainMenu)					//主菜单按钮
		COMMAND_HANDLER_EX(ID_BTN_CANCEL, BN_CLICKED, OnBtn_Cancel)				//取消登录
		NOTIFY_HANDLER_EX(ID_TAB_CTRL_MAIN, TCN_DROPDOWN, OnTabCtrlDropDown)
		NOTIFY_HANDLER_EX(ID_TAB_CTRL_MAIN, TCN_SELCHANGE, OnTabCtrlSelChange)
		NOTIFY_HANDLER_EX(ID_LIST_CTRL_BUDDY, NM_DBLCLK, OnBuddyListDblClk)		 //双击好友列表中的好友
		NOTIFY_HANDLER_EX(ID_LIST_CTRL_BUDDY, NM_RCLICK, OnBuddyListRButtonUp)	 //右键好友列表中的好友
		NOTIFY_HANDLER_EX(ID_LIST_CTRL_GROUP, NM_DBLCLK, OnGroupListDblClk)      //
		NOTIFY_HANDLER_EX(ID_LIST_CTRL_GROUP, NM_RCLICK, OnGroupListRButtonUp)
		NOTIFY_HANDLER_EX(ID_LIST_CTRL_RECENT, NM_DBLCLK, OnRecentListDblClk)
		NOTIFY_HANDLER_EX(ID_LIST_CTRL_RECENT, NM_RCLICK, OnRecentListRButtonUp)	 //右键好友列表中的好友
		COMMAND_HANDLER_EX(ID_PIC_HEAD, BN_CLICKED, OnPic_Clicked_Head)			 //点击我的头像
		
		COMMAND_ID_HANDLER_EX(IDM_ADD_TEAM, OnBuddyListAddTeam);					//好友列表添加分组
		COMMAND_ID_HANDLER_EX(IDM_DELETE_TEAM, OnBuddyListDeleteTeam)			//好友列表删除分组
		COMMAND_ID_HANDLER_EX(IDM_MODIFY_TEAM_NAME, OnBuddyListModifyTeamName)	//好友分组修改组名
		COMMAND_RANGE_HANDLER_EX(TEAM_MENU_ITEM_BASE, 0xF015, OnMoveBuddyToTeam)//移动联系人至指定分组

	
		COMMAND_ID_HANDLER_EX(ID_BTN_SIGN, OnBtn_Sign)								//个性签名按钮
		COMMAND_HANDLER_EX(ID_EDIT_SIGN, EN_KILLFOCUS, OnEdit_Sign_KillFocus)		//个性签名编辑框
		COMMAND_RANGE_HANDLER_EX(ID_MENU_IM_ONLINE, ID_MENU_IM_OFFLINE, OnMenu_Status)
		COMMAND_ID_HANDLER_EX(ID_ABOUT, OnMainMenu_About)

		COMMAND_ID_HANDLER_EX(IDM_REFRESH_BUDDY_LIST, OnRefreshBuddyList)		//刷新联系人列表
		COMMAND_ID_HANDLER_EX(IDM_SHOW_ONLINE_BUDDY_ONLY, OnShowOnlineBuddyOnly) //仅显示在线好友

		COMMAND_ID_HANDLER_EX(IDM_SEND_MSG, OnMenu_SendBuddyMessage)				//发送即时消息
		COMMAND_ID_HANDLER_EX(IDM_SEND_MSG_FROM_RECENT_LIST, OnMenu_SendBuddyMessageFromRecentList)
		COMMAND_ID_HANDLER_EX(IDM_VIEW_BUDDY_INFO, OnMenu_ViewBuddyInfo)			//查看好友资料
		COMMAND_ID_HANDLER_EX(IDM_VIEW_BUDDY_INFO_FROM_RECENT_LIST, OnMenu_ViewBuddyInfoFromRecentList)		
        COMMAND_ID_HANDLER_EX(IDM_MODIFY_BUDDY_MARK_NAME, OnMenu_ModifyBuddyMarkName)//修改好友备注
        COMMAND_ID_HANDLER_EX(IDM_DELETE_FRIEND, OnMenu_DeleteFriend)            //删除好友
		COMMAND_ID_HANDLER_EX(IDM_CLEAR_RECENT_LIST, OnClearAllRecentList)
		COMMAND_ID_HANDLER_EX(IDM_DELETE_RECENT_ITEM, OnDeleteRecentItem);

		COMMAND_ID_HANDLER_EX(IDM_SEND_GROUP_MSG, OnMenu_SendGroupMessage)		//右键菜单发送群消息
		COMMAND_ID_HANDLER_EX(IDM_VIEW_GROUP_INFO, OnMenu_ViewGroupInfo)			//右键菜单查看群资料
		COMMAND_ID_HANDLER_EX(IDM_CREATE_NEW_GROUP, OnMenu_CreateNewGroup)		//右键菜单创建群
		COMMAND_ID_HANDLER_EX(IDM_REFRESH_GROUP_LIST,OnMenu_RefreshGroupList) //刷新群组列表
        COMMAND_ID_HANDLER_EX(IDM_EXIT_GROUP, OnMenu_ExitGroup)					//右键菜单退出该群

		//显示昵称和账户、显示昵称、显示账号、显示清爽资料
		//COMMAND_RANGE_HANDLER_EX(ID_32911, ID_32914, OnMenu_ShowNameChoice)
        
        //网络错误
        MESSAGE_HANDLER_EX(FMG_MSG_NET_ERROR, OnNetError)

		MESSAGE_HANDLER_EX(FMG_MSG_LOGIN_RESULT, OnLoginResult)
		MESSAGE_HANDLER_EX(FMG_MSG_LOGOUT_RESULT, OnLogoutResult)

		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_BUDDY_INFO, OnUpdateBuddyInfo)		 //更新用户信息
		//MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_BUDDY_NUMBER, OnUpdateBuddyNumber)	 //更新用户昵称(TODO: 名字要改)
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_BUDDY_HEADPIC, OnUpdateBuddyHeadPic)
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_BUDDY_SIGN, OnUpdateBuddySign)
		MESSAGE_HANDLER_EX(FMG_MSG_SELF_STATUS_CHANGE, OnSelfStatusChange)

		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_CHATDLG_USERINFO, OnUpdateChatDlgOnlineStatus)			//更新聊天窗口中用户状态
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_BUDDY_LIST, OnUpdateBuddyList)		 //更新好友列表信息
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_GROUP_LIST, OnUpdateGroupList)
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_RECENT_LIST, OnUpdateRecentList)	 //更新最近联系人列表
		//MESSAGE_HANDLER_EX(FMG_MSG_GROUP_MSG, OnGroupMsg)
		//MESSAGE_HANDLER_EX(FMG_MSG_SESS_MSG, OnSessMsg)
		MESSAGE_HANDLER_EX(FMG_MSG_STATUS_CHANGE_MSG, OnStatusChangeMsg)
		MESSAGE_HANDLER_EX(FMG_MSG_KICK_MSG, OnKickMsg)
        MESSAGE_HANDLER_EX(FMG_MSG_SCREENSHOT, OnScreenshotMsg)
        
		MESSAGE_HANDLER_EX(FMG_MSG_SYS_GROUP_MSG, OnSysGroupMsg)
		
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_GMEMBER_NUMBER, OnUpdateGMemberNumber)
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_GROUP_NUMBER, OnUpdateGroupNumber)
		
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_GMEMBER_SIGN, OnUpdateGMemberSign)
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_GMEMBER_INFO, OnUpdateGMemberInfo)
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_GROUP_INFO, OnUpdateGroupInfo)				//更新群信息
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_GMEMBER_HEADPIC, OnUpdateGMemberHeadPic)
		MESSAGE_HANDLER_EX(FMG_MSG_UPDATE_GROUP_HEADPIC, OnUpdateGroupHeadPic)
		MESSAGE_HANDLER_EX(FMG_MSG_CHANGE_STATUS_RESULT, OnChangeStatusResult)
		
		//Dennis Think Begin
		MESSAGE_HANDLER_EX(FMG_MSG_ADDFREIND, OnSendAddFriendRequestResult)			//自己加好友是否发送成功
		MESSAGE_HANDLER_EX(FMG_MSG_RECVADDFRIENDREQUSET, OnRecvAddFriendRequest)		//收到加好友相关通知
		MESSAGE_HANDLER_EX(FMG_MSG_DELETEFRIEND, OnDeleteFriendResult)				//删除好友结果
		MESSAGE_HANDLER_EX(FMG_MSG_ADD_FRIEND_NOTIFY_REQ, OnAddFriendNotifyRequest)
		MESSAGE_HANDLER_EX(FMG_MSG_RECV_FRIEND_TEXT_MSG,OnRecvFriendTextMsg)
		MESSAGE_HANDLER_EX(FMG_MSG_USER_LOGIN_RSP, OnUserLoginRsp)

		MESSAGE_HANDLER_EX(FMG_MSG_NET_FAILED, OnNetFailed)
		MESSAGE_HANDLER_EX(FMG_MSG_NET_RECOVER, OnNetRecover)
		
		MESSAGE_HANDLER_EX(FMG_MSG_RECV_GROUP_MSG,OnRecvGroupMsg)

		MESSAGE_HANDLER_EX(FMG_MSG_FRIEND_FILE_RECV_REQ, OnRecvFriendFileReqMsg)
		MESSAGE_HANDLER_EX(FMG_MSG_FRIEND_FILE_SEND_RSP, OnSendFriendFileRspMsg)
		MESSAGE_HANDLER_EX(FMG_MSG_FRIEND_FILE_NOTIFY_REQ, OnNotifyFriendFileReqMsg)
		MESSAGE_HANDLER_EX(FMT_MSG_FRIEND_CHAT_HISTORY,OnRecvFriendHistoryMsg)
		MESSAGE_HANDLER_EX(FMT_MSG_GROUP_CHAT_HISTORY,OnRecvGroupHistoryMsg)

		MESSAGE_HANDLER_EX(FMG_MSG_SEND_FILE_PROGRESS, OnSendFileProgress)
		//MESSAGE_HANDLER_EX(FMG_MSG_SEND_FILE_RESULT, OnSendFileResult)
		MESSAGE_HANDLER_EX(FMG_MSG_RECV_FILE_PROGRESS, OnRecvFileProgress)
		MESSAGE_HANDLER_EX(FMT_MSG_FRIEND_FILE_TRANS_RESULT,OnRecvFriendFileTransResultNotifyMsg)
		//Dennis Think End

		MESSAGE_HANDLER_EX(WM_TRAYICON_NOTIFY, OnTrayIconNotify)
		MESSAGE_HANDLER_EX(WM_SHOW_BUDDY_CHAT_DLG, OnShowBuddyChatDlg)				//显示或关闭与好友聊天对话框
		MESSAGE_HANDLER_EX(WM_SHOW_GROUP_CHAT_DLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_SESS_CHAT_DLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_SYS_GROUP_CHAT_DLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_BUDDY_INFO_DLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_GMEMBER_INFO_DLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_GROUP_INFO_DLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_BUDDY_CHAT_DLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_GROUP_CHAT_DLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_SESS_CHAT_DLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_SYS_GROUP_CHAT_DLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_BUDDY_INFO_DLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_GMEMBER_INFO_DLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_GROUP_INFO_DLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_MAIN_DLG, OnCloseDlg)						//关闭主对话框
		MESSAGE_HANDLER_EX(WM_DEL_MSG_SENDER, OnDelMsgSender)
		MESSAGE_HANDLER_EX(WM_CANCEL_FLASH, OnCancelFlash)

		REFLECT_NOTIFICATIONS()													//消息反射通知宏
	END_MSG_MAP()

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnSysCommand(UINT nID, CPoint pt);
	void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	void OnGetMinMaxInfo(LPMINMAXINFO lpMMI);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnTimer(UINT_PTR nIDEvent);
	void OnSize(UINT nType, CSize size);
	void OnHotKey(int nHotKeyID, UINT uModifiers, UINT uVirtKey);
	void OnClose();
	void OnDestroy();
	void OnAppAbout(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnTrayIconNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnMenu_ShowMainPanel(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnMenu_LockPanel(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnMenu_Exit(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnMenu_Mute(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtn_Sign(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtn_UnlockPanel(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtn_ShowAddFriendDlg(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtn_ShowSystemSettingDlg(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnBtn_OpenMail(UINT uNotifyCode, int nID, CWindow wndCtl);  
	void OnBtn_ModifyPassword(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtn_MainMenu(UINT uNotifyCode, int nId, CWindow wndCtl);
	void OnBtn_MultiChat(UINT uNotifyCode, int nId, CWindow wndCtl);
	LRESULT OnTabCtrlDropDown(LPNMHDR pnmh);
	LRESULT OnTabCtrlSelChange(LPNMHDR pnmh);
	LRESULT OnBuddyListDblClk(LPNMHDR pnmh);
	LRESULT OnBuddyListRButtonUp(LPNMHDR pnmh);
    LRESULT OnBuddyListHover(LPNMHDR pnmh);   
	LRESULT OnGroupListDblClk(LPNMHDR pnmh);
	LRESULT OnGroupListRButtonUp(LPNMHDR pnmh);
	LRESULT OnRecentListDblClk(LPNMHDR pnmh);
	LRESULT OnRecentListRButtonUp(LPNMHDR pnmh);
	
	void OnRefreshBuddyList(UINT uNotifyCode, int nID, CWindow wndCtl);			// 刷新联系人列表
	void OnShowOnlineBuddyOnly(UINT uNotifyCode, int nID, CWindow wndCtl);		// 仅显示在线好友
	void OnMainMenu_About(UINT uNotifyCode, int nID, CWindow wndCtl);			// 主菜单关于菜单点击
	void OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl);				// “取消”按钮(取消登录)
	void OnPic_Clicked_Head(UINT uNotifyCode, int nID, CWindow wndCtl);			// “用户头像”控件

	void OnEdit_Sign_KillFocus(UINT uNotifyCode, int nID, CWindow wndCtl);	// “个性签名”编辑文本框
	void OnMenu_Status(UINT uNotifyCode, int nID, CWindow wndCtl);				// “用户在线状态”菜单

	void OnBuddyListAddTeam(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBuddyListDeleteTeam(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBuddyListModifyTeamName(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnMoveBuddyToTeam(UINT uNotifyCode, int nID, CWindow wndCtl);


	void OnMenu_SendBuddyMessage(UINT uNotifyCode, int nID, CWindow wndCtl);	//右键菜单发送即时消息
	void OnMenu_SendBuddyMessageFromRecentList(UINT uNotifyCode, int nID, CWindow wndCtl);	//最近联系人右键菜单发送即时消息
	void OnMenu_DeleteFriend(UINT uNotifyCode, int nID, CWindow wndCtl);		//右键菜单删除好友
	void OnClearAllRecentList(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnDeleteRecentItem(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnMenu_ViewBuddyInfo(UINT uNotifyCode, int nID, CWindow wndCtl);		//右键菜单查看好友资料
	void OnMenu_ViewBuddyInfoFromRecentList(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnMenu_ModifyBuddyMarkName(UINT uNotifyCode, int nID, CWindow wndCtl);		//右键菜单修改好友备注名称

	void OnMenu_SendGroupMessage(UINT uNotifyCode, int nID, CWindow wndCtl);	
	void OnMenu_ViewGroupInfo(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnMenu_ExitGroup(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnMenu_CreateNewGroup(UINT uNotifyCode, int nID, CWindow wndCtl);

	//Dennis Think Begin
	void OnMenu_RefreshGroupList(UINT uNotifyCode, int nID, CWindow wndCtl); //刷新群组列表
	//Dennis Think End
	void OnMenu_ShowNameChoice(UINT uNotifyCode, int nID, CWindow wndCtl);		//名称显示各个子菜单

	void CloseDialog(int nVal);
	void OnFindBtn(UINT uNotifyCode, int nID, CWindow wndCtl);

	BOOL InitTopToolBar();		// 初始化Top工具栏
	BOOL InitBottomToolBar();	// 初始化Bottom工具栏
	BOOL InitTabCtrl();			// 初始化Tab栏
	BOOL InitBuddyListCtrl();	// 初始化好友列表控件
	BOOL InitGroupListCtrl();	// 初始化群列表控件
	BOOL InitRecentListCtrl();	// 初始化最近联系人列表控件

	BOOL InitUI();
	void UninitUI();

    //返回true,表示登录对话框走的是正常登录流程;返回false,表示直接关闭了登录对话框
	bool StartLogin(BOOL bAutoLogin = FALSE);
	void CloseAllDlg();

    LRESULT OnNetError(UINT uMsg, WPARAM wParam, LPARAM lParam);    
	LRESULT OnLoginResult(UINT uMsg, WPARAM wParam, LPARAM lParam);			// 登录返回消息
	LRESULT OnLogoutResult(UINT uMsg, WPARAM wParam, LPARAM lParam);		// 注销返回消息
	LRESULT OnGoOffline(UINT uMsg, WPARAM wParam, LPARAM lParam);			// 下线
	LRESULT OnUpdateUserInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);		// 更新用户信息
	LRESULT OnUpdateBuddyList(UINT uMsg, WPARAM wParam, LPARAM lParam);		// 更新好友列表消息
	LRESULT OnUpdateGroupList(UINT uMsg, WPARAM wParam, LPARAM lParam);		// 更新群列表消息
	LRESULT OnUpdateRecentList(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 更新最近联系人列表消息
	LRESULT OnBuddyMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);			// 好友消息
	LRESULT OnGroupMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);			// 群消息
	LRESULT OnSessMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);				// 临时会话消息
	LRESULT OnStatusChangeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);		// 好友状态改变消息
	LRESULT OnKickMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);				// 被踢下线消息
    LRESULT OnScreenshotMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);		// 屏幕截图消息   
	LRESULT OnSysGroupMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);			// 群系统消息
	LRESULT OnUpdateBuddyNumber(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 更新好友号码
	LRESULT OnUpdateGMemberNumber(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 更新群成员号码_
	LRESULT OnUpdateGroupNumber(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 更新群号码
	LRESULT OnUpdateBuddySign(UINT uMsg, WPARAM wParam, LPARAM lParam);		// 更新好友个性签名
	LRESULT OnUpdateGMemberSign(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 更新群成员个性签名
	LRESULT OnUpdateBuddyInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);		// 更新好友信息
	LRESULT OnUpdateChatDlgOnlineStatus(UINT uMsg, WPARAM wParam, LPARAM lParam);	////更新聊天窗口中好友的在线与离线状态	
	LRESULT OnUpdateGMemberInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 更新群成员信息
	LRESULT OnUpdateGroupInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);		// 更新群信息
	LRESULT OnUpdateBuddyHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 更新好友头像图片
	LRESULT OnUpdateGMemberHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam);// 更新群成员头像图片
	LRESULT OnUpdateGroupHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 更新群头像图片
	LRESULT OnChangeStatusResult(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 改变在线状态返回消息
	LRESULT OnSendAddFriendRequestResult(UINT uMsg, WPARAM wParam, LPARAM lParam);//加好友向服务器发请求的成败结果
	LRESULT OnRecvAddFriendRequest(UINT uMsg, WPARAM wParam, LPARAM lParam);// 收到加好友的请求
	LRESULT OnAddFriendNotifyRequest(UINT uMsg, WPARAM wParam, LPARAM lParam);// 收到加好友结果的通知的请求
	LRESULT OnDeleteFriendResult(UINT message, WPARAM wParam, LPARAM lParam);//删除好友结果
	LRESULT OnSelfStatusChange(UINT message, WPARAM wParam, LPARAM lParam); //自己在另外终端上下线通知
	
	//Dennis Begin
	LRESULT OnRecvFriendTextMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);//收到的文本消息
	LRESULT OnUserLoginRsp(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnNetFailed(UINT uMsg, WPARAM wParam, LPARAM lParam);   //网络故障，失去连接
	LRESULT OnNetRecover(UINT uMsg, WPARAM wParam, LPARAM lParam);  //网络恢复
	LRESULT OnRecvGroupMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);  //网络恢复
	LRESULT OnRecvFriendHistoryMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);  //网络恢复
	LRESULT OnRecvGroupHistoryMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);  //网络恢复
	LRESULT OnSendFriendFileRspMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);//收到的文件请求消息
	LRESULT OnRecvFriendFileReqMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);//收到的文件回复消息
	LRESULT OnNotifyFriendFileReqMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);//收到的文件回复消息
	LRESULT OnSendFileProgress(UINT uMsg, WPARAM wParam, LPARAM lParam);  //发送文件的进度
	LRESULT OnRecvFileProgress(UINT uMsg, WPARAM wParam, LPARAM lParam);  //发送文件的进度
	LRESULT OnRecvFriendFileTransResultNotifyMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);//收到好友文件结果消息
	//Dennis End

	//


	LRESULT OnCloseDlg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnShowOrCloseDlg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnShowBuddyChatDlg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDelMsgSender(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCancelFlash(UINT uMsg, WPARAM wParam, LPARAM lParam);


	//void ShowBuddyChatDlg(UINT nUTalkUin, BOOL bShow, BOOL bShakeWindowMsg=FALSE, UINT nMsgID=1000);
	void ShowGroupChatDlg(const std::string strGroupId, BOOL bShow);
	void ShowSessChatDlg(UINT nGroupCode, UINT nUTalkUin, BOOL bShow);
	void ShowSysGroupChatDlg(UINT nGroupCode, BOOL bShow);
	void ShowBuddyInfoDlg(const std::string strFriendId, BOOL bShow);
																	 
	void ShowUserInfoDlg(UINT nUTalkUin, BOOL bShow);						// 显示或者关闭个人信息对话框
	void ShowGMemberInfoDlg(UINT nGroupCode, UINT nUTalkUin, BOOL bShow);
	void ShowGroupInfoDlg(const std::string& strGroupId, BOOL bShow);

	void NotifyBuddyChatDlg(UINT nUTalkUin, UINT uMsg);						// 通知好友聊天窗口更新
	void NotifyGroupChatDlg(UINT nGroupCode,								// 通知群聊天窗口更新
		UINT uMsg, WPARAM wParam, LPARAM lParam);
	void NotifySessChatDlg(UINT nUTalkUin, UINT uMsg);						// 通知临时会话聊天窗口更新
	void NotifyBuddyInfoDlg(UINT nUTalkUin, UINT uMsg);						// 通知好友信息窗口更新
	void NotifyGMemberInfoDlg(UINT nGroupCode, UINT nUTalkUin, UINT uMsg);	// 通知群成员信息窗口更新
	void NotifyGroupInfoDlg(UINT nGroupCode, UINT uMsg);					// 通知群信息窗口更新

	void UpdateBuddyTreeCtrl();
	void BuddyListSortOnStaus();											//根据在线和离线状态排列好友列表
	void UpdateGroupTreeCtrl();
	void UpdateRecentTreeCtrl();

	void OnTrayIcon_LButtunUp();
	void OnTrayIcon_RButtunUp();
	void OnTrayIcon_MouseHover();
	void OnTrayIcon_MouseLeave();

	BOOL LoadAppIcon(E_UI_ONLINE_STATUS nStatus);
	void DestroyAppIcon();
	BOOL LoadLoginIcon();
	void DestroyLoginIcon();
	BOOL LoadAddFriendIcon();
	void DestroyAddFriendIcon();
	void UpdateMsgIcon();

	CString GetHeadPicFullName(UINT nGroupCode, UINT nUTalkUin);
	void GetNumber(UINT nGroupCode, UINT nUTalkUin, UINT& nGroupNum, UINT& nUTalkNum);
	HICON ExtractIcon(LPCTSTR lpszFileName);

	void ShowPanel(BOOL bShow);												// 显示指定面板函数(bShow：TRUE表示显示主面板，FALSE表示显示登录面板)
	void ShowLockPanel();

	E_UI_ONLINE_STATUS GetStatusFromMenuID(int nMenuID);									// 从菜单ID获取对应的UTalk_STATUS
	void StatusMenuBtn_SetIconPic(CSkinButton& btnStatus, E_UI_ONLINE_STATUS nStatus);	// 根据指定状态设置状态菜单按钮的图标

	void CreateEssentialDirectories();										//登录成功以后创建必需文件夹

	void ShowAddFriendConfirmDlg();											//显示加好友请求对话框 

	BOOL InsertTeamMenuItem(CSkinMenu& popMenu);

	void SaveCurrentLogonUserToFile();										//将当前登录的账户保存到文件中
	void DeleteCurrentUserFromFile();										//从文件中删除当前登录的账户

	BOOL IsFilesTransferring();												//聊天对话框是否有文件正在进行传输


	//Dennis Begin
	void ShowBuddyChatDlg(const std::string& strFriendName);
	void CloseBuddyChatDlg(const std::string& strFriendName);
	void DoMsgList();
	void DoRecvAddFriendTroyIconCartoon();//好友消息闪烁动画
	void DoAddFriendRecvReqMsg(C_WND_MSG_OperateFriendResult* pResult);
	void DoAddFriendNotifyReqMsg(C_WND_MSG_AddFriendNotifyRequest* pResult);
	void DoRecvFriendChatTextMsg(C_WND_MSG_BuddyTextMessage* pResult);
	//Dennis End
public:

private:
	std::shared_ptr<spdlog::logger> m_loger=nullptr;
	CSkinDialog				m_SkinDlg;										//主框架对话框
	CLoginDlg				m_LoginDlg;			//登录对话框
	CFindFriendDlg*			m_pFindFriendDlg;								//查找好友对话框
	CMsgTipDlg				m_MsgTipDlg;
	CLogonUserInfoDlg       m_LogonUserInfoDlg;						
	CModifyPasswordDlg		m_ModifyPasswordDlg;							//修改密码的对话框

	CSkinButton				m_btnMainMenu;									//主界面菜单按钮
	CSkinMenu				m_SkinMenu;										//左下角带头像的菜单

	//系统托盘区相关的变量 Begin
	CSkinMenu               m_TrayMenuNormal;                               //托盘区按钮普通状态
	CSkinMenu               m_TrayMenuLock;								    //托盘区按钮的锁定状态
	bool                    m_bShowTrayMenu; //是否显示托盘区菜单
	//系统托盘区相关的变量 End
	
	CSkinButton				m_btnMultiChat;									//群发按钮
	CSkinButton				m_btnFind;										//查找好友按钮
	CSkinEdit				m_edtSign;                                      //个性签名展示
	CSkinEdit               m_edtSearch;                                    //查找按钮
	CSkinPictureBox			m_picHead;                                      //个人头像
	
	//BOOL					m_bPicHeadPress;

	CSkinStatic				m_staNickName;									//个人昵称静态展示
	
	CSkinButton				m_btnSign;						                //个人签名静态展示

	CSkinButton				m_btnMail;										//邮件按钮
	
	CSkinToolBar			m_tbTop;                                        //主界面顶层工具栏
	CSkinToolBar            m_tbBottom;                                     //主界面底层工具栏
	CSkinTabCtrl			m_TabCtrl;		                                 //主界面的Tab页控件

	CRecentListCtrl			m_RecentListCtrl;//最近聊天列表
	
    //主界面好友列表相关变量 Begin
	CBuddyListCtrl			m_BuddyListCtrl;//好友列表
	CSkinMenu               m_BuddySelectedMenu;//好友选中时的右键菜单
	CSkinMenu               m_BuddyEmptyAreaMenu;//好友列表空白区右键菜单
	CSkinMenu               m_BuddyGroupSelectMenu;//好友分组选中时的右键菜单
	//主界面好友列表相关变量 End


	//群组列表相关变量 Begin
	CBuddyListCtrl			m_GroupListCtrl;//群组列表
	CSkinMenu               m_GroupSelectedMenu;//群组选中时的右键菜单
	CSkinMenu               m_GroupEmptyAreaMenu;//群组列表空白区右键菜单
	CSkinMenu               m_GroupGroupSelectMenu;//群组分组选中时的右键菜单
	//群组列表相关变量 End


	//主界面
	CSkinMenu               m_MainBoardMenu;
	
	CSkinPictureBox			m_picLogining;//登录中的图片
	CSkinStatic				m_staUTalkNum;
	CSkinStatic				m_staLogining;
	CSkinButton				m_btnCancel;                                    //取消按钮
	CSkinButton				m_btnUnlock;									//解锁按钮


	std::map<std::string, CBuddyChatDlg*> m_mapBuddyChatDlg;//好友聊天对话框MAP
	std::map<std::string, CGroupChatDlg*> m_mapGroupChatDlg;//群组聊天对话框MAP


	std::map<std::string, CBuddyInfoDlg*> m_mapBuddyInfoDlg;//好友信息对话框MAP
 	std::map<CGMemberInfoMapKey, CBuddyInfoDlg*> m_mapGMemberInfoDlg;//群成员信息对话框MAP
	std::map<std::string, CGroupInfoDlg*> m_mapGroupInfoDlg;//群组信息对话框MAP

	CFaceList						m_FaceList;
	CTrayIcon						m_TrayIcon;


	HICON							m_hAppIcon;
	HICON							m_hLoginIcon[6];

	HICON							m_hMsgIcon;//消息图标
	HICON							m_hAddFriendIcon[2];

	HICON							m_hDlgIcon;			//对话框图标
	HICON                           m_hDlgSmallIcon;    //对话框的小图标
	CLoginAccountList				m_LoginAccountList; //所有的登录账号的列表
	LOGIN_ACCOUNT_INFO				m_stAccountInfo;						//当前即将要登录的账户信息
	CCascadeWinManager				m_CascadeWinManager;
		
	BOOL							m_bPanelLocked;							//主面板处于锁定状态

	HRGN							m_hHotRgn;


	CRect							m_rcTrayIconRect;        //系统托盘区矩形

	long							m_nCurSelIndexInMainTab;		//主面板Tab选择索引号

    MAINPANEL_STATUS                m_nMainPanelStatus;             //主面板状态
	std::vector<CWndMsgBase*>          m_msgList;                      //新的消息列表

	std::shared_ptr<CMsgProto> m_netProto;
	CUserMgr& m_userMgr;
	CUserConfig& m_userCfg;
	int m_nLoginTryTimes;

	//Core
private:
	E_UI_ONLINE_STATUS GetStatus() {
		if (nullptr != m_netProto) {
			return m_netProto->GetStatus();
		}
		else {
			return E_UI_ONLINE_STATUS::STATUS_OFFLINE;
		}
	}

	void InitMsgTypeMap();
	bool GetFriendList();
	bool IsOffLine();
	bool SendGetGroupList();

	LRESULT DoLoginSucceed();

	void ChangeTrayIconString(const CString strMsg);
	void SetTrayIconOnLine();
	void SetTrayIconOffLine();
	bool CreateGroupChatDlg(const std::string strGroupId);

	bool InitNetConnect();
};
#endif