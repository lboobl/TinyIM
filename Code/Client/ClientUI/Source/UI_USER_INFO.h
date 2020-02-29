
/**
 * @file UI_USER_INFO.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 用户信息的头文件
 * @version 0.1
 * @date 2019-10-16
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef _TINY_IM_USER_SESSION_DATA_H_
#define _TINY_IM_USER_SESSION_DATA_H_
//#include "jsoncpp-1.9.0/json.h"
#include <string>
#include <vector>
#include "UICommonDef.h"
#include "UICommonStruct.h"
#include "UICommonType.h"

//TODO 此处需要处理
extern TCHAR g_szHomePath[MAX_PATH];
extern char g_szHomePathAscii[MAX_PATH];
extern const UINT USER_THUMB_COUNT;
extern std::wstring g_strAppTitle;


class C_UI_BuddyInfo			// 好友信息
{
public:
    C_UI_BuddyInfo(void);
    ~C_UI_BuddyInfo(void);

public:
    void Reset();
    void FillFakeData();	//TODO: 填充虚假数据，仅为了测试，实际的数据应该等服务器端完善以后从服务器获取
    void SetBuddyInfo(C_UI_BuddyInfo* lpBuddyInfo);

public:
	UINT m_uUserIndex;
	std::string m_strUserId;		//用户唯一标识
	WString     m_strUserName;      //用户名
    WString		m_strAccount;		//用户账户名
    WString		m_strPassword;		// 密码
    WString		m_strNickName;		// 昵称
    WString		m_strMarkName;		// 备注
    long		m_nTeamIndex;		// 分组索引
    UINT		m_nFace;			// 头像
    E_UI_ONLINE_STATUS		m_nStatus;			// 在线状态
    E_UI_CLIENT_TYPE     m_nClientType;      // 客户端类型
    WString		m_strSign;			// 个性签名
    long		m_nGender;			// 性别
    long		m_nBirthday;		// 生日
    WString		m_strMobile;		// 手机
    WString		m_strAddress;		// 用户地址信息
    WString		m_strEmail;           //用户邮件
    BOOL		m_bUseCustomFace;		//是否使用自定义头像
    BOOL		m_bCustomFaceAvailable;	//自定义头像是否可用
    WString		m_strCustomFace;	    // 自定有头像名（文件名不是路径，本地不存在需要从服务器下载）
    WString		m_strRawCustomFace;		//原始自定义头像内容

	//CString    m_strCustomFaceName;

    UINT		m_uMsgID;			//会话消息ID，初始值为0，只存在于内存中不存档
};


class C_UI_BuddyTeamInfo		// 好友分组信息
{
public:
    C_UI_BuddyTeamInfo(void);
    ~C_UI_BuddyTeamInfo(void);

public:
    void Reset();
    int GetBuddyCount();
    int GetOnlineBuddyCount();
    C_UI_BuddyInfo* GetBuddy(int nIndex);
    void Sort();

public:
    int m_nIndex;			// 索引
    int m_nSort;			// 排列顺序
	std::string				m_strTeamId;
    WString m_strName;		// 分组名称
    std::vector<C_UI_BuddyInfo*> m_arrBuddyInfo;//分组的好友信息
};

class C_UI_OnlineBuddyInfo		// 在线好友信息
{
public:
    C_UI_OnlineBuddyInfo(void);
    ~C_UI_OnlineBuddyInfo(void);

public:
    void Reset();

public:
	std::string m_strUserId;
    UINT m_nUin;
	E_UI_ONLINE_STATUS m_nStatus;	// 在线状态
    int m_nClientType;		// 客户端类型
};

class C_UI_BuddyList	// 好友列表
{
public:
    C_UI_BuddyList(void);
    ~C_UI_BuddyList(void);

public:
    void Reset();//重置
    int GetBuddyTeamCount();//获取好友分组个数
    C_UI_BuddyTeamInfo* GetBuddyTeam(int nTeamIndex);//根据索引获取分组信息
    C_UI_BuddyTeamInfo* GetBuddyTeamByIndex(int nIndex);//根据索引获取分组信息
	bool IsFriend(const std::string strFriendId);
    int GetBuddyTotalCount();//获取总的好友个数
    int GetBuddyCount(int nTeamIndex);//获取某个分组的好友个数
    int GetOnlineBuddyCount(int nTeamIndex);//获取在线的好友个数
    C_UI_BuddyInfo* GetBuddy(int nTeamIndex, int nIndex);//获取具体的好友信息
    //C_UI_BuddyInfo* GetBuddy(UINT nUTalkUin);//根据编号获取好友信息
    void SortBuddyTeam();//好友分组排序
    void SortBuddy();//好友排序
    BOOL AddBuddyTeam(C_UI_BuddyTeamInfo* lpBuddyTeamInfo);//增加一个分组
    BOOL IsTeamNameExist(PCTSTR pszTeamName);//分组名是否存在

public:
    int m_nRetCode;  //返回码
    std::vector<C_UI_BuddyTeamInfo*> m_arrBuddyTeamInfo; //分组信息
};

class C_UI_GroupInfo			// 群信息
{
public:
    C_UI_GroupInfo(void);
    ~C_UI_GroupInfo(void);

public:
    void Reset();
    int GetMemberCount();						// 获取群成员总人数
    int GetOnlineMemberCount();					// 获取群成员在线人数
    C_UI_BuddyInfo* GetMember(int nIndex);			// 根据索引获取群成员信息
    //C_UI_BuddyInfo* GetMemberByUin(UINT nUTalkUin);	// 根据内部ID获取群成员信息
    C_UI_BuddyInfo* GetMemberByAccount(PCTSTR pszAccountName);	// 根据账号获取群成员信息
    void Sort();								// 对群成员列表按在线状态进行排序
    BOOL AddMember(C_UI_BuddyInfo* lpBuddyInfo);	// 添加群成员
    BOOL DelAllMember();						// 删除所有群成员
    BOOL IsHasGroupNumber();
    BOOL IsHasGroupInfo();
    BOOL IsMember(UINT uAccountID);

public:
	std::string m_strGroupId;
    //UINT m_nGroupCode;		// 群账号ID
    //UINT m_nGroupId;		// 群ID
    //UINT m_nGroupNumber;	// 群号码
    WString m_strAccount;	// 群账号
    WString m_strName;		// 群名称
    WString m_strMemo;		// 群公告
    WString m_strFingerMemo;// 群简介
	std::string m_strOwnerId;
    //UINT m_nOwnerUin;		// 群拥有者Uin
    UINT m_nCreateTime;		// 群创建时间
    int m_nFace;			// 群头像
    int m_nLevel;			// 群等级
    int m_nClass;			// 群分类索引
    UINT m_nOption;
    UINT m_nFlag;            //
    BOOL m_bHasGroupNumber;  //是否有群组编号
    BOOL m_bHasGroupInfo;   //是否有群组信息
    std::vector<C_UI_BuddyInfo*> m_arrMember;	// 群成员
};

class CGroupList	// 群列表
{
public:
    CGroupList(void);
    ~CGroupList(void);

public:
    void Reset();
    int GetGroupCount();							// 获取群总数
    C_UI_GroupInfo* GetGroup(int nIndex);				// 获取群信息(根据索引)
    //C_UI_GroupInfo* GetGroupByCode(UINT nGroupCode);	// 获取群信息(根据群代码)
    C_UI_GroupInfo* GetGroupById(const std::string strGroupId);		// 获取群信息(根据群Id)
    //C_UI_BuddyInfo* GetGroupMemberByCode(UINT nGroupCode, UINT nUTalkUin);// 根据群代码和群成员UTalkUin获取群成员信息
    //C_UI_BuddyInfo* GetGroupMemberById(UINT nGroupId, UINT nUTalkUin);	// 根据群Id和群成员UTalkUin获取群成员信息
    BOOL AddGroup(C_UI_GroupInfo* lpGroupInfo);		// 添加群
    //UINT GetGroupCodeById(UINT nGroupId);			// 由群Id获取群代码
    //UINT GetGroupIdByCode(UINT nGroupCode);			// 由群代码获取群Id

public:
    std::vector<C_UI_GroupInfo*> m_arrGroupInfo;
};


class C_UI_RecentInfo	// 最近联系人信息
{
public:
    C_UI_RecentInfo(void);
    ~C_UI_RecentInfo(void);

public:
    void Reset();

public:
	E_UI_CHAT_MSG_TYPE m_nType;		//消息类型（好友聊天信息、群组、讨论组）
    UINT	m_uUserID;
    UINT	m_uFaceID;		//头像类型
    UINT64  m_MsgTime;
    TCHAR   m_szName[MAX_RECENT_NAME_LENGTH];	//好友昵称或群名称
    TCHAR	m_szLastMsgText[MAX_LAST_MSG_TEXT_LENGTH];

};

class C_UI_RecentList			// 最近联系人列表
{
public:
    C_UI_RecentList(void);
    ~C_UI_RecentList(void);

public:
    void Reset();
    BOOL DeleteRecentItem(UINT uAccountID);
    BOOL AddRecent(C_UI_RecentInfo* lpRecentInfo);
    int GetRecentCount();
    C_UI_RecentInfo* GetRecent(int nIndex);

public:
    int m_nRetCode;
    std::vector<C_UI_RecentInfo*> m_arrRecentInfo;
};

class C_UI_FontInfo				// 字体信息
{
public:
    C_UI_FontInfo(void) : m_nSize(9), m_clrText(RGB(0, 0, 0)),/* m_strName(_T("微软雅黑"))*/
        m_bBold(FALSE), m_bItalic(FALSE), m_bUnderLine(FALSE){}
    ~C_UI_FontInfo(void){}
	std::string ColorHexString() const;
	bool ColorHexString(const std::string strColor);
