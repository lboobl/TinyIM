//协议及状态常量
/**
 * @file IUProtocolData.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief TinyIM的UI和Core交互的协议部分
 * @version 0.1
 * @date 2020-01-08
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef _TINY_IM_IU_PROTOCOL_DATA_H_
#define _TINY_IM_IU_PROTOCOL_DATA_H_
#include <stdint.h>
#include <set>
#include <list>
#include <map>
#include <string>
#include <vector>


#include "../msgStruct/CommonDef.h"
#include "UICommonDef.h"
#include "UI_USER_INFO.h"
#include "UICommonStruct.h"
#define DEFAULT_TEAMNAME _T("My Friends")

/**
 * @brief GUI消息的基础类
 * 
 */
class CWndMsgBase
{
public:
	CWndMsgBase();
	virtual ~CWndMsgBase();
public:
	E_UI_NET_DATA_TYPE m_uType;		//数据类型
	int		   m_nRetryTimes;	//当作为发送数据项时重试次数。
};

//class CRegisterRequest : public CWndMsgBase
//{
//public:
//	CRegisterRequest();
//	virtual ~CRegisterRequest();
//
//public:
//	char m_szAccountName[64];
//	char m_szNickName[64];
//	char m_szPassword[64];
//	
//};

/**
 * @brief GUI的注册回复消息(注册结果)
 * 
 */
class C_WND_MSG_RegisterRequest : public CWndMsgBase
{
public:
	C_WND_MSG_RegisterRequest();
	virtual ~C_WND_MSG_RegisterRequest();

public:
	E_UI_REGISTER_RESULT_CODE m_nResultCode;//错误码,标识注册成功或者失败
	char m_szMobile[64];//用户电话号码
	char m_szAccount[64];//用户账号
};

//class CLoginRequest : public CWndMsgBase
//{
//public:
//	CLoginRequest();
//	virtual ~CLoginRequest();
//public:
//	char		m_szAccountName[64];
//	char		m_szPassword[64];
//	long		m_nStatus;
//	LOGIN_TYPE	m_nLoginType;	//登录类型：0表示使用手机号登入，1表示使用账号登入
//};

/**
 * @brief GUI登录结果消息
 * 
 */
class C_WND_MSG_LoginResult : public CWndMsgBase
{
public:
	C_WND_MSG_LoginResult();
	virtual ~C_WND_MSG_LoginResult();
public:
	E_UI_LOGIN_RESULT_CODE m_LoginResultCode;//错误码,标识登录成功或者失败
	UINT m_uAccountID;
	TCHAR m_szAccountName[32];//用户ID
	TCHAR m_szNickName[32];//用户昵称
	TCHAR m_szSignature[256];//用户签名
	long m_nFace;//头像编号
    char m_szCustomFace[64];//用户自定义头像
    int  m_nGender;//用户性别
    int  m_nBirthday;//用户生日
	TCHAR m_szAddress[512];//用户地址
	TCHAR m_szPhoneNumber[64];//用户电话号码
	TCHAR m_szMail[256];//用户的邮箱
	E_UI_ONLINE_STATUS m_nStatus;//用户在线状态
};

//class CUserBasicInfoRequest : public CWndMsgBase
//{
//public:
//	CUserBasicInfoRequest();
//	virtual ~CUserBasicInfoRequest();
//
//public:
//	std::set<UINT> m_setAccountID;
//};

//class CUserBasicInfoResult : public CWndMsgBase
//{
//public:
//	CUserBasicInfoResult();
//	virtual ~CUserBasicInfoResult();
//
//public:
//    //key是分组的名字，value是该组好友的集合
//	std::map<std::string, std::list<UserBasicInfo_ST*>> m_mapUserBasicInfo;
//};

//class CChangeUserStatusRequest : public CWndMsgBase
//{
//public:
//    CChangeUserStatusRequest();
//	virtual  ~CChangeUserStatusRequest();
//
//public:
//    int32_t m_nNewStatus;
//};

