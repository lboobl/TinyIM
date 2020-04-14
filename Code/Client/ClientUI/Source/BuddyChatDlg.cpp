/**
 * @file BuddyChatDlg.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 单人聊天窗口的实现
 * @version 0.1
 * @date 2019-08-17
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "stdafx.h"
#include "net/IUProtocolData.h"
#include "Proto.h"
#include "BuddyChatDlg.h"
#include "UI_USER_INFO.h"
#include "Time.h"
#include "ChatDlgCommon.h"
#include "Utils.h"
#include "GDIFactory.h"
#include "EncodingUtil.h"
//#include "net/protocolstream.h"
//#include "net/Msg.h"
#include "UIText.h"
#include "FileTool.h"
#include "UIDefaultValue.h"
#include "UICommonStruct.h"
#include "Path.h"
#include <time.h>
#include <UtilTime.h>





/**
 * @brief 初始化一些数据
 * 
 */
void CBuddyChatDlg::DataMatchInit()
{
	m_strBuddyName.Format(_T("%s"), EncodeUtil::AnsiToUnicode(m_strFriendName).data());
}

/**
 * @brief Construct a new CBuddyChatDlg::CBuddyChatDlg object
 * 
 */
CBuddyChatDlg::CBuddyChatDlg(void):m_userConfig(CUserConfig::GetInstance())
{
	m_lpFaceList = NULL;
	m_lpCascadeWinManager = NULL;
	m_hMainDlg = NULL;

	m_hDlgIcon = m_hDlgSmallIcon = NULL;
	m_hRBtnDownWnd = NULL;
	memset(&m_ptRBtnDown, 0, sizeof(m_ptRBtnDown));
	m_pLastImageOle = NULL;
	m_cxPicBarDlg = 122;
	m_cyPicBarDlg = 24;

	m_nMsgLogIndexInToolbar = -1;

	m_strBuddyName = _T("好友昵称");


	m_bPressEnterToSendMessage = TRUE; 
	m_bMsgLogWindowVisible = FALSE;
	m_bFileTransferVisible = FALSE;

	m_HotRgn = NULL;

	m_bEnableAutoReply = FALSE;
	m_nMsgLogRecordOffset = 1;	
	m_nMsgLogCurrentPageIndex = 0;

	m_bDraged = FALSE;

	::SetRectEmpty(&m_rtRichRecv);
	::SetRectEmpty(&m_rtMidToolBar);
	::SetRectEmpty(&m_rtSplitter);
	::SetRectEmpty(&m_rtRichSend);

    m_nLastSendShakeWindowTime = 0;

	m_pSess = CMsgProto::GetInstance();
}

/**
 * @brief Destroy the CBuddyChatDlg::CBuddyChatDlg object
 * 
 */
CBuddyChatDlg::~CBuddyChatDlg(void)
{

}

/**
 * @brief 预处理消息
 * 
 * @param pMsg 
 * @return BOOL 
 */
BOOL CBuddyChatDlg::PreTranslateMessage(MSG* pMsg)
{
	//if (::GetForegroundWindow() == m_hWnd && !m_Accelerator.IsNull() && 
	//	m_Accelerator.TranslateAccelerator(m_hWnd, pMsg))
	//	return TRUE;

	if (pMsg->hwnd == m_richRecv.m_hWnd || pMsg->hwnd == m_richSend.m_hWnd || pMsg->hwnd == m_richMsgLog.m_hWnd)
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
				return TRUE;
		}
		else if (pMsg->message == WM_RBUTTONDOWN)	// 发送/接收文本框的鼠标右键按下消息
		{
			if (OnRichEdit_RBtnDown(pMsg))
				return TRUE;
		}

		if ( (pMsg->hwnd == m_richSend.m_hWnd) && (pMsg->message == WM_KEYDOWN) 
			&& (pMsg->wParam == 'V') && (::GetAsyncKeyState(VK_CONTROL)&0x8000) )	// 发送文本框的Ctrl+V消息
		{
			m_richSend.PasteSpecial(CF_TEXT);
			return TRUE;
		}

		if ((pMsg->hwnd == m_richSend.m_hWnd) && (pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN) )	
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
 * @brief 响应收到消息设置到具体的窗口
 * 
 * @param recvHandle 具体控件Handle
 * @param msg 聊天消息
 */
