/**
 * @file CommonMsg.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 定义交互消息的头文件，包括了序列化和反序列化
 * @version 0.1
 * @date 2019-07-18
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#ifndef _COMMON_MSG_H_
#define _COMMON_MSG_H_
#include "json11.hpp"
#include "CommonDef.h"
#include <stdlib.h>
#include <string.h>

 //用户基本信息
struct UserBaseInfo {
	std::string m_strUserId;    //用户编号
	std::string m_strUserName;  //用户名称
	std::string m_strSignature; //用户签名
	std::string m_strNickName;  //用户备注
	std::string m_strFaceId;    //用户头像
	std::string m_strAddress;   //用户住址
	std::string m_strBirthDate; //用户出生日期
	std::string m_strEmail;     //用户邮箱
	std::string m_strGender;    //用户性别
	CLIENT_ONLINE_TYPE m_eOnlineState;//用户在线状态
};

//好友分组的基本信息
struct TeamBaseInfo {
	std::string m_strTeamId;	//好友分组ID
	std::string m_strTeamName;	//好友分组名称
	std::vector<UserBaseInfo> m_teamUsers;//该分组中的所有好友
};

/**
 * @brief 群组信息结构体
 * 
 */
struct GroupInfo {
	std::string m_strGroupId;//群组ID
	std::string m_strGroupName;//群昵称
	std::string m_strGroupImage;//群头像
	std::vector<UserBaseInfo> m_GroupUsers;//群组成员
};
const int E_FONT_BOLD = 1;
const int E_FONT_ITALIC = 2;
const int E_FONT_UNDER_SCORE = 4;
struct FontInfo_s
{
public:
	std::string m_strFontName; //字体名称
	int			m_nFontSize;   //字体大小
	std::string m_strFontColorHex;  //字体颜色
	int         m_nFontStyle;//字体风格，粗体、斜体、下划线
	explicit FontInfo_s();
	bool IsBold() const;//是否粗体
	bool IsItalic() const;//是否斜体
	bool IsUnderScore() const;//是否是下划线
	void SetBold();//设置粗体
	void SetItalic();//设置斜体
	void SetUnderScore();//设置下划线
	bool FromString(const std::string& strJson);//从json构造
	std::string ToString() const;//转为json

public:

};

//好友聊天消息
struct FriendChatMsg_s {
	std::string m_strChatMsgId; //消息ID，由服务器生成
	std::string m_strSenderId; //发送方ID
	std::string m_strReceiverId;//接收方ID
	std::string m_strContext;  //信息内容
	FontInfo_s  m_fontInfo;  //字体信息
	std::string m_strMsgTime;  //消息接收时间
};

//群组聊天消息
struct GroupChatMsg_s {
	std::string m_strChatMsgId; //消息ID，由服务器生成
	std::string m_strSenderId; //发送方ID
	std::string m_strGroupId;//接收方ID
	std::string m_strContext;  //信息内容
	FontInfo_s  m_fontInfo;  //字体信息
	std::string m_strMsgTime;  //消息接收时间
};

//聊天信息的基本单元
struct ChatMsgElem
{
	CHAT_MSG_TYPE m_eType;
	std::string m_strContext;
	int m_nFaceId;
	std::string m_strImageName;
};

using ChatMsgElemVec = std::vector<ChatMsgElem>;

std::string MsgElemVec(const ChatMsgElemVec& vec);
ChatMsgElemVec MsgElemVec(const std::string strVec);

/**
 * @brief 消息体的头部
 * 
 */
struct Header
{
	//消息类型
	int32_t   m_type;//消息类型
	//消息长度
	int32_t    m_length;//消息长度
};



/**
 * @brief 用来传输消息的类型，将消息先转化为字符串，然后放入这个类型中，在实际的代码中使用std::shared_ptr来保存
 * 
 */
struct TransBaseMsg_t
{
public:
	/**
	 * @brief 获取消息类型
	 * 
	 * @return E_MsgType 
	 */
	E_MsgType GetType() const;

	/**
	 * @brief 获取消息体的长度
	 * 
	 * @return std::size_t 
	 */
	std::size_t GetSize() const;

	/**
	 * @brief 获取消息的字符串内容
	 * 
	 * @return std::string 
	 */
	std::string to_string() const;

	/**
	 * @brief 获取消息的实际数据
	 * 
	 * @return const char* 
	 */
	const char * GetData() const;

	/**
	 * @brief 使用消息类型和字符串构造传输的消息,发送消息的时候使用
	 *
	 * @param type 消息类型
	 * @param strMsg  消息字符串
	 */
	explicit TransBaseMsg_t(const E_MsgType& type, const std::string& strMsg);
	/**
	 * @brief 从内存直接构造消息，接收消息的时候使用
	 * 
	 * @param data 消息接收端的内存
	 */
	explicit TransBaseMsg_t(const char * data);

	virtual ~TransBaseMsg_t();
protected:
	//数据块内容
	char *   m_data;
	//是否自己保存了数据
	bool m_selfData = false;
};

using TransBaseMsg_S_PTR=std::shared_ptr<TransBaseMsg_t>;
/**
 * @brief Ip端口配置类
 * 
 */