//Dennis Begin
//好友聊天消息
class CBuddyChatUiMsg
{
public:
	E_UI_CONTENT_TYPE m_eMsgType;//消息类型
	std::string m_strChatMsgId;//消息ID
	WString      m_strSenderName;//发送者姓名
	WString      m_strTime;//消息时间
	WString      m_strContent;//消息内容
	WString      m_strOtherInfo;//其他内容
	C_UI_FontInfo    m_stFontInfo;//字体信息
};

using CBuddyChatUiMsgVector = std::vector<CBuddyChatUiMsg>;


/**
 * @brief 好友聊天的文本消息
 * 
 */
class C_WND_MSG_BuddyTextMessage :public CWndMsgBase
{
public:
	C_WND_MSG_BuddyTextMessage();
	std::string m_strSender;//发送者
	CBuddyChatUiMsg m_uiMsg;//好友聊天的消息
};

//class CGroupBasicInfoRequest : public CWndMsgBase
//{
//public:
//    CGroupBasicInfoRequest();
//	virtual ~CGroupBasicInfoRequest();
//
//public:
//    int  m_groupid;
//};

//class CGroupBasicInfoResult : public CWndMsgBase
//{
//public:
//    CGroupBasicInfoResult();
//    ~CGroupBasicInfoResult();
//
//public:
//    int                       m_groupid;
//    std::list<UserBasicInfo_ST*> m_listUserBasicInfo;
//};

//class CUserExtendInfoRequest : public CWndMsgBase
//{
//public:
//	CUserExtendInfoRequest();
//	~CUserExtendInfoRequest();
//
//public:
//	std::set<UINT> m_setAccountID;
//};

//class CUserExtendInfoResult : public CWndMsgBase
//{
//public:
//	CUserExtendInfoResult();
//	~CUserExtendInfoResult();
//public:
//	std::list<UserExtendInfo_ST*> m_listUserExtendInfo;
//};

//class CLoginUserFriendsIDRequest : public CWndMsgBase
//{
//public:
//	CLoginUserFriendsIDRequest();
//	~CLoginUserFriendsIDRequest();
//
//public:
//	UINT m_uAccountID;
//};

//class CLoginUserFriendsIDResult : public CWndMsgBase
//{
//public:
//	CLoginUserFriendsIDResult();
//	~CLoginUserFriendsIDResult();
//
//public:
//	UINT				m_uAccountID;
//	std::set<UINT>		m_setFriendIDs;
//	std::set<UINT>		m_setGroupIDs;
//};

//class CFriendStatus : public CWndMsgBase
//{
//public:
//	CFriendStatus();
//	~CFriendStatus();
//
//public:
//	UINT	m_uAccountID;
//	long	m_nStatus;
//    long    m_nClientType;
//    int     m_type;
//};

//class CFindFriendRequest : public CWndMsgBase
//{
//public:
//	CFindFriendRequest();
//	~CFindFriendRequest();
//
//public:
//	char m_szAccountName[64];
//	long m_nType;				//查找类型
//};

/**
 * @brief GUI的查找好友结果消息
 * 
 */
class C_WND_MSG_FindFriendResult : public CWndMsgBase
{
public:
	C_WND_MSG_FindFriendResult();
	~C_WND_MSG_FindFriendResult();

public:
	E_UI_ADD_FRIEND_RESULT m_nResultCode;//好友查找的结果
	char m_szAccountId[16];//用户ID
	char m_szAccountName[64];//用户名称
	char m_szNickName[64];//用户昵称
};

//class COperateFriendRequest : public CWndMsgBase
//{
//public:
//	COperateFriendRequest();
//	~COperateFriendRequest();
//
//public:
//	UINT m_uAccountID;
//	UINT m_uCmd;
//};

/**
 * @brief GUI添加好友的结果消息
 * 
 */
