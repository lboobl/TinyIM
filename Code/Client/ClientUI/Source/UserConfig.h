/**
 * @file UserConfig.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 用来保存用户设置的类
 * @version 0.1
 * @date 2019-08-04
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef _TINY_IM_USER_CONFIG_H_
#define _TINY_IM_USER_CONFIG_H_
#include <string>
#include "UICommonDef.h"

class CUserConfig
{
private:
	CUserConfig();
	~CUserConfig();
public:
	BOOL LoadConfig(PCTSTR pszFilePath);
	BOOL SaveConfig(PCTSTR pszFilePath);
	static CUserConfig& GetInstance();
	void EnableMute(BOOL bMute);
	void EnableDestroyAfterRead(BOOL bEnable);
	void EnableAutoReply(BOOL bAutoReply);
	void SetAutoReplyContent(PCTSTR pszAutoReplyContent);
	void EnableRevokeChatMsg(BOOL bEnable);

	BOOL IsEnableMute() const;
	BOOL IsEnableDestroyAfterRead() const;
	BOOL IsEnableAutoReply() const;
	PCTSTR  GetAutoReplyContent() const;
	BOOL IsEnableRevokeChatMsg() const;

	void SetMainDlgX(long x);
	void SetMainDlgY(long y);
	void SetMainDlgWidth(long nWidth);
	void SetMainDlgHeight(long nHeight);

	void EnableExitPrompt(BOOL bEnable);				
	BOOL IsEnableExitPrompt();

	void EnableExitWhenCloseMainDlg(BOOL bEnable);
	BOOL IsEnableExitWhenCloseMainDlg();

	void SetBuddyListHeadPicStyle(long nStyle);
	void EnableBuddyListShowBigHeadPicInSel(BOOL bEnable);

	void SetNameStyle(E_UI_NAME_STYLE nStyle);
	E_UI_NAME_STYLE GetNameStyle();

	void EnableSimpleProfile(BOOL bEnable);
	BOOL IsEnableSimpleProfile();

	long GetMainDlgX();
	long GetMainDlgY();
	long GetMainDlgWidth() const;
	long GetMainDlgHeight() const;

	long GetBuddyListHeadPicStyle() const;
	BOOL IsEnableBuddyListShowBigHeadPicInSel() const;

	
	void SetChatDlgWidth(long nWidth);
	void SetChatDlgHeight(long nHeight);

	long GetChatDlgWidth() const;
	long GetChatDlgHeight() const;

	void SetGroupDlgWidth(long nWidth);
	void SetGroupDlgHeight(long nHeight);

	long GetGroupDlgWidth() const;
	long GetGroupDlgHeight() const;
	
	void EnablePressEnterToSend(BOOL bEnable);
	void SetFontName(PCTSTR pszFontName);
	void SetFontSize(long nSize);
	void SetFontColor(long lColor);
	void EnableFontBold(BOOL bBold);
	void EnableFontItalic(BOOL bItalic);
	void EnableFontUnderline(BOOL bUnderline);

	BOOL IsEnablePressEnterToSend() const;
	PCTSTR GetFontName() const;
	long GetFontSize() const;
	long GetFontColor() const;
	BOOL IsEnableFontBold() const;
	BOOL IsEnableFontItalic() const;
	BOOL IsEnableFontUnderline() const;

	void SetFaceID(UINT uFaceID);
	UINT GetFaceID() const;

	void SetCustomFace(PCTSTR pszCustomFace);
	PCTSTR GetCustomFace() const;

	void EnableShowLastMsgInChatDlg(BOOL bEnable);
	BOOL IsEnableShowLastMsgInChatDlg();


private:
	BOOL	m_bMute;				//是否关闭所有声音
	BOOL    m_bDestroyAfterRead;	//是否阅后即焚
	BOOL	m_bAutoReply;			//是否自动回复
	TCHAR	m_szAutoReplyContent[256];	//自动回复内容
	BOOL	m_bEnableRevokeChatMsg; //是否启用消息撤回

	//主对话框
	long	m_xMainDlg; //主对话框横坐标
	long	m_yMainDlg; //主对话框纵坐标
	long	m_cxMainDlg; //主对话框宽度
	long	m_cyMainDlg; //主对话框高度

	BOOL    m_bEnableExitPrompt;					//退出是否提示
	BOOL	m_bExitWhenCloseMainDlg;				//关闭主对话框时退出程序（还是最小化到托盘）

	long	m_nBuddyListHeadPicStyle;				//好友列表中头像风格（大头像、小头像、标准头像）
	BOOL	m_bBuddyListShowBigHeadPicInSel;		//当显示模式是小头像时，选中某个头像是否显示大头像

	E_UI_NAME_STYLE	m_nNameStyleIndex;						//0显示昵称和账号、1显示昵称、2显示账号
	BOOL	m_bShowSimpleProfile;					//是否显示清爽资料
	
	UINT	m_uFaceID;								//当前用户头像ID
	TCHAR   m_szCustomFace[MAX_PATH];               //客户自定义头像的路径


	//聊天对话框
	long	m_cxChatDlg; //好友聊天对话框宽度
	long	m_cyChatDlg; //好友聊天对话框高度

	long	m_cxGroupDlg; //群组聊天对话框宽度
	long	m_cyGroupDlg;//群组聊天对话框高度
	BOOL	m_bPressEnterToSend;//是否按Enter发送
	std::wstring m_strFontName;//字体名称
	long	m_nFontSize;//字体大小
	long	m_lFontColor;//字体颜色
	BOOL	m_bFontBold;//是否是粗体
	BOOL	m_bFontItalic;//是否是斜体
	BOOL	m_bFontUnderline;//是否有下划线

	BOOL	m_bShowLastMsg;//是否显示最后的消息
};
#endif