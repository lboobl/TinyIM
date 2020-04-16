#include "stdafx.h"
#include "BuddyInfoDlg.h"
#include "ChangePicHeadDlg.h"
#include "UI_USER_INFO.h"
#include <assert.h>
#include "Utils.h"
#include "CustomMsgDef.h"

CBuddyInfoDlg::CBuddyInfoDlg(void)
{
	m_hDlgIcon = NULL;
	m_hDlgSmallIcon = NULL;
}

CBuddyInfoDlg::~CBuddyInfoDlg(void)
{
}

/**
 * @brief 预处理消息
 * 
 * @param pMsg 
 * @return BOOL 
 */
BOOL CBuddyInfoDlg::PreTranslateMessage(MSG* pMsg)
{
	//支持回车键查找
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam==VK_RETURN || pMsg->wParam==VK_ESCAPE))
 	{
		PostMessage(WM_CLOSE, 0, 0);
		return TRUE;
 	}

	return CWindow::IsDialogMessage(pMsg);
}


/**
 * @brief 响应初始化对话框
 * 
 * @param wndFocus 
 * @param lInitParam 
 * @return BOOL 
 */
BOOL CBuddyInfoDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	
	m_hDlgIcon = AtlLoadIconImage(IDR_MAIN_FRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(m_hDlgIcon, TRUE);
	m_hDlgSmallIcon = AtlLoadIconImage(IDR_MAIN_FRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(m_hDlgSmallIcon, FALSE);

	Init();		// 初始化

	UpdateCtrls();
	CenterWindow();


	return TRUE;
}


/**
 * @brief 设置窗口标题
 * 
 * @param pszWindowTitle 窗口标题内容
 */
void CBuddyInfoDlg::SetWindowTitle(PCTSTR pszWindowTitle)
{
	m_strWindowTitle = pszWindowTitle;
}


/**
 * @brief 响应窗口关闭
 * 
 */
void CBuddyInfoDlg::OnClose()
{	
	//::PostMessage(m_lpFMGClient->m_UserMgr.m_hCallBackWnd, WM_CLOSE_BUDDYINFODLG, NULL, m_nUTalkUin);
}

/**
 * @brief 响应窗口销毁
 * 
 */
void CBuddyInfoDlg::OnDestroy()
{
	SetMsgHandled(FALSE);

	UnInit();	// 反初始化

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

	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
}

/**
 * @brief 响应OK按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyInfoDlg::OnBtnOK(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	SendMessage(WM_CLOSE);
	if (IsWindow())
	{
		ShowWindow(SW_HIDE);
	}
}

/**
 * @brief 更新控件
 * 
 */
void CBuddyInfoDlg::UpdateCtrls()
{
	C_UI_BuddyInfo* pBuddyInfo=NULL;//= m_lpFMGClient->m_UserMgr.m_BuddyList.GetBuddy(m_nUTalkUin);
	if(pBuddyInfo == NULL)
	{
		return;
	}	

	SetDlgItemText(IDC_ACCOUNT, pBuddyInfo->m_strAccount.c_str());
	SetDlgItemText(IDC_NICK_NAME, pBuddyInfo->m_strNickName.c_str());
	SetDlgItemText(IDC_SIGNATURE, pBuddyInfo->m_strSign.c_str());
	if(pBuddyInfo->m_nGender != 0)
	{
		SetDlgItemText(IDC_GENDER, _T("女"));
	}	
	else
	{
		SetDlgItemText(IDC_GENDER, _T("男"));
	}	

	long nYear = pBuddyInfo->m_nBirthday/10000;
	long nMonth = (pBuddyInfo->m_nBirthday-nYear*10000)/100;
	long nDay = pBuddyInfo->m_nBirthday-nYear*10000-nMonth*100;
	TCHAR szBirthday[16] = {0};
	if(nYear<1900 || nMonth<=0 || nMonth>12 || nDay<=0 || nDay>31)
	{	
		_tcscpy_s(szBirthday, ARRAYSIZE(szBirthday), _T("1990年6月9日"));
	}
	else
	{
		_stprintf_s(szBirthday, ARRAYSIZE(szBirthday), _T("%d年%d月%d日"), nYear, nMonth, nDay);
	}
	SetDlgItemText(IDC_BIRTHDAY, szBirthday);

	
	SetDlgItemText(IDC_ADDRESS, pBuddyInfo->m_strAddress.c_str());
	SetDlgItemText(IDC_PHONE, pBuddyInfo->m_strMobile.c_str());
	SetDlgItemText(IDC_MAIL, pBuddyInfo->m_strEmail.c_str());
}


/**
 * @brief 初始化
 * 
 * @return BOOL 
 */
BOOL CBuddyInfoDlg::Init()
{	
	m_SkinDlg.SetBgPic(_T("LoginPanel_window_windowBkg.png"), CRect(4, 69, 4, 33));
	m_SkinDlg.SetCloseSysBtnPic(_T("SysBtn\\btn_close_normal.png"), 
		_T("SysBtn\\btn_close_highlight.png"), _T("SysBtn\\btn_close_down.png"));
	m_SkinDlg.SubclassWindow(m_hWnd);
	m_SkinDlg.MoveWindow(0, 0, 420, 420, FALSE);
	m_SkinDlg.SetTitleText(m_strWindowTitle);

	HDC hDlgBgDC = m_SkinDlg.GetBgDC();

	
	m_btnOK.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnOK.SetTransparent(TRUE, hDlgBgDC);
	m_btnOK.SetBgPic(_T("Button\\btn_normal.png"), _T("Button\\btn_highlight.png"),
		_T("Button\\btn_down.png"), _T("Button\\btn_focus.png"));
	m_btnOK.SubclassWindow(GetDlgItem(IDOK));


	return TRUE;
}



/**
 * @brief 反初始化
 * 
 */
void CBuddyInfoDlg::UnInit()
{	
	if (m_btnOK.IsWindow())
	{
		m_btnOK.DestroyWindow();
	}	
}