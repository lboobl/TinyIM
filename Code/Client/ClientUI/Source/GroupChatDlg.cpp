/**
 * @file GroupChatDlg.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 群聊功能对话框
 * @version 0.1
 * @date 2020-02-27
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "stdafx.h"
#include "GroupChatDlg.h"
#include "UI_USER_INFO.h"
#include "UtilTime.h"
#include "ChatDlgCommon.h"
#include "GDIFactory.h"
#include "IULog.h"
#include "EncodingUtil.h"
#include "LoginSettingsDlg.h"
#include "UIText.h"
#include "net/IUProtocolData.h"
#include "UIDefaultValue.h"
#include "FileTool.h"


/**
 * @brief Construct a new CGroupChatDlg::CGroupChatDlg object
 * 
 */
CGroupChatDlg::CGroupChatDlg(void):m_userConfig(CUserConfig::GetInstance())
{
	m_netProto = nullptr;
	m_lpFaceList = NULL;
	m_lpCascadeWinManager = NULL;
	m_hMainDlg = NULL;

	m_hDlgIcon = NULL;
	m_hDlgSmallIcon = NULL;
	m_hRBtnDownWnd = NULL;

	memset(&m_ptRBtnDown, 0, sizeof(m_ptRBtnDown));
	m_pLastImageOle = NULL;

	m_cxPicBarDlg = GROUP_DLG_PIC_BAR_DLG_WIDTH;
	m_cyPicBarDlg = GROUP_DLG_PIC_BAR_DLG_HEIGHT;

	m_strGroupName = _T("群名称");
	m_nMemberCnt = 0;
	m_nOnlineMemberCnt = 0;


	m_HotRgn = NULL;

	m_bPressEnterToSendMessage = TRUE;
	m_bMsgLogWindowVisible = FALSE;

	m_nMsgLogIndexInToolbar = -1;
	m_nMsgLogRecordOffset = 1;	
	m_nMsgLogCurrentPageIndex = 0;

	m_bDraged = FALSE;

	::SetRectEmpty(&m_rtRichRecv);
	::SetRectEmpty(&m_rtMidToolBar);
	::SetRectEmpty(&m_rtSplitter);
	::SetRectEmpty(&m_rtRichSend);
}

/**
 * @brief Destroy the CGroupChatDlg::CGroupChatDlg object
 * 
 */
CGroupChatDlg::~CGroupChatDlg(void)
{
}

//
/**
 * @brief 预处理消息
 * 
 * @param pMsg 待处理的消息
 * @return BOOL 处理结果
 */
BOOL CGroupChatDlg::PreTranslateMessage(MSG* pMsg)
{
	if ( (pMsg->hwnd == m_richRecv.m_hWnd) || 
		 (pMsg->hwnd == m_richSend.m_hWnd) || 
		 (pMsg->hwnd == m_richMsgLog.m_hWnd) )
	{
		if (pMsg->message == WM_MOUSEMOVE)			// 发送/接收文本框的鼠标移动消息
		{
			if (OnRichEdit_MouseMove(pMsg))
			{
				return TRUE;
			}
		}
		else if (pMsg->message == WM_LBUTTONDBLCLK) // 发送/接收文本框的鼠标双击消息
		{
			if (OnRichEdit_LBtnDblClk(pMsg))
			{
				return TRUE;
			}
		}
		else if (pMsg->message == WM_RBUTTONDOWN)	// 发送/接收文本框的鼠标右键按下消息
		{
			if (OnRichEdit_RBtnDown(pMsg))
			{
				return TRUE;
			}
		}
		
		if (   (pMsg->hwnd == m_richSend.m_hWnd) && 
			   (pMsg->message == WM_KEYDOWN) && 
			   (pMsg->wParam == 'V') )	// 发送文本框的Ctrl+V消息
		{
			BOOL bCtrlPressed = ::GetAsyncKeyState(VK_CONTROL) & 0x8000;
			if(bCtrlPressed)
			{
				m_richSend.PasteSpecial(CF_TEXT);
				return TRUE;
			}
		}

		if ( (pMsg->hwnd == m_richSend.m_hWnd) && 
			 (pMsg->message == WM_KEYDOWN) && 
			 (pMsg->wParam == VK_RETURN) )	
		{
			BOOL bCtrlPressed = ::GetAsyncKeyState(VK_CONTROL) & 0x8000;
			if(m_bPressEnterToSendMessage && !bCtrlPressed)
			{
				::SendMessage(m_hWnd, WM_COMMAND, ID_BTN_SEND, 0);
				return TRUE;
			}
			else if(m_bPressEnterToSendMessage && bCtrlPressed)
			{
				::SendMessage(m_richSend.m_hWnd, WM_KEYDOWN, VK_RETURN, 0);
				return TRUE;
			}
			else if(!m_bPressEnterToSendMessage && bCtrlPressed)
			{
				::SendMessage(m_hWnd, WM_COMMAND, ID_BTN_SEND, 0);
				return TRUE;
			}
			else if(!m_bPressEnterToSendMessage && !bCtrlPressed)
			{
				::SendMessage(m_richSend.m_hWnd, WM_KEYDOWN, VK_RETURN, 0);
				return TRUE;
			}
		}

	}

	return CWindow::IsDialogMessage(pMsg);
}


/**
 * @brief 响应更新群信息
 * 
 */
void CGroupChatDlg::OnUpdateGroupInfo()
{

	UpdateDlgTitle();					// 更新对话框标题
	UpdateGroupNameCtrl();				// 更新群名称控件
}




/**
 * @brief 响应初始化对话框
 * 
 * @param wndFocus 
 * @param lInitParam 
 * @return BOOL 
 */
BOOL CGroupChatDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_lpCascadeWinManager->Add(m_hWnd, GROUP_CHAT_DLG_WIDTH, GROUP_CHAT_DLG_HEIGHT);

	// set icons
	m_hDlgIcon = AtlLoadIconImage(IDI_GROUP_CHAT_DLG_32, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(m_hDlgIcon, TRUE);
	m_hDlgSmallIcon = AtlLoadIconImage(IDI_GROUP_CHAT_DLG_16, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(m_hDlgSmallIcon, FALSE);

	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);

	Init();

	//TODO: 作用
	SetHotRgn();

	PostMessage(WM_SET_DLG_INIT_FOCUS, 0, 0);		// 设置对话框初始焦点

	OnSizeNotShowMsgHistory();
	return TRUE;
}


/**
 * @brief 响应拷贝数据,即Ctrl+C的操作
 * 
 * @param wnd 
 * @param pCopyDataStruct 
 * @return BOOL 
 */
BOOL CGroupChatDlg::OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct)
{
	if (NULL == pCopyDataStruct)
	{
		return FALSE;
	}

	switch (pCopyDataStruct->dwData)
	{
	case IPC_CODE_MSG_LOG_PASTE:			// 消息记录浏览窗口粘贴消息
		{
			if ( (pCopyDataStruct->lpData != NULL) &&
				 (pCopyDataStruct->cbData > 0) )
			{
				//AddMsgToSendEdit((LPCTSTR)pCopyDataStruct->lpData);
			}
		}
		break;

	case IPC_CODE_MSG_LOG_EXIT:			// 消息记录浏览窗口退出消息
		{
			m_tbMid.SetItemCheckState(11, FALSE);
			m_tbMid.Invalidate();
		}
		break;
	}

	return TRUE;
}

/**
 * @brief 
 * TODO: 需要调研
 * @param nIDCtl 
 * @param lpMeasureItemStruct 
 */
void CGroupChatDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	m_SkinMenu.OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

/**
 * @brief 响应绘制每一项的功能
 * 
 * @param nIDCtl 
 * @param lpDrawItemStruct 
 */
void CGroupChatDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	m_SkinMenu.OnDrawItem(nIDCtl, lpDrawItemStruct);
}

/**
 * @brief 响应获取最大最小化信息
 * 
 * @param lpMMI 
 */
void CGroupChatDlg::OnGetMinMaxInfo(LPMINMAXINFO lpMMI)
{
	if(m_bMsgLogWindowVisible)
	{
		lpMMI->ptMinTrackSize.x = GROUP_CHAT_DLG_EXTEND_WIDTH;
		lpMMI->ptMinTrackSize.y = GROUP_CHAT_DLG_HEIGHT;
	}
	else
	{
		lpMMI->ptMinTrackSize.x = GROUP_CHAT_DLG_WIDTH;
		lpMMI->ptMinTrackSize.y = GROUP_CHAT_DLG_HEIGHT;
	}
}


/**
 * @brief 响应窗口移动消息
 * 
 * @param ptPos 
 */
void CGroupChatDlg::OnMove(CPoint ptPos)
{
	SetMsgHandled(FALSE);

	m_lpCascadeWinManager->SetPos(m_hWnd, ptPos.x, ptPos.y);
}

/**
 * @brief 响应鼠标移动消息
 * 
 * @param nFlags 
 * @param point 
 */
void CGroupChatDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (::GetCapture() == m_SplitterCtrl.m_hWnd)
	{
		ReCaculateCtrlPostion(point.y);
	}
}

/**
 * @brief 响应显示历史消息的情况,群聊消息的窗口大小改变
 * 
 */
void CGroupChatDlg::OnSizeShowMsgHistory()
{

	SetMsgHandled(FALSE);

	CRect rcClient;
	GetClientRect(&rcClient);
	//RC_DLG = rcTitle + rcTopToolBar + rcShowArea
	//rcTitle.Width() == rcTopToolBar.Width() + rcShowArea.Width()
	//rcTotal.Height() = rcTitle.Height() + rcTopToolBar.Height() + rcShowArea.Height()

	//标题栏区域
	CRect rcTitle(rcClient.TopLeft(),
		CSize(rcClient.Width(),
			GROUP_DLG_TITLE_HEIGHT));

	//顶部工具栏区域
	CRect rcTopToolBar(
		CPoint(rcClient.left,
			rcTitle.bottom),

		CSize(rcClient.Width(),
			GROUP_DLG_TOOL_BAR_HEIGHT));
	{
		if (m_tbTop.IsWindow())
		{
			m_tbTop.MoveWindow(rcTopToolBar, TRUE);
		}
	}

	//显示区域
	CRect rcShowArea(
		CPoint(rcTopToolBar.left,
			rcTopToolBar.bottom),
		CSize(rcClient.Width(),
			rcClient.Height() - GROUP_DLG_TITLE_HEIGHT - GROUP_DLG_TOOL_BAR_HEIGHT));

	{
		//左边显示区域
		
			CRect rcLeftShowArea(
			CPoint(rcShowArea.left,
				rcShowArea.top),
			CSize(rcShowArea.Width() - GROUP_DLG_MSG_LOG_WIDTH -GROUP_DLG_IN_BORDER_WIDTH,
				rcShowArea.Height()));
		//左边显示区域分解
		{
			//消息接收区
			long recvEditHeight = static_cast<long>(rcLeftShowArea.Height()*GROUP_DLG_RECV_EDIT_PERCENT) - 2;
			CRect rcRecvEdit(
				CPoint(rcLeftShowArea.left + GROUP_DLG_OUT_BORDER_WIDTH,
					rcLeftShowArea.top),
				CSize(rcLeftShowArea.Width(),
					recvEditHeight));
			{
				if (m_richRecv.IsWindow())
				{
					m_richRecv.MoveWindow(rcRecvEdit, TRUE);
				}
			}

			//字体选择等工具栏
			CRect rcMidToolBar(
				CPoint(rcLeftShowArea.left + GROUP_DLG_OUT_BORDER_WIDTH,
					rcRecvEdit.bottom),
				CSize(rcLeftShowArea.Width() - GROUP_DLG_OUT_BORDER_WIDTH,
					GROUP_DLG_TOOL_BAR_HEIGHT));
			{
				//字体工具栏
				CRect rcFontToolBar(
					CPoint(rcMidToolBar.left,
						rcMidToolBar.top - GROUP_DLG_TOOL_BAR_HEIGHT),
					rcMidToolBar.Size());

				if (m_FontSelDlg.IsWindow())
				{
					if (m_tbMid.GetItemCheckState(0))
					{
						m_FontSelDlg.MoveWindow(rcFontToolBar, TRUE);
					}
					else
					{
						m_FontSelDlg.MoveWindow(rcFontToolBar, TRUE);
						m_FontSelDlg.ShowWindow(HIDE_WINDOW);
					}
				}

				if (m_tbMid.IsWindow())
				{
					m_tbMid.MoveWindow(rcMidToolBar, TRUE);
				}
			}


			//消息发送区域
			CRect rcSendEdit(
				CPoint(rcLeftShowArea.left + GROUP_DLG_OUT_BORDER_WIDTH,
					rcMidToolBar.bottom),
				CSize(rcLeftShowArea.Width() - GROUP_DLG_OUT_BORDER_WIDTH,
					rcLeftShowArea.Height() - rcRecvEdit.Height() - GROUP_DLG_TOOL_BAR_HEIGHT * 2));
			{
				if (m_richSend.IsWindow())
				{
					m_richSend.MoveWindow(rcSendEdit, TRUE);
				}
			}
			//底部工具栏区域
			CRect rcBottomToolBar(
				CPoint(rcLeftShowArea.left,
					rcSendEdit.bottom),
				CSize(rcLeftShowArea.Width(),
					GROUP_DLG_TOOL_BAR_HEIGHT));
			//从右到左计算
			{
				const long GROUP_DLG_ARROW_BTN_WIDTH = 77;  //按钮宽度
				const long GROUP_DLG_ARROW_BTN_HEIGHT = 25; //按钮高度
				const long GROUP_DLG_BTN_DISTANCE = 10;     //按钮间距
				CRect rcBtnArrow(
					CPoint(rcBottomToolBar.right - GROUP_DLG_BTN_DISTANCE - GROUP_DLG_ARROW_BTN_WIDTH,
						rcBottomToolBar.top + (rcTopToolBar.Height() - GROUP_DLG_ARROW_BTN_HEIGHT) / 2),
					CSize(static_cast<int>(GROUP_DLG_BTN_DISTANCE * 1.5), GROUP_DLG_ARROW_BTN_HEIGHT));

				CRect rcBtnSend(
					CPoint(rcBtnArrow.left - GROUP_DLG_ARROW_BTN_WIDTH,
						rcBtnArrow.top),
					CSize(GROUP_DLG_ARROW_BTN_WIDTH, GROUP_DLG_ARROW_BTN_HEIGHT));


				CRect rcBtnClose(
					CPoint(rcBtnSend.left - GROUP_DLG_BTN_DISTANCE - GROUP_DLG_ARROW_BTN_WIDTH,
						rcBtnArrow.top),
					CSize(GROUP_DLG_ARROW_BTN_WIDTH, GROUP_DLG_ARROW_BTN_HEIGHT));

				if (m_btnArrow.IsWindow())
				{
					m_btnArrow.MoveWindow(rcBtnArrow, TRUE);
				}


				if (m_btnSend.IsWindow())
				{
					m_btnSend.MoveWindow(rcBtnSend, TRUE);
				}

				if (m_btnClose.IsWindow())
				{
					m_btnClose.MoveWindow(rcBtnClose, TRUE);
				}
			}
		}


		//右边显示区域
		CRect rcRightShowArea(
			CPoint(rcLeftShowArea.right + GROUP_DLG_IN_BORDER_WIDTH,
				rcShowArea.top),
			CSize(GROUP_DLG_MSG_LOG_WIDTH,
				rcShowArea.Height() - GROUP_DLG_OUT_BORDER_WIDTH));
		
		{
			CRect rcMsgLog(
				rcRightShowArea.TopLeft(),
				CSize(rcRightShowArea.Width(),
					rcRightShowArea.Height() -GROUP_DLG_TOOL_BAR_HEIGHT));
			{
				m_richMsgLog.MoveWindow(rcMsgLog, TRUE);
			}
			CRect rcMsgToolBar(
				CPoint(rcRightShowArea.left, rcMsgLog.bottom),
				CSize(rcRightShowArea.Width(), GROUP_DLG_TOOL_BAR_HEIGHT));
			{
				CSize btnSize(28, 25);
				const int BTN_DISTANCE = 40;
				//聊天记录翻页四个按钮
				CRect rcFirstBtn(
					CPoint(rcMsgToolBar.left + BTN_DISTANCE,
						rcMsgToolBar.top + 5),
					btnSize);
				if (m_btnFirstMsgLog.IsWindow())
				{
					m_btnFirstMsgLog.ShowWindow(SW_SHOW);
					m_btnFirstMsgLog.MoveWindow(rcFirstBtn, TRUE);
				}

				CRect rcPrevBtn(
					CPoint(rcMsgToolBar.left + BTN_DISTANCE * 2,
						rcMsgToolBar.top + 5),
					btnSize);
				if (m_btnPrevMsgLog.IsWindow())
				{
					m_btnPrevMsgLog.ShowWindow(SW_SHOW);
					m_btnPrevMsgLog.MoveWindow(rcPrevBtn, TRUE);
				}

				//if (m_staMsgLogPage.IsWindow())
				//	m_staMsgLogPage.MoveWindow(rcRightShowArea.left +  180, rcMsgToolBar.top, 60, 25, TRUE);

				CRect rcNextBtn(
					CPoint(rcMsgToolBar.left + BTN_DISTANCE * 3,
						rcMsgToolBar.top + 5),
					btnSize);
				if (m_btnNextMsgLog.IsWindow())
				{
					m_btnNextMsgLog.ShowWindow(SW_SHOW);
					m_btnNextMsgLog.MoveWindow(rcNextBtn, TRUE);
				}

				CRect rcLastBtn(
					CPoint(rcMsgToolBar.left + BTN_DISTANCE * 4,
						rcMsgToolBar.top + 5),
					btnSize);
				if (m_btnLastMsgLog.IsWindow())
				{
					m_btnLastMsgLog.ShowWindow(SW_SHOW);
					m_btnLastMsgLog.MoveWindow(rcLastBtn, TRUE);
				}
			}
		}
	}
}

