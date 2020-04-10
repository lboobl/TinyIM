// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"
#include "IniFile.h"
#include "net/IUProtocolData.h"
#include "SystemSettingDlg.h"
#include "ClosePromptDlg.h"
#include "UI_USER_INFO.h"
#include "ModifyMarkNameDlg.h"
#include "TeamDlg.h"
#include "BuddyChatDlg.h"
#include "GroupChatDlg.h"
#include "BuddyInfoDlg.h"
#include "GroupInfoDlg.h"
#include "FindFriendDlg.h"
#include "AddFriendConfirmDlg.h"
#include "Updater.h"
#include "UpdateDlg.h"
#include "CreateNewGroupDlg.h"
#include "GDIFactory.h"
#include "IULog.h"
#include "Startup.h"
#include "ChatDlgCommon.h"
#include "EncodingUtil.h"
#include "UIText.h"

//#include "net/Msg.h"
#include "Proto.h"
#include "UIDefaultValue.h"
#pragma comment(lib, "winmm.lib")

extern HWND g_hwndOwner;


//主菜单各个子菜单索引号
enum MAIN_PANEL_MEMU
{
	MAIN_PANEL_STATUS_SUBMENU_INDEX,
	MAIN_PANEL_TRAYICON_SUBMENU_INDEX,//这个是什么
	MAIN_PANEL_TRAYICON_SUBMENU2_INDEX,//这个是什么
	MAIN_PANEL_RECENT_SUBMENU_INDEX,
	MAIN_PANEL_BUDDYLIST_SUBMENU_INDEX,
	MAIN_PANEL_GROUP_SUBMENU_INDEX,
	MAIN_PANEL_BUDDYLIST_CONTEXT_SUBMENU_INDEX,
	MAIN_PANEL_MAIN_SUBMENU_INDEX,
	MAIN_PANEL_LOCK_SUBMENU_INDEX,
	MAIN_PANEL_RECENTLIST_CONTEXT_SUBMENU_INDEX,
	MAIN_PANEL_GROUPLIST_CONTEXT_SUBMENU_INDEX,
	MAIN_PANEL_BUDDYLIST_TEAM_CONTEXT_SUBMENU_INDEX,
	MAIN_PANEL_BUDDYLIST_BLANK_CONTEXT_SUBMENU_INDEX,
	MAIN_PANEL_GROUPLIST_BLANK_CONTEXT_SUBMENU_INDEX
};

static std::mutex g_mutex;

/**
 * @brief 对消息进行一些预处理
 * 
 * @param pMsg 
 * @return BOOL 
 */
BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == m_edtSign.m_hWnd)   
	{
		if (pMsg->message == WM_CHAR && pMsg->wParam == VK_RETURN) 
		{
			SetFocus();
		} 
	}

 	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
 	{
 		return FALSE;
 	}

	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

/**
 * @brief Construct a new CMainDlg::CMainDlg object
 * 
 */
CMainDlg::CMainDlg(void) :m_userMgr(CUserMgr::GetInstance()), m_userCfg(CUserConfig::GetInstance())
{	
	
	//UI
	m_hAppIcon = NULL;
	memset(m_hLoginIcon, 0, sizeof(m_hLoginIcon));
	memset(m_hAddFriendIcon, 0, sizeof(m_hAddFriendIcon));
	m_hDlgIcon = m_hDlgSmallIcon = NULL;
	m_pFindFriendDlg = new CFindFriendDlg();
	m_rcTrayIconRect.SetRectEmpty();
	m_nCurSelIndexInMainTab = -1;
	m_nMainPanelStatus = MAIN_PANEL_STATUS_NOT_LOGIN;

	m_hHotRgn = NULL;
	m_bPanelLocked = FALSE;
	//CORE
	m_loger = CreateLogger();
	m_netProto = CMsgProto::GetInstance();
	m_hMsgIcon = NULL;
	memset(&m_stAccountInfo, 0, sizeof(m_stAccountInfo));
	m_nLoginTryTimes = 0;

	InitNetConnect();

	//
	m_bShowTrayMenu = false;
}

//TODO: 此处的配置初始化代码待优化
bool CMainDlg::InitNetConnect()
{
	{
		std::string strServerIp = "127.0.0.1";
		int serverPort = 9000;
		{
			CIniFile iniFile;
			CString strIniPath(g_szHomePath);
			strIniPath += _T("config\\flamingo.ini");
			CString strTemp;
			iniFile.ReadString(_T("server"), _T("server"), _T("flamingo.hootina.org"), strTemp.GetBuffer(64), 64, strIniPath);
			strTemp.ReleaseBuffer();
			strServerIp = EncodeUtil::UnicodeToUtf8(strTemp);
			iniFile.ReadString(_T("server"), _T("port"), _T("20000"), strTemp.GetBuffer(32), 32, strIniPath);
			strTemp.ReleaseBuffer();
			serverPort = std::atoi(EncodeUtil::UnicodeToUtf8(strTemp));
		}
		m_netProto = CMsgProto::GetInstance();
		if (m_netProto)
		{
			m_netProto->SetIpPort(strServerIp, serverPort);
			m_netProto->StartConnect();
		}
	}
	return true;
}

CMainDlg::~CMainDlg(void)
{
	delete m_pFindFriendDlg;
}


/**
 * @brief 初始化对话框，将类和窗口的句柄关联
 * 
 * @param wndFocus 
 * @param lInitParam 
 * @return BOOL 
 */
BOOL CMainDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	// set icons
	{
		m_hDlgIcon = AtlLoadIconImage(IDR_MAIN_FRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
		SetIcon(m_hDlgIcon, TRUE);
		m_hDlgSmallIcon = AtlLoadIconImage(IDR_MAIN_FRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		SetIcon(m_hDlgSmallIcon, FALSE);
	}
	
	//Init Message Loop
	{
		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);
	}

	UIAddChildWindowContainer(m_hWnd);
	
	//去除最大化按钮
	ModifyStyle(WS_MAXIMIZEBOX, 0);
	InitUI();

	LoadAppIcon(GetStatus());
	
	m_TrayIcon.AddIcon(m_hWnd, WM_TRAYICON_NOTIFY, 1, m_hAppIcon, _T("TinyIM未登录"));

	//创建Users目录
	CString strAppPath(g_szHomePath);
	CString strUsersDirectory(strAppPath);
	strUsersDirectory += _T("Users");
	if (!Hootina::CPath::IsDirectoryExist(strUsersDirectory))
	{
		Hootina::CPath::CreateDirectory(strUsersDirectory);
	}
	
	// 加载系统表情列表
	WString strFileName = Hootina::CPath::GetAppPath() + _T("Face\\FaceConfig.xml");
	m_FaceList.LoadConfigFile(strFileName.c_str());

	// 加载登录帐号列表
	strFileName = Hootina::CPath::GetAppPath()+_T("Users\\LoginAccountList.dat");
	m_LoginAccountList.LoadFile(strFileName.c_str());

	//在这个函数里面显示登录对话框
    //如果登录对话框不是走正常登录,则主对话框也不应该创建出来
	if (!StartLogin(m_LoginAccountList.IsAutoLogin()))
	{
		return TRUE;
	}


	//设置默认位置
	long cxScreen = ::GetSystemMetrics(SM_CXSCREEN);
	cxScreen -= 110;
	::SetWindowPos(m_hWnd, NULL, cxScreen-280, 78, 280, 675, SWP_SHOWWINDOW);

	ModifyStyle(0, WS_CLIPCHILDREN);
	ModifyStyleEx(WS_EX_APPWINDOW, 0);

	m_BuddyListCtrl.SetStyle(BLCTRL_STYLE::BLC_BIG_ICON_STYLE);
	InitMsgTypeMap();
	return TRUE;
}


/**
 * @brief 响应系统命令
 * 
 * @param nID 
 * @param pt 
 */
void CMainDlg::OnSysCommand(UINT nID, CPoint pt)
{
	if (nID == SC_MINIMIZE)
	{
        ShowWindow(SW_HIDE);
		return;
	}

	SetMsgHandled(FALSE);
}


/**
 * @brief 皮肤按钮的命令
 * 
 * @param nIDCtl 
 * @param lpMeasureItemStruct 
 */
void CMainDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	m_SkinMenu.OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}


/**
 * @brief 绘画皮肤按钮
 * 
 * @param nIDCtl 
 * @param lpDrawItemStruct 
 */
void CMainDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	m_SkinMenu.OnDrawItem(nIDCtl, lpDrawItemStruct);
}


/**
 * @brief 响应最大最小化
 * TODO: 此处的变量需要默认值
 * @param lpMMI 
 */
void CMainDlg::OnGetMinMaxInfo(LPMINMAXINFO lpMMI)
{
	lpMMI->ptMinTrackSize.x = 280;
	lpMMI->ptMinTrackSize.y = 520;

	lpMMI->ptMaxTrackSize.x = 608;
	lpMMI->ptMaxTrackSize.y = ::GetSystemMetrics(SM_CYSCREEN);
}


/**
 * @brief 响应鼠标左键按下
 * 
 * @param nFlags 
 * @param point 
 */
void CMainDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (GetFocus() == m_edtSign.m_hWnd)
	{
		SetFocus();
	}
}

/**
 * @brief 响应定时器
 * 
 * @param nIDEvent 
 */
void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	m_TrayIcon.OnTimer(nIDEvent);
	if (!m_msgList.empty())
	{
		DoRecvAddFriendTroyIconCartoon();
	}
}




/**
 * @brief 响应主窗口大小变化,UI 有关
 * 
 * @param nType 
 * @param size 
 */
void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);

	//m_SkinDlg.SetBgPic(_T("main_panel_bg.png"), CRect(2, 280, 2, 127));	
	//m_SkinDlg.InvalidateRect(NULL, TRUE);

	CRect rcClient;
	GetClientRect(&rcClient);
	HDWP hdwp = ::BeginDeferWindowPos(18);
	
	if (m_btnMainMenu.IsWindow())
	{
		::DeferWindowPos(hdwp, m_btnMainMenu.m_hWnd, NULL, 6, rcClient.Height() - 28, 22, 20, SWP_NOZORDER);
	}
		//m_btnMainMenu.MoveWindow(6, rcClient.Height() - 28, 22, 20);
	
	if (m_btnMultiChat.IsWindow())
	{
		::DeferWindowPos(hdwp, m_btnMultiChat.m_hWnd, NULL, 30, rcClient.Height() - 28, 22, 20, SWP_NOZORDER);
	}
		//m_btnMultiChat.MoveWindow(30, rcClient.Height() - 28, 22, 20, TRUE);

	if (m_picHead.IsWindow())
	{
		::DeferWindowPos(hdwp, m_picHead.m_hWnd, NULL, 10, 30, 64, 64, SWP_NOZORDER);
	}
		//m_picHead.MoveWindow(10, 30, 64, 64, TRUE);
	
	if (m_staNickName.IsWindow())
	{
		::DeferWindowPos(hdwp, m_staNickName.m_hWnd, NULL, 80, 34, rcClient.Width() - 102, 20, SWP_NOZORDER);
	}
		//m_staNickName.MoveWindow(80, 34, 100, 20, FALSE);

	if (m_btnFind.IsWindow())
	{
		::DeferWindowPos(hdwp, m_btnFind.m_hWnd, NULL, rcClient.right - 60, rcClient.bottom - 29, 50, 20, SWP_NOZORDER);
	}
		//m_btnFind.MoveWindow(rcClient.right - 60, rcClient.bottom-29, 50, 20);

	if (m_btnSign.IsWindow())
	{
		::DeferWindowPos(hdwp, m_btnSign.m_hWnd, NULL, 80, 60, rcClient.Width() - 102, 22, SWP_NOZORDER);
	}
		//m_btnSign.MoveWindow(80, 60, rcClient.Width()-102, 22);

	if (m_edtSign.IsWindow())
	{
		::DeferWindowPos(hdwp, m_edtSign.m_hWnd, NULL, 80, 60, rcClient.Width() - 102, 22, SWP_NOZORDER);
	}
		//m_edtSign.MoveWindow(80, 60, rcClient.Width()-102, 22);

	if (m_edtSearch.IsWindow())
	{
		::DeferWindowPos(hdwp, m_edtSearch.m_hWnd, NULL, 0, 126, rcClient.Width(), 27, SWP_NOZORDER);
	}
		//m_edtSearch.MoveWindow(0, 126, rcClient.Width(), 27);

	if (m_tbBottom.IsWindow())
	{
		::DeferWindowPos(hdwp, m_tbBottom.m_hWnd, NULL, 46, rcClient.bottom - 60, 212, 22, SWP_NOZORDER);
	}
		//m_tbBottom.MoveWindow(46, rcClient.bottom-60, 212, 22);

	if (m_TabCtrl.IsWindow())
	{
		int nCount = m_TabCtrl.GetItemCount();
		if (nCount > 0)
		{
			int nWidth = (rcClient.Width()-2) / nCount;
			int nRemainder = (rcClient.Width()-2) % nCount;

			for (int i = 0; i < nCount; i++)
			{
				m_TabCtrl.SetItemSize(i, nWidth, 48, nWidth-19, 19);
			}

			m_TabCtrl.SetItemSize(nCount-1, nWidth+nRemainder, 48, nWidth+nRemainder-19, 19);
		}
		
		::DeferWindowPos(hdwp, m_TabCtrl.m_hWnd, NULL, 0, 153, rcClient.right-2, 49, SWP_NOZORDER);
		//m_TabCtrl.MoveWindow(0, 153, rcClient.right, 49);
	}

	if (m_BuddyListCtrl.IsWindow())
	{
		::DeferWindowPos(hdwp, m_BuddyListCtrl.m_hWnd, NULL, 0, 200, rcClient.Width(), rcClient.Height() - 236, SWP_NOZORDER);
	}
		//m_BuddyListCtrl.MoveWindow(0, 200, rcClient.Width(), rcClient.Height()-236);

	if (m_GroupListCtrl.IsWindow())
	{
		::DeferWindowPos(hdwp, m_GroupListCtrl.m_hWnd, NULL, 0, 200, rcClient.Width(), rcClient.Height() - 236, SWP_NOZORDER);
	}	//m_GroupListCtrl.MoveWindow(0, 200, rcClient.Width(), rcClient.Height()-236);

	if (m_RecentListCtrl.IsWindow())
	{
		::DeferWindowPos(hdwp, m_RecentListCtrl.m_hWnd, NULL, 0, 200, rcClient.Width(), rcClient.Height() - 236, SWP_NOZORDER);
		//m_RecentListCtrl.MoveWindow(0, 200, rcClient.Width(), rcClient.Height()-236);
	}

	if (m_picLogining.IsWindow())
	{
		::DeferWindowPos(hdwp, m_picLogining.m_hWnd, NULL, (rcClient.Width() - 220) / 2, 76, 220, 150, SWP_NOZORDER);
		//m_picLogining.MoveWindow((rcClient.Width() - 220) / 2, 76, 220, 150);
	}

	if (m_staUTalkNum.IsWindow())
	{
		::DeferWindowPos(hdwp, m_staUTalkNum.m_hWnd, NULL, rcClient.left, 226, rcClient.Width(), 14, SWP_NOZORDER);
		//m_staUTalkNum.MoveWindow(rcClient.left, 226, rcClient.Width(), 14);
	}

	if (m_staLogining.IsWindow())
	{
		::DeferWindowPos(hdwp, m_staLogining.m_hWnd, NULL, rcClient.left, 240, rcClient.Width(), 16, SWP_NOZORDER);
		//m_staLogining.MoveWindow(rcClient.left, 240, rcClient.Width(), 16);
	}

	if (m_btnCancel.IsWindow())
	{
		::DeferWindowPos(hdwp, m_btnCancel.m_hWnd, NULL, (rcClient.Width() - 86) / 2, 304, 86, 30, SWP_NOZORDER);
		//m_btnCancel.MoveWindow((rcClient.Width() - 86) / 2, 304, 86, 30);
	}

	if (m_btnUnlock.IsWindow())
	{
		::DeferWindowPos(hdwp, m_btnUnlock.m_hWnd, NULL, (rcClient.Width() - 86) / 2, 304, 86, 30, SWP_NOZORDER);
		//m_btnUnlock.MoveWindow((rcClient.Width() - 86) / 2, 304, 86, 30);
	}

	::EndDeferWindowPos(hdwp);

	//主窗口设置了WS_CLIPCHILDREN风格，主窗口采用了渐变色，所以这个签名静态文本框需要重新绘制一下，以防止背景抖动
	if (m_staNickName.IsWindow())
	{
		m_staNickName.Invalidate(FALSE);
	}
}


/**
 * @brief 响应热键
 * TODO: 了解热键注册的机制
 * @param nHotKeyID 
 * @param uModifiers 
 * @param uVirtKey 
 */
void CMainDlg::OnHotKey(int nHotKeyID, UINT uModifiers, UINT uVirtKey)
{
	switch (nHotKeyID)
	{
		case HOT_KEY_SHOW_MSG:
		{
			if (::IsWindowVisible(m_hWnd))
			{
				ShowWindow(SW_HIDE);
			}
			else
			{
				OnTrayIconNotify(WM_TRAYICON_NOTIFY, NULL, WM_LBUTTONUP);
			}
		}break;
		default:
		{

		}break;
	}
}


/**
 * @brief 主窗口关闭，用户可以选择是否关闭(UI有关)
 * 
 */
void CMainDlg::OnClose()
{
	if(m_userCfg.IsEnableExitPrompt())
	{
		CClosePromptDlg closePromptDlg;
		int nRet = closePromptDlg.DoModal(m_hWnd, NULL);

		//用户选择退出程序
		if(nRet == IDC_EXIT)
		{
			CloseDialog(IDOK);
		}
		//用户选择最小化窗口
		else if(nRet == IDC_MINIMIZE)
		{
			ShowWindow(SW_HIDE);
		}
	}
	else
	{
		if (m_userCfg.IsEnableExitWhenCloseMainDlg())
		{
			CloseDialog(IDOK);
		}
		else
		{
			ShowWindow(SW_HIDE);
		}
	}
}

/**
 * @brief 主窗口销毁，用户已经确认关闭,Windows消息
 * 
 */
void CMainDlg::OnDestroy()
{	
	//记录窗口的位置
	{
		CRect rcWindow;
		GetWindowRect(&rcWindow);
		m_userCfg.SetMainDlgX(rcWindow.left);
		m_userCfg.SetMainDlgY(rcWindow.top);
		m_userCfg.SetMainDlgWidth(rcWindow.Width());
		m_userCfg.SetMainDlgHeight(rcWindow.Height());

		m_userCfg.SetFaceID(m_userMgr.m_UserInfo.m_nFace);
		m_userCfg.SetCustomFace(m_userMgr.m_UserInfo.m_strCustomFace.c_str());
	}
	//m_userCfg.SaveConfig();
	
	//存储用户信息
	m_userMgr.SaveBuddyInfo();
	//存储好友分组信息
	m_userMgr.StoreTeamInfo();
	//存储最近联系人列表
	m_userMgr.StoreRecentList();


	UninitUI();

	m_TrayIcon.RemoveIcon();
	m_SkinMenu.DestroyMenu();


	DestroyAppIcon();
	DestroyLoginIcon();
	DestroyAddFriendIcon();

	if (m_hMsgIcon != NULL)
	{
		::DestroyIcon(m_hMsgIcon);
		m_hMsgIcon = NULL;
	}

	if (m_hDlgIcon != NULL)
	{
		::DestroyIcon(m_hDlgIcon);
		m_hDlgIcon = NULL;
	}

	if (m_hDlgSmallIcon != NULL)
	{
		::DestroyIcon(m_hDlgSmallIcon);
		m_hDlgSmallIcon = NULL;
	}

	::UnregisterHotKey(m_hWnd, HOT_KEY_SHOW_MSG);	// 反注册提取消息热键

	{
		{
			auto& config = CUserConfig::GetInstance();
			WString configPath = Hootina::CPath::GetAppPath() + _T("config\\userconfig.ini");
			config.SaveConfig(configPath.c_str());
		}
	}

	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	CGDIFactory::Uninit();

}


/**
 * @brief 响应关于对话框
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnAppAbout(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CAboutDlg dlg;
	dlg.DoModal(m_hWnd, NULL);
}

//关闭主窗口
void CMainDlg::CloseDialog(int nVal)
{
	if(IsFilesTransferring())
	{
		if (IDNO == ::MessageBox(m_hWnd, _T("您还有文件正在传输，确认要退出TinyIM？"), g_strAppTitle.c_str(), MB_YESNO | MB_ICONQUESTION))
		{
			return;
		}
	}
	

	if (m_LogonUserInfoDlg.IsWindow())
	{
		m_LogonUserInfoDlg.DestroyWindow();
	}

	if (m_ModifyPasswordDlg.IsWindow())
	{
		m_ModifyPasswordDlg.DestroyWindow();
	}


	m_TrayIcon.RemoveIcon();
	CloseAllDlg();
	m_CascadeWinManager.Clear();
	ShowWindow(SW_HIDE);
	if (m_netProto)
	{
		m_netProto->SendUserLogout();
	}
	if (GetStatus() != E_UI_ONLINE_STATUS::STATUS_OFFLINE)
	{
		//
	}
	else
	{
		//::PostQuitMessage(nVal);
	}
	DestroyWindow();
	//Thread not exit,Dennis TO DO;
	exit(0);
}

/**
 * @brief 初始化Top工具栏
 * 
 * @return BOOL 
 */
BOOL CMainDlg::InitTopToolBar()
{
	int nIndex = m_tbTop.AddItem(101, STBI_STYLE_BUTTON);

	m_tbTop.SetLeftTop(2, 0);
	m_tbTop.SetTransparent(TRUE, m_SkinDlg.GetBgDC());

	CRect rcTopToolBar(4, 80, 4+210, 80+20);
	m_tbTop.Create(m_hWnd, rcTopToolBar, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_TOOL_BAR_TOP);

	return TRUE;
}


/**
 * @brief 初始化Bottom工具栏
 * 
 * @return BOOL 
 */
BOOL CMainDlg::InitBottomToolBar()
{
	int nIndex = m_tbBottom.AddItem(ID_BTN_SET, STBI_STYLE_BUTTON);

	m_tbBottom.SetTransparent(TRUE, m_SkinDlg.GetBgDC());

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcBottomToolBar(46, rcClient.bottom-60, 46+212, (rcClient.bottom-60)+22);
	m_tbBottom.Create(m_hWnd, rcBottomToolBar, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_TOOL_BAR_BOTTOM);

	return TRUE;
}


/**
 * @brief 初始化Tab栏，即最近联系人，联系人和群组
 * 
 * @return BOOL 
 */
BOOL CMainDlg::InitTabCtrl()
{
	CRect rcClient;
	GetClientRect(&rcClient);

	int nWidth = (rcClient.Width()-2) / 3;
	int nRemainder = (rcClient.Width()-2) % 3;

	int nIndex = m_TabCtrl.AddItem(301, STCI_STYLE_DROPDOWN);
	m_TabCtrl.SetItemSize(nIndex, nWidth+nRemainder, 48, nWidth+nRemainder-19, 19);
	m_TabCtrl.SetItemToolTipText(nIndex, _T("最近联系人"));
	m_TabCtrl.SetItemIconPic(nIndex, _T("MainTabCtrl\\icon_last_normal.png"), _T("MainTabCtrl\\icon_last_selected.png"));

	nIndex = m_TabCtrl.AddItem(302, STCI_STYLE_DROPDOWN);
	m_TabCtrl.SetItemSize(nIndex, nWidth, 48, nWidth-19, 19);
	m_TabCtrl.SetItemToolTipText(nIndex, _T("联系人"));
	m_TabCtrl.SetItemIconPic(nIndex, _T("MainTabCtrl\\icon_contacts_normal.png"), _T("MainTabCtrl\\icon_contacts_selected.png"));

	nIndex = m_TabCtrl.AddItem(303, STCI_STYLE_DROPDOWN);
	m_TabCtrl.SetItemSize(nIndex, nWidth, 48, nWidth-19, 19);
	m_TabCtrl.SetItemToolTipText(nIndex, _T("群/讨论组"));
	m_TabCtrl.SetItemIconPic(nIndex, _T("MainTabCtrl\\icon_group_normal.png"), _T("MainTabCtrl\\icon_group_selected.png"));

	m_TabCtrl.SetBgPic(_T("MainTabCtrl\\main_tab_bkg.png"), CRect(5,1,5,1));
	m_TabCtrl.SetItemsBgPic(NULL, _T("MainTabCtrl\\main_tab_highlight.png"), _T("MainTabCtrl\\main_tab_check.png"), CRect(5,1,5,1));
	m_TabCtrl.SetItemsArrowPic(_T("MainTabCtrl\\main_tabbtn_highlight.png"), _T("MainTabCtrl\\main_tabbtn_down.png"));

	CRect rcTabCtrl(0, 154, rcClient.right, 154+48);
	m_TabCtrl.Create(m_hWnd, rcTabCtrl, NULL, WS_CHILD | WS_VISIBLE, NULL, ID_TAB_CTRL_MAIN, NULL);

	m_TabCtrl.SetCurSel(1);

	return TRUE;
}

/**
 * @brief 初始化好友列表控件
 * 
 * @return BOOL 
 */
BOOL CMainDlg::InitBuddyListCtrl()
{
	m_BuddyListCtrl.SetMargin(CRect(2,0,2,0));
	m_BuddyListCtrl.SetBgPic(_T("BuddyList\\Bg.png"));
	m_BuddyListCtrl.SetBuddyTeamHotBgPic(_T("BuddyList\\BuddyTeamHotBg.png"));
	m_BuddyListCtrl.SetBuddyItemHotBgPic(_T("BuddyList\\BuddyItemHotBg.png"), CRect(1,1,1,1));
	m_BuddyListCtrl.SetBuddyItemSelBgPic(_T("BuddyList\\BuddyItemSelBg.png"), CRect(1,1,1,1));
	m_BuddyListCtrl.SetHeadFramePic(_T("BuddyList\\Padding4Select.png"), CRect(6,6,6,6));
	m_BuddyListCtrl.SetNormalArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTexture.png"));
	m_BuddyListCtrl.SetHotArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTextureHighlight.png"));
	m_BuddyListCtrl.SetSelArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTextureHighlight.png"));
	m_BuddyListCtrl.SetNormalExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTexture.png"));
	m_BuddyListCtrl.SetHotExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTextureHighlight.png"));
	m_BuddyListCtrl.SetSelExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTextureHighlight.png"));
	m_BuddyListCtrl.SetStdGGHeadPic(_T("BuddyList\\trad_boy.png"));
	m_BuddyListCtrl.SetStdMMHeadPic(_T("BuddyList\\trad_girl.png"));
	m_BuddyListCtrl.SetStyle(BLC_STANDARD_ICON_STYLE);
	m_BuddyListCtrl.SetShowBigIconInSel(TRUE);
	m_BuddyListCtrl.SetBuddyTeamHeight(24);
	m_BuddyListCtrl.SetBuddyItemHeightInBigIcon(54);
	m_BuddyListCtrl.SetBuddyItemHeightInSmallIcon(28);
	m_BuddyListCtrl.SetBuddyItemHeightInStandardIcon(26);
	m_BuddyListCtrl.SetBuddyItemPadding(1);

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcListCtrl(0, 200, rcClient.Width(), 200+rcClient.Height()-236);
	m_BuddyListCtrl.Create(m_hWnd, rcListCtrl, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_LIST_CTRL_BUDDY);

	return TRUE;
}

// 初始化群列表控件
BOOL CMainDlg::InitGroupListCtrl()
{
	m_GroupListCtrl.SetMargin(CRect(2,0,2,0));
	m_GroupListCtrl.SetBgPic(_T("BuddyList\\Bg.png"));
	m_GroupListCtrl.SetBuddyTeamHotBgPic(_T("BuddyList\\BuddyTeamHotBg.png"));
	m_GroupListCtrl.SetBuddyItemHotBgPic(_T("BuddyList\\BuddyItemHotBg.png"), CRect(1,1,1,1));
	m_GroupListCtrl.SetBuddyItemSelBgPic(_T("BuddyList\\BuddyItemSelBg.png"), CRect(1,1,1,1));
	m_GroupListCtrl.SetHeadFramePic(_T("BuddyList\\Padding4Select.png"), CRect(6,6,6,6));
	m_GroupListCtrl.SetNormalArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTexture.png"));
	m_GroupListCtrl.SetHotArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTextureHighlight.png"));
	m_GroupListCtrl.SetSelArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTextureHighlight.png"));
	m_GroupListCtrl.SetNormalExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTexture.png"));
	m_GroupListCtrl.SetHotExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTextureHighlight.png"));
	m_GroupListCtrl.SetSelExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTextureHighlight.png"));
	m_GroupListCtrl.SetStdGGHeadPic(_T("BuddyList\\trad_boy.png"));
	m_GroupListCtrl.SetStdMMHeadPic(_T("BuddyList\\trad_girl.png"));
	m_GroupListCtrl.SetStyle(BLC_BIG_ICON_STYLE);
	m_GroupListCtrl.SetShowBigIconInSel(TRUE);
	m_GroupListCtrl.SetBuddyTeamHeight(24);
	m_GroupListCtrl.SetBuddyItemHeightInBigIcon(54);
	m_GroupListCtrl.SetBuddyItemHeightInSmallIcon(28);
	m_GroupListCtrl.SetBuddyItemHeightInStandardIcon(26);
	m_GroupListCtrl.SetBuddyItemPadding(1);

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcListCtrl(0, 200, rcClient.Width(), 200+rcClient.Height());
	m_GroupListCtrl.Create(m_hWnd, rcListCtrl, NULL, WS_CHILD, NULL, ID_LIST_CTRL_GROUP);

	return TRUE;
}