void CBuddyChatDlg::OnRecvMsgToHandle(const HWND recvHandle, const CBuddyChatUiMsg& msg)
{
	if (msg.m_eMsgType == E_UI_CONTENT_TYPE::CONTENT_TYPE_TEXT)
	{
		//处理时间部分
		{
			CString strText;

			WString tSpace = _T("  ");
			WString tSplash = _T("\r\n");
			WString tText = msg.m_strSenderName + tSpace + msg.m_strTime + tSplash;
			strText = tText.c_str();
			RichEdit_SetSel(recvHandle, -1, -1);
			RichEdit_ReplaceSel(recvHandle, strText,
				_T("微软雅黑"), 9, USER_NAME_COLOR, FALSE, FALSE, FALSE, FALSE, 0);
		}



		//处理内容部分
		{
			std::string strJson = EncodeUtil::UnicodeToUtf8(msg.m_strContent);
			RichEditMsgList msgList = CoreToUi(MsgElemVec(strJson));
				//RichEditMsg(strJson);
			RichEdit_SetSel(recvHandle, -1, -1);
			for (auto item : msgList)
			{
				switch (item.m_eType)
				{
				case E_RichEditType::TEXT:
				{
					RichEdit_ReplaceSel(recvHandle, item.m_strContext.c_str(),
						msg.m_stFontInfo.m_strName.c_str(),
						msg.m_stFontInfo.m_nSize,
						msg.m_stFontInfo.m_clrText,
						msg.m_stFontInfo.m_bBold,
						msg.m_stFontInfo.m_bItalic,
						msg.m_stFontInfo.m_bUnderLine,
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
					WString strNewPath = item.m_strImageName;
					std::string strImagePath = EncodeUtil::UnicodeToAnsi(strNewPath);
					if (!Hootina::CPath::IsFileExist(strNewPath.data()))
					{
						ERR(m_pSess->ms_loger, "Image File No Exist: {} [{} {}]", strImagePath,__FILENAME__, __LINE__);
						RichEdit_ReplaceSel(recvHandle, _T("-----接收图片失败------"),
							msg.m_stFontInfo.m_strName.c_str(),
							msg.m_stFontInfo.m_nSize,
							msg.m_stFontInfo.m_clrText,
							msg.m_stFontInfo.m_bBold,
							msg.m_stFontInfo.m_bItalic,
							msg.m_stFontInfo.m_bUnderLine,
							FALSE,
							0);
					}
					else
					{
						INFO(m_pSess->ms_loger, "Image File Good: {} [{} {}]", strImagePath, __FILENAME__, __LINE__);
						_RichEdit_InsertFace(recvHandle,
							strNewPath.data(),
							-1,
							-1);
					}
					
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
 * @brief 响应收到好友聊天消息
 * 
 * @param msg 好友聊天消息
 */
void CBuddyChatDlg::OnRecvMsg(const CBuddyChatUiMsg& msg)
{
	m_richRecv.Invalidate(TRUE);
	OnRecvMsgToHandle(m_richRecv.m_hWnd, msg);
}


/**
 * @brief 响应好友聊天记录消息
 * 
 * @param msg 好友聊天记录消息
 */
void CBuddyChatDlg::OnRecvLogMsg(const CBuddyChatUiMsg& msg)
{
	m_richMsgLog.ShowWindow(SW_SHOW);
	OnRecvMsgToHandle(m_richMsgLog.m_hWnd, msg);
}

/**
 * @brief 更新好友号码通知
 * 
 */
void CBuddyChatDlg::OnUpdateBuddyNumber()
{
	UpdateData();
	UpdateBuddyNameCtrl();
}

/**
 * @brief 更新好友签名通知
 * 
 */
void CBuddyChatDlg::OnUpdateBuddySign()
{
	UpdateData();
	UpdateBuddySignCtrl();
}


/**
 * @brief 更新好友头像通知
 * 
 */
void CBuddyChatDlg::OnUpdateBuddyHeadPic()
{
	long nFaceID = 0;
	BOOL bGray = FALSE;
	BOOL bMobile = FALSE;
	CString strThumbPath;
	C_UI_BuddyInfo* pBuddyInfo = GetBuddyInfoPtr();
	if(pBuddyInfo != NULL)
	{
		nFaceID = pBuddyInfo->m_nFace;

		//用户不可见或者离线的状态下，显示灰度图像
		if(  (pBuddyInfo->m_nStatus== E_UI_ONLINE_STATUS::STATUS_INVISIBLE)//用户不可见
		  || (pBuddyInfo->m_nStatus== E_UI_ONLINE_STATUS::STATUS_OFFLINE) )//用户离线
		{
			bGray = TRUE;
		}	

		//判断移动端在线
		if(pBuddyInfo->m_nStatus ==  E_UI_ONLINE_STATUS::STATUS_MOBILE_ONLINE)
		{
			bMobile = TRUE;
		}	
	}
	
	

	//好友头像图片
	strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, 2);
	
	m_picHead.SetBitmap(strThumbPath, FALSE, bGray);
	
	CString strMobileImagePath;
	strMobileImagePath.Format(_T("%sImage\\mobile_online.png"), g_szHomePath);
	if(bMobile)
	{
		m_picHead.SetMobileBitmap(strMobileImagePath, TRUE);
	}
	else
	{
		m_picHead.SetMobileBitmap(strMobileImagePath, FALSE);
	}

	m_hDlgIcon = ExtractIcon(strThumbPath, 64);
	m_hDlgSmallIcon = m_hDlgIcon;
	if(m_hDlgIcon == NULL)
	{
		m_hDlgIcon = AtlLoadIconImage(IDI_BUDDY_CHAT_DLG_32, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
		m_hDlgSmallIcon = AtlLoadIconImage(IDI_BUDDY_CHAT_DLG_16, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	}
	SetIcon(m_hDlgIcon, TRUE);
	SetIcon(m_hDlgSmallIcon, FALSE);


	if(m_picHead.IsWindow())
	{
		m_picHead.Invalidate(FALSE);
	}
}

/**
 * @brief 判断文件是否在传输
 * 
 * @return BOOL TRUE,在传输，FALSE，没有传输
 */
BOOL CBuddyChatDlg::IsFilesTransferring()
{
	return m_FileTransferCtrl.GetItemCount() > 0;
}


/**
 * @brief 初始化对话框
 * 
 * @param wndFocus 
 * @param lInitParam 
 * @return BOOL 
 */
BOOL CBuddyChatDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_lpCascadeWinManager->Add(m_hWnd, CHAT_DLG_WIDTH, CHAT_DLG_HEIGHT);

	CString strThumbPath;
	//Dennis Modify

	
	strThumbPath.Format(_T("%sImage\\UserThumbs\\%d.png"), g_szHomePath, 2);
	
	m_hDlgIcon = ExtractIcon(strThumbPath, 64);
	m_hDlgSmallIcon = m_hDlgIcon;
	if(m_hDlgIcon == NULL)
	{
		m_hDlgIcon = AtlLoadIconImage(IDI_BUDDY_CHAT_DLG_32, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
		m_hDlgSmallIcon = AtlLoadIconImage(IDI_BUDDY_CHAT_DLG_16, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	}
	SetIcon(m_hDlgIcon, TRUE);
	SetIcon(m_hDlgSmallIcon, FALSE);

	//添加消息循环
	{
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
	}

	UpdateData();


	Init();		// 初始化

	SetHotRgn();

    //ModifyStyle(0, WS_CLIPCHILDREN);

    //FIXME: win7用管理员权限启动,发送富文本还是没法接收窗口拖拽,其他窗口可以
    if (IsWindowsVistaOrGreater())
    {
        //win 7管理员权限启动 默认是从UIPI【用户界面特权隔离】移除WM_DROPFILES消息的,
        //这里去除这种限制
        //see: http://blog.csdn.net/whatday/article/details/44278605
        //see: https://msdn.microsoft.com/EN-US/library/windows/desktop/ms632675(v=vs.85).aspx
        typedef BOOL(WINAPI* ChangeWindowMessageFilterOkFn)(HWND, UINT, DWORD, PCHANGEFILTERSTRUCT);

        HMODULE hModUser32 = LoadLibrary(_T("user32.dll"));
        if (hModUser32 != NULL) 
        { 
            ChangeWindowMessageFilterOkFn pfnChangeWindowMessageFilter = (ChangeWindowMessageFilterOkFn)GetProcAddress(hModUser32, "ChangeWindowMessageFilterEx");
            if (pfnChangeWindowMessageFilter != NULL)
            {
                //MSGFLT_ADD = 1
                //WM_COPYGLOBALDATA = 0x0049
                pfnChangeWindowMessageFilter(m_hWnd, WM_DROPFILES, MSGFLT_ALLOW, NULL);
                pfnChangeWindowMessageFilter(m_hWnd, 0x0049, MSGFLT_ALLOW, NULL);

                //pfnChangeWindowMessageFilter(m_richSend.m_hWnd, WM_DROPFILES, MSGFLT_ALLOW, NULL);
                //pfnChangeWindowMessageFilter(m_richSend.m_hWnd, 0x0049, MSGFLT_ALLOW, NULL);
            }
            FreeLibrary(hModUser32);       
        }
    }

	//允许拖拽文件进窗口
	::DragAcceptFiles(m_hWnd, TRUE); 
    //::DragAcceptFiles(m_richSend.m_hWnd, TRUE);
	//PostMessage(WM_SET_DLG_INIT_FOCUS, 0, 0);		// 设置对话框初始焦点
	SetTimer(1001, 300, NULL);
	
	//CalculateMsgLogCountAndOffset();

	//Dennis Mask
	//if(m_userConfig.IsEnableShowLastMsgInChatDlg())
	//	ShowLastMsgInRecvRichEdit();


	return TRUE;
}


/**
 * @brief 响应拷贝数据到发送对话框
 * 
 * @param wnd 
 * @param pCopyDataStruct 
 * @return BOOL 
 */
BOOL CBuddyChatDlg::OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct)
{
	if (NULL == pCopyDataStruct)
	{
		return FALSE;
	}	

	switch (pCopyDataStruct->dwData)
	{
	case IPC_CODE_MSG_LOG_PASTE:			// 消息记录浏览窗口粘贴消息
		{
			if (pCopyDataStruct->lpData != NULL && pCopyDataStruct->cbData > 0)
			{
				AddMsgToSendEdit((LPCTSTR)pCopyDataStruct->lpData);
			}	
		}
		break;

	case IPC_CODE_MSG_LOG_EXIT:			// 消息记录浏览窗口退出消息
		{
			m_tbMid.SetItemCheckState(13, FALSE);
			m_tbMid.Invalidate();
		}
		break;
	}

	return TRUE;
}

/**
 * @brief 
 * 
 * @param nIDCtl 
 * @param lpMeasureItemStruct 
 */
void CBuddyChatDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	m_SkinMenu.OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

/**
 * @brief 响应对菜单的每一项进行绘制
 * 
 * @param nIDCtl 
 * @param lpDrawItemStruct 
 */
void CBuddyChatDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	m_SkinMenu.OnDrawItem(nIDCtl, lpDrawItemStruct);
}

/**
 * @brief 获取窗口最大和最小大小，防止窗口过大或者过小
 * 
 * @param lpMMI 
 */
void CBuddyChatDlg::OnGetMinMaxInfo(LPMINMAXINFO lpMMI)
{
	if (m_bMsgLogWindowVisible)
	{
		lpMMI->ptMinTrackSize.x = CHAT_DLG_EXPAND_WIDTH;
		lpMMI->ptMinTrackSize.y = CHAT_DLG_HEIGHT;
	}
	else
	{
		lpMMI->ptMinTrackSize.x = CHAT_DLG_WIDTH;
		lpMMI->ptMinTrackSize.y = CHAT_DLG_HEIGHT;
	}
}


/**
 * @brief 响应鼠标移动
 * 
 * @param nFlags 
 * @param point 
 */
void CBuddyChatDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (::GetCapture() == m_SplitterCtrl.m_hWnd)
	{
		ReCaculateCtrlPostion(point.y);
	}
}


/**
 * @brief 响应窗口移动
 * 
 * @param ptPos 
 */
void CBuddyChatDlg::OnMove(CPoint ptPos)
{
	SetMsgHandled(FALSE);

	m_lpCascadeWinManager->SetPos(m_hWnd, ptPos.x, ptPos.y);
}



/**
 * @brief 响应窗口大小变化
 * 
 * @param nType 
 * @param size 
 */
void CBuddyChatDlg::OnSize(UINT nType, CSize size)
{
	OnSizeShowHistory();
	ResizeImageInRecvRichEdit();


	//SetMsgHandled(TRUE);
}


/**
 * @brief 响应定时器
 * 
 * @param nIDEvent 定时器编号
 */
void CBuddyChatDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (1001 == nIDEvent)
	{
		//OnRecvMsg(m_nUTalkUin, NULL);	// 显示消息
		KillTimer(nIDEvent);
		SetTimer(1002, 300, NULL);
	}
	else if (nIDEvent == 1002)
	{
		if (!m_FontSelDlg.IsWindow())
		{
			m_FontSelDlg.Create(m_hWnd);
		}	
		KillTimer(nIDEvent);
	}
}


/**
 * @brief 响应文件拖拽消息
 * 
 * @param hDropInfo 
 */
void CBuddyChatDlg::OnDropFiles(HDROP hDropInfo)
{ 
	UINT nFileNum = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0); // 拖拽文件个数  
    TCHAR szFileName[MAX_PATH]={0};  
    for (UINT i=0; i<nFileNum; ++i)    
    {  
		::DragQueryFile(hDropInfo, i, szFileName, MAX_PATH);//获得拖曳的文件名  
        HandleFileDragResult(szFileName);    
    }  
    DragFinish(hDropInfo);      //释放hDropInfo  

    //InvalidateRect(hwnd, NULL, TRUE); 
}


/**
 * @brief 处理文件拖拽结果
 * 
 * @param lpszFileName 
 * @return BOOL 
 */
BOOL CBuddyChatDlg::HandleFileDragResult(PCTSTR lpszFileName)
{
	if(lpszFileName == NULL) 
	{
		return FALSE;
	}	
	
	//如果是文件夹，则发送文件夹
	if(Hootina::CPath::IsDirectory(lpszFileName))
	{
		//TODO: 发送文件夹
		return TRUE;
	}

	CString strFileExtension(Hootina::CPath::GetExtension(lpszFileName).c_str());
	strFileExtension.MakeLower();

	//如果是图片格式，则插入图片
	if( strFileExtension==_T("jpg")  ||
		strFileExtension==_T("jpeg") ||
	    strFileExtension==_T("png")  ||
	    strFileExtension==_T("bmp")  ||
		strFileExtension==_T("gif") )
	{		
		_RichEdit_InsertFace(m_richSend.m_hWnd, lpszFileName, -1, -1);
		m_richSend.SetFocus();
		return TRUE;
	}
	else
	{
		return SendOfflineFile(lpszFileName);
	}


	return FALSE;
}


/**
 * @brief 关闭聊天对话框
 * 
 */
void CBuddyChatDlg::OnClose()
{
	long nFileItemCount = m_FileTransferCtrl.GetItemCount();
	if(nFileItemCount > 0)
	{
		//TODO: 系统对话框太丑，自己做一个对话框！
		ShowWindow(SW_RESTORE);
        if (IDNO == ::MessageBox(m_hWnd, _T("有文件正在传输，关闭窗口将终止传输，确实要关闭吗？"), g_strAppTitle.c_str(), MB_YESNO | MB_ICONQUESTION))
		{
			return;
		}	

		//停止正在传输的文件
		C_WND_MSG_FileItemRequest* pFileItemRequest = NULL;
		FILE_TARGET_TYPE nTargetType = SEND_TYPE;
		CString strRecvFileResultMsgText;
		for(long i=0; i<nFileItemCount; ++i)
		{
			//注意：如果文件还没有进行传输，那么pFileItemRequest返回值为NULL，这样也不影响使用。
			pFileItemRequest = m_FileTransferCtrl.GetFileItemRequestByIndex((size_t)i);
			nTargetType = m_FileTransferCtrl.GetItemTargetTypeByIndex((size_t)i);
			if(pFileItemRequest != NULL)
			{
				//m_lpFMGClient->m_FileTask.RemoveItem(pFileItemRequest);
			}
			
			//提示对方您取消了文件的接收
			if(nTargetType == RECV_TYPE)
			{
				// Dennis Mask
				//strRecvFileResultMsgText.Format(_T("%s取消了接收文件[%s]。"), m_lpFMGClient->m_UserMgr.m_UserInfo.m_strNickName.c_str(), m_FileTransferCtrl.GetItemFileNameByIndex((size_t)i));
				//m_lpFMGClient->SendBuddyMsg(m_LoginUserId, m_strUserName.GetString(), m_UserId, m_strBuddyName.GetString(), time(NULL), strRecvFileResultMsgText.GetString(), m_hWnd);
			}		
		}
	}
	

	//模拟关闭消息记录窗口动作，以使再次打开该窗口时尺寸正常
	//m_bMsgLogWindowVisible = FALSE;
	//PostMessage(WM_COMMAND, IDC_BTN_MSG_LOG, 0);

	RecordWindowSize();

	//通知主窗口关闭当前聊天对话框
	//Dennis Mask
	std::string * pStr = new std::string(m_strFriendId);
	::PostMessage(m_hMainDlg, WM_CLOSE_BUDDY_CHAT_DLG, 0, (LPARAM)(pStr));
}


/**
 * @brief 销毁对话框
 * 
 */
void CBuddyChatDlg::OnDestroy()
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
 * @brief “好友名称”超链接控件
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnLnk_BuddyName(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	//::PostMessage(m_hMainDlg, WM_SHOW_BUDDYINFODLG, NULL, m_nUTalkUin);
}

/**
 * @brief “字体选择工具栏”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnBtn_Font(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	RECT rtRichRecv;
	::GetWindowRect(m_richRecv, &rtRichRecv);
	::ScreenToClient(m_hWnd, rtRichRecv);	
	
	if (BN_PUSHED == uNotifyCode)
	{
		m_FontSelDlg.ShowWindow(SW_SHOW);
		m_richRecv.MoveWindow(6, 106, rtRichRecv.right-rtRichRecv.left, rtRichRecv.bottom-rtRichRecv.top-32, TRUE);
		m_FontSelDlg.MoveWindow(6, rtRichRecv.bottom-32, rtRichRecv.right-rtRichRecv.left, 32, TRUE);	
	}
	else if (BN_UNPUSHED == uNotifyCode)
	{
		m_richRecv.MoveWindow(6, 106, rtRichRecv.right-rtRichRecv.left, rtRichRecv.bottom-rtRichRecv.top+32);
		m_FontSelDlg.ShowWindow(SW_HIDE);
	}
}


/**
 * @brief 响应"发送表情"按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnBtn_Face(UINT uNotifyCode, int nID, CWindow wndCtl)
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
			//int cy = 236;
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
 * @brief 响应"窗口抖动"
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 * 
 * 参考：http://www.rupeng.com/forum/thread-6423-1-1.html
 */
void CBuddyChatDlg::OnShakeWindow(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	//Dennis Mask
	//if (m_lpFMGClient->IsOffline())
	//{
    //    MessageBox(_T("您已经处于离线状态，无法发送窗口抖动，请上线后再次尝试。"), g_strAppTitle.c_str());
	//	return;
//	}
	
  /*  CString strInfo(_T("                                            ☆您发送了一个窗口抖动☆\r\n"));
    time_t nNow = time(NULL);
    //发生窗口抖动的最小时间间隔是5秒
    if (nNow - m_nLastSendShakeWindowTime <= 5)
    {
        strInfo = _T("                                        ☆您发送的窗口抖动过于频繁，请稍后再发。☆\r\n");
    }
    else
    {
        m_nLastSendShakeWindowTime = nNow;
        ShakeWindow(m_hWnd, 1);
       // m_lpFMGClient->SendBuddyMsg(m_LoginUserId, m_strUserName.GetString(), m_UserId, m_strBuddyName.GetString(), nNow, _T("/s[\"1\"]"), m_hWnd);
    }

	RichEdit_SetSel(m_richRecv.m_hWnd, -1, -1);
    RichEdit_ReplaceSel(m_richRecv.m_hWnd, strInfo, _T("微软雅黑"), 10, RGB(0,0,0), FALSE, FALSE, FALSE, FALSE, 0);
	m_richRecv.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	*/
}


/**
 * @brief 响应“发送图片”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnBtn_Image(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	BOOL bOpenFileDialog = TRUE;
	LPCTSTR lpszDefExt = NULL;
	LPCTSTR lpszFileName = NULL;
	DWORD dwFlags = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR|OFN_EXTENSIONDIFFERENT;
	LPCTSTR lpszFilter = _T("图像文件(*.bmp;*.jpg;*.jpeg;*.gif;*.png)\0*.bmp;*.jpg;*.jpeg;*.gif;*.png\0\0");
	HWND hWndParent = m_hWnd;

	CFileDialog fileDlg(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, hWndParent);
	fileDlg.m_ofn.lpstrTitle = _T("打开图片");
	if (fileDlg.DoModal() == IDOK)
	{
		/*UINT64 nFileSize = IUGetFileSize2(fileDlg.m_ofn.lpstrFile);
		if(nFileSize > MAX_CHAT_IMAGE_SIZE)
		{
            ::MessageBox(m_hWnd, _T("图片大小超过10M，请使用文件方式发送或使用截图工具。"), g_strAppTitle.c_str(), MB_OK | MB_ICONINFORMATION);
			return;
		}*/
		
		_RichEdit_InsertFace(m_richSend.m_hWnd, fileDlg.m_ofn.lpstrFile, -1, -1);
		m_richSend.SetFocus();
	}
}


/**
 * @brief 响应“截屏”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnBtn_ScreenShot(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	DWORD dwSucceedExitCode = 2;
	CString strCatchScreen;
	strCatchScreen.Format(_T("%sCatchScreen.exe %u"), g_szHomePath, dwSucceedExitCode);
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};
	if(!CreateProcess(NULL, strCatchScreen.GetBuffer(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
        ::MessageBox(m_hWnd, _T("启动截图工具失败！"), g_strAppTitle.c_str(), MB_OK | MB_ICONERROR);
	}
	if(pi.hProcess != NULL)
	{
		::WaitForSingleObject(pi.hProcess, INFINITE);
		
		dwSucceedExitCode = 0;

		if(::GetExitCodeProcess(pi.hProcess, &dwSucceedExitCode) && dwSucceedExitCode==2)
		{
			m_richSend.PasteSpecial(CF_TEXT);
		}	
	}
}

// “消息记录”按钮
void CBuddyChatDlg::OnBtn_MsgLog(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_bMsgLogWindowVisible = !m_bMsgLogWindowVisible;

	CRect rtWindow;
	GetWindowRect(&rtWindow);

	//获取当前窗口在屏幕的位置	
	if(m_bMsgLogWindowVisible)
	{
		if(m_TabMgr.GetItemCount() == 0)
		{
			m_SkinDlg.SetBgPic(CHAT_EXPAND_BG_IMAGE_NAME, CRect(4, 100, 445, 32));
			::SetWindowPos(m_SkinDlg.m_hWnd, NULL, 0, 0, rtWindow.Width()+RIGHT_CHAT_WINDOW_WIDTH, rtWindow.Height(), SWP_NOMOVE|SWP_NOZORDER);
		}
		
		m_TabMgr.AddItem(_T("消息记录"), m_richMsgLog.m_hWnd);
		if (m_pSess)
		{
			m_pSess->GetChatHistoryReq(m_strFriendId, "", HISTORY_DIRECTION::E_LAST_MSG);
		}
		OpenMsgLogBrowser();

		m_tbMid.SetItemText(m_nMsgLogIndexInToolbar, _T("<<"));
		
	}
	else
	{
		CloseMsgLogBrowser();

		m_tbMid.SetItemText(m_nMsgLogIndexInToolbar, _T(">>"));
		m_TabMgr.RemoveItem(m_richMsgLog.m_hWnd);
		
		//TabMgr的项个数为0才去收起右侧窗口
		if(m_TabMgr.GetItemCount() == 0)
		{
			m_SkinDlg.SetBgPic(CHAT_BG_IMAGE_NAME, CRect(4, 100, 4, 32));
			::SetWindowPos(m_SkinDlg.m_hWnd, NULL, 0, 0, rtWindow.Width()-RIGHT_CHAT_WINDOW_WIDTH, rtWindow.Height(), SWP_NOMOVE|SWP_NOZORDER);
		}			
	}
}

//
/**
 * @brief 显示文件传输控件
 * 
 * @param bShow 是否显示
 */
void CBuddyChatDlg::DisplayFileTransfer(BOOL bShow)
{
	//if(bShow)
	//{
	//	m_TabMgr.Active(m_FileTransferCtrl.m_hWnd);
	//}	
	////文件传输界面已经存在了，则激活文件传输窗口
	//if(bShow && m_bFileTransferVisible)	
	//{
	//	return;
	//}	

	//m_bFileTransferVisible = bShow;

	////获取当前窗口在屏幕的位置	
	//CRect rtWindow;
	//GetWindowRect(&rtWindow);

	//if(m_bFileTransferVisible)
	//{
	//	m_SkinDlg.SetBgPic(CHAT_EXPAND_BG_IMAGE_NAME, CRect(4, 100, 445, 32));
	//	
	//	if(m_TabMgr.GetItemCount() <= 0)
	//	{
	//		::SetWindowPos(m_SkinDlg.m_hWnd,
	//						NULL,
	//						0, 
	//						0, 
	//						rtWindow.Width()+RIGHT_CHAT_WINDOW_WIDTH, 
	//						rtWindow.Height(), 
	//						SWP_NOMOVE|SWP_NOZORDER);
	//	}	
	//	
	//	//m_tbMid.SetItemText(m_nMsgLogIndexInToolbar, _T("<<"));
	//	m_TabMgr.AddItem(_T("传送文件"), m_FileTransferCtrl.m_hWnd, FALSE);
	//	//m_TabMgr.ShowWindow(SW_SHOW);
	//	ShowFileTransferCtrl(TRUE);
	//}
	//else
	//{
	//	//m_tbMid.SetItemText(m_nMsgLogIndexInToolbar, _T(">>"));
	//	//如果文件队列中已经没有文件在传输了，则隐藏文件传输控件
	//	if(m_FileTransferCtrl.GetItemCount() == 0)
	//	{
	//		m_TabMgr.RemoveItem(m_FileTransferCtrl.m_hWnd);
	//		ShowFileTransferCtrl(FALSE);
	//	}

	//	m_SkinDlg.SetBgPic(CHAT_BG_IMAGE_NAME, CRect(4, 100, 4, 32));
	//	//AtlTrace(_T("rtWindow.Width()-RIGHT_CHAT_WINDOW_WIDTH=%d\n"), rtWindow.Width()-RIGHT_CHAT_WINDOW_WIDTH);
	//	if(m_TabMgr.GetItemCount() <= 0)
	//	{
	//		m_bMsgLogWindowVisible = TRUE;	
	//		SendMessage(WM_COMMAND, ID_BUDDY_DLG_SHOW_LOG_MSG_BTN, 0);
	//		//::SetWindowPos(m_SkinDlg.m_hWnd, NULL, 0, 0, rtWindow.Width()-RIGHT_CHAT_WINDOW_WIDTH, rtWindow.Height(), SWP_NOMOVE|SWP_NOZORDER);
	//		m_bMsgLogWindowVisible = FALSE;
	//	}

	//}
}

/**
 * @brief 响应“点击另存为”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnBtn_SaveAs(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	OnMenu_SaveAs(uNotifyCode, nID, wndCtl);
}


/**
 * @brief 响应"消息日志记录页面"
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnMsgLogPage(UINT uNotifyCode, int nID, CWindow wndCtl)
{

	switch(nID)
	{
	//消息记录第一条消息记录
	case IDC_FIRST_MSG_LOG:
		{	
		if (m_pSess)
		{
			m_pSess->GetChatHistoryReq(m_strFriendId, "", HISTORY_DIRECTION::E_FIRST_MSG);
		}
		/*{
			if (m_nMsgLogCurrentPageIndex == 1)
			{
				return;
			}
			m_nMsgLogRecordOffset = 1;
			m_nMsgLogCurrentPageIndex = 1;
		}*/
		}break;

	//消息记录上一条消息
	case IDC_PREV_MSG_LOG:
		{
			if (m_pSess)
			{
				m_pSess->GetChatHistoryReq(m_strFriendId, "", HISTORY_DIRECTION::E_PREV_MSG);
			}
		}break;

	//消息记录下一条消息
	case IDC_NEXT_MSG_LOG:
		{
			if (m_pSess)
			{
				m_pSess->GetChatHistoryReq(m_strFriendId, "", HISTORY_DIRECTION::E_NEXT_MSG);
			}
		}break;

	//消息记录最后一条消息
	case IDC_LAST_MSG_LOG:
		{
			if (m_pSess)
			{
				m_pSess->GetChatHistoryReq(m_strFriendId, "", HISTORY_DIRECTION::E_NEXT_MSG);
			}
		}
		break;
	}
	m_staMsgLogPage.Invalidate(FALSE);

	OpenMsgLogBrowser();
}


