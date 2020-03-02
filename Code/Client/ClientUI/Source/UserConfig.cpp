/**
 * @file UserConfig.cpp
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 用户配置的实现文件
 * @version 0.1
 * @date 2020-03-02
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "stdafx.h"
#include "UserConfig.h"
#include "IniFile.h"


static CUserConfig * g_UserConfig=nullptr;

/**
 * @brief 单例模式,获取实例
 * 
 * @return CUserConfig& 
 */
CUserConfig& CUserConfig::GetInstance()
{
	if (g_UserConfig == nullptr)
	{
		g_UserConfig = new CUserConfig();
	}
	
	return *g_UserConfig;
}
/**
 * @brief Construct a new CUserConfig::CUserConfig object
 * 
 */
CUserConfig::CUserConfig() : m_strFontName(_T("微软雅黑"))
{
	m_bMute = FALSE;
	m_bDestroyAfterRead = FALSE;
	m_bAutoReply = FALSE;
	memset(m_szAutoReplyContent, 0, sizeof(m_szAutoReplyContent));
	m_bEnableRevokeChatMsg = FALSE;

	m_xMainDlg = 0;
	m_yMainDlg = 0;
	m_cxMainDlg = 0;
	m_cyMainDlg = 0;

	m_bEnableExitPrompt = TRUE;
	m_bExitWhenCloseMainDlg = FALSE;

	m_nBuddyListHeadPicStyle = 0;
	m_bBuddyListShowBigHeadPicInSel = TRUE;

	m_nNameStyleIndex = E_UI_NAME_STYLE::NAME_STYLE_SHOW_NICKNAME_AND_ACCOUNT;
	m_bShowSimpleProfile = FALSE;

	m_cxChatDlg = 587;
	m_cyChatDlg = 535;

	m_cxGroupDlg = 670;
	m_cyGroupDlg = 600;
	
	m_bPressEnterToSend = TRUE;
	m_nFontSize = 12;
	m_lFontColor = 0;
	m_bFontBold = FALSE;
	m_bFontItalic = FALSE;
	m_bFontItalic = FALSE;
	m_bFontUnderline = FALSE;

	m_uFaceID = 0;
	memset(m_szCustomFace, 0, sizeof(m_szCustomFace));

	m_bShowLastMsg = TRUE;
}

/**
 * @brief Destroy the CUserConfig::CUserConfig object
 * 
 */
CUserConfig::~CUserConfig()
{
}

/**
 * @brief 从文件中加载配置，加载配置文件
 * 
 * @param pszFilePath 配置文件名称
 * @return BOOL 加载结果，TRUE，成功，FALSE，失败
 */