struct IpPortCfg
{
	std::string m_strServerIp;
	int 		m_nPort;
	/**
	 * @brief 获取配置的字符串表示，类似于 127.0.0.1:80
	 * 
	 * @return std::string 
	 */
	std::string to_string();

	/**
	 * @brief 检查配置是否有效
	 * 
	 * @return true 配置有效
	 * @return false 配置无效
	 */
	bool Valid();
};

struct MySqlCfg
{
public:
	std::string m_strDbIp;//数据库IP地址
	int         m_nDbPort;//数据库端口
	std::string m_strUserName;//数据库用户名
	std::string m_strPassword;//数据库的密码
	std::string m_strDbName;//数据库名称
};
/**
 * @brief 基础消息类型，所有的消息从该类型继承
 * 
 */
struct BaseMsg
{
protected:
	E_MsgType m_type;
public:
	/**
	 * @brief 获取消息类型
	 * 
	 * @return E_MsgType 
	 */
	E_MsgType GetMsgType() const 
	{
		return m_type;
	}

	explicit BaseMsg()
	{
		m_type=E_MsgType::Base_Type;
	}
	
	/**
	 * @brief 消息序列化,需要子类继承
	 * 
	 * @return std::string 序列化以后的字符串
	 */
	virtual std::string ToString() const
	{
		return "";
	}

	/**
	 * @brief 消息反序列化，需要子类继承
	 * 
	 * @param strJson 待反序列化的子串
	 * @return true 反序列化成功
	 * @return false 反序列化失败
	 */
    virtual bool FromString(const std::string& strJson)
	{
		if(strJson.empty())
		{
			return false;
		}
		return true;
	}

	virtual bool Valid() const {
		return false;
	}

	virtual std::string ToPrintString() const {
		return ToString();
	}
};


/**
 * @brief 网络连接断开上报消息
 * 
 */
struct NetFailedReportMsg final :public BaseMsg
{
public:
	std::string m_strClientId;//客户端标识，用来标识是哪个客户端的发送的心跳请求
public:
	explicit NetFailedReportMsg();

	explicit NetFailedReportMsg(const std::string& clientId);
	
	virtual bool FromString(const std::string& strJson) override;

	virtual std::string ToString() const override;
};


/**
 * @brief 网络连接恢复上报消息
 * 
 */
struct NetRecoverReportMsg final :public BaseMsg
{
public:
	std::string m_strClientId;//客户端标识，用来标识是哪个客户端的发送的心跳请求
public:
	explicit NetRecoverReportMsg();

	explicit NetRecoverReportMsg(const std::string& clientId);

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;
};

/**
 * @brief 心跳请求消息
 * 
 */
struct KeepAliveReqMsg final:public BaseMsg
{
public:
	std::string m_strClientId;//客户端标识，用来标识是哪个客户端的发送的心跳请求
public:
	explicit KeepAliveReqMsg();

	explicit KeepAliveReqMsg(const std::string& clientId);

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};

/**
 * @brief 心跳回复消息
 * 
 */
class KeepAliveRspMsg final:public BaseMsg
{
public:
	std::string m_strClientId;//心跳请求中携带的客户端标识
public:
	explicit KeepAliveRspMsg();

	explicit KeepAliveRspMsg(const std::string& clientId);

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};


/**
 * @brief 登录请求消息
 *
 */
class UserLoginReqMsg final :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserName;//用户名
	std::string m_strPassword;//密码
	CLIENT_OS_TYPE m_eOsType;//操作系统类型
	CLIENT_NET_TYPE m_eNetType;//网络类型
	CLIENT_ONLINE_TYPE m_eOnlineType;//在线类型
public:
	explicit UserLoginReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
};


/**
 * @brief 用户登录回复消息
 *
 */
class UserLoginRspMsg final :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_eErrCode; //错误代码
	std::string m_strErrMsg; //错误信息
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strUserName; //用户名
	UserBaseInfo m_userInfo;//用户的基本信息
	
public:
	explicit UserLoginRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
};


/**
 * @brief 用户退出登录消息
 *
 */
class UserLogoutReqMsg final :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserName; //用户名
	std::string m_strPassword; //密码
	CLIENT_OS_TYPE m_eOsType;  //操作系统平台类型
public:
	explicit UserLogoutReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
	
	virtual bool Valid() const override;
};


/**
 * @brief 用户退出登录回复消息
 * 
 */
class UserLogoutRspMsg final :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_eErrCode; //错误代码
	std::string m_strErrMsg;    //错误消息
	std::string m_strMsgId;     //消息ID
	std::string m_strUserName;  //用户名
public:
	UserLogoutRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
	
};


/**
 * @brief 用户注册请求消息
 * 
 */
class UserRegisterReqMsg final :public BaseMsg
{
public:
	std::string m_strMsgId;//请求消息ID，在同样的消息回复的时候，会带回来
	std::string m_strUserName;//用户名
	std::string m_strPassword;//密码
	std::string m_strNickName;//昵称

public:
	UserRegisterReqMsg();

	std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
	
};


/**
 * @brief 用户注册回复消息
 * 
 */