/**
 * @brief 响应按“回车键发送消息”
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnPressEnterMenuItem(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_bPressEnterToSendMessage = TRUE;
}


/**
 * @brief 响应"ctrl+回车键发送消息"
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnPressCtrlEnterMenuItem(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_bPressEnterToSendMessage = FALSE;
}


/**
 * @brief 响应"自动回复"
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnAutoReply(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_bEnableAutoReply = !m_bEnableAutoReply;
}


/**
 * @brief 响应“关闭”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnBtn_Close(UINT uNotifyCode, int nID, CWindow wndCtl)
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
void CBuddyChatDlg::OnBtn_Send(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    if (m_pSess->GetStatus() == E_UI_ONLINE_STATUS::STATUS_OFFLINE)
	{
        MessageBox(_T("您已经处于离线状态，无法发送消息，请上线后再次尝试。"), g_strAppTitle.c_str());
		return;
	}

	int nCustomPicCnt = RichEdit_GetCustomPicCount(m_richSend.m_hWnd);
	if (nCustomPicCnt > 1)
	{
        MessageBox(_T("每条消息最多包含1张图片，多张图片请分条发送。"), g_strAppTitle.c_str());
		return;
	}

	WString strText;
	RichEdit_GetText(m_richSend.m_hWnd, strText);
	if (strText.empty())
	{
		::MessageBox(m_hWnd, _T("发送内容不能为空！"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}

	//最大消息长度
	if(strText.length() > 1800)
	{
		::MessageBox(m_hWnd, _T("您发送的内容太长，请分条发送！"), g_strAppTitle.c_str(), MB_OK|MB_ICONINFORMATION);
		return;
	}

	{
		RichEditMsgList msgList = RichEdit_GetMsg(m_richSend.m_hWnd);
		m_pSess->SendChatTxtMsg(m_strFriendId, msgList, m_FontSelDlg.GetFontInfo());
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
void CBuddyChatDlg::OnBtn_Arrow(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(8);
	if (PopupMenu.IsMenu())
	{
		CRect rc;
		m_btnArrow.GetClientRect(&rc);
		m_btnArrow.ClientToScreen(&rc);
		m_bPressEnterToSendMessage = m_userConfig.IsEnablePressEnterToSend();
		if(m_bPressEnterToSendMessage)
		{
			PopupMenu.CheckMenuItem(ID_BUDDY_DLG_PRESS_ENTER_MENU, MF_CHECKED);
			PopupMenu.CheckMenuItem(ID_BUDDY_DLG_PRESS_CTRL_ENTER_MENU, MF_UNCHECKED);
		}
		else
		{
			PopupMenu.CheckMenuItem(ID_BUDDY_DLG_PRESS_ENTER_MENU, MF_UNCHECKED);
			PopupMenu.CheckMenuItem(ID_BUDDY_DLG_PRESS_CTRL_ENTER_MENU, MF_CHECKED);
		}

		
		PopupMenu.CheckMenuItem(IDM_AUTO_REPLY, m_bEnableAutoReply ? MF_CHECKED : MF_UNCHECKED);
		

		PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, 
			rc.left, rc.bottom + 4, m_hWnd, &rc);
	}
}


/**
 * @brief 响应 "打开文件传输项"
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnOpenTransferFileItem(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if(m_strLinkUrl.IsEmpty())
	{
		return;
	}	

	if(nID == IDM_OPEN_FILE)
	{
		::ShellExecute(NULL, _T("open"), m_strLinkUrl, NULL, NULL, SW_SHOWNORMAL);
	}	
	else if(nID == IDM_OPEN_DIRECTORY)
	{
		//参考：http://zhidao.baidu.com/link?url=uEc51-5yf52fP2PHVl7mvAx2CpA8s1R5j8cyCWhYR8AaNE7KQaswGoguJtCI-ZlXMtiD02Iq6K_hBrbCvXoHfGy8ez3t3KtdHeIydox8wDu
		CString strCmdLine;
		strCmdLine.Format(_T("/n,/select,%s"), m_strLinkUrl);
		::ShellExecute(NULL, _T("open"), _T("explorer.exe"), strCmdLine, NULL, SW_SHOWNORMAL);
	}
}


/**
 * @brief 响应工具栏的点击消息
 * 
 * TODO: 此处的Menu索引值待优化
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CBuddyChatDlg::OnToolbarDropDown(LPNMHDR pnmh)
{
	NMTOOLBAR* pnmtb = (NMTOOLBAR*)pnmh;
	CSkinMenu PopupMenu;
	int nIndex = -1;
	CRect rc(pnmtb->rcButton);

	switch (pnmtb->iItem)
	{
	case 101:
	{
		nIndex = 0;
		m_tbTop.ClientToScreen(&rc);
	}break;

	case 102:
	{
		nIndex = 1;
		m_tbTop.ClientToScreen(&rc);
	}break;

	case IDC_BTN_SEND_FILE:
	{
		nIndex = 0;
		m_tbTop.ClientToScreen(&rc);
	}break;	
		
	case 106:
	{
		nIndex = 3;
		m_tbTop.ClientToScreen(&rc);
	}break;

	case 107:
	{
		nIndex = 4;
		m_tbTop.ClientToScreen(&rc);
	}break;

	case 211:
	{
		nIndex = 5;
		m_tbMid.ClientToScreen(&rc);
	}break;

	case 212:
	{
		nIndex = 6;
		m_tbMid.ClientToScreen(&rc);
	}break;

	case ID_BUDDY_DLG_SHOW_LOG_MSG_BTN:
	{
		nIndex = 7;
		m_tbMid.ClientToScreen(&rc);
	}break;

	default:
		return 0;
	}

	PopupMenu = m_SkinMenu.GetSubMenu(nIndex);
	if (PopupMenu.IsMenu())
	{
		PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, 
			rc.left, rc.bottom + 4, m_hWnd, &rc);
	}

	return 0;
}


/**
 * @brief 响应"更新字体信息"
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CBuddyChatDlg::OnUpdateFontInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	C_UI_FontInfo fontInfo = m_FontSelDlg.GetFontInfo();
	RichEdit_SetDefFont(m_richSend.m_hWnd, 
	    fontInfo.m_strName.c_str(),
		fontInfo.m_nSize, 
		fontInfo.m_clrText, 
		fontInfo.m_bBold,
		fontInfo.m_bItalic, 
		fontInfo.m_bUnderLine, 
		FALSE);
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
LRESULT CBuddyChatDlg::OnFaceCtrlSel(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
 * @brief 设置对话框初始焦点为发送框
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CBuddyChatDlg::OnSetDlgInitFocus(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_richSend.SetFocus();
	return 0;
}



/**
 * @brief 响应“接收消息”富文本框链接点击消息
 * 
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CBuddyChatDlg::OnRichEdit_Recv_Link(LPNMHDR pnmh)
{
	//CString strUrl;

	if (pnmh->code == EN_LINK)
	{
		ENLINK*pLink = (ENLINK*)pnmh;
		if (pLink->msg == WM_LBUTTONUP)
		{
			m_richRecv.SetSel(pLink->chrg);
			m_richRecv.GetSelText(m_strLinkUrl);

			//TODO 判断是否为超链接，此处代码需要重构
			if ( (m_strLinkUrl.Left(7).MakeLower()==_T("http://")) || 
			     (m_strLinkUrl.Left(8).MakeLower()==_T("https://")) ||
				 ((m_strLinkUrl.GetLength() >= 7) && (m_strLinkUrl.Left(4).MakeLower()==_T("www."))))
			{
				::ShellExecute(NULL, _T("open"), m_strLinkUrl, NULL, NULL, SW_SHOWNORMAL);
			}
			else if(::PathFileExists(m_strLinkUrl))
			{
				DWORD dwPos = GetMessagePos();
				CPoint point(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos));

				CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(12);
				PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, m_hWnd);
			}
		}
	}
	return 0;
}


/**
 * @brief 响应富文本框发送粘贴的消息
 * 
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CBuddyChatDlg::OnRichEdit_Send_Paste(LPNMHDR pnmh)
{
	NMRICHEDITOLECALLBACK* lpOleNotify = (NMRICHEDITOLECALLBACK*)pnmh;
	if ( (lpOleNotify != NULL) && 
	     (lpOleNotify->hdr.code == EN_PASTE) && 
		 (lpOleNotify->hdr.hwndFrom == m_richSend.m_hWnd) )
	{
		AddMsgToSendEdit(lpOleNotify->lpszText);
	}
	return 0;
}


/**
 * @brief 响应“剪切”菜单,完成发送框的剪切
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnMenu_Cut(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_richSend.Cut();
}

// 
/**
 * @brief 响应“复制”菜单,根据焦点位置的不同，完成发送编辑框，接收编辑框，历史消息编辑框的复制
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnMenu_Copy(UINT uNotifyCode, int nID, CWindow wndCtl)
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
	else if(hWnd == m_richMsgLog.m_hWnd)
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
void CBuddyChatDlg::OnMenu_Paste(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_richSend.PasteSpecial(CF_TEXT);
}


/**
 * @brief 响应“全部选择”菜单,根据焦点来确定是发送框的全选还是接收框的全选
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnMenu_SelAll(UINT uNotifyCode, int nID, CWindow wndCtl)
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
 * @brief 响应“清屏”菜单，完成接收框的清屏
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnMenu_Clear(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_richRecv.SetWindowText(_T(""));
}


/**
 * @brief 响应“显示比例”菜单,发送缩放比例消息
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnMenu_ZoomRatio(UINT uNotifyCode, int nID, CWindow wndCtl)
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
		}
		break;
	case ID_MENU_ZOOM_RATIO_75:
		{
			::SendMessage(m_richRecv.m_hWnd, EM_SETZOOM, 3, 4);
		}break;
	case ID_MENU_ZOOM_RATIO_50:
		{
			::SendMessage(m_richRecv.m_hWnd, EM_SETZOOM, 1, 2);
		}break;
	default:
		return;
	}

	CSkinMenu menuPopup = m_SkinMenu.GetSubMenu(10);
	for (int i = ID_MENU_ZOOM_RATIO_400; i <= ID_MENU_ZOOM_RATIO_50; i++)
	{
		if (i != nID)
		{
			menuPopup.CheckMenuItem(i, MF_BYCOMMAND|MF_UNCHECKED);
		}	
		else
		{
			menuPopup.CheckMenuItem(i, MF_BYCOMMAND|MF_CHECKED);
		}	
	}	
}


/**
 * @brief 响应“另存为”菜单
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnMenu_SaveAs(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	IImageOle* pImageOle = NULL;
	BOOL bRet = RichEdit_GetImageOle(m_hRBtnDownWnd, m_ptRBtnDown, &pImageOle);
	if ( (!bRet) || 
	     (NULL == pImageOle) )
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

	TCHAR cFileName[MAX_PATH] = {0};
	BOOL bOpenFileDialog = FALSE;
	LPCTSTR lpszDefExt;
	CString strFileNamePrefix;
	GenerateChatImageSavedName(strFileNamePrefix);
	DWORD dwFlags = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR|OFN_EXTENSIONDIFFERENT;
	LPCTSTR lpszFilter;
	HWND hWndParent = m_hWnd;

	GUID guid = {0};
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
 * @brief 响应消息导出按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnMenu_ExportMsgLog(UINT uNotifyCode, int nID, CWindow wndCtl)
{
}

/**
 * @brief 响应查找消息记录
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnMenu_FindInMsgLog(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	//查找消息记录
}


/**
 * @brief 响应"删除选中消息记录"菜单
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnMenu_DeleteSelectMsgLog(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if(IDYES != ::MessageBox(m_hWnd, _T("删除的消息记录无法恢复，确实要删除选中的消息记录吗？"), _T("删除确认"), MB_YESNO|MB_ICONWARNING))
	{
		return;
	}	
	
	m_richMsgLog.SetReadOnly(FALSE);
	m_richMsgLog.Cut();
	INPUT Input={0};

	::SendMessage(m_richMsgLog.m_hWnd, WM_KEYDOWN, VK_BACK, 0);
	m_richMsgLog.SetReadOnly(TRUE);

	//判断剪贴板的数据格式是否可以处理。
	if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
	{
		return;
	} 
	// Open the clipboard.  
	if (!::OpenClipboard(m_richMsgLog.m_hWnd))  
	{
		return;  
	}	

	HGLOBAL hMem = ::GetClipboardData(CF_UNICODETEXT);;
	LPCTSTR lpStr = NULL;
	if(hMem != NULL)  
	{
		lpStr = (LPCTSTR)::GlobalLock(hMem);
		if (lpStr != NULL)
		{
			//显示输出。
			::OutputDebugString(lpStr);
			//释放锁内存。
			::GlobalUnlock(hMem);
		}
	}
	::CloseClipboard();
}


/**
 * @brief 响应清除聊天记录
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnMenu_ClearMsgLog(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if(IDYES != ::MessageBox(m_hWnd, _T("删除的消息记录无法恢复，确实要删除与该好友的所有消息记录吗？"), _T("删除确认"), MB_YESNO|MB_ICONWARNING))
	{
		return;
	}	
	
	m_richMsgLog.SetWindowText(_T(""));
	m_richRecv.SetWindowText(_T(""));
	m_staMsgLogPage.SetWindowText(_T("0/0"));
}

void CBuddyChatDlg::SendFileOnLine(CString strFileName)
{
	std::string strStdFileName = EncodeUtil::UnicodeToAnsi(strFileName.GetBuffer());
	auto pSess = CMsgProto::GetInstance();
	pSess->SendFriendOnLineFileMediumTransMode(this->m_strFriendId ,strStdFileName);
	strFileName.ReleaseBuffer();
}
void CBuddyChatDlg::SendFileOnLineP2P(CString strFileName) 
{
	std::string strStdFileName = EncodeUtil::UnicodeToAnsi(strFileName.GetBuffer());
	auto pSess = CMsgProto::GetInstance();
	pSess->SendFriendOnLineFileP2PMode(this->m_strFriendId, strStdFileName);
	strFileName.ReleaseBuffer();
}
/**
 * @brief 响应"发送文件"菜单
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnMenu_SendFile(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	TCHAR	cFileName[MAX_PATH] = {0};
	BOOL	bOpenFileDialog = TRUE;
	LPCTSTR lpszDefExt = NULL;
	LPCTSTR lpszFileName = _T("选择文件");
	LPCTSTR lpszFilter = _T("所有文件(*.)\0\0");
	DWORD	dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR | OFN_EXTENSIONDIFFERENT;
	HWND	hWndParent = m_hWnd;

	CFileDialog fileDlg(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, hWndParent);
	if (fileDlg.DoModal() == IDOK)
	{
		CString strSavePath = fileDlg.m_ofn.lpstrFile;
		CString strExtName = (Hootina::CPath::GetExtension(strSavePath)).c_str();
		
		CString strFileTypeThumbs = Hootina::CPath::GetAppPath().c_str();
		strFileTypeThumbs += _T("\\Skins\\Skin0\\fileTypeThumbs\\");
		strFileTypeThumbs += strExtName;
		strFileTypeThumbs += _T(".png");
		m_SendFileThumbPicture.SetBitmap(strFileTypeThumbs);
		if (!m_bMsgLogWindowVisible)
		{
			::SendMessage(m_btnMsgLog.m_hWnd, BM_CLICK, 0, 0);
			m_bMsgLogWindowVisible = TRUE;
		}
		//::SendMessage(m_btnMsgLog.m_hWnd, BM_CLICK, 0, 0);
		m_RightTabCtrl.SetCurSel(1);
		m_richMsgLog.ShowWindow(SW_HIDE);
		ShowFileTransferCtrl(TRUE);
		//m_staSendFileDesc.SetWindowText(Hootina::CPath::GetFileName(strSavePath).c_str());

		SendFileOnLine(strSavePath);
	}
}


/**
 * @brief 响应“发送离线文件”菜单
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnMenu_SendOfflineFile(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	TCHAR	cFileName[MAX_PATH] = {0};
	BOOL	bOpenFileDialog = TRUE;
	LPCTSTR lpszDefExt = NULL;
	LPCTSTR lpszFileName = _T("选择文件");
	LPCTSTR lpszFilter = _T("所有文件(*.)\0\0");
	DWORD	dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR | OFN_EXTENSIONDIFFERENT;
	HWND	hWndParent = m_hWnd;

	CFileDialog fileDlg(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, hWndParent);
	if (fileDlg.DoModal() != IDOK)
	{
		return;
	}	

	SendOfflineFile(fileDlg.m_szFileName);
}


/**
 * @brief 响应“发送文件夹”菜单
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnMenu_SendDirectory(UINT uNotifyCode, int nID, CWindow wndCtl)
{

}


/**
 * @brief 响应“发送文件设置”菜单
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CBuddyChatDlg::OnMenu_SendFileSettings(UINT uNotifyCode, int nID, CWindow wndCtl)
{

}

bool CBuddyChatDlg::IsShowHistory()
{
	return m_bMsgLogWindowVisible;
}

bool CBuddyChatDlg::IsShowFileTrans()
{
	return IsFilesTransferring() && m_bFileTransferVisible;
}

void CBuddyChatDlg::OnSizeSetWindowSize()
{
	if (IsWindow())
	{
		if (IsShowFileTrans() || IsShowHistory())
		{
			CRect oldWndRect;

			GetWindowRect(&oldWndRect);
			CRect newWndRect = oldWndRect;
			if (oldWndRect.Width() < GROUP_CHAT_DLG_EXTEND_WIDTH)
			{
				newWndRect.left = (oldWndRect.left + oldWndRect.right) / 2 - GROUP_CHAT_DLG_EXTEND_WIDTH / 2;
				newWndRect.right = newWndRect.left + GROUP_CHAT_DLG_EXTEND_WIDTH;
			}

			if (oldWndRect.Height() < GROUP_DLG_MIN_HEIGHT)
			{
				newWndRect.top = (oldWndRect.top + oldWndRect.bottom) / 2 - GROUP_DLG_MIN_HEIGHT / 2;
				newWndRect.bottom = newWndRect.top + GROUP_DLG_MIN_HEIGHT;
			}
			MoveWindow(&newWndRect, TRUE);
		}
		else
		{
			CenterWindow();

			CRect oldWndRect;

			GetWindowRect(&oldWndRect);
			CRect newWndRect = oldWndRect;
			if (oldWndRect.Width() < CHAT_DLG_WIDTH)
			{
				newWndRect.left = (oldWndRect.left + oldWndRect.right) / 2 - CHAT_DLG_WIDTH / 2;
				newWndRect.right = newWndRect.left + CHAT_DLG_WIDTH;
			}

			if (oldWndRect.Height() < CHAT_DLG_HEIGHT)
			{
				newWndRect.top = (oldWndRect.top + oldWndRect.bottom) / 2 - CHAT_DLG_HEIGHT / 2;
				newWndRect.bottom = newWndRect.top + CHAT_DLG_HEIGHT;
			}
			MoveWindow(&newWndRect, TRUE);
		}
	}
}
void CBuddyChatDlg::OnSizeShowLeftArea(const CRect& rcLeftShowArea)
{
	//左边显示区域分解
	//顶部工具栏区域
	CRect rcTopToolBar(
		CPoint(rcLeftShowArea.left,
			rcLeftShowArea.top),

		CSize(rcLeftShowArea.Width(),
			GROUP_DLG_TOOL_BAR_HEIGHT));
	{
		if (m_tbTop.IsWindow())
		{
			m_tbTop.MoveWindow(rcTopToolBar, TRUE);
		}
	}
	{
		//消息接收区
		long recvEditHeight = static_cast<long>(rcLeftShowArea.Height()*BUDDY_DLG_RECV_EDIT_HEIGHT_PERCENT) - 2;
		CRect rcRecvEdit(
			CPoint(rcLeftShowArea.left + GROUP_DLG_OUT_BORDER_WIDTH,
				rcLeftShowArea.top + GROUP_DLG_TOOL_BAR_HEIGHT),
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


		
		{
			//消息发送区域
			CRect rcSendEdit(
				CPoint(rcLeftShowArea.left + GROUP_DLG_OUT_BORDER_WIDTH,
					rcMidToolBar.bottom),
				CSize(rcLeftShowArea.Width() - GROUP_DLG_OUT_BORDER_WIDTH,
					rcLeftShowArea.Height() - rcRecvEdit.Height() - GROUP_DLG_TOOL_BAR_HEIGHT * 2));
			if (m_richSend.IsWindow())
			{
				m_richSend.MoveWindow(rcSendEdit, TRUE);
			}
		}

		{
			//从右到左计算
			const long GROUP_DLG_ARROW_BTN_WIDTH = 77;  //按钮宽度
			const long GROUP_DLG_ARROW_BTN_HEIGHT = 25; //按钮高度
			const long GROUP_DLG_BTN_DISTANCE = 10;     //按钮间距

			//底部工具栏区域
			CRect rcBottomToolBar(
				CPoint(rcLeftShowArea.left,
					rcLeftShowArea.bottom - GROUP_DLG_TOOL_BAR_HEIGHT),
				CSize(rcLeftShowArea.Width(),
					GROUP_DLG_TOOL_BAR_HEIGHT));

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
}
void CBuddyChatDlg::OnSizeShowRightArea(const CRect& rcRightShowArea)
{
	{
		if (m_TabMgr.IsWindow())
		{
			m_TabMgr.MoveWindow(rcRightShowArea, TRUE);
			//历史消息
			{
				if (IsShowHistory())
				{
					m_TabMgr.AddItem(_T("聊天记录"), m_richMsgLog.m_hWnd, TRUE);
					CRect rcMsgLog(
						CPoint(rcRightShowArea.left, rcRightShowArea.top + GROUP_DLG_TOOL_BAR_HEIGHT),
						CSize(rcRightShowArea.Width(),
							rcRightShowArea.Height() - GROUP_DLG_TOOL_BAR_HEIGHT * 2));
					if (m_richMsgLog.IsWindow())
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

			//文件传输
			{
				if (IsFilesTransferring())
				{
					m_TabMgr.AddItem(_T("文件传输"), m_FileTransferCtrl.m_hWnd, TRUE);
					DisplayFileTransfer(TRUE);
					ShowFileTransferCtrl(TRUE);
				}
			}
		}
	}
}
void CBuddyChatDlg::OnSizeShowHistory()
{
	//OnSizeSetWindowSize();
	SetMsgHandled(FALSE);

	CRect rcClient;
	GetClientRect(&rcClient);
	//标题栏区域
	CRect rcTitle(rcClient.TopLeft(),
		CSize(rcClient.Width(),
			GROUP_DLG_TITLE_HEIGHT));

	

	//显示区域
	CRect rcShowArea(
		CPoint(rcClient.left,
			rcTitle.bottom),
		CSize(rcClient.Width(),
			rcClient.Height() - GROUP_DLG_TITLE_HEIGHT));

	if(IsShowFileTrans()||IsShowHistory())
	{
		//左边显示区域

		CRect rcLeftShowArea(
			CPoint(rcShowArea.left,
				rcShowArea.top),
			CSize(rcShowArea.Width() - GROUP_DLG_MSG_LOG_WIDTH - GROUP_DLG_IN_BORDER_WIDTH,
				rcShowArea.Height()));

		OnSizeShowLeftArea(rcLeftShowArea);

		//右边显示区域
		CRect rcRightShowArea(
			CPoint(rcLeftShowArea.right + GROUP_DLG_IN_BORDER_WIDTH,
				rcShowArea.top),
			CSize(GROUP_DLG_MSG_LOG_WIDTH,
				rcShowArea.Height() - GROUP_DLG_OUT_BORDER_WIDTH));
		OnSizeShowRightArea(rcRightShowArea);
	}
	else
	{
		OnSizeShowLeftArea(rcShowArea);
		OnSizeHideRightArea();
	}
	//SetMsgHandled(TRUE);
}

/**
 * @brief 响应发送离线文件
 * 
 * @param pszFileName 
 * @return BOOL 
 */