BOOL CUserConfig::LoadConfig(PCTSTR pszFilePath)
{
	int cxScreenResolution = ::GetSystemMetrics(SM_CXSCREEN);
	const long nMainDlgDefaultX = cxScreenResolution-330;
	const long nMainDlgDefaultY  = 150;
	const long nMainDlgDefaultWidth = 280;
	const long nMainDlgDefaultHeight = 720;

	const long nChatDlgDefaultWidth = 587;
	const long nChatDlgDefaultHeight = 535;

	const long nGroupDlgDefaultWidth = 670;
	const long nGroupDlgDefaultHeight = 600;
	
	CIniFile iniFile;
	//App
	m_bMute = iniFile.ReadInt(_T("App"), _T("Mute"), 0, pszFilePath) ? TRUE : FALSE;
	m_bDestroyAfterRead = iniFile.ReadInt(_T("App"), _T("DestroyAfterRead"), 0, pszFilePath) ? TRUE : FALSE;
	m_bAutoReply = iniFile.ReadInt(_T("App"), _T("AutoReply"), 0, pszFilePath) ? TRUE : FALSE;
	iniFile.ReadString(_T("App"), _T("AutoReplyContent"), _T(""), m_szAutoReplyContent, ARRAYSIZE(m_szAutoReplyContent), pszFilePath);
	m_bEnableRevokeChatMsg = iniFile.ReadInt(_T("App"), _T("RevokeChatMsg"), 0, pszFilePath) ? TRUE : FALSE;
	
	//主对话框
	m_xMainDlg = iniFile.ReadInt(_T("MainDlg"), _T("MainDlgX"), nMainDlgDefaultX, pszFilePath);
	m_yMainDlg = iniFile.ReadInt(_T("MainDlg"), _T("MainDlgY"),  nMainDlgDefaultY, pszFilePath);
	m_cxMainDlg = iniFile.ReadInt(_T("MainDlg"), _T("MainDlgWidth"), nMainDlgDefaultWidth, pszFilePath);
	m_cyMainDlg = iniFile.ReadInt(_T("MainDlg"), _T("MainDlgHeight"), nMainDlgDefaultHeight, pszFilePath);
	m_bEnableExitPrompt = iniFile.ReadInt(_T("MainDlg"), _T("ExitPrompt"), 1, pszFilePath) ? TRUE : FALSE;
	m_bExitWhenCloseMainDlg = iniFile.ReadInt(_T("MainDlg"), _T("ExitWhenClose"), 0, pszFilePath) ? TRUE : FALSE;

	m_nBuddyListHeadPicStyle = iniFile.ReadInt(_T("MainDlg"), _T("BuddyListHeadPicStyle"), 0, pszFilePath);
	m_bBuddyListShowBigHeadPicInSel = iniFile.ReadInt(_T("MainDlg"), _T("ShowBigHeadPicInSel"), 0, pszFilePath) ? TRUE : FALSE;

	m_nNameStyleIndex =static_cast<E_UI_NAME_STYLE>(iniFile.ReadInt(_T("MainDlg"), _T("NameStyleIndex"), 0, pszFilePath));
	m_bShowSimpleProfile = iniFile.ReadInt(_T("MainDlg"), _T("SimpleProfile"), 0, pszFilePath) ? TRUE:FALSE;

	// 聊天对话框
	m_cxChatDlg = iniFile.ReadInt(_T("ChatDlg"), _T("ChatDlgWidth"), nChatDlgDefaultWidth, pszFilePath);
	m_cyChatDlg = iniFile.ReadInt(_T("ChatDlg"), _T("ChatDlgWidth"), nChatDlgDefaultHeight, pszFilePath);
	m_bPressEnterToSend = iniFile.ReadInt(_T("ChatDlg"), _T("EnableEnterToSend"), 1, pszFilePath) ? TRUE : FALSE;
	//iniFile.ReadString(_T("ChatDlg"), _T("FontName"), _T("微软雅黑"), m_strFontName.GetBuffer(32), 32, pszFilePath);
	//m_strFontName.ReleaseBuffer();
	m_nFontSize = iniFile.ReadInt(_T("ChatDlg"), _T("FontSize"), 12, pszFilePath);
	m_lFontColor = iniFile.ReadInt(_T("ChatDlg"), _T("FontColor"), 0,pszFilePath);
	m_bFontBold = iniFile.ReadInt(_T("ChatDlg"), _T("FontBold"), 0, pszFilePath) ? TRUE : FALSE;
	m_bFontItalic = iniFile.ReadInt(_T("ChatDlg"), _T("FontItalic"), 0, pszFilePath) ? TRUE : FALSE;
	m_bFontUnderline = iniFile.ReadInt(_T("ChatDlg"), _T("FontUnderLine"), 0, pszFilePath) ? TRUE : FALSE;
	m_bShowLastMsg = iniFile.ReadInt(_T("ChatDlg"), _T("EnableShowLastMsg"), 1, pszFilePath) ? TRUE : FALSE;

	m_uFaceID = (UINT)iniFile.ReadInt(_T("UserInfo"), _T("FaceID"), 0, pszFilePath);
	iniFile.ReadString(_T("UserInfo"), _T("CustomFace"), _T(""), m_szCustomFace, ARRAYSIZE(m_szCustomFace), pszFilePath);

	m_cxGroupDlg = iniFile.ReadInt(_T("GroupDlg"), _T("GroupDlgWidth"), nGroupDlgDefaultWidth, pszFilePath);
	m_cyGroupDlg = iniFile.ReadInt(_T("GroupDlg"), _T("GroupDlgWidth"), nGroupDlgDefaultHeight, pszFilePath);

	return TRUE;
}