/**
 * @brief 响应隐藏历史消息的情况,群聊消息的窗口大小改变
 * TODO: OnSizeShowMsgHistory 合并 
 */
void CGroupChatDlg::OnSizeNotShowMsgHistory()
{

	SetMsgHandled(FALSE);

	CRect rcClient;
	GetClientRect(&rcClient);
	//RC_DLG = rcTitle + rcTopToolBar + rcShowArea
	//rcTitle.Width() == rcTopToolBar.Width() + rcShowArea.Width()
	//rcTotal.Height() = rcTitle.Height() + rcTopToolBar.Height() + rcShowArea.Height()

	//标题栏区域
	CRect rcTitle(rcClient.TopLeft(),
		CSize(rcClient.Width(),
			GROUP_DLG_TITLE_HEIGHT));

	//顶部工具栏区域
	CRect rcTopToolBar(
		CPoint(rcClient.left,
			rcTitle.bottom),

		CSize(rcClient.Width(),
			GROUP_DLG_TOOL_BAR_HEIGHT));
	{
		if (m_tbTop.IsWindow())
		{
			m_tbTop.MoveWindow(rcTopToolBar, TRUE);
		}
	}

	//显示区域
	CRect rcShowArea(
		CPoint(rcTopToolBar.left,
			rcTopToolBar.bottom),
		CSize(rcClient.Width(),
			rcClient.Height() - GROUP_DLG_TITLE_HEIGHT - GROUP_DLG_TOOL_BAR_HEIGHT));

	{
		//左边显示区域
		CRect rcLeftShowArea(
			CPoint(rcShowArea.left,
				rcShowArea.top),
			CSize(rcShowArea.Width() - GROUP_DLG_MEMBER_LIST_WIDTH,
				rcShowArea.Height()));
		//左边显示区域分解
		{
			//消息接收区
			long recvEditHeight = static_cast<long>(rcLeftShowArea.Height()*GROUP_DLG_RECV_EDIT_PERCENT) - 2;
			CRect rcRecvEdit(
				CPoint(rcLeftShowArea.left+GROUP_DLG_OUT_BORDER_WIDTH,
					rcLeftShowArea.top),
				CSize(rcLeftShowArea.Width(),
					recvEditHeight));
			{
				if (m_richRecv.IsWindow())
				{
					m_richRecv.MoveWindow(rcRecvEdit, TRUE);
				}
			}

			//字体选择等工具栏
			CRect rcMidToolBar(
				CPoint(rcLeftShowArea.left+GROUP_DLG_OUT_BORDER_WIDTH,
					rcRecvEdit.bottom),
				CSize(rcLeftShowArea.Width()-GROUP_DLG_OUT_BORDER_WIDTH,
					GROUP_DLG_TOOL_BAR_HEIGHT));
			{
				//字体工具栏
				CRect rcFontToolBar(
					CPoint(rcMidToolBar.left,
						rcMidToolBar.top-GROUP_DLG_TOOL_BAR_HEIGHT),
					rcMidToolBar.Size());

				if (m_FontSelDlg.IsWindow())
				{
					if (m_tbMid.GetItemCheckState(0))
					{
						m_FontSelDlg.MoveWindow(rcFontToolBar, TRUE);
					}
					else
					{
						m_FontSelDlg.MoveWindow(rcFontToolBar, TRUE);
						m_FontSelDlg.ShowWindow(HIDE_WINDOW);
					}
				}

				if (m_tbMid.IsWindow())
				{
					m_tbMid.MoveWindow(rcMidToolBar, TRUE);
				}
			}


			//消息发送区域
			CRect rcSendEdit(
				CPoint(rcLeftShowArea.left+GROUP_DLG_OUT_BORDER_WIDTH,
					rcMidToolBar.bottom),
				CSize(rcLeftShowArea.Width()-GROUP_DLG_OUT_BORDER_WIDTH,
					rcLeftShowArea.Height() - rcRecvEdit.Height() - GROUP_DLG_TOOL_BAR_HEIGHT * 2));
			{
				if (m_richSend.IsWindow())
				{
					m_richSend.MoveWindow(rcSendEdit, TRUE);
				}
			}
			//底部工具栏区域
			CRect rcBottomToolBar(
				CPoint(rcLeftShowArea.left,
					rcSendEdit.bottom),
				CSize(rcLeftShowArea.Width(),
					GROUP_DLG_TOOL_BAR_HEIGHT));
			//从右到左计算
			{
				const long GROUP_DLG_ARROW_BTN_WIDTH = 77;  //按钮宽度
				const long GROUP_DLG_ARROW_BTN_HEIGHT = 25; //按钮高度
				const long GROUP_DLG_BTN_DISTANCE = 10;     //按钮间距
				CRect rcBtnArrow(
					CPoint(rcBottomToolBar.right - GROUP_DLG_BTN_DISTANCE - GROUP_DLG_ARROW_BTN_WIDTH,
						rcBottomToolBar.top + (rcTopToolBar.Height() - GROUP_DLG_ARROW_BTN_HEIGHT) / 2),
					CSize(static_cast<int>(GROUP_DLG_BTN_DISTANCE * 1.5), GROUP_DLG_ARROW_BTN_HEIGHT));

				CRect rcBtnSend(
					CPoint(rcBtnArrow.left-GROUP_DLG_ARROW_BTN_WIDTH,
						rcBtnArrow.top),
					CSize(GROUP_DLG_ARROW_BTN_WIDTH, GROUP_DLG_ARROW_BTN_HEIGHT));


				CRect rcBtnClose(
					CPoint(rcBtnSend.left - GROUP_DLG_BTN_DISTANCE - GROUP_DLG_ARROW_BTN_WIDTH,
						rcBtnArrow.top),
					CSize(GROUP_DLG_ARROW_BTN_WIDTH, GROUP_DLG_ARROW_BTN_HEIGHT));

				if (m_btnArrow.IsWindow())
				{
					m_btnArrow.MoveWindow(rcBtnArrow, TRUE);
				}


				if (m_btnSend.IsWindow())
				{
					m_btnSend.MoveWindow(rcBtnSend, TRUE);
				}

				if (m_btnClose.IsWindow())
				{
					m_btnClose.MoveWindow(rcBtnClose, TRUE);
				}
			}
		}

		//右边显示区域
		CRect rcRightShowArea(
			CPoint(rcLeftShowArea.right+GROUP_DLG_IN_BORDER_WIDTH,
				rcShowArea.top),
			CSize(GROUP_DLG_MEMBER_LIST_WIDTH-GROUP_DLG_OUT_BORDER_WIDTH,
				rcShowArea.Height()-GROUP_DLG_OUT_BORDER_WIDTH));


		//右边显示区域分解
		{
			//群公告信息
			CRect rcGroupNotice(
				CPoint(rcRightShowArea.left,
					rcRightShowArea.top),
				CSize(rcRightShowArea.Width(),
					GROUP_DLG_NOTICE_HEIGHT));
			{
				CRect rcNoticeTitle(
					CPoint(rcRightShowArea.left,
						rcRightShowArea.top),
					CSize(rcRightShowArea.Width(),
						GROUP_DLG_STATIC_TEXT_HEIGHT));
				if (m_staMemoTitle.IsWindow())
				{
					//m_staMemoTitle.ShowWindow(HIDE_WINDOW);
					m_staMemoTitle.MoveWindow(rcNoticeTitle, TRUE);
				}

				CRect rcNoticeEdit(
					CPoint(rcNoticeTitle.left,
						rcNoticeTitle.bottom),
					CSize(rcRightShowArea.Width(),
						GROUP_DLG_NOTICE_HEIGHT-GROUP_DLG_STATIC_TEXT_HEIGHT));
				if (m_edtMemo.IsWindow())
				{
					//m_edtMemo.ShowWindow(HIDE_WINDOW);
					m_edtMemo.MoveWindow(rcNoticeEdit, TRUE);
				}
			}
			//群成员列表
			CRect rcGroupMember(
				CPoint(rcRightShowArea.left,
					rcGroupNotice.bottom),
				CSize(rcRightShowArea.Width(),
					rcRightShowArea.Height() - GROUP_DLG_NOTICE_HEIGHT));
			{
				CRect rcMemberTitle(
					CPoint(rcRightShowArea.left,
					rcGroupNotice.bottom),
					CSize(rcRightShowArea.Width(),
						GROUP_DLG_STATIC_TEXT_HEIGHT));
				if (m_staMemberTitle.IsWindow())
				{
					m_staMemberTitle.MoveWindow(rcMemberTitle, TRUE);
				}

				CRect rcMemberList(
					CPoint(rcRightShowArea.left,
						rcMemberTitle.bottom),
					CSize(rcRightShowArea.Width(),
						rcGroupMember.Height() - rcMemberTitle.Height()));

				if (m_GroupMemberListCtrl.IsWindow())
				{
					m_GroupMemberListCtrl.MoveWindow(rcMemberList, TRUE);
				}
			}
		}
	}
}


/**
 * @brief 响应窗口大小变化
 * TODO 此函数过于复杂，要分析每种情况分别处理和提取
 * @param nType 
 * @param size 
 */
void CGroupChatDlg::OnSize(UINT nType, CSize size)
{
	OnSizeNotShowMsgHistory();
	ResizeImageInRecvRichEdit();
}