BOOL CBuddyChatDlg::SendOfflineFile(PCTSTR pszFileName)
{
	auto pSess = CMsgProto::GetInstance();
	std::string strStdFileName = EncodeUtil::UnicodeToAnsi(pszFileName);
	pSess->SendFriendOffLineFile(this->m_strFriendId, strStdFileName);

	if(true)
	{
		if (pszFileName == NULL)
		{
			return FALSE;
		}

		CString strSavePath(pszFileName);
		CString strExtName(Hootina::CPath::GetExtension(strSavePath).c_str());

		CString strFileTypeThumbs(Hootina::CPath::GetAppPath().c_str());
		strFileTypeThumbs += _T("\\Skins\\Skin0\\fileTypeThumbs\\");
		strFileTypeThumbs += strExtName;
		strFileTypeThumbs += _T(".png");

		m_richMsgLog.ShowWindow(SW_HIDE);
		DisplayFileTransfer(TRUE);

		CString strImage;
		long nItemID = m_FileTransferCtrl.AddItem();
		WString strFileName(Hootina::CPath::GetFileName(strSavePath));
		m_FileTransferCtrl.SetItemFileFullNameByID(nItemID, strSavePath);
		m_FileTransferCtrl.SetItemFileNameByID(nItemID, strFileName.c_str());
		if (!Hootina::CPath::IsFileExist(strFileTypeThumbs))
		{
			strFileTypeThumbs.Format(_T("%sSkins\\Skin0\\fileTypeThumbs\\unknown.png"), g_szHomePath);
		}
		m_FileTransferCtrl.SetItemFileTypePicByID(nItemID, strFileTypeThumbs);
		m_FileTransferCtrl.SetItemFileSizeByID(nItemID, Hootina::CPath::GetFileSize(pszFileName));
		m_FileTransferCtrl.SetItemTargetTypeByID(nItemID, SEND_TYPE);

		m_FileTransferCtrl.Invalidate(FALSE);

		m_mapSendFileInfo.insert(std::pair<CString, long>(pszFileName, -1));

		C_WND_MSG_FileItemRequest* pFileItemRequest = new C_WND_MSG_FileItemRequest();
		pFileItemRequest->m_nID = nItemID;
		_tcscpy_s(pFileItemRequest->m_szFilePath, ARRAYSIZE(pFileItemRequest->m_szFilePath), strSavePath);
		pFileItemRequest->m_hwndReflection = m_hWnd;
		pFileItemRequest->m_nFileType = E_UI_FILE_ITEM_TYPE::FILE_ITEM_UPLOAD_CHAT_OFFLINE_FILE;
		//CreateEvent失败会返回NULL，不是INVALID_HANDLE_VALUE，所以可以这么写
		pFileItemRequest->m_hCancelEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

		m_FileTransferCtrl.SetFileItemRequestByID(nItemID, pFileItemRequest);

		//m_lpFMGClient->m_FileTask.AddItem(pFileItemRequest);
	}
	return TRUE;
}


/**
 * @brief 响应回复收到离线文件
 * 
 * @param lpszDownloadName 
 * @param pszFileName 
 * @param nFileSize 
 * @return BOOL 
 */