/**
 * @brief 保存配置到文件
 * 
 * @param pszFilePath 要保存的文件名
 * @return BOOL 保存结果，TRUE 成功，FALSE 失败
 */
BOOL CUserConfig::SaveConfig(PCTSTR pszFilePath)
{
	CIniFile iniFile;
	//App
	iniFile.WriteInt(_T("App"), _T("Mute"), m_bMute, pszFilePath);
	iniFile.WriteInt(_T("App"), _T("DestroyAfterRead"), m_bDestroyAfterRead, pszFilePath);
	iniFile.WriteInt(_T("App"), _T("AutoReply"), m_bAutoReply, pszFilePath);
	iniFile.WriteString(_T("App"), _T("AutoReplyContent"), m_szAutoReplyContent, pszFilePath);
	iniFile.WriteInt(_T("App"), _T("RevokeChatMsg"), m_bEnableRevokeChatMsg, pszFilePath);
	
	//主对话框
	iniFile.WriteInt(_T("MainDlg"), _T("MainDlgX"), m_xMainDlg, pszFilePath);
	iniFile.WriteInt(_T("MainDlg"), _T("MainDlgY"),  m_yMainDlg, pszFilePath);
	iniFile.WriteInt(_T("MainDlg"), _T("MainDlgWidth"), m_cxMainDlg, pszFilePath);
	iniFile.WriteInt(_T("MainDlg"), _T("MainDlgHeight"), m_cyMainDlg, pszFilePath);
	iniFile.WriteInt(_T("MainDlg"), _T("ExitPrompt"), m_bEnableExitPrompt, pszFilePath);
	iniFile.WriteInt(_T("MainDlg"), _T("ExitWhenClose"), m_bExitWhenCloseMainDlg, pszFilePath);

	iniFile.WriteInt(_T("MainDlg"), _T("BuddyListHeadPicStyle"), m_nBuddyListHeadPicStyle, pszFilePath);
	iniFile.WriteInt(_T("MainDlg"), _T("ShowBigHeadPicInSel"), m_bBuddyListShowBigHeadPicInSel, pszFilePath);

	iniFile.WriteInt(_T("MainDlg"), _T("NameStyleIndex"), static_cast<long>(m_nNameStyleIndex), pszFilePath);
	iniFile.WriteInt(_T("MainDlg"), _T("SimpleProfile"), m_bShowSimpleProfile, pszFilePath);

	// 聊天对话框
	iniFile.WriteInt(_T("ChatDlg"), _T("ChatDlgWidth"), m_cxChatDlg, pszFilePath);
	iniFile.WriteInt(_T("ChatDlg"), _T("ChatDlgHeight"), m_cyChatDlg, pszFilePath);
	iniFile.WriteInt(_T("ChatDlg"), _T("EnableEnterToSend"), m_bPressEnterToSend, pszFilePath);
	//iniFile.WriteString(_T("ChatDlg"), _T("FontName"), m_strFontName, pszFilePath);
	iniFile.WriteInt(_T("ChatDlg"), _T("FontSize"), m_nFontSize, pszFilePath);
	iniFile.WriteInt(_T("ChatDlg"), _T("FontColor"), m_lFontColor, pszFilePath);
	iniFile.WriteInt(_T("ChatDlg"), _T("FontBold"), m_bFontBold, pszFilePath);
	iniFile.WriteInt(_T("ChatDlg"), _T("FontItalic"), m_bFontItalic, pszFilePath);
	iniFile.WriteInt(_T("ChatDlg"), _T("FontUnderLine"), m_bFontUnderline, pszFilePath);
	iniFile.WriteInt(_T("ChatDlg"), _T("EnableShowLastMsg"), m_bShowLastMsg, pszFilePath);

	iniFile.WriteInt(_T("UserInfo"), _T("FaceID"), m_uFaceID, pszFilePath);
	iniFile.WriteString(_T("UserInfo"), _T("CustomFace"), m_szCustomFace, pszFilePath);

	iniFile.WriteInt(_T("GroupDlg"), _T("GroupDlgWidth"), m_cxGroupDlg, pszFilePath);
	iniFile.WriteInt(_T("GroupDlg"), _T("GroupDlgHeight"), m_cyGroupDlg, pszFilePath);
	
	return TRUE;
}