class UserRegisterRspMsg final :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_eErrCode; //错误代码
	std::string m_strErrMsg;   //错误信息
	std::string m_strMsgId;//消息ID
	std::string m_strUserName; //用户名
public:
	UserRegisterRspMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
	
};


/**
 * @brief 用户注销请求消息
 * 
 */
class UserUnRegisterReqMsg final :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserName;//用户名
	std::string m_strPassword;//密码
	std::string m_strVerifyCode;//验证码，暂不使用

public:
	UserUnRegisterReqMsg();

	std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;

};


/**
 * @brief 用户注销回复消息
 * 
 */
class UserUnRegisterRspMsg final :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_eErrCode; //错误代码
	std::string m_strErrMsg;    //错误信息
	std::string m_strMsgId;     //消息ID
	std::string m_strUserName;  //用户名

public:
	UserUnRegisterRspMsg();
	
	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
	
};


/**
 * @brief 好友聊天发送文字消息，发送方到服务器
 * 
 */
class FriendChatSendTxtReqMsg final :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strSenderId;   //发送者
	std::string m_strReceiverId; //接收者
	std::string m_strContext;  //信息内容
	FontInfo_s  m_fontInfo;//字体信息
public:
	FriendChatSendTxtReqMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
	
};

/**
 * @brief 好友聊天发送文字消息的回复消息，服务器到发送方
 * 
 */
class FriendChatSendTxtRspMsg final :public BaseMsg {
public:
	ERROR_CODE_TYPE m_eErrCode;//错误代码
	std::string m_strErrMsg;  //错误信息
	std::string m_strMsgId; //消息ID，由服务器生成
	FriendChatMsg_s m_chatMsg;
public:
	FriendChatSendTxtRspMsg();
	
	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
};




/**
 * @brief 好友聊天接收文字消息，服务器到接收方
 * 
 */
class FriendChatRecvTxtReqMsg final :public BaseMsg {

public:
	std::string m_strMsgId;    //消息ID
	FriendChatMsg_s m_chatMsg;
public:
	FriendChatRecvTxtReqMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
	
};


/**
 * @brief 好友聊天接收文字消息的回复消息，接收方到服务器
 * 
 */
class FriendChatRecvTxtRspMsg final :public BaseMsg {
public:
	std::string m_strMsgId;    //消息ID
	std::string m_strUserId;   //发送方
	std::string m_strFriendId; //接收方
	std::string m_strChatMsgId; //
public:
	FriendChatRecvTxtRspMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
	
};



/**
获取好友列表请求
*/
class GetFriendListReqMsg :public BaseMsg {
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
public:
	GetFriendListReqMsg();
	
	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
};

/**
 * @brief 获取好友列表回复
 * 
 */
class GetFriendListRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_errCode;//错误代码
	std::string m_strErrMsg;//错误消息
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::vector<TeamBaseInfo> m_teamVec;//分组数据
public:
	GetFriendListRspMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override; 
};



/**
 * @brief 添加好友请求
 * 
 */
class AddFriendSendReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strFriendId;//被添加的用户ID
public:
	AddFriendSendReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
};


/**
 * @brief 添加好友回复
 * 
 */
class AddFriendSendRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_eErrCode; //错误码
	std::string m_strErrMsg;	//错误信息
	std::string m_strMsgId;		//消息ID
	std::string m_strUserId;     //用户ID
	std::string m_strFriendId;   //好友ID

public:
	AddFriendSendRspMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;

};



/**
 * @brief 添加好友朋友接收方请求消息
 * 
 */
class AddFriendRecvReqMsg :public BaseMsg {
public:
	std::string m_strMsgId; //消息ID
	std::string m_strUserId;//用户ID
	std::string m_strFriendId;//好友ID
public:
	AddFriendRecvReqMsg();
	
	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;


	virtual bool Valid() const override;
	
};

/**
 * @brief 被添加好友回复消息
 * 
 */
class AddFriendRecvRspMsg :public BaseMsg {
public:
	ERROR_CODE_TYPE m_errCode;//错误码
	std::string m_errMsg;//错误代码
	std::string m_strMsgId; //消息ID
	std::string m_strUserId; //用户ID
	std::string m_strFriendId; //好友ID
	E_FRIEND_OPTION m_option;//用户选择[同意/拒绝]
public:
	AddFriendRecvRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;

};


/**
 * @brief 通知添加好友的结果通知请求
 * 
 */
class AddFriendNotifyReqMsg :public BaseMsg {
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strFriendId;//好友ID
	
	E_FRIEND_OPTION m_option;//用户选择[同意/拒绝]
public:
	AddFriendNotifyReqMsg();
	
	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
	
};

/**
 * @brief 添加好友通知回复
 * 
 */
class AddFriendNotifyRspMsg :public BaseMsg {
public:
	std::string m_strMsgId;//消息ID
public:
	AddFriendNotifyRspMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
};


/**
 * @brief 删除好友请求
 * 
 */
class RemoveFriendReqMsg :public BaseMsg {
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId; //用户ID
	std::string m_strFriendId; //好友ID
public:
	RemoveFriendReqMsg();
	
	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
};

/**
 * @brief 删除好友回复
 * 
 */
