#ifndef _TINY_IM_UI_COMMON_STRUCT_H_
#define _TINY_IM_UI_COMMON_STRUCT_H_
#include "UICommonDef.h"
#include "UICommonType.h"
#include <vector>
const int MAX_PATH_LENGTH = 256;
struct BuddyInfo
{
	int	uAccountID;
    unsigned short	szAccountName[32];
    unsigned short	szMarkName[32];
    long	nTeamIndex;
    long	nGroupCount;		//群个数
    int	GroupID[64];		//群号ID
};

//好友分组信息结构体
struct BuddyTeamInfo
{
    long	nTeamIndex;      //分组索引
    unsigned short	szTeamName[16];  //分组名称
};

#define MAX_LAST_MSG_TEXT_LENGTH 18
#define MAX_RECENT_NAME_LENGTH	 32

struct RecentInfo
{
	E_UI_CHAT_MSG_TYPE	nType;		//消息类型（好友聊天信息、群组、讨论组）
    int	uUserID;
    int	uFaceID;	//头像类型
	long  uMsgTime;
    unsigned short   szName[MAX_RECENT_NAME_LENGTH];
    unsigned short   szLastMsgText[MAX_LAST_MSG_TEXT_LENGTH];
};

//记录当前添加好友的状态信息
struct AddFriendInfo_ST
{
	E_UI_OPERATE_FRIEND nCmd;				//操作类型（Apply/Agree/Refuse）
	int	uAccountID;			//账户ID
	unsigned short	szAccountName[32];	//账户名
	unsigned short	szNickName[32];		//昵称
};



struct UserBasicInfo_ST
{
	int  uAccountID;
	char  szAccountName[32];
	char  szNickName[64];
    char  szMarkName[64];
    char  szSignature[256];
    char  szAddress[51225];
	int  uFaceID;
    int   nStatus;              //在线状态
    int   clientType;           //客户端类型
    char  customFace[64];
    int   nGender;
    int   nBirthday;
    char  szPhoneNumber[64];
    char  szMail[64];
};

struct UserExtendInfo_ST
{
	int	uAccountID;
	int	uGender;
	int	uShengXiao;
	int	uConstel;
	int    uBlood;
	int	uBirthday;

	char	szCountry[32];			// 国家
	char	szProvince[32];			// 省份
	char	szCity[32];				// 城市
	char	szPhone[32];			// 电话
	char	szMobile[32];			// 手机
	char	szOccupation[32];		// 职业
	char	szCollege[32];			// 毕业院校
	char	szEmail[48];			// 邮箱
	char	szHomepage[64];			// 个人主页
	char	szSignature[256];		// 个性签名
	char	szPersonal[512];		// 个人说明	
	char    szCustomFace[MAX_PATH_LENGTH];	// 自定义头像名
};

struct FileProgress
{
	unsigned short szDestPath[MAX_PATH_LENGTH];
	long  nPercent;				//百分比，0～100之间(值为-1时，为获取文件md5值)
	long  nVerificationPercent;	//获取md5值进度（0～100）
};

struct RichEditMsg_st
{
	E_RichEditType m_eType;
	WString m_strContext;
	int m_nFaceId;
	WString m_strImageName;
};

using RichEditMsgList = std::vector<RichEditMsg_st>;

std::string RichEditMsg(const RichEditMsgList& msgList);
RichEditMsgList RichEditMsg(const std::string& strMsg);

#endif