/**
 * @brief 
 * TODO:函数不明白干嘛的,好像是合并什么区域
 */
void CGroupChatDlg::SetHotRgn()
{
	RECT rtWindow;
	HRGN hTemp;
	m_picHead.GetClientRect(&rtWindow);
	m_HotRgn = ::CreateRectRgnIndirect(&rtWindow);

	m_lnkGroupName.GetClientRect(&rtWindow);
	hTemp = ::CreateRectRgnIndirect(&rtWindow);
	::CombineRgn(m_HotRgn, m_HotRgn, hTemp, RGN_AND);
	::DeleteObject(hTemp);

	m_staGroupCategory.GetClientRect(&rtWindow);
	hTemp = ::CreateRectRgnIndirect(&rtWindow);
	::CombineRgn(m_HotRgn, m_HotRgn, hTemp, RGN_AND);
	::DeleteObject(hTemp);

	m_richRecv.GetClientRect(&rtWindow);
	hTemp = ::CreateRectRgnIndirect(&rtWindow);
	::CombineRgn(m_HotRgn, m_HotRgn, hTemp, RGN_AND);
	::DeleteObject(hTemp);

	m_richSend.GetClientRect(&rtWindow);
	hTemp = ::CreateRectRgnIndirect(&rtWindow);
	::CombineRgn(m_HotRgn, m_HotRgn, hTemp, RGN_AND);
	::DeleteObject(hTemp);

	//m_richMsgLog.GetClientRect(&rtWindow);
	//hTemp = ::CreateRectRgnIndirect(&rtWindow);
	//::CombineRgn(m_HotRgn, m_HotRgn, hTemp, RGN_AND);
	//::DeleteObject(hTemp);

	//m_btnMsgLog.GetClientRect(&rtWindow);
	//hTemp = ::CreateRectRgnIndirect(&rtWindow);
	//::CombineRgn(m_HotRgn, m_HotRgn, hTemp, RGN_AND);
	//::DeleteObject(hTemp);

	m_btnClose.GetClientRect(&rtWindow);
	hTemp = ::CreateRectRgnIndirect(&rtWindow);
	::CombineRgn(m_HotRgn, m_HotRgn, hTemp, RGN_AND);
	::DeleteObject(hTemp);

	m_btnSend.GetClientRect(&rtWindow);
	hTemp = ::CreateRectRgnIndirect(&rtWindow);
	::CombineRgn(m_HotRgn, m_HotRgn, hTemp, RGN_AND);
	::DeleteObject(hTemp);

	m_btnArrow.GetClientRect(&rtWindow);
	hTemp = ::CreateRectRgnIndirect(&rtWindow);
	::CombineRgn(m_HotRgn, m_HotRgn, hTemp, RGN_AND);
	::DeleteObject(hTemp);
	
	m_staMemberTitle.GetClientRect(&rtWindow);
	hTemp = ::CreateRectRgnIndirect(&rtWindow);
	::CombineRgn(m_HotRgn, m_HotRgn, hTemp, RGN_AND);
	::DeleteObject(hTemp);
	
	m_GroupMemberListCtrl.GetClientRect(&rtWindow);
	hTemp = ::CreateRectRgnIndirect(&rtWindow);
	::CombineRgn(m_HotRgn, m_HotRgn, hTemp, RGN_AND);
	::DeleteObject(hTemp);

	m_staMemoTitle.GetClientRect(&rtWindow);
	hTemp = ::CreateRectRgnIndirect(&rtWindow);
	::CombineRgn(m_HotRgn, m_HotRgn, hTemp, RGN_AND);
	::DeleteObject(hTemp);

	m_edtMemo.GetClientRect(&rtWindow);
	hTemp = ::CreateRectRgnIndirect(&rtWindow);
	::CombineRgn(m_HotRgn, m_HotRgn, hTemp, RGN_AND);
	::DeleteObject(hTemp);

	m_tbMid.GetClientRect(&rtWindow);
	hTemp = ::CreateRectRgnIndirect(&rtWindow);
	::CombineRgn(m_HotRgn, m_HotRgn, hTemp, RGN_AND);
	::DeleteObject(hTemp);

	m_SkinDlg.SetHotRegion(m_HotRgn);
}

/**
 * @brief 响应设置回车键发送消息的设置
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnPressEnterMenuItem(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_bPressEnterToSendMessage = TRUE;
	m_userConfig.EnablePressEnterToSend(TRUE);
}

/**
 * @brief 设置 Ctrl+回车键 发送消息
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnPressCtrlEnterMenuItem(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_bPressEnterToSendMessage = FALSE;
	m_userConfig.EnablePressEnterToSend(m_bPressEnterToSendMessage);
}



/**
 * @brief 响应群聊对话框关闭
 * 
 */
void CGroupChatDlg::OnClose()
{
	DestroyWindow();
}

/**
 * @brief 响应群聊对话框销毁
 * 
 */
