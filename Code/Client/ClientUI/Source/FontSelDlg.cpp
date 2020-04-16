/**
 * @file FontSelDlg.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 字体选择对话框实现文件
 * @version 0.1
 * @date 2020-02-29
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "stdafx.h"
#include "FontSelDlg.h"
#include "UIDefaultValue.h"
#include "UserConfig.h"
#include "UIWndMsgDef.h"
static C_UI_FontInfo g_fontInfo;		// 字体信息
TCHAR g_cHotKey = _T('D');	// 提取消息热键


/**
 * @brief Construct a new CFontSelDlg::CFontSelDlg object
 * 
 */
CFontSelDlg::CFontSelDlg(void):m_userConfig(CUserConfig::GetInstance())
{
}

/**
 * @brief Destroy the CFontSelDlg::CFontSelDlg object
 * 
 */
CFontSelDlg::~CFontSelDlg(void)
{
}


/**
 * @brief 获取全局字体信息
 * 
 * @return C_UI_FontInfo& 
 */
C_UI_FontInfo& CFontSelDlg::GetPublicFontInfo()
{
	return g_fontInfo;
}


/**
 * @brief 获取字体信息
 * 
 * @return C_UI_FontInfo& 
 */
C_UI_FontInfo& CFontSelDlg::GetFontInfo()
{
	return m_fontInfo;
}

/**
 * @brief 响应初始化对话框
 * 
 * @param wndFocus 
 * @param lInitParam 
 * @return BOOL 
 */
BOOL CFontSelDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	Init();		// 初始化

	ModifyStyle(WS_CLIPSIBLINGS|WS_CLIPCHILDREN, 0);
	return FALSE;
}

/**
 * @brief 响应对话框被关闭
 * 
 */
void CFontSelDlg::OnClose()
{
	DestroyWindow();
}

/**
 * @brief 响应对话框被销毁
 * 
 */
void CFontSelDlg::OnDestroy()
{
	UnInit();	// 反初始化
}

/**
 * @brief 响应“系统字体”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CFontSelDlg::OnBtn_SysFont(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (BN_PUSHED == uNotifyCode)
	{
		m_tbFontClass.SetItemCheckState(1, FALSE);
	}
	else if (BN_UNPUSHED == uNotifyCode)
	{
		m_tbFontClass.SetItemCheckState(0, TRUE);
	}
}


/**
 * @brief 响应“会员炫彩字”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CFontSelDlg::OnBtn_MemberFont(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (BN_PUSHED == uNotifyCode)
	{
		m_tbFontClass.SetItemCheckState(0, FALSE);
	}
	else if (BN_UNPUSHED == uNotifyCode)
	{
		m_tbFontClass.SetItemCheckState(1, TRUE);
	}
}


/**
 * @brief 响应“字体名称”组合框,选择不同字体的时候使用
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CFontSelDlg::OnCbo_SelChange_FontName(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCurSel = m_cboFontName.GetCurSel();

	CString strText;
	m_cboFontName.GetLBText(nCurSel, strText);

	if (m_fontInfo.m_strName != strText.GetBuffer())
	{
		m_fontInfo.m_strName = strText.GetBuffer();
		g_fontInfo.m_strName = m_fontInfo.m_strName;
		long nIndex = m_cboFontName.FindString(0, m_fontInfo.m_strName.c_str());
		::PostMessage(GetParent(), WM_UPDATE_FONTINFO, NULL, NULL);
	}
}


/**
 * @brief 响应“字体大小”组合框,选择不同字号的时候使用
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CFontSelDlg::OnCbo_SelChange_FontSize(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCurSel = m_cboFontSize.GetCurSel();

	CString strText;
	m_cboFontSize.GetLBText(nCurSel, strText);

	int nSize = _tcstol(strText.GetBuffer(), NULL, 10);
	if (m_fontInfo.m_nSize != nSize)
	{
		m_fontInfo.m_nSize = nSize;
		g_fontInfo.m_nSize = m_fontInfo.m_nSize;
		::PostMessage(GetParent(), WM_UPDATE_FONTINFO, NULL, NULL);
	}
}


/**
 * @brief 响应字体风格的“加粗”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CFontSelDlg::OnBtn_Bold(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (BN_PUSHED == uNotifyCode || BN_UNPUSHED == uNotifyCode)
	{
		BOOL bBold = ((BN_PUSHED == uNotifyCode) ? TRUE : FALSE);
		if (m_fontInfo.m_bBold != bBold)
		{
			m_fontInfo.m_bBold = bBold;
			g_fontInfo.m_bBold = m_fontInfo.m_bBold;
			::PostMessage(GetParent(), WM_UPDATE_FONTINFO, NULL, NULL);
		}
	}
}


/**
 * @brief 响应字体风格的“倾斜”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CFontSelDlg::OnBtn_Italic(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (BN_PUSHED == uNotifyCode || BN_UNPUSHED == uNotifyCode)
	{
		BOOL bItalic = ((BN_PUSHED == uNotifyCode) ? TRUE : FALSE);
		if (m_fontInfo.m_bItalic != bItalic)
		{
			m_fontInfo.m_bItalic = bItalic;
			g_fontInfo.m_bItalic = m_fontInfo.m_bItalic;
			::PostMessage(GetParent(), WM_UPDATE_FONTINFO, NULL, NULL);
		}
	}
}


/**
 * @brief 响应字体风格的“下划线”按钮
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CFontSelDlg::OnBtn_UnderLine(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (BN_PUSHED == uNotifyCode || BN_UNPUSHED == uNotifyCode)
	{
		BOOL bUnderLine = ((BN_PUSHED == uNotifyCode) ? TRUE : FALSE);
		if (m_fontInfo.m_bUnderLine != bUnderLine)
		{
			m_fontInfo.m_bUnderLine = bUnderLine;
			g_fontInfo.m_bUnderLine = m_fontInfo.m_bUnderLine;
			::PostMessage(GetParent(), WM_UPDATE_FONTINFO, NULL, NULL);
		}
	}
}


/**
 * @brief 响应字体“颜色”按钮,修改字体颜色
 * 
 * @param uNotifyCode 
 * @param nID 
 * @param wndCtl 
 */