BOOL CBuddyChatDlg::RecvOfflineFile(PCTSTR lpszDownloadName, PCTSTR pszFileName, long nFileSize)
{
	if( (pszFileName==NULL) || 
	    (nFileSize==NULL) )
	{
		return FALSE;
	}	

	CString strExtName(Hootina::CPath::GetExtension(pszFileName).c_str());
	CString strFileTypeThumbs(Hootina::CPath::GetAppPath().c_str());
	strFileTypeThumbs += _T("\\Skins\\Skin0\\fileTypeThumbs\\");
	strFileTypeThumbs += strExtName;
	strFileTypeThumbs += _T(".png");
	if(!Hootina::CPath::IsFileExist(strFileTypeThumbs))
	{
		strFileTypeThumbs.Format(_T("%sSkins\\Skin0\\fileTypeThumbs\\unknown.png"), g_szHomePath);
	}	

	if(!m_bMsgLogWindowVisible)
	{
		m_richMsgLog.ShowWindow(SW_HIDE);
	}	
	DisplayFileTransfer(TRUE);
	
	CString strImage;
	long nItemID = m_FileTransferCtrl.AddItem();
	m_FileTransferCtrl.SetItemFileNameByID(nItemID, pszFileName);
	
	m_FileTransferCtrl.SetItemFileTypePicByID(nItemID, strFileTypeThumbs);
	m_FileTransferCtrl.SetItemFileSizeByID(nItemID, nFileSize);
	m_FileTransferCtrl.SetItemTargetTypeByID(nItemID, RECV_TYPE);
	char szDownloadName[MAX_PATH] = {0};
    EncodeUtil::UnicodeToUtf8(lpszDownloadName, szDownloadName, ARRAYSIZE(szDownloadName));
	m_FileTransferCtrl.SetItemDownloadNameByID(nItemID, szDownloadName);

	m_FileTransferCtrl.Invalidate(FALSE);

	return TRUE;
}


/**
 * @brief 响应 发送/接收文本框的鼠标移动消息
 * 
 * @param pMsg 
 * @return BOOL 
 */
BOOL CBuddyChatDlg::OnRichEdit_MouseMove(MSG* pMsg)
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
			if (pt.y > rc2.bottom-m_cyPicBarDlg)
			{
				pt.y = rc2.bottom-m_cyPicBarDlg;
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

// 
/**
 * @brief 响应"发送/接收文本框的鼠标双击消息"
 * 
 * @param pMsg 
 * @return BOOL 
 */
BOOL CBuddyChatDlg::OnRichEdit_LBtnDblClk(MSG* pMsg)
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

				}	::ShellExecute(NULL, _T("open"), bstrFileName, NULL, NULL, SW_SHOWNORMAL);
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

void CBuddyChatDlg::OnFileRecvReqMsg(C_WND_MSG_FileRecvReq * pMsg)
{
	if (nullptr != pMsg)
	{
		CString strFileName = EncodeUtil::AnsiToUnicode(pMsg->m_szFileName);
		CString strSendName = EncodeUtil::AnsiToUnicode(pMsg->m_szUserId);
		CString strToName = EncodeUtil::AnsiToUnicode(pMsg->m_szFriendId);
		if (m_FileTransferCtrl.IsWindow())
		{
			m_FileTransferCtrl.ShowWindow(SW_SHOW);
			long nId = m_FileTransferCtrl.AddItem();
			BOOL bResult = m_FileTransferCtrl.SetItemDownloadNameByID(nId, pMsg->m_szFileName);
			m_FileTransferCtrl.SetAcceptButtonVisibleByID(nId, TRUE);
			m_FileTransferCtrl.SetSaveAsButtonVisibleByID(nId, TRUE);
			m_FileTransferCtrl.SetCancelButtonVisibleByID(nId, TRUE);
			if (bResult)
			{
			}
		}
		OnSizeShowHistory();
	}
}

void CBuddyChatDlg::OnFileSendRspMsg(C_WND_MSG_FileSendRsp * pMsg)
{
	if (nullptr != pMsg)
	{
		if (pMsg->m_eErrCode == ERROR_CODE_TYPE::E_CODE_SUCCEED)
		{
			CString strFileName = EncodeUtil::AnsiToUnicode(pMsg->m_szFileName);
			CString strSendName = EncodeUtil::AnsiToUnicode(pMsg->m_szUserId);
			CString strToName = EncodeUtil::AnsiToUnicode(pMsg->m_szFriendId);
			CString strContext;
			strContext.Format(_T("%s 发送 %s 给 %s 成功"), strSendName, strFileName, strToName);
			//MessageBox(strContext, _T("发送文件请求"));
		}
		else
		{
			CString strFileName = EncodeUtil::AnsiToUnicode(pMsg->m_szFileName);
			CString strSendName = EncodeUtil::AnsiToUnicode(pMsg->m_szUserId);
			CString strToName = EncodeUtil::AnsiToUnicode(pMsg->m_szFriendId);
			CString strContext;
			strContext.Format(_T("%s 发送 %s 给 %s 失败 "), strSendName, strFileName, strToName);
			//MessageBox(strContext, _T("发送文件请求"));
		}
	}
}

void CBuddyChatDlg::OnFileNotifyReqMsg(C_WND_MSG_FileNotifyReq* pMsg)
{
	if (nullptr != pMsg)
	{
		CString strFileName = EncodeUtil::AnsiToUnicode(pMsg->m_szFileName);
		CString strSendName = EncodeUtil::AnsiToUnicode(pMsg->m_szUserId);
		CString strToName = EncodeUtil::AnsiToUnicode(pMsg->m_szFriendId);
		CString strContext;
		strContext.Format(_T("%s 已经接收了 %s 的文件 %s"), strToName, strSendName, strFileName);
		//MessageBox(strContext, _T("接收文件通知"));
	}
	{
		auto pSess = CMsgProto::GetInstance();
		pSess->SendFriendNotifyFileRsp(*pMsg);
	}
}

/**
 * @brief 响应"发送/接收文本框的鼠标右键按下消息"
 * 
 * @param pMsg 
 * @return BOOL 
 */
BOOL CBuddyChatDlg::OnRichEdit_RBtnDown(MSG* pMsg)
{
	//TODO 此处代码有重复，需要考虑代码提取
	if (pMsg->hwnd == m_richSend.m_hWnd)
	{
		m_hRBtnDownWnd = pMsg->hwnd;
		m_ptRBtnDown.x = GET_X_LPARAM(pMsg->lParam);
		m_ptRBtnDown.y = GET_Y_LPARAM(pMsg->lParam);

		CSkinMenu menuPopup = m_SkinMenu.GetSubMenu(9);

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
			pMsg->pt.x,
			pMsg->pt.y, 
			m_hWnd,
			 NULL);
	}
	else if (pMsg->hwnd == m_richRecv.m_hWnd)
	{
		m_hRBtnDownWnd = pMsg->hwnd;
		m_ptRBtnDown.x = GET_X_LPARAM(pMsg->lParam);
		m_ptRBtnDown.y = GET_Y_LPARAM(pMsg->lParam);

		CSkinMenu menuPopup = m_SkinMenu.GetSubMenu(10);

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
		CSkinMenu menuPopup = m_SkinMenu.GetSubMenu(11);
		menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pMsg->pt.x, pMsg->pt.y, m_hWnd, NULL);
	}

	return FALSE;
}


/**
 * @brief 获取好友信息指针
 * 
 * @return C_UI_BuddyInfo* 
 */
C_UI_BuddyInfo* CBuddyChatDlg::GetBuddyInfoPtr()
{
	return NULL;
}

/**
 * @brief 获取用户信息指针
 * 
 * @return C_UI_BuddyInfo* 
 */
C_UI_BuddyInfo* CBuddyChatDlg::GetUserInfoPtr()
{
	/*if (m_lpFMGClient != NULL)
		return m_lpFMGClient->GetUserInfo();
	else
		return NULL;*/
	return nullptr;
}



/**
 * @brief 更新数据
 * 
 */
void CBuddyChatDlg::UpdateData()
{
	C_UI_BuddyInfo* lpBuddyInfo = GetBuddyInfoPtr();
	if (lpBuddyInfo != NULL)
	{
		//m_nUTalkNumber = lpBuddyInfo->m_uUserID;
		m_strBuddyName = lpBuddyInfo->m_strNickName.c_str();
		
		m_strBuddySign = lpBuddyInfo->m_strSign.c_str();
		
	}

	C_UI_BuddyInfo* lpUserInfo = GetUserInfoPtr();
	if (lpUserInfo != NULL)
	{
		/*m_nUserNumber = lpUserInfo->m_nUTalkNum;*/
		m_strUserName = lpUserInfo->m_strNickName.c_str();
	}
}




/**
 * @brief 更新对话框标题栏
 * 
 */
void CBuddyChatDlg::UpdateDlgTitle()
{
	SetWindowText(m_strBuddyName);
}



/**
 * @brief 更新好友名称控件
 * 
 */
void CBuddyChatDlg::UpdateBuddyNameCtrl()
{
	std::wstring strName = EncodeUtil::Utf8ToUnicode(m_strFriendName);
	m_lnkBuddyName.SetLabel(strName.data());
}


/**
 * @brief 更新好友签名控件
 * 
 */
void CBuddyChatDlg::UpdateBuddySignCtrl()
{
	m_staBuddySign.SetWindowText(m_strBuddySign);
	//TODO: 加一个tooltip
	//m_staBuddySign.SetTooltipText(m_strBuddySign);
}


/**
 * @brief 初始化Top工具栏
 * 
 * @return BOOL 
 */
BOOL CBuddyChatDlg::InitTopToolBar()
{
	int nIndex = 0;	
	{
		nIndex = m_tbTop.AddItem(IDC_BTN_SEND_FILE, STBI_STYLE_DROPDOWN);
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

	CRect rcTopToolBar(3, 70, CHAT_DLG_WIDTH-3, 102);
	m_tbTop.Create(m_hWnd, rcTopToolBar, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_TOOL_BAR_TOP);
	
	return TRUE;
}


/**
 * @brief 初始化Middle工具栏
 * 
 * @return BOOL 
 */
BOOL CBuddyChatDlg::InitMidToolBar()
{
	int nIndex = 0;
	{
		nIndex = m_tbMid.AddItem(ID_BUDDY_DLG_FONT_BTN, STBI_STYLE_BUTTON | STBI_STYLE_CHECK);
		m_tbMid.SetItemSize(nIndex, 30, 27);
		m_tbMid.SetItemPadding(nIndex, 1);
		m_tbMid.SetItemToolTipText(nIndex, _T("字体选择工具栏"));
		m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"),
			_T("aio_toolbar_down.png"), CRect(3, 3, 3, 3));
		m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_font.png"));
	}
	{	
		nIndex = m_tbMid.AddItem(ID_BUDDY_DLG_FACE_BTN, STBI_STYLE_BUTTON | STBI_STYLE_CHECK);
		m_tbMid.SetItemSize(nIndex, 30, 27);
		m_tbMid.SetItemPadding(nIndex, 1);
		m_tbMid.SetItemToolTipText(nIndex, _T("选择表情"));
		m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"),
			_T("aio_toolbar_down.png"), CRect(3, 3, 3, 3));
		m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_face.png"));
	}
	if(0)
	{
		nIndex = m_tbMid.AddItem(ID_BUDDY_DLG_SHAKE_WINDOW_BTN, STBI_STYLE_BUTTON);
		m_tbMid.SetItemSize(nIndex, 30, 27);
		m_tbMid.SetItemPadding(nIndex, 1);
		m_tbMid.SetItemToolTipText(nIndex, _T("向好友发送窗口抖动"));
		m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"),
			_T("aio_toolbar_down.png"), CRect(3, 3, 3, 3));
		m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_twitter.png"));

	}


	nIndex = m_tbMid.AddItem(ID_BUDDY_DLG_IMAGE, STBI_STYLE_BUTTON);
	m_tbMid.SetItemSize(nIndex, 30, 27);
	m_tbMid.SetItemPadding(nIndex, 1);
	m_tbMid.SetItemToolTipText(nIndex, _T("发送图片"));
	m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"),
		_T("aio_toolbar_down.png"), CRect(3, 3, 3, 3));
	m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_sendpic.png"));



	if (0)
	{
		nIndex = m_tbMid.AddItem(ID_BUDDY_DLG_SCREEN_SHOT_BTN, STBI_STYLE_BUTTON);
		m_tbMid.SetItemSize(nIndex, 30, 27, 27, 0);
		//m_tbMid.SetItemSize(nIndex, 30, 27);
		m_tbMid.SetItemPadding(nIndex, 1);
		m_tbMid.SetItemToolTipText(nIndex, _T("屏幕截图"));
		m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"),
			_T("aio_toolbar_down.png"), CRect(3, 3, 3, 3));
	}

	{
		nIndex = m_tbMid.AddItem(ID_BUDDY_DLG_SHOW_LOG_MSG_BTN, STBI_STYLE_BUTTON);
		m_nMsgLogIndexInToolbar = nIndex;
		m_tbMid.SetItemSize(nIndex, 90, 27, 27, 0);
		m_tbMid.SetItemPadding(nIndex, 1);
		m_tbMid.SetItemMargin(nIndex, CHAT_DLG_WIDTH - 235, 0);
		m_tbMid.SetItemText(nIndex, _T(">>"));
		m_tbMid.SetItemToolTipText(nIndex, _T("点击查看消息记录"));
		m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_msglog.png"));
	}

	m_tbMid.SetLeftTop(2, 2);
	m_tbMid.SetBgPic(_T("MidToolBar\\bg.png"), CRect(0,0,0,0));

	CRect rcMidToolBar(2, CHAT_DLG_HEIGHT-168, CHAT_DLG_WIDTH-2, CHAT_DLG_HEIGHT-136);
	m_tbMid.Create(m_hWnd, rcMidToolBar, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_TOOL_BAR_MID);

	return TRUE;
}


/**
 * @brief 初始化IRichEditOleCallback接口
 * 
 * @return BOOL 
 */