class C_WND_MSG_OperateFriendResult : public CWndMsgBase
{
public:
	C_WND_MSG_OperateFriendResult();
	~C_WND_MSG_OperateFriendResult();

public:
	UINT m_uAccountID;//用户ID
	E_UI_OPERATE_FRIEND m_uCmd;//好友操作的具体类型
	char m_szAccountName[64];//好友名称
	char m_szNickName[64];//好友昵称
	char m_szMsgId[64];//消息ID
};

/**
 * @brief GUI添加好友通知请求(被添加方接收的消息)
 * 
 */
class C_WND_MSG_AddFriendNotifyRequest :public CWndMsgBase
{
public:
	C_WND_MSG_AddFriendNotifyRequest();
	~C_WND_MSG_AddFriendNotifyRequest();
public:
	char m_szMsgId[64];//消息ID
	char m_szUserName[64];      //请求方用户名
	char m_szFriendName[64];	//好友方用户名
	char m_szFriendNickName[64];		//好友方昵称
	E_FRIEND_OPTION m_eOption; //好友操作类型
};


//class CAddTeamInfoRequest : public CWndMsgBase
//{
//public:
//    CAddTeamInfoRequest();
//    ~CAddTeamInfoRequest();
//
//public:
//    int          m_opType;          //操作类型
//    std::wstring m_strNewTeamName;
//    std::wstring m_strOldTeamName;  
//};

//class CAddTeamInfoResult : public CWndMsgBase
//{
//public:
//    CAddTeamInfoResult();
//    ~CAddTeamInfoResult();
//
//public:
//    int          m_opType;          //操作类型  
//};

//class CMoveFriendRequest : public CWndMsgBase
//{
//public:
//    CMoveFriendRequest();
//    ~CMoveFriendRequest();
//
//public:
//    int          m_nFriendID;          //操作类型
//    std::wstring m_strNewTeamName;
//    std::wstring m_strOldTeamName;
//};

//class CSentChatMessage : public CWndMsgBase
//{
//public:
//	CSentChatMessage();
//	~CSentChatMessage();
//
//public:
//	CMsgItem* m_pMsgItem;
//	HWND	  m_hwndChat;
//};


/**
 * @TODO: 暂时没有用到[考虑之后删除]
 * @brief GUI消息[]
 * 
 */
class C_WND_MSG_SentChatConfirmImageMessage : public CWndMsgBase
{
public:
	C_WND_MSG_SentChatConfirmImageMessage();
	~C_WND_MSG_SentChatConfirmImageMessage();

public:
	E_UI_CHAT_CONFIRM_TYPE	m_nType;
	HWND				m_hwndChat;
	char*				m_pszConfirmBody;
	UINT				m_uConfirmBodySize;		//消息体的长度，包括最后末尾结束的\0
	UINT				m_uSenderID;
	std::set<UINT>		m_setTargetIDs;
	//UINT	  m_uTargetID;

};

//class CRecvChatMessage : public CWndMsgBase
//{
//public:
//	CRecvChatMessage();
//	~CRecvChatMessage();
//
//public:
//	UINT  m_uMsgID;			//消息ID
//	UINT  m_uSenderID;		//发送人ID
//	UINT  m_uTargetID;		//接收者ID
//	UINT  m_uMsgBodySize;	//消息正文内容长度
//	char* m_pszMsgBody;		//消息正文内容
//};

//class CModifyPasswordRequest : public CWndMsgBase
//{
//public:
//	CModifyPasswordRequest();
//	~CModifyPasswordRequest();
//
//public:
//	char m_szOldPassword[64];
//	char m_szNewPassword[64];
//};

//class CModifyPasswordResult : public CWndMsgBase
//{
//public:
//	CModifyPasswordResult();
//	~CModifyPasswordResult();
//
//public:
//	long	m_nResultCode;
//};