class RemoveFriendRspMsg :public BaseMsg {
public:
	ERROR_CODE_TYPE m_eErrCode; //错误码
	std::string m_strErrMsg; //错误信息
	std::string m_strMsgId;  //消息ID
	std::string m_strUserId; //用户名
	std::string m_strFriendId; //好友用户名
public:
	RemoveFriendRspMsg();
	
	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
	
};


/**
 * @brief 查找好友请求
 * 
 */
class FindFriendReqMsg :public BaseMsg {
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strWantedName;//被查找的用户名
public:
	FindFriendReqMsg();

	virtual std::string ToString() const override;


	virtual bool FromString(const std::string& strJson) override;


	virtual bool Valid() const override;
};


/**
 * @brief 查找好友回复
 * 
 */
class FindFriendRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_eErrCode;//错误码
	std::string m_errMsg;//错误消息
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::vector<UserBaseInfo> m_friendInfoVec;//符合的所有用户
public:
	FindFriendRspMsg();
	
	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

};

/* 添加好友分组请求*/
class AddTeamReqMsg :public BaseMsg {
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId; //用户名
	std::string m_strTeamName; //群组名称
public:
	AddTeamReqMsg();
	
	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

};


/* 添加好友分组回复*/
class AddTeamRspMsg :public BaseMsg {
public:
	ERROR_CODE_TYPE m_eErrCode; //错误码
	std::string m_strErrMsg; //错误信息
	std::string m_strMsgId;//消息ID

	std::string m_strUserId;//用户名
	std::string m_strTeamId; //分组编号

	std::string m_strTeamName;//分组名称
public:
	AddTeamRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};


/* 删除好友分组请求*/
class RemoveTeamReqMsg :public BaseMsg {
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户名
	std::string m_strTeamId;//分组编号
public:
	RemoveTeamReqMsg();
	
	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};

/* 删除好友分组回复*/
class RemoveTeamRspMsg :public BaseMsg {
public:
	ERROR_CODE_TYPE m_eErrorCode;//错误码
	std::string m_strErrMsg;//错误信息
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户名
	std::string m_strTeamId;//分组编号
	std::string m_strTeamName;//分组名
public:
	RemoveTeamRspMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

};



/* 移动好友分组请求*/
class MoveFriendToTeamReqMsg :public BaseMsg {
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户名
	std::string m_strFriendId;//好友用户名
	std::string m_strSrcTeamId;//现在的分组ID
	std::string m_strDstTeamId;//新的分组ID
public:
	MoveFriendToTeamReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

};


/* 移动好友分组回复*/
class MoveFriendToTeamRspMsg :public BaseMsg {
public:
	ERROR_CODE_TYPE m_eErrCode;//错误码
	std::string m_strErrMsg;//错误信息
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户名
	std::string m_strFriendId;//好友用户名
	std::string m_strSrcTeamId;//原来的分组ID
	std::string m_strDstTeamId;//目标的分组ID
public:
	MoveFriendToTeamRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};




/* 创建群聊请求*/
class CreateGroupReqMsg :public BaseMsg {
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strGroupName;//群组名称
public:
	CreateGroupReqMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;

};




/* 创建群聊回复*/
class CreateGroupRspMsg :public BaseMsg {
public:
	ERROR_CODE_TYPE m_eErrCode;//错误码
	std::string m_strErrMsg;//错误信息
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strGroupId;//群组ID
	std::string m_strGroupName;//群组名称
public:
	CreateGroupRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;

};


/* 解散群聊请求*/
class DestroyGroupReqMsg :public BaseMsg {
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strGroupId;//群组ID
	std::string m_strGroupName;//群组名称
public:
	DestroyGroupReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
};

/* 删除群聊回复*/
class DestroyGroupRspMsg :public BaseMsg {
public:
	ERROR_CODE_TYPE m_eErrorCode;//错误代码
	std::string m_strErrMsg;//错误信息
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strGroupId;//群组ID
	std::string m_strGroupName;//群组名称
public:
	DestroyGroupRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
};


/**
 * @brief 查找群组请求消息
 * 
 */
class FindGroupReqMsg :public BaseMsg 
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户名
	std::string m_strGroupName;//被查找的用户名
public:
	FindGroupReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
};


/**
 * @brief 查找群组回复消息
 * 
 */
class FindGroupRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_eErrCode;//错误码
	std::string m_errMsg;//错误信息
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strGroupId;//群组ID
	std::string m_strGroupName;//群组名称
	std::string m_strGroupOwner;//群组创建者
public:
	FindGroupRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
};


/**
 * @brief 添加用户到群组的请求消息
 * 
 */
class AddToGroupReqMsg :public BaseMsg 
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//请求者的用户名
	std::string m_strGroupId; //要加入的群组的ID
public:
	AddToGroupReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;

};


/**
 * @brief 添加用户到群组的回复消息
 * 
 */
class AddToGroupRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_eErrCode;//错误码
	std::string m_errMsg;//错误信息
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strGroupId;//群组ID

public:
	AddToGroupRspMsg();
	
	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
	
};


/**
 * @brief 管理员或者群主收到的添加申请
 * 
 */