public:
    int m_nSize;			// 字体大小
    COLORREF m_clrText;		// 字体颜色
    WString m_strName;		// 字体名称
    BOOL m_bBold;			// 是否加粗
    BOOL m_bItalic;			// 是否倾斜
    BOOL m_bUnderLine;		// 是否带下划线
};

class C_UI_CustomFaceInfo		// 自定义表情信息
{
public:
    C_UI_CustomFaceInfo(void) : m_nFileId(0), m_dwFileSize(0), m_bOnline(TRUE){}
    ~C_UI_CustomFaceInfo(void){}

public:
    WString m_strName;			// 接收自定义表情使用参数(TODO: 这个字段先保留，不要用，留作以后扩展)
    UINT    m_nFileId;
    WString m_strKey;
    WString m_strServer;

    DWORD   m_dwFileSize;		// 发送自定义表情使用参数
    WString m_strFileName;      //
    WString m_strFilePath;      //
    BOOL	m_bOnline;			//当是文件类型时，该字段代表文件是离线文件还是在线文件
};

class C_UI_Content				// 消息内容
{
public:
    C_UI_Content(void) : m_nType(E_UI_CONTENT_TYPE::CONTENT_TYPE_UNKNOWN), m_nFaceId(0), m_nShakeTimes(0){}
    ~C_UI_Content(void){}

public:
	E_UI_CONTENT_TYPE m_nType;			// 内容类型
    C_UI_FontInfo	m_FontInfo;			// 字体信息
    WString			m_strText;			// 文本信息
    int				m_nFaceId;			// 系统表情Id
    int				m_nShakeTimes;		// 窗口抖动次数
    C_UI_CustomFaceInfo m_CFaceInfo;		// 自定义表情信息（当发送的是m_nType，m_CFaceInfo是文件的有关信息）
};