void CGroupChatDlg::OnDestroy()
{
	SetMsgHandled(FALSE);

	CloseMsgLogBrowser();

	m_lpCascadeWinManager->Del(m_hWnd);

	UnInit();	// 反初始化控件

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
 * @brief 响应“字体选择工具栏”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnBtn_Font(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	RECT rtRichRecv;
	::GetWindowRect(m_richRecv, &rtRichRecv);
	::ScreenToClient(m_hWnd, rtRichRecv);	
	OnSizeNotShowMsgHistory();
	if (BN_PUSHED == uNotifyCode)
	{
		m_FontSelDlg.ShowWindow(SW_SHOW);
	}
	else if (BN_UNPUSHED == uNotifyCode)
	{
		m_FontSelDlg.ShowWindow(SW_HIDE);
	}
}


/**
 * @brief 响应“表情”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnBtn_Face(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (BN_PUSHED == uNotifyCode)
	{
		m_FaceSelDlg.SetFaceList(m_lpFaceList);
		if (!m_FaceSelDlg.IsWindow())
		{
			m_FaceSelDlg.Create(m_hWnd);

			CRect rcBtn;
			m_tbMid.GetItemRectByIndex(1, rcBtn);
			m_tbMid.ClientToScreen(&rcBtn);

			int cx = 432;
            int cy = 306;
			int x = rcBtn.left - cx / 2;
			int y = rcBtn.top - cy;

			m_FaceSelDlg.SetWindowPos(NULL, x, y, cx, cy, NULL);
			m_FaceSelDlg.ShowWindow(SW_SHOW);
		}
	}
	else if (BN_UNPUSHED == uNotifyCode)
	{

	}
}

/**
 * @brief 响应工具栏的“发送图片”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnBtn_Image(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	BOOL bOpenFileDialog = TRUE;
	LPCTSTR lpszDefExt = NULL;
	LPCTSTR lpszFileName = NULL;
	DWORD dwFlags = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR|OFN_EXTENSIONDIFFERENT;
	LPCTSTR lpszFilter = _T("图像文件(*.bmp;*.jpg;*.jpeg;*.gif;*.png)\0*.bmp;*.jpg;*.jpeg;*.gif;*.png\0\0");;
	HWND hWndParent = m_hWnd;

	CFileDialog fileDlg(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, hWndParent);
	fileDlg.m_ofn.lpstrTitle = _T("打开图片");
	if (fileDlg.DoModal() == IDOK)
	{
		_RichEdit_InsertFace(m_richSend.m_hWnd, fileDlg.m_ofn.lpstrFile, -1, -1);
		m_richSend.SetFocus();
	}
}



/**
 * @brief 响应工具栏的“截图工具”
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnBtn_ScreenShot(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	DWORD dwSucceedExitCode = 2;
	CString strCatchScreen;
	strCatchScreen.Format(_T("%sCatchScreen.exe %u"), g_szHomePath, dwSucceedExitCode);
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};
	if(!CreateProcess(NULL, strCatchScreen.GetBuffer(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		::MessageBox(m_hWnd, _T("启动截图工具失败！"), g_strAppTitle.c_str(), MB_OK|MB_ICONERROR);
	}
	if (pi.hProcess != NULL)
	{
		::WaitForSingleObject(pi.hProcess, INFINITE);

		dwSucceedExitCode = 0;

		if (::GetExitCodeProcess(pi.hProcess, &dwSucceedExitCode) && dwSucceedExitCode == 2)
		{
			m_richSend.PasteSpecial(CF_TEXT);
		}
	}
}


/**
 * @brief 响应“点击另存为”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnBtn_SaveAs(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	OnMenu_SaveAs(uNotifyCode, nID, wndCtl);
}

/**
 * @brief 响应“关闭”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnBtn_Close(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	PostMessage(WM_CLOSE);
}


/**
 * @brief 响应“发送”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnBtn_Send(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCustomPicCnt = RichEdit_GetCustomPicCount(m_richSend.m_hWnd);
	if (nCustomPicCnt > 1)
	{
		MessageBox(_T("每条消息最多包含1张图片，多张图片请分条发送。"), _T("提示"));
		return;
	}


	{
		RichEditMsgList msgList = RichEdit_GetMsg(m_richSend.m_hWnd);
		if (m_netProto) {
			m_netProto->SendGroupChatTextMsg(m_strGroupId, msgList, m_FontSelDlg.GetFontInfo());
		}
		else {

		}
	}
	m_richSend.SetWindowText(_T(""));
	m_richSend.SetFocus();
}


/**
 * @brief 响应“箭头”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnBtn_Arrow(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(5);
	if (PopupMenu.IsMenu())
	{
		CRect rc;
		m_btnArrow.GetClientRect(&rc);
		m_btnArrow.ClientToScreen(&rc);
		m_bPressEnterToSendMessage = m_userConfig.IsEnablePressEnterToSend();
		if(m_bPressEnterToSendMessage)
		{
			PopupMenu.CheckMenuItem(ID_GROUP_CHAT_DLG_PRESS_ENTER_MENU, MF_CHECKED);
			PopupMenu.CheckMenuItem(ID_GROUP_CHAT_DLG_PRESS_CTRL_ENTER_MENU, MF_UNCHECKED);
		}
		else
		{
			PopupMenu.CheckMenuItem(ID_GROUP_CHAT_DLG_PRESS_ENTER_MENU, MF_UNCHECKED);
			PopupMenu.CheckMenuItem(ID_GROUP_CHAT_DLG_PRESS_CTRL_ENTER_MENU, MF_CHECKED);
		}

		PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, 
			rc.left, rc.bottom + 4, m_hWnd, &rc);
	}
}

/**
 * @brief 响应工具栏的按钮被按下,此类将顶部工具栏,中部工具栏和底部工具栏的消息处理都放到这个函数里了
 * TODO: switch的编码和GetSubMenu的编码部分要重构
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CGroupChatDlg::OnToolbarDropDown(LPNMHDR pnmh)
{
	NMTOOLBAR* pnmtb = (NMTOOLBAR*)pnmh;
	CSkinMenu PopupMenu;
	CRect rc(pnmtb->rcButton);

	switch (pnmtb->iItem)
	{
	case 101:
	{
		m_tbTop.ClientToScreen(&rc);
		PopupMenu = m_SkinMenu.GetSubMenu(0);
	}break;

	case 108:
	{
		m_tbTop.ClientToScreen(&rc);
		PopupMenu = m_SkinMenu.GetSubMenu(1);
	}break;

	case 208:
	{
		m_tbMid.ClientToScreen(&rc);
		PopupMenu = m_SkinMenu.GetSubMenu(2);
	}break;

	case 209:
	{
		m_tbMid.ClientToScreen(&rc);
		PopupMenu = m_SkinMenu.GetSubMenu(3);
	}	break;

	case 212:
	{
		m_tbMid.ClientToScreen(&rc);
		PopupMenu = m_SkinMenu.GetSubMenu(4);
	}break;

	default:
	{
		return 0;
	}
	}

	if (PopupMenu.IsMenu())
	{
		PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, 
			rc.left,
			rc.bottom + 4,
			m_hWnd, 
			&rc);
	}

	return 0;
}


/**
 * @brief 响应更新字体信息
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CGroupChatDlg::OnUpdateFontInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	C_UI_FontInfo fontInfo = m_FontSelDlg.GetFontInfo();
	RichEdit_SetDefFont(m_richSend.m_hWnd, fontInfo.m_strName.c_str(),
		fontInfo.m_nSize, fontInfo.m_clrText, fontInfo.m_bBold,
		fontInfo.m_bItalic, fontInfo.m_bUnderLine, FALSE);
	return 0;
}


/**
 * @brief 响应“表情”控件选取消息
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CGroupChatDlg::OnFaceCtrlSel(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nFaceId = m_FaceSelDlg.GetSelFaceId();
	int nFaceIndex = m_FaceSelDlg.GetSelFaceIndex();
	CString strFileName = m_FaceSelDlg.GetSelFaceFileName();
	if (!strFileName.IsEmpty())
	{
		_RichEdit_InsertFace(m_richSend.m_hWnd, strFileName, nFaceId, nFaceIndex);
		m_richSend.SetFocus();
	}

	m_tbMid.SetItemCheckState(1, FALSE);
	m_tbMid.Invalidate();

	return 0;
}

/**
 * @brief 设置对话框初始焦点
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CGroupChatDlg::OnSetDlgInitFocus(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_richSend.SetFocus();
	return 0;
}

/**
 * @brief 响应“群成员”列表双击消息
 * 
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CGroupChatDlg::OnGMemberList_DblClick(LPNMHDR pnmh)
{
	int nCurSel = m_GroupMemberListCtrl.GetCurSelItemIndex();
	if (nCurSel != -1)
	{
		MessageBox(_T("暂不支持"), _T("TinyIM"));
	}
	return 0;
}


/**
 * @brief 响应“群成员”列表右键单击消息
 * TODO: GetSubMenu的地方需要修改
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CGroupChatDlg::OnGMemberList_RClick(LPNMHDR pnmh)
{
	int nCurSel = m_GroupMemberListCtrl.GetCurSelItemIndex();
	if (nCurSel != -1)
	{
		DWORD dwPos = GetMessagePos();
		CPoint point(LOWORD(dwPos), HIWORD(dwPos));

		CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(6);
		PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, m_hWnd);
	}
	return 0;
}

/**
 * @brief 响应“剪切”菜单
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnMenu_Cut(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_richSend.Cut();
}


/**
 * @brief 响应弹出的右键“复制”菜单
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnMenu_Copy(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	HWND hWnd = GetFocus();
	if (hWnd == m_richSend.m_hWnd)
	{
		m_richSend.Copy();
	}
	else if (hWnd == m_richRecv.m_hWnd)
	{
		m_richRecv.Copy();
	}
	else if (hWnd == m_richMsgLog.m_hWnd)
	{
		m_richMsgLog.Copy();
	}
}

/**
 * @brief 响应“粘贴”菜单
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnMenu_Paste(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_richSend.PasteSpecial(CF_TEXT);
}


/**
 * @brief 响应“全部选择”菜单
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnMenu_SelAll(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	HWND hWnd = GetFocus();
	if (hWnd == m_richSend.m_hWnd)
	{
		m_richSend.SetSel(0, -1);
	}
	else if (hWnd == m_richRecv.m_hWnd)
	{
		m_richRecv.SetSel(0, -1);
	}
}

/**
 * @brief 响应接收消息文本框的“清屏”菜单
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnMenu_Clear(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_richRecv.SetWindowText(_T(""));
}


/**
 * @brief 响应“显示比例”菜单,完成图片的放大和缩小
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnMenu_ZoomRatio(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	switch (nID)
	{
	case ID_MENU_ZOOM_RATIO_400:
	{
		::SendMessage(m_richRecv.m_hWnd, EM_SETZOOM, 16, 4);
	}break;
	case ID_MENU_ZOOM_RATIO_200:
	{
		::SendMessage(m_richRecv.m_hWnd, EM_SETZOOM, 8, 4);
	}break;
	case ID_MENU_ZOOM_RATIO_150:
	{
		::SendMessage(m_richRecv.m_hWnd, EM_SETZOOM, 6, 4);
	}break;
	case ID_MENU_ZOOM_RATIO_125:
	{
		::SendMessage(m_richRecv.m_hWnd, EM_SETZOOM, 5, 4);
	}break;
	case ID_MENU_ZOOM_RATIO_100:
	{
		::SendMessage(m_richRecv.m_hWnd, EM_SETZOOM, 0, 0);
	}break;
	case ID_MENU_ZOOM_RATIO_75:
	{
		::SendMessage(m_richRecv.m_hWnd, EM_SETZOOM, 3, 4);
	}break;
	case ID_MENU_ZOOM_RATIO_50:
	{
		::SendMessage(m_richRecv.m_hWnd, EM_SETZOOM, 1, 2);
	}break;
	default:
	{
		return;
	}
	}

	CSkinMenu menuPopup = m_SkinMenu.GetSubMenu(8);
	for (int i = ID_MENU_ZOOM_RATIO_400; i <= ID_MENU_ZOOM_RATIO_50; i++)
	{
		if (i != nID)
		{
			menuPopup.CheckMenuItem(i, MF_BYCOMMAND | MF_UNCHECKED);
		}
		else
		{
			menuPopup.CheckMenuItem(i, MF_BYCOMMAND | MF_CHECKED);
		}
	}	
}


/**
 * @brief 响应“图片另存为”菜单
 *
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnMenu_SaveAs(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	IImageOle* pImageOle = NULL;

	BOOL bRet = RichEdit_GetImageOle(m_hRBtnDownWnd, m_ptRBtnDown, &pImageOle);
	if (!bRet || NULL == pImageOle)
	{
		return;
	}

	CString strFileName;

	BSTR bstrFileName = NULL;
	HRESULT hr = pImageOle->GetFileName(&bstrFileName);
	if (SUCCEEDED(hr))
	{
		strFileName = bstrFileName;
	}

	if (bstrFileName != NULL)
	{
		::SysFreeString(bstrFileName);
	}

	TCHAR cFileName[MAX_PATH] = { 0 };
	BOOL bOpenFileDialog = FALSE;
	LPCTSTR lpszDefExt;
	CString strFileNamePrefix;
	GenerateChatImageSavedName(strFileNamePrefix);
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR | OFN_EXTENSIONDIFFERENT;
	LPCTSTR lpszFilter;
	HWND hWndParent = m_hWnd;

	GUID guid = { 0 };
	hr = pImageOle->GetRawFormat(&guid);

	if (InlineIsEqualGUID(guid, Gdiplus::ImageFormatJPEG))
	{
		lpszDefExt = _T(".jpg");
		lpszFilter = _T("图像文件(*.jpg)\0*.jpg\0图像文件(*.bmp)\0*.bmp\0\0");
	}
	else if (InlineIsEqualGUID(guid, Gdiplus::ImageFormatPNG))
	{
		lpszDefExt = _T(".png");
		lpszFilter = _T("图像文件(*.png)\0*.png\0\0");
	}
	else if (InlineIsEqualGUID(guid, Gdiplus::ImageFormatGIF))
	{
		lpszDefExt = _T(".gif");
		lpszFilter = _T("图像文件(*.gif)\0*.gif\0图像文件(*.jpg)\0*.jpg\0图像文件(*.bmp)\0*.bmp\0\0");
	}
	else
	{
		lpszDefExt = _T(".jpg");
		lpszFilter = _T("图像文件(*.jpg)\0*.jpg\0图像文件(*.bmp)\0*.bmp\0\0");
	}

	CFileDialog fileDlg(bOpenFileDialog, lpszDefExt, strFileNamePrefix, dwFlags, lpszFilter, hWndParent);
	fileDlg.m_ofn.lpstrTitle = _T("保存图片");
	if (fileDlg.DoModal() == IDOK)
	{
		CString strSavePath = fileDlg.m_ofn.lpstrFile;
		CString strExtName = (_T(".") + Hootina::CPath::GetExtension(strSavePath)).c_str();
		GUID guid2 = GetFileTypeGuidByExtension(strExtName);

		if (InlineIsEqualGUID(guid, guid2))
		{
			CopyFile(strFileName, strSavePath, FALSE);
		}
		else
		{
			BSTR bstrSavePath = ::SysAllocString(strSavePath);
			if (bstrSavePath != NULL)
			{
				pImageOle->SaveAsFile(bstrSavePath);
				::SysFreeString(bstrSavePath);
			}
		}
	}

	if (pImageOle != NULL)
	{
		pImageOle->Release();
	}
}

/**
 * @brief 响应 “发送/接收” 文本框的鼠标移动消息
 * 
 * @param pMsg 
 * @return BOOL 
 */
BOOL CGroupChatDlg::OnRichEdit_MouseMove(MSG* pMsg)
{
	IImageOle* pNewImageOle = NULL;
	RECT rc = {0};

	POINT pt = {LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)};
	IImageOle* pImageOle = NULL;
	BOOL bRet = RichEdit_GetImageOle(pMsg->hwnd, pt, &pImageOle);
	if (bRet && pImageOle != NULL)
	{
		pNewImageOle = pImageOle;
		pImageOle->GetObjectRect(&rc);
	}
	
	if (pImageOle != NULL)
	{
		pImageOle->Release();
	}
	if (m_pLastImageOle != pNewImageOle)
	{
		m_pLastImageOle = pNewImageOle;
		if (m_pLastImageOle != NULL)
		{
			m_hRBtnDownWnd = pMsg->hwnd;
			m_ptRBtnDown = pt;

			if (!m_PicBarDlg.IsWindow())
			{
				m_PicBarDlg.Create(m_hWnd);
			}

			RECT rc2 = {0};
			::GetClientRect(pMsg->hwnd, &rc2);
			POINT pt = {rc.right, rc.bottom-m_cyPicBarDlg};
			if (pt.x < rc2.left)
			{
				pt.x = rc2.left;
			}
			
			if (pt.x > rc2.right)
			{
				pt.x = rc2.right;
			}
			
			if (pt.y > rc2.bottom - m_cyPicBarDlg)
			{
				pt.y = rc2.bottom - m_cyPicBarDlg;
			}
			::ClientToScreen(pMsg->hwnd, &pt);

			::SetWindowPos(m_PicBarDlg.m_hWnd, NULL, pt.x, pt.y, m_cxPicBarDlg, m_cyPicBarDlg, SWP_NOACTIVATE|SWP_SHOWWINDOW);
		}
		else
		{
			::ShowWindow(m_PicBarDlg.m_hWnd, SW_HIDE);
		}
	}
	return FALSE;
}

/**
 * @brief  响应“接收文本框”的鼠标双击消息
 * 
 * @param pMsg 
 * @return BOOL 
 */
BOOL CGroupChatDlg::OnRichEdit_LBtnDblClk(MSG* pMsg)
{
	POINT pt = {GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)};

	IImageOle* pImageOle = NULL;
	BOOL bRet = RichEdit_GetImageOle(pMsg->hwnd, pt, &pImageOle);
	if (bRet && pImageOle != NULL)					// 双击的是图片
	{
		LONG nFaceId = -1, nFaceIndex = -1;
		pImageOle->GetFaceId(&nFaceId);
		pImageOle->GetFaceIndex(&nFaceIndex);
		if (-1 == nFaceId && -1 == nFaceIndex)		// 非系统表情
		{
			BSTR bstrFileName = NULL;				// 获取图片文件名
			HRESULT hr = pImageOle->GetFileName(&bstrFileName);
			if (SUCCEEDED(hr))						// 调用图片浏览器程序打开图片
			{
				CString strExeName = Hootina::CPath::GetAppPath().c_str();
				strExeName += _T("picView.exe");

				CString strArg = _T("\"");
				strArg += bstrFileName;
				strArg += _T("\"");

				if (Hootina::CPath::IsFileExist(strExeName))
				{
					HWND hWnd = ::FindWindow(_T("DUI_WINDOW"), _T("ImageView"));
					if (::IsWindow(hWnd))
					{
						::SendMessage(hWnd, WM_CLOSE, 0, 0);
					}
					::ShellExecute(NULL, NULL, strExeName, strArg, NULL, SW_SHOWNORMAL);
				}
				else
				{
					::ShellExecute(NULL, _T("open"), bstrFileName, NULL, NULL, SW_SHOWNORMAL);
				}
			}
			if (bstrFileName != NULL)
			{
				::SysFreeString(bstrFileName);
			}
		}
	}
	
	if (pImageOle != NULL)
	{
		pImageOle->Release();
	}
	return bRet;
}


/**
 * @brief 响应“发送/接收/聊天记录 文本框”的鼠标右键按下消息
 * 
 * @param pMsg 
 * @return BOOL 
 */
BOOL CGroupChatDlg::OnRichEdit_RBtnDown(MSG* pMsg)
{
	if (pMsg->hwnd == m_richSend.m_hWnd)
	{
		m_hRBtnDownWnd = pMsg->hwnd;
		m_ptRBtnDown.x = GET_X_LPARAM(pMsg->lParam);
		m_ptRBtnDown.y = GET_Y_LPARAM(pMsg->lParam);

		CSkinMenu menuPopup = m_SkinMenu.GetSubMenu(7);

		UINT nSel = ((m_richSend.GetSelectionType() != SEL_EMPTY) ? 0 : MF_GRAYED);
		menuPopup.EnableMenuItem(ID_MENU_CUT, MF_BYCOMMAND|nSel);
		menuPopup.EnableMenuItem(ID_MENU_COPY, MF_BYCOMMAND|nSel);

		UINT nPaste = (m_richSend.CanPaste() ? 0 : MF_GRAYED) ;
		menuPopup.EnableMenuItem(ID_MENU_PASTE, MF_BYCOMMAND|nPaste);

		IImageOle* pImageOle = NULL;
		BOOL bRet = RichEdit_GetImageOle(pMsg->hwnd, m_ptRBtnDown, &pImageOle);
		UINT nSaveAs = ((bRet && pImageOle != NULL) ? 0 : MF_GRAYED) ;
		menuPopup.EnableMenuItem(ID_MENU_SAVE_AS, MF_BYCOMMAND|nSaveAs);
		if (pImageOle != NULL)
		{
			pImageOle->Release();
		}

		menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
			pMsg->pt.x, pMsg->pt.y, m_hWnd, NULL);
	}
	else if (pMsg->hwnd == m_richRecv.m_hWnd)
	{
		m_hRBtnDownWnd = pMsg->hwnd;
		m_ptRBtnDown.x = GET_X_LPARAM(pMsg->lParam);
		m_ptRBtnDown.y = GET_Y_LPARAM(pMsg->lParam);

		CSkinMenu menuPopup = m_SkinMenu.GetSubMenu(8);

		UINT nSel = ((m_richRecv.GetSelectionType() != SEL_EMPTY) ? 0 : MF_GRAYED);
		menuPopup.EnableMenuItem(ID_MENU_COPY, MF_BYCOMMAND|nSel);
		//menuPopup.EnableMenuItem(ID_MENU_CLEAR, MF_BYCOMMAND|nSel);

		IImageOle* pImageOle = NULL;
		BOOL bRet = RichEdit_GetImageOle(pMsg->hwnd, m_ptRBtnDown, &pImageOle);
		UINT nSaveAs = ((bRet && pImageOle != NULL) ? 0 : MF_GRAYED) ;
		menuPopup.EnableMenuItem(ID_MENU_SAVE_AS, MF_BYCOMMAND|nSaveAs);
		if (pImageOle != NULL)
		{
			pImageOle->Release();
		}

		menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
			pMsg->pt.x, pMsg->pt.y, m_hWnd, NULL);
	}
	else if(pMsg->hwnd == m_richMsgLog.m_hWnd)
	{
		CSkinMenu menuPopup = m_SkinMenu.GetSubMenu(9);
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pMsg->pt.x, pMsg->pt.y, m_hWnd, NULL);
	}

	return FALSE;
}