/**
 * @brief 开启静音,或者关闭静音
 * 
 * @param bMute 是否开启静音
 */
void CUserConfig::EnableMute(BOOL bMute)
{
	m_bMute = bMute;
}

/**
 * @brief 开启或关闭阅后即焚
 * 
 * @param bEnable 是否开启阅后即焚
 */
void CUserConfig::EnableDestroyAfterRead(BOOL bEnable)
{
	m_bDestroyAfterRead = bEnable;
}


/**
 * @brief 是否开启自动回复
 * 
 * @param bAutoReply 是否开启自动回复
 */
void CUserConfig::EnableAutoReply(BOOL bAutoReply)
{
	m_bAutoReply = bAutoReply;
}

/**
 * @brief 设置自动回复的内容
 * 
 * @param pszAutoReplyContent 
 */
void CUserConfig::SetAutoReplyContent(PCTSTR pszAutoReplyContent)
{
	_tcscpy_s(m_szAutoReplyContent, ARRAYSIZE(m_szAutoReplyContent), pszAutoReplyContent);
}

/**
 * @brief 是否允许撤回消息
 * 
 * @param bEnable 
 */
void CUserConfig::EnableRevokeChatMsg(BOOL bEnable)
{
	m_bEnableRevokeChatMsg = bEnable;
}

/**
 * @brief 是静音
 * 
 * @return BOOL TRUE,静音,FALSE 不静音
 */
BOOL CUserConfig::IsEnableMute() const
{
	return m_bMute;
}

/**
 * @brief 允许阅后即焚
 * 
 * @return BOOL 
 */
BOOL CUserConfig::IsEnableDestroyAfterRead() const
{
	return m_bDestroyAfterRead;
}
	
/**
 * @brief 允许自动回复
 * 
 * @return BOOL 
 */
BOOL CUserConfig::IsEnableAutoReply() const
{
	return m_bAutoReply;
}

/**
 * @brief 获取自动回复的内容
 * 
 * @return PCTSTR 
 */
PCTSTR  CUserConfig::GetAutoReplyContent() const
{
	return m_szAutoReplyContent;
}

/**
 * @brief 允许撤回消息
 * 
 * @return BOOL 
 */
BOOL CUserConfig::IsEnableRevokeChatMsg() const
{
	return m_bEnableRevokeChatMsg;
}

/**
 * @brief 设置主对话框的X坐标
 * 
 * @param x 主对话框X坐标
 */
void CUserConfig::SetMainDlgX(long x)
{
	m_xMainDlg = x;
}

/**
 * @brief 设置主对话框Y坐标
 * 
 * @param y 主对话框Y坐标
 */
void CUserConfig::SetMainDlgY(long y)
{
	m_yMainDlg = y;
}

/**
 * @brief 设置主对话框宽度
 * 
 * @param nWidth 
 */
void CUserConfig::SetMainDlgWidth(long nWidth)
{
	m_cxMainDlg = nWidth;
}

/**
 * @brief 设置主对话框高度
 * 
 * @param nHeight 
 */
void CUserConfig::SetMainDlgHeight(long nHeight)
{
	m_cyMainDlg = nHeight;
}

/**
 * @brief 设置立即退出
 * 
 * @param bEnable 
 */
void CUserConfig::EnableExitPrompt(BOOL bEnable)
{
	m_bEnableExitPrompt = bEnable;
}

/**
 * @brief 是否立即退出
 * 
 * @return BOOL 
 */
BOOL CUserConfig::IsEnableExitPrompt()
{
	return m_bEnableExitPrompt;
}

/**
 * @brief 设置关闭主对话框时退出
 * 
 * @param bEnable 
 */
void CUserConfig::EnableExitWhenCloseMainDlg(BOOL bEnable)
{
	m_bExitWhenCloseMainDlg = bEnable;
}

/**
 * @brief 关闭主对话框时退出
 * 
 * @return BOOL 
 */
BOOL CUserConfig::IsEnableExitWhenCloseMainDlg()
{
	return m_bExitWhenCloseMainDlg;
}

/**
 * @brief 设置好友列表的头像风格
 * 
 * @param nStyle 
 */