/**
 * @brief 初始化最近联系人列表控件
 * 
 * @return BOOL 
 */
BOOL CMainDlg::InitRecentListCtrl()
{
	m_RecentListCtrl.SetMargin(CRect(2,0,2,0));
	m_RecentListCtrl.SetBgPic(_T("BuddyList\\Bg.png"));
	//m_RecentListCtrl.SetBuddyTeamHotBgPic(_T("BuddyList\\BuddyTeamHotBg.png"));
	m_RecentListCtrl.SetBuddyItemHotBgPic(_T("BuddyList\\BuddyItemHotBg.png"), CRect(1,1,1,1));
	m_RecentListCtrl.SetBuddyItemSelBgPic(_T("BuddyList\\BuddyItemSelBg.png"), CRect(1,1,1,1));
	m_RecentListCtrl.SetHeadFramePic(_T("BuddyList\\Padding4Select.png"), CRect(6,6,6,6));
	
	//m_RecentListCtrl.AddBuddyItemSmallIcon(int nID, LPCTSTR lpszToolTipText, LPCTSTR lpszFileName);
	//m_RecentListCtrl.SetNormalArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTexture.png"));
	//m_RecentListCtrl.SetHotArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTextureHighlight.png"));
	//m_RecentListCtrl.SetSelArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTextureHighlight.png"));
	//m_RecentListCtrl.SetNormalExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTexture.png"));
	//m_RecentListCtrl.SetHotExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTextureHighlight.png"));
	//m_RecentListCtrl.SetSelExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTextureHighlight.png"));
	
	m_RecentListCtrl.SetStdGGHeadPic(_T("BuddyList\\trad_boy.png"));
	m_RecentListCtrl.SetStdMMHeadPic(_T("BuddyList\\trad_girl.png"));
	//m_RecentListCtrl.SetStyle(BLC_BIG_ICON_STYLE);
	m_RecentListCtrl.SetShowBigIconInSel(TRUE);
	//m_RecentListCtrl.SetBuddyTeamHeight(24);
	m_RecentListCtrl.SetBuddyTeamHeight(0);
	m_RecentListCtrl.SetBuddyItemHeightInBigIcon(54);
	m_RecentListCtrl.SetBuddyItemHeightInSmallIcon(28);
	m_RecentListCtrl.SetBuddyItemHeightInStandardIcon(26);
	m_RecentListCtrl.SetBuddyItemPadding(1);
	m_RecentListCtrl.SetBuddyTeamExpand(0, TRUE);
	//m_RecentListCtrl.SetTransparent(TRUE, m_SkinDlg.GetBgDC());

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcListCtrl(0, 200, rcClient.Width(), 200+rcClient.Height()-236);
	m_RecentListCtrl.Create(m_hWnd, rcListCtrl, NULL, WS_CHILD, NULL, ID_LIST_CTRL_RECENT);

	return TRUE;
}


/**
 * @brief 初始化UI
 * 
 * @return BOOL 
 */
BOOL CMainDlg::InitUI()
{
	m_SkinDlg.SetBgPic(_T("main_panel_bg.png"), CRect(2, 280, 2, 127));
	m_SkinDlg.SetBgColor(RGB(255, 255, 255));
	m_SkinDlg.SetMinSysBtnPic(_T("SysBtn\\btn_mini_normal.png"), _T("SysBtn\\btn_mini_highlight.png"), _T("SysBtn\\btn_mini_down.png"));
	m_SkinDlg.SetCloseSysBtnPic(_T("SysBtn\\btn_close_normal.png"), _T("SysBtn\\btn_close_highlight.png"), _T("SysBtn\\btn_close_down.png"));
	m_SkinDlg.SubclassWindow(m_hWnd);
	m_SkinDlg.SetTitleText(_T(""));


	HDC hDlgBgDC = m_SkinDlg.GetBgDC();

	CRect rcClient;
	GetClientRect(&rcClient);

	m_btnMainMenu.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnMainMenu.SetTransparent(TRUE, hDlgBgDC);
	m_btnMainMenu.SetBgPic(_T("menu_btn_normal.png"),
		_T("menu_btn_highlight.png"), _T("menu_btn_highlight.png"), _T("menu_btn_normal.png"));
	m_btnMainMenu.SubclassWindow(GetDlgItem(ID_BTN_MAIN_MENU));
	m_btnMainMenu.MoveWindow(6, rcClient.Height() - 28, 22, 20, TRUE);
	m_btnMainMenu.SetToolTipText(_T("主菜单"));

	m_btnMultiChat.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnMultiChat.SetTransparent(TRUE, hDlgBgDC);
	m_btnMultiChat.SetBgPic(_T("multichat_btn_normal.png"),
		_T("multichat_btn_highlight.png"), _T("multichat_btn_highlight.png"), _T("multichat_btn_normal.png"));
	m_btnMultiChat.SubclassWindow(GetDlgItem(ID_BTN_MULTI_CHAT));

	m_btnMultiChat.MoveWindow(30, rcClient.Height() - 28, 22, 20, TRUE);
	m_btnMultiChat.SetToolTipText(_T("消息群发"));

	//m_btnSystemSet.SetButtonType(SKIN_ICON_BUTTON);
	//m_btnSystemSet.SetButtonType(SKIN_PUSH_BUTTON);
	//m_btnSystemSet.SetTransparent(TRUE, hDlgBgDC);
	//m_btnSystemSet.SetBgPic(NULL, _T("allbtn_highlight.png"), _T("allbtn_down.png"), NULL);
	//m_btnSystemSet.SetBgPic(_T("SystemSet_normal.png"), _T("SystemSet_highlight.png"), _T("SystemSet_highlight.png"), _T("SystemSet_normal.png"));
	//m_btnSystemSet.SetIconPic(_T("SystemSet.png"));
	//m_btnSystemSet.SubclassWindow(GetDlgItem(ID_BTN_SYSTEM_SET));
	//m_btnSystemSet.MoveWindow(28 + 6, rcClient.Height() - 28, 22, 20, TRUE);
	//m_btnSystemSet.SetToolTipText(_T("打开系统设置"));





	//m_btnFind.SetButtonType(SKIN_FIND_BUTTON);
	m_btnFind.SetTransparent(TRUE, hDlgBgDC);
	m_btnFind.SetBgPic(_T("添加好友_默认状态.png"), _T("添加好友_鼠标状态.png"), _T("添加好友_鼠标状态.png"), _T("添加好友_鼠标状态.png"));
	//m_btnFind.SetIconPic(_T("find.png"));
	m_btnFind.SubclassWindow(GetDlgItem(ID_BTN_FIND));
	m_btnFind.MoveWindow(rcClient.right - 60, rcClient.bottom - 29, 50, 20, FALSE);
	m_btnFind.SetWindowText(_T("     查找"));
	m_btnFind.SetToolTipText(_T("查找联系人和群组"));


	//更新用户头像
	//TODO :用户头像是48*48大小的，这个地方的图片太大了
	{
		m_picHead.SetTransparent(TRUE, hDlgBgDC);
		m_picHead.SetShowCursor(TRUE);
		//m_picHead.SetBgPic(_T("HeadCtrl\\Padding4Normal.png"), _T("HeadCtrl\\Padding4Hot.png"), _T("HeadCtrl\\Padding4Hot.png"));
		m_picHead.SetBgPic(_T("HeadCtrl\\0.png"), _T("HeadCtrl\\0.png"), _T("HeadCtrl\\0.png"));
		m_picHead.SubclassWindow(GetDlgItem(ID_PIC_HEAD));
		m_picHead.MoveWindow(10, 30, 64, 64, FALSE);
		m_picHead.SetToolTipText(_T("点击修改个人资料"));
	}

	//主界面用户昵称
	{
		m_staNickName.SetTransparent(TRUE, hDlgBgDC);
		m_staNickName.SetTextColor(RGB(255, 255, 255));
		m_staNickName.SubclassWindow(GetDlgItem(ID_STATIC_NICK_NAME));
		m_staNickName.SetWindowText(_T("用户昵称"));
		HFONT hFontNickName = CGDIFactory::GetBoldFont(20);
		m_staNickName.SetFont(hFontNickName);
		m_staNickName.MoveWindow(80, 60, rcClient.Width() - 102, 20, TRUE);
	}


	//主界面，用户签名
	{
		m_btnSign.SetButtonType(SKIN_ICON_BUTTON);
		m_btnSign.SetTransparent(TRUE, hDlgBgDC);
		m_btnSign.SetBgPic(NULL, _T("allbtn_highlight2.png"), _T("allbtn_down2.png"), NULL, CRect(3, 0, 3, 0));
		m_btnSign.SetTextAlign(DT_LEFT);
		m_btnSign.SubclassWindow(GetDlgItem(ID_BTN_SIGN));
		m_btnSign.MoveWindow(80, 60, rcClient.Width() - 102, 22, TRUE);
		m_btnSign.SetWindowText(_T("这家伙很懒,什么都没有留下1。"));
		m_btnSign.SetToolTipText(_T("这家伙很懒,什么都没有留下1。"));
	}

	HFONT hFontSign = CGDIFactory::GetFont(19);
	m_btnSign.SetFont(hFontSign);
	m_edtSign.SetBgNormalPic(_T("SignEditBg.png"), CRect(1, 1, 1, 1));
	m_edtSign.SubclassWindow(GetDlgItem(ID_EDIT_SIGN));
	m_edtSign.MoveWindow(80, 60, rcClient.Width() - 102, 22, TRUE);
	m_edtSign.ShowWindow(SW_HIDE);
	m_edtSign.SetFont(hFontSign);

	//Load Menu
	{
		m_SkinMenu.SetBgPic(_T("Menu\\menu_left_bg.png"), _T("Menu\\menu_right_bg.png"));
	m_SkinMenu.SetSelectedPic(_T("Menu\\menu_selected.png"));
	m_SkinMenu.SetSepartorPic(_T("Menu\\menu_separtor.png"));
	m_SkinMenu.SetArrowPic(_T("Menu\\menu_arrow.png"));
	m_SkinMenu.SetCheckPic(_T("Menu\\menu_check.png"));
	m_SkinMenu.SetTextColor(RGB(0, 20, 35));
	m_SkinMenu.SetSelTextColor(RGB(254, 254, 254));
	m_SkinMenu.LoadMenu(ID_MENU_MAIN_PANEL);

	DWORD dwMenuIDs[] = { ID_MENU_IM_ONLINE, ID_MENU_QME, ID_MENU_AWAY,
		ID_MENU_BUSY, ID_MENU_MUTE, ID_MENU_INVISIBLE, ID_MENU_IM_OFFLINE,
		ID_MENU_LOCK, ID_MENU_GROUP_HOME_PAGE };
	CString strFileNames[] = { _T("Status\\imonline.png"), _T("Status\\Qme.png"),
		_T("Status\\away.png"), _T("Status\\busy.png"), _T("Status\\mute.png"),
		_T("Status\\invisible.png"), _T("Status\\imoffline.png"), _T("lock20.png"),
		_T("groupmainpage.png") };

	CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(MAIN_PANEL_STATUS_SUBMENU_INDEX);
	for (int i = 0; i < 9; i++)
	{
		PopupMenu.SetIcon(dwMenuIDs[i], FALSE, strFileNames[i], strFileNames[i]);
	}

	PopupMenu = m_SkinMenu.GetSubMenu(MAIN_PANEL_TRAYICON_SUBMENU2_INDEX);
	for (int i = 0; i < 9; i++)
	{
		PopupMenu.SetIcon(dwMenuIDs[i], FALSE, strFileNames[i], strFileNames[i]);
	}

	PopupMenu = m_SkinMenu.GetSubMenu(3);
	PopupMenu.SetIcon(0, TRUE, _T("modehead.png"), _T("modehead.png"));
	PopupMenu = m_SkinMenu.GetSubMenu(4);
	PopupMenu.SetIcon(0, TRUE, _T("modehead.png"), _T("modehead.png"));
	PopupMenu = m_SkinMenu.GetSubMenu(5);
	PopupMenu.SetIcon(0, TRUE, _T("modehead.png"), _T("modehead.png"));
	PopupMenu = m_SkinMenu.GetSubMenu(6);
	PopupMenu.SetIcon(0, TRUE, _T("modehead.png"), _T("modehead.png"));
	//托盘区右键菜单
	{
		m_TrayMenuNormal.LoadMenu(ID_MENU_TRAY_NORMAL);
		m_TrayMenuLock.LoadMenu(ID_MENU_TRAY_LOCK);
	}

	//好友列表部分的相关右键菜单
	{
		m_BuddyEmptyAreaMenu.LoadMenu(ID_MENU_BUDDY_EMPTY_POP);
		m_BuddyGroupSelectMenu.LoadMenu(ID_MENU_BUDDY_GROUP_SELECT);
		m_BuddySelectedMenu.LoadMenu(IDR_MENU_BUDDY_SELECT);
	}

	//群组列表部分相关的右键菜单
	{
		m_GroupSelectedMenu.LoadMenu(ID_MENU_GROUP_SELECT);
		m_GroupEmptyAreaMenu.LoadMenu(ID_MENU_GROUP_LIST_EMTPY_AREA_RIGHT);
		m_GroupGroupSelectMenu.LoadMenu(ID_MENU_GROUP_GROUP_SELECTED);
	}
}


	//主界面底部按钮
	{
		m_MainBoardMenu.LoadMenu(IDR_MENU_MAIN_BOARD);
	}
	//PopupMenu = m_SkinMenu.GetSubMenu(0);

	//m_btnStatus.SetButtonType(SKIN_MENU_BUTTON);
	//m_btnStatus.SetTransparent(TRUE, hDlgBgDC);
	//m_btnStatus.SetBgPic(NULL, _T("menubtn_highlight.png"), _T("menubtn_highlight.png"), NULL);
	//m_btnStatus.SetArrowPic(_T("MainPanel_FolderNode_expandTexture.png"));
	//m_btnStatus.SetIconPic(_T("Status\\imonline.png"));
	//m_btnStatus.SubclassWindow(GetDlgItem(ID_BTN_STATUS));
	//m_btnStatus.MoveWindow(54, 30, 35, 22, TRUE);
	//m_btnStatus.SetMenu(PopupMenu.m_hMenu);

	m_edtSearch.SetBgNormalPic(_T("SearchBar\\bg.png"), CRect(0,0,0,0));
	m_edtSearch.SetIconPic(_T("SearchBar\\main_search_normal.png"));
	m_edtSearch.SetDefaultText(_T("搜索：联系人、讨论组、群"));
	m_edtSearch.SubclassWindow(GetDlgItem(ID_EDIT_SEARCH));
	m_edtSearch.MoveWindow(0, 126, rcClient.Width(), 28, FALSE);

	//邮件按钮
	{
		m_btnMail.SetButtonType(SKIN_ICON_BUTTON);
		m_btnMail.SetTransparent(TRUE, hDlgBgDC);
		m_btnMail.SetBgPic(NULL, _T("allbtn_highlight.png"), _T("allbtn_down.png"), NULL);
		m_btnMail.SetIconPic(_T("MidToolBar\\aio_quickbar_msglog.png"));
		m_btnMail.SubclassWindow(GetDlgItem(IDC_BTN_MAIL));
		m_btnMail.MoveWindow(10, 100, 22, 20, TRUE);
	}


    CIniFile iniFile;
    CString strIniFile;
    strIniFile.Format(_T("%sconfig\\flamingo.ini"), g_szHomePath);
    TCHAR szMyWebsiteLinkTooltip[32];
    memset(szMyWebsiteLinkTooltip, 0, sizeof(szMyWebsiteLinkTooltip));
    iniFile.ReadString(_T("ui"), _T("mywebsitetooltiptext"), UI_DEFAULT_MYWEBSITE_TOOLTIP, szMyWebsiteLinkTooltip, ARRAYSIZE(szMyWebsiteLinkTooltip), strIniFile);
    m_btnMail.SetToolTipText(szMyWebsiteLinkTooltip);
	
	//m_btnMsgBox.SetButtonType(SKIN_ICON_BUTTON);
	//m_btnMsgBox.SetTransparent(TRUE, hDlgBgDC);
	//m_btnMsgBox.SetBgPic(NULL, _T("allbtn_highlight.png"), _T("allbtn_down.png"), NULL);
	//m_btnMsgBox.SetIconPic(_T("MsgBox.png"));
	//m_btnMsgBox.SubclassWindow(GetDlgItem(ID_BTN_MSG_BOX));
	//m_btnMsgBox.MoveWindow(rcClient.Width()-48, 80, 22, 20, TRUE);
	//m_btnMsgBox.SetToolTipText(_T("打开消息盒子"));


	m_picLogining.SetTransparent(TRUE, hDlgBgDC);
	m_picLogining.SubclassWindow(GetDlgItem(ID_PIC_LOGINING));
	m_picLogining.MoveWindow((rcClient.Width() - 220) / 2, 76, 220, 150, TRUE);

	m_staUTalkNum.SetTransparent(TRUE, hDlgBgDC);
	m_staUTalkNum.SubclassWindow(GetDlgItem(ID_STATIC_UTalkNUM));
	m_staUTalkNum.MoveWindow(rcClient.left, 226, rcClient.Width(), 14, FALSE);

	m_staLogining.SetTransparent(TRUE, hDlgBgDC);
	m_staLogining.SubclassWindow(GetDlgItem(ID_STATIC_LOGINING));
	m_staLogining.MoveWindow(rcClient.left, 240, rcClient.Width(), 16, FALSE);

	m_btnCancel.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnCancel.SetTransparent(TRUE, hDlgBgDC);
	m_btnCancel.SetBgPic(_T("Button\\login_btn_normal.png"), _T("Button\\login_btn_highlight.png"),
		_T("Button\\login_btn_down.png"), _T("Button\\login_btn_focus.png"));
	//m_btnCancel.SetRound(4, 4);
	m_btnCancel.SubclassWindow(GetDlgItem(ID_BTN_CANCEL));
	m_btnCancel.MoveWindow((rcClient.Width() - 86) / 2, 304, 86, 30, TRUE);
	
	m_btnUnlock.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnUnlock.SetTransparent(TRUE, hDlgBgDC);
	m_btnUnlock.SetBgPic(_T("Button\\login_btn_normal.png"), _T("Button\\login_btn_highlight.png"),_T("Button\\login_btn_down.png"), _T("Button\\login_btn_focus.png"));
	m_btnUnlock.SubclassWindow(GetDlgItem(IDC_UNLOCK));
	m_btnUnlock.MoveWindow((rcClient.Width() - 86) / 2, 304, 86, 30, TRUE);

	//InitTopToolBar();		// 初始化Top工具栏
	//InitBottomToolBar();	// 初始化Bottom工具栏
	InitTabCtrl();			// 初始化Tab栏
	InitBuddyListCtrl();	// 初始化好友列表控件
	InitGroupListCtrl();	// 初始化群列表控件
	InitRecentListCtrl();	// 初始化最近联系人列表控件


	return TRUE;
}

/**
 * @brief 反初始化UI
 * 
 */
void CMainDlg::UninitUI()
{
	if (m_picHead.IsWindow())
	{
		m_picHead.DestroyWindow();
	}

	if (m_staNickName.IsWindow())
	{
		m_staNickName.DestroyWindow();
	}

}

// 
/**
 * @brief 显示指定面板函数(bShow：TRUE表示显示主面板，FALSE表示显示登录面板)
 * 
 * TODO: bool参数待优化
 * @param bShow 
 */
void CMainDlg::ShowPanel(BOOL bShow)
{
	int nShow = bShow ? SW_HIDE : SW_SHOW;
	m_picLogining.ShowWindow(nShow);
	m_staUTalkNum.ShowWindow(nShow);
	m_staLogining.ShowWindow(nShow);
	m_btnCancel.ShowWindow(nShow);


    nShow = bShow ? SW_SHOW : SW_HIDE;
	m_btnMail.ShowWindow(nShow);

	m_btnMainMenu.ShowWindow(nShow);

	m_btnMultiChat.ShowWindow(nShow);

	//m_btnSystemSet.ShowWindow(nShow);
	//m_btnMsgMgr.ShowWindow(nShow);
	//m_btnSafe.ShowWindow(nShow);
	m_btnFind.ShowWindow(nShow);
	//m_btnStatus.ShowWindow(nShow);
	m_edtSearch.ShowWindow(nShow);
	m_picHead.ShowWindow(nShow);
	m_staNickName.ShowWindow(nShow);
	m_btnSign.ShowWindow(nShow);
	//m_tbTop.ShowWindow(nShow);
	//m_tbBottom.ShowWindow(nShow);
	//m_btnMsgBox.ShowWindow(nShow);
	//m_btnChangeExterior.ShowWindow(nShow);
	//m_btnAppMgr.ShowWindow(nShow);;
	m_TabCtrl.ShowWindow(nShow);
	m_BuddyListCtrl.ShowWindow(nShow);


	m_btnUnlock.ShowWindow(SW_HIDE);
	
	RECT rtWindow;
	HRGN hTemp = NULL;
	if (bShow)	// 显示主面板
	{
		m_SkinDlg.SetBgPic(_T("main_panel_bg.png"), CRect(2, 135, 2, 67));
		m_picLogining.SetBitmap(NULL, TRUE);
		//StatusMenuBtn_SetIconPic(m_btnStatus, m_FMGClient.GetStatus());

		//::SetTimer(m_hWnd, ID_TIMER_DETECT_CLOSE_TO_SIDE, 500, NULL);
		//设置热点区域
		m_picHead.GetClientRect(&rtWindow);
		m_hHotRgn = ::CreateRectRgnIndirect(&rtWindow);

		m_edtSearch.GetClientRect(&rtWindow);
		hTemp = ::CreateRectRgnIndirect(&rtWindow);
		::CombineRgn(m_hHotRgn, m_hHotRgn, hTemp, RGN_AND);
		::DeleteObject(hTemp);
		
		m_staNickName.GetClientRect(&rtWindow);
		hTemp = ::CreateRectRgnIndirect(&rtWindow);
		::CombineRgn(m_hHotRgn, m_hHotRgn, hTemp, RGN_AND);
		::DeleteObject(hTemp);

		m_btnSign.GetClientRect(&rtWindow);
		hTemp = ::CreateRectRgnIndirect(&rtWindow);
		::CombineRgn(m_hHotRgn, m_hHotRgn, hTemp, RGN_AND);
		::DeleteObject(hTemp);

		m_TabCtrl.GetClientRect(&rtWindow);
		hTemp = ::CreateRectRgnIndirect(&rtWindow);
		::CombineRgn(m_hHotRgn, m_hHotRgn, hTemp, RGN_AND);
		::DeleteObject(hTemp);

		m_BuddyListCtrl.GetClientRect(&rtWindow);
		hTemp = ::CreateRectRgnIndirect(&rtWindow);
		::CombineRgn(m_hHotRgn, m_hHotRgn, hTemp, RGN_AND);
		::DeleteObject(hTemp);

		m_GroupListCtrl.GetClientRect(&rtWindow);
		hTemp = ::CreateRectRgnIndirect(&rtWindow);
		::CombineRgn(m_hHotRgn, m_hHotRgn, hTemp, RGN_AND);
		::DeleteObject(hTemp);

		m_RecentListCtrl.GetClientRect(&rtWindow);
		hTemp = ::CreateRectRgnIndirect(&rtWindow);
		::CombineRgn(m_hHotRgn, m_hHotRgn, hTemp, RGN_AND);
		::DeleteObject(hTemp);

		m_btnFind.GetClientRect(&rtWindow);
		hTemp = ::CreateRectRgnIndirect(&rtWindow);
		::CombineRgn(m_hHotRgn, m_hHotRgn, hTemp, RGN_AND);
		::DeleteObject(hTemp);
		
	}
	else		// 显示登录面板
	{
		m_SkinDlg.SetBgPic(_T("LoginPanel_window_windowBkg.png"), CRect(4, 65, 4, 4));

		WString strFileName = Hootina::CPath::GetAppPath() + _T("Image\\Logining.png");
		m_picLogining.SetBitmap(strFileName.c_str(), TRUE);

		m_staUTalkNum.SetWindowText(m_stAccountInfo.szUser);
		m_staLogining.SetWindowText(_T("正在登录"));
		m_staLogining.SetFocus();

		m_btnCancel.GetClientRect(&rtWindow);
		m_hHotRgn = ::CreateRectRgnIndirect(&rtWindow);
	}

	InvalidateRect(NULL, TRUE);
	
	m_SkinDlg.SetHotRegion(m_hHotRgn);

	ModifyStyleEx(WS_EX_APPWINDOW, 0);

	m_bPanelLocked = FALSE;
}

/**
 * @brief 锁定界面操作
 * 
 */
void CMainDlg::ShowLockPanel()
{
	//显示下列窗口
	m_SkinDlg.SetBgPic(_T("LoginPanel_window_windowBkg.png"), CRect(4, 65, 4, 4));
	WString strFileName = Hootina::CPath::GetAppPath() + _T("Image\\Logining.png");
	m_picLogining.SetBitmap(strFileName.c_str(), TRUE);
	m_picLogining.ShowWindow(SW_SHOW);
	m_btnUnlock.ShowWindow(SW_SHOW);
	m_staUTalkNum.ShowWindow(SW_SHOW);
	m_staLogining.ShowWindow(SW_SHOW);

	m_staUTalkNum.SetWindowText(m_stAccountInfo.szUser);
	m_staLogining.SetWindowText(_T("已锁定"));
	
	
	//隐藏下列窗口
	m_btnCancel.ShowWindow(SW_HIDE);
	m_btnMainMenu.ShowWindow(SW_HIDE);
	//m_btnSystemSet.ShowWindow(SW_HIDE);
	m_btnFind.ShowWindow(SW_HIDE);
	m_edtSearch.ShowWindow(SW_HIDE);
	m_picHead.ShowWindow(SW_HIDE);
	m_staNickName.ShowWindow(SW_HIDE);
	m_btnSign.ShowWindow(SW_HIDE);
	m_edtSign.ShowWindow(SW_HIDE);
    m_btnMultiChat.ShowWindow(SW_HIDE);

    m_btnMail.ShowWindow(SW_HIDE);

	m_TabCtrl.ShowWindow(SW_HIDE);
	m_RecentListCtrl.ShowWindow(SW_HIDE);
	m_BuddyListCtrl.ShowWindow(SW_HIDE);
	m_GroupListCtrl.ShowWindow(SW_HIDE);

	if (m_pFindFriendDlg != NULL && m_pFindFriendDlg->IsWindow())
	{
		m_pFindFriendDlg->ShowWindow(SW_HIDE);
	}

	if(m_MsgTipDlg.IsWindow())
	{
		m_MsgTipDlg.ShowWindow(SW_HIDE);
	}


	//隐藏所有的群聊窗口
	auto  iter2 = m_mapGroupChatDlg.begin();
	for(;iter2!=m_mapGroupChatDlg.end(); ++iter2)
	{
		if (iter2->second != NULL && iter2->second->IsWindow())
		{
			iter2->second->ShowWindow(SW_HIDE);
		}
	}

	//隐藏所有的临时会话窗口



	InvalidateRect(NULL, TRUE);

	m_bPanelLocked = TRUE;
}


//开始登录
/**
 * @brief 处理开始登录
 * 
 * @param bAutoLogin 
 * @return true 
 * @return false 
 */