//class CCreateNewGroupRequest
//class CCreateNewGroupRequest : public CWndMsgBase
//{
//public:
//	CCreateNewGroupRequest();
//	~CCreateNewGroupRequest();
//
//public:
//	char m_szGroupName[64];
//};

//class CCreateGroupResult

/**
 * @brief GUI创建群组的结果的消息
 * 
 */
class C_WND_MSG_CreateNewGroupResult : public CWndMsgBase
{
public:
	C_WND_MSG_CreateNewGroupResult();
	~C_WND_MSG_CreateNewGroupResult();

public:
	UINT	m_uError;//错误码
	char	m_szGroupId[16];//群组ID
	char	m_szGroupName[64];//群组名称
	char	m_szAccount[32];//用户ID
};

//class CModifyFriendMakeNameRequest
//class CModifyFriendMakeNameRequest : public CWndMsgBase
//{
//public:
//    CModifyFriendMakeNameRequest();
//    ~CModifyFriendMakeNameRequest();
//
//public:
//    UINT	m_uFriendID;
//    TCHAR   m_szNewMarkName[64];
//};

//class CModifyFriendMakeNameResult
//class CModifyFriendMakeNameResult : public CWndMsgBase
//{
//public:
//    CModifyFriendMakeNameResult();
//    ~CModifyFriendMakeNameResult();
//
//public:
//    UINT	m_uFriendID;
//    //char	m_szNewMarkName[64];
//};

//class CHeartbeatMessageRequest : public CWndMsgBase
//{
//public:
//	CHeartbeatMessageRequest();
//	~CHeartbeatMessageRequest();
//};

//class CHeartbeatMessageResult : public CWndMsgBase
//{
//public:
//	~CHeartbeatMessageResult();
//	CHeartbeatMessageResult();
//};

//class CUpdateLogonUserInfoRequest
//class CUpdateLogonUserInfoRequest : public CWndMsgBase
//{
//public:
//	CUpdateLogonUserInfoRequest();
//	~CUpdateLogonUserInfoRequest();
//
//public:
//	char	m_szNickName[64];
//	char    m_szSignature[512];
//	UINT	m_uGender;
//	long	m_nBirthday;
//	char    m_szAddress[64];
//	char    m_szPhone[32];
//	char    m_szMail[48];
//	UINT	m_uFaceID;
//	TCHAR   m_szCustomFace[MAX_PATH];
//	BOOL    m_bUseCustomThumb;				//是否使用自定义头像
//};

//class CUpdateLogonUserInfoResult
//class CUpdateLogonUserInfoResult : public CWndMsgBase
//{
//public:
//	CUpdateLogonUserInfoResult();
//	~CUpdateLogonUserInfoResult();
//
//public:
//    char	m_szNickName[64];
//    char    m_szSignature[512];
//    UINT	m_uGender;
//    long	m_nBirthday;
//    char    m_szAddress[64];
//    char    m_szPhone[32];
//    char    m_szMail[48];
//    UINT	m_uFaceID;
//    char    m_szCustomFace[MAX_PATH];
//    BOOL    m_bUseCustomThumb;
//};

//class CTargetInfoChangeResult
//class CTargetInfoChangeResult : public CWndMsgBase
//{
//public:
//	CTargetInfoChangeResult();
//	~CTargetInfoChangeResult();
//
//public:
//	UINT m_uAccountID;
//
//};

//class CScreenshotInfo
//{
//public:
//    CScreenshotInfo();
//    ~CScreenshotInfo();
//
//public:
//    std::string m_strBmpHeader;
//    std::string m_strBmpData;
//    UINT        m_targetId;
//};

/*
文件传输速率消息
*/
class C_WND_MSG_FileProcessMsg :public CWndMsgBase {
public:
	char			m_szUserId[16];
	char            m_szFriendId[16];
	TCHAR			m_szFilePath[MAX_PATH];		//文件全饰路径（用于上传，上传时必填, 下载时作为目标文件路径）
	int             m_nTransPercent;//传输百分比
	double	        m_dTransSpeed;//传输速率
};

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
// 文件传输内容
/////////////////////////////////////////////////////
class C_UI_BuddyMessage;
/**
 * @brief 
 * @TODO 可能用不到
 */