BOOL CBuddyChatDlg::InitRichEditOleCallback()
{
	//TODO  此处注意代码重构和合并
	IRichEditOleCallback2* pRichEditOleCallback2 = NULL;

	HRESULT hr = ::CoCreateInstance(
	CLSID_ImageOle, 
	NULL, 
	CLSCTX_INPROC_SERVER,
	__uuidof(IRichEditOleCallback2), 
	(void**)&pRichEditOleCallback2);
	
	if (SUCCEEDED(hr))
	{
		pRichEditOleCallback2->SetNotifyHwnd(m_hWnd);
		pRichEditOleCallback2->SetRichEditHwnd(m_richRecv.m_hWnd);
		m_richRecv.SetOleCallback(pRichEditOleCallback2);
		pRichEditOleCallback2->Release();
	}

	pRichEditOleCallback2 = NULL;
	hr = ::CoCreateInstance(CLSID_ImageOle, 
	   NULL,
	   CLSCTX_INPROC_SERVER,
	   __uuidof(IRichEditOleCallback2), 
	   (void**)&pRichEditOleCallback2);
	
	if (SUCCEEDED(hr))
	{
		pRichEditOleCallback2->SetNotifyHwnd(m_hWnd);
		pRichEditOleCallback2->SetRichEditHwnd(m_richSend.m_hWnd);
		m_richSend.SetOleCallback(pRichEditOleCallback2);
		pRichEditOleCallback2->Release();
	}

	pRichEditOleCallback2 = NULL;
	hr = ::CoCreateInstance(CLSID_ImageOle, 
	   NULL,
	   CLSCTX_INPROC_SERVER,
	   __uuidof(IRichEditOleCallback2), 
	   (void**)&pRichEditOleCallback2);
	
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
 * @brief 初始化Tab栏
 * 
 * @return BOOL 
 */
BOOL CBuddyChatDlg::InitRightTabWindow()
{
	CRect rcRightTabCtrl(CHAT_DLG_WIDTH, 70, CHAT_DLG_WIDTH-3, 102);
	//m_RightTabCtrl.Create(m_hWnd, rcRightTabCtrl, NULL, WS_CHILD | WS_VISIBLE, NULL, ID_TAB_CTRL_CHAT, NULL);
	//m_RightTabCtrl.SetTransparent(TRUE, m_SkinDlg.GetBgDC());
	//m_RightTabCtrl.ShowWindow(SW_HIDE);


	//long nWidth = 80;
	//long nWidthClose = 21;

	//int nIndex = m_RightTabCtrl.AddItem(301, STCI_STYLE_DROPDOWN);
	//m_RightTabCtrl.SetItemSize(nIndex, nWidth, 21, nWidth - nWidthClose, 20);
	//m_RightTabCtrl.SetItemText(nIndex, _T("消息记录"));
	//m_RightTabCtrl.SetCurSel(0);

	//nIndex = m_RightTabCtrl.AddItem(ID_BUDDY_DLG_SAVE_AS_BTN, STCI_STYLE_DROPDOWN);
	//m_RightTabCtrl.SetItemSize(nIndex, nWidth, 21, nWidth - nWidthClose, 20);
	//m_RightTabCtrl.SetItemText(nIndex, _T("传送文件"));
	//m_RightTabCtrl.SetItemVisible(nIndex, FALSE);

	// 消息记录富文本对话框
	CRect rcMsgLog;
	rcMsgLog.left = rcRightTabCtrl.left;
	rcMsgLog.top = rcRightTabCtrl.top + 30;
	rcMsgLog.right = rcRightTabCtrl.right;
	rcMsgLog.bottom = rcRightTabCtrl.bottom+300;
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|ES_MULTILINE|ES_AUTOVSCROLL|WS_VSCROLL|ES_WANTRETURN;
	m_richMsgLog.Create(m_hWnd, rcMsgLog, NULL, dwStyle, WS_EX_TRANSPARENT, ID_RICH_EDIT_MSG_LOG);
	m_richMsgLog.SetTransparent(FALSE, m_SkinDlg.GetBgDC());
	m_richMsgLog.SetAutoURLDetect();
	m_richMsgLog.SetReadOnly();
	//m_richMsgLog.ShowWindow(SW_HIDE);
	
	CRect rcTabMgr(CHAT_DLG_WIDTH-1, 69, CHAT_DLG_WIDTH+RIGHT_CHAT_WINDOW_WIDTH-2, 101);
	m_TabMgr.Create(m_hWnd, rcTabMgr, NULL, WS_CHILD | WS_VISIBLE, NULL, ID_CHAT_TAB_MGR, NULL);
	m_TabMgr.ShowWindow(SW_HIDE);

	return TRUE;
}


/**
 * @brief 响应显示历史消息的按钮
 * 
 * @param bShow 
 */
void CBuddyChatDlg::ShowMsgLogButtons(BOOL bShow)
{
	DWORD dwFlag = (bShow ? SW_SHOW : SW_HIDE); 
	
	m_btnFirstMsgLog.ShowWindow(dwFlag);
	m_btnPrevMsgLog.ShowWindow(dwFlag);
	m_staMsgLogPage.ShowWindow(dwFlag);
	m_btnNextMsgLog.ShowWindow(dwFlag);
	m_btnLastMsgLog.ShowWindow(dwFlag);
}


/**
 * @brief 初始化文件传输控件
 * 
 */
void CBuddyChatDlg::InitFileTransferCtrl()
{
	HDC hDlgBgDC = m_SkinDlg.GetBgDC();

	CRect rcTabMgr;
	m_TabMgr.GetClientRect(&rcTabMgr);

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcFileTransferCtrl(rcTabMgr.left, rcTabMgr.bottom, rcTabMgr.right, rcClient.bottom);
	m_FileTransferCtrl.Create(m_hWnd, rcFileTransferCtrl, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_FILE_TRANSFER);
	m_FileTransferCtrl.SetTransparent(TRUE, hDlgBgDC);
	
	
	ShowFileTransferCtrl(FALSE);
}


void CBuddyChatDlg::OnSendFileProcess(C_WND_MSG_FileProcessMsg* pMsg)
{
	//TODO: 需要区分具体的文件
	{
		if (m_FileTransferCtrl.IsWindow())
		{
			long nID = m_FileTransferCtrl.GetItemIDByFullName(pMsg->m_szFilePath);
			m_FileTransferCtrl.SetItemProgressPercentByID(nID, pMsg->m_nTransPercent);
		}
	}
}

void CBuddyChatDlg::OnRecvFileProcess(C_WND_MSG_FileProcessMsg* pMsg)
{
	//TODO: 需要区分具体的文件
	{
		if (m_FileTransferCtrl.IsWindow())
		{
			long nID = m_FileTransferCtrl.GetItemIDByFullName(pMsg->m_szFilePath);
			m_FileTransferCtrl.SetItemProgressPercentByID(nID, pMsg->m_nTransPercent);
		}
	}
}

/**
 * @brief 显示文件传输控件
 * 
 * @param bShow 
 */
void CBuddyChatDlg::ShowFileTransferCtrl(BOOL bShow)
{
	/*DWORD dwFlag = (bShow ? SW_SHOW : SW_HIDE);

	if(bShow)
	{
		CRect rcTabMgr;
		m_TabMgr.GetClientRect(&rcTabMgr);

		CRect rcClient;
		GetClientRect(&rcClient);

		CRect rcFileTransferCtrl(rcClient.right-rcTabMgr.Width()-6, 100, rcClient.right-2, rcClient.bottom-32);
		m_FileTransferCtrl.MoveWindow(&rcFileTransferCtrl, FALSE); 
	}

	m_FileTransferCtrl.ShowWindow(dwFlag);
	m_bFileTransferVisible = bShow;*/
	//m_RightTabCtrl.SetItemVisible(1, bShow);
}


/**
 * @brief 销毁文件传输控件
 * 
 */
void CBuddyChatDlg::DestroyFileTransferCtrl()
{
	if(m_SendFileThumbPicture.IsWindow())
	{
		m_SendFileThumbPicture.DestroyWindow();
	}

	if(m_staSendFileDesc.IsWindow())
	{
		m_staSendFileDesc.DestroyWindow();
	}	

	if(m_staSendFileDesc.IsWindow())
	{
		m_staSendFileDesc.DestroyWindow();
	}	

	/*if(m_ProgressSendFile.IsWindow())
	{
		m_ProgressSendFile.DestroyWindow();
	}	*/

	//if(m_lnkSendOffline.IsWindow())
	//{
	//	m_lnkSendOffline.DestroyWindow();
	//}	

	//if(m_lnkSendFileCancel.IsWindow())
	//{
	//	m_lnkSendFileCancel.DestroyWindow();
	//}	
}


/**
 * @brief 响应TabCtrl消息，未使用
 * 
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CBuddyChatDlg::OnTabCtrlDropDown(LPNMHDR pnmh)
{
	int nCurSel = m_RightTabCtrl.GetCurSel();


	return 1;
}


/**
 * @brief 响应TabCtrl改变消息
 * 
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CBuddyChatDlg::OnTabCtrlSelChange(LPNMHDR pnmh)
{
	int nCurSel = m_RightTabCtrl.GetCurSel();
	switch (nCurSel)
	{
	case 0:
		{
			ShowMsgLogButtons(TRUE);
			m_richMsgLog.ShowWindow(SW_SHOW);
			m_richMsgLog.SetFocus();
			ShowFileTransferCtrl(FALSE);
			OpenMsgLogBrowser();
			m_bFileTransferVisible = FALSE;
		}break;

	case 1:
		{
			ShowMsgLogButtons(FALSE);
			ShowFileTransferCtrl(TRUE);
			m_richMsgLog.ShowWindow(SW_HIDE);
		}break;

	default:
		break;
	}

	return 1;
}

/**
 * @brief 响应TabMgr点击消息
 * 
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CBuddyChatDlg::OnClickTabMgr(LPNMHDR pnmh)
{
	TWI_HEADER* pTwi = (TWI_HEADER*)pnmh;
	if( (pTwi==NULL) || 
	    (pTwi->nmhdr.hwndFrom!=m_TabMgr.m_hWnd)  )
	{
		return 0;
	}
		
	if(pTwi->hwndItem == m_richMsgLog.m_hWnd)
	{
		//激活消息记录窗口
		if(pTwi->nClickType == CLICK_TYPE_ACTIVATE)
		{
			ShowMsgLogButtons(TRUE);
			m_richMsgLog.ShowWindow(SW_SHOW);
			m_richMsgLog.SetFocus();
			ShowFileTransferCtrl(FALSE);
			OpenMsgLogBrowser();
			m_bFileTransferVisible = FALSE;
		}
		//关闭消息记录窗口
		else if(pTwi->nClickType == CLICK_TYPE_CLOSE)
		{
			PostMessage(WM_COMMAND, ID_BUDDY_DLG_SHOW_LOG_MSG_BTN, 0);
		}
	}
	else if(pTwi->hwndItem == m_FileTransferCtrl.m_hWnd)
	{
		//激活文件传输窗口
		if(pTwi->nClickType == CLICK_TYPE_ACTIVATE)
		{
			ShowMsgLogButtons(FALSE);
			ShowFileTransferCtrl(TRUE);
			m_richMsgLog.ShowWindow(SW_HIDE);
		}
		//关闭文件传输窗口
		else if(pTwi->nClickType == CLICK_TYPE_CLOSE)
		{
			m_TabMgr.RemoveItem(m_FileTransferCtrl.m_hWnd);
		}
	}

	return 1;
}


/**
 * @brief 响应文件传输按钮
 * 
 * @param pnmh 
 * @return LRESULT 
 */
LRESULT CBuddyChatDlg::OnBtn_FileTransfer(LPNMHDR pnmh)
{
	PFILE_TRANSFER_NMHDREX pNMHDREx = (PFILE_TRANSFER_NMHDREX)pnmh;
	if((pNMHDREx== NULL) || 
	   (pNMHDREx->btnArea==BTN_NONE) || 
	   (pNMHDREx->nID<0) )
	{
		return 0;
	}	

	CString strFileName( m_FileTransferCtrl.GetItemFileNameByID(pNMHDREx->nID));
	if(strFileName == "")
	{
		m_FileTransferCtrl.RemoveItemByID(pNMHDREx->nID);
		return 0;
	}
	
	CString strFileExtension(Hootina::CPath::GetExtension(strFileName).c_str());
	if(strFileExtension == "")
	{
		strFileExtension = _T("*");
	}	

	if(pNMHDREx->btnArea == BTN_CANCEL)
	{
		C_WND_MSG_FileItemRequest* pItemRequest = m_FileTransferCtrl.GetFileItemRequestByID(pNMHDREx->nID);
        //TODO: 接收文件请求还没开始下载的文件，请求可能为NULL
        if (pItemRequest != NULL)
        {
            //m_lpFMGClient->m_FileTask.RemoveItem(pItemRequest);
        }
		
		if(pNMHDREx->nTargetType == RECV_TYPE)
		{
			CString strRecvFileResultMsgText;

			CString strInfo;
			strInfo.Format(_T("                                            ☆您取消了接受文件[%s]。☆\r\n"), strFileName);
			RichEdit_SetSel(m_richRecv.m_hWnd, -1, -1);
			RichEdit_ReplaceSel(m_richRecv.m_hWnd, strInfo, _T("微软雅黑"), 10, RGB(0,0,0), FALSE, FALSE, FALSE, FALSE, 0);
		}
		else if(pNMHDREx->nTargetType == SEND_TYPE)
		{
			CString strInfo;
			strInfo.Format(_T("                                            ☆您取消了发送文件[%s]。☆\r\n"), strFileName);
			RichEdit_SetSel(m_richRecv.m_hWnd, -1, -1);
			RichEdit_ReplaceSel(m_richRecv.m_hWnd, strInfo, _T("微软雅黑"), 10, RGB(0,0,0), FALSE, FALSE, FALSE, FALSE, 0);
		}
		
		m_richRecv.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		m_FileTransferCtrl.RemoveItemByID(pNMHDREx->nID);
		DisplayFileTransfer(FALSE);	
		return 1;
	}
	
	CStringA strDownloadName(m_FileTransferCtrl.GetItemDownloadNameByID(pNMHDREx->nID));
	C_WND_MSG_FileItemRequest* pFileItemRequest = NULL;
	pFileItemRequest = new C_WND_MSG_FileItemRequest();
	pFileItemRequest->m_hCancelEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if(pNMHDREx->btnArea == BTN_ACCEPT)
	{
		//CString strDefaultPath(m_lpFMGClient->m_UserMgr.GetDefaultRecvFilePath().c_str());
		//if(strDefaultPath == "")
		{
			::MessageBox(m_hWnd, _T("无法保存文件至我的文档，请使用[另存为]按钮重新指定文件保存路径！"), g_strAppTitle.c_str(), MB_OK|MB_ICONERROR);
			delete pFileItemRequest;
			return 1;
		}
		CString strFilePath;
		//strFilePath.Format(_T("%s\\%s"), strDefaultPath, strFileName);
		if(Hootina::CPath::IsFileExist(strFilePath))
		{
			CString strInfo;
			strInfo.Format(_T("[%s]文件已存在，是否覆盖？"), strFilePath);
			if(IDNO == ::MessageBox(m_hWnd, strInfo, g_strAppTitle.c_str(), MB_YESNO|MB_ICONQUESTION))
			{
				delete pFileItemRequest;
				return 1;
			}
		}
		_tcscpy_s(pFileItemRequest->m_szFilePath, ARRAYSIZE(pFileItemRequest->m_szFilePath), strFilePath);	
		strcpy_s(pFileItemRequest->m_szUtfFilePath, ARRAYSIZE(pFileItemRequest->m_szUtfFilePath), strDownloadName);
	}
	else if(pNMHDREx->btnArea == BTN_SAVEAS)
	{
		DWORD dwFlags = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR|OFN_EXTENSIONDIFFERENT;
		CFileDialog fileDlg(FALSE, NULL, strFileName, dwFlags, _T("所有类型(*.*)\0*.*\0\0"), m_hWnd);
		fileDlg.m_ofn.lpstrTitle = _T("另存为");
		if (fileDlg.DoModal() != IDOK)
		{
			delete pFileItemRequest;
			return 1;
		}
		_tcscpy_s(pFileItemRequest->m_szFilePath, ARRAYSIZE(pFileItemRequest->m_szFilePath), fileDlg.m_ofn.lpstrFile);
		strcpy_s(pFileItemRequest->m_szUtfFilePath, ARRAYSIZE(pFileItemRequest->m_szUtfFilePath), strDownloadName);
	}

	m_FileTransferCtrl.SetItemSaveNameByID(pNMHDREx->nID, pFileItemRequest->m_szFilePath);
	m_FileTransferCtrl.SetFileItemRequestByID(pNMHDREx->nID, pFileItemRequest);

	pFileItemRequest->m_nID = pNMHDREx->nID;
	pFileItemRequest->m_hwndReflection = m_hWnd;
	pFileItemRequest->m_nFileType = E_UI_FILE_ITEM_TYPE::FILE_ITEM_DOWNLOAD_CHAT_OFFLINE_FILE;

	//m_lpFMGClient->m_FileTask.AddItem(pFileItemRequest);

	m_mapRecvFileInfo.insert(std::pair<CString, long>(pFileItemRequest->m_szFilePath, -1));

	m_FileTransferCtrl.SetAcceptButtonVisibleByID(pNMHDREx->nID, FALSE);
	m_FileTransferCtrl.SetSaveAsButtonVisibleByID(pNMHDREx->nID, FALSE);

	m_FileTransferCtrl.Invalidate(FALSE);
	
	return 1;
}


/**
 * @brief 初始化控件
 * 
 * @return BOOL 
 */
BOOL CBuddyChatDlg::Init()
{
	m_SkinDlg.SubclassWindow(m_hWnd);
	m_SkinDlg.SetBgPic(CHAT_BG_IMAGE_NAME, CRect(CHATDLG_LEFT_FIXED_WIDTH, CHATDLG_TOP_FIXED_HEIGHT, CHATDLG_RIGHT_FIXED_WIDTH, CHATDLG_BOTTOM_FIXED_HEIGHT));
	m_SkinDlg.SetMinSysBtnPic(_T("SysBtn\\btn_mini_normal.png"), _T("SysBtn\\btn_mini_highlight.png"), _T("SysBtn\\btn_mini_down.png"));
	m_SkinDlg.SetMaxSysBtnPic(_T("SysBtn\\btn_max_normal.png"), _T("SysBtn\\btn_max_highlight.png"), _T("SysBtn\\btn_max_down.png"));
	m_SkinDlg.SetRestoreSysBtnPic(_T("SysBtn\\btn_restore_normal.png"), _T("SysBtn\\btn_restore_highlight.png"), _T("SysBtn\\btn_restore_down.png"));
	m_SkinDlg.SetCloseSysBtnPic(_T("SysBtn\\btn_close_normal.png"), _T("SysBtn\\btn_close_highlight.png"), _T("SysBtn\\btn_close_down.png"));
	

	HDC hDlgBgDC = m_SkinDlg.GetBgDC();

	
	m_picHead.SubclassWindow(GetDlgItem(ID_PIC_HEAD));
	m_picHead.SetTransparent(TRUE, hDlgBgDC);
	m_picHead.SetShowCursor(TRUE);
	m_picHead.SetBgPic(_T("HeadCtrl\\Padding4Normal.png"), _T("HeadCtrl\\Padding4Hot.png"), _T("HeadCtrl\\Padding4Hot.png"));
	m_picHead.MoveWindow(10, 10, 54, 54, FALSE);
	
	//WString strNickName(m_lpFMGClient->m_UserMgr.GetNickName(m_nUTalkUin));
	CString strTooltip;
	//strTooltip.Format(_T("点击查看%s的资料"), strNickName.c_str());
	m_picHead.SetToolTipText(strTooltip);

	m_lnkBuddyName.SubclassWindow(GetDlgItem(ID_LINK_BUDDY_NAME));
	m_lnkBuddyName.SetTransparent(TRUE, hDlgBgDC);
	m_lnkBuddyName.SetLinkColor(RGB(0,0,0));
	m_lnkBuddyName.SetHoverLinkColor(RGB(0,0,0));
	m_lnkBuddyName.SetVisitedLinkColor(RGB(0,0,0));
	m_lnkBuddyName.MoveWindow(70, 12, 60, 14, FALSE);

	HFONT hFontBuddyNameLink = CGDIFactory::GetFont(22);
	m_lnkBuddyName.SetNormalFont(hFontBuddyNameLink);
	
	m_staBuddySign.SubclassWindow(GetDlgItem(ID_STATIC_BUDDY_SIGN));
	m_staBuddySign.SetTransparent(TRUE, hDlgBgDC);
	m_staBuddySign.MoveWindow(70, 38, CHAT_DLG_WIDTH-50, 20, FALSE);

	HFONT hFontBuddySignature = CGDIFactory::GetFont(19);
	m_staBuddySign.SetFont(hFontBuddySignature);

	//图片上传进度信息文本
	m_staPicUploadProgress.SubclassWindow(GetDlgItem(IDC_STATIC_PIC_PROGRESS));
	m_staPicUploadProgress.SetTransparent(TRUE, hDlgBgDC);
	m_staPicUploadProgress.MoveWindow(10, CHAT_DLG_HEIGHT-25, 380, 25, FALSE);
	m_staPicUploadProgress.ShowWindow(SW_HIDE);

	m_btnClose.SubclassWindow(GetDlgItem(ID_BTN_CLOSE));
	m_btnClose.SetTransparent(TRUE, hDlgBgDC);
	m_btnClose.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnClose.SetBgPic(_T("Button\\btn_close_normal.png"), _T("Button\\btn_close_highlight.png"),_T("Button\\btn_close_down.png"), _T("Button\\btn_close_focus.png"));
	m_btnClose.MoveWindow(CHAT_DLG_WIDTH-190, CHAT_DLG_HEIGHT-30, 77, 25, FALSE);

	m_btnSend.SubclassWindow(GetDlgItem(ID_BTN_SEND));
	m_btnSend.SetTransparent(TRUE, hDlgBgDC);
	m_btnSend.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnSend.SetTextColor(RGB(255, 255, 255));
	m_btnSend.SetBgPic(_T("Button\\btn_send_normal.png"), _T("Button\\btn_send_highlight.png"),_T("Button\\btn_send_down.png"), _T("Button\\btn_send_focus.png"));
	m_btnSend.MoveWindow(CHAT_DLG_WIDTH-110, CHAT_DLG_HEIGHT-30, 77, 25, FALSE);

	m_btnArrow.SubclassWindow(GetDlgItem(ID_BTN_ARROW));
	m_btnArrow.SetTransparent(TRUE, hDlgBgDC);
	m_btnArrow.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnArrow.SetBgPic(_T("Button\\btnright_normal.png"), _T("Button\\btnright_highlight.png"),_T("Button\\btnright_down.png"), _T("Button\\btnright_fouce.png"));
	m_btnArrow.MoveWindow(CHAT_DLG_WIDTH-33, CHAT_DLG_HEIGHT-30, 28, 25, FALSE);

	//消息记录的四个按钮
	m_btnFirstMsgLog.SubclassWindow(GetDlgItem(IDC_FIRST_MSG_LOG));
	m_btnFirstMsgLog.SetTransparent(TRUE, hDlgBgDC);
	m_btnFirstMsgLog.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnFirstMsgLog.SetToolTipText(_T("第一页"));
	//m_btnFirstMsgLog.SetBgPic(_T("Button\\btnright_normal.png"), _T("Button\\btnright_highlight.png"),_T("Button\\btnright_down.png"), _T("Button\\btnright_fouce.png"));
	m_btnFirstMsgLog.MoveWindow(CHAT_DLG_WIDTH+110, CHAT_DLG_HEIGHT-30, 28, 25, FALSE);

	m_btnPrevMsgLog.SubclassWindow(GetDlgItem(IDC_PREV_MSG_LOG));
	m_btnPrevMsgLog.SetTransparent(TRUE, hDlgBgDC);
	m_btnPrevMsgLog.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnPrevMsgLog.SetToolTipText(_T("上一页"));
	//m_btnPrevMsgLog.SetBgPic(_T("Button\\btnright_normal.png"), _T("Button\\btnright_highlight.png"),_T("Button\\btnright_down.png"), _T("Button\\btnright_fouce.png"));
	m_btnPrevMsgLog.MoveWindow(CHAT_DLG_WIDTH+140, CHAT_DLG_HEIGHT-30, 28, 25, FALSE);

	m_staMsgLogPage.SubclassWindow(GetDlgItem(IDC_STATIC_MSG_LOG_PAGE));
	m_staMsgLogPage.SetTransparent(TRUE, hDlgBgDC);
	m_staMsgLogPage.MoveWindow(CHAT_DLG_WIDTH+170, CHAT_DLG_HEIGHT-24, 60, 25, FALSE);

	m_btnNextMsgLog.SubclassWindow(GetDlgItem(IDC_NEXT_MSG_LOG));
	m_btnNextMsgLog.SetTransparent(TRUE, hDlgBgDC);
	m_btnNextMsgLog.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnNextMsgLog.SetToolTipText(_T("下一页"));
	//m_btnNextMsgLog.SetBgPic(_T("Button\\btnright_normal.png"), _T("Button\\btnright_highlight.png"),_T("Button\\btnright_down.png"), _T("Button\\btnright_fouce.png"));
	m_btnNextMsgLog.MoveWindow(CHAT_DLG_WIDTH+240, CHAT_DLG_HEIGHT-30, 28, 25, FALSE);

	m_btnLastMsgLog.SubclassWindow(GetDlgItem(IDC_LAST_MSG_LOG));
	m_btnLastMsgLog.SetTransparent(TRUE, hDlgBgDC);
	m_btnLastMsgLog.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnLastMsgLog.SetToolTipText(_T("最后页"));
	//m_btnLastMsgLog.SetBgPic(_T("Button\\btnright_normal.png"), _T("Button\\btnright_highlight.png"),_T("Button\\btnright_down.png"), _T("Button\\btnright_fouce.png"));
	m_btnLastMsgLog.MoveWindow(CHAT_DLG_WIDTH+270, CHAT_DLG_HEIGHT-30, 28, 25, FALSE);

	ShowMsgLogButtons(FALSE);

	m_SkinMenu.LoadMenu(ID_MENU_BUDDY_CHAT);
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

	// 接收消息富文本框控件
	CRect rcRecv(6, 107, 583, 430);
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|ES_MULTILINE|ES_AUTOVSCROLL|WS_VSCROLL|ES_WANTRETURN;
    m_richRecv.Create(m_hWnd, rcRecv, NULL, dwStyle, WS_EX_TRANSPARENT, ID_RICH_EDIT_RECV);
	m_richRecv.SetTransparent(TRUE, hDlgBgDC);	
	DWORD dwMask = m_richRecv.GetEventMask();
	dwMask = dwMask | ENM_LINK  | ENM_MOUSEEVENTS | ENM_SCROLLEVENTS | ENM_KEYEVENTS;
	m_richRecv.SetEventMask(dwMask);
	m_richRecv.SetAutoURLDetect();
	m_richRecv.SetReadOnly();

	CRect rcSend(6, 405, 603, 495);
    m_richSend.Create(m_hWnd, rcSend, NULL, dwStyle, WS_EX_TRANSPARENT, ID_RICH_EDIT_SEND);
	m_richSend.SetTransparent(TRUE, hDlgBgDC);

	//接收richedit与midToolbar之间的分隔条
	CRect rcSplitter(6, 400, 603, 405);
	m_SplitterCtrl.Create(m_hWnd, rcSplitter, NULL, WS_CHILD|WS_VISIBLE, 0, ID_SPLITTER_CTRL);

	if(!m_FontSelDlg.IsWindow())
	{
		m_FontSelDlg.Create(m_hWnd);
		m_FontSelDlg.ShowWindow(SW_HIDE);
	}
	
	InitRightTabWindow();


	//初始化与文件传输相关的控件
	InitFileTransferCtrl();
	ShowFileTransferCtrl(FALSE);
	
	// 发送消息富文本框控件
	//C_UI_FontInfo fontInfo = m_FontSelDlg.GetPublicFontInfo();
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

	CString strCustomFontName(m_userConfig.GetFontName());
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

		if(!bFound)
			strCustomFontName = _T("微软雅黑");
	}
	else
		strCustomFontName = _T("微软雅黑");

	
	m_userConfig.SetFontName(strCustomFontName);
	fontInfo.m_strName = strCustomFontName;
	fontInfo.m_nSize = m_userConfig.GetFontSize();
	fontInfo.m_clrText = m_userConfig.GetFontColor();
	fontInfo.m_bBold = m_userConfig.IsEnableFontBold();
	fontInfo.m_bItalic = m_userConfig.IsEnableFontItalic();
	fontInfo.m_bUnderLine = m_userConfig.IsEnableFontUnderline();

	RichEdit_SetDefFont(m_richSend.m_hWnd, fontInfo.m_strName.c_str(),
		fontInfo.m_nSize, fontInfo.m_clrText, fontInfo.m_bBold,
		fontInfo.m_bItalic, fontInfo.m_bUnderLine, FALSE);

	//更新发送编辑框的字体信息
	RichEdit_SetDefFont(m_richSend.m_hWnd, 
		fontInfo.m_strName.c_str(),
		fontInfo.m_nSize, 
		fontInfo.m_clrText, 
		FALSE,
		fontInfo.m_bItalic,
		fontInfo.m_bUnderLine, 
		FALSE);


	UpdateDlgTitle();
	UpdateBuddyNameCtrl();
	UpdateBuddySignCtrl();
	OnUpdateBuddyHeadPic();

	m_Accelerator.LoadAccelerators(ID_ACCE_CHATDLG);

	InitRichEditOleCallback();	// 初始化IRichEditOleCallback接口

	
	return TRUE;
}