bool CMainDlg::StartLogin(BOOL bAutoLogin/* = FALSE*/)
{
	//m_FMGClient.SetCallBackWnd(m_hWnd);
	if (bAutoLogin)
	{
//调试版就不要升级啦
/*#ifndef _DEBUG
        Updater updater(&m_FMGClient.m_FileTask);
		//updater.m_lpProcotol = &m_FMGClient.m_IUProtocol;

		//需要升级
		if(CheckOnlyOneInstance() && updater.IsNeedUpdate())
		{
			if(IDYES == ::MessageBox(m_hWnd, _T("检测到新版本，是否现在就升级？"), _T("新版本提示"), MB_YESNO|MB_ICONQUESTION|MB_TOPMOST))
			{
				CUpdateDlg updateDlg;
				updateDlg.m_pFMGClient =  &m_FMGClient;
				updateDlg.m_aryFileInfo = updater.m_aryUpdateFileList;
				//updateDlg.m_lpProtocol = &m_FMGClient.m_IUProtocol;
				updateDlg.DoModal(m_hWnd, NULL);
			}
		}
#endif*/
		
		BOOL bRet = m_LoginAccountList.GetLastLoginAccountInfo(&m_stAccountInfo);
		if (!bRet)
		{
			return true;
		}

	}
	else
	{
		ShowWindow(SW_HIDE);
		LoadAppIcon(GetStatus());
		m_TrayIcon.AddIcon(m_hWnd, WM_TRAYICON_NOTIFY, 1, m_hAppIcon, _T("TinyIM未登录"));

		//ShowWindow(SW_HIDE);
		//ShowWindow(SW_SHOW);

		//m_LoginDlg.m_lpFMGClient = &m_FMGClient;
		m_LoginDlg.m_pLoginAccountList = &m_LoginAccountList;
		m_LoginDlg.SetDefaultAccount(m_userMgr.m_UserInfo.m_strAccount.c_str());
		m_LoginDlg.SetDefaultPassword(m_userMgr.m_UserInfo.m_strPassword.c_str());
		m_LoginDlg.m_hMainDlg = m_hWnd;
		if (m_LoginDlg.DoModal(g_hwndOwner) != IDOK)	// 显示登录对话框
		{
			CloseDialog(IDOK);
			return false;
		}
		m_LoginDlg.GetLoginAccountInfo(&m_stAccountInfo);
	}

    m_nMainPanelStatus = MAINPANEL_STATUS_LOGINING;
    
    ShowPanel(FALSE);		// 显示登录面板
	ShowWindow(SW_SHOW);

	LoadLoginIcon();

    return true;
}


/**
 * @brief 响应系统托盘区图标消息
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnTrayIconNotify(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// wParam为uID, lParam为鼠标消息
	m_TrayIcon.OnTrayIconNotify(wParam, lParam);


	UINT uID = (UINT)wParam;
	UINT uIconMsg = (UINT)lParam;

	switch (uIconMsg)
	{
	case WM_LBUTTONUP:
	{
		OnTrayIcon_LButtunUp();
	}break;
	case WM_RBUTTONUP:
	{
		OnTrayIcon_RButtunUp();
	}break;
	case WM_MOUSEHOVER:
	{
		OnTrayIcon_MouseHover();
	}break;
	case WM_MOUSELEAVE:
	{
		OnTrayIcon_MouseLeave();
	}
		break;
	}
	return 0;
}


/**
 * @brief 响应显示主列表,菜单
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnMenu_ShowMainPanel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (m_LoginDlg.IsWindow())
	{
		m_LoginDlg.ShowWindow(SW_SHOW);
		m_LoginDlg.SetFocus();
	}
	else if (IsWindow())
	{
		ShowWindow(SW_SHOW);
		SetFocus();
	}
}

/**
 * @brief 响应锁定面板按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnMenu_LockPanel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	ShowLockPanel();
}

/**
 * @brief 响应退出菜单
//响应托盘区退出
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnMenu_Exit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (m_LoginDlg.IsWindow())
	{
		m_LoginDlg.PostMessage(WM_CLOSE);
	}
	else if (IsWindow())
	{
		//PostMessage(WM_CLOSE);
		CloseDialog(IDOK);
	}
}

/**
 * @brief 响应静音按钮菜单
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnMenu_Mute(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    m_userCfg.EnableMute(!m_userCfg.IsEnableMute());
}

/**
 * @brief 点击更改个性签名，编辑签名按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnBtn_Sign(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strText;
	m_btnSign.GetWindowText(strText);
	m_edtSign.SetWindowText(strText);

	m_btnSign.ShowWindow(SW_HIDE);
	m_edtSign.ShowWindow(SW_SHOW);

	m_edtSign.SetSel(0, -1);
	m_edtSign.SetFocus();
}

/**
 * @brief 解锁面板按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnBtn_UnlockPanel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	ShowPanel(TRUE);
	//恢复原来的菜单
}

/**
 * @brief 主菜单按钮压下
 * 
 * @param uNotifyCode 
 * @param nId 
 * @param wndCtl 
 */
void CMainDlg::OnBtn_MainMenu(UINT uNotifyCode, int nId, CWindow wndCtl)
{
	CSkinMenu PopupMenu;
	CRect rcItem;
	GetClientRect(&rcItem);

	PopupMenu = m_MainBoardMenu.GetSubMenu(0);
	if (PopupMenu.IsMenu())
	{
		m_SkinDlg.ClientToScreen(&rcItem);
		PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, 
			rcItem.right-rcItem.Width(), rcItem.bottom - 100, m_hWnd, &rcItem);
	}
}

/**
 * @brief 弹出添加好友对话框,点击添加好友按钮后的操作
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnBtn_ShowAddFriendDlg(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	// 显示查找好友对话框
	if (m_pFindFriendDlg == NULL)
	{
		return;
	}

	if(m_pFindFriendDlg->IsWindow())
	{
		m_pFindFriendDlg->ShowWindow(SW_SHOWNORMAL);	
		m_pFindFriendDlg->SetFocus();
	}
	else
	{
		m_pFindFriendDlg->m_netProto = m_netProto;
		if (m_pFindFriendDlg->Create(m_hWnd, NULL) == NULL)
		{
			return;
		}
		m_pFindFriendDlg->ShowWindow(SW_SHOWNORMAL);	
	}
}


//
/**
 * @brief 响应显示系统设置对话框按钮(UI)
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnBtn_ShowSystemSettingDlg(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CSystemSettingDlg systemSettingDlg;
	systemSettingDlg.m_netProto = m_netProto;
	systemSettingDlg.DoModal(m_hWnd, NULL);
}



/**
 * @brief 响应打开邮件
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnBtn_OpenMail(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    CIniFile iniFile;
    CString strIniFile;
    strIniFile.Format(_T("%sconfig\\flamingo.ini"), g_szHomePath);
    TCHAR szMyWebsiteLink[512];
    memset(szMyWebsiteLink, 0, sizeof(szMyWebsiteLink));
    iniFile.ReadString(_T("ui"), _T("mywebsitelink"), UI_DEFAULT_MYWEBSITE, szMyWebsiteLink, ARRAYSIZE(szMyWebsiteLink), strIniFile);

    ::ShellExecute(NULL, _T("open"), szMyWebsiteLink, NULL, NULL, SW_SHOWNORMAL);
}


/**
 * @brief 响应修改密码
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnBtn_ModifyPassword(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_ModifyPasswordDlg.m_netProto = m_netProto;
	if (!m_ModifyPasswordDlg.IsWindow())
	{
		m_ModifyPasswordDlg.Create(NULL);
	}
	
	m_ModifyPasswordDlg.ShowWindow(SW_SHOW);
	::SetForegroundWindow(m_ModifyPasswordDlg.m_hWnd);
	m_ModifyPasswordDlg.SetFocus();
}


/**
 * @brief 此函数建议优化掉
 * TODO:
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnTabCtrlDropDown(LPNMHDR pnmh)
{
	CSkinMenu PopupMenu;
	CRect rcItem;
	int nCurSel = 0;

	nCurSel = m_TabCtrl.GetCurSel();

	switch (nCurSel)
	{
	case 0:
	{
		MessageBox(_T("最近联系列表"), _T("TinyIM"));
	}break;

	case 1:
	{	
		MessageBox(_T("好友列表"), _T("TinyIM"));
	}break;

	case 2:
	{
		MessageBox(_T("群组列表"), _T("TinyIM"));
	}break;
	default:
	{
		MessageBox(_T("未知列表"), _T("TinyIM"));
	}break;
	}
	return 0;
}

/**
 * @brief 响应好友列表双击事件
 * 
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnBuddyListDblClk(LPNMHDR pnmh)
{
	int nTeamIndex = 0;
	int nIndex = 0;
	m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nIndex);
	if (nTeamIndex >= 0 && nIndex >= 0)
	{
		std::string strFriendId = m_BuddyListCtrl.GetBuddyItemUserId(nTeamIndex, nIndex);
		if (!strFriendId.empty())
		{
			ShowBuddyChatDlg(strFriendId);
		}
	}
	return 0;
}


/**
 * @brief 响应好友列表右键单击
 * 
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnBuddyListRButtonUp(LPNMHDR pnmh)
{
	CPoint pt;
	GetCursorPos(&pt);

	//::SetForegroundWindow(m_hWnd);
    BLNMHDREx* hdr = (BLNMHDREx*)pnmh;
	CSkinMenu PopupMenu;
	//选中好友的状态
	if(hdr->nPostionFlag == POSITION_ON_ITEM)
	{
		PopupMenu = m_BuddySelectedMenu.GetSubMenu(0);
		if (GetFriendList())
		{
			PopupMenu.EnableMenuItem(IDM_MOVE_ITEM, MF_ENABLED|MF_BYCOMMAND);
			InsertTeamMenuItem(PopupMenu);
		}
		else
		{
			//TODO: 画法里面不支持MF_BYCOMMAND禁用菜单
			PopupMenu.EnableMenuItem(8, MF_GRAYED|MF_BYPOSITION);
		}
	}
	//选中好友分组的状态
	else if(hdr->nPostionFlag == POSITION_ON_TEAM)
	{
		PopupMenu = m_BuddyGroupSelectMenu.GetSubMenu(0);
	}
	//好友列表的空白部分
	else if(hdr->nPostionFlag == POSITION_ON_BLANK)
	{
		PopupMenu = m_BuddyEmptyAreaMenu.GetSubMenu(0);
	}

	PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);

	return 1;
}

/**
 * @brief 群组聊天双击
 * 
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnGroupListDblClk(LPNMHDR pnmh)
{
	int nTeamIndex = 0 ;
	int nIndex = 0 ;
	m_GroupListCtrl.GetCurSelIndex(nTeamIndex, nIndex);

	//if (nTeamIndex != -1 && nIndex != -1)
	//{
	//	UINT nGroupCode = m_GroupListCtrl.GetBuddyItemID(nTeamIndex, nIndex);
	//	std::string strGroupId = m_GroupListCtrl.GetBuddyItemUserId(nTeamIndex, nIndex);
	//	ShowGroupChatDlg(strGroupId, TRUE);
	//}	
	return 0;
}


/**
 * @brief 群组聊天右击，显示群组聊天对应的弹出菜单
 * 
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnGroupListRButtonUp(LPNMHDR pnmh)
{
	CPoint pt;
	GetCursorPos(&pt);

	//::SetForegroundWindow(m_hWnd);
    BLNMHDREx* hdr = (BLNMHDREx*)pnmh;
	if(hdr->nPostionFlag == POSITION_ON_ITEM)
	{
		CSkinMenu PopupMenu = m_GroupSelectedMenu.GetSubMenu(0);
		PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);
	}
	else if(hdr->nPostionFlag==POSITION_ON_TEAM)
	{
		CSkinMenu PopupMenu = m_GroupGroupSelectMenu.GetSubMenu(0);
		PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);
	}
	else if(hdr->nPostionFlag == POSITION_ON_BLANK)
	{
		CSkinMenu PopupMenu = m_GroupEmptyAreaMenu.GetSubMenu(0);
		PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);
	}


	return (LRESULT)1;
}


/**
 * @brief 最近聊天列表双击
 * TODO: 待修改
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnRecentListDblClk(LPNMHDR pnmh)
{
	int nTeamIndex = 0;
	int nIndex = 0;
	m_RecentListCtrl.GetCurSelIndex(nTeamIndex, nIndex);

	if (nTeamIndex != -1 && nIndex != -1)
	{
		//UINT nUTalkUin = m_RecentListCtrl.GetBuddyItemID(nTeamIndex, nIndex);
		//if(IsGroupTarget(nUTalkUin))
		//{
		//	SendMessage(WM_SHOW_GROUP_CHAT_DLG, (WPARAM)nUTalkUin, 0);
		//}
		//else
		//{
		//	if(m_userMgr.IsFriend(nUTalkUin))
		//	{
		//		WString strNickName = m_RecentListCtrl.GetBuddyItemNickName(nTeamIndex, nIndex);
		//		CString strInfo;
		//		strInfo.Format(_T("%s已经不是您的好友，请先加对方为好友后再聊天。"), strNickName.c_str());
		//		::MessageBox(m_hWnd, strInfo, g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		//		return 1;
		//	}
		//	
		//	//SendMessage(WM_SHOW_BUDDY_CHAT_DLG, 0, nUTalkUin);
		//}
	}	
	return 0;
}


/**
 * @brief 最近聊天列表右击(UI)
 * 
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnRecentListRButtonUp(LPNMHDR pnmh)
{
	CPoint pt;
	GetCursorPos(&pt);

	CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(MAIN_PANEL_RECENTLIST_CONTEXT_SUBMENU_INDEX);
	PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);

	return (LRESULT)1;
}

/**
 * @brief 响应TAB页面选择切换，即好友，最近聊天，群聊切换
 * 
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnTabCtrlSelChange(LPNMHDR pnmh)
{
	int nCurSel = m_TabCtrl.GetCurSel();
	switch (nCurSel)
	{
	case 0:
	{
		m_RecentListCtrl.ShowWindow(SW_SHOW);
		m_BuddyListCtrl.ShowWindow(SW_HIDE);
		m_GroupListCtrl.ShowWindow(SW_HIDE);
		m_RecentListCtrl.SetBuddyTeamExpand(0, TRUE);
		m_RecentListCtrl.SetFocus();
	}break;

	case 1:
	{
		m_RecentListCtrl.ShowWindow(SW_HIDE);
		m_BuddyListCtrl.ShowWindow(SW_SHOW);
		m_GroupListCtrl.ShowWindow(SW_HIDE);
		m_BuddyListCtrl.SetFocus();
	}break;
	case 2:
	{	
		m_RecentListCtrl.ShowWindow(SW_HIDE);
		m_BuddyListCtrl.ShowWindow(SW_HIDE);
		m_GroupListCtrl.ShowWindow(SW_SHOW);
		m_GroupListCtrl.SetFocus();
	}break;
	}

	m_nCurSelIndexInMainTab = nCurSel;

	return 0;
}


/**
 * @brief 响应好友列表刷新,UI
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnRefreshBuddyList(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	GetFriendList();
}


/**
 * @brief 只显示在线的好友(UI)
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnShowOnlineBuddyOnly(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	::SendMessage(m_hWnd, FMG_MSG_UPDATE_BUDDY_LIST, 0, 0);
}

/**
 * @brief 主界面关于菜单
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnMainMenu_About(UINT uNotifyCode, int nID, CWindow wndCtl)
{

}

/**
 * @brief 响应“取消”按钮(取消登录)
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CloseDialog(IDCANCEL);
	CString strMainExe = g_szHomePath;
	strMainExe.Format(_T("%TinyIM.exe"), g_szHomePath);
	if((int)::ShellExecute(NULL, NULL, strMainExe.GetString(), NULL, NULL, SW_SHOW) <= 32)
	{
		::MessageBox(NULL, _T("显示登录对话框失败！"), g_strAppTitle.c_str(), MB_OK|MB_ICONERROR);
	}
}


/**
 * @brief “用户头像”控件，显示用户信息
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnPic_Clicked_Head(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_LogonUserInfoDlg.m_netProto = m_netProto;
	if (!m_LogonUserInfoDlg.IsWindow())
	{
		m_LogonUserInfoDlg.Create(NULL);
	}
	
	m_LogonUserInfoDlg.ShowWindow(SW_SHOW);
	::SetForegroundWindow(m_LogonUserInfoDlg.m_hWnd);
}

/**
 * @brief “个性签名”编辑文本框
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnEdit_Sign_KillFocus(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strOldText;
	CString strNewText;
	m_btnSign.GetWindowText(strOldText);
	m_edtSign.GetWindowText(strNewText);

	strNewText.Trim();
	if(strNewText.GetLength() > MAX_SIGN_LENGTH)
	{
		::MessageBox(m_hWnd, _T("个性签名不能超过127个字符！"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}
	
	BOOL bModify = TRUE;
	if(IsOffLine())
	{
		::MessageBox(m_hWnd, _T("您当前处于离线状态，无法修改个性签名，请上线后重试。"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
	}
	else
	{
		if(!strOldText.IsEmpty() && strNewText.IsEmpty())
		{
			if(::MessageBox(m_hWnd, _T("确定要更改个性签名吗？"), g_strAppTitle.c_str(), MB_YESNO|MB_ICONQUESTION) != IDYES)
			{
				bModify = FALSE;
			}
		}

		if(bModify && strOldText!=strNewText)
		{
			m_btnSign.SetWindowText(strNewText);
		}
	}

	if (!bModify)
	{
		m_btnSign.SetWindowText(strOldText);
	}
	
	m_edtSign.ShowWindow(SW_HIDE);
	m_btnSign.ShowWindow(SW_SHOW);
}


/**
 * @brief 响应菜单的修改用户在线状态
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnMenu_Status(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	E_UI_ONLINE_STATUS nNewStatus = GetStatusFromMenuID(nID);
	//如果需要更改的目标状态与当前状态一样，不处理
	if (nNewStatus == GetStatus())
	{
		return;
	}

    if (nNewStatus == E_UI_ONLINE_STATUS::STATUS_OFFLINE)
    {
        //重连标志清零
        m_nMainPanelStatus = MAINPANEL_STATUS_USERGOOFFLINE;
    }
	else if (nNewStatus == E_UI_ONLINE_STATUS::STATUS_ONLINE)
	{	
	}
}

/**
 * @brief 
 * 
 * @param uNotifyCode 增加好友分组
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnBuddyListAddTeam(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if(m_userMgr.m_BuddyList.GetBuddyTeamCount() > MAX_TEAM_COUNT)
	{
		::MessageBox(m_hWnd, _T("分组数量不能超过20个。"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}
	
	CTeamDlg newTeamDlg;
	newTeamDlg.SetType(TEAM_OPERATION_ADD_TEAM);
	newTeamDlg.m_netProto = m_netProto;
	
	if (newTeamDlg.DoModal(m_hWnd, NULL) != IDOK)
	{
		return;
	}

	::PostMessage(m_hWnd, FMG_MSG_UPDATE_BUDDY_LIST, 0, 0);
}


/**
 * @brief 删除好友分组
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnBuddyListDeleteTeam(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nTeamIndex = 0;
	int nBuddyIndex = 0;
	m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nBuddyIndex);
	if (nTeamIndex < 0 || nBuddyIndex>0)
	{
		return;
	}
	
	if(m_netProto && m_netProto->m_BuddyList.GetBuddyTeamCount() <= 1)
	{
		::MessageBox(m_hWnd, _T("至少得有一个分组。"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}

	std::string strTeamId = m_BuddyListCtrl.GetBuddyTeamID(nTeamIndex);
    if (strTeamId == DEFAULT_TEAM_ID)
    {
        ::MessageBox(m_hWnd, _T("默认分组不能删除！"), g_strAppTitle.c_str(), MB_ICONINFORMATION | MB_OK);
        return;
    }

	if (IDYES != ::MessageBox(m_hWnd, _T("删除该分组以后，该组下的好友将移至默认分组中。\n确定要删除该分组吗？"), g_strAppTitle.c_str(), MB_YESNO | MB_ICONQUESTION))
	{
		return;
	}

	if(m_netProto)
	{
		m_netProto->SendRemoveTeamReq(strTeamId);
	} 
}


/**
 * @brief 修改分组名称(UI)
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnBuddyListModifyTeamName(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nTeamIndex = 0;
	int nBuddyIndex = 0;

	m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nBuddyIndex);
	if (nTeamIndex < 0 || nBuddyIndex>0)
	{
		return;
	}

	CTeamDlg newTeamDlg;
	newTeamDlg.SetType(TEAM_OPERATION_MODIFY_TEAM_NAME);
	newTeamDlg.m_netProto = m_netProto;
	newTeamDlg.m_nTeamIndex = nTeamIndex;
	if (newTeamDlg.DoModal(m_hWnd, NULL) != IDOK)
	{
		return;
	}

	::PostMessage(m_hWnd, FMG_MSG_UPDATE_BUDDY_LIST, 0, 0);
}


/**
 * @brief 响应移动好友到分组(UI)
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnMoveBuddyToTeam(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nTeamIndex=-1;
	int nIndex=-1;
	m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nIndex);
	if (nTeamIndex < 0 || nIndex < 0)
	{
		return;
	}

	std::string strFriendId = m_BuddyListCtrl.GetBuddyItemUserId(nTeamIndex, nIndex);
	std::string strOldTeamId = m_BuddyListCtrl.GetBuddyTeamID(nTeamIndex);
	std::string strNewTeamId = m_BuddyListCtrl.GetBuddyTeamID(nID - TEAM_MENU_ITEM_BASE);

	if (m_netProto)
	{
		m_netProto->MoveFriendToTeam(strFriendId, strOldTeamId, strNewTeamId);
	}
}

/**
 * @brief 处理右键菜单删除好友
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnMenu_DeleteFriend(UINT uNotifyCode, int nID, CWindow wndCtl)
{

	int nTeamIndex = 0;
	int nBuddyIndex = 0;

	m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nBuddyIndex);
	if( (nTeamIndex==-1) || (nBuddyIndex==-1))
	{
		::MessageBox(m_hWnd, _T("请在需要删除的好友上按下右键菜单！"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}

	CString strNickName(m_BuddyListCtrl.GetBuddyItemNickName(nTeamIndex, nBuddyIndex));
	CString strMarkName(m_BuddyListCtrl.GetBuddyItemMarkName(nTeamIndex, nBuddyIndex));

	CString strInfo;
	strInfo.Format(_T("确定要删除%s(%s)吗？"), strMarkName, strNickName);
	int nRet = ::MessageBox(m_hWnd, strInfo, g_strAppTitle.c_str(), MB_YESNO|MB_ICONQUESTION);
	if (IDNO == nRet)
	{
		return;
	}
	
	//Core
	{
		std::string strFriendId = m_BuddyListCtrl.GetBuddyItemUserId(nTeamIndex, nBuddyIndex);
		if (m_netProto && !strFriendId.empty())
		{
			m_netProto->SendRemoveFriend(strFriendId);
		}
	}
}


/**
 * @brief 清空最近聊天列表
 * TODO: 界面从哪儿过来的
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnClearAllRecentList(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nRet = ::MessageBox(m_hWnd, _T("清空会话列表后无法恢复，确定要清空会话列表吗？"), g_strAppTitle.c_str(), MB_YESNO|MB_ICONWARNING);
	if (nRet != IDYES)
	{
		return;
	}

	m_userMgr.ClearRecentList();
	::PostMessage(m_hWnd, FMG_MSG_UPDATE_RECENT_LIST, 0, 0);
}


/**
 * @brief 删除最近聊天列表中的某一项(UI)
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnDeleteRecentItem(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nTeamIndex = 0;
	int nRecentIndex = 0;
	m_RecentListCtrl.GetCurSelIndex(nTeamIndex, nRecentIndex);
	if( (nTeamIndex==-1) || (nRecentIndex==-1) )
	{
		::MessageBox(m_hWnd, _T("请在需要删除的会话上按下右键菜单！"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}

	int nRet = ::MessageBox(m_hWnd, _T("确定要删除该会话吗？"), g_strAppTitle.c_str(), MB_YESNO|MB_ICONQUESTION);
	if (IDNO == nRet)
	{
		return;
	}

	//UINT uUserID = (UINT)m_RecentListCtrl.GetBuddyItemID(nTeamIndex, nRecentIndex);	
	//m_userMgr.DeleteRecentItem(uUserID);
	::PostMessage(m_hWnd, FMG_MSG_UPDATE_RECENT_LIST, 0, 0);
}


/**
 * @brief 好友列表,响应鼠标右键点击发送消息，
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnMenu_SendBuddyMessage(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	OnBuddyListDblClk(NULL);
}


/**
 * @brief 最近联系人列表,响应鼠标右键点击发送消息(UI)
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnMenu_SendBuddyMessageFromRecentList(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	OnRecentListDblClk(NULL);
}

//
/**
 * @brief 鼠标右键显示好友基本信息,好友列表右键菜单消息
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnMenu_ViewBuddyInfo(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nTeamIndex = 0;
	int nIndex = 0;
	m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nIndex);
	if (nTeamIndex < 0 || nIndex < 0)
	{
		return;
	}

	std::string strFriendId = m_BuddyListCtrl.GetBuddyItemUserId(nTeamIndex, nIndex);
	ShowBuddyInfoDlg(strFriendId, TRUE);
}


/**
 * @brief 鼠标右键显示好友基本信息，最近聊天列表(UI)
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnMenu_ViewBuddyInfoFromRecentList(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nTeamIndex = 0;
	int nIndex = 0;
	m_RecentListCtrl.GetCurSelIndex(nTeamIndex, nIndex);
	if (nTeamIndex < 0 || nIndex < 0)
	{
		return;
	}

	/*UINT nUTalkUin = m_RecentListCtrl.GetBuddyItemID(nTeamIndex, nIndex);
	::PostMessage(m_hWnd, WM_SHOW_BUDDY_INFO_DLG, NULL, nUTalkUin);*/
}


/**
 * @brief 响应修改好友备注的右键菜单消息(UI)
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnMenu_ModifyBuddyMarkName(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nTeamIndex = 0;
	int nIndex = 0;
	m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nIndex);
	if (nTeamIndex < 0 || nIndex < 0)
	{
		return;
	}

	//UINT uUserID = m_BuddyListCtrl.GetBuddyItemID(nTeamIndex, nIndex);
	//if (uUserID == 0)
	//{
	//	return;
	//}

	CModifyMarkNameDlg modifyMarkNameDlg;
	//modifyMarkNameDlg.m_uUserID = uUserID;
	//modifyMarkNameDlg.m_pFMGClient = &m_FMGClient;
	if (modifyMarkNameDlg.DoModal(m_hWnd, NULL) != IDOK)
	{
		return;
	}

	::PostMessage(m_hWnd, FMG_MSG_UPDATE_BUDDY_LIST, 0, 0);
}




/**
 * @brief 响应发送群聊消息的菜单,鼠标右键菜单(UI)
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnMenu_SendGroupMessage(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	OnGroupListDblClk(NULL);
}


/**
 * @brief 响应右键菜单的显示群资料(UI)
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnMenu_ViewGroupInfo(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nTeamIndex = 0;
	int nIndex = 0;
	m_GroupListCtrl.GetCurSelIndex(nTeamIndex, nIndex);

	if (nTeamIndex != -1 && nIndex != -1)
	{
		//TODO: GetBuddyItemUserId待修改
		std::string strGroupId = m_GroupListCtrl.GetBuddyItemUserId(nTeamIndex, nIndex);
		ShowGroupInfoDlg(strGroupId,TRUE);
	}		
}


//
/**
 * @brief 响应右键菜单的退出群组(UI)
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnMenu_ExitGroup(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nTeamIndex, nIndex;
	m_GroupListCtrl.GetCurSelIndex(nTeamIndex, nIndex);

	if (nTeamIndex != -1 && nIndex != -1)
	{
		std::string strGroupId = m_GroupListCtrl.GetBuddyItemUserId(nTeamIndex, nIndex);
		CString strPromptInfo;
		strPromptInfo.Format(_T("确实要退出群[%s(%s)]？"), m_GroupListCtrl.GetBuddyItemMarkName(nTeamIndex, nIndex), m_GroupListCtrl.GetBuddyItemNickName(nTeamIndex, nIndex));
		if (IDNO == ::MessageBox(m_hWnd, strPromptInfo, g_strAppTitle.c_str(), MB_YESNO | MB_ICONQUESTION))
		{
			return;
		}
		m_netProto->SendQuitFromGroupReq(strGroupId);
	}		
}


/**
 * @brief 响应创建新群组的右键菜单(UI)
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnMenu_CreateNewGroup(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CCreateNewGroupDlg dlg;
	if (IDOK != dlg.DoModal(NULL, NULL))
	{
		return;
	}
}


/**
 * @brief 响应刷新群组列表(UI)
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CMainDlg::OnMenu_RefreshGroupList(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	SendGetGroupList();
}



/**
 * @brief 响应网络失去连接
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnNetFailed(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LoadAppIcon(E_UI_ONLINE_STATUS::STATUS_OFFLINE);
	CString strIconInfo;
	strIconInfo.Format(_T("%s\r\n%s\r\n"), m_userMgr.m_UserInfo.m_strNickName.c_str(), m_userMgr.m_UserInfo.m_strAccount.c_str());
	m_TrayIcon.ModifyIcon(m_hAppIcon, strIconInfo, 1, TRUE, _T("温馨提示"), _T("您已经掉线，正在帮您重连......"), 3000);
	return 0;
}


/**
 * @brief 网络恢复
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnNetRecover(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//先将任务栏图标恢复成正常状态
	LoadAppIcon(E_UI_ONLINE_STATUS::STATUS_ONLINE);
	SetTrayIconOnLine();
	return 0;
}


/**
 * @brief 响应网络错误
 * TODO: 似乎和OnNetFailed重复
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnNetError(UINT uMsg, WPARAM wParam, LPARAM lParam)
{        
    //重连定时器已经开启，直接退出，避免重复开启
    
    m_userMgr.ResetToOfflineStatus();

    ::SendMessage(m_userMgr.m_hCallBackWnd, FMG_MSG_UPDATE_BUDDY_INFO, 0, (LPARAM)(m_userMgr.m_UserInfo.m_uUserIndex));
    ::SendMessage(m_userMgr.m_hCallBackWnd, FMG_MSG_UPDATE_BUDDY_LIST, 0, 0);
    ::SendMessage(m_userMgr.m_hCallBackWnd, FMG_MSG_UPDATE_RECENT_LIST, 0, 0);
    ::SendMessage(m_userMgr.m_hCallBackWnd, FMG_MSG_UPDATE_GROUP_LIST, 0, 0);
    
    return 1;
}


/**
 * @brief 处理登录成功的消息
 * 
 * @return LRESULT 
 */
