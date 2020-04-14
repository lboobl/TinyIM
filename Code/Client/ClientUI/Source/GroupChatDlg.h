
/**
 * @file GroupChatDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 群聊对话框
 * @version 0.1
 * @date 2019-08-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _TINY_IM_C_GROUP_CHAT_DLG_H_ 
#define _TINY_IM_C_GROUP_CHAT_DLG_H_
#include "resource.h"
#include "CustomMsgDef.h"
#include "SkinLib/SkinLib.h"
#include "RichEditUtil.h"
#include "CascadeWinManager.h"
#include "FontSelDlg.h"
#include "FaceSelDlg.h"
#include "PicBarDlg.h"
#include "SplitterCtrl.h"
#include "UI_USER_INFO.h"
#include "IUProtocolData.h"
#include "Proto.h"
#include "UICommonDef.h"
#include "UIWndMsgDef.h"
#include "UserConfig.h"
class CGroupChatDlg : public CDialogImpl<CGroupChatDlg>, public CMessageFilter
{
public:
	CGroupChatDlg(void);
	~CGroupChatDlg(void);

	enum { IDD = IDD_GROUP_CHAT_DLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP_EX(CGroupChatDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_COPYDATA(OnCopyData)
		MSG_WM_MEASUREITEM(OnMeasureItem)
		MSG_WM_DRAWITEM(OnDrawItem)
		MSG_WM_GETMINMAXINFO(OnGetMinMaxInfo)
		MSG_WM_MOVE(OnMove)
		MSG_WM_SIZE(OnSize)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_TIMER(OnTimer)
		//MSG_WM_DROPFILES(OnDropFiles)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)

		COMMAND_HANDLER_EX(ID_LINK_GROUP_NAME, BN_CLICKED, OnLnk_GroupName)
		COMMAND_ID_HANDLER_EX(ID_GROUP_CHAT_DLG_FONT_BTN, OnBtn_Font)
		COMMAND_ID_HANDLER_EX(ID_GROUP_CHAT_DLG_FACE_BTN, OnBtn_Face)
		COMMAND_ID_HANDLER_EX(ID_GROUP_CHAT_DLG_IMAGE_BTN, OnBtn_Image)
		COMMAND_ID_HANDLER_EX(ID_GROUP_CHAT_DLG_SCREEN_SHOT_BTN, OnBtn_ScreenShot)
		COMMAND_ID_HANDLER_EX(ID_GROUP_CHAT_DLG_MSG_LOG_BTN, OnBtn_MsgLog)
		COMMAND_HANDLER_EX(ID_GROUP_CHAT_DLG_MSG_PROMPT_BTN, BN_CLICKED, OnBtn_MsgNotPrompt)
		COMMAND_ID_HANDLER_EX(ID_GROUP_CHAT_DLG_SAVE_AS_BTN, OnBtn_SaveAs)

		COMMAND_HANDLER_EX(ID_BTN_CLOSE, BN_CLICKED, OnBtn_Close)
		COMMAND_ID_HANDLER_EX(ID_BTN_SEND, OnBtn_Send)
		COMMAND_HANDLER_EX(ID_BTN_ARROW, BN_CLICKED, OnBtn_Arrow)

		COMMAND_ID_HANDLER_EX(ID_GROUP_CHAT_DLG_PRESS_ENTER_MENU, OnPressEnterMenuItem)					//回车键发送消息
		COMMAND_ID_HANDLER_EX(ID_GROUP_CHAT_DLG_PRESS_CTRL_ENTER_MENU, OnPressCtrlEnterMenuItem)				//Ctrl+回车发送消息

		COMMAND_RANGE_HANDLER_EX(IDC_FIRST_MSG_LOG, IDC_LAST_MSG_LOG, OnMsgLogPage) //消息记录最前一条，上一条，下一条，最后一条四个按钮

		NOTIFY_HANDLER_EX(ID_TOOL_BAR_TOP, TBN_DROPDOWN, OnToolbarDropDown)
		NOTIFY_HANDLER_EX(ID_TOOL_BAR_MID, TBN_DROPDOWN, OnToolbarDropDown)

		MESSAGE_HANDLER_EX(WM_UPDATE_FONTINFO, OnUpdateFontInfo)
		MESSAGE_HANDLER_EX(FACE_CTRL_SEL, OnFaceCtrlSel)
		MESSAGE_HANDLER_EX(WM_SET_DLG_INIT_FOCUS, OnSetDlgInitFocus)

		NOTIFY_HANDLER_EX(ID_RICH_EDIT_RECV, EN_LINK, OnRichEdit_Recv_Link)
		NOTIFY_HANDLER_EX(ID_RICH_EDIT_SEND, EN_PASTE, OnRichEdit_Send_Paste)
		NOTIFY_HANDLER_EX(ID_LIST_MERBER, NM_DBLCLK, OnGMemberList_DblClick)
		NOTIFY_HANDLER_EX(ID_LIST_MERBER, NM_RCLICK, OnGMemberList_RClick)
		COMMAND_ID_HANDLER_EX(ID_MENU_CUT, OnMenu_Cut)
		COMMAND_ID_HANDLER_EX(ID_MENU_COPY, OnMenu_Copy)
		COMMAND_ID_HANDLER_EX(ID_MENU_PASTE, OnMenu_Paste)
		COMMAND_ID_HANDLER_EX(ID_MENU_SELECT_ALL, OnMenu_SelAll)
		COMMAND_ID_HANDLER_EX(ID_MENU_CLEAR, OnMenu_Clear)
		COMMAND_RANGE_HANDLER_EX(ID_MENU_ZOOM_RATIO_400, ID_MENU_ZOOM_RATIO_50, OnMenu_ZoomRatio)
		COMMAND_ID_HANDLER_EX(ID_MENU_SAVE_AS, OnMenu_SaveAs)
		COMMAND_ID_HANDLER_EX(ID_MENU_VIEW_INFO, OnMenu_ViewInfo)
		COMMAND_ID_HANDLER_EX(ID_MENU_SEND_MSG, OnMenu_SendMsg)


		MESSAGE_HANDLER_EX(FMG_MSG_SENDCHATMSG_RESULT, OnSendChatMsgResult)	

		MESSAGE_HANDLER_EX(FMG_MSG_SEND_FILE_PROGRESS, OnSendFileProgress)
		MESSAGE_HANDLER_EX(FMG_MSG_SEND_FILE_RESULT, OnSendFileResult)
		//MESSAGE_HANDLER_EX(FMG_MSG_RECV_FILE_PROGRESS, OnRecvFileProgress)
		MESSAGE_HANDLER_EX(FMG_MSG_RECV_FILE_RESULT, OnRecvFileResult)
	END_MSG_MAP()

public:
	//void OnRecvMsg(UINT nGroupCode, UINT nMsgId);				// 接收群消息
	void OnUpdateGroupInfo();									// 更新群信息
	void OnUpdateGroupNumber();									// 更新群号码
	void OnUpdateGroupHeadPic();								// 更新群头像
	void OnUpdateGMemberNumber(WPARAM wParam, LPARAM lParam);	// 更新群成员号码
	void OnUpdateGMemberHeadPic(WPARAM wParam, LPARAM lParam);	// 更新群成员头像

	void OnRecvGroupMsg(C_UI_GroupMessage* pGroupMsg);
	void OnRecvGroupLogMsg(C_UI_GroupMessage* pGroupMsg);
private:

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	BOOL OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct);
	void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	void OnGetMinMaxInfo(LPMINMAXINFO lpMMI);
	void OnMove(CPoint ptPos);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnSize(UINT nType, CSize size);
	void OnTimer(UINT_PTR nIDEvent);
	void OnClose();
	void OnDestroy();

	void OnLnk_GroupName(UINT uNotifyCode, int nID, CWindow wndCtl);// “群名称”超链接控件
	void OnBtn_Font(UINT uNotifyCode, int nID, CWindow wndCtl);		// “字体选择工具栏”按钮
	void OnBtn_Face(UINT uNotifyCode, int nID, CWindow wndCtl);		// “表情”按钮
	void OnBtn_Image(UINT uNotifyCode, int nID, CWindow wndCtl);	// “发送图片”按钮
	void OnBtn_MsgNotPrompt(UINT uNotifyCode, int nID, CWindow wndCtl);	// “来消息不提示”按钮
	void OnBtn_ScreenShot(UINT uNotifyCode, int nID, CWindow wndCtl);	// 截图工具
	void OnBtn_MsgLog(UINT uNotifyCode, int nID, CWindow wndCtl);	// “消息记录”按钮
	void OnBtn_SaveAs(UINT uNotifyCode, int nID, CWindow wndCtl);	// “点击另存为”按钮

	void OnBtn_Close(UINT uNotifyCode, int nID, CWindow wndCtl);	// “关闭”按钮
	void OnBtn_Send(UINT uNotifyCode, int nID, CWindow wndCtl);		// “发送”按钮
	void OnBtn_Arrow(UINT uNotifyCode, int nID, CWindow wndCtl);	// “箭头”按钮

	LRESULT OnToolbarDropDown(LPNMHDR pnmh);

	void OnMsgLogPage(UINT uNotifyCode, int nID, CWindow wndCtl);		// 消息记录翻页按钮

	LRESULT OnUpdateFontInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 更新字体信息
	LRESULT OnFaceCtrlSel(UINT uMsg, WPARAM wParam, LPARAM lParam);		// “表情”控件选取消息
	LRESULT OnSetDlgInitFocus(UINT uMsg, WPARAM wParam, LPARAM lParam);	// 设置对话框初始焦点
	
	LRESULT OnRichEdit_Recv_Link(LPNMHDR pnmh);						//	“接收消息”富文本框链接点击消息
	LRESULT OnRichEdit_Send_Paste(LPNMHDR pnmh);
	LRESULT OnGMemberList_DblClick(LPNMHDR pnmh);					// “群成员”列表双击消息
	LRESULT OnGMemberList_RClick(LPNMHDR pnmh);						// “群成员”列表右键单击消息
	void OnMenu_Cut(UINT uNotifyCode, int nID, CWindow wndCtl);		// “剪切”菜单
	void OnMenu_Copy(UINT uNotifyCode, int nID, CWindow wndCtl);	// “复制”菜单
	void OnMenu_Paste(UINT uNotifyCode, int nID, CWindow wndCtl);	// “粘贴”菜单
	void OnMenu_SelAll(UINT uNotifyCode, int nID, CWindow wndCtl);	// “全部选择”菜单
	void OnMenu_Clear(UINT uNotifyCode, int nID, CWindow wndCtl);	// “清屏”菜单
	void OnMenu_ZoomRatio(UINT uNotifyCode, int nID, CWindow wndCtl);// “显示比例”菜单
	void OnMenu_SaveAs(UINT uNotifyCode, int nID, CWindow wndCtl);	// “另存为”菜单
	void OnMenu_ViewInfo(UINT uNotifyCode, int nID, CWindow wndCtl);// “查看资料”菜单
	void OnMenu_SendMsg(UINT uNotifyCode, int nID, CWindow wndCtl);	// “发送消息”菜单
	//void OnMenu_DeleteSelectMsgLog(UINT uNotifyCode, int nID, CWindow wndCtl);//“删除选中消息记录”菜单
	//void OnMenu_ClearMsgLog(UINT uNotifyCode, int nID, CWindow wndCtl); //“清空消息记录”菜单

	void OnPressEnterMenuItem(UINT uNotifyCode, int nID, CWindow wndCtl);		//回车键发送消息
	void OnPressCtrlEnterMenuItem(UINT uNotifyCode, int nID, CWindow wndCtl);	//Ctrl+回车发送消息

	LRESULT OnSendChatMsgResult(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnSendFileProgress(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSendFileResult(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnRecvFileResult(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	BOOL OnRichEdit_MouseMove(MSG* pMsg);		// 发送/接收文本框的鼠标移动消息
	BOOL OnRichEdit_LBtnDblClk(MSG* pMsg);		// 发送/接收文本框的鼠标双击消息
	BOOL OnRichEdit_RBtnDown(MSG* pMsg);		// 发送/接收文本框的鼠标右键按下消息


	void UpdateData();					// 更新信息
	void UpdateDlgTitle();				// 更新对话框标题栏
	BOOL UpdateGroupNameCtrl();			// 更新群名称控件
	BOOL UpdateGroupMemo();				// 更新群公告
	BOOL UpdateGroupMemberList();		// 更新群成员列表

	BOOL InitTopToolBar();				// 初始化Top工具栏
	BOOL InitMidToolBar();				// 初始化Middle工具栏
	BOOL InitRichEditOleCallback();		// 初始化IRichEditOleCallback接口

	BOOL Init();						// 初始化
	BOOL UnInit();						// 反初始化

	void ShowMsgLogButtons(BOOL bShow);


	void SetHotRgn();

	BOOL HandleFileDragResult(PCTSTR lpszFileName);

	int FindMemberListByUin(UINT nUTalkUin);
	void _RichEdit_ReplaceSel(HWND hWnd, LPCTSTR lpszNewText);
	BOOL _RichEdit_InsertFace(HWND hWnd, LPCTSTR lpszFileName, int nFaceId, int nFaceIndex);
	BOOL HandleFontInfo(LPCTSTR& p, WString& strText, std::vector<C_UI_Content*>& arrContent);
	BOOL HandleSysFaceId(HWND hRichEditWnd, LPCTSTR& p, CString& strText);
	BOOL HandleSysFaceIndex(HWND hRichEditWnd, LPCTSTR& p, CString& strText);
	BOOL HandleCustomPic(HWND hRichEditWnd, LPCTSTR& p, CString& strText);
	void AddMsg(HWND hRichEditWnd, LPCTSTR lpText);
	void AddMsgToSendEdit(LPCTSTR lpText);
	void AddMsgToRecvEdit(time_t nTime, LPCTSTR lpText);
	void AddMsgToRecvEdit(C_UI_BuddyMessage* lpGroupMsg);
	void AddMsgToRecvEdit(std::vector<C_UI_Content*>& arrContent);
	//void AddMsgToRecvEdit(std::vector<GROUP_MSG_LOG*>& arrMsgLog);

	//Dennis Begin
	void AddUserToRecvEdit(const std::string strUserName, const time_t nTime);
	void AddTextToRecvEdit(const std::string strContext);
	void AddSendTextToRecvEdit(const CString& strContext);
	//Dennis End

	void ShowLastMsgInRecvRichEdit();

	void OpenMsgLogBrowser();			// 打开消息记录浏览窗口
	void CloseMsgLogBrowser();			// 关闭消息记录浏览窗口

	//void AddMsgToMsgLogEdit(std::vector<GROUP_MSG_LOG*>& arrMsgLog);
	void AnalyseContent(WString& strContent, HWND hRichWnd=NULL);
	void AddMsgToMsgLogEdit(std::vector<C_UI_Content*>& arrContent);
	BOOL HandleSysFaceId(LPCTSTR& p, WString& strText, std::vector<C_UI_Content*>& arrContent);
	BOOL HandleCustomPic(LPCTSTR& p, WString& strText, std::vector<C_UI_Content*>& arrContent);

	BOOL InitRightTabWindow();

	void ResizeImageInRecvRichEdit();
	BOOL GetImageDisplaySizeInRichEdit(PCTSTR pszFileName, HWND hWnd, long& nWidth, long& nHeight);	//按窗口尺寸来缩放图片大小

	void ReCaculateCtrlPostion(long nMouseY);

	void OnRecvToHandle(const HWND recvHandle, C_UI_GroupMessage* pGroupMsg);
	void OnSizeNotShowMsgHistory();
public:
	std::shared_ptr<CMsgProto> m_netProto; //网络协议指针
	CFaceList*				m_lpFaceList; //头像列表
	CCascadeWinManager*		m_lpCascadeWinManager;//TODO 
	HWND					m_hMainDlg; //对话框HANDLE
	std::string				m_strGroupId; //群组唯一标识
private:
	CUserConfig&			m_userConfig;
	CSkinDialog				m_SkinDlg;//对话框
	CSkinPictureBox			m_picHead;//群组头像控件
	CSkinHyperLink			m_lnkGroupName;//群组名称链接
	CSkinStatic				m_staGroupCategory;//TODO
	CSkinStatic				m_staMemoTitle;//TODO 
	CSkinStatic             m_staMemberTitle;//TODO
	CSkinEdit				m_edtMemo;//TODO 
	CSkinButton				m_btnClose;//关闭按钮
	CSkinButton             m_btnSend;//消息发送按钮
	CSkinButton             m_btnArrow;//TODO
	CSkinMenu				m_SkinMenu;//TODO 
	CSkinToolBar			m_tbTop;//顶部工具栏 
	CSkinToolBar            m_tbMid;//中部工具栏
	CFontSelDlg				m_FontSelDlg;//字体选择对话框
	CFaceSelDlg				m_FaceSelDlg;//头像选择对话框
	CPicBarDlg				m_PicBarDlg;//TODO 
	
	CSkinRichEdit			m_richRecv;//消息接收控件
	CSkinRichEdit           m_richSend;//消息发送编辑控件
	
	CSkinTabCtrl			m_RightTabCtrl;	//聊天窗口右边的Tab窗口
	CSkinListCtrl			m_GroupMemberListCtrl;   //群成员列表

	//历史消息记录相关 begin
	CSkinRichEdit           m_richMsgLog;//历史消息富文本控件
	CSkinButton				m_btnFirstMsgLog;//跳转到第一条消息按钮
	CSkinButton             m_btnPrevMsgLog;//前一条消息按钮
	CSkinButton             m_btnNextMsgLog;//下一条消息按钮
	CSkinButton             m_btnLastMsgLog;//最后一条消息按钮
	CSkinStatic				m_staMsgLogPage;//显示当前是第几页的历史消息

	BOOL					m_bMsgLogWindowVisible;  //是否显示历史消息控件
	long					m_nMsgLogIndexInToolbar;

	//CMessageLogger			m_MsgLogger;				// 消息记录
	long					m_nMsgLogRecordOffset;		//当前消息记录的偏移量+1
	long					m_nMsgLogCurrentPageIndex;	//消息记录当前页码
	//历史消息记录相关 End

	CSplitterCtrl			m_SplitterCtrl;//历史消息扩展控件
	
	CAccelerator			m_Accelerator;//TODO 
	HICON					m_hDlgIcon;//对话框图标
	HICON                   m_hDlgSmallIcon;//对话框小图标
	HWND					m_hRBtnDownWnd;//TODO
	POINT					m_ptRBtnDown;//TODO 
	IImageOle*				m_pLastImageOle;//TODO
	int						m_cxPicBarDlg;//TODO 
	int                     m_cyPicBarDlg;//TODO 

	CString					m_strCurLink;//TODO

	CString					m_strGroupName;//群组名

	int						m_nMemberCnt;			//群成员总数
	int                     m_nOnlineMemberCnt;		//在线的群成员数

	HRGN					m_HotRgn;

	std::map<CString, long>	m_mapSendFileInfo;		//发送文件队列，键是文件名，如果键值为-1则是文件；如果键值大于0则是图片在窗口位置的索引
	std::map<CString, long>	m_mapRecvFileInfo;		//接收文件队列，键是文件显示名，键值为-1是收取的离线文件；键值大于1是收取的聊天图片应插入的位置索引

	BOOL					m_bPressEnterToSendMessage;	//TRUE按回车键发送消息, FALSE按Ctrl+Enter发送消息




	BOOL					m_bDraged;					//用户是否拖拽过发送文本框的尺寸
	RECT					m_rtRichRecv;		//接收消息区域的富文本框矩形
	RECT					m_rtMidToolBar;
	RECT					m_rtSplitter; 
	RECT					m_rtRichSend; //发送富文本控件矩形区域

	void OnSizeShowMsgHistory();
};

#endif