class C_WND_MSG_FileItemRequest : public CWndMsgBase
{
public:
	C_WND_MSG_FileItemRequest();
	~C_WND_MSG_FileItemRequest();

public:
	long			m_nID;
	char			m_szUtfFilePath[MAX_PATH];	//文件utf8路径（用于下载，下载时必填）
	TCHAR			m_szFilePath[MAX_PATH];		//文件全饰路径（用于上传，上传时必填, 下载时作为目标文件路径）
	HWND			m_hwndReflection;			//							 (非必须字段)
	HANDLE			m_hCancelEvent;				// 取消事件					 (非必须字段)
	UINT			m_uSenderID;
	std::set<UINT>	m_setTargetIDs;
	UINT			m_uAccountID;				//账户id，用于下载头像
	E_UI_FILE_ITEM_TYPE m_nFileType;				//目前有聊天图片、离线文件和自定义头像三种类型
	C_UI_BuddyMessage*  m_pBuddyMsg;

	BOOL			m_bPending;					//当该项已经在下载或者上传为FALSE，反之为TRUE
};


/**
 * @brief GUI文件接收请求消息
 * 
 */
class C_WND_MSG_FileRecvReq :public CWndMsgBase
{
public:
	C_WND_MSG_FileRecvReq();
	~C_WND_MSG_FileRecvReq();

public:
	char m_szMsgId[32];//消息ID
	char m_szUserId[16];//用户ID
	char m_szFriendId[16];//好友ID
	char m_szFileName[256];//文件名称
	CLIENT_ONLINE_TYPE m_eOnlineType;//[在线/离线]传输
};

/**
 * @brief GUI发送文件回复
 * 
 */
class C_WND_MSG_FileSendRsp :public CWndMsgBase
{
public:
	C_WND_MSG_FileSendRsp();
	~C_WND_MSG_FileSendRsp();
public:
	ERROR_CODE_TYPE m_eErrCode;//错误类型
	char m_szMsgId[32];//消息ID
	char m_szUserId[16];//用户ID
	char m_szFriendId[16];//好友ID
	char m_szFileName[256];//文件名称
	E_FRIEND_OPTION m_eOption;//好友选择
};

/**
 * @brief GUI发送文件的通知消息[接收方做出选择以后,通知发送方]
 * 
 */
class C_WND_MSG_FileNotifyReq :public CWndMsgBase
{
public:
	C_WND_MSG_FileNotifyReq();
	~C_WND_MSG_FileNotifyReq();
public:
	char m_szMsgId[32];//消息ID
	char m_szUserId[16];//用户ID
	char m_szFriendId[16];//好友ID
	char m_szFileName[256];//文件名称
	CLIENT_ONLINE_TYPE m_eOnlineType;//文件传输在线/离线
	E_FRIEND_OPTION m_eOption;//用户选择
};

/**
 * @brief GUI文件上传结果
 * 
 */
class CUploadFileResult
{
public:
	CUploadFileResult();
	~CUploadFileResult();

public:
	void Clone(const CUploadFileResult* pSource);

public:
	E_UI_FILE_ITEM_TYPE m_nFileType;
	BOOL			m_bSuccessful;					//是否上传成功
	int64_t			m_nFileSize;					//文件大小
	TCHAR			m_szLocalName[MAX_PATH];		//本地文件名
						
	char			m_szMd5[64];					//文件的md5值
	char			m_szRemoteName[MAX_PATH];		//上传成功以后文件在服务器上的路径名

	UINT			m_uSenderID;
	std::set<UINT>	m_setTargetIDs;
	HWND			m_hwndReflection;
};



#endif