/**
 * @brief 更新群聊对话框标题栏,群名称修改后会被调用
 * 
 */
void CGroupChatDlg::UpdateDlgTitle()
{
	SetWindowText(m_strGroupName);
}


/**
 * @brief 更新“群名称”控件
 * 
 * @return BOOL 
 */
BOOL CGroupChatDlg::UpdateGroupNameCtrl()
{
	m_lnkGroupName.SetLabel(m_strGroupName);
	return TRUE;
}

/**
 * @brief 初始化对话框顶部工具栏,主要是发送文件等功能
 * 
 * @return BOOL 
 */
BOOL CGroupChatDlg::InitTopToolBar()
{
	
	{
		int nIndex = m_tbTop.AddItem(IDC_BTN_SEND_FILE, STBI_STYLE_DROPDOWN);
		m_tbTop.SetItemSize(nIndex, 38, 28, 28, 10);
		m_tbTop.SetItemPadding(nIndex, 1);
		m_tbTop.SetItemToolTipText(nIndex, _T("传送文件"));
		m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"),
			_T("aio_toolbar_down.png"), CRect(3, 3, 3, 3));
		m_tbTop.SetItemLeftBgPic(nIndex, _T("aio_toolbar_leftnormal.png"),
			_T("aio_toolbar_leftdown.png"), CRect(0, 0, 0, 0));
		m_tbTop.SetItemRightBgPic(nIndex, _T("aio_toolbar_rightnormal.png"),
			_T("aio_toolbar_rightdown.png"), CRect(0, 0, 0, 0));
		m_tbTop.SetItemArrowPic(nIndex, _T("aio_littletoolbar_arrow.png"));
		m_tbTop.SetItemIconPic(nIndex, _T("BuddyTopToolBar\\sendfile.png"));
	}

	m_tbTop.SetLeftTop(0, 0);
	m_tbTop.SetTransparent(TRUE, m_SkinDlg.GetBgDC());

	CRect rcTopToolBar(3, 70, GROUP_CHAT_DLG_WIDTH-1, 102);
	m_tbTop.Create(m_hWnd, rcTopToolBar, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_TOOL_BAR_TOP);

	return TRUE;
}


/**
 * @brief 初始化Middle工具栏,主要包括(字体选择、表情图片、发送图片、发送截图等功能)
 * 
 * @return BOOL 
 */
BOOL CGroupChatDlg::InitMidToolBar()
{
	int nIndex = 0;
	//工具栏字体选择
	{
		int nIndex = m_tbMid.AddItem(ID_GROUP_CHAT_DLG_FONT_BTN, STBI_STYLE_BUTTON | STBI_STYLE_CHECK);
		m_tbMid.SetItemSize(nIndex, 30, 27);
		m_tbMid.SetItemPadding(nIndex, 1);
		m_tbMid.SetItemToolTipText(nIndex, _T("字体选择工具栏"));
		m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"),
			_T("aio_toolbar_down.png"), CRect(3, 3, 3, 3));
		m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_font.png"));
	}
	//工具栏选择表情
	{
		nIndex = m_tbMid.AddItem(ID_GROUP_CHAT_DLG_FACE_BTN, STBI_STYLE_BUTTON | STBI_STYLE_CHECK);
		m_tbMid.SetItemSize(nIndex, 30, 27);
		m_tbMid.SetItemPadding(nIndex, 1);
		m_tbMid.SetItemToolTipText(nIndex, _T("选择表情"));
		m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"),
			_T("aio_toolbar_down.png"), CRect(3, 3, 3, 3));
		m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_face.png"));
	}
	//工具栏发送图片按钮
	{
		nIndex = m_tbMid.AddItem(ID_GROUP_CHAT_DLG_IMAGE_BTN, STBI_STYLE_BUTTON);
		m_tbMid.SetItemSize(nIndex, 30, 27);
		m_tbMid.SetItemPadding(nIndex, 1);
		m_tbMid.SetItemToolTipText(nIndex, _T("发送图片"));
		m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"),
			_T("aio_toolbar_down.png"), CRect(3, 3, 3, 3));
		m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_sendpic.png"));
	}

	//工具栏屏幕截图按钮
	{
		nIndex = m_tbMid.AddItem(ID_GROUP_CHAT_DLG_SCREEN_SHOT_BTN, STBI_STYLE_BUTTON);
		m_tbMid.SetItemSize(nIndex, 30, 27, 27, 0);
		m_tbMid.SetItemPadding(nIndex, 1);
		m_tbMid.SetItemToolTipText(nIndex, _T("屏幕截图"));
		m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"),
			_T("aio_toolbar_down.png"), CRect(3, 3, 3, 3));
		m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_cut.png"));
	}

	//工具栏查看历史消息的按钮
	{
		nIndex = m_tbMid.AddItem(ID_GROUP_CHAT_DLG_MSG_LOG_BTN, STBI_STYLE_BUTTON);
		m_nMsgLogIndexInToolbar = nIndex;
		m_tbMid.SetItemSize(nIndex, 90, 27, 27, 0);
		m_tbMid.SetItemPadding(nIndex, 1);
		m_tbMid.SetItemMargin(nIndex, 260, 0);
		m_tbMid.SetItemText(nIndex, _T(">>"));
		m_tbMid.SetItemToolTipText(nIndex, _T("点击查看消息记录"));
		m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_msglog.png"));
	}
	m_tbMid.SetLeftTop(2, 4);
	m_tbMid.SetBgPic(_T("MidToolBar\\bg.png"), CRect(0,0,0,0));

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcMidToolBar(2, GROUP_CHAT_DLG_HEIGHT-167, GROUP_CHAT_DLG_WIDTH-GROUP_MEMBER_LIST_WIDTH, GROUP_CHAT_DLG_HEIGHT-136);
	m_tbMid.Create(m_hWnd, rcMidToolBar, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_TOOL_BAR_MID);

	return TRUE;
}


/**
 * @brief 响应查看“消息记录”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnBtn_MsgLog(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_bMsgLogWindowVisible = !m_bMsgLogWindowVisible;

	CRect rtWindow;
	GetWindowRect(&rtWindow);

	//获取当前窗口在屏幕的位置	
	if(m_bMsgLogWindowVisible)
	{
		m_SkinDlg.SetBgPic(CHAT_EXPAND_BG_IMAGE_NAME, CRect(4, 100, 445, 32));
		if(IsZoomed())
		{
			OnSize(SIZE_MAXIMIZED, CSize(rtWindow.Width(), rtWindow.Height()));
		}
		else
		{
			m_SkinDlg.MoveWindow(rtWindow.left, rtWindow.top, rtWindow.Width() + GROUP_DLG_MSG_LOG_WIDTH, rtWindow.Height(), FALSE);
		}


		m_tbMid.SetItemText(m_nMsgLogIndexInToolbar, _T("<<"));
		m_RightTabCtrl.ShowWindow(SW_SHOW);
		m_richMsgLog.ShowWindow(SW_SHOW);

		OpenMsgLogBrowser();

		//TODO: 如果不调用这一行，会消息记录会产生黑屏，为什么消息记录控件会产生黑屏？
		UpdateWindow();
	}
	else
	{
		m_SkinDlg.SetBgPic(CHAT_BG_IMAGE_NAME, CRect(4, 100, 4, 32));
		if(IsZoomed())
		{
			OnSize(SIZE_MAXIMIZED, CSize(rtWindow.Width(), rtWindow.Height()));
		}
		else
		{
			m_SkinDlg.MoveWindow(rtWindow.left, rtWindow.top, rtWindow.Width() - GROUP_DLG_MSG_LOG_WIDTH, rtWindow.Height(), FALSE);
			//m_SkinDlg.MoveWindow2(rtWindow.left, rtWindow.top, rtWindow.Width()+RIGHT_CHAT_WINDOW_WIDTH, rtWindow.Height(),FALSE);
		}

		m_tbMid.SetItemText(m_nMsgLogIndexInToolbar, _T(">>"));
		m_RightTabCtrl.ShowWindow(SW_HIDE);
		m_richMsgLog.ShowWindow(SW_HIDE);

		CloseMsgLogBrowser();
	}
}

/**
 * @brief 初始化IRichEditOleCallback接口
 * 
 * TODO: 具体作用不清楚,需要再看看
 * @return BOOL 
 */
BOOL CGroupChatDlg::InitRichEditOleCallback()
{
	IRichEditOleCallback2* pRichEditOleCallback2 = NULL;
	HRESULT hr = ::CoCreateInstance(CLSID_ImageOle, NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IRichEditOleCallback2), (void**)&pRichEditOleCallback2);
	if (SUCCEEDED(hr))
	{
		pRichEditOleCallback2->SetNotifyHwnd(m_hWnd);
		pRichEditOleCallback2->SetRichEditHwnd(m_richRecv.m_hWnd);
		m_richRecv.SetOleCallback(pRichEditOleCallback2);
		pRichEditOleCallback2->Release();
	}

	pRichEditOleCallback2 = NULL;
	hr = ::CoCreateInstance(CLSID_ImageOle, NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IRichEditOleCallback2), (void**)&pRichEditOleCallback2);
	if (SUCCEEDED(hr))
	{
		pRichEditOleCallback2->SetNotifyHwnd(m_hWnd);
		pRichEditOleCallback2->SetRichEditHwnd(m_richSend.m_hWnd);
		m_richSend.SetOleCallback(pRichEditOleCallback2);
		pRichEditOleCallback2->Release();
	}

	pRichEditOleCallback2 = NULL;
	hr = ::CoCreateInstance(CLSID_ImageOle, NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IRichEditOleCallback2), (void**)&pRichEditOleCallback2);
	if (SUCCEEDED(hr))
	{
		pRichEditOleCallback2->SetNotifyHwnd(m_hWnd);
		pRichEditOleCallback2->SetRichEditHwnd(m_richMsgLog.m_hWnd);
		m_richMsgLog.SetOleCallback(pRichEditOleCallback2);
		pRichEditOleCallback2->Release();
	}

	return SUCCEEDED(hr);
}


/**
 * @brief 群聊对话框初始化
 * 
 * @return BOOL 
 */
