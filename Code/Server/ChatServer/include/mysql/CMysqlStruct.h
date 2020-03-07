/**
 * @file CMysqlStruct.h
 * @author DennisMi (https://www.dennisthink.com/)
 * @brief 数据库表格对应的结构体
 * @version 0.1
 * @date 2019-11-26
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#ifndef _DENNIS_C_MYSQL_STRUCT_H_
#define _DENNIS_C_MYSQL_STRUCT_H_
#include <string>
#include "CommonDef.h"
const int MAX_USER_ID_LEN = 8;
//用户的最基本的信息，只保存用户名和密码
struct T_USER_BEAN
{
	std::string m_strF_USER_ID;//用户ID，8个字节，由系统分配
    std::string m_strF_USER_NAME;//用户名称
    std::string m_strF_PASS_WORD;//用户密码
	std::string m_strF_NICK_NAME;//用户昵称

	bool IsValid()  const {
		if (m_strF_USER_ID.length() > MAX_USER_ID_LEN ||
			m_strF_USER_ID.empty())
		{
			return false;
		}

		if (m_strF_USER_NAME.empty())
		{
			return false;
		}

		if (m_strF_PASS_WORD.empty())
		{
			return false;
		}

		if (m_strF_NICK_NAME.empty())
		{
			return false;
		}
		return true;
	}
};

//用户的其他信息，不保存密码
struct T_USER_INFO_BEAN
{
	std::string m_strF_USER_ID;//用户ID，8个字节，由系统分配
	std::string m_strF_USER_NAME;//用户名
	std::string m_strF_ADDRESS;//地址
	std::string m_strF_BIRTH_DATE;//生日
	std::string m_strF_EMAIL_ADDR;//邮箱
	std::string m_strF_NICK_NAME;//昵称
	std::string m_strF_SIGNATURE;//个性签名
	std::string m_strF_FACE_ID;//头像编号
	CLIENT_STATE m_eOnlineState;
};

//好友聊天消息
struct T_USER_CHAT_MSG 
{
	std::string m_strF_MSG_ID;//消息ID
	CHAT_MSG_TYPE m_eChatMsgType;
	std::string m_strF_FROM_ID;//发送方ID
	std::string m_strF_TO_ID;//接收方ID
	std::string m_strF_MSG_CONTEXT;//消息内容
	std::string m_strF_OTHER_INFO;//其他信息例如字体等
	std::string m_strF_READ_FLAG;//已读标识(UNREAD  READ)
	std::string m_strF_CREATE_TIME;//消息创建时间
	T_USER_CHAT_MSG() {
		m_eChatMsgType = CHAT_MSG_TYPE::E_CHAT_UNKNOWN_TYPE;
		m_strF_OTHER_INFO = "";
	}
};

/**
 * @brief 群组聊天消息
 * 
 */
struct T_GROUP_CHAT_MSG
{
	std::string m_strF_MSG_ID;//消息ID
	CHAT_MSG_TYPE m_eChatMsgType;//消息类型
	std::string m_strF_SENDER_ID;//发送者ID
	std::string m_strF_GROUP_ID;//群组ID
	std::string m_strF_MSG_CONTEXT;//消息内容
	std::string m_strF_OTHER_INFO;//其他的信息(字体信息)
	std::string m_strF_CREATE_TIME;//消息创建时间
};

//添加好友消息
struct T_ADD_FRIEND_MSG_BEAN {
	std::string m_strF_MSG_ID;//消息ID
	std::string m_strF_USER_ID;//发起方用户编号
	std::string m_strF_FRIEND_ID;//接受方用户名
	E_ADD_FRIEND_STATUS m_eF_ADD_FRIEND_STATUS;//添加状态
	E_FRIEND_OPTION m_eF_FRIEND_OPTION;//添加选择
};

/**
 * @brief 好友分组元数据
 * 
 */
struct T_USER_TEAM_BEAN {
	std::string m_strF_TEAM_ID;//分组ID
	std::string m_strF_USER_ID;//好友ID
	std::string m_strF_TEAM_NAME;//分组名称
};


/**
 * @brief 群组元数据
 * 
 */
struct T_GROUP_BEAN {
	std::string m_strF_GROUP_ID;//群组ID
	std::string m_strF_GROUP_NAME;//群组名称
};

/**
 * @brief 群成员元数据
 * 
 */
struct T_GROUP_RELATION_BEAN {
	std::string m_strF_GROUP_ID;//群组ID
	std::string m_strF_USER_ID;//用户ID
	std::string m_strF_LAST_READ_MSG_ID;//最后一条阅读消息的ID
	E_GROUP_MEMBER_ROLE m_eRole;//群成员角色
};

/**
 * @brief 好友关系元数据
 * 
 */
struct T_FRIEND_RELATION_BEAN {
	std::string m_strF_USER_ID;//用户ID
	std::string m_strF_TEAM_ID;//分组ID
	std::string m_strF_FRIEND_ID;//好友ID
	E_FRIEND_RELATION m_eF_STATUS;//好友关系
};

/**
 * @brief 文件Hash元数据
 * 
 */
struct T_FILE_HASH_BEAN
{
	std::string m_strF_USER_ID;//用户ID
	std::string m_strF_FILE_NAME;//文件名
	std::string m_strF_FILE_HASH;//文件HASH
};
#endif