/**
 * @brief 
 * 
 */
void CBuddyChatDlg::SetHotRgn()
{
	RECT rtWindow;
	HRGN hTemp;
	
	CRect rtClient;
	GetClientRect(&rtClient);
	rtWindow.left = 0;
	rtWindow.top = 0;
	rtWindow.right = rtClient.right;
	rtWindow.bottom = 106;
	m_HotRgn = ::CreateRectRgnIndirect(&rtWindow);

	m_picHead.GetClientRect(&rtWindow);
	hTemp = ::CreateRectRgnIndirect(&rtWindow);
	::CombineRgn(m_HotRgn, m_HotRgn, hTemp, RGN_DIFF);
	::DeleteObject(hTemp);

	m_lnkBuddyName.GetClientRect(&rtWindow);
	hTemp = ::CreateRectRgnIndirect(&rtWindow);
	::CombineRgn(m_HotRgn, m_HotRgn, hTemp, RGN_DIFF);
	::DeleteObject(hTemp);

	m_staBuddySign.GetClientRect(&rtWindow);
	hTemp = ::CreateRectRgnIndirect(&rtWindow);
	::CombineRgn(m_HotRgn, m_HotRgn, hTemp, RGN_DIFF);
	::DeleteObject(hTemp);
	
	m_SkinDlg.SetDragRegion(m_HotRgn);
}


/**
 * @brief 反初始化控件
 * 
 * @return BOOL 
 */