class AddToGroupRecvReqMsg :public BaseMsg {
public:
	std::string m_strMsgId;//消息ID
	std::string m_strAdminId;//群组的管理者
	std::string m_strUserId;//请求者的用户名
	std::string m_strGroupId; //要加入的群组的ID
public:
	AddToGroupRecvReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;

};


/**
 * @brief 管理员或者群主是否通过申请
 * 
 */
class AddToGroupRecvRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_eErrCode;//错误码
	std::string m_errMsg;//错误信息
	E_FRIEND_OPTION m_eOption;//管理员的选择
	std::string m_strAdminId;//管理员ID
	std::string m_strUserId;//用户ID
	std::string m_strGroupId;//群组ID
public:
	AddToGroupRecvRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
	
};


/**
 * @brief 添加到群组的申请结果通知消息
 * 
 */
class AddToGroupNotifyReqMsg :public BaseMsg {
public:
	std::string m_strMsgId;//消息ID
	std::string m_strAdminId;//群组的管理者
	std::string m_strUserId;//请求者的用户名
	std::string m_strGroupId; //要加入的群组的ID
	E_FRIEND_OPTION m_eOption;//是否同意
public:
	AddToGroupNotifyReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;

};


/**
 * @brief 添加到群组的申请消息回复
 * 
 */
class AddToGroupNotifyRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_eErrCode;//错误码
	std::string m_errMsg;//错误信息
	E_FRIEND_OPTION m_eOption;//用户选择
	std::string m_strMsgId;//消息ID
	std::string m_strAdminId;//群组管理员ID
	std::string m_strUserId;//用户ID
	std::string m_strGroupId;//群组ID
public:
	AddToGroupNotifyRspMsg();
	
	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

	virtual bool Valid() const override;
};


/**
 * @brief 邀请好友到群组的申请
 * 
 */
class InviteFriendToGroupReqMsg :public BaseMsg {
public:
	std::string m_strSenderId;//请求的发送者
	std::string m_strReciverId;//被请求的用户名
	std::string m_strGroupId; //要加入的群组的ID
public:
	InviteFriendToGroupReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

};


/**
 * @brief 邀请好友到群组的回复
 * 
 */
class InviteFriendToGroupRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_eErrCode;//错误代码
	std::string m_errMsg;//错误信息
	std::string m_strMsgId;//消息ID
	std::string m_strSenderId;//发送者ID
	std::string m_strReciverId;//接收者ID
	std::string m_strGroupId;//群组ID
public:
	InviteFriendToGroupRspMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

};


/**
 * @brief 被邀请方收到的邀请请求
 * 
 */
class InviteFriendToGroupRecvReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strSenderId;//发送者ID
	std::string m_strReciverId;//接收者ID
	std::string m_strGroupId;//群组ID
public:
	InviteFriendToGroupRecvReqMsg();
	
	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

};


/**
 * @brief 被邀请方的回复消息
 * 
 */
class InviteFriendToGroupRecvRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_errCode;//错误码
	std::string m_errMsg;//错误信息
	std::string m_strMsgId;//消息ID
	std::string m_strSenderId;//发送者ID
	std::string m_strReciverId;//接收者ID
	std::string m_strGroupId;//群组ID
	E_FRIEND_OPTION m_eOption;//用户选择
public:
	InviteFriendToGroupRecvRspMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

};


/**
 * @brief 邀请方收到被邀请的方加入群组的通知请求
 * 
 */
class InviteFriendToGroupNotifyReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strSenderId;//发送者ID
	std::string m_strReciverId;//接收者ID
	std::string m_strGroupId;//群组ID
	E_FRIEND_OPTION m_eOption;//用户的选择
public:
	InviteFriendToGroupNotifyReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

};

/**
 * @brief 邀请方收被邀请方加入群组的通知回复
 * 
 */
class InviteFriendToGroupNotifyRspMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strSenderId;//发送者ID
	std::string m_strReciverId;//接收者ID
	std::string m_strGroupId;//群组ID
public:
	InviteFriendToGroupNotifyRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

};


/* 退出群组请求 */
class QuitFromGroupReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strGroupId;//群组ID
public:
	QuitFromGroupReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};

/* 退出群组回复 */
class QuitFromGroupRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_errCode; //错误代码
	std::string m_errMsg;      //错误回复
	std::string m_strMsgId;    //消息ID
	std::string m_strUserId;   //用户ID
	std::string m_strGroupId;  //群组ID
public:
	QuitFromGroupRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};
/**
 * @brief 发送群组文本消息请求[发送方--->服务器]
 * 
 */
class SendGroupTextMsgReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	GroupChatMsg_s m_chatMsg;
public:
	SendGroupTextMsgReqMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

};


/**
 * @brief 发送群组文本消息回复[服务器--->发送方]
 * 
 */
class SendGroupTextMsgRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_eErrCode;//错误码
	std::string m_errMsg;//错误信息
	std::string m_strMsgId;//消息ID
	GroupChatMsg_s m_chatMsg;
public:
	SendGroupTextMsgRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

};


/**
 * @brief 接收群组的文本聊天消息请求[服务器--->接收方]
 * 
 */
class RecvGroupTextMsgReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	GroupChatMsg_s m_chatMsg;
public:
	RecvGroupTextMsgReqMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

};

/**
 * @brief 接收群组的文本聊天消息的回复[接收方--->服务器]
 * 
 */
class RecvGroupTextMsgRspMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strGroupId;//群组ID
	std::string m_strChatMsgId;
public:
	RecvGroupTextMsgRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson);

};

/**
 * @brief 好友发送文件请求[发送方--->服务器]
 * 
 */
class FriendSendFileMsgReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//发送方ID
	std::string m_strFriendId;//接收方ID
	std::string m_strFileName;//文件名
	FILE_TRANS_TYPE m_transMode;
public:
	FriendSendFileMsgReqMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

};


/**
 * @brief 好友发送文件回复[服务器--->发送方]
 * 
 */
class FriendSendFileMsgRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_eErrCode;//错误码
	std::string m_errMsg;//错误信息
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//发送者ID
	std::string m_strFriendId;//接受者ID
	std::string m_strFileName;//文件名称
	FILE_TRANS_TYPE m_transMode;
public:
	FriendSendFileMsgRspMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

};

/**
 * @brief 好友发送文件接收请求[服务器---->接收方]
 * 
 */
class FriendRecvFileMsgReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//发送方ID
	std::string m_strFriendId;//接收方Id
	std::string m_strFileName;//文件名称
	FILE_TRANS_TYPE m_transMode;
public:
	FriendRecvFileMsgReqMsg();
	
	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;
};

/**
 * @brief 好友发送文件的接收回复[接收方--->服务器]
 * 
 */
class FriendRecvFileMsgRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_eErrCode;//错误代码
	std::string m_errMsg;//错误信息
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//文件发送者ID
	std::string m_strFriendId;//文件接收者ID
	std::string m_strFileName;//文件名
	int m_nFileId;
	E_FRIEND_OPTION m_eOption;//用户选择
	FILE_TRANS_TYPE m_transMode;
public:
	FriendRecvFileMsgRspMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;
};


/**
 * @brief 好友接受文件通知请求[服务器---->发送方]
 * 
 */
class FriendNotifyFileMsgReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId;
	std::string m_strUserId;
	std::string m_strFriendId;
	std::string m_strFileName;
	int m_nFileId;
	E_FRIEND_OPTION m_eOption;
	FILE_TRANS_TYPE m_transMode;
public:
	FriendNotifyFileMsgReqMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;
};


/**
 * @brief 好友接受文件通知回复[发送方---->服务器]
 * 
 */
class FriendNotifyFileMsgRspMsg :public BaseMsg
{
public:
	std::string m_strMsgId;
public:
	FriendNotifyFileMsgRspMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

};


/**
 * @brief 获取群组列表请求[发送方--->服务器]
 * 
 */
class GetGroupListReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息Id
	std::string m_strUserId;//用户ID
public:
	GetGroupListReqMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

};


/**
 * @brief 获取群组列表回复[服务器--->发送方]
 * 
 */
class GetGroupListRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_errCode;//错误代码
	std::string m_errMsg;//错误消息
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::vector<GroupInfo> m_GroupList;//群组列表
public:
	GetGroupListRspMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;

};


/**
 * @brief 文件数据发送请求[发送方--->服务器]
 * 
 */
class FileDataSendReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//发送者ID
	std::string m_strFriendId;//接受者ID
	int		    m_nFileId;//文件ID
	int			m_nDataTotalCount;//文件总数据包数
	int			m_nDataIndex;//文件数据包索引
	int			m_nDataLength;//文件数据长度
	char		m_szData[1024];//实际数据
public:
	FileDataSendReqMsg();

	virtual std::string ToString() const override;


	virtual bool FromString(const std::string& strJson) override;

};

/**
 * @brief 文件数据发送回复[服务器---->发送方]
 * 
 */
class FileDataSendRspMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//发送者ID
	std::string m_strFriendId;//接受者ID
	int		    m_nFileId;//文件ID
	int			m_nDataTotalCount;//文件总数据包数
	int			m_nDataIndex;//文件数据包索引
public:
	FileDataSendRspMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;
};


/**
 * @brief 文件数据接收请求[服务器---->接收方]
 * 
 */
class FileDataRecvReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//发送者ID
	std::string m_strFriendId;//接受者ID
	int		    m_nFileId;//文件ID
	int			m_nDataTotalCount;//文件总数据包数
	int			m_nDataIndex;//文件数据包索引
	int			m_nDataLength;//文件数据长度
	char		m_szData[1024];//实际数据
public:
	FileDataRecvReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};


/**
 * @brief 文件数据接收回复[接收方---->服务器]
 * 
 */
class FileDataRecvRspMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//发送者ID
	std::string m_strFriendId;//接受者ID
	int		    m_nFileId;//文件ID
	int			m_nDataTotalCount;//文件总数据包数
	int			m_nDataIndex;//文件数据包索引
public:
	FileDataRecvRspMsg();

	virtual std::string ToString() const override;
	
	virtual bool FromString(const std::string& strJson) override;
};



/**
 * @brief 文件校验请求消息[文件发送方]---->[服务器]---->[文件接收方]
 * 
 */