BOOL CGroupChatDlg::Init()
{
	m_SkinDlg.SubclassWindow(m_hWnd);
	m_SkinDlg.SetBgPic(_T("BuddyChatDlgBg.png"), CRect(4, 100, 4, 32));
	m_SkinDlg.SetMinSysBtnPic(_T("SysBtn\\btn_mini_normal.png"), _T("SysBtn\\btn_mini_highlight.png"), _T("SysBtn\\btn_mini_down.png"));
	m_SkinDlg.SetMaxSysBtnPic(_T("SysBtn\\btn_max_normal.png"), _T("SysBtn\\btn_max_highlight.png"), _T("SysBtn\\btn_max_down.png"));
	m_SkinDlg.SetRestoreSysBtnPic(_T("SysBtn\\btn_restore_normal.png"), _T("SysBtn\\btn_restore_highlight.png"), _T("SysBtn\\btn_restore_down.png"));
	m_SkinDlg.SetCloseSysBtnPic(_T("SysBtn\\btn_close_normal.png"), _T("SysBtn\\btn_close_highlight.png"), _T("SysBtn\\btn_close_down.png"));
	//m_SkinDlg.MoveWindow(0, 0, GROUPDLG_WIDTH, GROUPDLG_HEIGHT, FALSE);

	HDC hDlgBgDC = m_SkinDlg.GetBgDC();

	CRect rcClient;
	GetClientRect(&rcClient);

	m_picHead.SubclassWindow(GetDlgItem(ID_PIC_HEAD));
	m_picHead.SetTransparent(TRUE, hDlgBgDC);
	m_picHead.SetShowCursor(TRUE);
	m_picHead.SetBgPic(_T("HeadCtrl\\Padding4Normal.png"), _T("HeadCtrl\\Padding4Hot.png"), _T("HeadCtrl\\Padding4Hot.png"));
	m_picHead.MoveWindow(10, 10, 54, 54, FALSE);
	m_picHead.SetToolTipText(_T("点击查看群资料"));

	WString strFileName;
	//if (m_lpFMGClient != NULL && m_nGroupNumber != 0)
	//{
	//	strFileName = m_lpFMGClient->GetGroupHeadPicFullName(m_nGroupNumber);
	//}
	
	if (!Hootina::CPath::IsFileExist(strFileName.c_str()))
	{
		strFileName = Hootina::CPath::GetAppPath() + _T("Image\\DefGroupHeadPic.png");
	}

	m_picHead.SetBitmap(strFileName.c_str());

	m_lnkGroupName.SubclassWindow(GetDlgItem(ID_LINK_GROUP_NAME));
	m_lnkGroupName.MoveWindow(70, 12, 60, 14, FALSE);
	m_lnkGroupName.SetTransparent(TRUE, hDlgBgDC);
	m_lnkGroupName.SetLinkColor(RGB(0,0,0));
	m_lnkGroupName.SetHoverLinkColor(RGB(0,0,0));
	m_lnkGroupName.SetVisitedLinkColor(RGB(0,0,0));

	HFONT hFontGroupNameLink = CGDIFactory::GetFont(22);
	m_lnkGroupName.SetNormalFont(hFontGroupNameLink);
	
	HFONT hFontGroupCategory = CGDIFactory::GetFont(18);
	m_staGroupCategory.SubclassWindow(GetDlgItem(ID_STATIC_GROUP_CATEGORY));
	m_staGroupCategory.MoveWindow(70, 38, 537, 20, FALSE);
	m_staGroupCategory.SetTransparent(TRUE, hDlgBgDC);
	m_staGroupCategory.SetFont(hFontGroupCategory);
	m_staGroupCategory.SetWindowText(_T("分类：我的群"));

	m_staMemoTitle.SubclassWindow(GetDlgItem(ID_STATIC_MEMO_TITLE));
	m_staMemoTitle.MoveWindow(rcClient.right-200, 98, GROUP_MEMBER_LIST_WIDTH-2, 30, FALSE);
	m_staMemoTitle.SetTransparent(TRUE, hDlgBgDC);
	m_staMemoTitle.SetNormalBgPic(_T("MemoTitleBg.png"));

	m_edtMemo.SetMultiLine(TRUE);
	m_edtMemo.SubclassWindow(GetDlgItem(ID_EDIT_MEMO));
	m_edtMemo.MoveWindow(rcClient.right-200, 124, GROUP_MEMBER_LIST_WIDTH-2, 156, FALSE);

	m_staMemberTitle.SubclassWindow(GetDlgItem(ID_STATIC_MERBER_TITLE));
	m_staMemberTitle.MoveWindow(rcClient.right-200, 267, GROUP_MEMBER_LIST_WIDTH-2, 35, FALSE);
	m_staMemberTitle.SetTransparent(TRUE, hDlgBgDC);
	m_staMemberTitle.SetNormalBgPic(_T("MemoTitleBg.png"));

	m_btnClose.SubclassWindow(GetDlgItem(ID_BTN_CLOSE));
	m_btnClose.SetTransparent(TRUE, hDlgBgDC);
	m_btnClose.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnClose.SetBgPic(_T("Button\\btn_close_normal.png"), _T("Button\\btn_close_highlight.png"),_T("Button\\btn_close_down.png"), _T("Button\\btn_close_focus.png"));
	m_btnClose.MoveWindow(GROUP_CHAT_DLG_WIDTH-400, GROUP_CHAT_DLG_HEIGHT-30, 77, 25, FALSE);

	m_btnSend.SubclassWindow(GetDlgItem(ID_BTN_SEND));
	m_btnSend.SetTransparent(TRUE, hDlgBgDC);
	m_btnSend.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnSend.SetTextColor(RGB(255, 255, 255));
	m_btnSend.SetBgPic(_T("Button\\btn_send_normal.png"), _T("Button\\btn_send_highlight.png"),_T("Button\\btn_send_down.png"), _T("Button\\btn_send_focus.png"));
	m_btnSend.MoveWindow(GROUP_CHAT_DLG_WIDTH-310, GROUP_CHAT_DLG_HEIGHT-30, 77, 25, FALSE);

	m_btnArrow.SubclassWindow(GetDlgItem(ID_BTN_ARROW));
	m_btnArrow.SetTransparent(TRUE, hDlgBgDC);
	m_btnArrow.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnArrow.SetBgPic(_T("Button\\btnright_normal.png"), _T("Button\\btnright_highlight.png"),_T("Button\\btnright_down.png"), _T("Button\\btnright_fouce.png"));
	m_btnArrow.MoveWindow(GROUP_CHAT_DLG_WIDTH-235, GROUP_CHAT_DLG_HEIGHT-30, 28, 25, FALSE);

	////图片上传进度信息文本
	//m_staPicUploadProgress.SubclassWindow(GetDlgItem(IDC_STATIC_PIC_PROGRESS));
	//m_staPicUploadProgress.SetTransparent(TRUE, hDlgBgDC);
	//m_staPicUploadProgress.MoveWindow(10, CHATDLG_HEIGHT-25, 380, 25, FALSE);
	//m_staPicUploadProgress.ShowWindow(SW_HIDE);
	
	//CRect rtClient;
	//GetClientRect(&rtClient);
	//消息记录的四个按钮
	m_btnFirstMsgLog.SubclassWindow(GetDlgItem(IDC_FIRST_MSG_LOG));
	m_btnFirstMsgLog.SetTransparent(TRUE, hDlgBgDC);
	m_btnFirstMsgLog.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnFirstMsgLog.SetToolTipText(_T("第一页"));
	//m_btnFirstMsgLog.SetBgPic(_T("Button\\btnright_normal.png"), _T("Button\\btnright_highlight.png"),_T("Button\\btnright_down.png"), _T("Button\\btnright_fouce.png"));
	m_btnFirstMsgLog.MoveWindow(GROUP_CHAT_DLG_WIDTH+110, GROUP_CHAT_DLG_HEIGHT-30, 28, 25, FALSE);

	m_btnPrevMsgLog.SubclassWindow(GetDlgItem(IDC_PREV_MSG_LOG));
	m_btnPrevMsgLog.SetTransparent(TRUE, hDlgBgDC);
	m_btnPrevMsgLog.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnPrevMsgLog.SetToolTipText(_T("上一页"));
	//m_btnPrevMsgLog.SetBgPic(_T("Button\\btnright_normal.png"), _T("Button\\btnright_highlight.png"),_T("Button\\btnright_down.png"), _T("Button\\btnright_fouce.png"));
	m_btnPrevMsgLog.MoveWindow(GROUP_CHAT_DLG_WIDTH+140, GROUP_CHAT_DLG_HEIGHT-30, 28, 25, FALSE);

	m_staMsgLogPage.SubclassWindow(GetDlgItem(IDC_STATIC_MSG_LOG_PAGE));
	m_staMsgLogPage.SetTransparent(TRUE, hDlgBgDC);
	m_staMsgLogPage.MoveWindow(GROUP_CHAT_DLG_WIDTH+170, GROUP_CHAT_DLG_HEIGHT-24, 60, 25, FALSE);

	m_btnNextMsgLog.SubclassWindow(GetDlgItem(IDC_NEXT_MSG_LOG));
	m_btnNextMsgLog.SetTransparent(TRUE, hDlgBgDC);
	m_btnNextMsgLog.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnNextMsgLog.SetToolTipText(_T("下一页"));
	//m_btnNextMsgLog.SetBgPic(_T("Button\\btnright_normal.png"), _T("Button\\btnright_highlight.png"),_T("Button\\btnright_down.png"), _T("Button\\btnright_fouce.png"));
	m_btnNextMsgLog.MoveWindow(GROUP_CHAT_DLG_WIDTH+240, GROUP_CHAT_DLG_HEIGHT-30, 28, 25, FALSE);

	m_btnLastMsgLog.SubclassWindow(GetDlgItem(IDC_LAST_MSG_LOG));
	m_btnLastMsgLog.SetTransparent(TRUE, hDlgBgDC);
	m_btnLastMsgLog.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnLastMsgLog.SetToolTipText(_T("最后页"));
	//m_btnLastMsgLog.SetBgPic(_T("Button\\btnright_normal.png"), _T("Button\\btnright_highlight.png"),_T("Button\\btnright_down.png"), _T("Button\\btnright_fouce.png"));
	m_btnLastMsgLog.MoveWindow(GROUP_CHAT_DLG_WIDTH+270, GROUP_CHAT_DLG_HEIGHT-30, 28, 25, FALSE);

	ShowMsgLogButtons(FALSE);

	m_SkinMenu.LoadMenu(ID_MENU_GROUP_CHAT);
	m_SkinMenu.SetBgPic(_T("Menu\\menu_left_bg.png"), _T("Menu\\menu_right_bg.png"));
	m_SkinMenu.SetSelectedPic(_T("Menu\\menu_selected.png"));
	m_SkinMenu.SetSepartorPic(_T("Menu\\menu_separtor.png"));
	m_SkinMenu.SetArrowPic(_T("Menu\\menu_arrow.png"));
	m_SkinMenu.SetCheckPic(_T("Menu\\menu_check.png"));
	m_SkinMenu.SetTextColor(RGB(0, 20, 35));
	m_SkinMenu.SetSelTextColor(RGB(254, 254, 254));

	InitTopToolBar();				// 初始化Top工具栏
	InitMidToolBar();				// 初始化Middle工具栏
	m_PicBarDlg.Create(m_hWnd);		// 创建图片悬浮工具栏

	//CRect rcRecv = CRect(CPoint(6, rcClient.top+44+44), CSize(rcClient.Width()-6-209, rcClient.Height()-228));
	CRect rcRecv(6, 107, 583-GROUP_MEMBER_LIST_WIDTH-2, 366);
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|ES_MULTILINE|ES_AUTOVSCROLL|WS_VSCROLL|ES_WANTRETURN;
	m_richRecv.Create(m_hWnd, rcRecv, NULL, dwStyle, WS_EX_TRANSPARENT, ID_RICH_EDIT_RECV);
	m_richRecv.SetTransparent(TRUE, hDlgBgDC);

	//接收richedit与midToolbar之间的分隔条
	CRect rcSplitter(2, GROUP_CHAT_DLG_HEIGHT-135, GROUP_CHAT_DLG_WIDTH-GROUP_MEMBER_LIST_WIDTH, GROUP_CHAT_DLG_HEIGHT-130);
	m_SplitterCtrl.Create(m_hWnd, rcSplitter, NULL, WS_CHILD|WS_VISIBLE, 0, ID_SPLITTER_CTRL);
	
	CRect rcSend(6, 407, 583-GROUP_MEMBER_LIST_WIDTH-2, 502);
	m_richSend.Create(m_hWnd, rcSend, NULL, dwStyle, WS_EX_TRANSPARENT, ID_RICH_EDIT_SEND);
	m_richSend.SetTransparent(TRUE, hDlgBgDC);

	if(!m_FontSelDlg.IsWindow())
	{
		m_FontSelDlg.Create(m_hWnd);
		m_FontSelDlg.ShowWindow(SW_HIDE);
	}

	InitRightTabWindow();

	// 发送消息富文本框控件
	//C_UI_FontInfo fontInfo = m_FontSelDlg.GetPublicFontInfo();
	//RichEdit_SetDefFont(m_richSend.m_hWnd, fontInfo.m_strName.c_str(),
	//	fontInfo.m_nSize, fontInfo.m_clrText, fontInfo.m_bBold,
	//	fontInfo.m_bItalic, fontInfo.m_bUnderLine, FALSE);

	C_UI_FontInfo fontInfo;
	std::vector<WString> arrSysFont;
	EnumSysFont(&arrSysFont);
	long nCustomFontNameIndex = -1;
	if(arrSysFont.empty())
	{
		::MessageBox(m_hWnd, _T("初始化聊天对话框失败！"), g_strAppTitle.c_str(), MB_OK|MB_ICONERROR);
		return FALSE;
	}
	
	size_t nFontCount = arrSysFont.size();

	CString strCustomFontName;// (m_lpFMGClient->m_UserConfig.GetFontName());
	if(!strCustomFontName.IsEmpty())
	{
		BOOL bFound = FALSE;
		for(size_t i=0; i<nFontCount; ++i)
		{
			if(strCustomFontName.CompareNoCase(arrSysFont[i].c_str()) == 0)
			{
				bFound = TRUE;
				break;
			}
		}

		if (!bFound)
		{
			strCustomFontName = _T("微软雅黑");
		}
	}
	else
	{
		strCustomFontName = _T("微软雅黑");
	}

	
	//m_lpFMGClient->m_UserConfig.SetFontName(strCustomFontName);
	fontInfo.m_strName = strCustomFontName;
	//fontInfo.m_nSize = m_lpFMGClient->m_UserConfig.GetFontSize();
	//fontInfo.m_clrText = m_lpFMGClient->m_UserConfig.GetFontColor();
	//fontInfo.m_bBold = m_lpFMGClient->m_UserConfig.IsEnableFontBold();
	//fontInfo.m_bItalic = m_lpFMGClient->m_UserConfig.IsEnableFontItalic();
	//fontInfo.m_bUnderLine = m_lpFMGClient->m_UserConfig.IsEnableFontUnderline();

	RichEdit_SetDefFont(m_richSend.m_hWnd, fontInfo.m_strName.c_str(),
		fontInfo.m_nSize, fontInfo.m_clrText, fontInfo.m_bBold,
		fontInfo.m_bItalic, fontInfo.m_bUnderLine, FALSE);

	// 接收消息富文本框控件
	DWORD dwMask = m_richRecv.GetEventMask();
	dwMask = dwMask | ENM_LINK  | ENM_MOUSEEVENTS | ENM_SCROLLEVENTS | ENM_KEYEVENTS;
	m_richRecv.SetEventMask(dwMask);

	m_richRecv.SetReadOnly();

	CRect rcListCtrl(rcClient.right-200, 302, rcClient.right-3, rcClient.bottom-6);
	m_GroupMemberListCtrl.Create(m_hWnd, rcListCtrl, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_LIST_MERBER);

	m_GroupMemberListCtrl.SetBgPic(_T("ListCtrl\\listctrlbg.png"));
	m_GroupMemberListCtrl.SetItemSelBgPic(_T("ListCtrl\\listitem_select.png"));
	m_GroupMemberListCtrl.SetHeadVisible(FALSE);
	m_GroupMemberListCtrl.AddColumn(_T("群成员"), NULL, DT_LEFT, 184);

	UpdateGroupNameCtrl();	// 更新群名称控件
	UpdateDlgTitle();		// 更新对话框标题

	m_Accelerator.LoadAccelerators(ID_ACCE_CHATDLG);

	InitRichEditOleCallback();	// 初始化IRichEditOleCallback接口

	return TRUE;
}