void CUserConfig::SetBuddyListHeadPicStyle(long nStyle)
{
	m_nBuddyListHeadPicStyle = nStyle;
}

/**
 * @brief 允许好友列表显示大头像
 * 
 * @param bEnable 
 */
void CUserConfig::EnableBuddyListShowBigHeadPicInSel(BOOL bEnable)
{
	m_bBuddyListShowBigHeadPicInSel = bEnable;
}

/**
 * @brief 设置名称风格
 * 
 * @param nStyle 
 */
void CUserConfig::SetNameStyle(E_UI_NAME_STYLE nStyle)
{
	m_nNameStyleIndex = nStyle;
}

/**
 * @brief 获取名称风格
 * 
 * @return long 
 */
E_UI_NAME_STYLE CUserConfig::GetNameStyle()
{
	return m_nNameStyleIndex;
}

/**
 * @brief 设置简单资料
 * 
 * @param bEnable 
 */
void CUserConfig::EnableSimpleProfile(BOOL bEnable)
{
	m_bShowSimpleProfile = bEnable;
}

/**
 * @brief 是否简单资料
 * 
 * @return BOOL 
 */
BOOL CUserConfig::IsEnableSimpleProfile()
{
	return m_bShowSimpleProfile;
}

/**
 * @brief 获取主对话框的X坐标
 * 
 * @return long 
 */
long CUserConfig::GetMainDlgX()
{
	long cxScreen = ::GetSystemMetrics(SM_CXSCREEN);
	if(m_xMainDlg>cxScreen || m_xMainDlg<0)
		m_xMainDlg = cxScreen-300;
	
	return m_xMainDlg;
}

/**
 * @brief 获取主对话框的Y坐标
 * 
 * @return long 
 */
long CUserConfig::GetMainDlgY()
{
	long cyScreen = ::GetSystemMetrics(SM_CYSCREEN);
	if(m_yMainDlg>cyScreen || m_yMainDlg<0)
		m_yMainDlg = cyScreen-30;
	
	return m_yMainDlg;
}

/**
 * @brief 获取主对话框的宽度
 * 
 * @return long 
 */
long CUserConfig::GetMainDlgWidth() const
{
	return m_cxMainDlg;
}

/**
 * @brief 获取主对话框的高度
 * 
 * @return long 
 */
long CUserConfig::GetMainDlgHeight() const
{
	return m_cyMainDlg;
}

/**
 * @brief 获取好友列表的头像
 * 
 * @return long 
 */
long CUserConfig::GetBuddyListHeadPicStyle() const
{
	return m_nBuddyListHeadPicStyle;
}

/**
 * @brief 是否允许在好友列表显示头像
 * 
 * @return BOOL 
 */
BOOL CUserConfig::IsEnableBuddyListShowBigHeadPicInSel() const
{
	return m_bBuddyListShowBigHeadPicInSel;
}

/**
 * @brief 设置聊天窗口的宽度
 * 
 * @param nWidth 
 */
void CUserConfig::SetChatDlgWidth(long nWidth)
{
	m_cxChatDlg = nWidth;
}

/**
 * @brief 设置聊天窗口的高度
 * 
 * @param nHeight 
 */
void CUserConfig::SetChatDlgHeight(long nHeight)
{
	m_cyChatDlg = nHeight;
}

/**
 * @brief 获取聊天窗口的宽度
 * 
 * @return long 
 */
long CUserConfig::GetChatDlgWidth() const
{
	return m_cxChatDlg;
}


/**
 * @brief 获取聊天窗口的高度
 * 
 * @return long 
 */
long CUserConfig::GetChatDlgHeight() const
{
	return m_cyChatDlg;
}

/**
 * @brief 设置群聊天窗口宽度
 * 
 * @param nWidth 
 */
void CUserConfig::SetGroupDlgWidth(long nWidth)
{
	m_cxGroupDlg = nWidth;
}

/**
 * @brief 设置群聊天窗口的高度
 * 
 * @param nHeight 
 */
void CUserConfig::SetGroupDlgHeight(long nHeight)
{
	m_cyGroupDlg = nHeight;
}


/**
 * @brief 获取群聊天窗口的宽度
 * 
 * @return long 
 */
long CUserConfig::GetGroupDlgWidth() const
{
	return m_cxGroupDlg;
}