BOOL CBuddyChatDlg::UnInit()
{
	if (m_PicBarDlg.IsWindow())
	{
		m_PicBarDlg.DestroyWindow();
	}	

	if (m_picHead.IsWindow())
	{
		m_picHead.DestroyWindow();
	}	

	if (m_lnkBuddyName.IsWindow())
	{
		m_lnkBuddyName.DestroyWindow();
	}	

	if (m_staBuddySign.IsWindow())
	{
		m_staBuddySign.DestroyWindow();
	}	

	if (m_btnMsgLog.IsWindow())
	{
		m_btnMsgLog.DestroyWindow();
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

	if (m_richMsgLog.IsWindow())
	{
		m_richMsgLog.DestroyWindow();
	}	

	m_Accelerator.DestroyObject();
	//	m_menuRichEdit.DestroyMenu();

	DestroyFileTransferCtrl();

	return TRUE;
}


/**
 * @brief 富文本编辑框替代光标
 * 
 * @param hWnd 
 * @param lpszNewText 
 */
void CBuddyChatDlg::_RichEdit_ReplaceSel(HWND hWnd, LPCTSTR lpszNewText)
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
 * @brief 富文本编辑框插入表情，
 * 
 * @param hWnd 
 * @param lpszFileName 
 * @param nFaceId 
 * @param nFaceIndex 
 * @return BOOL 
 */
BOOL CBuddyChatDlg::_RichEdit_InsertFace(HWND hWnd, LPCTSTR lpszFileName, int nFaceId, int nFaceIndex)
{
	//TODO 分析此处有没有可以合并的代码
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

		long lStartChar = 0; 
		long lEndChar = 0;
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

//处理系统表情ID
BOOL CBuddyChatDlg::HandleSysFaceId(HWND hRichEditWnd, LPCTSTR& p, CString& strText)
{
	//int nFaceId = GetBetweenInt(p+2, _T("[\""), _T("\"]"), -1);
	//CFaceInfo* lpFaceInfo = m_lpFaceList->GetFaceInfoById(nFaceId);
	//if (lpFaceInfo != NULL)
	//{
	//	if (!strText.IsEmpty())
	//	{
	//		_RichEdit_ReplaceSel(hRichEditWnd, strText); 
	//		strText = _T("");
	//	}

	//	_RichEdit_InsertFace(hRichEditWnd, lpFaceInfo->m_strFileName.c_str(), 
	//		lpFaceInfo->m_nId, lpFaceInfo->m_nIndex);

	//	p = _tcsstr(p+2, _T("\"]"));
	//	p++;
	//	return TRUE;
	//}
	return FALSE;
}


/**
 * @brief 处理系统表情索引
 * 
 * @param hRichEditWnd 
 * @param p 
 * @param strText 
 * @return BOOL 
 */
BOOL CBuddyChatDlg::HandleSysFaceIndex(HWND hRichEditWnd, LPCTSTR& p, CString& strText)
{
	/*int nFaceIndex = GetBetweenInt(p+2, _T("[\""), _T("\"]"), -1);
	CFaceInfo* lpFaceInfo = m_lpFaceList->GetFaceInfoByIndex(nFaceIndex);
	if (lpFaceInfo != NULL)
	{
		if (!strText.IsEmpty())
		{
			_RichEdit_ReplaceSel(hRichEditWnd, strText); 
			strText = _T("");
		}

		_RichEdit_InsertFace(hRichEditWnd, lpFaceInfo->m_strFileName.c_str(), 
			lpFaceInfo->m_nId, lpFaceInfo->m_nIndex);

		p = _tcsstr(p+2, _T("\"]"));
		p++;
		return TRUE;
	}*/
	return FALSE;
}

//处理客户图片
BOOL CBuddyChatDlg::HandleCustomPic(HWND hRichEditWnd, LPCTSTR& p, CString& strText)
{
	//CString strFileName = GetBetweenString(p+2, _T("[\""), _T("\"]")).c_str();
	//if (!strFileName.IsEmpty())
	//{
	//	if (!strText.IsEmpty())
	//	{
	//		_RichEdit_ReplaceSel(hRichEditWnd, strText); 
	//		strText = _T("");
	//	}

	//	//if (::PathIsRelative(strFileName))
	//	//	strFileName = m_lpFMGClient->GetChatPicFullName(strFileName).c_str();

	//	_RichEdit_InsertFace(hRichEditWnd, strFileName, -1, -1);

	//	p = _tcsstr(p+2, _T("\"]"));
	//	p++;
	//	return TRUE;
	//}
	return FALSE;
}

/**
 * @brief 将消息添加到发送的编辑框
 * 
 * @param lpText 
 */
void CBuddyChatDlg::AddMsgToSendEdit(LPCTSTR lpText)
{
	//AddMsg(m_richSend.m_hWnd, lpText);
	m_richSend.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

void CBuddyChatDlg::OnFileTransResult(C_WND_MSG_FriendFileResult* pMsg)
{
	if (pMsg)
	{
		CString strFileName = EncodeUtil::Utf8ToUnicode(pMsg->m_szFileName);
		CString strResult;
		if (pMsg->m_eResult == FILE_TRANS_RESULT::E_TRANS_SUCCEED)
		{
			strResult = _T("文件传输成功");
		}
		else
		{
			strResult = _T("文件传输失败");
		}
		AddNotifyMsgToRecvEdit(strFileName+strResult);
	}
}
void CBuddyChatDlg::AddNotifyMsgToRecvEdit(const CString& strMsg)
{
	RichEdit_SetSel(m_richRecv.m_hWnd, -1, -1);
	RichEdit_ReplaceSel(m_richRecv.m_hWnd, strMsg, _T("微软雅黑"), 10, RGB(255, 0, 0), FALSE, FALSE, FALSE, FALSE, 0);
	RichEdit_ReplaceSel(m_richRecv.m_hWnd, _T("\r\n"));
	RichEdit_SetStartIndent(m_richRecv.m_hWnd, 0);
	::PostMessage(m_richRecv.m_hWnd, WM_VSCROLL, SB_BOTTOM, 0);
}

/**
 * @brief 插入自动回复的内容
 * 
 */
void CBuddyChatDlg::InsertAutoReplyContent()
{
	////TODO 
	//RichEdit_ReplaceSel(m_richRecv.m_hWnd, _T("\r\n"));
	//CString strAutoReplyContent;
	////(m_userConfig.GetAutoReplyContent());
	//if(strAutoReplyContent == "")
	//{
	//	strAutoReplyContent = _T("[自动回复]您好，我现在有事不在，稍候再联系您。");
	//}	
	//else
	//{
	//	strAutoReplyContent.Insert(0, _T("[自动回复]"));
	//}	

	//TCHAR szTime[32] = {0};
	//time_t nAutoReplyTime = time(NULL);
	//FormatTime(nAutoReplyTime, _T("%H:%M:%S"), szTime, ARRAYSIZE(szTime));

	//CString strText;
	//strText.Format(_T("%s  %s\r\n"), m_strUserName, szTime);

	//RichEdit_SetSel(m_richRecv.m_hWnd, -1, -1);
	//RichEdit_ReplaceSel(m_richRecv.m_hWnd, strText, _T("宋体"), 10, RGB(0, 0, 255), FALSE, FALSE, FALSE, FALSE, 0);
	//
	//C_UI_FontInfo fontInfo = m_FontSelDlg.GetFontInfo();
	//RichEdit_ReplaceSel(m_richRecv.m_hWnd, strAutoReplyContent, _T("微软雅黑"), fontInfo.m_nSize, fontInfo.m_clrText, fontInfo.m_bBold, fontInfo.m_bItalic, fontInfo.m_bUnderLine, FALSE, 300);
	////字体信息格式是：/0["字体名,字号,颜色,粗体,斜体,下划线"]
	//TCHAR szFontInfo[1024] = {0};
	//TCHAR szColor[32] = {0};
	//RGBToHexStr(fontInfo.m_clrText, szColor, ARRAYSIZE(szColor));
	//LPCTSTR lpFontFmt = _T("/o[\"%s,%d,%s,%d,%d,%d\"]");
	//wsprintf(szFontInfo, lpFontFmt, fontInfo.m_strName.c_str(), fontInfo.m_nSize, szColor, fontInfo.m_bBold, fontInfo.m_bItalic, fontInfo.m_bUnderLine);

	//strAutoReplyContent += szFontInfo;
}



/**
 * @brief 打开消息记录浏览窗口
 * 
 */
void CBuddyChatDlg::OpenMsgLogBrowser()
{
	if (m_pSess)
	{
		m_richMsgLog.SetWindowText(_T(""));
		m_richMsgLog.Invalidate(TRUE);

		ShowMsgLogButtons(TRUE);
		m_richMsgLog.SetFocus();
	}
}


/**
 * @brief 关闭消息记录浏览窗口
 * 
 */
void CBuddyChatDlg::CloseMsgLogBrowser()
{
	::SendMessage(m_richMsgLog.m_hWnd, WM_SETTEXT, 0, 0L);
	//if (m_dwThreadId != NULL)
	//{
	//	::PostThreadMessage(m_dwThreadId, WM_CLOSE_MSGLOGDLG, 0, 0);
	//	m_dwThreadId = NULL;
	//}

	ShowMsgLogButtons(FALSE);

	//CalculateMsgLogCountAndOffset();
}

/**
 * @brief 在富文本编辑器中显示图片
 * 
 * @param pszFileName 图片的全路径
 * @param hWnd 编辑框Handle
 * @param nWidth 宽度
 * @param nHeight 高度
 * @return BOOL 
 */
BOOL CBuddyChatDlg::GetImageDisplaySizeInRichEdit(PCTSTR pszFileName, HWND hWnd, long& nWidth, long& nHeight)
{

	//TODO  此处长宽的计算可以考虑合并
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
 * @brief 重新调整接收编辑器的图片大小
 * 
 */
void CBuddyChatDlg::ResizeImageInRecvRichEdit()
{
	std::vector<ImageInfo*> arrImageInfo;
	RichEdit_GetImageInfo(m_richRecv, arrImageInfo);
	size_t nSize = arrImageInfo.size();
	if(nSize == 0)
	{
		return;
	}	
	
	ImageInfo* pImage = NULL;
	for(size_t i=0; i<nSize; ++i)
	{
		pImage = arrImageInfo[i];
		if(pImage == NULL)
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
 * @brief 响应发送消息的结果
 * 
 * @param uMsg 
 * @param wParam 
 * @param lParam 
 * @return LRESULT 
 */
LRESULT CBuddyChatDlg::OnSendChatMsgResult(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
 * @brief 显示接收到的最后一条消息
 * 
 */
void CBuddyChatDlg::ShowLastMsgInRecvRichEdit()
{
	
}

void CBuddyChatDlg::OnSizeHideRightArea()
{
	if(m_TabMgr.IsWindow())
	{
		m_TabMgr.ShowWindow(SW_HIDE);
	}

	if (m_FileTransferCtrl.IsWindow())
	{
		m_FileTransferCtrl.ShowWindow(SW_HIDE);
	}

	if (m_richMsgLog.IsWindow())
	{
		m_richMsgLog.ShowWindow(SW_HIDE);
	}

	if (m_btnFirstMsgLog.IsWindow())
	{
		m_btnFirstMsgLog.ShowWindow(SW_HIDE);
	}
	
	if (m_btnPrevMsgLog.IsWindow())
	{
		m_btnPrevMsgLog.ShowWindow(SW_HIDE);
	}

	if (m_btnNextMsgLog.IsWindow())
	{
		m_btnNextMsgLog.ShowWindow(SW_HIDE);
	}

	if (m_btnLastMsgLog.IsWindow())
	{
		m_btnLastMsgLog.ShowWindow(SW_HIDE);
	}
}

/**
 * @brief 记录窗口大小
 * 
 */
void CBuddyChatDlg::RecordWindowSize()
{	
	if(IsZoomed() || IsIconic())
	{
		return;
	}	

	CRect rtWindow;
	GetWindowRect(&rtWindow);
	if (m_bMsgLogWindowVisible || m_bFileTransferVisible)
	{
		m_userConfig.SetChatDlgWidth(rtWindow.Width() - RIGHT_CHAT_WINDOW_WIDTH);
	}
	else
	{
		m_userConfig.SetChatDlgWidth(rtWindow.Width());
	}

	m_userConfig.SetChatDlgHeight(rtWindow.Height());
}


/**
 * @brief 重新计算控件的位置
 * 
 * @param nMouseY 
 */
void CBuddyChatDlg::ReCaculateCtrlPostion(long nMouseY)
{
	CRect rtClient;
	::GetClientRect(m_hWnd, &rtClient);
	
	//不允许将发送框尺寸拉的太大或者太小，那样会影响某些控件内部的画法（某些控件内部画法要求必须满足一定的尺寸）
	if( (nMouseY<=280) || 
	    (nMouseY>= rtClient.Height()-95) )
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
    if (m_FontSelDlg.IsWindow() && m_FontSelDlg.IsWindowVisible())
	{
		//AtlTrace(_T("nMouseY-ptBase.y-3*CHATDLG_TOOLBAR_HEIGHT: %d\n"), nMouseY-ptBase.y-3*CHATDLG_TOOLBAR_HEIGHT);
		//TODO: nMouseY-ptBase.y-2*CHATDLG_TOOLBAR_HEIGHT为什么不起作用呢？
		::DeferWindowPos(hdwp, m_richRecv, NULL, 0, 0, rtRichRecv.Width(), nMouseY-ptBase.y-2*CHATDLG_TOOLBAR_HEIGHT, SWP_NOMOVE|SWP_NOZORDER);
		::DeferWindowPos(hdwp, m_FontSelDlg, NULL, 0, ptBase.y+rtRichRecv.Height()-CHATDLG_TOOLBAR_HEIGHT, 0, CHATDLG_TOOLBAR_HEIGHT, SWP_NOSIZE|SWP_NOZORDER);
	}
	else
	{
		::DeferWindowPos(hdwp, m_richRecv, NULL, 0, 0, rtRichRecv.Width(), nMouseY-ptBase.y-CHATDLG_TOOLBAR_HEIGHT, SWP_NOMOVE|SWP_NOZORDER);
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
	if(m_bMsgLogWindowVisible || m_bFileTransferVisible)
	{
			::DeferWindowPos(hdwp, 
			        m_richSend, 
					NULL,
					6,
					ptBase.y+rtRichRecv.Height()+rtSplitter.Height()+rtMidToolbar.Height(), 
					rtClient.Width()-8-RIGHT_CHAT_WINDOW_WIDTH, 
					nHeightRichSend, 
					SWP_NOZORDER);
	}
	else
	{
		::DeferWindowPos(hdwp, 
				m_richSend, 
				NULL,
				6, 
				ptBase.y+rtRichRecv.Height()+rtSplitter.Height()+rtMidToolbar.Height(), 
				rtClient.Width()-8,
				nHeightRichSend,
				SWP_NOZORDER);
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


/**
 * @brief 响应以P2P方式发送文件的菜单
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 * @return LRESULT 
 */
LRESULT CBuddyChatDlg::OnSendFileP2p(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	// TODO: 在此添加命令处理程序代码
	TCHAR	cFileName[MAX_PATH] = { 0 };
	BOOL	bOpenFileDialog = TRUE;
	LPCTSTR lpszDefExt = NULL;
	LPCTSTR lpszFileName = _T("选择文件");
	LPCTSTR lpszFilter = _T("所有文件(*.)\0\0");
	DWORD	dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR | OFN_EXTENSIONDIFFERENT;
	HWND	hWndParent = m_hWnd;

	CFileDialog fileDlg(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, hWndParent);
	if (fileDlg.DoModal() == IDOK)
	{
		CString strSavePath = fileDlg.m_ofn.lpstrFile;
		CString strExtName = (Hootina::CPath::GetExtension(strSavePath)).c_str();

		CString strFileTypeThumbs = Hootina::CPath::GetAppPath().c_str();
		strFileTypeThumbs += _T("\\Skins\\Skin0\\fileTypeThumbs\\");
		strFileTypeThumbs += strExtName;
		strFileTypeThumbs += _T(".png");
		m_SendFileThumbPicture.SetBitmap(strFileTypeThumbs);
		if (!m_bMsgLogWindowVisible)
		{
			::SendMessage(m_btnMsgLog.m_hWnd, BM_CLICK, 0, 0);
			m_bMsgLogWindowVisible = TRUE;
		}
		//::SendMessage(m_btnMsgLog.m_hWnd, BM_CLICK, 0, 0);
		m_RightTabCtrl.SetCurSel(1);
		m_richMsgLog.ShowWindow(SW_HIDE);
		ShowFileTransferCtrl(TRUE);
		//m_staSendFileDesc.SetWindowText(Hootina::CPath::GetFileName(strSavePath).c_str());

		SendFileOnLineP2P(strSavePath);
	}
	return 0;
}