/**
 * @brief 初始化右侧Tab栏
 * 
 * @return BOOL 
 */
BOOL CGroupChatDlg::InitRightTabWindow()
{
	CRect rcRightTabCtrl(GROUP_CHAT_DLG_WIDTH, 75, GROUP_CHAT_DLG_WIDTH+ GROUP_DLG_MSG_LOG_WIDTH -3, 102);
	m_RightTabCtrl.Create(m_hWnd, rcRightTabCtrl, NULL, WS_CHILD | WS_VISIBLE, NULL, ID_TAB_CTRL_CHAT, NULL);
	m_RightTabCtrl.SetTransparent(TRUE, m_SkinDlg.GetBgDC());
	//m_RightTabCtrl.ShowWindow(SW_HIDE);


	long nWidth = 80;
	long nWidthClose = 21;

	int nIndex = m_RightTabCtrl.AddItem(ID_GROUP_CHAT_DLG_MSG_LOG_BTN, STCI_STYLE_DROPDOWN);
	m_RightTabCtrl.SetItemSize(nIndex, nWidth, 21, nWidth - nWidthClose, 20);
	m_RightTabCtrl.SetItemText(nIndex, _T("消息记录"));
	m_RightTabCtrl.SetCurSel(0);

	//nIndex = m_RightTabCtrl.AddItem(ID_GROUP_CHAT_DLG_SAVE_AS_BTN, STCI_STYLE_DROPDOWN);
	//m_RightTabCtrl.SetItemSize(nIndex, nWidth, 21, nWidth - nWidthClose, 20);
	//m_RightTabCtrl.SetItemText(nIndex, _T("传送文件"));
	//m_RightTabCtrl.SetItemVisible(nIndex, FALSE);

	// 消息记录富文本对话框
	CRect rtClient;
	GetClientRect(&rtClient);
	CRect rcMsgLog;
	rcMsgLog.left = rcRightTabCtrl.left + 5;
	rcMsgLog.top = rcRightTabCtrl.top + 30;
	rcMsgLog.right = rcRightTabCtrl.right;
	rcMsgLog.bottom = rtClient.Height()-30;
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|ES_MULTILINE|ES_AUTOVSCROLL|WS_VSCROLL|ES_WANTRETURN;
	m_richMsgLog.Create(m_hWnd, rcMsgLog, NULL, dwStyle, WS_EX_TRANSPARENT, ID_RICH_EDIT_MSG_LOG);
	m_richMsgLog.ShowWindow(SW_HIDE);
	m_richMsgLog.SetTransparent(TRUE, m_SkinDlg.GetBgDC());
	m_richMsgLog.SetAutoURLDetect();
	m_richMsgLog.SetReadOnly();
	

	return TRUE;
}


/**
 * @brief 显示或隐藏消息记录的翻阅按钮
 * 
 * @param bShow 
 */
void CGroupChatDlg::ShowMsgLogButtons(BOOL bShow)
{
	DWORD dwFlag = (bShow ? SW_SHOW : SW_HIDE); 
	
	m_btnFirstMsgLog.ShowWindow(dwFlag);
	m_btnPrevMsgLog.ShowWindow(dwFlag);
	m_staMsgLogPage.ShowWindow(dwFlag);
	m_btnNextMsgLog.ShowWindow(dwFlag);
	m_btnLastMsgLog.ShowWindow(dwFlag);
}

/**
 * @brief 反初始化
 * 
 * @return BOOL 
 */
BOOL CGroupChatDlg::UnInit()
{
	if (m_PicBarDlg.IsWindow())
	{
		m_PicBarDlg.DestroyWindow();
	}


	if (m_picHead.IsWindow())
	{
		m_picHead.DestroyWindow();
	}

	if (m_lnkGroupName.IsWindow())
	{
		m_lnkGroupName.DestroyWindow();
	}

	if (m_staGroupCategory.IsWindow())
	{
		m_staGroupCategory.DestroyWindow();
	}


	if (m_staMemoTitle.IsWindow())
	{
		m_staMemoTitle.DestroyWindow();
	}


	if (m_staMemberTitle.IsWindow())
	{
		m_staMemberTitle.DestroyWindow();
	}

	if (m_edtMemo.IsWindow())
	{
		m_edtMemo.DestroyWindow();
	}
	
	if (m_btnClose.IsWindow())
	{
		m_btnClose.DestroyWindow();
	}
	
	if (m_btnSend.IsWindow())
	{
		m_btnSend.DestroyWindow();
	}
	
	if (m_btnArrow.IsWindow())
	{
		m_btnArrow.DestroyWindow();
	}
	
	m_SkinMenu.DestroyMenu();

	if (m_tbTop.IsWindow())
	{
		m_tbTop.DestroyWindow();
	}

	if (m_tbMid.IsWindow())
	{
		m_tbMid.DestroyWindow();
	}

	if (m_FontSelDlg.IsWindow())
	{
		m_FontSelDlg.DestroyWindow();
	}

	if (m_FaceSelDlg.IsWindow())
	{
		m_FaceSelDlg.DestroyWindow();
	}

	if (m_richRecv.IsWindow())
	{
		m_richRecv.DestroyWindow();
	}

	if (m_richSend.IsWindow())
	{
		m_richSend.DestroyWindow();
	}

	if (m_GroupMemberListCtrl.IsWindow())
	{
		m_GroupMemberListCtrl.DestroyWindow();
	}

	m_Accelerator.DestroyObject();
//	m_menuRichEdit.DestroyMenu();

	return TRUE;
}





/**
 * @brief 超文本窗口插入文本
 * 
 * @param hWnd 文本窗口Handle
 * @param lpszNewText 待插入的文本
 */
void CGroupChatDlg::_RichEdit_ReplaceSel(HWND hWnd, LPCTSTR lpszNewText)
{
	if (hWnd == m_richRecv.m_hWnd)
	{
		C_UI_FontInfo fontInfo = m_FontSelDlg.GetFontInfo();
		RichEdit_ReplaceSel(hWnd, lpszNewText, 
			fontInfo.m_strName.c_str(), fontInfo.m_nSize, 
			fontInfo.m_clrText, fontInfo.m_bBold, fontInfo.m_bItalic, 
			fontInfo.m_bUnderLine, FALSE, 300);
	}
	else
	{
		RichEdit_ReplaceSel(hWnd, lpszNewText);
	}
}


/**
 * @brief 超文本窗口插入 表情/图片
 * 
 * @param hWnd 文本窗口句柄
 * @param lpszFileName 文件名称
 * @param nFaceId 表情ID
 * @param nFaceIndex 表情索引
 * @return BOOL 
 */
BOOL CGroupChatDlg::_RichEdit_InsertFace(HWND hWnd, LPCTSTR lpszFileName, int nFaceId, int nFaceIndex)
{
	ITextServices* pTextServices;
	ITextHost* pTextHost;
	BOOL bRet;
	long nImageWidth = 0;
	long nImageHeight = 0;
	GetImageDisplaySizeInRichEdit(lpszFileName, hWnd, nImageWidth, nImageHeight);

	if (hWnd == m_richRecv.m_hWnd)
	{
		pTextServices = m_richRecv.GetTextServices();
		pTextHost = m_richRecv.GetTextHost();

		long lStartChar = 0, lEndChar = 0;
		RichEdit_GetSel(hWnd, lStartChar, lEndChar);
		bRet = RichEdit_InsertFace(pTextServices, pTextHost, lpszFileName, nFaceId, nFaceIndex, RGB(255,255,255), TRUE, 40, nImageWidth, nImageHeight);
		if (bRet)
		{
			lEndChar = lStartChar + 1;
			RichEdit_SetSel(hWnd, lStartChar, lEndChar);
			RichEdit_SetStartIndent(hWnd, 300);
			RichEdit_SetSel(hWnd, lEndChar, lEndChar);
		}
	}
	else if (hWnd == m_richSend.m_hWnd)
	{
		pTextServices = m_richSend.GetTextServices();
		pTextHost = m_richSend.GetTextHost();

		long lStartChar = 0, lEndChar = 0;
		RichEdit_GetSel(hWnd, lStartChar, lEndChar);
		long nWidthImage = 0;
		long nHeightImage = 0;
		
		bRet = RichEdit_InsertFace(pTextServices, pTextHost, lpszFileName, nFaceId, nFaceIndex, RGB(255,255,255), TRUE, 40, nImageWidth, nImageHeight);
		if (bRet)
		{
			lEndChar = lStartChar + 1;
			RichEdit_SetSel(hWnd, lStartChar, lEndChar);
			//RichEdit_SetStartIndent(hWnd, 300);
			RichEdit_SetSel(hWnd, lEndChar, lEndChar);
		}
	}
	else if (hWnd == m_richMsgLog.m_hWnd)
	{
		pTextServices = m_richMsgLog.GetTextServices();
		pTextHost = m_richMsgLog.GetTextHost();

		long lStartChar = 0, lEndChar = 0;
		RichEdit_GetSel(hWnd, lStartChar, lEndChar);
		bRet = RichEdit_InsertFace(pTextServices, pTextHost, lpszFileName, nFaceId, nFaceIndex, RGB(255,255,255), TRUE, 40, nImageWidth, nImageHeight);
		if (bRet)
		{
			lEndChar = lStartChar + 1;
			RichEdit_SetSel(hWnd, lStartChar, lEndChar);
			RichEdit_SetStartIndent(hWnd, 300);
			RichEdit_SetSel(hWnd, lEndChar, lEndChar);
		}
	}

	if (pTextServices != NULL)
	{
		pTextServices->Release();
	}

	if (pTextHost != NULL)
	{
		pTextHost->Release();
	}

	return bRet;
}


/**
 * @brief 响应收到群组聊天记录的消息
 * 
 * @param pGroupMsg 
 */
void CGroupChatDlg::OnRecvGroupLogMsg(C_UI_GroupMessage* pGroupMsg)
{
	if (nullptr != pGroupMsg)
	{
		OnRecvToHandle(m_richMsgLog.m_hWnd, pGroupMsg);
		m_richMsgLog.ShowWindow(SW_SHOW);
	}
}

/**
 * @brief 响应收到群消息
 * 
 * @param pGroupMsg 
 */
void CGroupChatDlg::OnRecvGroupMsg(C_UI_GroupMessage* pGroupMsg)
{
	if (nullptr != pGroupMsg)
	{
		OnRecvToHandle(m_richRecv.m_hWnd, pGroupMsg);
	}
}


/**
 * @brief 将群组聊天消息,在对应的富文本控件显示出来
 * 
 * @param recvHandle 对应的富文本控件的Handle
 * @param pGroupMsg 群组聊天消息
 */
void CGroupChatDlg::OnRecvToHandle(const HWND recvHandle,C_UI_GroupMessage* pGroupMsg)
{
	#define USER_NAME_COLOR  RGB(0,128,64)
	if (pGroupMsg->m_eType == E_UI_CONTENT_TYPE::CONTENT_TYPE_TEXT)
	{
		//处理时间部分
		{
			CString strText;

			WString tSpace = _T("  ");
			WString tSplash = _T("\r\n");
			WString tLeftBrackets = _T("(");
			WString tRightBrackets = _T(")");
			WString tText = pGroupMsg->m_strSenderName + tLeftBrackets + pGroupMsg->m_strSenderId + tRightBrackets + tSpace + pGroupMsg->m_strMsgTime + tSplash;
			strText = tText.c_str();
			RichEdit_SetSel(recvHandle, -1, -1);
			RichEdit_ReplaceSel(recvHandle, strText,
				_T("微软雅黑"), 10, USER_NAME_COLOR, FALSE, FALSE, FALSE, FALSE, 0);
		}



		//处理内容部分
		{
			std::string strJson = EncodeUtil::UnicodeToUtf8(pGroupMsg->m_strContext);
			RichEditMsgList msgList = CoreToUi(MsgElemVec(strJson));
			RichEdit_SetSel(recvHandle, -1, -1);
			for (const auto item : msgList)
			{
				switch (item.m_eType)
				{
				case E_RichEditType::TEXT:
				{
					RichEdit_ReplaceSel(recvHandle, item.m_strContext.c_str(),
						pGroupMsg->m_stFontInfo.m_strName.c_str(),
						pGroupMsg->m_stFontInfo.m_nSize,
						pGroupMsg->m_stFontInfo.m_clrText,
						pGroupMsg->m_stFontInfo.m_bBold,
						pGroupMsg->m_stFontInfo.m_bItalic,
						pGroupMsg->m_stFontInfo.m_bUnderLine,
						FALSE,
						0);
				}break;
				case E_RichEditType::FACE:
				{
					CFaceInfo* lpFaceInfo = m_lpFaceList->GetFaceInfoById(item.m_nFaceId);
					if (lpFaceInfo != NULL)
					{
						_RichEdit_InsertFace(recvHandle,
							lpFaceInfo->m_strFileName.c_str(),
							lpFaceInfo->m_nId,
							lpFaceInfo->m_nIndex);
					}
				}break;
				case E_RichEditType::IMAGE:
				{
					WString strNewPath = Hootina::CPath::GetAppPath() + EncodeUtil::AnsiToUnicode(m_netProto->UserId()) + _T("\\");
					WString imgName = item.m_strImageName;
					_RichEdit_InsertFace(recvHandle,
						imgName.data(),
						-1,
						-1);
				}break;
				default:
				{

				}break;
				}
			}
		}
		RichEdit_ReplaceSel(recvHandle, _T("\r\n"));
		RichEdit_SetStartIndent(recvHandle, 0);
		::PostMessage(recvHandle, WM_VSCROLL, SB_BOTTOM, 0);
	}
}