class FileVerifyReqMsg:public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//发送者ID
	std::string m_strFriendId;//接受者ID
	std::string m_strFileName;//文件名称
	int		    m_nFileId;//文件ID
	int			m_nFileSize;//文件大小
	std::string m_strFileHash;//文件的Hash值
public:
	FileVerifyReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;

};

/**
 * @brief 文件校验回复消息[文件接收方]--->[服务器]--->[文件发送方]
 * 
 */
class FileVerifyRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_eErrCode;//错误码
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//发送者ID
	std::string m_strFriendId;//接受者ID
	std::string m_strFileName;//文件名称
	int		    m_nFileId;//文件ID
	std::string m_strFileHash;//文件的Hash值
public:
	FileVerifyRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};

/* 用户被踢出的请求消息 */
class UserKickOffReqMsg : public BaseMsg
{
public:
	std::string m_strMsgId;			//消息ID
	std::string m_strUserId;		//用户ID
	std::string m_strNewLoginIp;    //新登录的IP地址D
public:
	UserKickOffReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};


/* 用户被踢出的消息的回复 */
class UserKickOffRspMsg :public BaseMsg
{
public:
	std::string m_strMsgId;			//消息ID
	std::string m_strUserId;		//用户ID
public:
	UserKickOffRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};


/**
 * @brief 用户有未读消息的通知 [服务器]--->[接收方]
 * 
 */
class FriendUnReadNotifyReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId;			//消息ID
	std::string m_strUserId;		//用户ID
public:
	FriendUnReadNotifyReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};

/**
 * @brief 用户有未读消息的通知回复 [接收方]--->[服务器]
 * 
 */
class FriendUnReadNotifyRspMsg :public BaseMsg
{
public:
	std::string m_strMsgId; //消息ID
	std::string m_strUserId;//用户ID
	FriendUnReadNotifyRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};

/*
更新好友列表通知请求 [服务器]----->[客户端]
*/
class UpdateFriendListNotifyReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId; //消息ID
	std::string m_strUserId;//用户ID
	UpdateFriendListNotifyReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};

/*
更新好友列表通知的回复
*/
class UpdateFriendListNotifyRspMsg :public BaseMsg
{
public:
	std::string m_strMsgId; //消息ID
	std::string m_strUserId;//用户ID
	UpdateFriendListNotifyRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};


/*
更新群组列表通知请求 [服务器]----->[客户端]
*/
class UpdateGroupListNotifyReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId; //消息ID
	std::string m_strUserId;//用户ID
	UpdateGroupListNotifyReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};

/*
更新群组列表通知回复
*/
class UpdateGroupListNotifyRspMsg :public BaseMsg
{
public:
	std::string m_strMsgId; //消息ID
	std::string m_strUserId;//用户ID
	UpdateGroupListNotifyRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};

/*
好友的UDP的地址查询请求
*/
class QueryUserUdpAddrReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId; //消息ID
	std::string m_strUserId;//用户ID
	std::string m_strUdpUserId;//收到的UDP用户的ID
	QueryUserUdpAddrReqMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};

/*
好友的UDP地址查询回复
*/
class QueryUserUdpAddrRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_errCode;//错误代码
	std::string m_strMsgId; //消息ID
	std::string m_strUserId;//用户ID
	std::string m_strUdpUserId;//好友ID
	IpPortCfg	m_udpEndPt;//好友UDP地址
	QueryUserUdpAddrRspMsg();

	virtual std::string ToString() const override;

	virtual bool FromString(const std::string& strJson) override;
};

/**
 * @brief 用作HTTP请求的超时回复
 * 
 */
class NormalRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_errCode;
	std::string m_errMsg;
public:
	NormalRspMsg(const ERROR_CODE_TYPE type);
	NormalRspMsg();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};



/**
 * @brief 获取好友的聊天消息记录的请求消息
 * 
 */
class GetFriendChatHistoryReq :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strFriendId;//好友ID
	std::string m_strChatMsgId;//聊天消息的最后一条ID
	HISTORY_DIRECTION m_eDirection;
public:
	GetFriendChatHistoryReq();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};

/**
 * @brief 获取好友聊天记录的回复消息
 * 
 */
class GetFriendChatHistoryRsp :public BaseMsg {
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strFriendId;//好友ID
	std::string m_strChatMsgId;//聊天消息的最后一条ID
	HISTORY_DIRECTION m_eDirection;
	std::vector<FriendChatMsg_s> m_msgHistory;
public:
	GetFriendChatHistoryRsp();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};


/**
 * @brief 获取群组聊天记录的请求
 * 
 */
class GetGroupChatHistoryReq :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strGroupId;//好友ID
	std::string m_strChatMsgId;//聊天消息的最后一条ID
	HISTORY_DIRECTION m_eDirection;
public:
	GetGroupChatHistoryReq();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};

/**
 * @brief 获取群组聊天记录的回复
 * 
 */
class GetGroupChatHistoryRsp :public BaseMsg {
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strGroupId;//好友ID
	std::string m_strChatMsgId;//聊天消息的最后一条ID
	HISTORY_DIRECTION m_eDirection;
	std::vector<SendGroupTextMsgRspMsg> m_msgHistory;
public:
	GetGroupChatHistoryRsp();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};