LRESULT CMainDlg::DoLoginSucceed()
{
	ShowPanel(TRUE);
	ShowWindow(SW_SHOW);
	Invalidate();

	//读取重连时间间隔
	CIniFile iniFile;
	CString strIniFile;
	strIniFile.Format(_T("%sconfig\\flamingo.ini"), g_szHomePath);


	CreateEssentialDirectories();

	// 保存登录帐号列表
	LOGIN_ACCOUNT_INFO* lpAccount = m_LoginAccountList.Find(m_stAccountInfo.szUser);
	if (lpAccount != NULL)
	{
		memcpy(lpAccount, &m_stAccountInfo, sizeof(LOGIN_ACCOUNT_INFO));
	}
	else
	{
		m_LoginAccountList.Add(m_stAccountInfo.szUser, m_stAccountInfo.szPwd,
			m_stAccountInfo.nStatus, m_stAccountInfo.bRememberPwd, m_stAccountInfo.bAutoLogin);
	}
	m_LoginAccountList.SetLastLoginUser(m_stAccountInfo.szUser);
	WString strFileName = Hootina::CPath::GetAppPath() + _T("Users\\LoginAccountList.dat");
	m_LoginAccountList.SaveFile(strFileName.c_str());



	// 注册提取消息热键(Ctrl+Alt+D)
	::RegisterHotKey(m_hWnd, HOT_KEY_SHOW_MSG, MOD_CONTROL | MOD_ALT, g_cHotKey);

	//加载加好友通知Icon
	LoadAddFriendIcon();


	//重连成功后，保持主窗口原来的位置
	if (m_nMainPanelStatus != MAINPANEL_STATUS_RECONNECTING)
	{
		SetWindowPos(NULL,
			m_userCfg.GetMainDlgX(),
			m_userCfg.GetMainDlgY(),
			m_userCfg.GetMainDlgWidth(),
			m_userCfg.GetMainDlgHeight(),
			SWP_SHOWWINDOW);
	}


	m_userMgr.LoadTeamInfo();
	m_userMgr.LoadBuddyInfo();
	//加载最近联系人列表
	m_userMgr.LoadRecentList();
	if (m_userMgr.GetRecentListCount() > 0 && m_nCurSelIndexInMainTab < 0)
	{
		m_TabCtrl.SetCurSel(0);
		OnTabCtrlSelChange(NULL);
	}
	else
	{
		if (m_nCurSelIndexInMainTab < 0)
		{
			m_TabCtrl.SetCurSel(1);
		}
		else
		{
			m_TabCtrl.SetCurSel(m_nCurSelIndexInMainTab);
		}
		OnTabCtrlSelChange(NULL);
	}

	::SetForegroundWindow(m_hWnd);
	::SetFocus(m_hWnd);

	m_nMainPanelStatus = MAINPANEL_STATUS_LOGIN;

	//登录成功获取好友列表
	GetFriendList();
	//登录成功获取群组列表
	SendGetGroupList();
	OnUserLoginRsp(0, 0, 0);



	CenterWindow(::GetDesktopWindow());
	return 1;
}

/**
 * @brief 响应用户登录结果
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnLoginResult(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	E_UI_LOGIN_RESULT_CODE nCode = static_cast<E_UI_LOGIN_RESULT_CODE>(lParam);
    
    //重连不成功，清除以发送登录请求包标识，再次发送重连登录数据包
    if (m_nMainPanelStatus == MAINPANEL_STATUS_RECONNECTING && 
		nCode != E_UI_LOGIN_RESULT_CODE::LOGIN_SUCCESS)
    {
        return 0;
    }

	E_UI_ONLINE_STATUS nStatus = m_netProto->GetStatus();

 	LoadAppIcon(nStatus);
	
	switch (nCode)
	{
	case E_UI_LOGIN_RESULT_CODE::LOGIN_SUCCESS:				// 登录成功
		{
			DoLoginSucceed();
			SetTrayIconOnLine();
		}break;

	case E_UI_LOGIN_RESULT_CODE::LOGIN_FAILED:				// 登录失败
		{
			if (m_nLoginTryTimes < MAX_LOGIN_TRY_TIMES)
			{
				m_nLoginTryTimes++;
				StartLogin();

				m_nMainPanelStatus = MAIN_PANEL_STATUS_NOT_LOGIN;
			}
			else
			{
				m_nLoginTryTimes = 0;
				MessageBox(_T("网络故障，登录失败！"), g_strAppTitle.c_str(), MB_OK);
			}
			SetTrayIconOffLine();
		}break;

	case E_UI_LOGIN_RESULT_CODE::LOGIN_UNREGISTERED:
		{						
			MessageBox(_T("用户名未注册！"), g_strAppTitle.c_str(), MB_OK);
			StartLogin();
            m_nMainPanelStatus = MAIN_PANEL_STATUS_NOT_LOGIN;
		}
		break;

	case E_UI_LOGIN_RESULT_CODE::LOGIN_PASSWORD_ERROR:		// 密码错误
		{
			MessageBox(_T("密码错误！"), g_strAppTitle.c_str(), MB_OK);
			StartLogin();
            m_nMainPanelStatus = MAIN_PANEL_STATUS_NOT_LOGIN;
		}
		break;
	
	case E_UI_LOGIN_RESULT_CODE::LOGIN_SERVER_REFUSED:
		{
			ShowPanel(FALSE);		// 显示登录面板
			MessageBox(_T("服务器拒绝您的登录请求！"), g_strAppTitle.c_str(), MB_OK);
			StartLogin();
            m_nMainPanelStatus = MAIN_PANEL_STATUS_NOT_LOGIN;
	}break;

	case E_UI_LOGIN_RESULT_CODE::LOGIN_USER_CANCEL_LOGIN:	// 用户取消登录
		{
			StartLogin();
            m_nMainPanelStatus = MAIN_PANEL_STATUS_NOT_LOGIN;
		}
		break;
	}

	return 0;
}


/**
 * @brief 响应退出登录结果
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnLogoutResult(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CloseDialog(IDOK);
	return 0;
}


/**
 * @brief 更新用户信息
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnUpdateUserInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//CBuddyInfoResult* lpBuddyInfoResult = (CBuddyInfoResult*)lParam;
	//if (NULL == lpBuddyInfoResult)
	//	return 0;
	////m_FMGClient.OnUpdateBuddyInfo(lpBuddyInfoResult);
	//delete lpBuddyInfoResult;

	return 0;
}

// 
/**
 * @brief 响应更新好友列表消息
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnUpdateBuddyList(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT uAccountID = (UINT)wParam;

	BuddyListSortOnStaus();
	
	UpdateBuddyTreeCtrl();		// 更新好友列表控件

    //TODO: 后期放到更新最近联系人和群组列表之后
    //m_FMGClient.m_RecvMsgThread.EnableUI(true);

	return 1;
}


/**
 * @brief 响应更新群列表消息
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnUpdateGroupList(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//BOOL bSuccess = (BOOL)lParam;
	//if (!bSuccess)
	//	return 0;

	UpdateGroupTreeCtrl();		// 更新群列表控件


	return 0;
}

// 
/**
 * @brief 响应更新最近联系人列表消息(UI)
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnUpdateRecentList(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	UpdateRecentTreeCtrl();		// 更新最近联系人列表控件

	return 0;
}

LRESULT CMainDlg::OnRecvGroupMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	C_UI_GroupMessage * pGroupMsg = (C_UI_GroupMessage *)(lParam);
	if (nullptr != pGroupMsg)
	{
		CreateGroupChatDlg(pGroupMsg->m_strGroupId);
		auto item = m_mapGroupChatDlg.find(pGroupMsg->m_strGroupId);
		if (item != m_mapGroupChatDlg.end())
		{
			item->second->OnRecvGroupMsg(pGroupMsg);
			item->second->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		
	}
	return 1;
}

/**
 * @brief 根据群组ID创建群组
 * 
 * @param strGroupId 群组ID
 * @return true 
 * @return false 
 */
bool CMainDlg::CreateGroupChatDlg(const std::string strGroupId)
{
	if (m_mapGroupChatDlg.find(strGroupId) == m_mapGroupChatDlg.end())
	{
		CGroupChatDlg* lpGroupChatDlg = new CGroupChatDlg;
		if (lpGroupChatDlg != NULL)
		{
			long nLastWidth = m_userCfg.GetGroupDlgWidth();
			long nLastHeight = m_userCfg.GetGroupDlgHeight();
			lpGroupChatDlg->m_lpFaceList = &m_FaceList;
			lpGroupChatDlg->m_lpCascadeWinManager = &m_CascadeWinManager;
			lpGroupChatDlg->m_hMainDlg = m_hWnd;
			lpGroupChatDlg->m_strGroupId = strGroupId;
			m_mapGroupChatDlg[strGroupId] = lpGroupChatDlg;
			lpGroupChatDlg->m_netProto = m_netProto;
			lpGroupChatDlg->Create(NULL);
			lpGroupChatDlg->ShowWindow(SW_SHOW);
			lpGroupChatDlg->SetWindowPos(NULL, 0, 0, nLastWidth, nLastHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
			SendMessage(lpGroupChatDlg->m_hWnd, WM_SIZE, (WPARAM)SIZE_RESTORED, (LPARAM)MAKELONG(nLastWidth, nLastHeight));
			::SetForegroundWindow(lpGroupChatDlg->m_hWnd);
			return true;
		}
		else 
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}

/**
 * @brief 响应收到好友发送文件的请求消息
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */

LRESULT CMainDlg::OnRecvFriendFileReqMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	C_WND_MSG_FileRecvReq * pGroupMsg = (C_WND_MSG_FileRecvReq*)(lParam);
	if (nullptr != pGroupMsg)
	{
		ShowBuddyChatDlg(pGroupMsg->m_szFriendId);

		auto item = m_mapBuddyChatDlg.find(pGroupMsg->m_szFriendId);
		if (item != m_mapBuddyChatDlg.end())
		{
			item->second->OnFileRecvReqMsg(pGroupMsg);
		}
	}
	return 1;
}

/**
 * @brief 响应好友间发送文件的回复
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnSendFriendFileRspMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)//收到的文件回复消息
{
	C_WND_MSG_FileSendRsp * pGroupMsg = (C_WND_MSG_FileSendRsp*)(lParam);
	if (nullptr != pGroupMsg)
	{
		ShowBuddyChatDlg(pGroupMsg->m_szFriendId);
		auto item = m_mapBuddyChatDlg.find(pGroupMsg->m_szFriendId);
		if (item != m_mapBuddyChatDlg.end())
		{
			item->second->OnFileSendRspMsg(pGroupMsg);
		}
	}
	return 1;
}



/**
 * @brief 响应状态改变消息
 * TODO: 暂时没有使用
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnStatusChangeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//UINT nUTalkUin = (UINT)lParam;
	//if (0 == nUTalkUin)
	//	return 0;

	//C_UI_BuddyList* lpBuddyList = NULL;// m_FMGClient.GetBuddyList();
	//if (NULL == lpBuddyList)
	//	return 0;

	///*C_UI_BuddyInfo* lpBuddyInfo = lpBuddyList->GetBuddy(nUTalkUin);
	//if (NULL == lpBuddyInfo)
	//	return 0;*/

	//C_UI_BuddyTeamInfo* lpBuddyTeam = lpBuddyList->GetBuddyTeam(lpBuddyInfo->m_nTeamIndex);
	//if (NULL == lpBuddyTeam)
	//	return 0;

	//int nOnlineCnt = lpBuddyTeam->GetOnlineBuddyCount();
	//BOOL bOnline = TRUE;
	//if(lpBuddyInfo->m_nStatus==E_UI_ONLINE_STATUS::STATUS_OFFLINE || lpBuddyInfo->m_nStatus== E_UI_ONLINE_STATUS::STATUS_INVISIBLE)
	//	bOnline = FALSE;

	//if (bOnline)	// 播放好友上线声音
	//{
 //       // 是否启用静音
 //       if (!m_userCfg.IsEnableMute())
 //       {
 //           WString strFileName = Hootina::CPath::GetAppPath() + _T("Sound\\Global.wav");
 //           ::sndPlaySound(strFileName.c_str(), SND_ASYNC);
 //       }
	//}

	//int nTeamIndex, nIndex;	// 获取好友项索引
	//m_BuddyListCtrl.GetItemIndexByID(nUTalkUin, nTeamIndex, nIndex);

	//// 更新好友分组在线人数
	//m_BuddyListCtrl.SetBuddyTeamCurCnt(nTeamIndex, nOnlineCnt);

	//// 更新好友头像
	//CString strThumbPath;
	//if(lpBuddyInfo->m_bUseCustomFace && lpBuddyInfo->m_bCustomFaceAvailable)
	//{
	//	//strThumbPath.Format(_T("%s%d.png"), m_userMgr.GetCustomUserThumbFolder().c_str(), lpBuddyInfo->m_uUserID);
	//	//if(!Hootina::CPath::IsFileExist(strThumbPath))
	//	//	strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, lpBuddyInfo->m_nFace);
	//}
	//else
	//	strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, lpBuddyInfo->m_nFace);
	//m_BuddyListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strThumbPath, !bOnline);

	//// 设置好友在线状态并启动上线动画
	//m_BuddyListCtrl.SetBuddyItemOnline(nTeamIndex, nIndex, bOnline, TRUE);


	//TODO  枚举类型合并
    /*if (lpBuddyInfo->m_nStatus == online_type_android_cellular || lpBuddyInfo->m_nStatus == online_type_android_wifi)
	{
		strThumbPath.Format(_T("%sImage\\mobile_online.png"), g_szHomePath);
		m_BuddyListCtrl.SetBuddyItemMobilePic(nTeamIndex, nIndex, strThumbPath, TRUE);
	}
    else if (lpBuddyInfo->m_nStatus == online_type_offline || lpBuddyInfo->m_nStatus == online_type_pc_invisible)
	{
		strThumbPath.Format(_T("%sImage\\mobile_online.png"), g_szHomePath);
		m_BuddyListCtrl.SetBuddyItemMobilePic(nTeamIndex, nIndex, strThumbPath, FALSE);
	}*/


	m_BuddyListCtrl.Invalidate();	// 刷新好友列表控件

	return 0;
}


/**
 * @brief 被踢下线消息
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnKickMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPCTSTR lpszReason = _T("被踢下线！");
	MessageBox(lpszReason, _T("提示"), MB_OK);
	PostMessage(WM_CLOSE);
	return 0;
}


/**
 * @brief 响应截屏消息
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnScreenshotMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //CScreenshotInfo* pScreenshotInfo = (CScreenshotInfo*)lParam;
    //if (pScreenshotInfo == NULL)
    //    return 0;
   

    

    //BITMAPINFOHEADER*	bmpHeader = (BITMAPINFOHEADER*)pScreenshotInfo->m_strBmpHeader.c_str();

    //const char* bmpbuf = pScreenshotInfo->m_strBmpData.c_str();
    return 0;

    //HDC hdc = ::GetDC(m_pRemoteDesktopDlg->m_hWnd);
    //BITMAPINFO* bmpinfo = (BITMAPINFO*)bmpHeader;//位图信息 
    //HBITMAP hbitmap = CreateDIBitmap(hdc, bmpHeader, CBM_INIT, bmpbuf, bmpinfo, DIB_RGB_COLORS);

    //PBITMAPINFO bmpInf = (PBITMAPINFO)pScreenshotInfo->m_strBmpHeader.c_str();
    //m_pRemoteDesktopDlg->MoveWindow(0, 0, bmpInf->bmiHeader.biWidth - 200, bmpInf->bmiHeader.biHeight - 200, FALSE);

    //CClientDC dc(m_pRemoteDesktopDlg->m_hWnd);

    //int nPaletteSize = 0;
    //BYTE* buf = ((BYTE*)bmpInf) +
    //    sizeof(BITMAPINFOHEADER) +
    //    sizeof(RGBQUAD)*nPaletteSize;

    //int nOffset;
    //BYTE r, g, b;
    //int nWidth = bmpInf->bmiHeader.biWidth*bmpInf->bmiHeader.biBitCount / 8;
    //nWidth = ((nWidth + 3) / 4) * 4; //4字节对齐

    ////if (bmpInf->bmiHeader.biBitCount == 8)
    ////{
    ////    for (int i = 0; i<bmpInf->bmiHeader.biHeight; i++)
    ////    {
    ////        for (int j = 0; j<bm.bmWidth; j++)
    ////        {
    ////            RGBQUAD rgbQ;
    ////            rgbQ = bmpInf->bmiColors[buf[i*nWidth + j]];
    ////            dc.SetPixel(j, bm.bmHeight - i, RGB(rgbQ.rgbRed, rgbQ.rgbGreen, rgbQ.rgbBlue)); //测试显示
    ////        }
    ////    }
    ////}
    ////else if (bmpInf->bmiHeader.biBitCount == 16)
    ////{
    ////    for (int i = 0; i<bm.bmHeight; i++)
    ////    {
    ////        nOffset = i*nWidth;
    ////        for (int j = 0; j<bm.bmWidth; j++)
    ////        {
    ////            b = buf[nOffset + j * 2] & 0x1F;
    ////            g = buf[nOffset + j * 2] >> 5;
    ////            g |= (buf[nOffset + j * 2 + 1] & 0x03) << 3;
    ////            r = (buf[nOffset + j * 2 + 1] >> 2) & 0x1F;

    ////            r *= 8;
    ////            b *= 8;
    ////            g *= 8;

    ////            dc.SetPixel(j, bm.bmHeight - i, RGB(r, g, b)); //测试显示
    ////        }
    ////    }
    ////}
    ////else if (bmpInf->bmiHeader.biBitCount == 24)
    ////{
    ////    for (int i = 0; i<bm.bmHeight; i++)
    ////    {
    ////        nOffset = i*nWidth;
    ////        for (int j = 0; j<bm.bmWidth; j++)
    ////        {
    ////            b = buf[nOffset + j * 3];
    ////            g = buf[nOffset + j * 3 + 1];
    ////            r = buf[nOffset + j * 3 + 2];

    ////            dc.SetPixel(j, bm.bmHeight - i, RGB(r, g, b)); //测试显示
    ////        }
    ////    }
    ////}
    ///*else*/ if (bmpInf->bmiHeader.biBitCount == 32)
    //{
    //    for (int i = 0; i<bmpInf->bmiHeader.biHeight; i++)
    //    {
    //        nOffset = i*nWidth;
    //        for (int j = 0; j<bmpInf->bmiHeader.biWidth; j++)
    //        {
    //            b = buf[nOffset + j * 4];
    //            g = buf[nOffset + j * 4 + 1];
    //            r = buf[nOffset + j * 4 + 2];

    //            dc.SetPixel(j, bmpInf->bmiHeader.biHeight - i, RGB(r, g, b)); //测试显示
    //        }
    //    }
    //}
    //
    //return 0;
}

// 群系统消息

/**
 * @brief 响应系统的群消息
 * TODO: 暂时没有使用
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnSysGroupMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//UINT nGroupCode = (UINT)lParam;
	//if (0 == nGroupCode)
	//	return 0;

	//WString strFileName = Hootina::CPath::GetAppPath() + _T("Sound\\system.wav");	// 播放来系统消息提示音
	//::sndPlaySound(strFileName.c_str(), SND_ASYNC);

	//UpdateMsgIcon();

	//if (m_MsgTipDlg.IsWindow())
	//	m_MsgTipDlg.AddMsgSender(long_SYSGROUP, nGroupCode);

	//if (NULL == m_dwMsgTimerId)
	//	m_dwMsgTimerId = SetTimer(2, ::GetCaretBlinkTime(), NULL);

	return 0;
}



/**
 * @brief 响应群组成员号码更新
 * TODO: 用途不明确
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnUpdateGMemberNumber(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nUTalkUin = (UINT)lParam;
	if (0 == nGroupCode || 0 == nUTalkUin)
		return 0;

 	NotifyGroupChatDlg(nGroupCode, FMG_MSG_UPDATE_GMEMBER_NUMBER, wParam, lParam);// 通知群聊天窗口更新
	NotifyGMemberInfoDlg(nGroupCode, nUTalkUin, FMG_MSG_UPDATE_GMEMBER_NUMBER);	// 通知群成员信息窗口更新
	NotifySessChatDlg(nUTalkUin, FMG_MSG_UPDATE_GMEMBER_NUMBER);				// 通知群成员聊天窗口更新

	return 0;
}


/**
 * @brief 响应群组号码的更新
 * TODO: 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnUpdateGroupNumber(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)lParam;
	if (0 == nGroupCode)
		return 0;

 	NotifyGroupChatDlg(nGroupCode, FMG_MSG_UPDATE_GROUP_NUMBER, 0, 0);// 通知群聊天窗口更新
 	NotifyGroupInfoDlg(nGroupCode, FMG_MSG_UPDATE_GROUP_NUMBER);		// 通知群信息窗口更新

	return 0;
}


/**
 * @brief 响应好友个性签名更新的消息(UI)
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnUpdateBuddySign(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nUTalkUin = lParam;
	if (0 == nUTalkUin)
		return 0;

	C_UI_BuddyInfo* lpBuddyInfo = NULL;// m_FMGClient.GetUserInfo(nUTalkUin);
	if (lpBuddyInfo == NULL)
		return 0;

	if (m_userMgr.m_UserInfo.m_uUserIndex == nUTalkUin)	// 更新用户个性签名
	{
		if (!lpBuddyInfo->m_strSign.empty())
		{
			m_btnSign.SetWindowText(lpBuddyInfo->m_strSign.c_str());
			m_btnSign.SetToolTipText(lpBuddyInfo->m_strSign.c_str());
		}
		else
		{
			m_btnSign.SetWindowText(_T(""));
			m_btnSign.SetToolTipText(_T("点击修改个性签名"));
		}
	}
	else	// 更新好友个性签名
	{
		//C_UI_BuddyList* lpBuddyList = NULL;// m_FMGClient.GetBuddyList();		// 更新好友列表控件的个性签名
		//if (lpBuddyList != NULL)
		//{
		//	C_UI_BuddyInfo* lpBuddyInfo = lpBuddyList->GetBuddy(nUTalkUin);
		//	if (lpBuddyInfo != NULL)
		//	{
		//		int nTeamIndex, nIndex;
		//		m_BuddyListCtrl.GetItemIndexByID(nUTalkUin, nTeamIndex, nIndex);
		//		m_BuddyListCtrl.SetBuddyItemSign(nTeamIndex, nIndex, lpBuddyInfo->m_strSign.c_str());
		//	}
		//}

		NotifyBuddyChatDlg(nUTalkUin, FMG_MSG_UPDATE_BUDDY_SIGN);		// 通知好友聊天窗口更新
		NotifyBuddyInfoDlg(nUTalkUin, FMG_MSG_UPDATE_BUDDY_SIGN);		// 通知好友信息窗口更新
	}

	return 0;
}


/**
 * @brief 响应群组成员个性签名更新的消息
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnUpdateGMemberSign(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nUTalkUin = (UINT)lParam;
	if (0 == nGroupCode || 0 == nUTalkUin)
		return 0;

	NotifyGMemberInfoDlg(nGroupCode, nUTalkUin, FMG_MSG_UPDATE_GMEMBER_SIGN);	// 通知群成员信息窗口更新

	return 0;
}


/**
 * @brief 更新用户信息，暂时没有使用
 * TODO:调用位置不明确
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnUpdateBuddyInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nUTalkUin = lParam;
	if (0 == nUTalkUin)
		return 0;
	
	C_UI_BuddyInfo* lpBuddyInfo = NULL;// m_FMGClient.GetUserInfo(nUTalkUin);
 	if (lpBuddyInfo == NULL)	// 更新用户昵称
		return 0;
	
	if(lpBuddyInfo->m_uUserIndex == m_userMgr.m_UserInfo.m_uUserIndex)
	{
		//如果昵称为空，则显示账户名
		if(lpBuddyInfo->m_strNickName.empty())
			m_staNickName.SetWindowText(lpBuddyInfo->m_strAccount.c_str());
		else
			m_staNickName.SetWindowText(lpBuddyInfo->m_strNickName.c_str());	

		//m_staNickName.Invalidate(FALSE);

		E_UI_ONLINE_STATUS nStatus = m_netProto->GetStatus();
		LoadAppIcon(nStatus);
		CString strIconInfo;
		if (nStatus == E_UI_ONLINE_STATUS::STATUS_OFFLINE)
		{
			strIconInfo.Format(_T("%s\r\n%s\r\n离线"), m_userMgr.m_UserInfo.m_strNickName.c_str(), m_userMgr.m_UserInfo.m_strAccount.c_str());
		}
		else
		{
			strIconInfo.Format(_T("%s\r\n%s\r\n在线"), m_userMgr.m_UserInfo.m_strNickName.c_str(), m_userMgr.m_UserInfo.m_strAccount.c_str());
		}
		m_TrayIcon.ModifyIcon(m_hAppIcon, strIconInfo);
	}
	else
	{
		//int nTeamIndex, nIndex;	// 获取好友项索引
		//m_BuddyListCtrl.GetItemIndexByID(nUTalkUin, nTeamIndex, nIndex);
		//m_BuddyListCtrl.SetBuddyItemMarkName(nTeamIndex, nIndex, lpBuddyInfo->m_strMarkName.c_str());
		//m_BuddyListCtrl.SetBuddyItemNickName(nTeamIndex, nIndex, lpBuddyInfo->m_strNickName.c_str());
		//m_BuddyListCtrl.Invalidate(FALSE);
	}


	//同时更新头像信息 
	PostMessage(FMG_MSG_UPDATE_BUDDY_NUMBER, 0, (LPARAM)nUTalkUin);
	PostMessage(FMG_MSG_UPDATE_BUDDY_HEADPIC, 0, (LPARAM)nUTalkUin);
	PostMessage(FMG_MSG_UPDATE_BUDDY_SIGN, 0, (LPARAM)nUTalkUin);
	
	//通知群聊窗口更新
	for(const auto& iter : m_userMgr.m_GroupList.m_arrGroupInfo)
	{
		if(iter->IsMember(nUTalkUin))
		{
			PostMessage(FMG_MSG_UPDATE_GROUP_INFO, nUTalkUin, 0);
		}
	}
	// 通知好友信息窗口更新
	NotifyBuddyInfoDlg(nUTalkUin, FMG_MSG_UPDATE_BUDDY_INFO);		
	
#ifdef _DEBUG
	static long i = 0;
	AtlTrace(_T("Update Buddy Info: %d.\n"), i);
	i++;
#endif
	return 0;
}


/**
 * @brief 更新聊天对话框的在线状态
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnUpdateChatDlgOnlineStatus(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT uAccountID = (UINT)wParam;
	//long nStatus = (long)lParam;
	/*std::map<UINT, CBuddyChatDlg*>::iterator iter;
	iter = m_mapBuddyChatDlg.find(uAccountID);
	if (iter != m_mapBuddyChatDlg.end())
	{
		CBuddyChatDlg* pBuddyChatDlg = iter->second;
#ifndef _DEBUG
		if(pBuddyChatDlg == NULL)
		{
			LOG_ERROR("Why 0x%08x is null pointer of a chat dlg？", pBuddyChatDlg);
			return 0;
		}
#endif
															
		pBuddyChatDlg->OnUpdateBuddyNumber();
		pBuddyChatDlg->OnUpdateBuddySign();		// 更新好友签名通知
		pBuddyChatDlg->OnUpdateBuddyHeadPic();
		//pBuddyChatDlg->OnUpdateBuddyHeadPic();
#ifndef _DEBUG
		if(!pBuddyChatDlg->IsWindow())
		{
			LOG_ERROR("Why 0x%08x is a invalid window of a chat dlg？", pBuddyChatDlg);
			return 0;
		}
#endif

		pBuddyChatDlg->Invalidate(FALSE);
	}
	*/
	return 1;
}

