/**
 * @file FontSelDlg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 字体选择对话框
 * @version 0.1
 * @date 2019-08-08
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _TINY_IM_C_FONT_SEL_DLG_H_ 
#define _TINY_IM_C_FONT_SEL_DLG_H_

#include "resource.h"
#include "SkinLib/SkinLib.h"
#include "UI_USER_INFO.h"
#include "Utils.h"
#include "UserConfig.h"



extern C_UI_FontInfo g_fontInfo;				// 字体信息
extern TCHAR g_cHotKey;						// 提取消息热键

//BOOL LoadMiscConfig(LPCTSTR lpszFileName);	// 加载杂项配置
//BOOL SaveMiscConfig(LPCTSTR lpszFileName);	// 保存杂项配置


class CFontSelDlg : public CDialogImpl<CFontSelDlg>
{
public:
	CFontSelDlg(void);
	~CFontSelDlg(void);

	enum { IDD = IDD_FONT_SEL_DLG };

	BEGIN_MSG_MAP_EX(CFontSelDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER_EX(ID_GROUP_CHAT_DLG_SYS_FONT_BTN, OnBtn_SysFont)
		COMMAND_ID_HANDLER_EX(ID_GROUP_CHAT_DLG_MEMBER_FONT_BTN, OnBtn_MemberFont)
		COMMAND_HANDLER_EX(ID_COMBO_FONT_NAME, CBN_SELCHANGE, OnCbo_SelChange_FontName)
		COMMAND_HANDLER_EX(ID_COMBO_FONT_SIZE, CBN_SELCHANGE, OnCbo_SelChange_FontSize)
		COMMAND_ID_HANDLER_EX(ID_GROUP_CHAT_DLG_BOLD_BTN, OnBtn_Bold)
		COMMAND_ID_HANDLER_EX(ID_GROUP_CHAT_DLG_ITALIC_BTN, OnBtn_Italic)
		COMMAND_ID_HANDLER_EX(ID_GROUP_CHAT_DLG_UNDER_LINE_BTN, OnBtn_UnderLine)
		COMMAND_HANDLER_EX(ID_GROUP_CHAT_DLG_COLOR_BTN, BN_CLICKED, OnBtn_Color)
	END_MSG_MAP()

public:
	C_UI_FontInfo& GetPublicFontInfo();		// 获取全局字体信息
	C_UI_FontInfo& GetFontInfo();			// 获取字体信息

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnClose();
	void OnDestroy();
	void OnBtn_SysFont(UINT uNotifyCode, int nID, CWindow wndCtl);		// “系统字体”按钮
	void OnBtn_MemberFont(UINT uNotifyCode, int nID, CWindow wndCtl);	// “会员炫彩字”按钮
	void OnCbo_SelChange_FontName(UINT uNotifyCode, int nID, CWindow wndCtl);	// “字体名称”组合框
	void OnCbo_SelChange_FontSize(UINT uNotifyCode, int nID, CWindow wndCtl);	// “字体大小”组合框
	void OnBtn_Bold(UINT uNotifyCode, int nID, CWindow wndCtl);			// “加粗”按钮
	void OnBtn_Italic(UINT uNotifyCode, int nID, CWindow wndCtl);		// “倾斜”按钮
	void OnBtn_UnderLine(UINT uNotifyCode, int nID, CWindow wndCtl);	// “下划线”按钮
	void OnBtn_Color(UINT uNotifyCode, int nID, CWindow wndCtl);		// “颜色”按钮

	BOOL InitFontClassToolBar();	// 初始化字体分类工具栏
	BOOL InitFontNameComboBox();	// 初始化字体名称组合框
	BOOL InitFontSizeComboBox();	// 初始化字体大小组合框
	BOOL InitFontAttrToolBar();		// 初始化字体属性工具栏

	BOOL Init();					// 初始化
	void UnInit();					// 反初始化



private:
	CSkinDialog			m_SkinDlg;//对话框主体
	CSkinToolBar		m_tbFontClass; //字体类型工具栏
	CSkinToolBar		m_tbFontAttr; //字体属性工具栏
	CSkinComboBox		m_cboFontName;//字体名称组合框
	CSkinComboBox		m_cboFontSize;//字体大小组合框
	C_UI_FontInfo			m_fontInfo;//界面的字体信息
	CUserConfig&		m_userConfig;//用户设置
};

#endif