/**
 * @brief 查找聊天记录的请求
 * 
 */
class SearchChatHistoryReq :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strSearchWord;//查找的内容
public:
	SearchChatHistoryReq();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};

/**
 * @brief 查找聊天记录回复
 * 
 */
class SearchChatHistoryRsp :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::vector<FriendChatMsg_s> m_friendChatMsgVec;
	std::vector<SendGroupTextMsgRspMsg> m_groupChatMsgVec;
public:
	SearchChatHistoryRsp();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};


/**
 * @brief 同步好友聊天记录的消息请求
 * TODO: 需要增加好友的ID?
 */
class AsyncFriendChatMsgReq :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strChatMsgId;//最后一条聊天记录的ID
public:
	AsyncFriendChatMsgReq();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};

/**
 * @brief 同步好友聊天消息记录的回复
 * 
 */
class AsyncFriendChatMsgRsp :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_eCode;//错误代码
	std::string m_errMsg;//错误消息
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::vector<FriendChatMsg_s> m_chatMsgVec;
public:
	AsyncFriendChatMsgRsp();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};


/**
 * @brief 同步群聊消息记录请求
 * TODO:是否需要分不同的群组同步
 */
class AsyncGroupChatMsgReq :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strChatMsgId;//最后一条消息记录的ID
public:
	AsyncGroupChatMsgReq();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};

/**
 * @brief 同步群聊消息回复
 * 
 */
class AsyncGroupChatMsgRsp :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_eCode;//错误代码
	std::string m_errMsg;//错误消息
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::vector<GroupChatMsg_s> m_chatMsgVec;//群组聊天消息数组
public:
	AsyncGroupChatMsgRsp();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};

/**
 * @brief 开始发送文件数据请求消息[文件发送方---->文件接收方]
 * 为发送图片而设置
 */
class FileSendDataBeginReq :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strFriendId;//好友ID
	std::string m_strFileName;//文件名
	int m_nFileId;//文件Id
	std::string m_strFileHash;//文件的Hash值
	FILE_TYPE   m_eFileType;
public:
	FileSendDataBeginReq();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};

/**
 * @brief 发送文件数据开始回复消息
 * 
 * 主要是为了发送图片做准备
 */
class FileSendDataBeginRsp :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_errCode;//错误码
	std::string m_errMsg;//错误消息
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strFriendId;//好友ID
	std::string m_strFileName;//文件名
	int m_nFileId;//文件ID
	FILE_TYPE   m_eFileType;
public:
	FileSendDataBeginRsp();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};

/**
 * @brief 文件传输进度通知消息
 * 
 */
class FileTransProgressNotifyReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strOtherId;//好友ID或者群组ID
	std::string m_strFileName;//文件名
	int    m_nTransPercent;//传输百分比
	int	   m_nTransSpeed;//传输速度
	FILE_TRANS_DIRECTION m_eDirection;
public:
	FileTransProgressNotifyReqMsg();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};

class FileDownLoadReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strFriendId;//好友ID
	std::string m_strFileName;//文件名
	std::string m_strRelateMsgId;//相关的MsgId(由哪个MsgId发起的)
	FILE_TYPE m_eFileType;
public:
	FileDownLoadReqMsg();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};

class FileDownLoadRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_errCode;//错误码
	std::string m_errMsg;//错误消息
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strFriendId;//好友ID
	std::string m_strFileName;//文件名
	std::string m_strFileHash;
	std::string m_strRelateMsgId;
	FILE_TYPE m_eFileType;
public:
	FileDownLoadRspMsg();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};


class GetRandomUserReqMsg :public BaseMsg 
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
public:
	GetRandomUserReqMsg();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};

class GetRandomUserRspMsg :public BaseMsg
{
public:
	ERROR_CODE_TYPE m_errCode;//错误码
	std::string m_errMsg;//错误消息
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strFriendName;
public:
	GetRandomUserRspMsg();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};


class UdpP2pStartReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strFriendId;//好友ID
public:
	UdpP2pStartReqMsg();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};

class UdpP2pStartRspMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strFriendId;//好友ID
public:
	UdpP2pStartRspMsg();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};
class UdpMultiCastReqMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
public:
	UdpMultiCastReqMsg();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};

class UdpMultiCastRspMsg :public BaseMsg
{
public:
	std::string m_strMsgId;//消息ID
	std::string m_strUserId;//用户ID
	std::string m_strFriendId;//好友ID
public:
	UdpMultiCastRspMsg();
	virtual std::string ToString() const override;
	virtual bool FromString(const std::string& strJson) override;
};
/**
 * @brief 服务器配置
 * 
 */
class CServerConfig
{
public:
	std::vector<IpPortCfg> m_clientCfgVec;
	IpPortCfg              m_serverCfg;
	explicit CServerConfig()
	{
		m_serverCfg.m_strServerIp = "127.0.0.1";
		m_serverCfg.m_nPort = 9000;

		m_clientCfgVec.push_back({"127.0.0.1",8000});
	}
};

#endif