/**
 * @brief 更新群成员信息
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnUpdateGMemberInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nUTalkUin = (UINT)lParam;
	if (0 == nGroupCode || 0 == nUTalkUin)
	{
		return 0;
	}
	NotifyGMemberInfoDlg(nGroupCode, nUTalkUin, FMG_MSG_UPDATE_GMEMBER_INFO);	// 通知群成员信息窗口更新

	return 0;
}


/**
 * @brief 响应更新群信息
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnUpdateGroupInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT uAccountID = (UINT)wParam;

 	NotifyGroupChatDlg(uAccountID, FMG_MSG_UPDATE_GROUP_INFO, 0, 0);	// 通知群聊天窗口更新
 	NotifyGroupInfoDlg(uAccountID, FMG_MSG_UPDATE_GROUP_INFO);		// 通知群信息窗口更新
//	C_UI_GroupInfo* pGroupInfo = NULL;
//	C_UI_BuddyInfo* pBuddyInfo = NULL;
//	std::map<UINT, CGroupChatDlg*>::iterator iterGroupChatDlg;
//	UINT nGroupCode = 0;
//	for(const auto& iter : m_userMgr.m_GroupList.m_arrGroupInfo)
//	{
//		pGroupInfo = iter;
//		if(pGroupInfo == NULL)
//			continue;
//
//		pBuddyInfo = pGroupInfo->GetMemberByUin(uAccountID);
//		if(pBuddyInfo == NULL)
//			continue;
//		
//		nGroupCode = pGroupInfo->m_nGroupCode;
//		iterGroupChatDlg = m_mapGroupChatDlg.find(nGroupCode);
//		if(iterGroupChatDlg != m_mapGroupChatDlg.end())
//		{
//#ifndef _DEBUG
//			if(iterGroupChatDlg->second->IsWindow())
//			{
//#endif
//				iterGroupChatDlg->second->OnUpdateGroupInfo();
//#ifndef _DEBUG
//			}
//#endif
//		}
//		
//	}

	return 0;
}

// HTREEITEM CMainDlg::FindTreeItemByUTalkUin(CTreeViewCtrl& treeCtrl, UINT nUTalkUin)
// {
// 	HTREEITEM hItem = treeCtrl.GetRootItem();
// 	while (hItem != NULL)
// 	{
// 		UINT nUTalkUin2 = (UINT)treeCtrl.GetItemData(hItem);
// 		if (nUTalkUin2 == nUTalkUin)
// 			return hItem;
// 
// 		HTREEITEM hFindItem = FindTreeItemByUTalkUin2(treeCtrl, hItem, nUTalkUin);
// 		if (hFindItem != NULL)
// 			return hFindItem;
// 
// 		hItem = treeCtrl.GetNextItem(hItem, TVGN_NEXT);
// 	}
// 	return NULL;
// }
// 
// HTREEITEM CMainDlg::FindTreeItemByUTalkUin2(CTreeViewCtrl& treeCtrl, HTREEITEM hItem, UINT nUTalkUin)
// {
// 	if (treeCtrl.ItemHasChildren(hItem))
// 	{
// 		HTREEITEM hChildItem = treeCtrl.GetChildItem(hItem);
// 
// 		while (hChildItem != NULL)
// 		{
// 			UINT nUTalkUin2 = (UINT)treeCtrl.GetItemData(hItem);
// 			if (nUTalkUin2 == nUTalkUin)
// 				return hItem;
// 
// 			HTREEITEM hFindItem = FindTreeItemByUTalkUin2(treeCtrl, hChildItem, nUTalkUin);
// 			if (hFindItem != NULL)
// 				return hFindItem;
// 
// 			hChildItem = treeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
// 		}
// 	}
// 
// 	UINT nUTalkUin2 = (UINT)treeCtrl.GetItemData(hItem);
// 	return  (nUTalkUin2 == nUTalkUin) ? hItem : NULL;
// }

// 
/**
 * @brief 响应更新好友头像图片
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnUpdateBuddyHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nUTalkUin = (UINT)lParam;
	if(nUTalkUin == 0)
		return 0;

	C_UI_BuddyInfo* lpBuddyInfo = NULL;// m_FMGClient.GetUserInfo(nUTalkUin);
	if(lpBuddyInfo == NULL)
		return 0;

	CString strThumbPath;
	if(lpBuddyInfo->m_bUseCustomFace  && lpBuddyInfo->m_bCustomFaceAvailable)
	{
		//strThumbPath.Format(_T("%s%d.png"), m_userMgr.GetCustomUserThumbFolder().c_str(), lpBuddyInfo->m_uUserID);
		//if(!Hootina::CPath::IsFileExist(strThumbPath))
		//	strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, lpBuddyInfo->m_nFace);
	}
	else
	{
		strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, lpBuddyInfo->m_nFace);
	}
	
	//更新自己的头像
	if (m_userMgr.m_UserInfo.m_uUserIndex == nUTalkUin)	// 更新用户头像
	{
		BOOL bGray = GetStatus() != E_UI_ONLINE_STATUS::STATUS_OFFLINE ? FALSE : TRUE;
		m_picHead.SetBitmapWithoutCache(strThumbPath, FALSE, bGray);
		m_picHead.Invalidate();
		return 0;
	}

	// 更新好友列表头像
	UINT nUTalkNum = 0;
	BOOL bGray = FALSE;
	C_UI_BuddyList* lpBuddyList = NULL;// m_FMGClient.GetBuddyList();
	if (lpBuddyList != NULL)
	{
		//C_UI_BuddyInfo* lpBuddyInfo = lpBuddyList->GetBuddy(nUTalkUin);
		//if (lpBuddyInfo != NULL)
		//{
		//	nUTalkNum = lpBuddyInfo->m_uUserIndex;
		//	bGray = lpBuddyInfo->m_nStatus != E_UI_ONLINE_STATUS::STATUS_OFFLINE ? FALSE : TRUE;
		//}
	}

	//int nTeamIndex, nIndex;
	//m_BuddyListCtrl.GetItemIndexByID(nUTalkUin, nTeamIndex, nIndex);
	//m_BuddyListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strThumbPath, bGray);

	// 更新最近联系人列表头像
	int nItemCnt = m_RecentListCtrl.GetBuddyItemCount(0);
	for (int i = 0; i < nItemCnt; i++)
	{
		//int nIndex = m_RecentListCtrl.GetBuddyItemID(0, i);

		//C_UI_RecentList* lpRecentList = NULL;// m_FMGClient.GetRecentList();
		//if (lpRecentList != NULL)
		//{
		//	C_UI_RecentInfo* lpRecentInfo = lpRecentList->GetRecent(nIndex);
		//	if (lpRecentInfo != NULL)
		//	{
		//		if (E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_BUDDY == lpRecentInfo->m_nType)			// 好友
		//		{
		//			if (nUTalkUin == lpRecentInfo->m_uUserID)
		//			{
		//				m_RecentListCtrl.GetItemIndexByID(nIndex, nTeamIndex, nIndex);
		//				m_RecentListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strThumbPath, bGray);
		//				break;
		//			}
		//		}
		//	}
		//}
	}

	NotifyBuddyChatDlg(nUTalkUin, FMG_MSG_UPDATE_BUDDY_HEADPIC);		// 通知好友聊天窗口更新

	return 0;
}

// 
/**
 * @brief 更新群成员头像图片
 * TODO: 调用未知不明确
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnUpdateGMemberHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nUTalkUin = (UINT)lParam;

	NotifyGroupChatDlg(nGroupCode, FMG_MSG_UPDATE_GMEMBER_HEADPIC, wParam, lParam);	// 通知群聊天窗口更新
	NotifySessChatDlg(nUTalkUin, FMG_MSG_UPDATE_GMEMBER_HEADPIC);						// 通知群成员聊天窗口更新

	return 0;
}


/**
 * @brief 响应更新群头像图片
 * TODO: 调用位置不明确
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnUpdateGroupHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;

	// 更新群列表头像
	UINT nGroupNum = 0;
	CGroupList* lpGroupList = m_netProto->GetGroupList();
	if (lpGroupList != NULL)
	{
		C_UI_GroupInfo* lpGroupInfo = NULL;// lpGroupList->GetGroupByCode(nGroupCode);
		if (lpGroupInfo != NULL)
		{
			//nGroupNum = lpGroupInfo->m_nGroupNumber;
		}
	}

	WString strFileName;// m_FMGClient.GetGroupHeadPicFullName(nGroupNum);
	if (!Hootina::CPath::IsFileExist(strFileName.c_str()))
		strFileName = Hootina::CPath::GetAppPath() + _T("Image\\DefGroupHeadPic.png");
	/*int nTeamIndex, nIndex;
	m_GroupListCtrl.GetItemIndexByID(nGroupCode, nTeamIndex, nIndex);
	m_GroupListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strFileName.c_str(), FALSE);*/

	// 更新最近联系人列表头像
	int nItemCnt = m_RecentListCtrl.GetBuddyItemCount(0);
	for (int i = 0; i < nItemCnt; i++)
	{
		//int nIndex = m_RecentListCtrl.GetBuddyItemID(0, i);

		//C_UI_RecentList* lpRecentList = NULL;// m_FMGClient.GetRecentList();
		//if (lpRecentList != NULL)
		//{
		//	C_UI_RecentInfo* lpRecentInfo = lpRecentList->GetRecent(nIndex);
		//	if (lpRecentInfo != NULL)
		//	{
		//		if (E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_GROUP == lpRecentInfo->m_nType)	// 群
		//		{
		//			CGroupList* lpGroupList = NULL;// m_FMGClient.GetGroupList();
		//			if (lpGroupList != NULL)
		//			{
		//				//C_UI_GroupInfo* lpGroupInfo = lpGroupList->GetGroupById(lpRecentInfo->m_uUserID);
		//				//if (lpGroupInfo != NULL && nGroupCode == lpGroupInfo->m_nGroupCode)
		//				//{
		//				//	m_RecentListCtrl.GetItemIndexByID(nIndex, nTeamIndex, nIndex);
		//				//	m_RecentListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strFileName.c_str(), FALSE);
		//				//	break;
		//				//}
		//			}
		//		}
		//	}
		//}
	}

	NotifyGroupChatDlg(nGroupCode, FMG_MSG_UPDATE_GROUP_HEADPIC, wParam, lParam);// 通知群聊天窗口更新

	return 0;
}


/**
 * @brief 改变在线状态返回消息
 * TODO: 暂时没有使用
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnChangeStatusResult(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bSuccess = (BOOL)wParam;
	long nNewStatus = lParam;
	if (!bSuccess)
	{
		MessageBox(_T("改变在线状态失败！"));
	}
	return 0;
}


/**
 * @brief 响应添加好友请求的结果
 * TODO: 暂时没有使用
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnSendAddFriendRequestResult(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	E_UI_ADD_FRIEND_RESULT eResult = static_cast<E_UI_ADD_FRIEND_RESULT>(wParam);
	if(eResult == E_UI_ADD_FRIEND_RESULT::ADD_FRIEND_FAILED)
	{
		::MessageBox(m_hWnd, _T("网络故障，发送加好友请求失败，请稍后重试！"), g_strAppTitle.c_str(), MB_OK|MB_ICONERROR);
	}

	return 1;
}

/**
 * @brief 响应接收到被添加好友的请求
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnRecvAddFriendRequest(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	
	C_WND_MSG_OperateFriendResult* pResult = (C_WND_MSG_OperateFriendResult*)lParam;
	if(pResult == NULL)
		return (LRESULT)0;

	//当账号是自己的ID，cmd为Apply，账户和昵称为空时，是自己主动发送申请的成功应答，忽略掉
	if( ( pResult->m_uCmd== E_UI_OPERATE_FRIEND::Apply ) &&
	    ( pResult->m_uAccountID== m_userMgr.m_UserInfo.m_uUserIndex) &&
		( *(pResult->m_szAccountName) ==0 ) &&
		( *(pResult->m_szNickName) ==0) )
	{
		delete pResult;
		return 0;
	}
	//m_dwAddFriendTimerId = SetTimer(ADD_FRIEND_REQUEST_TIMER_ID, ::GetCaretBlinkTime(), NULL);
	//WString strFileName(Hootina::CPath::GetAppPath() + _T("Sound\\system.wav"));	// 播放加好友提示音
	//::sndPlaySound(strFileName.c_str(), SND_ASYNC);
	
	{
		
		m_msgList.push_back(pResult);
		DoMsgList();
	}
	DoRecvAddFriendTroyIconCartoon();
	return 1;
}

/**
 * @brief 响应收到好友消息
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnRecvFriendTextMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	C_WND_MSG_BuddyTextMessage * pResult = (C_WND_MSG_BuddyTextMessage*)lParam;
	if (pResult == NULL)
	{
		return (LRESULT)0;
	}
	//播放声音
	//{
	//	WString strFileName(Hootina::CPath::GetAppPath() + _T("Sound\\system.wav"));	// 播放加好友提示音
	//	::sndPlaySound(strFileName.c_str(), SND_ASYNC);
	//}
	//处理接收消息
	DoRecvFriendChatTextMsg(pResult);
	//显示消息盒子
	/*{
		if (m_MsgTipDlg.IsWindow())
		{
			m_MsgTipDlg.AddMsgSender(E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_BUDDY, pResult->m_strSender);
		}
	}*/
	return 0;
}


LRESULT CMainDlg::OnRecvFriendHistoryMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	C_WND_MSG_BuddyTextMessage * pResult = (C_WND_MSG_BuddyTextMessage*)lParam;
	if (pResult == NULL)
	{
		return (LRESULT)0;
	}
	ShowBuddyChatDlg(pResult->m_strSender);
	auto item = m_mapBuddyChatDlg.find(pResult->m_strSender);
	if (item != m_mapBuddyChatDlg.end())
	{
		item->second->OnRecvLogMsg(pResult->m_uiMsg);
	}
	else
	{
		MessageBox(_T("DoRecvFriendChatTextMsg"), _T("DoRecvFriendChatTextMsg"));
	}
	return 0;
}

LRESULT CMainDlg::OnRecvGroupHistoryMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	C_UI_GroupMessage * pResult = (C_UI_GroupMessage*)lParam;
	if (pResult == NULL)
	{
		return (LRESULT)0;
	}
	ShowGroupChatDlg(pResult->m_strGroupId, TRUE);
	auto item = m_mapGroupChatDlg.find(pResult->m_strGroupId);
	if (item != m_mapGroupChatDlg.end())
	{
		item->second->OnRecvGroupLogMsg(pResult);
	}
	else
	{
		MessageBox(_T("DoRecvFriendChatTextMsg"), _T("DoRecvFriendChatTextMsg"));
	}
	return 0;
}

/**
 * @brief 响应添加好友请求
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnAddFriendNotifyRequest(UINT uMsg, WPARAM wParam, LPARAM lParam)// 收到加好友结果的通知的请求
{
	{
		//WString strFileName(Hootina::CPath::GetAppPath() + _T("Sound\\system.wav"));	// 播放加好友提示音
		//::sndPlaySound(strFileName.c_str(), SND_ASYNC);
		
		C_WND_MSG_AddFriendNotifyRequest * pResult = (C_WND_MSG_AddFriendNotifyRequest*)lParam;
		if (pResult == NULL)
		{
			return (LRESULT)0;
		}

		m_msgList.push_back(pResult);
		DoMsgList();
		DoRecvAddFriendTroyIconCartoon();
	}
	return LRESULT(1);
}


/**
 * @brief 响应删除好友结果
 * 
 * @param message 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnDeleteFriendResult(UINT message, WPARAM wParam, LPARAM lParam)
{
	UINT uAccountID = (UINT)lParam;
	E_UI_DELETE_FRIEND_RESULT eResult = static_cast<E_UI_DELETE_FRIEND_RESULT>(wParam);
	if(!IsGroupTarget(uAccountID))
	{
		if (eResult == E_UI_DELETE_FRIEND_RESULT::DELETE_FRIEND_FAILED)
		{
			::MessageBox(m_hWnd, _T("因网络问题，删除好友失败，请稍后重试！"), g_strAppTitle.c_str(), MB_OK | MB_ICONWARNING);
		}

		else if (eResult == E_UI_DELETE_FRIEND_RESULT::DELETE_FRIEND_SUCCESS)
		{
			//删完之后已经没有好友了，则不用重新请求好友列表
			if (m_userMgr.m_BuddyList.GetBuddyTotalCount() == 0)
			{
				::SendMessage(m_hWnd, FMG_MSG_UPDATE_BUDDY_LIST, 0, 0);
			}
			else
			{
				GetFriendList();
				//删除好友成功以后更新好友列表
			}

			//删除成功之后，关闭之前与该好友的聊天窗口
			//Dennis Mask
			//ShowBuddyChatDlg(lParam, FALSE);

            ::MessageBox(m_hWnd, _T("删除好友成功！"), g_strAppTitle.c_str(), MB_OK | MB_ICONINFORMATION);
		}
	}
	else
	{
		::MessageBox(m_hWnd, _T("退群成功！"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		ShowGroupChatDlg("", FALSE);

		GetFriendList();
		::PostMessage(m_hWnd, FMG_MSG_UPDATE_GROUP_LIST, 0, 0);
	}
	
	//主界面已有的消息仍然继续保持闪动
	//TODO: 这段代码有问题：
	//当删除某个有新消息的好友时或者群组时，另外一个有消息的群组仍然闪烁，但另外一个有消息的好友就不闪烁了！！
	//int nTeamIndex, nIndex;
	int nCount = 0;// m_userMgr.m_MsgList.GetMsgSenderCount();
	UINT nUTalkUin = 0;
	C_UI_MessageSender* pSender = NULL;
	for(int i=0; i<nCount; ++i)
	{
		pSender = m_userMgr.m_MsgList.GetMsgSender(i);
		if(pSender != NULL)
		{
			nUTalkUin = pSender->GetSenderId();
			if(nUTalkUin > 0)
			{
				if(!IsGroupTarget(nUTalkUin))
				{
					//m_BuddyListCtrl.GetItemIndexByID(nUTalkUin, nTeamIndex, nIndex);
					//m_BuddyListCtrl.SetBuddyItemHeadFlashAnim(nTeamIndex, nIndex, TRUE);
					//m_BuddyListCtrl.Invalidate(FALSE);
				}
				else
				{
					//m_GroupListCtrl.GetItemIndexByID(nUTalkUin, nTeamIndex, nIndex);
					//m_GroupListCtrl.SetBuddyItemHeadFlashAnim(nTeamIndex, nIndex, TRUE);
				}
			}
		}
	}
	
	
	return (LRESULT)1;
}

//
/**
 * @brief 响应自己的状态改变
 * TODO: 调用位置不明
 * @param message 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnSelfStatusChange(UINT message, WPARAM wParam, LPARAM lParam)
{
	UINT uAccountID = (UINT)wParam;
	if(uAccountID != m_userMgr.m_UserInfo.m_uUserIndex)
		return (LRESULT)0;

	E_UI_ONLINE_STATUS nStatus = static_cast<E_UI_ONLINE_STATUS>(lParam);
	
	//自己在另外的PC端上线
    //处于重连状态，忽略服务器推送的下线通知
    if (m_nMainPanelStatus != MAINPANEL_STATUS_RECONNECTING &&
		nStatus != E_UI_ONLINE_STATUS::STATUS_MOBILE_ONLINE && 
		nStatus != E_UI_ONLINE_STATUS::STATUS_MOBILE_OFFLINE)
	{
		//m_FMGClient.GoOffline();
        //m_FMGClient.Uninit();
        //ShowPanel(FALSE);
		CloseAllDlg();
		
		ShowWindow(SW_SHOW);
		CString strInfo;
		//strInfo.Format(_T("尊敬的%s(%s)\r\n    您的账号在另一台机器上登录，你被迫下线。如果这不是你本人亲自操作，建议你修改密码！"), m_userMgr.m_UserInfo.m_strNickName.c_str(),  m_userMgr.m_UserInfo.m_strAccount.c_str());
		
		//CMobileStatusDlg dlg;
		//dlg.SetWindowTitle(_T("下线通知"));
		//dlg.SetInfoText(strInfo);
		//dlg.EnableAutoDisappear(FALSE);
		//dlg.DoModal(m_hWnd, NULL);

		m_userMgr.ClearUserInfo();
		m_userMgr.m_UserInfo.Reset();

		m_TrayIcon.RemoveIcon();
		memset(&m_stAccountInfo, 0, sizeof(LOGIN_ACCOUNT_INFO));
		StartLogin(FALSE);

		return (LRESULT)1;
	}
	else if(nStatus== E_UI_ONLINE_STATUS::STATUS_MOBILE_ONLINE)
	{
		//CString strInfo;
		//strInfo.Format(_T("尊敬的%s(%s)\r\n    您的账号在手机端上线。\r\n    如果这不是你本人亲自操作，建议你修改密码。"), m_userMgr.m_UserInfo.m_strNickName.c_str(),  m_userMgr.m_UserInfo.m_strAccount.c_str());
		//CMobileStatusDlg dlg;
		//
		//dlg.SetWindowTitle(_T("手机上线提示"));
		//dlg.SetInfoText(strInfo);
		//dlg.EnableAutoDisappear(TRUE);
		//dlg.DoModal(NULL, NULL);
		return (LRESULT)1;
	}
	//else if(nStatus==STATUS_MOBILE_OFFLINE)
	//{
	//	::MessageBox(m_hWnd, _T("您的账号在移动端Flamingo下线。\r\n如果这不是你本人亲自操作，建议你修改密码！"), _T("温馨提示"), MB_OK|MB_ICONINFORMATION);
	//	return (LRESULT)1;
	//}

	return (LRESULT)1;
}

/**
 * @brief 响应显示好友聊天对话框消息,消息由GroupChatDlg发出
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnShowBuddyChatDlg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	std::string * pFriend = (std::string*)(lParam);
	if (nullptr == pFriend)
	{
		return 0;
	}
	std::string strFriend = *pFriend;
	delete pFriend;
	ShowBuddyChatDlg(strFriend);
	return 1;
}


//
//TODO: 需要认真阅读
/**
 * @brief 响应显示或者关闭对话框
 * TODO:计划将此函数拆分不同的功能放到不同的函数中
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnShowOrCloseDlg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nUTalkUin = (UINT)lParam;

	{
		//TODO :不明白代码这样处理是为了什么，先注释掉
		/*if (nUTalkUin == m_userMgr.m_UserInfo.m_uUserID)
		{
			PostMessage(WM_COMMAND, (WPARAM)ID_PIC_HEAD, 0);
			return 1;
		}*/
	}
	int nTeamIndex, nIndex;
	m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nIndex);
	std::string strUserId = m_BuddyListCtrl.GetBuddyItemUserId(nTeamIndex, nIndex);
	switch (uMsg)
	{
	case WM_SHOW_BUDDY_CHAT_DLG:
	{
		int nTeamIndex = 0;
		int nIndex = 0;
		m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nIndex);
		std::string strUserId = m_BuddyListCtrl.GetBuddyItemUserId(nTeamIndex, nIndex);
		ShowBuddyChatDlg(strUserId);
	}break;
		
	case WM_SHOW_GROUP_CHAT_DLG:
	{
		int nTeamIndex = 0;
		int nIndex = 0;
		m_GroupListCtrl.GetCurSelIndex(nTeamIndex, nIndex);
		std::string strUserId = m_GroupListCtrl.GetBuddyItemUserId(nTeamIndex, nIndex);
		ShowGroupChatDlg(strUserId, TRUE);
	}break;
	case WM_SHOW_SESS_CHAT_DLG:
	{
		//ShowSessChatDlg(nGroupCode, nUTalkUin, TRUE);
	}break;
	case WM_SHOW_SYS_GROUP_CHAT_DLG:
	{
		//ShowSysGroupChatDlg(nGroupCode, TRUE);
	}break;
	case WM_SHOW_BUDDY_INFO_DLG:
	{
		//ShowBuddyInfoDlg(nUTalkUin, TRUE);
	}break;
	case WM_SHOW_GMEMBER_INFO_DLG:
	{
		//ShowGMemberInfoDlg(nGroupCode, nUTalkUin, TRUE);
	}break;
	case WM_SHOW_GROUP_INFO_DLG:
	{
		//ShowGroupInfoDlg(nGroupCode, TRUE);
	}break;
	case WM_CLOSE_BUDDY_CHAT_DLG:
	{
		std::string * pStr = reinterpret_cast<std::string*>(lParam);
		CloseBuddyChatDlg(*pStr);
	}break;
	case WM_CLOSE_GROUP_CHAT_DLG:
	{
		//ShowGroupChatDlg(nGroupCode, FALSE);
	}break;
	case WM_CLOSE_SESS_CHAT_DLG:
	{
		//ShowSessChatDlg(nGroupCode, nUTalkUin, FALSE);
	}	break;
	case WM_CLOSE_SYS_GROUP_CHAT_DLG:
	{
		//ShowSysGroupChatDlg(nGroupCode, FALSE);
	}break;
	case WM_CLOSE_BUDDY_INFO_DLG:
	{
		//ShowBuddyInfoDlg(nUTalkUin, FALSE);
	}break;
	case WM_CLOSE_GMEMBER_INFO_DLG:
	{	
		//ShowGMemberInfoDlg(nGroupCode, nUTalkUin, FALSE);
	}break;
	case WM_CLOSE_GROUP_INFO_DLG:
	{
		//ShowGroupInfoDlg(nGroupCode, FALSE);
	}break;
	case WM_SHOW_USER_INFO_DLG:
	{
		//ShowUserInfoDlg(nUTalkUin, TRUE);
	}break;
	case WM_CLOSE_USER_INFO_DLG:
	{
		//ShowUserInfoDlg(nUTalkUin, FALSE);
	}break;
	}
	return 0;
}

//
/**
 * @brief 响应关闭对话框,退出整个程序
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnCloseDlg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CloseDialog(IDOK);
	return (LRESULT)1;
}



/**
 * @brief 删除消息发送者
 * TODO: 确定是否是UI消息
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnDelMsgSender(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	E_UI_CHAT_MSG_TYPE eType = static_cast<E_UI_CHAT_MSG_TYPE>(wParam);
	UINT nSenderId = (UINT)lParam;

	if (m_MsgTipDlg.IsWindow())
	{
		m_MsgTipDlg.ShowWindow(SW_SHOW);
		//m_MsgTipDlg.DelMsgSender(eType, nSenderId);
	}

	if (eType == E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_BUDDY)
	{
		//int nTeamIndex, nIndex;
		//m_BuddyListCtrl.GetItemIndexByID(nSenderId, nTeamIndex, nIndex);
		//m_BuddyListCtrl.SetBuddyItemHeadFlashAnim(nTeamIndex, nIndex, FALSE);
		////更新最近联系人中新消息的数目
		//SendMessage(FMG_MSG_UPDATE_RECENT_LIST, 0, 0);
	}
	else if (eType == E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_GROUP)
	{
		//int nTeamIndex, nIndex;
		//m_GroupListCtrl.GetItemIndexByID(nSenderId, nTeamIndex, nIndex);
		//m_GroupListCtrl.SetBuddyItemHeadFlashAnim(nTeamIndex, nIndex, FALSE);
		////更新最近联系人中新消息的数目
		//SendMessage(FMG_MSG_UPDATE_RECENT_LIST, 0, 0);
	}

	UpdateMsgIcon();
	
	//TODO: 以下代码放到UpdateMsgIcon中去
	C_UI_MessageList* lpMsgList = NULL;// m_FMGClient.GetMessageList();
	if (lpMsgList != NULL && lpMsgList->GetMsgSenderCount() <= 0)
	{
		if (m_MsgTipDlg.IsWindow())
			m_MsgTipDlg.DestroyWindow();
	}

	return 0;
}

/**
 * @brief 取消闪烁
 * TODO: 需要确定是否是UI消息
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnCancelFlash(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_MsgTipDlg.IsWindow())
	{
		::PostMessage(m_MsgTipDlg.m_hWnd, WM_CLOSE, 0, 0);
	}
	//先将任务栏图标恢复成正常状态
	SetTrayIconOnLine();

	return 0;
}

/**
 * @brief 设置托盘区信息为在线
 * 
 */
void CMainDlg::SetTrayIconOnLine()
{
	CString strIconInfo;
	strIconInfo.Format(_T("%s\r\n%s\r\n在线"),
		m_userMgr.m_UserInfo.m_strNickName.data(),
		m_userMgr.m_UserInfo.m_strAccount.data());

	m_TrayIcon.ModifyIcon(m_hAppIcon, strIconInfo);
}