void CFontSelDlg::OnBtn_Color(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CColorDialog colorDlg=m_userConfig.GetFontColor();
	if (colorDlg.DoModal() == IDOK)
	{
		COLORREF clrText = colorDlg.GetColor();
		if (m_fontInfo.m_clrText != clrText)
		{
			m_fontInfo.m_clrText = clrText;
			g_fontInfo.m_clrText = m_fontInfo.m_clrText;
			::PostMessage(GetParent(), WM_UPDATE_FONTINFO, NULL, NULL);
		}
	}
}


/**
 * @brief 初始化字体分类工具栏
 * 
 * @return BOOL 
 */
BOOL CFontSelDlg::InitFontClassToolBar()
{
	int nIndex = m_tbFontClass.AddItem(ID_GROUP_CHAT_DLG_SYS_FONT_BTN, STBI_STYLE_BUTTON|STBI_STYLE_CHECK);
	m_tbFontClass.SetItemSize(nIndex, 20, 20);
	m_tbFontClass.SetItemPadding(nIndex, 0);
	m_tbFontClass.SetItemCheckState(nIndex, TRUE);
	m_tbFontClass.SetItemToolTipText(nIndex, _T("系统字体"));
	m_tbFontClass.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbFontClass.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_sysfont_tab_button.png"));

	m_tbFontClass.SetItemPadding(nIndex, 8);


	nIndex = m_tbFontClass.AddItem(103, STBI_STYLE_SEPARTOR);
	m_tbFontClass.SetItemSize(nIndex, 2, 20);
	m_tbFontClass.SetItemPadding(nIndex, 0);
	m_tbFontClass.SetItemSepartorPic(nIndex, _T("aio_qzonecutline_normal.png"));

	m_tbFontClass.SetTransparent(TRUE, m_SkinDlg.GetBgDC());

	CRect rcFontClass(4, 7, 4+55, 7+32);
	m_tbFontClass.Create(m_hWnd, rcFontClass, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_TOOL_BAR_FONT_CLASS);

	return TRUE;
}


/**
 * @brief 初始化字体名称组合框
 * 
 * @return BOOL 
 */