class C_UI_BuddyMessage
{
public:
    C_UI_BuddyMessage(void);
    ~C_UI_BuddyMessage(void);

public:
    static BOOL IsType(LPCTSTR lpType);
    void		Reset();
    //BOOL		Parse(Json::Value& JsonValue);
    BOOL		IsShakeWindowMsg();

public:
	E_UI_CONTENT_TYPE m_nMsgType;
    UINT					m_nMsgId;
    UINT					m_nMsgId2;
    UINT					m_nFromUin;
    UINT					m_nToUin;
    UINT					m_nReplyIp;
    UINT64					m_nTime;
    HWND					m_hwndFrom;
    WString					m_strNickName;					//后门字段
    std::vector<C_UI_Content*> m_arrContent;
};


class C_UI_GroupMessage
{
public:
    C_UI_GroupMessage(void);
    ~C_UI_GroupMessage(void);

public:
    static BOOL IsType(LPCTSTR lpType);
    void Reset();
    //BOOL Parse(Json::Value& JsonValue);

public:
	E_UI_CONTENT_TYPE m_eType;
	std::string m_strGroupId;
	WString m_strSenderId;
	WString m_strSenderName;
	WString m_strContext;
	WString m_strMsgTime;
	C_UI_FontInfo m_stFontInfo;
};