/**
 * @brief 设置托盘区信息为离线
 * 
 */
void CMainDlg::SetTrayIconOffLine()
{
	CString strIconInfo;
	strIconInfo.Format(_T("%s\r\n%s\r\n离线"),
		m_userMgr.m_UserInfo.m_strNickName.c_str(), 
		m_userMgr.m_UserInfo.m_strAccount.c_str());
	m_TrayIcon.ModifyIcon(m_hAppIcon, strIconInfo);
}

/**
 * @brief 修改托盘区字符串
 * 
 * @param strMsg 
 */
void CMainDlg::ChangeTrayIconString(const CString strMsg)
{
	CString strIconInfo;
	strIconInfo.Format(_T("%s\r\n%s\r\n在线"),
		m_userMgr.m_UserInfo.m_strNickName.data(),
		m_userMgr.m_UserInfo.m_strAccount.data());

	m_TrayIcon.ModifyIcon(m_hAppIcon, strIconInfo);
}

/**
 * @brief 关闭好友聊天对话框
 * 
 * @param strFriendName 好友ID
 */
void CMainDlg::CloseBuddyChatDlg(const std::string& strFriendName)
{
	auto findItem = m_mapBuddyChatDlg.find(strFriendName);
	if (findItem != m_mapBuddyChatDlg.end())
	{
		CBuddyChatDlg* lpBuddyChatDlg = findItem->second;
		if (lpBuddyChatDlg != NULL)
		{
			if (lpBuddyChatDlg->IsWindow())
			{
				lpBuddyChatDlg->DestroyWindow();
				delete lpBuddyChatDlg;
			}
		}
		m_mapBuddyChatDlg.erase(findItem);
	}
}

/**
 * @brief 显示好友聊天对话框
 * 
 * @param strUserId 好友ID
 */
void CMainDlg::ShowBuddyChatDlg(const std::string& strUserId)
{
	auto findItem = m_mapBuddyChatDlg.find(strUserId);
	if (findItem != m_mapBuddyChatDlg.end())
	{
		CBuddyChatDlg* lpBuddyChatDlg = findItem->second;
		if (lpBuddyChatDlg != NULL)
		{
			if (!lpBuddyChatDlg->IsWindow())
			{
				lpBuddyChatDlg->Create(NULL);
			}
			//lpBuddyChatDlg->ShowWindow(SW_SHOW);
			lpBuddyChatDlg->ShowWindow(SW_RESTORE);
			::SetForegroundWindow(lpBuddyChatDlg->m_hWnd);
		}
	}
	else
	{
		//首次走这个逻辑
		CBuddyChatDlg* lpBuddyChatDlg = new CBuddyChatDlg;
		if (lpBuddyChatDlg != NULL)
		{
			long nLastWidth = m_userCfg.GetChatDlgWidth();
			long nLastHeight = m_userCfg.GetChatDlgHeight();
			lpBuddyChatDlg->m_lpFaceList = &m_FaceList;
			lpBuddyChatDlg->m_lpCascadeWinManager = &m_CascadeWinManager;
			lpBuddyChatDlg->m_hMainDlg = m_hWnd;

			lpBuddyChatDlg->m_strChatUserName = m_netProto->UserName();
			m_mapBuddyChatDlg[strUserId] = lpBuddyChatDlg;

			UserBaseInfo friendInfo = m_netProto->GetFriendInfoById(strUserId);
			lpBuddyChatDlg->m_strFriendId = strUserId;
			lpBuddyChatDlg->m_strFriendName = friendInfo.m_strNickName;

			lpBuddyChatDlg->Create(NULL);
			lpBuddyChatDlg->ShowWindow(SW_SHOW);
			lpBuddyChatDlg->SetWindowPos(NULL, 0, 0, nLastWidth, nLastHeight, SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW);
			lpBuddyChatDlg->DataMatchInit();
			
			SendMessage(lpBuddyChatDlg->m_hWnd, WM_SIZE, (WPARAM)SIZE_RESTORED, (LPARAM)MAKELONG(nLastWidth, nLastHeight));
			::SetForegroundWindow(lpBuddyChatDlg->m_hWnd);
		}
	}
}

//显示好友聊天对话框
/*void CMainDlg::ShowBuddyChatDlg(UINT nUTalkUin, BOOL bShow, BOOL bShakeWindowMsg, UINT nMsgID)
{
	if (nUTalkUin == 0)
		return;

	if (bShow)
	{
		/*std::map<UINT, CBuddyChatDlg*>::iterator iter;
		iter = m_mapBuddyChatDlg.find(nUTalkUin);
		if (iter != m_mapBuddyChatDlg.end())
		{
			CBuddyChatDlg* lpBuddyChatDlg = iter->second;
			if (lpBuddyChatDlg != NULL)
			{
				if (!lpBuddyChatDlg->IsWindow())
					lpBuddyChatDlg->Create(NULL);
				//lpBuddyChatDlg->ShowWindow(SW_SHOW);
				lpBuddyChatDlg->ShowWindow(SW_RESTORE);
				::SetForegroundWindow(lpBuddyChatDlg->m_hWnd);
				if(bShakeWindowMsg)
				{
					::Sleep(3000);
					ShakeWindow(lpBuddyChatDlg->m_hWnd, 1);
					lpBuddyChatDlg->OnRecvMsg(nUTalkUin, nMsgID);
				}
			}
		}
		else
		{
			//首次走这个逻辑
			CBuddyChatDlg* lpBuddyChatDlg = new CBuddyChatDlg;
			if (lpBuddyChatDlg != NULL)
			{
				long nLastWidth = m_userCfg.GetChatDlgWidth();
				long nLastHeight = m_userCfg.GetChatDlgHeight();
				//lpBuddyChatDlg->m_lpFMGClient = &m_FMGClient;
				lpBuddyChatDlg->m_lpFaceList = &m_FaceList;
				lpBuddyChatDlg->m_lpCascadeWinManager = &m_CascadeWinManager;
				lpBuddyChatDlg->m_hMainDlg = m_hWnd;
				lpBuddyChatDlg->m_nUTalkUin = nUTalkUin;
				//lpBuddyChatDlg->m_strUserName = 
                lpBuddyChatDlg->m_UserId = nUTalkUin;
                lpBuddyChatDlg->m_LoginUserId = m_userMgr.m_UserInfo.m_uUserID;
				//lpBuddyChatDlg->m_strFriendName = m_BuddyListCtrl.GetBuddyItemUserName()
				m_mapBuddyChatDlg[nUTalkUin] = lpBuddyChatDlg;
				lpBuddyChatDlg->Create(NULL);
				lpBuddyChatDlg->ShowWindow(SW_SHOW);
				lpBuddyChatDlg->SetWindowPos(NULL, 0, 0, nLastWidth, nLastHeight, SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW);
				SendMessage(lpBuddyChatDlg->m_hWnd, WM_SIZE, (WPARAM)SIZE_RESTORED, (LPARAM)MAKELONG(nLastWidth, nLastHeight));
				::SetForegroundWindow(lpBuddyChatDlg->m_hWnd);
				if(bShakeWindowMsg)
				{
					ShakeWindow(lpBuddyChatDlg->m_hWnd, 1);
					lpBuddyChatDlg->OnRecvMsg(nUTalkUin, nMsgID);
				}
			}
		}
	}
	else
	{
		/*std::map<UINT, CBuddyChatDlg*>::iterator iter;
		iter = m_mapBuddyChatDlg.find(nUTalkUin);
		if (iter != m_mapBuddyChatDlg.end())
		{
			CBuddyChatDlg* lpBuddyChatDlg = iter->second;
			if (lpBuddyChatDlg != NULL)
			{
				if (lpBuddyChatDlg->IsWindow())
				{
					lpBuddyChatDlg->DestroyWindow();
					delete lpBuddyChatDlg;
				}
				
			}
			m_mapBuddyChatDlg.erase(iter);
		}
		
	}
}*/



/**
 * @brief 显示群聊天对话框
 * TODO: BOOL类型待优化
 * @param strGroupId 群组唯一标识
 * @param bShow 
 */
void CMainDlg::ShowGroupChatDlg(const std::string strGroupId, BOOL bShow)
{
	if (strGroupId.empty() )
	{
		return;
	}

	if (bShow)
	{
		auto iter = m_mapGroupChatDlg.find(strGroupId);
		if (iter != m_mapGroupChatDlg.end())
		{
			CGroupChatDlg* lpGroupChatDlg = iter->second;
			if (lpGroupChatDlg != NULL)
			{
				if (!lpGroupChatDlg->IsWindow())
				{
					lpGroupChatDlg->Create(NULL);
				}
				lpGroupChatDlg->ShowWindow(SW_RESTORE);
				::SetForegroundWindow(lpGroupChatDlg->m_hWnd);
			}
		}
		else
		{
			CreateGroupChatDlg(strGroupId);
			/*CGroupChatDlg* lpGroupChatDlg = new CGroupChatDlg;
			if (lpGroupChatDlg != NULL)
			{
				long nLastWidth = m_userCfg.GetGroupDlgWidth();
				long nLastHeight  = m_userCfg.GetGroupDlgHeight();
				lpGroupChatDlg->m_lpFaceList = &m_FaceList;
				lpGroupChatDlg->m_lpCascadeWinManager = &m_CascadeWinManager;
				lpGroupChatDlg->m_hMainDlg = m_hWnd;
				lpGroupChatDlg->m_strGroupId = strGroupId;
				m_mapGroupChatDlg[strGroupId] = lpGroupChatDlg;
				lpGroupChatDlg->m_netProto = m_netProto;
				lpGroupChatDlg->Create(NULL);
				lpGroupChatDlg->ShowWindow(SW_SHOW);
				lpGroupChatDlg->SetWindowPos(NULL, 0, 0, nLastWidth, nLastHeight, SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW);
				SendMessage(lpGroupChatDlg->m_hWnd, WM_SIZE, (WPARAM)SIZE_RESTORED, (LPARAM)MAKELONG(nLastWidth, nLastHeight));
				::SetForegroundWindow(lpGroupChatDlg->m_hWnd);
			}*/
		}
	}
	else
	{
		std::string strGroupId = "";
		auto iter = m_mapGroupChatDlg.find(strGroupId);
		if (iter != m_mapGroupChatDlg.end())
		{
			CGroupChatDlg* lpGroupChatDlg = iter->second;
			if (lpGroupChatDlg != NULL)
			{
				if (lpGroupChatDlg->IsWindow())
				{
					lpGroupChatDlg->DestroyWindow();
				}
				delete lpGroupChatDlg;
			}
			m_mapGroupChatDlg.erase(iter);
		}
	}
}


/**
 * @brief 显示会话对话框
 * TODO:是否可以删除
 * @param nGroupCode 
 * @param nUTalkUin 
 * @param bShow 
 */
void CMainDlg::ShowSessChatDlg(UINT nGroupCode, UINT nUTalkUin, BOOL bShow)
{
	//if (nUTalkUin == 0)
	//{
	//	return;
	//}

	//if (bShow)
	//{
	//	std::map<UINT, CSessChatDlg*>::iterator iter;
	//	iter = m_mapSessChatDlg.find(nUTalkUin);
	//	if (iter != m_mapSessChatDlg.end())
	//	{
	//		CSessChatDlg* lpSessChatDlg = iter->second;
	//		if (lpSessChatDlg != NULL)
	//		{
	//			if (!lpSessChatDlg->IsWindow())
	//			{
	//				lpSessChatDlg->Create(NULL);
	//			}
	//			lpSessChatDlg->ShowWindow(SW_SHOW);
	//			::SetForegroundWindow(lpSessChatDlg->m_hWnd);
	//		}
	//	}
	//	else
	//	{
	//		CSessChatDlg* lpSessChatDlg = new CSessChatDlg;
	//		if (lpSessChatDlg != NULL)
	//		{
	//			//lpSessChatDlg->m_lpFMGClient = &m_FMGClient;
	//			lpSessChatDlg->m_lpFaceList = &m_FaceList;
	//			lpSessChatDlg->m_lpCascadeWinManager = &m_CascadeWinManager;
	//			lpSessChatDlg->m_hMainDlg = m_hWnd;
	//			lpSessChatDlg->m_nGroupCode = nGroupCode;
	//			lpSessChatDlg->m_nUTalkUin = nUTalkUin;
	//			m_mapSessChatDlg[nUTalkUin] = lpSessChatDlg;
	//			lpSessChatDlg->Create(NULL);
	//			lpSessChatDlg->ShowWindow(SW_SHOW);
	//			::SetForegroundWindow(lpSessChatDlg->m_hWnd);
	//		}
	//	}
	//}
	//else
	//{
	//	std::map<UINT, CSessChatDlg*>::iterator iter;
	//	iter = m_mapSessChatDlg.find(nUTalkUin);
	//	if (iter != m_mapSessChatDlg.end())
	//	{
	//		CSessChatDlg* lpSessChatDlg = iter->second;
	//		if (lpSessChatDlg != NULL)
	//		{
	//			if (lpSessChatDlg->IsWindow())
	//				lpSessChatDlg->DestroyWindow();
	//			delete lpSessChatDlg;
	//		}
	//		m_mapSessChatDlg.erase(iter);
	//	}
	//}
}



/**
 * @brief 显示系统聊天对话框
 * TODO: 需要确定系统消息类型
 * @param nGroupCode 
 * @param bShow 
 */
void CMainDlg::ShowSysGroupChatDlg(UINT nGroupCode, BOOL bShow)
{
	/*CString strText, strGroupName, strAdminName, strAdminUin, strMsg;
	UINT nGroupNumber;

	CGroupList* lpGroupList = m_FMGClient.GetGroupList();
	C_UI_MessageList* lpMsgList = m_FMGClient.GetMessageList();
	if (NULL == lpGroupList || NULL == lpMsgList)
		return;

	C_UI_GroupInfo* lpGroupInfo = lpGroupList->GetGroupByCode(nGroupCode);
	C_UI_MessageSender* lpMsgSender = lpMsgList->GetMsgSender(long_SYSGROUP, nGroupCode);
	if (NULL == lpMsgSender)
		return;

	if (lpGroupInfo != NULL)
		strGroupName = lpGroupInfo->m_strName.c_str();

	int nMsgCnt = lpMsgSender->GetMsgCount();
	for (int i = 0; i < nMsgCnt; i++)
	{
		C_UI_SysGroupMessage* lpSysGroupMsg = lpMsgSender->GetSysGroupMsg(i);
		if (lpSysGroupMsg != NULL)
		{
			strAdminName = lpSysGroupMsg->m_strAdminNickName.c_str();
			strAdminUin = lpSysGroupMsg->m_strAdminUin.c_str();
			strMsg = lpSysGroupMsg->m_strMsg.c_str();
			nGroupNumber = lpSysGroupMsg->m_nGroupNumber;

			if (lpSysGroupMsg->m_strSubType == _T("group_request_join_agree"))
			{
				LPCTSTR lpFmt = _T("管理员已同意您加入群%s(%u)");
				strText.Format(lpFmt, strGroupName, nGroupNumber);
				MessageBox(strText, _T("提示"), MB_OK);
			}
			else if (lpSysGroupMsg->m_strSubType == _T("group_request_join_deny"))
			{
				LPCTSTR lpFmt = _T("管理员已拒绝您加入群%s(%u)。拒绝理由：%s");
				strText.Format(lpFmt, strGroupName, nGroupNumber, strMsg);
				MessageBox(strText, _T("提示"), MB_OK);
			}
			else if (lpSysGroupMsg->m_strSubType == _T("group_leave"))
			{
				LPCTSTR lpFmt = _T("%s(%u)管理员%s(%s)已将您移除出该群。");
				strText.Format(lpFmt, strGroupName, nGroupNumber, strAdminName, strAdminUin);
				MessageBox(strText, _T("提示"), MB_OK);
			}
		}
	}*/
	//lpMsgList->DelMsgSender(long_SYSGROUP, nGroupCode);
	//OnDelMsgSender(NULL, long_SYSGROUP, nGroupCode);
}



/**
 * @brief 显示用户信息对话框
 * TODO: 确定是否可以删除
 * @param nUTalkUin 
 * @param bShow 
 */
void CMainDlg::ShowUserInfoDlg(UINT nUTalkUin, BOOL bShow)
{
	if (nUTalkUin == 0)
	{
		return;
	}

	if (!bShow)
	{
		DestroyWindow();
	}
}

LRESULT CMainDlg::OnRecvFriendFileTransResultNotifyMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	C_WND_MSG_FriendFileResult * pMsg = (C_WND_MSG_FriendFileResult*)(lParam);
	if (pMsg)
	{
		auto item = m_mapBuddyChatDlg.find(pMsg->m_szFriendId);
		if (item != m_mapBuddyChatDlg.end())
		{
			item->second->OnFileTransResult(pMsg);
		}
	}
	return 0;
}
/**
 * @brief 显示好友信息对话框
 * TODO: 显示好友资料对话框
 * @param strFriendId 
 * @param bShow 
 */
void CMainDlg::ShowBuddyInfoDlg(const std::string strFriendId, BOOL bShow)
{
	if (bShow)
	{
		auto item = m_mapBuddyInfoDlg.find(strFriendId);
		if (item != m_mapBuddyInfoDlg.end())
		{
			CBuddyInfoDlg* lpBuddyInfoDlg = item->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (!lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->Create(NULL);
				lpBuddyInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyInfoDlg->m_hWnd);
			}
		}
		else
		{
			CBuddyInfoDlg* lpBuddyInfoDlg = new CBuddyInfoDlg;
			// 好友资料对话框
			if (strFriendId != m_netProto->UserId())
			{
				//TODO  此处是否需要优化
				WString strFriendNickName = EncodeUtil::Utf8ToUnicode(m_netProto->GetFriendInfoById(strFriendId).m_strNickName);
				strFriendNickName += _T("的资料");
				lpBuddyInfoDlg->SetWindowTitle(strFriendNickName.c_str());
			}

			if (lpBuddyInfoDlg != NULL)
			{
				m_mapBuddyInfoDlg[strFriendId] = lpBuddyInfoDlg;
				//lpBuddyInfoDlg->m_lpFMGClient = &m_FMGClient;
				//lpBuddyInfoDlg-> = nUTalkUin;
				lpBuddyInfoDlg->Create(NULL);
				lpBuddyInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyInfoDlg->m_hWnd);
			}
		}
	}
	else
	{
		/*std::map<UINT, CBuddyInfoDlg*>::iterator iter;
		iter = m_mapBuddyInfoDlg.find(nUTalkUin);
		if (iter != m_mapBuddyInfoDlg.end())
		{
			CBuddyInfoDlg* lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->DestroyWindow();
				delete lpBuddyInfoDlg;
			}
			m_mapBuddyInfoDlg.erase(iter);
		}*/
	}
}

//
/**
 * @brief 显示群成员信息的对话框
 * 
 * TODO: 优化掉bool变量
 * @param nGroupCode 群组唯一标识
 * @param nUTalkUin 用户唯一标识
 * @param bShow 
 */
void CMainDlg::ShowGMemberInfoDlg(UINT nGroupCode, UINT nUTalkUin, BOOL bShow)
{
	if (0 == nGroupCode || 0 == nUTalkUin)
		return;

	if (bShow)
	{
		CGMemberInfoMapKey key;
		key.m_nGroupCode = nGroupCode;
		key.m_nUTalkUin = nUTalkUin;
		std::map<CGMemberInfoMapKey, CBuddyInfoDlg*>::iterator iter;
		iter = m_mapGMemberInfoDlg.find(key);
		if (iter != m_mapGMemberInfoDlg.end())
		{
			CBuddyInfoDlg* lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (!lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->Create(NULL);
				lpBuddyInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyInfoDlg->m_hWnd);
			}
		}
		else
		{
			CBuddyInfoDlg* lpBuddyInfoDlg = new CBuddyInfoDlg;
			if (lpBuddyInfoDlg != NULL)
			{
				m_mapGMemberInfoDlg[key] = lpBuddyInfoDlg;
				//lpBuddyInfoDlg->m_lpFMGClient = &m_FMGClient;
				//lpBuddyInfoDlg->m_hMainDlg = m_hWnd;
				lpBuddyInfoDlg->m_nUTalkUin = nUTalkUin;
				//lpBuddyInfoDlg->m_bIsGMember = TRUE;
				//lpBuddyInfoDlg->m_nGroupCode = nGroupCode;
				lpBuddyInfoDlg->Create(NULL);
				lpBuddyInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyInfoDlg->m_hWnd);
			}
		}
	}
	else
	{
		CGMemberInfoMapKey key;
		key.m_nGroupCode = nGroupCode;
		key.m_nUTalkUin = nUTalkUin;
		std::map<CGMemberInfoMapKey, CBuddyInfoDlg*>::iterator iter;
		iter = m_mapGMemberInfoDlg.find(key);
		if (iter != m_mapGMemberInfoDlg.end())
		{
			CBuddyInfoDlg* lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->DestroyWindow();
				delete lpBuddyInfoDlg;
			}
			m_mapGMemberInfoDlg.erase(iter);
		}
	}
}


/**
 * @brief 显示群信息的对话框
 * TODO: 后期优化掉bool变量
 * @param strGroupId 群组唯一标识
 * @param bShow 
 */
void CMainDlg::ShowGroupInfoDlg(const std::string& strGroupId, BOOL bShow)
{
	if (bShow)
	{
		
		auto iter = m_mapGroupInfoDlg.find(strGroupId);
		if (iter != m_mapGroupInfoDlg.end())
		{
			CGroupInfoDlg* lpGroupInfoDlg = iter->second;
			if (lpGroupInfoDlg != NULL)
			{
				if (!lpGroupInfoDlg->IsWindow())
					lpGroupInfoDlg->Create(NULL);
				lpGroupInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpGroupInfoDlg->m_hWnd);
			}
		}
		else
		{
			CGroupInfoDlg* lpGroupInfoDlg = new CGroupInfoDlg;
			if (lpGroupInfoDlg != NULL)
			{
				m_mapGroupInfoDlg[strGroupId] = lpGroupInfoDlg;
//				lpGroupInfoDlg->m_lpFMGClient = &m_FMGClient;
				lpGroupInfoDlg->m_hMainDlg = m_hWnd;
				lpGroupInfoDlg->m_strGroupId = strGroupId;
				lpGroupInfoDlg->Create(NULL);
				lpGroupInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpGroupInfoDlg->m_hWnd);
			}
		}
	}
	else
	{
		auto iter = m_mapGroupInfoDlg.find(strGroupId);
		if (iter != m_mapGroupInfoDlg.end())
		{
			CGroupInfoDlg* lpGroupInfoDlg = iter->second;
			if (lpGroupInfoDlg != NULL)
			{
				if (lpGroupInfoDlg->IsWindow())
					lpGroupInfoDlg->DestroyWindow();
				delete lpGroupInfoDlg;
			}
			m_mapGroupInfoDlg.erase(iter);
		}
	}
}

// 
/**
 * @brief 通知好友聊天窗口更新
 * 
 * TODO:待更新
 * @param nUTalkUin 好友唯一标识
 * @param uMsg 
 */
void CMainDlg::NotifyBuddyChatDlg(UINT nUTalkUin, UINT uMsg)
{
	/*std::map<UINT, CBuddyChatDlg*>::iterator iter;
	iter = m_mapBuddyChatDlg.find(nUTalkUin);
	if (iter != m_mapBuddyChatDlg.end())
	{
		CBuddyChatDlg* lpBuddyChatDlg = iter->second;
		if (lpBuddyChatDlg != NULL && lpBuddyChatDlg->IsWindow())
		{
			switch (uMsg)
			{
			case FMG_MSG_UPDATE_BUDDY_NUMBER:
				lpBuddyChatDlg->OnUpdateBuddyNumber();
				break;

			case FMG_MSG_UPDATE_BUDDY_SIGN:
				lpBuddyChatDlg->OnUpdateBuddySign();
				break;

			case FMG_MSG_UPDATE_BUDDY_HEADPIC:
				lpBuddyChatDlg->OnUpdateBuddyHeadPic();
				break;
			}
		}
	}*/
}

// 

/**
 * @brief 通知群聊天窗口更新
 * TODO: 待更新
 * @param nGroupCode 群组唯一标识
 * @param uMsg 消息类型
 * @param wParam 
 * @param lParam 
 */
void CMainDlg::NotifyGroupChatDlg(UINT nGroupCode, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//std::map<UINT, CGroupChatDlg*>::iterator iter;
	std::string strGroupId;
	auto iter = m_mapGroupChatDlg.find(strGroupId);
	if (iter != m_mapGroupChatDlg.end())
	{
		CGroupChatDlg* lpGroupChatDlg = iter->second;
		if (lpGroupChatDlg != NULL && lpGroupChatDlg->IsWindow())
		{
			switch (uMsg)
			{
			case FMG_MSG_UPDATE_GROUP_INFO:
			{
				lpGroupChatDlg->OnUpdateGroupInfo();
			}break;

			case FMG_MSG_UPDATE_GROUP_NUMBER:
			{
				lpGroupChatDlg->OnUpdateGroupNumber();
			}break;

			case FMG_MSG_UPDATE_GROUP_HEADPIC:
			{
				lpGroupChatDlg->OnUpdateGroupHeadPic(); 
			}break;

			case FMG_MSG_UPDATE_GMEMBER_NUMBER:
			{
				lpGroupChatDlg->OnUpdateGMemberNumber(wParam, lParam);
			}break;

			case FMG_MSG_UPDATE_GMEMBER_HEADPIC:
			{
				lpGroupChatDlg->OnUpdateGMemberHeadPic(wParam, lParam);
			}break;
			}
		}
	}
}


/**
 * @brief 通知临时会话聊天窗口更新
 * TODO: 待完善
 * @param nUTalkUin 用户唯一标识
 * @param uMsg 消息类型
 */
void CMainDlg::NotifySessChatDlg(UINT nUTalkUin, UINT uMsg)
{
	/*std::map<UINT, CSessChatDlg*>::iterator iter;
	iter = m_mapSessChatDlg.find(nUTalkUin);
	if (iter != m_mapSessChatDlg.end())
	{
		CSessChatDlg* lpSessChatDlg = iter->second;
		if (lpSessChatDlg != NULL && lpSessChatDlg->IsWindow())
		{
 			switch (uMsg)
 			{
			case FMG_MSG_UPDATE_GMEMBER_NUMBER:
				lpSessChatDlg->OnUpdateGMemberNumber();
				break;
 			case FMG_MSG_UPDATE_GMEMBER_HEADPIC:
 				lpSessChatDlg->OnUpdateGMemberHeadPic();
 				break;
 			}
		}
	}*/
}


/**
 * @brief 通知好友信息窗口更新
 * 
 * @param nUTalkUin 待完善
 * @param uMsg 
 */
void CMainDlg::NotifyBuddyInfoDlg(UINT nUTalkUin, UINT uMsg)
{
	if(nUTalkUin == m_userMgr.m_UserInfo.m_uUserIndex)
	{
		if (m_LogonUserInfoDlg.IsWindow())
		{
			m_LogonUserInfoDlg.UpdateCtrlData();
		}

		return;
	}
	
	//std::map<UINT, CBuddyInfoDlg*>::iterator iter;
	//iter = m_mapBuddyInfoDlg.find(nUTalkUin);
	//if (iter != m_mapBuddyInfoDlg.end())
	//{
	//	CBuddyInfoDlg* lpBuddyInfoDlg = iter->second;
	//	if (lpBuddyInfoDlg != NULL && lpBuddyInfoDlg->IsWindow())
	//	{
	//		switch (uMsg)
	//		{
	//			case FMG_MSG_UPDATE_BUDDY_INFO:
	//			{
	//				lpBuddyInfoDlg->UpdateCtrls();
	//			}break;

	//			case FMG_MSG_UPDATE_BUDDY_NUMBER:
	//			{
	//				//lpBuddyInfoDlg->OnUpdateBuddyNumber();
	//			}break;

	//			case FMG_MSG_UPDATE_BUDDY_SIGN:
	//			{	//lpBuddyInfoDlg->OnUpdateBuddySign();


	//			}break;

	//			default:
	//			{

	//			}break;
	//		}
	//	}
	//}
}


/**
 * @brief 通知群成员信息窗口更新
 * TODO: 待修改
 * @param nGroupCode 群组唯一标志
 * @param nUTalkUin 
 * @param uMsg 
 */