BOOL CFontSelDlg::InitFontNameComboBox()
{
	m_cboFontName.SubclassWindow(GetDlgItem(ID_COMBO_FONT_NAME));

	m_cboFontName.SetRedraw(FALSE);
	m_cboFontName.m_Edit.EnableWindow(FALSE);

	m_cboFontName.SetArrowWidth(19);
	m_cboFontName.MoveWindow(10, 6, 100, 30, FALSE);
	m_cboFontName.SetItemHeight(-1, 18);

	m_cboFontName.SetTransparent(TRUE, m_SkinDlg.GetBgDC());
	m_cboFontName.SetBgNormalPic(_T("frameBorderEffect_normalDraw.png"), CRect(2,2,2,2));
	m_cboFontName.SetBgHotPic(_T("frameBorderEffect_mouseDownDraw.png"), CRect(2,2,2,2));
	m_cboFontName.SetArrowNormalPic(_T("ComboBox\\inputbtn_normal_bak.png"));
	m_cboFontName.SetArrowHotPic(_T("ComboBox\\inputbtn_highlight.png"));
	m_cboFontName.SetArrowPushedPic(_T("ComboBox\\inputbtn_down.png"));

	std::vector<WString> arrSysFont;
	EnumSysFont(&arrSysFont);

	for (int i = 0; i < (int)arrSysFont.size(); i++)
	{
		m_cboFontName.AddString(arrSysFont[i].c_str());
	}
	arrSysFont.clear();
	
	CString strCustomFontName=m_userConfig.GetFontName();
	if(strCustomFontName.IsEmpty())
	{
		strCustomFontName = _T("微软雅黑");
		m_userConfig.SetFontName(strCustomFontName);
	}
	int nIndex = m_cboFontName.FindString(0, strCustomFontName);

	if(nIndex < 0)
		nIndex = 0;
	m_cboFontName.SetCurSel(nIndex);

	m_cboFontName.SetRedraw(TRUE);

	return TRUE;
}

/**
 * @brief 初始化字体大小组合框
 * 
 * @return BOOL 
 */
BOOL CFontSelDlg::InitFontSizeComboBox()
{
	m_cboFontSize.SubclassWindow(GetDlgItem(ID_COMBO_FONT_SIZE));

	m_cboFontSize.SetRedraw(FALSE);
	m_cboFontSize.m_Edit.EnableWindow(FALSE);
	m_cboFontSize.SetArrowWidth(19);
	m_cboFontSize.MoveWindow(166 - 48, 6, 50, 30, FALSE);
	m_cboFontSize.SetItemHeight(-1, 18);

	m_cboFontSize.SetTransparent(TRUE, m_SkinDlg.GetBgDC());
	m_cboFontSize.SetBgNormalPic(_T("frameBorderEffect_normalDraw.png"), CRect(2,2,2,2));
	m_cboFontSize.SetBgHotPic(_T("frameBorderEffect_mouseDownDraw.png"), CRect(2,2,2,2));
	m_cboFontSize.SetArrowNormalPic(_T("ComboBox\\inputbtn_normal_bak.png"));
	m_cboFontSize.SetArrowHotPic(_T("ComboBox\\inputbtn_highlight.png"));
	m_cboFontSize.SetArrowPushedPic(_T("ComboBox\\inputbtn_down.png"));

	CString strFontSize;
	for (int i = USER_CONTENT_MIN_FONT_SIZE ; i <= USER_CONTENT_MAX_FONT_SIZE ; i += 2)
	{
		strFontSize.Format(_T("%d"), i);
		m_cboFontSize.AddString(strFontSize);
	}
	
	m_fontInfo.m_nSize = m_userConfig.GetFontSize();
	strFontSize.Format(_T("%d"), m_fontInfo.m_nSize);
	int nIndex = m_cboFontSize.FindString(0, strFontSize);
	m_cboFontSize.SetCurSel(nIndex);

	m_cboFontSize.SetRedraw(TRUE);

	return TRUE;
}

/**
 * @brief 初始化字体属性工具栏
 * 
 * @return BOOL 
 */