class C_UI_SessMessage		// 临时消息
{
public:
    C_UI_SessMessage(void);
    ~C_UI_SessMessage(void);

public:
    static BOOL IsType(LPCTSTR lpType);
    void Reset();
    //BOOL Parse(Json::Value& JsonValue);

public:
    UINT m_nMsgId;
    UINT m_nMsgId2;
    UINT m_nFromUin;
    UINT m_nToUin;
    UINT m_nMsgType;
    UINT m_nReplyIp;
    UINT m_nTime;
    UINT m_nGroupId;
    UINT m_nUTalkNum;
    UINT m_nServiceType;
    UINT m_nFlags;
    std::vector<C_UI_Content*> m_arrContent;
};

class C_UI_StatusChangeMessage	// 状态改变通知消息
{
public:
    C_UI_StatusChangeMessage(void);
    ~C_UI_StatusChangeMessage(void);

public:
    static BOOL IsType(LPCTSTR lpType);
    void Reset();
   // BOOL Parse(Json::Value& JsonValue);

public:
    UINT m_nUTalkUin;
	E_UI_ONLINE_STATUS m_nStatus;	// 在线状态
    int m_nClientType;		// 客户端类型
};

class C_UI_KickMessage	// 被踢下线通知消息(例如号码在另一地点登录)
{
public:
    C_UI_KickMessage(void);
    ~C_UI_KickMessage(void);

public:
    static BOOL IsType(LPCTSTR lpType);
    void Reset();
   // BOOL Parse(Json::Value& JsonValue);

public:
    UINT m_nMsgId;
    UINT m_nMsgId2;
    UINT m_nFromUin;
    UINT m_nToUin;
    UINT m_nMsgType;
    UINT m_nReplyIp;
    BOOL m_bShowReason;			// 是否显示被踢下线原因
    WString m_strReason;		// 被踢下线原因
};

class C_UI_SysGroupMessage	// 群系统消息
{
public:
    C_UI_SysGroupMessage(void);
    ~C_UI_SysGroupMessage(void);

public:
    static BOOL IsType(LPCTSTR lpType);
    void Reset();
   // BOOL Parse(Json::Value& JsonValue);

public:
    UINT m_nMsgId;
    UINT m_nMsgId2;
    UINT m_nFromUin;
    UINT m_nToUin;
    UINT m_nMsgType;
    UINT m_nReplyIp;
    WString m_strSubType;
    UINT m_nGroupCode;
    UINT m_nGroupNumber;
    UINT m_nAdminUin;
    WString m_strMsg;
    UINT m_nOpType;
    UINT m_nOldMember;
    WString m_strOldMember;
    WString m_strAdminUin;
    WString m_strAdminNickName;
};

class C_UI_UpdateFriendInfoMsg	//更新好友信息
{
public:
    C_UI_UpdateFriendInfoMsg(void);
    ~C_UI_UpdateFriendInfoMsg(void);

public:
    static BOOL IsType(LPCTSTR lpType);
    void Reset();
    //BOOL Parse(Json::Value& JsonValue);

public:
    UINT m_uMsgType;
    UINT m_uMsgId;
    UINT m_uMsgId2;
    UINT m_uFromUin;
    UINT m_uToUin;
    E_UI_CLIENT_TYPE m_uClientType;
    UINT64 m_uTime;
};

class C_UI_MessageSender	// 消息发送者
{
public:
    C_UI_MessageSender(E_UI_CHAT_MSG_TYPE nType, UINT nSenderId, UINT nGroupCode);
    ~C_UI_MessageSender(void);

public:
    BOOL AddMsg(void* lpMsg);						// 添加消息
    BOOL DelMsg(int nIndex);						// 删除消息(根据索引)
    BOOL DelMsgById(UINT nMsgId);					// 删除消息(根据消息Id)
    BOOL DelAllMsg();								// 删除所有消息