/**
 * @brief 获取群聊天窗口的高度
 * 
 * @return long 
 */
long CUserConfig::GetGroupDlgHeight() const
{
	return m_cyGroupDlg;
}

/**
 * @brief 设置按下回车键发送
 * 
 * @param bEnable 
 */
void CUserConfig::EnablePressEnterToSend(BOOL bEnable)
{
	m_bPressEnterToSend = bEnable;
}

/**
 * @brief 设置字体名称
 * 
 * @param pszFontName 
 */
void CUserConfig::SetFontName(PCTSTR pszFontName)
{
	m_strFontName = pszFontName;
}

/**
 * @brief 设置字体大小
 * 
 * @param nSize 
 */
void CUserConfig::SetFontSize(long nSize)
{
	m_nFontSize = nSize;
}

/**
 * @brief 设置字体颜色
 * 
 * @param lColor 
 */
void CUserConfig::SetFontColor(long lColor)
{
	m_lFontColor = lColor;
}

/**
 * @brief 设置字体粗体
 * 
 * @param bBold 
 */
void CUserConfig::EnableFontBold(BOOL bBold)
{
	m_bFontBold = bBold;
}

/**
 * @brief 设置字体斜体
 * 
 * @param bItalic 
 */
void CUserConfig::EnableFontItalic(BOOL bItalic)
{
	m_bFontItalic = bItalic;
}

/**
 * @brief 设置字体下划线
 * 
 * @param bUnderline 
 */
void CUserConfig::EnableFontUnderline(BOOL bUnderline)
{
	m_bFontUnderline = bUnderline;
}

/**
 * @brief 允许按下回车键发送
 * 
 * @return BOOL 
 */
BOOL CUserConfig::IsEnablePressEnterToSend() const
{
	return m_bPressEnterToSend;
}

/**
 * @brief 获取字体名称
 * 
 * @return PCTSTR 
 */
PCTSTR CUserConfig::GetFontName() const
{
	return m_strFontName.c_str();
}

/**
 * @brief 获取字体大小
 * 
 * @return long 
 */
long CUserConfig::GetFontSize() const
{
	return m_nFontSize;
}

/**
 * @brief 获取字体颜色
 * 
 * @return long 
 */
long CUserConfig::GetFontColor() const
{
	return m_lFontColor;
}

/**
 * @brief 允许字体粗体
 * 
 * @return BOOL 
 */
BOOL CUserConfig::IsEnableFontBold() const
{
	return m_bFontBold;
}

/**
 * @brief 允许字体斜体
 * 
 * @return BOOL 
 */
BOOL CUserConfig::IsEnableFontItalic() const
{
	return m_bFontItalic;
}

/**
 * @brief 允许字体下划线
 * 
 * @return BOOL 
 */
BOOL CUserConfig::IsEnableFontUnderline() const
{
	return m_bFontUnderline;
}

/**
 * @brief 设置头像ID
 * 
 * @param uFaceID 
 */
void CUserConfig::SetFaceID(UINT uFaceID)
{
	m_uFaceID = uFaceID;
}

/**
 * @brief 获取头像ID
 * 
 * @return UINT 
 */
UINT CUserConfig::GetFaceID() const
{
	return m_uFaceID;
}

/**
 * @brief 设置自定义头像
 * 
 * @param pszCustomFace 
 */
void CUserConfig::SetCustomFace(PCTSTR pszCustomFace)
{
	_tcscpy_s(m_szCustomFace, ARRAYSIZE(m_szCustomFace), pszCustomFace);
}

/**
 * @brief 获取自定义头像的路径
 * 
 * @return PCTSTR 
 */
PCTSTR CUserConfig::GetCustomFace() const
{
	return m_szCustomFace;
}

/**
 * @brief 设置显示最近的聊天消息
 * 
 * @param bEnable 
 */
void CUserConfig::EnableShowLastMsgInChatDlg(BOOL bEnable)
{
	m_bShowLastMsg = bEnable;
}

/**
 * @brief 允许显示最近的聊天消息
 * 
 * @return BOOL 
 */
BOOL CUserConfig::IsEnableShowLastMsgInChatDlg()
{
	return m_bShowLastMsg;
}