void CMainDlg::NotifyGMemberInfoDlg(UINT nGroupCode, UINT nUTalkUin, UINT uMsg)
{
	CGMemberInfoMapKey key;
	key.m_nGroupCode = nGroupCode;
	key.m_nUTalkUin = nUTalkUin;
	std::map<CGMemberInfoMapKey, CBuddyInfoDlg*>::iterator iter;
	iter = m_mapGMemberInfoDlg.find(key);
	if (iter != m_mapGMemberInfoDlg.end())
	{
		CBuddyInfoDlg* lpBuddyInfoDlg = iter->second;
		if (lpBuddyInfoDlg != NULL && lpBuddyInfoDlg->IsWindow())
		{
			switch (uMsg)
			{
			case FMG_MSG_UPDATE_GMEMBER_INFO:
				//lpBuddyInfoDlg->OnUpdateBuddyInfo();
				break;

			case FMG_MSG_UPDATE_GMEMBER_NUMBER:
				//lpBuddyInfoDlg->OnUpdateBuddyNumber();
				break;

			case FMG_MSG_UPDATE_GMEMBER_SIGN:
				//lpBuddyInfoDlg->OnUpdateBuddySign();
				break;
			}
		}
	}
}

// 
/**
 * @brief 通知群信息窗口更新
 * TODO: 需要确定使用的地方
 * 
 * @param nGroupCode 群ID
 * @param uMsg 
 */
void CMainDlg::NotifyGroupInfoDlg(UINT nGroupCode, UINT uMsg)
{
	/*std::map<UINT, CGroupInfoDlg*>::iterator iter;
	iter = m_mapGroupInfoDlg.find(nGroupCode);
	if (iter != m_mapGroupInfoDlg.end())
	{
		CGroupInfoDlg* lpGroupInfoDlg = iter->second;
		if (lpGroupInfoDlg != NULL && lpGroupInfoDlg->IsWindow())
		{
			switch (uMsg)
			{
			case FMG_MSG_UPDATE_GROUP_INFO:
				lpGroupInfoDlg->OnUpdateGroupInfo();
				break;

			case FMG_MSG_UPDATE_GROUP_NUMBER:
				lpGroupInfoDlg->OnUpdateGroupNumber();
				break;
			}
		}
	}*/
}


/**
 * @brief 更新好友列表,主界面的好友列表(UI)
 * 
 * @param uAccountID 
 */
void CMainDlg::UpdateBuddyTreeCtrl()
{
	C_UI_BuddyList * lpBuddyList = nullptr;
	
	//GetBudyList
	{
		if (nullptr != m_netProto)
		{
			lpBuddyList = &(m_netProto->m_BuddyList);
		}
		if (nullptr == lpBuddyList)
		{
			return;
		}
	}



	int nBuddyTeamCount = lpBuddyList->GetBuddyTeamCount();
	//记录上一次好友分组控件是处于收起还是展开状态
	std::vector<BOOL> aryTeamExpandStatus;
	aryTeamExpandStatus.resize(nBuddyTeamCount);
	for(long i=0; i<nBuddyTeamCount; ++i)
		aryTeamExpandStatus[i] = m_BuddyListCtrl.IsBuddyTeamExpand(i);

	m_BuddyListCtrl.DelAllItems();

	BOOL bShowSimpleProfile = m_userCfg.IsEnableSimpleProfile();
	auto nNameStyle = m_userCfg.GetNameStyle();
					

	//用户头像
	CString strThumbPath;
	BOOL bMale = TRUE;
	int nBuddyCount = 0;
	int nValidBuddyCount = 0;
	int nOnlineBuddyCount = 0;
	int nTeamIndex = 0;

	for (int i = 0; i < nBuddyTeamCount; i++)
	{
        nValidBuddyCount = 0;
        nBuddyCount = lpBuddyList->GetBuddyCount(i);
		nOnlineBuddyCount = lpBuddyList->GetOnlineBuddyCount(i);
		nTeamIndex = 0;

		C_UI_BuddyTeamInfo* lpBuddyTeamInfo = lpBuddyList->GetBuddyTeam(i);
		if (lpBuddyTeamInfo != NULL)
		{
			nTeamIndex = m_BuddyListCtrl.AddBuddyTeam(i);
			m_BuddyListCtrl.SetBuddyTeamName(nTeamIndex, lpBuddyTeamInfo->m_strName.c_str());
			m_BuddyListCtrl.SetBuddyTeamCurCnt(nTeamIndex, nOnlineBuddyCount);
			m_BuddyListCtrl.SetBuddyTeamExpand(nTeamIndex, aryTeamExpandStatus[i]);
			m_BuddyListCtrl.SetBuddyTeamID(nTeamIndex, lpBuddyTeamInfo->m_strTeamId);
		}

		for (int j = 0; j < nBuddyCount; j++)
		{
			C_UI_BuddyInfo* lpBuddyInfo = lpBuddyList->GetBuddy(i, j);
			if (lpBuddyInfo != NULL)
			{
				if (lpBuddyInfo->m_strAccount.empty())
				{
					continue;
				}

				CString strUTalkNum;
				//strUTalkNum.Format(_T("%u"),lpBuddyInfo->m_nUTalkNum);
				strUTalkNum.Format(_T("%s"), lpBuddyInfo->m_strAccount.c_str());

				bMale = lpBuddyInfo->m_nGender != 0 ? FALSE : TRUE;
				BOOL bGray = FALSE;

				if (lpBuddyInfo->m_nStatus == E_UI_ONLINE_STATUS::STATUS_OFFLINE)
				{
					bGray = TRUE;
				}
	
				if (lpBuddyInfo->m_nFace >= (UINT)USER_THUMB_COUNT)
				{
					lpBuddyInfo->m_nFace = 0;
				}
				
				if (lpBuddyInfo->m_bUseCustomFace && lpBuddyInfo->m_bCustomFaceAvailable)
				{
					strThumbPath.Format(_T("%s%d.png"), m_userMgr.GetCustomUserThumbFolder().c_str(), lpBuddyInfo->m_uUserIndex);
					if(!Hootina::CPath::IsFileExist(strThumbPath))
						strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, lpBuddyInfo->m_nFace);
				}
				else
				{
					strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, lpBuddyInfo->m_nFace);
				}

				//int nIndex = m_BuddyListCtrl.AddBuddyItem(nTeamIndex, lpBuddyInfo->m_nUTalkUin);
				int nIndex = m_BuddyListCtrl.AddBuddyItem(nTeamIndex, lpBuddyInfo->m_strUserId);

				m_BuddyListCtrl.SetBuddyItemNickName(nTeamIndex, nIndex, lpBuddyInfo->m_strNickName.c_str());
				m_BuddyListCtrl.SetBuddyItemMarkName(nTeamIndex, nIndex, lpBuddyInfo->m_strMarkName.c_str());

				//清爽资料不显示个性签名
				if (bShowSimpleProfile)
				{
					m_BuddyListCtrl.SetBuddyItemSign(nTeamIndex, nIndex, lpBuddyInfo->m_strSign.c_str(), FALSE);
				}
				else
				{
					m_BuddyListCtrl.SetBuddyItemSign(nTeamIndex, nIndex, lpBuddyInfo->m_strSign.c_str(), TRUE);
				}

				m_BuddyListCtrl.SetBuddyItemGender(nTeamIndex, nIndex, bMale);
				m_BuddyListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strThumbPath, bGray);
				
				//SetBuddyItemOnline最后一个参数是上线动画
				m_BuddyListCtrl.SetBuddyItemOnline(nTeamIndex, nIndex, !bGray,FALSE);
                /*if (lpBuddyInfo->m_nStatus == online_type_android_cellular || lpBuddyInfo->m_nStatus == online_type_android_wifi)
				{
					strThumbPath.Format(_T("%sImage\\mobile_online.png"), g_szHomePath);
					m_BuddyListCtrl.SetBuddyItemMobilePic(nTeamIndex, nIndex, strThumbPath, TRUE);
				}
                else if (lpBuddyInfo->m_nStatus == online_type_offline || lpBuddyInfo->m_nStatus == online_type_pc_invisible)
                {
                    strThumbPath.Format(_T("%sImage\\mobile_online.png"), g_szHomePath);
                    m_BuddyListCtrl.SetBuddyItemMobilePic(nTeamIndex, nIndex, strThumbPath, FALSE);
                }*/
                    
				++nValidBuddyCount;
			}
		}
		m_BuddyListCtrl.SetBuddyTeamMaxCnt(nTeamIndex, nValidBuddyCount);
		
		nTeamIndex++;
	}

	if (m_BuddyListCtrl.IsWindowVisible())
		m_BuddyListCtrl.Invalidate();
}

/**
 * @brief 更新群组聊天树形结构(UI)
 * 
 */
void CMainDlg::UpdateGroupTreeCtrl()
{
	RUN_HERE(m_loger)
	CGroupList* lpGroupList = m_netProto->GetGroupList();
	if (NULL == lpGroupList)
	{
		RUN_HERE(m_loger)
		return;
	}

	m_GroupListCtrl.DelAllItems();

	int nTeamIndex = m_GroupListCtrl.AddBuddyTeam(0);
	m_GroupListCtrl.SetBuddyTeamName(nTeamIndex, _T("我的群"));
	m_GroupListCtrl.SetBuddyTeamExpand(nTeamIndex, TRUE);

	int nActualGroupCount = 0;
	int nGroupCount = lpGroupList->GetGroupCount();
	for (int i = 0; i < nGroupCount; i++)
	{
		C_UI_GroupInfo* lpGroupInfo = lpGroupList->GetGroup(i);
		if (lpGroupInfo == NULL || lpGroupInfo->m_strName.empty())
		{
			continue;
		}

		WString strFileName;// = m_FMGClient.GetGroupHeadPicFullName(lpGroupInfo->m_nGroupNumber);
		if (!Hootina::CPath::IsFileExist(strFileName.c_str()))
			strFileName = Hootina::CPath::GetAppPath() + _T("Image\\DefGroupHeadPic.png");

		//TODO: Dennis Notice
		int nIndex = m_GroupListCtrl.AddBuddyItem(nTeamIndex,lpGroupInfo->m_strGroupId);
		m_GroupListCtrl.SetBuddyItemNickName(nTeamIndex, nIndex, lpGroupInfo->m_strAccount.c_str());
		m_GroupListCtrl.SetBuddyItemMarkName(nTeamIndex, nIndex, lpGroupInfo->m_strName.c_str());
		m_GroupListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strFileName.c_str(), FALSE);

		++nActualGroupCount;
		
	}

	m_GroupListCtrl.SetBuddyTeamMaxCnt(nTeamIndex, nActualGroupCount);
	m_GroupListCtrl.SetBuddyTeamCurCnt(nTeamIndex, nActualGroupCount);

	if (m_GroupListCtrl.IsWindowVisible())
	{
		m_GroupListCtrl.Invalidate();
	}
}


/**
 * @brief 更新最近聊天的树形结构(UI)
 * 
 */
void CMainDlg::UpdateRecentTreeCtrl()
{
	//{
	//	C_UI_RecentList* lpRecentList = NULL;// m_FMGClient.GetRecentList();
	//	if (NULL == lpRecentList)
	//		return;

	//	m_RecentListCtrl.DelAllItems();

	//	//TODO: 目前只有一组最近联系人
	//	int nTeamIndex = m_RecentListCtrl.AddBuddyTeam(-1);
	//	//m_RecentListCtrl.SetBuddyTeamName(nTeamIndex, _T("最近联系人"));
	//	m_RecentListCtrl.SetBuddyTeamName(nTeamIndex, _T(""));
	//	m_RecentListCtrl.SetBuddyTeamExpand(nTeamIndex, TRUE);

	//	int nCount = 0;
	//	CString strUserID;
	//	UINT uFaceID = 0;
	//	CString strThumbPath;
	//	TCHAR szTime[8];
	//	long nNewMsgCount;
	//	C_UI_MessageSender* pMessageSender = NULL;

	//	int nRecentCount = lpRecentList->GetRecentCount();
	//	for (int i = nRecentCount - 1; i >= 0; --i)
	//	{
	//		C_UI_RecentInfo* lpRecentInfo = lpRecentList->GetRecent(i);
	//		if (lpRecentInfo == NULL)
	//			continue;

	//		if (E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_BUDDY == lpRecentInfo->m_nType)			// 好友
	//		{
	//			C_UI_BuddyList* lpBuddyList = NULL;// m_FMGClient.GetBuddyList();
	//			//如果还是最近好友，则根据好友信息显示
	//			if (lpBuddyList != NULL)
	//			{
	//				C_UI_BuddyInfo* lpBuddyInfo = NULL;// lpBuddyList->GetBuddy(lpRecentInfo->m_uUserID);
	//				if (lpBuddyInfo != NULL)
	//				{
	//					BOOL bGray = TRUE;
	//					if (lpBuddyInfo->m_nStatus != E_UI_ONLINE_STATUS::STATUS_OFFLINE && lpBuddyInfo->m_nStatus != E_UI_ONLINE_STATUS::STATUS_INVISIBLE)
	//						bGray = FALSE;

	//					if (lpBuddyInfo->m_bUseCustomFace&& lpBuddyInfo->m_bCustomFaceAvailable)
	//					{
	//						strThumbPath.Format(_T("%s%d.png"), m_userMgr.GetCustomUserThumbFolder().c_str(), lpBuddyInfo->m_uUserIndex);
	//						if (!Hootina::CPath::IsFileExist(strThumbPath))
	//							strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, lpBuddyInfo->m_nFace);
	//					}
	//					else
	//						strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, lpBuddyInfo->m_nFace);

	//					int nIndex = m_RecentListCtrl.AddBuddyItem(nTeamIndex, i);
	//					//strUserID.Format(_T("%u"), lpBuddyInfo->m_uUserID);
	//					m_RecentListCtrl.SetBuddyItemID(nTeamIndex, nIndex, lpBuddyInfo->m_uUserIndex);
	//					m_RecentListCtrl.SetBuddyItemAccountID(nTeamIndex, nIndex, lpBuddyInfo->m_uUserIndex);

	//					m_RecentListCtrl.SetBuddyItemNickName(nTeamIndex, nIndex, lpBuddyInfo->m_strNickName.c_str());
	//					//m_RecentListCtrl.SetBuddyItemGender(nTeamIndex, nIndex, bGender);
	//					m_RecentListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strThumbPath, bGray);
	//					m_RecentListCtrl.SetBuddyItemLastMsg(nTeamIndex, nIndex, lpRecentInfo->m_szLastMsgText);
	//					if (lpBuddyInfo->m_nStatus == E_UI_ONLINE_STATUS::STATUS_MOBILE_ONLINE)
	//					{
	//						strThumbPath.Format(_T("%sImage\\mobile_online.png"), g_szHomePath);
	//						m_RecentListCtrl.SetBuddyItemMobilePic(nTeamIndex, nIndex, strThumbPath);
	//					}

	//					memset(szTime, 0, sizeof(szTime));
	//					if (IsToday(lpRecentInfo->m_MsgTime))
	//						FormatTime(lpRecentInfo->m_MsgTime, _T("%H:%M"), szTime, ARRAYSIZE(szTime));
	//					else
	//						FormatTime(lpRecentInfo->m_MsgTime, _T("%m-%d"), szTime, ARRAYSIZE(szTime));
	//					m_RecentListCtrl.SetBuddyItemDate(nTeamIndex, nIndex, szTime);

	//					pMessageSender = m_userMgr.m_MsgList.GetMsgSender(E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_BUDDY, lpBuddyInfo->m_uUserIndex);
	//					if (pMessageSender != NULL)
	//						nNewMsgCount = pMessageSender->GetDisplayMsgCount();
	//					else
	//						nNewMsgCount = 0;
	//					m_RecentListCtrl.SetBuddyItemNewMsgCount(nTeamIndex, nIndex, nNewMsgCount);

	//					nCount++;
	//				}
	//				//如果已经不是好友了，则根据最近联系人中的信息显示
	//				//else
	//				//{
	//				//	uFaceID = lpRecentInfo->m_uFaceID;
	//				//	if(uFaceID > USER_THUMB_COUNT)
	//				//		uFaceID = 0;

	//				//	strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, uFaceID);

	//				//	int nIndex = m_RecentListCtrl.AddBuddyItem(nTeamIndex, i);
	//				//	strUserID.Format(_T("%u"), lpRecentInfo->m_uUserID);
	//				//	m_RecentListCtrl.SetBuddyItemAccountID(nTeamIndex, nIndex, lpRecentInfo->m_uUserID);
	//				//	m_RecentListCtrl.SetBuddyItemID(nTeamIndex, nIndex, lpRecentInfo->m_uUserID);

	//				//	m_RecentListCtrl.SetBuddyItemNickName(nTeamIndex, nIndex, lpRecentInfo->m_szName);
	//				//	//m_RecentListCtrl.SetBuddyItemGender(nTeamIndex, nIndex, bGender);
	//				//	m_RecentListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strThumbPath, TRUE);
	//				//	m_RecentListCtrl.SetBuddyItemLastMsg(nTeamIndex, nIndex, lpRecentInfo->m_szLastMsgText);
	//				//	
	//				//	memset(szTime, 0, sizeof(szTime));
	//				//	if(IsToday(lpRecentInfo->m_MsgTime))	
	//				//		FormatTime(lpRecentInfo->m_MsgTime, _T("%H:%M"), szTime, ARRAYSIZE(szTime));
	//				//	else
	//				//		FormatTime(lpRecentInfo->m_MsgTime, _T("%m-%d"), szTime, ARRAYSIZE(szTime));
	//				//	m_RecentListCtrl.SetBuddyItemDate(nTeamIndex, nIndex, szTime);

	//				//	nCount++;
	//				//}
	//			}
	//		}
	//		else if (E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_GROUP == lpRecentInfo->m_nType)	// 群
	//		{
	//			CGroupList* lpGroupList = NULL;// m_FMGClient.GetGroupList();
	//			if (lpGroupList != NULL)
	//			{
	//				C_UI_GroupInfo* lpGroupInfo = NULL;// lpGroupList->GetGroupByCode(lpRecentInfo->m_uUserID);
	//				if (lpGroupInfo != NULL)
	//				{

	//					strThumbPath.Format(_T("%sImage\\DefGroupHeadPic.png"), g_szHomePath);

	//					int nIndex = m_RecentListCtrl.AddBuddyItem(nTeamIndex, i);
	//					//m_RecentListCtrl.SetBuddyItemID(nTeamIndex, nIndex, lpGroupInfo->m_nGroupCode);
	//					m_RecentListCtrl.SetBuddyItemNickName(nTeamIndex, nIndex, lpGroupInfo->m_strName.c_str());
	//					m_RecentListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strThumbPath, FALSE);
	//					m_RecentListCtrl.SetBuddyItemLastMsg(nTeamIndex, nIndex, lpRecentInfo->m_szLastMsgText);

	//					memset(szTime, 0, sizeof(szTime));
	//					if (IsToday(lpRecentInfo->m_MsgTime))
	//						FormatTime(lpRecentInfo->m_MsgTime, _T("%H:%M"), szTime, ARRAYSIZE(szTime));
	//					else
	//						FormatTime(lpRecentInfo->m_MsgTime, _T("%m-%d"), szTime, ARRAYSIZE(szTime));
	//					m_RecentListCtrl.SetBuddyItemDate(nTeamIndex, nIndex, szTime);

	//					pMessageSender = m_userMgr.m_MsgList.GetMsgSender(E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_GROUP, lpRecentInfo->m_uUserID);
	//					if (pMessageSender != NULL)
	//						nNewMsgCount = pMessageSender->GetDisplayMsgCount();
	//					else
	//						nNewMsgCount = 0;
	//					m_RecentListCtrl.SetBuddyItemNewMsgCount(nTeamIndex, nIndex, nNewMsgCount);

	//					nCount++;
	//				}
	//			}
	//		}
	//		else if (E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_SESS == lpRecentInfo->m_nType)	// 好像是讨论组
	//		{
	//		}

	//	}// end for-loop

	//	m_RecentListCtrl.SetBuddyTeamMaxCnt(nTeamIndex, nCount);
	//	m_RecentListCtrl.SetBuddyTeamCurCnt(nTeamIndex, nCount);

	//	if (m_RecentListCtrl.IsWindowVisible())
	//		m_RecentListCtrl.Invalidate();
	//}
}



/**
 * @brief 响应鼠标左键点击托盘区图标,有消息提示的时候使用(UI)
 * 
 */
void CMainDlg::OnTrayIcon_LButtunUp()
{
	DoMsgList();
	{
		if (m_LoginDlg.IsWindow())
		{
			m_LoginDlg.ShowWindow(SW_SHOW);
			::SetForegroundWindow(m_LoginDlg.m_hWnd);
		}
		else if (IsWindow())
		{
			ShowWindow(SW_SHOW);
			::SetForegroundWindow(m_hWnd);
		}
	}

	//先将任务栏图标恢复成正常状态
	SetTrayIconOnLine();
	if (m_bShowTrayMenu)
	{
		m_bShowTrayMenu = false;
	}
}

/**
 * @brief 鼠标右键点击托盘区图标，一般做设置用(UI)
 * 
 */
void CMainDlg::OnTrayIcon_RButtunUp()
{
	if (m_MsgTipDlg.IsWindow())
	{
		m_MsgTipDlg.DestroyWindow();
	}

	CPoint pt;
	GetCursorPos(&pt);

	::SetForegroundWindow(m_hWnd);

	CSkinMenu PopupMenu;
	if (m_bPanelLocked)
	{
		PopupMenu = m_TrayMenuLock.GetSubMenu(0);
	}
	else
	{
		PopupMenu = m_TrayMenuNormal.GetSubMenu(0);
	}
    //根据是否已经静音，修改“关闭所有声音”选项
	if (m_userCfg.IsEnableMute())
	{
		PopupMenu.ModifyMenu(ID_MENU_MUTE, MF_BYCOMMAND | MF_STRING /*| MF_POPUP*/, ID_MENU_MUTE, _T("关闭所有声音(已关闭)"));
	}
	else
	{
		PopupMenu.ModifyMenu(ID_MENU_MUTE, MF_BYCOMMAND | MF_STRING /*| MF_POPUP*/, ID_MENU_MUTE, _T("关闭所有声音"));
	}
	PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);
	m_bShowTrayMenu = true;
	// BUGFIX: See "PRB: Menus for Notification Icons Don't Work Correctly"
	//::PostMessage(m_hWnd, WM_NULL, 0, 0);
}


/**
 * @brief 响应鼠标移动到托盘区的消息(UI)
 * 
 */
void CMainDlg::OnTrayIcon_MouseHover()
{
	if (m_bShowTrayMenu)
	{

	}
	//if (m_dwMsgTimerId != NULL)
	else
	{
		//C_UI_MessageList* lpMsgList = NULL;// m_FMGClient.GetMessageList();
		//if (lpMsgList != NULL && lpMsgList->GetMsgSenderCount() > 0)
		{
			CRect rcTrayIcon(0, 0, 0, 0);
			
			//Win7系统不能采取这种方式获取任务栏托盘图标的位置
			if(!RunTimeHelper::IsVista())
				m_TrayIcon.GetTrayIconRect(&rcTrayIcon);
			
			//TODO: 权宜之计，后面要从根本上解决位置不对的问题
			if((rcTrayIcon.left==0 && rcTrayIcon.right==0) || (rcTrayIcon.top==0 && rcTrayIcon.bottom==0))
			{	
				POINT pt;
				::GetCursorPos(&pt);
				rcTrayIcon.left   = pt.x-80;
				rcTrayIcon.top    = pt.y-90;
				rcTrayIcon.right  = pt.x+20;
				rcTrayIcon.bottom = pt.y+10;
			}
			m_rcTrayIconRect = rcTrayIcon; 
			m_MsgTipDlg.m_rcTrayIcon2 = rcTrayIcon;
			//m_MsgTipDlg.m_lpFMGClient = &m_FMGClient;
			m_MsgTipDlg.m_hMainDlg = m_hWnd;
			m_MsgTipDlg.m_rcTrayIcon = rcTrayIcon;
			if (!m_MsgTipDlg.IsWindow())
			{
				m_MsgTipDlg.Create(m_hWnd);
			}
			
			//m_MsgTipDlg.ShowWindow(SW_SHOWNOACTIVATE);	
		}
	}
	//::OutputDebugString(_T("WM_MOUSEHOVER\n"));
}



/**
 * @brief 响应鼠标离开托盘区(UI)
 * 
 */
void CMainDlg::OnTrayIcon_MouseLeave()
{
	if (m_bShowTrayMenu)
	{

	}
	else
	{
		if (m_MsgTipDlg.IsWindow())
		{
			CRect rcWindow;
			m_MsgTipDlg.GetWindowRect(&rcWindow);

			POINT pt = { 0 };
			::GetCursorPos(&pt);

			if (!::PtInRect(&rcWindow, pt) && !::PtInRect(&m_rcTrayIconRect, pt))
			{
				m_MsgTipDlg.DestroyWindow();
				//m_MsgTipDlg.ShowWindow(SW_HIDE);
			}
			else
			{
				m_MsgTipDlg.StartTrackMouseLeave();
			}
		}
	}
	//::OutputDebugString(_T("WM_MOUSELEAVE\n"));
}


/**
 * @brief 加载应用图标
 * 
 * @param nStatus 
 * @return BOOL 
 */