BOOL CFontSelDlg::InitFontAttrToolBar()
{
	int nIndex = m_tbFontAttr.AddItem(ID_GROUP_CHAT_DLG_BOLD_BTN, STBI_STYLE_BUTTON|STBI_STYLE_CHECK);
	m_tbFontAttr.SetItemSize(nIndex, 30, 24);
	m_tbFontAttr.SetItemPadding(nIndex, 0);
	m_tbFontAttr.SetItemToolTipText(nIndex, _T("加粗"));
	m_tbFontAttr.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbFontAttr.SetItemIconPic(nIndex, _T("MidToolBar\\Bold.png"));

	nIndex = m_tbFontAttr.AddItem(ID_GROUP_CHAT_DLG_ITALIC_BTN, STBI_STYLE_BUTTON|STBI_STYLE_CHECK);
	m_tbFontAttr.SetItemSize(nIndex, 30, 24);
	m_tbFontAttr.SetItemPadding(nIndex, 0);
	m_tbFontAttr.SetItemToolTipText(nIndex, _T("倾斜"));
	m_tbFontAttr.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbFontAttr.SetItemIconPic(nIndex, _T("MidToolBar\\Italic.png"));

	nIndex = m_tbFontAttr.AddItem(ID_GROUP_CHAT_DLG_UNDER_LINE_BTN, STBI_STYLE_BUTTON|STBI_STYLE_CHECK);
	m_tbFontAttr.SetItemSize(nIndex, 30, 24);
	m_tbFontAttr.SetItemPadding(nIndex, 0);
	m_tbFontAttr.SetItemToolTipText(nIndex, _T("下划线"));
	m_tbFontAttr.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbFontAttr.SetItemIconPic(nIndex, _T("MidToolBar\\underline.png"));

	nIndex = m_tbFontAttr.AddItem(ID_GROUP_CHAT_DLG_COLOR_BTN, STBI_STYLE_BUTTON);
	m_tbFontAttr.SetItemSize(nIndex, 30, 24);
	m_tbFontAttr.SetItemPadding(nIndex, 0);
	m_tbFontAttr.SetItemToolTipText(nIndex, _T("颜色"));
	m_tbFontAttr.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbFontAttr.SetItemIconPic(nIndex, _T("MidToolBar\\color.png"));

	m_tbFontAttr.SetTransparent(TRUE, m_SkinDlg.GetBgDC());

	CRect rcFontAttr(217 - 48, 6, 217+104, 6+32);
	m_tbFontAttr.Create(m_hWnd, rcFontAttr, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_TOOL_BAR_FONT_ATTR);

	m_tbFontAttr.SetItemCheckState(0, m_fontInfo.m_bBold);
	m_tbFontAttr.SetItemCheckState(1, m_fontInfo.m_bItalic);
	m_tbFontAttr.SetItemCheckState(2, m_fontInfo.m_bUnderLine);
	return TRUE;
}


/**
 * @brief 初始化
 * 
 * @return BOOL 
 */
BOOL CFontSelDlg::Init()
{
	//Dennis Comment
	const auto& userConfig = CUserConfig::GetInstance();
	{
		g_fontInfo.m_strName = userConfig.GetFontName();
		g_fontInfo.m_bBold = userConfig.IsEnableFontBold();
		g_fontInfo.m_bItalic = userConfig.IsEnableFontItalic();
		g_fontInfo.m_bUnderLine = userConfig.IsEnableFontUnderline();
		g_fontInfo.m_clrText = userConfig.GetFontColor();

		m_fontInfo = g_fontInfo;
	}

	m_SkinDlg.SubclassWindow(m_hWnd);
	m_SkinDlg.SetBgPic(_T("ChatFrame_FontSetup_background.bmp"), CRect(0, 1, 1, 0));
	
	//InitFontClassToolBar();		// 初始化字体分类工具栏
	InitFontNameComboBox();		// 初始化字体名称组合框
	InitFontSizeComboBox();		// 初始化字体大小组合框
	InitFontAttrToolBar();		// 初始化字体属性工具栏

	return TRUE;
}

/**
 * @brief 反初始化
 * 
 */
void CFontSelDlg::UnInit()
{
	if (m_tbFontClass.IsWindow())
	{
		m_tbFontClass.DestroyWindow();
	}

	if (m_tbFontAttr.IsWindow())
	{
		m_tbFontAttr.DestroyWindow();
	}

	if (m_cboFontName.IsWindow())
	{
		m_cboFontName.DestroyWindow();
	}

	if (m_cboFontSize.IsWindow())
	{
		m_cboFontSize.DestroyWindow();
	}
}