/**
 * @brief 响应发送群组消息的结果
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CGroupChatDlg::OnSendChatMsgResult(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CString strInfo;
	E_UI_SEND_CHAT_MSG_RESULT eResult = static_cast<E_UI_SEND_CHAT_MSG_RESULT>(wParam);
	if(eResult == E_UI_SEND_CHAT_MSG_RESULT::SEND_WHOLE_MSG_FAILED)
	{
		strInfo = _T("                                            ★消息发送失败，请重试！★\r\n");
	}
	else if(eResult == E_UI_SEND_CHAT_MSG_RESULT::SEND_IMGAE_FAILED)
	{
		strInfo = _T("                                            ★图片发送失败，请重试！★\r\n");
	}
	else if(eResult == E_UI_SEND_CHAT_MSG_RESULT::SEND_FILE_FAILED)
	{
		strInfo = _T("                                            ★文件发送失败，请重试！★\r\n");
	}

	RichEdit_SetSel(m_richRecv.m_hWnd, -1, -1);
	RichEdit_ReplaceSel(m_richRecv.m_hWnd, strInfo, _T("微软雅黑"), 10, RGB(255,0,0), FALSE, FALSE, FALSE, FALSE, 0);

	return (LRESULT)1;
}


/**
 * @brief 响应发送文件进度消息
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CGroupChatDlg::OnSendFileProgress(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	FileProgress* pFileProgress = (FileProgress*)lParam;
	if (pFileProgress == NULL)
	{
		return (LRESULT)0;
	}

	long nPercent = pFileProgress->nPercent;


	//delete pFileProgress;
	
	return (LRESULT)1;
}


/**
 * @brief 响应发送文件结果
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CGroupChatDlg::OnSendFileResult(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 1;
}




/**
 * @brief 响应接收文件结果
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CGroupChatDlg::OnRecvFileResult(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 1;
}



/**
 * @brief 打开消息记录浏览窗口
 * 
 */
void CGroupChatDlg::OpenMsgLogBrowser()
{

	m_richMsgLog.ShowWindow(SW_SHOW);
	OnSizeShowMsgHistory();
	//
	{
		m_RightTabCtrl.ShowWindow(SW_HIDE);
		m_staGroupCategory.ShowWindow(SW_HIDE);
		m_staMemoTitle.ShowWindow(SW_HIDE);
		m_edtMemo.ShowWindow(SW_HIDE);
		m_staMemberTitle.ShowWindow(SW_HIDE);
		m_GroupMemberListCtrl.ShowWindow(SW_HIDE);
	}
	m_richMsgLog.Invalidate(TRUE);
}

/**
 * @brief 关闭消息记录浏览窗口
 * 
 */
void CGroupChatDlg::CloseMsgLogBrowser()
{
	::SendMessage(m_richMsgLog.m_hWnd, WM_SETTEXT, 0, 0L);

	ShowMsgLogButtons(FALSE);

	{
		m_RightTabCtrl.ShowWindow(SW_SHOW);
		m_staGroupCategory.ShowWindow(SW_SHOW);
		m_staMemoTitle.ShowWindow(SW_SHOW);
		m_edtMemo.ShowWindow(SW_SHOW);
		m_staMemberTitle.ShowWindow(SW_SHOW);
		m_GroupMemberListCtrl.ShowWindow(SW_SHOW);
	}
	OnSizeNotShowMsgHistory();
}



/**
 * @brief 调整消息接收区的图片大小
 * 
 */
void CGroupChatDlg::ResizeImageInRecvRichEdit()
{
	std::vector<ImageInfo*> arrImageInfo;
	RichEdit_GetImageInfo(m_richRecv, arrImageInfo);
	size_t nSize = arrImageInfo.size();
	if (nSize == 0)
	{
		return;
	}

	ImageInfo* pImage = NULL;
	for(size_t i=0; i<nSize; ++i)
	{
		pImage = arrImageInfo[i];
		if (pImage == NULL)
		{
			continue;
		}

		RichEdit_SetSel(m_richRecv.m_hWnd, pImage->nStartPos, pImage->nEndPos);
		_RichEdit_ReplaceSel(m_richRecv.m_hWnd, _T(""));
		_RichEdit_InsertFace(m_richRecv.m_hWnd, pImage->szPath, -1, -1);
		delete pImage;
	}

	arrImageInfo.clear();

}

/**
 * @brief 获取富文本框中的图片大小
 * 
 * @param pszFileName 
 * @param hWnd 
 * @param nWidth 
 * @param nHeight 
 * @return BOOL 
 */
BOOL CGroupChatDlg::GetImageDisplaySizeInRichEdit(PCTSTR pszFileName, HWND hWnd, long& nWidth, long& nHeight)
{
	//1像素约等于20缇
	const long TWIPS = 20;
	long nWidthImage = 0;
	long nHeightImage = 0;
	GetImageWidthAndHeight(pszFileName, nWidthImage, nHeightImage);

	if(hWnd == m_richSend.m_hWnd)
	{
		CRect rtRichSend;
		::GetClientRect(hWnd, &rtRichSend);
		//图片太小，不缩放
		if(nHeightImage <= rtRichSend.Height())
		{
			nWidth = 0;
			nHeight = 0;
			return TRUE;
		}
		//出错
		if(nHeightImage == 0)
		{
			nWidth = 0;
			nHeight = 0;
			return FALSE;
		}
		//按比例缩放
		nWidth = rtRichSend.Height()*nWidthImage/nHeightImage*TWIPS;
		nHeight = rtRichSend.Height()*TWIPS;
	}
	else if(hWnd==m_richRecv.m_hWnd)
	{
		CRect rtRecv;
		::GetClientRect(hWnd, &rtRecv);

		//图片太小，不缩放
		if(nHeightImage <= rtRecv.Height())
		{
			nWidth = 0;
			nHeight = 0;
			return TRUE;
		}
		//出错
		if(nHeightImage == 0)
		{
			nWidth = 0;
			nHeight = 0;
			return FALSE;
		}
		//按比例缩放
		//图片宽度为窗口的四分之三
		nWidth = rtRecv.Height()*3/4*nWidthImage/nHeightImage*TWIPS;
		nHeight = rtRecv.Height()*3/4*TWIPS;
	}
	else if(hWnd == m_richMsgLog.m_hWnd)
	{
		CRect rtMsgLog;
		::GetClientRect(hWnd, &rtMsgLog);

		//图片太小，不缩放
		if(nWidthImage <= rtMsgLog.Width()-20)
		{
			nWidth = 0;
			nHeight = 0;
			return TRUE;
		}
		//出错
		if(nHeightImage == 0)
		{
			nWidth = 0;
			nHeight = 0;
			return FALSE;
		}
		//按比例缩放
		//图片宽度为窗口的四分之一
		nWidth = (rtMsgLog.Width()/4)*TWIPS;
		nHeight = nWidth*nHeightImage/nWidthImage;
	}

	return TRUE;
}




/**
 * @brief 响应消息记录翻页消息
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CGroupChatDlg::OnMsgLogPage(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (m_netProto)
	{
		switch (nID)
		{
		case IDC_FIRST_MSG_LOG:
		{
			m_netProto->GetGroupHistoryReq(m_strGroupId, HISTORY_DIRECTION::E_FIRST_MSG);
		}break;

		case IDC_PREV_MSG_LOG:
		{
			m_netProto->GetGroupHistoryReq(m_strGroupId, HISTORY_DIRECTION::E_PREV_MSG);
		}break;

		case IDC_NEXT_MSG_LOG:
		{
			m_netProto->GetGroupHistoryReq(m_strGroupId, HISTORY_DIRECTION::E_NEXT_MSG);
		}break;

		case IDC_LAST_MSG_LOG:
		{
			m_netProto->GetGroupHistoryReq(m_strGroupId, HISTORY_DIRECTION::E_LAST_MSG);
		}break;
		default:
		{
			m_netProto->GetGroupHistoryReq(m_strGroupId, HISTORY_DIRECTION::E_LAST_MSG);
		}break;
		}
	}

	OpenMsgLogBrowser();
}

/**
 * @brief 重新计算控件的位置
 * 
 * @param nMouseY 
 */
void CGroupChatDlg::ReCaculateCtrlPostion(long nMouseY)
{
		CRect rtClient;
	::GetClientRect(m_hWnd, &rtClient);
	
	//不允许将发送框尺寸拉的太大或者太小，那样会影响某些控件内部的画法（某些控件内部画法要求必须满足一定的尺寸）
	if (nMouseY <= 280 || nMouseY >= rtClient.Height() - 95)
	{
		return;
	}
	
	RECT rtBase;
	::GetWindowRect(m_richRecv, &rtBase);
	POINT ptBase;
	ptBase.x = rtBase.left;
	ptBase.y = rtBase.top;
	::ScreenToClient(m_hWnd, &ptBase);
	
	CRect rtRichRecv, rtSplitter, rtMidToolbar, rtRichSend;
	::GetClientRect(m_richRecv, &rtRichRecv);
	::GetClientRect(m_SplitterCtrl, &rtSplitter);
	::GetClientRect(m_tbMid, &rtMidToolbar);
	::GetClientRect(m_richSend, &rtRichSend);
	HDWP hdwp = ::BeginDeferWindowPos(5);
	//接收框
	if(m_FontSelDlg.IsWindowVisible())
	{
		//AtlTrace(_T("nMouseY-ptBase.y-3*CHATDLG_TOOLBAR_HEIGHT: %d\n"), nMouseY-ptBase.y-3*CHATDLG_TOOLBAR_HEIGHT);
		//TODO: nMouseY-ptBase.y-2*CHATDLG_TOOLBAR_HEIGHT为什么不起作用呢？
		::DeferWindowPos(hdwp, m_richRecv, NULL, 0, 0, rtRichRecv.Width(), nMouseY-ptBase.y-2*CHATDLG_TOOLBAR_HEIGHT, SWP_NOMOVE|SWP_NOZORDER);
		::DeferWindowPos(hdwp, m_FontSelDlg, NULL, 0, ptBase.y+rtRichRecv.Height()-CHATDLG_TOOLBAR_HEIGHT, 0, CHATDLG_TOOLBAR_HEIGHT, SWP_NOSIZE|SWP_NOZORDER);
	}
	else
	{
		::DeferWindowPos(hdwp, m_richRecv, NULL, 0, 0, rtRichRecv.Width(), nMouseY - ptBase.y - CHATDLG_TOOLBAR_HEIGHT, SWP_NOMOVE | SWP_NOZORDER);
	}

	//MidToolBar
	::GetClientRect(m_SplitterCtrl, &rtSplitter);
	//AtlTrace(_T("ptBase.y+rtRichRecv.Height()+rtSplitter.Height(): %d\n"), ptBase.y+rtRichRecv.Height()+rtSplitter.Height());
	::DeferWindowPos(hdwp, m_tbMid, NULL, 3, ptBase.y+rtRichRecv.Height(), 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	//分割线
	::GetClientRect(m_richRecv, &rtRichRecv);
	//AtlTrace(_T("ptBase.y+rtRichRecv.Height(): %d\n"), ptBase.y+rtRichRecv.Height());
	::DeferWindowPos(hdwp, m_SplitterCtrl, NULL, 6, ptBase.y+rtRichRecv.Height()+rtMidToolbar.Height(), 0, 0, SWP_NOSIZE|SWP_NOZORDER);
	//发送框
	long nHeightRichSend = rtClient.Height()-100-44-(rtRichRecv.Height()+rtSplitter.Height()+rtMidToolbar.Height());
	::GetClientRect(m_tbMid, &rtMidToolbar);
	//AtlTrace(_T("RichSend top: %d\n"), ptBase.y+rtRichRecv.bottom-rtRichRecv.top+rtSplitter.top-rtSplitter.bottom+rtMidToolbar.bottom-rtMidToolbar.top);
	if (m_bMsgLogWindowVisible)
	{
		::DeferWindowPos(hdwp, m_richSend, NULL, 6, ptBase.y + rtRichRecv.Height() + rtSplitter.Height() + rtMidToolbar.Height(), rtClient.Width() - 8 - GROUP_MEMBER_LIST_WIDTH - GROUP_DLG_MSG_LOG_WIDTH, nHeightRichSend, SWP_NOZORDER);
	}
	else
	{
		::DeferWindowPos(hdwp, m_richSend, NULL, 6, ptBase.y + rtRichRecv.Height() + rtSplitter.Height() + rtMidToolbar.Height(), rtClient.Width() - 5 - GROUP_MEMBER_LIST_WIDTH, nHeightRichSend, SWP_NOZORDER);
	}
	::EndDeferWindowPos(hdwp);

	ResizeImageInRecvRichEdit();

	//记录下最新的接受文本框、midTooBar和发送文本框的最新位置
	m_bDraged = TRUE;
	::GetWindowRect(m_richRecv, &m_rtRichRecv);
	::ScreenToClient(m_hWnd, m_rtRichRecv);

	::GetWindowRect(m_tbMid, &m_rtMidToolBar);
	::ScreenToClient(m_hWnd, m_rtMidToolBar);

	::GetWindowRect(m_SplitterCtrl, &m_rtSplitter);
	::ScreenToClient(m_hWnd, m_rtSplitter);

	::GetWindowRect(m_richSend, &m_rtRichSend);
	::ScreenToClient(m_hWnd, m_rtRichSend);
}