	E_UI_CHAT_MSG_TYPE GetMsgType();					// 获取消息类型
    UINT GetSenderId();								// 获取发送者Id
    int GetMsgCount();								// 获取消息总数
    int GetDisplayMsgCount();						// 除去图片上传完成的确认消息的条数
    UINT GetGroupCode();							// 获取群代码

    C_UI_BuddyMessage* GetBuddyMsg(int nIndex);		// 获取好友消息(根据索引)
    C_UI_BuddyMessage* GetGroupMsg(int nIndex);		// 获取群消息(根据索引)
    C_UI_SessMessage* GetSessMsg(int nIndex);			// 获取临时会话消息(根据索引)

    C_UI_BuddyMessage* GetBuddyMsgById(UINT nMsgId);	// 获取好友消息(根据消息Id)
    C_UI_BuddyMessage* GetGroupMsgById(UINT nMsgId);	// 获取群消息(根据消息Id)
    C_UI_SessMessage* GetSessMsgById(UINT nMsgId);		// 获取临时会话消息(根据消息Id)
    C_UI_SysGroupMessage* GetSysGroupMsgById(UINT nMsgId);// 获取群系统消息(根据消息Id)

private:
	E_UI_CHAT_MSG_TYPE m_nType;
	std::string m_strSenderId;
	std::string m_strGroupId;
    UINT m_nSenderId;
    UINT m_nGroupCode;
    std::vector<void*> m_arrMsg;
};

class C_UI_MessageList	// 消息列表
{
public:
    C_UI_MessageList(void);
    ~C_UI_MessageList(void);

public:
    BOOL AddMsg(E_UI_CHAT_MSG_TYPE nType, UINT nSenderId, UINT nGroupCode, void* lpMsg);		// 添加消息
    BOOL DelMsg(E_UI_CHAT_MSG_TYPE nType, UINT nMsgId);										// 删除消息

    BOOL DelMsgSender(int nIndex);												// 删除消息发送者(根据索引)
    BOOL DelMsgSender(E_UI_CHAT_MSG_TYPE nType, UINT nSenderId);								// 删除消息发送者(根据消息类型和发送者Id)
    BOOL DelAllMsgSender();									// 删除所有消息发送者

    int GetMsgSenderCount();								// 获取消息发送者总数
    C_UI_MessageSender* GetMsgSender(int nIndex);				// 获取消息发送者(根据索引)
    C_UI_MessageSender* GetMsgSender(E_UI_CHAT_MSG_TYPE nType, UINT nSenderId);// 获取消息发送者(根据消息类型和发送者Id)
    C_UI_MessageSender* GetLastMsgSender();					// 获取最后一个发送消息过来的发送者

    C_UI_BuddyMessage* GetBuddyMsg(UINT nMsgId);				// 获取好友消息(根据消息Id)
    C_UI_BuddyMessage* GetGroupMsg(UINT nMsgId);				// 获取群消息(根据消息Id)
    C_UI_SessMessage* GetSessMsg(UINT nMsgId);					// 获取临时会话消息(根据消息Id)

    C_UI_BuddyMessage* GetBuddyMsg(UINT nUTalkUin, UINT nMsgId);	// 获取好友消息(根据发送者Id和消息Id)
    C_UI_BuddyMessage* GetGroupMsg(UINT nGroupCode, UINT nMsgId);	// 获取群消息(根据发送者Id和消息Id)
    C_UI_SessMessage* GetSessMsg(UINT nUTalkUin, UINT nMsgId);	// 获取临时会话消息(根据发送者Id和消息Id)

    struct MSG_IDENTITY
    {
		E_UI_CHAT_MSG_TYPE nType;
        UINT nSenderId;
    };

private:
    void AddMsgIdentity(E_UI_CHAT_MSG_TYPE nType, UINT nSenderId);
    void DelMsgIdentity(E_UI_CHAT_MSG_TYPE nType, UINT nSenderId);

private:
    std::vector<C_UI_MessageSender*> m_arrMsgSender;
    std::vector<MSG_IDENTITY> m_arrMsgIdentity;
};



//Dennis End
#endif