BOOL CMainDlg::LoadAppIcon(E_UI_ONLINE_STATUS nStatus)
{
	DestroyAppIcon();

	WString strFileName;
	switch (nStatus)
	{
		case E_UI_ONLINE_STATUS::STATUS_ONLINE:
		{
			strFileName = Hootina::CPath::GetAppPath() + _T("Image\\imonline.ico");
		}break;
		case E_UI_ONLINE_STATUS::STATUS_OFFLINE:
		{
			strFileName = Hootina::CPath::GetAppPath() + _T("Image\\offline.ico");
		}break;
	}
	m_hAppIcon = AtlLoadIconImage(strFileName.c_str(), LR_DEFAULTCOLOR|LR_LOADFROMFILE, 
		::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	return m_hAppIcon != NULL ? TRUE : FALSE;
}


/**
 * @brief 销毁应用图标
 * TODO: 需要对ICON总结
 */
void CMainDlg::DestroyAppIcon()
{
	if (m_hAppIcon != NULL)
	{
		::DestroyIcon(m_hAppIcon);
		m_hAppIcon = NULL;
	}
}

/**
 * @brief 加载登录图标
 * 
 * @return BOOL 
 */
BOOL CMainDlg::LoadLoginIcon()
{
	DestroyLoginIcon();

	CString strFileName;
	for (int i = 0; i < 6; i++)
	{
		strFileName.Format(_T("%sImage\\Loading_%d.ico"), Hootina::CPath::GetAppPath().c_str(), i+1);
		m_hLoginIcon[i] = AtlLoadIconImage((LPCTSTR)strFileName, LR_DEFAULTCOLOR|LR_LOADFROMFILE, 
			::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	}

	return TRUE;
}


/**
 * @brief 销毁登录图标
 * 
 */
void CMainDlg::DestroyLoginIcon()
{
	for (int i = 0; i < 6; i++)
	{
		if (m_hLoginIcon[i] != NULL)
		{
			::DestroyIcon(m_hLoginIcon[i]);
			m_hLoginIcon[i] = NULL;
		}
	}
}


/**
 * @brief 加载添加好友图标
 * 
 * TODO: 作用点不明
 * @return BOOL 
 */
BOOL CMainDlg::LoadAddFriendIcon()
{
	DestroyAddFriendIcon();

	CString strFileName;
	for (int i = 0; i < ARRAYSIZE(m_hAddFriendIcon); i++)
	{
		strFileName.Format(_T("%sImage\\speaker_%d.ico"), Hootina::CPath::GetAppPath().c_str(), i+1);
		m_hAddFriendIcon[i] = AtlLoadIconImage((LPCTSTR)strFileName, LR_DEFAULTCOLOR|LR_LOADFROMFILE, 
			::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	}

	return TRUE;
}


/**
 * @brief 销毁添加好友的图标
 * TODO: 调用点不明
 * 
 */
void CMainDlg::DestroyAddFriendIcon()
{
	for (int i = 0; i < ARRAYSIZE(m_hAddFriendIcon); i++)
	{
		if (m_hAddFriendIcon[i] != NULL)
		{
			::DestroyIcon(m_hAddFriendIcon[i]);
			m_hAddFriendIcon[i] = NULL;
		}
	}
}


/**
 * @brief 更新消息图标，
 * TODO: 调用点位置不明
 * 
 */
void CMainDlg::UpdateMsgIcon()
{
	C_UI_MessageList* lpMsgList = NULL;// m_FMGClient.GetMessageList();
	if (NULL == lpMsgList)
		return;

	C_UI_MessageSender* lpMsgSender = lpMsgList->GetLastMsgSender();
	if (NULL == lpMsgSender)
		return;
	
	E_UI_CHAT_MSG_TYPE nMsgType = lpMsgSender->GetMsgType();
	UINT nSenderId = lpMsgSender->GetSenderId();
	UINT nGroupCode = lpMsgSender->GetGroupCode();

	if (1)
	{


		if (m_hMsgIcon != NULL)
		{
			::DestroyIcon(m_hMsgIcon);
			m_hMsgIcon = NULL;
		}

		CString strHeadPicFileName;

		if (E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_BUDDY == nMsgType)
		{
			strHeadPicFileName = GetHeadPicFullName(0, nSenderId);
			strHeadPicFileName.Format(_T("%sUsers\\%s\\UserThumb\\%d.png"), g_szHomePath, m_userMgr.m_UserInfo.m_strAccount.c_str(), nSenderId);
			if (!Hootina::CPath::IsFileExist(strHeadPicFileName))
			{
				strHeadPicFileName.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, m_userMgr.GetFaceID(nSenderId));
			}
		}
		else if (E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_GROUP == nMsgType)
		{
			strHeadPicFileName = GetHeadPicFullName(nSenderId, 0);
		}
		else if (E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_SESS == nMsgType)
		{
			strHeadPicFileName = GetHeadPicFullName(nGroupCode, nSenderId);
		}
// 		else if (long_SYSGROUP == nMsgType)
// 			strHeadPicFileName = Hootina::CPath::GetAppPath() + _T("Image\\DefSysHeadPic.ico");

		m_hMsgIcon = ExtractIcon(strHeadPicFileName);
		if (NULL == m_hMsgIcon)
		{
			if (E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_BUDDY == nMsgType || E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_SESS == nMsgType)
				m_hMsgIcon = AtlLoadIconImage(IDI_BUDDY, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
			else if (E_UI_CHAT_MSG_TYPE::FMG_MSG_TYPE_GROUP == nMsgType)
				m_hMsgIcon = AtlLoadIconImage(IDI_GROUP_CHAT_DLG_16, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
			//else if (long_SYSGROUP == nMsgType)
			//	m_hMsgIcon = AtlLoadIconImage(IDI_SYS, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		}
	}
}

/**
 * @brief 获取头像全路径
 * TODO: 调用点未知不明
 * @param nGroupCode 
 * @param nUTalkUin 
 * @return CString 
 */
CString CMainDlg::GetHeadPicFullName(UINT nGroupCode, UINT nUTalkUin)
{
	return _T("");
}


/**
 * @brief 导出图标
 * 
 * @param lpszFileName 
 * @return HICON 
 * TODO: 作用不知道
 */
HICON CMainDlg::ExtractIcon(LPCTSTR lpszFileName)
{
	if (NULL == lpszFileName || NULL == *lpszFileName)
	{
		return NULL;
	}

	int cx = 16, cy = 16;
	HBITMAP hBmp = NULL;

	Gdiplus::Bitmap imgHead(lpszFileName);
	if (imgHead.GetLastStatus() != Gdiplus::Ok)
	{
		return NULL;
	}

	if (imgHead.GetWidth() != cx || imgHead.GetHeight() != cy)
	{
		Gdiplus::Bitmap* pThumbnail = (Gdiplus::Bitmap*)imgHead.GetThumbnailImage(cx, cy, NULL, NULL);
		if (pThumbnail != NULL)
		{
			pThumbnail->GetHBITMAP(Gdiplus::Color(255,255,255), &hBmp);
			delete pThumbnail;
		}
	}
	else
	{
		imgHead.GetHBITMAP(Gdiplus::Color(255,255,255), &hBmp);
	}

	if (NULL == hBmp)
		return NULL;

	HICON hIcon = NULL;

	CImageList imgList;
	BOOL bRet = imgList.Create(cx, cy, TRUE | ILC_COLOR32, 1, 1);
	if (bRet)
	{
		imgList.SetBkColor(RGB(255,255,255));
		imgList.Add(hBmp);
		hIcon = imgList.ExtractIcon(0);
	}
	::DeleteObject(hBmp);
	imgList.Destroy();

	return hIcon;
}

/**
 * @brief 关闭所有的对话框
 * 
 */
void CMainDlg::CloseAllDlg()
{
	//关闭好友聊天窗口
	{
		for (auto iter : m_mapBuddyChatDlg)
		{
			CBuddyChatDlg* lpBuddyChatDlg = iter.second;
			if (lpBuddyChatDlg != NULL)
			{
				if (lpBuddyChatDlg->IsWindow())
				{
					lpBuddyChatDlg->DestroyWindow();
				}

				delete lpBuddyChatDlg;
			}
		}
		m_mapBuddyChatDlg.clear();
	}

	//关闭群组聊天窗口
	{
		for (auto iter = m_mapGroupChatDlg.begin(); iter != m_mapGroupChatDlg.end(); iter++)
		{
			CGroupChatDlg* lpGroupChatDlg = iter->second;
			if (lpGroupChatDlg != NULL)
			{
				if (lpGroupChatDlg->IsWindow())
					lpGroupChatDlg->DestroyWindow();
				delete lpGroupChatDlg;
			}
		}
		m_mapGroupChatDlg.clear();
	}



	//关闭好友消息对话框
	{
		for (auto iter = m_mapBuddyInfoDlg.begin(); iter != m_mapBuddyInfoDlg.end(); iter++)
		{
			CBuddyInfoDlg* lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->DestroyWindow();
				delete lpBuddyInfoDlg;
			}
		}
		m_mapBuddyInfoDlg.clear();
	}

	//关闭群组成员信息对话框
	{
		std::map<CGMemberInfoMapKey, CBuddyInfoDlg*>::iterator iter;
		for (iter = m_mapGMemberInfoDlg.begin(); iter != m_mapGMemberInfoDlg.end(); iter++)
		{
			CBuddyInfoDlg* lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->DestroyWindow();
				delete lpBuddyInfoDlg;
			}
		}
		m_mapGMemberInfoDlg.clear();
	}

	//关闭群组信息对话框
	{
		for (auto iter = m_mapGroupInfoDlg.begin(); iter != m_mapGroupInfoDlg.end(); iter++)
		{
			CGroupInfoDlg* lpGroupInfoDlg = iter->second;
			if (lpGroupInfoDlg != NULL)
			{
				if (lpGroupInfoDlg->IsWindow())
					lpGroupInfoDlg->DestroyWindow();
				delete lpGroupInfoDlg;
			}
		}
		m_mapGroupInfoDlg.clear();
	}

	//销毁查找好友对话框
	if (m_pFindFriendDlg != NULL && m_pFindFriendDlg->IsWindow())
	{
		m_pFindFriendDlg->DestroyWindow();
	}

	//销毁我的资料对话框
	if (m_LogonUserInfoDlg.IsWindow())
	{
		m_LogonUserInfoDlg.DestroyWindow();
	}
}


/**
 * @brief 从菜单ID获取对应的UTalk_STATUS
 * 
 * @param nMenuID 
 * @return E_UI_ONLINE_STATUS 
 * TODO: 作用不明朗
 */
E_UI_ONLINE_STATUS CMainDlg::GetStatusFromMenuID(int nMenuID)
{
	switch (nMenuID)
	{
		case ID_MENU_IM_ONLINE:
		{
			return E_UI_ONLINE_STATUS::STATUS_ONLINE;
		}break;
		case ID_MENU_IM_OFFLINE:
		{
			return E_UI_ONLINE_STATUS::STATUS_OFFLINE;
		}break;
		default:
		{
			return E_UI_ONLINE_STATUS::STATUS_OFFLINE;
		}break;
	}
}

// 

/**
 * @brief 根据指定状态设置状态菜单按钮的图标
 * 
 * @param btnStatus 状态按钮
 * @param nStatus 在线离线状态
 * 
 * TODO: 需要搞懂作用
 */
void CMainDlg::StatusMenuBtn_SetIconPic(CSkinButton& btnStatus, E_UI_ONLINE_STATUS nStatus)
{
	LPCTSTR lpszFileName;

	switch (nStatus)
	{
		case E_UI_ONLINE_STATUS::STATUS_ONLINE:
		{
			lpszFileName = _T("Status\\imonline.png");
		}break;
		case E_UI_ONLINE_STATUS::STATUS_OFFLINE:
		{
			lpszFileName = _T("Status\\imoffline.png");
		}break;
		default:
		{
			//TODO: ERR_LOG
			return;
		}	
	}

	btnStatus.SetIconPic(lpszFileName);
	btnStatus.Invalidate();
}

//
/**
 * @brief 根据在线状态对好友列表进行排序，在线排在前面，离线排在后面
 * TODO: 此处代码可能需要删除
 * 
 */
void CMainDlg::BuddyListSortOnStaus()
{
	long nTeamCount = m_userMgr.m_BuddyList.m_arrBuddyTeamInfo.size();
	for(long i=0; i<nTeamCount; ++i)
	{
		m_userMgr.m_BuddyList.m_arrBuddyTeamInfo[i]->Sort();
	}
}


/**
 * @brief 添加好友确认消息的对话框
 * TODO:此处代码可能不再使用，需要考虑删除
 */
void CMainDlg::ShowAddFriendConfirmDlg()
{	
	//size_t nCount = 0;// m_FMGClient.m_aryAddFriendInfo.size();
	//if(nCount == 0)
	//	return;

	//AddFriendInfo_ST* pAddFriendInfo = NULL;// m_FMGClient.m_aryAddFriendInfo[nCount - 1];
	//if(pAddFriendInfo == NULL)
	//	return;

	//CString strWindowTitle;
	//CString strInfo;
	//CAddFriendConfirmDlg AddFriendConfirmDlg;
	////AddFriendConfirmDlg.m_pFMGClient = &m_FMGClient;

	////别人加自己
	//if(pAddFriendInfo->nCmd== E_UI_OPERATE_FRIEND::Apply)
	//{
	//	strWindowTitle = _T("加好友请求");
	//	strInfo.Format(_T("%s(%s)请求您添加为好友，是否同意？"), pAddFriendInfo->szNickName, pAddFriendInfo->szAccountName);
	//	AddFriendConfirmDlg.ShowAgreeButton(TRUE);
	//	AddFriendConfirmDlg.ShowRefuseButton(TRUE);
	//	AddFriendConfirmDlg.ShowOKButton(FALSE);
	//}
	////自己主动拒绝别人或者被别人拒绝
	//else if(pAddFriendInfo->nCmd== E_UI_OPERATE_FRIEND::Refuse)
	//{
	//	strWindowTitle = g_strAppTitle.c_str();
	//	strInfo.Format(_T("您或者对方(%s(%s))拒绝了加好友请求。"), pAddFriendInfo->szNickName, pAddFriendInfo->szAccountName);
	//	AddFriendConfirmDlg.ShowAgreeButton(FALSE);
	//	AddFriendConfirmDlg.ShowRefuseButton(FALSE);
	//	AddFriendConfirmDlg.ShowOKButton(TRUE);
	//}
	////自己或者别人同意了加好友请求
	//else if(pAddFriendInfo->nCmd== E_UI_OPERATE_FRIEND::Agree)
	//{		
	//	strWindowTitle = g_strAppTitle.c_str();
 //       //群号大于0xFFFFFFF
	//	if (pAddFriendInfo->uAccountID < 0xFFFFFFF)
	//	{
	//		strInfo.Format(_T("您和%s(%s)已经是好友啦，开始聊天吧。"), pAddFriendInfo->szNickName, pAddFriendInfo->szAccountName);
	//	}
	//	else
	//	{
	//		strInfo.Format(_T("您已经成功加入群【%s(%s)】啦，开始聊天吧。"), pAddFriendInfo->szNickName, pAddFriendInfo->szAccountName);
	//	}
	//	AddFriendConfirmDlg.ShowAgreeButton(FALSE);
	//	AddFriendConfirmDlg.ShowRefuseButton(FALSE);
	//	AddFriendConfirmDlg.ShowOKButton(TRUE);
	//	
	//	//m_userMgr.AddFriend(pAddFriendInfo->uAccountID, pAddFriendInfo->szAccountName, pAddFriendInfo->szNickName);
	//	//PostMessage(FMG_MSG_UPDATE_BUDDY_LIST, 0, 0);
	//	//重新获取下好友列表并更新
 //       //m_FMGClient.GetFriendList();
	//}
	//
	//AddFriendConfirmDlg.SetWindowInfo(strInfo);
	//AddFriendConfirmDlg.SetWindowTitle(strWindowTitle);

	//int nRet = AddFriendConfirmDlg.DoModal(m_hWnd, NULL);
	//
	//if(nRet==ID_ADDCONFIRM_REFUSE || nRet==ID_ADD_CONFIRM_AGREE)
	//{		
	//	E_UI_OPERATE_FRIEND uCmd = (nRet==ID_ADD_CONFIRM_AGREE ? E_UI_OPERATE_FRIEND::Agree : E_UI_OPERATE_FRIEND::Refuse);
	//	//m_FMGClient.ResponseAddFriendApply(pAddFriendInfo->uAccountID, uCmd);
	//}
	//
	//DELETE_PTR(pAddFriendInfo);
	//m_FMGClient.m_aryAddFriendInfo.erase(m_FMGClient.m_aryAddFriendInfo.end()-1);
}


//
/**
 * @brief 插入好友分组的一项,构建鼠标弹出的右键菜单(CreateUI)
 * @image https://www.dennisthink.com/wp-content/uploads/2019/10/MoveFriendToTeamMenu.jpg
 * @param popMenu 弹出的菜单
 * @return BOOL 创建结果。
 */
BOOL CMainDlg::InsertTeamMenuItem(CSkinMenu& popMenu)
{
	
	int nTeamIndex = -1;
	int nItemIndex = -1;
	m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nItemIndex);
	if (nTeamIndex < 0 || nItemIndex < 0)
	{
		return FALSE;
	}

	CSkinMenu* subMenu = new CSkinMenu();
	subMenu->CreatePopupMenu();


	size_t nCount = m_netProto->m_BuddyList.m_arrBuddyTeamInfo.size();
	for(size_t i=0; i<nCount; ++i)
	{
		if(i == nTeamIndex)
		{
			continue;
		}	
		
		subMenu->AppendMenu(MF_STRING|MF_BYPOSITION, TEAM_MENU_ITEM_BASE+i,m_netProto->m_BuddyList.m_arrBuddyTeamInfo[i]->m_strName.c_str());
	}

	popMenu.ModifyMenu(8, MF_POPUP|MF_BYPOSITION, (UINT_PTR)subMenu->m_hMenu, NULL);

	return TRUE;
}


/**
 * @brief 保存当前用户的登录信息到文件
 * 
 */
void CMainDlg::SaveCurrentLogonUserToFile()
{	
	CIniFile iniFile;
	CString strIniFile;
	strIniFile.Format(_T("%sconfig\\flamingo.ini"), g_szHomePath);
	CString strAccountList;
	iniFile.ReadString(_T("LogonUserList"), _T("AccountName"), _T(""), strAccountList.GetBuffer(256), 256, strIniFile);
	strAccountList.ReleaseBuffer();
	strAccountList += m_userMgr.m_UserInfo.m_strAccount.c_str();
	strAccountList +=_T(";");
	iniFile.WriteString(_T("LogonUserList"), _T("AccountName"), strAccountList, strIniFile);
}


/**
 * @brief 从配置文件中，删除当前的登录用户信息
 * 
 */
void CMainDlg::DeleteCurrentUserFromFile()
{	
	CIniFile iniFile;
	CString strIniFile;
	strIniFile.Format(_T("%sconfig\\flamingo.ini"), g_szHomePath);
	CString strAccountList;
	iniFile.ReadString(_T("LogonUserList"), _T("AccountName"), _T(""), strAccountList.GetBuffer(256), 256, strIniFile);
	strAccountList.ReleaseBuffer();
	
	CString strCurrentAccount;
	strCurrentAccount.Format(_T("%s;"), m_userMgr.m_UserInfo.m_strAccount.c_str());
	strAccountList.Replace(strCurrentAccount, _T(""));
	iniFile.WriteString(_T("LogonUserList"), _T("AccountName"), strAccountList, strIniFile);
}


/**
 * @brief 判断是否有文件在传输
 * 
 * @return BOOL TRUE 有文件传输,FALSE 没有文件传输
 */
BOOL CMainDlg::IsFilesTransferring()
{
	/*std::map<UINT, CBuddyChatDlg*>::iterator iter;
	for (iter = m_mapBuddyChatDlg.begin(); iter != m_mapBuddyChatDlg.end(); iter++)
	{
		CBuddyChatDlg* lpBuddyChatDlg = iter->second;
		if (lpBuddyChatDlg == NULL)
			continue;
		if(lpBuddyChatDlg->IsFilesTransferring())
			return TRUE;
	}*/
	
	return FALSE;
}


/**
 * @brief 创建必备目录
 * 
 */
void CMainDlg::CreateEssentialDirectories()
{
	CString strAppPath(g_szHomePath);
	
	CString strUsersDirectory(strAppPath);
	strUsersDirectory += _T("Users");
	if (!Hootina::CPath::IsDirectoryExist(strUsersDirectory))
	{
		Hootina::CPath::CreateDirectory(strUsersDirectory);
	}

	CString strCurrentUserDirectory;
	strCurrentUserDirectory.Format(_T("%s\\%s"), strUsersDirectory, m_userMgr.m_UserInfo.m_strAccount.c_str());

	if (!Hootina::CPath::IsDirectoryExist(strCurrentUserDirectory))
	{
		Hootina::CPath::CreateDirectory(strCurrentUserDirectory);
	}

	CString strChatImagesDirectory;
	strChatImagesDirectory.Format(_T("%s\\ChatImage\\"), strCurrentUserDirectory);
	if (!Hootina::CPath::IsDirectoryExist(strChatImagesDirectory))
	{
		Hootina::CPath::CreateDirectory(strChatImagesDirectory);
	}

	CString strUserThumbDirectory;
	strUserThumbDirectory.Format(_T("%s\\UserThumb\\"), strCurrentUserDirectory);
	if (!Hootina::CPath::IsDirectoryExist(strUserThumbDirectory))
	{
		Hootina::CPath::CreateDirectory(strUserThumbDirectory);
	}
}


/**
 * @brief 好友消息闪烁动画
 * TODO: 此处可能有问题
 */
void CMainDlg::DoRecvAddFriendTroyIconCartoon()
{
	static BOOL bSwitch = FALSE;

	bSwitch = !bSwitch;
	if (bSwitch)
	{
		m_TrayIcon.ModifyIcon(m_hAddFriendIcon[0], _T("加好友请求"));
	}
	else
	{
		m_TrayIcon.ModifyIcon(m_hAddFriendIcon[1], _T("加好友请求"));
	}
}

/**
 * @brief 响应好友发送文件的请求消息
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnNotifyFriendFileReqMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	C_WND_MSG_FileNotifyReq * pMsg = (C_WND_MSG_FileNotifyReq*)(lParam);
	if (nullptr != pMsg)
	{
		ShowBuddyChatDlg(pMsg->m_szFriendId);
		auto item = m_mapBuddyChatDlg.find(pMsg->m_szFriendId);
		if (item != m_mapBuddyChatDlg.end())
		{
			item->second->OnFileNotifyReqMsg(pMsg);
		}
	}
	return 1;
}


LRESULT CMainDlg::OnSendFileProgress(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	C_WND_MSG_FileProcessMsg * pMsg = (C_WND_MSG_FileProcessMsg*)(lParam);
	if (nullptr != pMsg)
	{
		auto item = m_mapBuddyChatDlg.find(pMsg->m_szFriendId);
		if (item != m_mapBuddyChatDlg.end())
		{
			item->second->OnSendFileProcess(pMsg);
		}
	}
	return 0;
}

LRESULT CMainDlg::OnRecvFileProgress(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	C_WND_MSG_FileProcessMsg * pMsg = (C_WND_MSG_FileProcessMsg*)(lParam);
	if (nullptr != pMsg)
	{
		auto item = m_mapBuddyChatDlg.find(pMsg->m_szFriendId);
		if (item != m_mapBuddyChatDlg.end())
		{
			item->second->OnRecvFileProcess(pMsg);
		}
	}
	return 0;
}

/**
 * @brief 响应登录结果消息()
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CMainDlg::OnUserLoginRsp(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto pSess = m_netProto;
	if(pSess)
	{
		//主界面用户昵称
		{
			m_staNickName.SetWindowText(pSess->m_loginResult.m_szNickName);
			m_userMgr.m_UserInfo.m_strNickName = pSess->m_loginResult.m_szNickName;
		}


		//主界面，用户签名
		{
			m_btnSign.SetWindowText(pSess->m_loginResult.m_szSignature);
			m_btnSign.SetToolTipText(pSess->m_loginResult.m_szSignature);
		}

		
	}
	CenterWindow(::GetDesktopWindow());
	return 0;
}


/**
 * @brief 处理被添加好友的通知消息
 * 
 * @param pResult 被添加好友的通知消息
 */
void CMainDlg::DoAddFriendNotifyReqMsg(C_WND_MSG_AddFriendNotifyRequest* pResult) 
{
	if (pResult == NULL)
		return ;

	CString strWindowTitle;
	CString strInfo;
	CAddFriendConfirmDlg AddFriendConfirmDlg;
	if (pResult->m_eOption == E_FRIEND_OPTION::E_AGREE_ADD)
	{
		strWindowTitle = g_strAppTitle.c_str();
		TCHAR szData[128] = { 0 };
		EncodeUtil::Utf8ToUnicode(pResult->m_szFriendName, szData, ARRAYSIZE(szData));

		TCHAR szNickName[128] = { 0 };
		EncodeUtil::Utf8ToUnicode(pResult->m_szFriendNickName, szNickName, ARRAYSIZE(szNickName));

		strInfo.Format(_T("您和%s(%s)已经是好友啦，开始聊天吧。"), szNickName, szData);
		AddFriendConfirmDlg.ShowAgreeButton(FALSE);
		AddFriendConfirmDlg.ShowRefuseButton(FALSE);
		AddFriendConfirmDlg.ShowOKButton(TRUE);
	}
	else if (pResult->m_eOption == E_FRIEND_OPTION::E_REFUSE_ADD)
	{
		strWindowTitle = g_strAppTitle.c_str();
		TCHAR szData[128] = { 0 };
		EncodeUtil::Utf8ToUnicode(pResult->m_szFriendName, szData, ARRAYSIZE(szData));

		TCHAR szNickName[128] = { 0 };
		EncodeUtil::Utf8ToUnicode(pResult->m_szFriendNickName, szNickName, ARRAYSIZE(szNickName));

		strInfo.Format(_T("您或者对方(%s(%s))拒绝了加好友请求。"), szNickName, szData);
		AddFriendConfirmDlg.ShowAgreeButton(FALSE);
		AddFriendConfirmDlg.ShowRefuseButton(FALSE);
		AddFriendConfirmDlg.ShowOKButton(TRUE);
	}

	AddFriendConfirmDlg.SetWindowInfo(strInfo);
	AddFriendConfirmDlg.SetWindowTitle(strWindowTitle);

	AddFriendConfirmDlg.DoModal(m_hWnd, 0);
	//
	{
		auto pSess = m_netProto;
		if (pSess) {
			pSess->SendAddFriendNotifyRsp(pResult->m_szMsgId);
		}
	}
}

/**
 * @brief 处理被添加好友的消息
 * 
 * @param pResult 被添加好友的结果消息
 */
void CMainDlg::DoAddFriendRecvReqMsg(C_WND_MSG_OperateFriendResult* pResult)
{
	if (pResult)
	{
		TCHAR szAccountName[64] = { 0 };
		EncodeUtil::Utf8ToUnicode(pResult->m_szAccountName, szAccountName, ARRAYSIZE(szAccountName));


		TCHAR szNickName[64] = { 0 };
		EncodeUtil::Utf8ToUnicode(pResult->m_szNickName, szNickName, ARRAYSIZE(szNickName));

		CString strWindowTitle;
		CString strInfo;
		CAddFriendConfirmDlg AddFriendConfirmDlg;

		strWindowTitle = _T("加好友请求");
		strInfo.Format(_T("%s(%s)请求您添加为好友，是否同意？"), szNickName, szAccountName);
		AddFriendConfirmDlg.ShowAgreeButton(TRUE);
		AddFriendConfirmDlg.ShowRefuseButton(TRUE);
		AddFriendConfirmDlg.ShowOKButton(FALSE);

		AddFriendConfirmDlg.SetWindowInfo(strInfo);
		AddFriendConfirmDlg.SetWindowTitle(strWindowTitle);

		AddFriendConfirmDlg.DoModal(m_hWnd, 0);

		if (AddFriendConfirmDlg.IsAgree())
		{
			if (m_netProto) {
				m_netProto->SendAddFriendRecvAsnyc(pResult->m_szAccountName, pResult->m_szMsgId, E_UI_OPERATE_FRIEND::Agree);
			}
		}
		else
		{
			if (m_netProto) {
				m_netProto->SendAddFriendRecvAsnyc(pResult->m_szAccountName, pResult->m_szMsgId, E_UI_OPERATE_FRIEND::Refuse);
			}
		}
	}
}

/**
 * @brief 处理收到好友的聊天消息()
 * 
 * @param pResult 好友的聊天消息
 */
void CMainDlg::DoRecvFriendChatTextMsg(C_WND_MSG_BuddyTextMessage* pResult)
{
	ShowBuddyChatDlg(pResult->m_strSender);
	auto item = m_mapBuddyChatDlg.find(pResult->m_strSender);
	if (item != m_mapBuddyChatDlg.end())
	{
		item->second->OnRecvMsg(pResult->m_uiMsg);
	}
	else
	{
		MessageBox(_T("DoRecvFriendChatTextMsg"), _T("DoRecvFriendChatTextMsg"));
	}
}



/**
 * @brief 处理消息的入口函数(以后可能会优化,因为添加好友的时候，在没有NotifyRsp的时候，就已经成为好友了，这个可能需要修改)
 * (core)
 */
void CMainDlg::DoMsgList()
{
	//std::lock_guard<std::mutex> lock(g_mutex);
	{
		for (std::size_t i = 0; i < m_msgList.size(); i++)
		{
			switch (m_msgList[i]->m_uType)
			{
			case E_UI_NET_DATA_TYPE::NET_DATA_OPERATE_FRIEND:
				{
					DoAddFriendRecvReqMsg(dynamic_cast<C_WND_MSG_OperateFriendResult*>(m_msgList[i]));
				}break;
				case E_UI_NET_DATA_TYPE::NET_DATA_ADD_FRIEND_NOTIFY_REQ:
				{
					DoAddFriendNotifyReqMsg(dynamic_cast<C_WND_MSG_AddFriendNotifyRequest*>(m_msgList[i]));
				}break;
				/*case E_UI_NET_DATA_TYPE::NET_DATA_FRIEND_RECV_TEXT_REQ:
				{
					DoRecvFriendChatTextMsg(dynamic_cast<C_WND_MSG_BuddyTextMessage*>(m_msgList[i]));
				}break;*/
				default:
				{

				}break;
			}
		}
		m_msgList.clear();
	}
}

/**
 * @brief 初始化消息MAP,用于由Proto到MainWindows的PostMessage(Core)
 * 
 */
void CMainDlg::InitMsgTypeMap()
{
	//Init Message Map
	if (m_netProto) {
		m_netProto->AddMap(E_MsgType::AddFriendRecvReq_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::AddFriendNotifyReq_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::GetFriendListRsp_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::FriendChatSendTxtMsgRsp_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::FriendChatReceiveTxtMsgReq_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::UserLoginRsp_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::NetFailedReport_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::NetRecoverReport_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::GetGroupListRsp_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::RecvGroupTextMsgReq_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::SendGroupTextMsgRsp_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::FriendSendFileMsgRsp_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::FriendRecvFileMsgReq_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::FriendNotifyFileMsgReq_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::UserKickOffReq_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::RecvGroupTextMsgRsp_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::GetFriendChatHistoryRsp_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::GetGroupChatHistoryRsp_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::FileTransProgressNotifyReq_Type, m_hWnd);
		m_netProto->AddMap(E_MsgType::FriendFileTransResultNotifyReq_Type, m_hWnd);
	}
}

/**
 * @brief 发送获取好友列表请求(core)
 * 
 * @return true 发送成功
 * @return false 发送失败
 */
bool CMainDlg::GetFriendList()
{
	if(nullptr != m_netProto){
		return m_netProto->GetFriendList();
	} else {
		return false;
	}
}

/**
 * @brief 判断用户是否离线(core)
 * 
 * @return true 离线
 * @return false 在线
 */
bool CMainDlg::IsOffLine(){
	if(nullptr != m_netProto){
		return m_netProto->GetStatus() == E_UI_ONLINE_STATUS::STATUS_OFFLINE;
	} else {
		return true;
	}
}

/**
 * @brief 发送获取群组列表的消息(core)
 * 
 * @return true 发送成功
 * @return false 发送失败
 */
bool CMainDlg::SendGetGroupList() {
	if (nullptr != m_netProto) {
		return m_netProto->SendGetGroupList();
	}
	else {
		return true;
	}
}